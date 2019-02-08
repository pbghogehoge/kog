/*
 *   PbgMidi.cpp   : ＭＩＤＩコントロール
 *
 */

#include "PbgMidi.h"
#include "MidiSub.h"
#include "PbgMem.h"
#include "PbgFile.h"
#include "PbgError.h"
#include "LzUty\\LzDecode.h"



namespace Pbg {



// コンストラクタ //
MidiEnum::MidiEnum()
{
	int		i, n;

	// デバイス数を取得する //
	m_NumDevices = midiOutGetNumDevs() + 1;

	// デバイス性能の格納先を作成する //
	m_pDeviceCaps = NewEx(MIDIOUTCAPS[m_NumDevices]);
	if(NULL == m_pDeviceCaps){	// 失敗
		m_NumDevices = 0;		// デバイス数を０とする
		return;
	}

	// ループ用にローカル変数へと代入 //
	n = m_NumDevices;

	// 各デバイスの性能を取得する //
	for(i=0; i<n; i++){
		midiOutGetDevCaps(i-1, &m_pDeviceCaps[i], sizeof(MIDIOUTCAPS));
	}
}


// デストラクタ //
MidiEnum::~MidiEnum()
{
	// デバイス性能格納バッファを解放する //
	DeleteArray(m_pDeviceCaps);
}


// デバイス数を取得する //
int MidiEnum::GetNumDevices(void)
{
	// デバイス数を返すのみ //
	return m_NumDevices;
}


// デバイス名を取得する //
BOOL MidiEnum::GetDeviceName(int ID, char *pDeviceName)
{
	// 格納先が無効 //
	if(NULL == pDeviceName) return FALSE;

	// デバイス番号が不正ならば、文字列を "" として返す //
	if((ID >= m_NumDevices) || (ID < 0)){
		pDeviceName[0] = '\0';
		return FALSE;
	}

	// 文字列のコピーを //
	strcpy(pDeviceName, m_pDeviceCaps[ID].szPname);

	return TRUE;
}


// デバイスを開く //
HMIDIOUT MidiEnum::OpenDevice(int ID)
{
	UINT		Ret;
	HMIDIOUT	hMidiOut;

	// デバイスＩＤが範囲外である //
	if((ID >= m_NumDevices) || (ID < 0)) return NULL;

	// デバイスをオープンする
	Ret = midiOutOpen(&hMidiOut, ID-1, 0, 0, CALLBACK_NULL);

	if(MMSYSERR_NOERROR == Ret) return hMidiOut;	// 成功
	else                        return NULL;		// 失敗
}


// デバイスを閉じる //
void MidiEnum::CloseDevice(HMIDIOUT &rhMidi)
{
	if(rhMidi){		// デバイスが生きていれば
		midiOutClose(rhMidi);	// クローズして
		rhMidi = NULL;			// ハンドルの無効化
	}
}






// コンストラクタ //
PbgMidi::PbgMidi()
{
	m_NumTracks    = 0;					// トラック数
	m_aTrack       = NULL;				// トラックデータの格納先
	m_ExtraTempo   = MIDI_STDTEMPO;		// 標準のテンポ
	m_FadeTarget   = MIDI_STDVOLUME;	// フェードアウト対象
	m_MasterVolume = MIDI_STDVOLUME;	// マスターボリューム
	m_pCallBack    = NULL;				// イベント通知関数

	m_bAlwaysCloseDevice = FALSE;		// デバイスを閉じるか
}


// デストラクタ //
PbgMidi::~PbgMidi()
{
	m_Critical.EnterCS();			// クリティカルセクションに突入
	m_bAlwaysCloseDevice = TRUE;	// 最後は閉じないと困る
	m_Critical.LeaveCS();			// クリティカルセクションから脱出

	// 各種オブジェクトを破棄 //
	Cleanup();
}


// ファイルから曲をロード //
BOOL PbgMidi::Load(char *pFileName)
{
	Pbg::CPbgFile		File;
	DWORD				Size, ReadSize;
	BOOL				bRet;
	BYTE				*pMemory;

	if(FALSE == File.Open(pFileName, "r")){
		return FALSE;
	}

	Size    = File.GetSize();		// サイズ取得
	pMemory = NewEx(BYTE[Size]);	// メモリ確保

	// ファイルからメモリに転送 //
	if(FALSE == File.Read(pMemory, Size, &ReadSize)){
		DeleteEx(pMemory);
		return FALSE;
	}

	bRet = LoadFromMemory(pMemory);
	DeleteEx(pMemory);

	return bRet;
}


// 圧ファイルからロード(ID) //
BOOL PbgMidi::LoadP(char *pPackFileName, char *pFileID)
{
	Pbg::LzDecode		File;		// ファイル解凍用
	BOOL				bRet;		// 戻り値格納先
	BYTE				*pMemory;	// 一時解凍先

	// ファイルをオープンする。壊れていたり、存在しない場合は失敗 //
	if(FALSE == File.Open(pPackFileName)){
		return FALSE;
	}

	pMemory = (BYTE *)File.Decode(pFileID);	// まずは解凍を試みる
	if(NULL == pMemory) return FALSE;		// 解凍出来なかった場合

	bRet = LoadFromMemory(pMemory);		// 読み込み
	MemFree(pMemory);					// 解放

	return bRet;
}


// 圧ファイルからロード(No) //
BOOL PbgMidi::LoadP(char *pPackFileName, DWORD FileNo)
{
	Pbg::LzDecode		File;		// ファイル解凍用
	BOOL				bRet;		// 戻り値格納用
	BYTE				*pMemory;	// 一時解凍先

	// ファイルをオープンする。壊れていたり、存在しない場合は失敗 //
	if(FALSE == File.Open(pPackFileName)){
		return FALSE;
	}

	// 解凍出来なかった場合 //
	pMemory = (BYTE *)File.Decode(FileNo);
	if(NULL == pMemory) return FALSE;

	bRet = LoadFromMemory(pMemory);		// 読み込み
	MemFree(pMemory);					// 解放

	return bRet;
}


// 再生を開始する //
BOOL PbgMidi::Play(BOOL bLoop)
{
	// 再生中に呼び出される場合もあるので、まずは停止させる //
	Stop();

	// ロード出来ていない場合 //
	if(NULL == m_aTrack) return FALSE;

	// デバイスが立ち上がっていない場合 //
	if(NULL == m_Port[0].m_hMidi){
		// Ａポート //
		m_Port[0].m_hMidi = OpenDevice(m_Port[0].m_PortID);
		if(NULL == m_Port[0].m_hMidi) return FALSE;

		// Ｂポート //
		if(m_Port[0].m_PortID != m_Port[1].m_PortID){
			m_Port[1].m_hMidi = OpenDevice(m_Port[1].m_PortID);
			if(NULL == m_Port[1].m_hMidi) return FALSE;
		}
		else{
			m_Port[1].m_hMidi = m_Port[0].m_hMidi;
		}
	}

	// トラックその他を初期化する //
	InitializeTrack();

	if(bLoop) m_PlayState = MIDIST_LOOP;	// ループ再生
	else      m_PlayState = MIDIST_PLAY;	// 通常再生

	// タイマーを立ち上げる //
	if(FALSE == m_Timer.Start(MidiCallbackFn, 10, (DWORD)this)){
		m_PlayState = MIDIST_STOP;	// 失敗したなら、停止状態へと
		return FALSE;				// 失敗..
	}

	// 正常終了 //
	return TRUE;
}


// [CS] 停止する //
void PbgMidi::Stop(void)
{
	m_Critical.EnterCS();	// クリティカルセクション入
	m_Timer.Stop();			// タイマーを停止させる
	m_Critical.LeaveCS();	// クリティカルセクション出

	// 以下の関数は、メインの再生部からも呼び出される          //
	// Stop() を直接呼び出すとデッドロックを引き起こすので注意 //
	InternalStop();
}


// コールバック関数が使用する停止用関数 //
void PbgMidi::InternalStop(void)
{
	// この部分で、ＧＭリセット等々 //
	GM_Reset();

	if(m_bAlwaysCloseDevice){
		// Ａポートを閉じる //
		CloseDevice(m_Port[0].m_hMidi);

		// 必要に応じて、Ｂポートも閉じる //
		if(m_Port[0].m_PortID != m_Port[1].m_PortID){
			CloseDevice(m_Port[1].m_hMidi);
		}
		else{	// Ａポートとして動作していた場合
			m_Port[1].m_hMidi = NULL;
		}

		//PbgError("Device Closed.");
	}

	m_PlayState  = MIDIST_STOP;		// 停止して
	m_bIsFadeOut = FALSE;			// フェードを切る

	// ポート情報を初期化する //
	InitializePortData();
}


// [CS] フェード(Volume までフェード) //
void PbgMidi::Fade(BYTE Speed, BYTE Volume)
{
	m_Critical.EnterCS();	// クリティカルセクション突入

	switch(m_PlayState){
		// フェードアウトをかける条件を満たしている //
		case MIDIST_PLAY: case MIDIST_LOOP:// case MIDIST_FADE:
			m_FadeWait   = (255-Speed)>>4;	// フェードの待ち時間
			m_bIsFadeOut = TRUE;			// フェード状態へ
			//m_PlayState  = MIDIST_FADE;	// フェード状態へ
			m_FadeCount  = 0;				// カウンタ
			m_FadeOutCh  = 0;				// フェードアウト対象チャンネル
			m_FadeTarget = Volume;			// フェード最終値
		break;

		// 停止中にフェードはできません... //
		default:
		break;
	}

	m_Critical.LeaveCS();	// クリティカルセクション脱出
}


// [CS] テンポ //
void PbgMidi::Tempo(char Tempo)
{
	m_Critical.EnterCS();	// クリティカルセクション突入

	m_ExtraTempo = MIDI_STDTEMPO + Tempo;

	m_Critical.LeaveCS();	// クリティカルセクション脱出
}


// [CS] マスターボリューム変更 //
void PbgMidi::SetMasterVolume(BYTE Volume)
{
	int		FadeVolume;

	m_Critical.EnterCS();		// クリティカルセクション突入
	m_MasterVolume = Volume;		// マスターボリュームに反映
	FadeVolume     = m_FadeTarget;	// フェードアウト用ボリューム
	m_Critical.LeaveCS();		// クリティカルセクション脱出

	Fade(255, FadeVolume);
}


// データを解放する //
void PbgMidi::Cleanup(void)
{
	Stop();

	// 全トラックのデータを解放する //
	if(m_aTrack){
		DeleteArray(m_aTrack);
	}

	m_NumTracks     = 0;
	m_MusicTitle[0] = '\0';
}


// [CS] イベント通知関数をセットする(NULL で解除できる) //
void PbgMidi::SetNotify(PBGMIDI_CALLBACK pFunction)
{
	m_Critical.EnterCS();
	m_pCallBack = pFunction;
	m_Critical.LeaveCS();
}


// [CS] ノートオンの状態を取得 //
void PbgMidi::GetNoteOn(int Track, BYTE *pTable)
{
	BYTE		*pOn, *pOff;
	int			i;

	m_Critical.EnterCS();

	pOn  = m_Port[0].m_NoteOn[Track];
	pOff = m_Port[0].m_NoteOffMask[Track];

	memcpy(pTable, pOn, 128);

	for(i=0; i<128; i++){
		pOn[i] &= pOff[i];
	}

	m_Critical.LeaveCS();
}


// [CS] パンの状態を取得 //
void PbgMidi::GetPanPod(BYTE *pTable)
{
	m_Critical.EnterCS();
	memcpy(pTable, m_Port[0].m_PanPod, 16);
	m_Critical.LeaveCS();
}


// [CS] ボリュームの状態を取得 //
void PbgMidi::GetVolume(BYTE *pTable)
{
	m_Critical.EnterCS();
	memcpy(pTable, m_Port[0].m_Volume, 16);
	m_Critical.LeaveCS();
}


// [CS] エクスプレッションの状態を取得 //
void PbgMidi::GetExpression(BYTE *pTable)
{
	m_Critical.EnterCS();
	memcpy(pTable, m_Port[0].m_Expression, 16);
	m_Critical.LeaveCS();
}


// [CS] 現在の再生時間を取得する //
DWORD PbgMidi::GetPlayTimes(void)
{
	DWORDLONG		time;
	BYTE			state;

	m_Critical.EnterCS();
		time  = m_PlayTime;
		state = m_PlayState;
	m_Critical.LeaveCS();

	if(MIDIST_STOP == state) return 0;
	else                     return (DWORD)time;
}


// [CS] タイトル文字列を取得する //
BOOL PbgMidi::GetTitle(char *pData)
{
	if(NULL == pData) return FALSE;

	m_Critical.EnterCS();	// クリティカルセクション突入
	strcpy(pData, m_MusicTitle);
	m_Critical.LeaveCS();	// クリティカルセクション脱出

	if('\0' != pData[0]) return TRUE;
	else                 return FALSE;
}


// [CS] 現在の状態を取得する //
BYTE PbgMidi::GetStatus(void)
{
	BYTE	temp;

	m_Critical.EnterCS();	// クリティカルセクション突入
	temp = m_PlayState;
	m_Critical.LeaveCS();	// クリティカルセクション脱出

	return temp;
}


// [CS] 使用デバイスを変更する(32ch 対応) //
BOOL PbgMidi::ChangeDevice(int PortA_ID, int PortB_ID)
{
	BYTE	State;		// 停止前の状態
	int		NumDevice;	// MIDI デバイス数
	BOOL	DeviceFlag;	// デバイス解放フラグ

	// これは駄目です //
	if(PortA_ID < 0 || PortB_ID < 0) return FALSE;

	m_Critical.EnterCS();	// クリティカルセクション突入
	NumDevice  = GetNumDevices();		// デバイス数
	DeviceFlag = m_bAlwaysCloseDevice;	// 解放フラグ
	State      = m_PlayState;			// 現在の状態
	m_Critical.LeaveCS();	// クリティカルセクション脱出

	// デバイスＩＤがおかしい //
	if((PortA_ID >= NumDevice) || (PortB_ID >= NumDevice)){
		return FALSE;
	}

	// 再生中ならば、強制的に停止する              //
	// 出来れば変更は停止中にやってもらいたいが... //
	if(State != MIDIST_STOP){
		Stop();
	}

	// デバイスを強制的に閉じる                     //
	// (ここの時点でタイマーは停止している点に注意) //
	m_bAlwaysCloseDevice = TRUE;
	InternalStop();
	m_bAlwaysCloseDevice = DeviceFlag;

	// この時点でタイマーは停止しているはずだから、//
	// 問題なく代入できる                          //
	m_Port[0].m_PortID = PortA_ID;	// ポートＡ
	m_Port[1].m_PortID = PortB_ID;	// ポートＢ

	// 停止前の状態により分岐する //
	switch(State){
		case MIDIST_PLAY:	return Play(FALSE);		// 通常再生中
		case MIDIST_LOOP:	return Play(TRUE);		// ループ再生中

		default:		// それ以外(STOP, ...)
		return TRUE;
	}
}


// メモリから読み込みを行う //
BOOL PbgMidi::LoadFromMemory(BYTE *pData)
{
	SMFHeader		*pHead;
	SMFMainInfo		*pInfo;
	SMFTrack		*pTrack;
	BYTE			*pCurrent;
	BYTE			*pTemp;

	int				i, NumTracks, size;

	Cleanup();

	// それは無理です //
	if(NULL == pData) return FALSE;

	pHead = (SMFHeader *)pData;

	// ヘッダを調べる //
	if(mmioFOURCC('M', 'T', 'h', 'd') != pHead->MThd){
		return FALSE;
	}

	pInfo = (SMFMainInfo *)(pData + sizeof(SMFHeader));

	// ヘッダ情報 //
	m_Format    = ConvWord(pInfo->Format);		// フォーマット
	m_NumTracks = ConvWord(pInfo->Track);		// トラック数
	m_TimeBase  = ConvWord(pInfo->TimeBase);	// タイムベース
	m_Tempo     = 1000000;						// テンポ

	// トラック数 //
	NumTracks = m_NumTracks;

	// トラックデータ格納に必要なメモリを確保する //
	m_aTrack = NewEx(MidiTrack[NumTracks]);
	if(NULL == m_aTrack) return FALSE;

	// ヘッダサイズの取得 //
	size = ConvDWord(pHead->Size) + sizeof(SMFHeader);

	// データの開始位置に移動する //
	pCurrent = pData + size;

	// 各トラックにデータを転送 //
	for(i=0; i<NumTracks; i++){
		pTrack    = (SMFTrack *)pCurrent;	// ヘッダ取得
		pCurrent += sizeof(SMFTrack);		// 現在位置を移動

		size = ConvDWord(pTrack->Size);	// トラックのサイズを取得

		pTemp = NewEx(BYTE[size]);
		if(NULL == pTemp){
			DeleteArray(m_aTrack);
			return FALSE;
		}

		memcpy(pTemp, pCurrent, size);

		// 一通り完了したので、本体に代入 //
		m_aTrack[i].m_pDataHead   = pTemp;	// データ先頭
		m_aTrack[i].m_pCurrentPos = pTemp;	// 現在位置
		m_aTrack[i].m_DataSize    = size;	// トラックのサイズを格納
		m_aTrack[i].m_bFinished   = FALSE;	// 完了フラグ
		m_aTrack[i].m_TargetPort  = 0;		// Ａポートを

		// タイトルの存在確認 //
		if('\0' == m_MusicTitle[0]){
			CheckTitle(pCurrent, size);
		}

		// 次のデータへと進む //
		pCurrent += size;
	}

	return TRUE;
}


// 各トラックの初期化を行う //
void PbgMidi::InitializeTrack(void)
{
	int			i, n;
	MidiTrack	*pTrack;

	if(NULL == m_aTrack) return;

	// フェード用ボリューム(注意 : マスターは変更せず) //
	// 2001/06/06 追加                                 //
	m_ExtraVolume = MIDI_STDVOLUME;
	m_FadeTarget  = MIDI_STDVOLUME;	// フェードアウト対象

	m_PlayTime    = 0;	// 再生時間
	m_PlayCount1  = 0;	// 再生用カウンタ１
	m_PlayCount2  = 0;	// 再生用カウンタ２

	n = m_NumTracks;

	// 各トラックの初期化を行う //
	for(i=0; i<n; i++){
		pTrack = m_aTrack + i;

		pTrack->m_pCurrentPos = pTrack->m_pDataHead;	// 現在位置
		pTrack->m_bFinished   = FALSE;					// 終了したか
		pTrack->m_Status      = 0;						// 状態
		pTrack->m_TargetPort  = 0;						// 出力対象ポート

		// ウェイトカウントを初期値にセット //
		pTrack->m_WaitCount = GetWaitCount(&(pTrack->m_pCurrentPos));
	}

	InitializePortData();
}


// ノート、パンなどのポート情報を初期化する //
void PbgMidi::InitializePortData(void)
{
	int			i, n;
	MidiPort	*pPort;

	// 出力ポート用テーブルの初期化をする //
	for(n=0; n<MIDI_NUMPORT; n++){
		pPort = &m_Port[n];

		for(i=0; i<16; i++){
			memset(pPort->m_NoteOn[i], 0, 128);		// ノートＯＮ/ＯＦＦ
			memset(pPort->m_NoteOffMask[i], 0, 128);// ノートオフマスク
			pPort->m_PanPod[i]     = 0x40;			// パン
			pPort->m_Expression[i] = 0x7f;			// エクスプレッション
			pPort->m_Volume[i]     = 0x64;			// ボリューム
		}
	}
}


// ＧＭリセットを放つ //
void PbgMidi::GM_Reset(void)
{
	BYTE		msg[6] = {0xf0, 0x7e, 0x7f, 0x09, 0x01, 0xf7};
	MIDIHDR		mh;
	HMIDIOUT	hMidA, hMidB;
	int			i;

	mh.dwFlags         = 0;
	mh.dwOffset        = 0;
	mh.dwBufferLength  = 6;
	mh.dwBytesRecorded = 6;
	mh.lpData          = (char *)msg;

	hMidA = m_Port[0].m_hMidi;
	hMidB = m_Port[1].m_hMidi;

	if(hMidA){
		for(i=0; i<16; i++){
			SendShortMsg(hMidA, 0xb0+i, 0x7b, 0x00);// オール・ノート・オフ
			SendShortMsg(hMidA, 0xb0+i, 0x78, 0x00);// オール・サウンド・オフ
		}
		midiOutReset(hMidA);

		midiOutPrepareHeader(hMidA, &mh, sizeof(MIDIHDR));
		midiOutLongMsg(hMidA, &mh, sizeof(MIDIHDR));
		midiOutUnprepareHeader(hMidA, &mh, sizeof(MIDIHDR));

		if(hMidA != hMidB){
			for(i=0; i<16; i++){
				SendShortMsg(hMidB, 0xb0+i, 0x7b, 0x00);// オール・ノート・オフ
				SendShortMsg(hMidB, 0xb0+i, 0x78, 0x00);// オール・サウンド・オフ
			}
			midiOutReset(hMidB);

			midiOutPrepareHeader(hMidB, &mh, sizeof(MIDIHDR));
			midiOutLongMsg(hMidB, &mh, sizeof(MIDIHDR));
			midiOutUnprepareHeader(hMidB, &mh, sizeof(MIDIHDR));
		}
	}

	// ＧＭリセットの後はちょっとだけ待つ必要がある //
	Sleep(50);
}



} // namespace Pbg
