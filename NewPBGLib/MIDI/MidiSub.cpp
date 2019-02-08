/*
 *   MidiSub.cpp   : MIDI 管理・サブ
 *
 */

#include "MidiSub.h"
#include "PbgMidi.h"
#include "MidiNotify.h"
#include "PbgError.h"



namespace Pbg {



// 解析部 //
IVOID PbgMidi::MidiParse(MidiTrack *pTrack, MidiPort *pPort, int BaseVolume)
{
	int			i, j, count;
	BYTE		st1, st2, code;
	BYTE		data[1024];		// SysEx 送出用にちょっと大きく...
	MIDIHDR		mh;
	HMIDIOUT	hmidi;

	hmidi = pPort[pTrack->m_TargetPort].m_hMidi;

	st1 = *(pTrack->m_pCurrentPos);
	if(st1 < 0x80){
		st1 = pTrack->m_Status;
	}
	else{
		pTrack->m_pCurrentPos++;
	}

	st2 = st1 & 0xf0;

	switch(st2){
	case 0xf0:		// SysEx or 制御用データ
		if(0xf0 == st1){
			count = GetWaitCount(&(pTrack->m_pCurrentPos));
			mh.lpData          = (char *)data;
			mh.dwFlags         = 0;
			mh.dwOffset        = 0;
			mh.dwBufferLength  = count + 1;
			mh.dwBytesRecorded = count + 1;
			data[0]            = 0xf0;

			for(i=1; i<=count; i++){
				data[i] = *(pTrack->m_pCurrentPos ++);
			}

			midiOutPrepareHeader(hmidi, &mh, sizeof(MIDIHDR));
			midiOutLongMsg(hmidi, &mh, sizeof(MIDIHDR));
			midiOutUnprepareHeader(hmidi, &mh, sizeof(MIDIHDR));
		}
		else{
			code  = *(pTrack->m_pCurrentPos ++);
			count = GetWaitCount(&(pTrack->m_pCurrentPos));
			switch(code){
			case 0x2f:	// トラック終了
				pTrack->m_bFinished = TRUE;
			return;

			case 0x51:	// テンポ
				m_PlayCount2 += (m_PlayCount1 * m_TimeBase * 1000) / m_Tempo;
				m_PlayCount1  = 0;
				m_Tempo       = 0;
				for(i=0; i<count; i++){
					m_Tempo += (m_Tempo << 8) + (*(pTrack->m_pCurrentPos ++));
				}
			break;

			case 0x21:	// ポート変更
				pTrack->m_TargetPort = *(pTrack->m_pCurrentPos);

			default:
				pTrack->m_pCurrentPos += count;
			}
		}
	break;

	case 0xb0:		// コントロールチェンジ
		switch(*(pTrack->m_pCurrentPos)){
		case 0x07:	// ボリューム
			data[1] = *(pTrack->m_pCurrentPos ++);

			i = *(pTrack->m_pCurrentPos ++);
			pPort[pTrack->m_TargetPort].m_Volume[st1&0x0f] = i;

			data[2] = (i * BaseVolume) >> 16;
		break;

/*				case(0x07):	// ボリューム
					Mid_VolumeTable[st1&0x0f] = *(track->work+1);
				break;

				case(0x0a):	// パンポット
					Mid_PanpodTable[st1&0x0f] = *(track->work+1);
				break;

				case(0x0b):	// エクスプレッション
					Mid_ExpressionTable[st1&0x0f] = *(track->work+1);
				break;
*/
		case 0x0a:	// パン
			data[1] = *(pTrack->m_pCurrentPos ++);
			data[2] = *(pTrack->m_pCurrentPos ++);

			pPort[pTrack->m_TargetPort].m_PanPod[st1&0x0f] = data[2];
		break;

		case 0x0b:	// エクスプレッション
			data[1] = *(pTrack->m_pCurrentPos ++);
			data[2] = *(pTrack->m_pCurrentPos ++);
			pPort[pTrack->m_TargetPort].m_Expression[st1&0x0f] = data[2];
		break;

		default:
			data[1] = *(pTrack->m_pCurrentPos ++);
			data[2] = *(pTrack->m_pCurrentPos ++);
		break;
		}

		data[0] = st1;
		midiOutShortMsg(hmidi, *((DWORD *)data));
	break;

/*
		case(0x80):					// ノートオフ
			Mid_NoteTable[st1&0x0f][*(track->work)]  = *(track->work+1) = 0;

		case(0x90):case(0xa0):		// ３バイト：発音 or 変更
			if(Mid_PlayTable[st1&0x0f][*(track->work)] < *(track->work+1)){
				Mid_PlayTable[st1&0x0f][*(track->work)]  = *(track->work+1);
				Mid_PlayTable2[st1&0x0f][*(track->work)] = *(track->work+1);
			}
			//Mid_PlayTable[st1&0x0f][*(track->work)]  += *(track->work+1);
			//Mid_PlayTable2[st1&0x0f][*(track->work)] += *(track->work+1);
			Mid_NoteTable[st1&0x0f][*(track->work)]  = *(track->work+1);
			if(Mid_NoteTable[st1&0x0f][*(track->work)])
				Mid_NoteWTable[st1&0x0f][*(track->work)] = 5;
*/
	case 0x80:	// ノートオフ
		i = *(pTrack->m_pCurrentPos);
		//pPort[pTrack->m_TargetPort].m_NoteOn[st1&0x0f][i] = 0;
		pPort[pTrack->m_TargetPort].m_NoteOffMask[st1&0x0f][i] = 0;

		data[0] = st1;
		data[1] = *(pTrack->m_pCurrentPos ++);
		data[2] = *(pTrack->m_pCurrentPos ++);

		midiOutShortMsg(hmidi, *((DWORD *)data));
	break;

	case 0x90:	// ノートオン
	case 0xa0:	// 変更
		i = *(pTrack->m_pCurrentPos);
		j = *(pTrack->m_pCurrentPos + 1);
		if(j){
			pPort[pTrack->m_TargetPort].m_NoteOn[st1&0x0f][i] = j;
			pPort[pTrack->m_TargetPort].m_NoteOffMask[st1&0x0f][i] = 0xff;
		}
		else{
			pPort[pTrack->m_TargetPort].m_NoteOffMask[st1&0x0f][i] = 0;
		}

	case 0xe0:	// ３バイト系命令
		data[0] = st1;
		data[1] = *(pTrack->m_pCurrentPos ++);
		data[2] = *(pTrack->m_pCurrentPos ++);

		midiOutShortMsg(hmidi, *((DWORD *)data));
	break;

	case 0xc0:	// ２バイト系命令
	case 0xd0:
		data[0] = st1;
		data[1] = *(pTrack->m_pCurrentPos ++);

		midiOutShortMsg(hmidi, *((DWORD *)data));
	break;
	}

	pTrack->m_Status     = st1;
	pTrack->m_WaitCount += GetWaitCount(&(pTrack->m_pCurrentPos));
}


// タイマー呼び出しのコールバック関数 //
void CALLBACK PbgMidi::
	MidiCallbackFn(UINT uID, UINT uMsg, DWORD dwUser, DWORD, DWORD)
{
	PbgMidi		*pThis = (PbgMidi *)dwUser;

	int				i, msg, volume, nTrack;
	int				BaseVolume;
	BYTE			*pTable;
	BYTE			Ch;
	BOOL			bFlag;
	MidiTrack		*pTrack;
	DWORDLONG		Current, Delay, pc1, pc2;
	HMIDIOUT		hMidi[MIDI_NUMPORT];

	// クリティカルセクション突入 //
	pThis->m_Critical.EnterCS();

	bFlag   = FALSE;						// 再生フラグ
	nTrack  = pThis->m_NumTracks;			// トラック数
	pc1     = pThis->m_PlayCount1;			// 再生カウント１
	pc2     = pThis->m_PlayCount2;			// 再生カウント２
	Delay   = pThis->m_Timer.GetInterval();	// この関数の呼び出し間隔

	// 現在の経過時間 //
	Current = pc2 + (pc1 * pThis->m_TimeBase * 1000) / pThis->m_Tempo;

	// 出力ポートを配列に格納 //
	for(i=0; i<MIDI_NUMPORT; i++){
		hMidi[i] = pThis->m_Port[i].m_hMidi;
	}

	// 基本となるボリュームを求める //
	BaseVolume = (pThis->m_ExtraVolume+1) * (pThis->m_MasterVolume+1);

	// 各トラックごとの処理 //
	pTrack = pThis->m_aTrack;	// まず、先頭を指す
	for(i=0; i<nTrack; i++, pTrack++){
		if(FALSE == pTrack->m_bFinished){
			bFlag = TRUE;

			while((FALSE == pTrack->m_bFinished) && (pTrack->m_WaitCount <= Current)){
				// 再生ルーチンへと //
				pThis->MidiParse(pTrack, pThis->m_Port, BaseVolume);
			}
		}
	}

	// テンポの補正機能を追加 2001/06/06 //
	pThis->m_PlayTime   += Delay;
	pThis->m_PlayCount1 += ((Delay * pThis->m_ExtraTempo)>>7);
	//Mid_Data.playcount1+=((Mid_Time.delay*Mid_MulTempo)>>7);

	// フェードイン＆アウト //
//	if(MIDIST_FADE == pThis->m_PlayState){
	if(pThis->m_bIsFadeOut){
		pThis->m_FadeCount += (int)Delay;

		// ボリューム変更の時！ //
		if(pThis->m_FadeCount >= pThis->m_FadeWait){
			pThis->m_FadeCount = 0;

			Ch = pThis->m_FadeOutCh;

			if(Ch >= 16){
				pThis->m_FadeOutCh = Ch = 0;

				volume =  pThis->m_FadeTarget - pThis->m_ExtraVolume;

				if(     volume > 0) (pThis->m_ExtraVolume) += min(volume, 2);
				else if(volume < 0) (pThis->m_ExtraVolume) += max(volume, -2);

				if(pThis->m_FadeTarget == pThis->m_ExtraVolume){
					pThis->m_bIsFadeOut = FALSE;
					//pThis->m_PlayState = MIDIST_PLAY;

					// イベント通知 //
					if((0 == pThis->m_FadeTarget) && pThis->m_pCallBack){
						if(MRET_STOP == pThis->m_pCallBack(MIDIN_FADEZERO, 0)){
							bFlag              = FALSE;
							pThis->m_PlayState = MIDIST_PLAY;
						}
					}
				}
			}
			else{
				pThis->m_FadeOutCh = Ch + 2;
			}


			// 変更が必要 //
			BaseVolume = (pThis->m_ExtraVolume+1) * (pThis->m_MasterVolume+1);

			pTable = pThis->m_Port[0].m_Volume;
			msg = (BaseVolume * pTable[Ch]) >> 16;
			SendShortMsg(hMidi[0], 0xb0+Ch, 0x07, (BYTE)msg);
			msg = (BaseVolume * pTable[Ch+1]) >> 16;
			SendShortMsg(hMidi[0], 0xb0+Ch+1, 0x07, (BYTE)msg);

			// 32Ch - Mode //
			if(hMidi[0] != hMidi[1]){
				pTable = pThis->m_Port[1].m_Volume;
				msg = (BaseVolume * pTable[Ch]) >> 16;
				SendShortMsg(hMidi[1], 0xb0+Ch, 0x07, (BYTE)msg);
				msg = (BaseVolume * pTable[Ch+1]) >> 16;
				SendShortMsg(hMidi[1], 0xb0+Ch+1, 0x07, (BYTE)msg);
			}
		}
	}

	// 全てのトラックが停止している場合 //
	if(FALSE == bFlag){
		switch(pThis->m_PlayState){
			case MIDIST_LOOP:
				pThis->InitializeTrack();
			break;

			case MIDIST_PLAY:
			case MIDIST_STOP:
				pThis->InternalStop();	// 内部停止要求
				pThis->m_Timer.Stop();	// 自分自身を殺す

				// イベント通知 //
				if(pThis->m_pCallBack){
					pThis->m_pCallBack(MIDIN_STOP, 0);
				}
			break;
		}
	}

	// クリティカルセクション脱出 //
	pThis->m_Critical.LeaveCS();
}


void DeleteSpace(char *pData)
{
	int		l;

	// 後ろのほうにある空白を消去する //
	for(l=strlen(pData)-1; l>0; l--){
		if(' ' == pData[l]){
			pData[l] = '\0';
		}
		else return;
	}
}


// タイトル解析 //
FVOID PbgMidi::CheckTitle(BYTE *pData, DWORD dwSize)
{
	DWORD		i;

	for(i=0; i<dwSize; i++, pData++){
		if(0xff == pData[0]){
			if(0x03 == pData[1]){
				memcpy(m_MusicTitle, pData+3, pData[2]);
				m_MusicTitle[pData[2]] = '\0';
				DeleteSpace(m_MusicTitle);
				return;
			}
			else if((0x2f == pData[1]) && (0x00 == pData[2])) break;
		}
	}
}



} // namespace Pbg
