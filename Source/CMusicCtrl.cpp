/*
 *   CMusicCtrl.cpp   : 音楽管理クラス
 *
 */

#include "CMusicCtrl.h"
#include "SECtrl.h"
#include "CCfgIO.h"


#define MCST_STOP	0x00	// 状態：停止
#define MCST_PLAY	0x01	// 状態：再生
#define MCST_FADE	0x02	// 状態：フェード



Pbg::CSound			*g_pSnd   = NULL;



// コンストラクタ //
CMusicCtrl::CMusicCtrl(HWND hWnd, CCfgIO *pCfgIO)
{
	int		NumDevice;

	// 設定ファイルから初期化情報を取得 //
	m_pCfgIO = pCfgIO;
	pCfgIO->GetSndCfg(&m_SndCfg);

//	m_MusicDevice = MDEV_NULL;	// 再生デバイス
	m_State       = MCST_STOP;	// 停止中
	m_bEnableMid  = FALSE;		// デバイスが有効
	m_bEnableWave = FALSE;		// 波デバイスが有効

//	m_MidiDev = 0;		// ＭＩＤＩデバイス
//	m_Volume  = 255;	// ＭＩＤＩボリューム

	m_FileNo = 0;	// オープンしているファイル番号


	if(NULL == g_pSnd){
		g_pSnd = NewEx(Pbg::CSound);
		if(g_pSnd){
			// コレは失敗しても無視すべき //
			if(FALSE == g_pSnd->Initialize(hWnd)){
				PbgError("-> サウンド機能は使用できません");
				//DeleteEx(g_pSnd);
			}
			else{
				InitSE(m_SndCfg.m_SEVolume);

				if(FALSE == g_pSnd->CreateWaveMusicBuffer(&m_pWave)){
					PbgError("-> 曲の再生にＷＡＶＥが使用できません");
					m_pWave = NULL;
				}
				else{
					m_bEnableWave = TRUE;
				}
			}
		}
	}

//	*m_FileName = '\0';


	// ここから、コンフィグの妥当性をチェックする //
	// 参考：データの妥当性をチェックしないメンバ
	//        m_SndCfg.m_MusicVolume
	//        m_SndCfg.m_SEVolume

	// まずはＭＩＤＩ系の妥当性チェック //
	NumDevice = m_Midi.GetNumDevices();
	if(NumDevice > 1){
		// ＭＩＤＩデバイスの数が変化している場合 //
		if(NumDevice != m_SndCfg.m_MaxMidiDevice){
			m_SndCfg.m_MaxMidiDevice     = NumDevice;
			m_SndCfg.m_CurrentMidiDevice = 0;		// まっぱにする
		}
		// ＭＩＤＩデバイスＩＤが不正 //
		else if(m_SndCfg.m_CurrentMidiDevice >= NumDevice){
			m_SndCfg.m_CurrentMidiDevice = 0;		// やっぱりまっぱ
		}

		// デバイスＩＤを初期化 //
		ChangeMidDev(m_SndCfg.m_CurrentMidiDevice);

		// ＭＩＤＩは使用可能である //
		m_bEnableMid = TRUE;
	}
	// ＭＩＤＩによる曲の再生が行えない場合 //
	else{
		m_SndCfg.m_MaxMidiDevice     = 0;
		m_SndCfg.m_CurrentMidiDevice = 0;

		// 使用できないのにＭＩＤＩが選択されている場合 //
		if(MDEV_MIDI == m_SndCfg.m_MusicDevice){
			// このユーザは曲を再生することを望んでいるハズ //
			m_SndCfg.m_MusicDevice = MDEV_WAVE;
		}
	}

	switch(m_SndCfg.m_MusicDevice){
		case MDEV_WAVE:
			// ＷＡＶＥによる再生要求があるが、再生できない場合 //
			if(FALSE == m_bEnableWave){
				m_SndCfg.m_MusicDevice = MDEV_NULL;
			}
		break;

		case MDEV_MIDI:	// すでにチェック済み
		case MDEV_NULL:	// チェックの必要無し
		break;

		default:
			m_SndCfg.m_MusicDevice = MDEV_NULL;
			PbgError("サウンド系の設定ファイルが壊れとるぞぃ");
		break;
	}

	ChangeMusicDev(m_SndCfg.m_MusicDevice);
}


// デストラクタ //
CMusicCtrl::~CMusicCtrl()
{
	// 設定を保存する //
	m_pCfgIO->SetSndCfg(&m_SndCfg);

	DeleteEx(g_pSnd);
}


// 再生する //
FVOID CMusicCtrl::Play(void)
{
	if(NULL == this) return;

	// 再ロードを行う //
	Load(m_FileNo);

	// ボリュームを反映 //
	SetMusicVolume(m_SndCfg.m_MusicVolume);

	switch(m_SndCfg.m_MusicDevice){
	case MDEV_MIDI:
		if(FALSE == m_Midi.Play(TRUE)) return;
	break;

	case MDEV_WAVE:
		if(NULL == m_pWave) return;
		if(FALSE == m_pWave->Play()) return;
	break;

	case MDEV_NULL:
	break;

	default:
	return;
	}

	m_State = MCST_PLAY;
}


// 停止する //
FVOID CMusicCtrl::Stop(void)
{
	if(NULL == this) return;

	switch(m_SndCfg.m_MusicDevice){
	case MDEV_MIDI:
		m_Midi.Stop();
	break;

	case MDEV_WAVE:
		if(NULL == m_pWave) return;
		m_pWave->Stop();
	break;

	case MDEV_NULL:
	break;

	default:
	return;
	}

	m_State = MCST_STOP;
}


// フェードイン・アウト //
FVOID CMusicCtrl::Fade(BYTE Speed, BYTE Volume)
{
	if(NULL == this) return;

	switch(m_SndCfg.m_MusicDevice){
	case MDEV_MIDI:
		m_Midi.Fade(Speed, Volume);
	break;

	case MDEV_WAVE:
	return;

	case MDEV_NULL:
	default:
	return;
	}

	m_State = MCST_FADE;
}


// デバイス(MIDI/WAVE)を変更する //
FBOOL CMusicCtrl::ChangeMusicDev(MUSIC_DEVICE MDevice)
{
	BYTE	State;

	if(NULL == this) return FALSE;

	switch(MDevice){
	case MDEV_MIDI:
	break;

	case MDEV_WAVE:
		if(NULL == m_pWave) return FALSE;
	break;

	case MDEV_NULL:
	break;

	default:
	return FALSE;
	}

	State = m_State;	// 現在の状態を取得した後に
	Stop();				// 停止する

	// デバイスを変更 //
	m_SndCfg.m_MusicDevice = MDevice;

	// 再生中の変更の場合、そのデバイスでの再生を試みる //
	// ロードの処理が必要かも                           //
	if(MCST_PLAY == State){
		Play();
	}

	return TRUE;
}


// 曲のロードを行う(番号) //
FBOOL CMusicCtrl::Load(int FileNo)
{
	char	buf[1024];

	m_FileNo = FileNo;

	if(FileNo >=  0){
		wsprintf(buf, "kog_%02d", m_FileNo);
	}
	else{
		strcpy(buf, "kog_sj");
	}

	return LoadFromID(buf);
}


// ボリュームを変更する //
FVOID CMusicCtrl::SetMusicVolume(BYTE Volume)
{
	m_SndCfg.m_MusicVolume = Volume;

	if(NULL == this) return;

	// デバイスごとの処理を行う //
	switch(m_SndCfg.m_MusicDevice){
	case MDEV_MIDI:
		m_Midi.SetMasterVolume(Volume);
	break;

	case MDEV_WAVE:
		if(m_pWave) m_pWave->SetVolume(Volume);
	break;

	case MDEV_NULL: default:
	return;
	}
}


// ボリュームを取得する //
FBYTE CMusicCtrl::GetMusicVolume(void)
{
	if(NULL == this) return 0;

	return m_SndCfg.m_MusicVolume;
}


// 効果音のボリュームをセット //
FVOID CMusicCtrl::SetSoundVolume(BYTE Volume)
{
	m_SndCfg.m_SEVolume = Volume;
	g_pSnd->SetMasterVolume(Volume);
}


// 効果音のボリュームを取得 //
FBYTE CMusicCtrl::GetSoundVolume(void)
{
	if(NULL == g_pSnd) return 0;

	return m_SndCfg.m_SEVolume;
}


// ロードされている曲名を取得 //
FVOID CMusicCtrl::GetTitle(char *pTitle)
{
	char		*FileID      = "MUSIC/TITLE";
	DWORD		StringLength = 450;

	Pbg::LzDecode	Decode;
	char			*pTemp;
	DWORD			Size, Offset;

	// タイトルを無効化する //
	strcpy(pTitle, "ファイル名が取得できませんでした");

	// ファイルを開いて //
	if(FALSE == Decode.Open("ENEMY.DAT")) return;

	// 曲名のデコード //
	pTemp = (char *)Decode.Decode(FileID);
	if(NULL == pTemp) return;

	// 格納されているファイル名の数を取得 //
	Size = Decode.GetFileSize(FileID) / StringLength;
	if(Size <= m_FileNo) return;

	// 後は、埋め込み＆終了処理 //
	Offset = StringLength * m_FileNo;	// 文字列開始位置を取得
	strcpy(pTitle, pTemp + Offset);		// 名前をコピーしたら
	MemFree(pTemp);						// 解放
}


// 現在選択されているデバイスを取得 //
MUSIC_DEVICE CMusicCtrl::GetMusicDev(void)
{
	if(NULL == this) return MDEV_NULL;

	return m_SndCfg.m_MusicDevice;
}


// ＭＩＤＩ出力ポートを変更 //
FBOOL CMusicCtrl::ChangeMidDev(BYTE DevID)
{
	if(NULL == this) return FALSE;

	if(m_Midi.ChangeDevice(DevID, DevID)){
		m_SndCfg.m_CurrentMidiDevice = DevID;
		return TRUE;
	}

	return FALSE;
}


// ＭＩＤＩデバイス数を取得 //
FBYTE CMusicCtrl::GetNumMidDev(void)
{
	if(NULL == this) return 0;

	return (BYTE)m_Midi.GetNumDevices();
}


// ＭＩＤＩデバイス名を取得 //
FBOOL CMusicCtrl::GetMidDevName(char *pName, BYTE DevID)
{
	if(NULL == this) return FALSE;

	return m_Midi.GetDeviceName(DevID, pName);
}


// 現在のデバイスを取得 //
FBYTE CMusicCtrl::GetCurrentMidDev(void)
{
	if(NULL == this) return 0;

	return m_SndCfg.m_CurrentMidiDevice;
}


// ＭＩＤＩデバイスは有効か //
FBOOL CMusicCtrl::IsEnableMid(void)
{
	return m_bEnableMid;		// みぢデバイスが有効
}


// 現在の波形を取得する //
FVOID CMusicCtrl::GetWaveScope(BYTE Buffer[128+128])
{
	if((MDEV_WAVE == m_SndCfg.m_MusicDevice) && m_pWave){
		m_pWave->GetCurrentScope(Buffer);
	}
	else{
		ZEROMEM(Buffer);
	}
}


// ＭＩＤＩデバイスは有効か //
FBOOL CMusicCtrl::IsEnableWave(void)
{
	return m_bEnableWave;		// 波デバイスが有効
}


// *.pos ファイル解析用構造体 //
typedef struct tagPosData {
	DWORD	Start;
	DWORD	End;
} PosData;


// 曲のロードを行う //
FBOOL CMusicCtrl::LoadFromID(char *pFileID)
{
	char			buf[MAX_PATH];
	Pbg::LzDecode	Decode;
	PosData			*pPosData;
	BOOL			ret;

	if(NULL == this)    return FALSE;
	if(NULL == pFileID) return FALSE;
/*
	// ここで、どの曲をロードしたのかを覚えておく必要がある //
	if(pFileID != pFileID){		// デバイス変更による、再ロード対策
		strcpy(pFileID, pFileID);
	}
*/
	// デバイスごとの処理を行う //
	switch(m_SndCfg.m_MusicDevice){
	case MDEV_MIDI:
		wsprintf(buf, "MUSIC/%s", pFileID);
	return m_Midi.LoadP("MUSIC.DAT", buf);

	case MDEV_WAVE:
		if(NULL == m_pWave){
			PbgError("内部エラー：ＷＡＶＥが使用できないのに選択されている");
			return FALSE;
		}

		wsprintf(buf, "WAVE\\%s.wav", pFileID);
		if(FALSE == m_pWave->Load(buf)) return FALSE;

		// *.pos ファイルをオープンする //
		if(FALSE == Decode.Open("MUSIC.DAT")){
			PbgLog("MUSIC.DAT がオープンできない");
			return FALSE;
		}

		wsprintf(buf, "POS/%s", pFileID);
		pPosData = (PosData *)Decode.Decode(buf);
		if(NULL == pPosData){
			PbgLog("ループ位置定義ファイルが開けない");
			return FALSE;
		}

		//////////////////////////////////
//		wsprintf(buf, "LoopPos : %u - %u", pPosData->Start, pPosData->End);
//		PbgError(buf);
		//////////////////////////////////

		// ループ位置をセットする //
		ret = m_pWave->SetLoopPosition(pPosData->Start, pPosData->End);
		MemFree(pPosData);

		if(FALSE == ret){
			PbgLog("ループ位置が定義できない");
			return FALSE;
		}
	return TRUE;

	case MDEV_NULL: default:
	return TRUE;
	}
}


// エクスプレッションを取得する //
FVOID CMusicCtrl::GetExpression(BYTE *pTable)
{
	m_Midi.GetExpression(pTable);
}


// パンを取得する //
FVOID CMusicCtrl::GetPanPod(BYTE *pTable)
{
	m_Midi.GetPanPod(pTable);
}


// パートのボリュームを取得する //
FVOID CMusicCtrl::GetPartVolume(BYTE *pTable)
{
	m_Midi.GetVolume(pTable);
}


// ノートＯＮ／ＯＦＦの取得 //
FVOID CMusicCtrl::GetNoteOn(int Track, BYTE *pTable)
{
	m_Midi.GetNoteOn(Track, pTable);
}
