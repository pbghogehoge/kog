/*
 *   CWaveMusic.cpp   : WAVEによる曲の再生
 *
 */

#include "CWaveMusic.h"
#include "PbgError.h"
#include "DSTable.h"



namespace Pbg {



// コンストラクタ //
CWaveMusic::CWaveMusic(LPDIRECTSOUND lpDS)
{
	m_lpDSNotify = NULL;	// 書き込みタイミング通知用
	m_lpDSBuffer = NULL;

	m_lpDS       = lpDS;	// ＤＳインターフェース
	m_IsPaused   = FALSE;	// 一時停止中か

	ZEROMEM(m_PosNotify);

	// イベントを作成 //
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_BufferSize      = 0;		// バッファ全体のサイズ
	m_NotifySize      = 0;		// 通知サイズ
	m_NextWriteOffset = 0;		// 次に書き込む場所(バイト指定)
	m_Progress        = 0;		// 何バイトまで再生したのかね
	m_LastPos         = 0;		// 前回の書き込み位置

	m_LoopStart = 0;		// ループ開始バイト
	m_LoopEnd   = 0;		// ループ終了バイト

	// ボリュームの初期値 //
	m_Volume = 255;

	// 読み込み対象クラスのポイント用 //
	m_pFile = NULL;
}


// デストラクタ //
CWaveMusic::~CWaveMusic()
{
	// 止めて、解放 //
	Initialize();

	// イベントハンドルを閉じる //
	if(NULL != m_hEvent){
		CloseHandle(m_hEvent);
	}
}


// 再生する(ループ再生フラグ付き) //
BOOL CWaveMusic::Play()
{
	DWORD		Length;
	HRESULT		hr;
	VOID		*pBuffer;
	BOOL		IsRestart;

	// バッファが無くちゃ、再生出来ない //
	if(NULL == m_lpDSBuffer) return FALSE;

	// ポーズ中かどうかを保存する //
	IsRestart = m_IsPaused;

	Stop();		// 止める

	// ポーズ中じゃない？ //
	if(FALSE == IsRestart){
		m_pFile->Reset();		// 最初に戻る
		m_Progress = 0;			// 再生していないから０
	}
	else{
		// ここに、巻き戻し処理を加える //
		m_pFile->Seek(m_Progress);
	}

	// 途中から再生する場合も、再生バッファに関しては先頭になる //
	hr = m_lpDSBuffer->SetCurrentPosition(0);
	m_LastPos  = 0;

	// 通知イベントを再設定する //
	hr = m_lpDSNotify->SetNotificationPositions(
					NUM_WAVMUSIC_NOTIFY+1, m_PosNotify);
	if(FAILED(hr)){
		PbgError("CWaveMusic::Play()  SetNotificationPositions() に失敗");
		Initialize();	// 初期化
		return FALSE;
	}

	hr = m_lpDSBuffer->Lock(0, m_BufferSize, &pBuffer, &Length, NULL, NULL, 0);
	if(FAILED(hr)){
		if(FALSE == Restore()){
			PbgError("CWaveMusic::Play()  Restore() に失敗");
			Initialize();
			return FALSE;
		}

		hr = m_lpDSBuffer->Lock(0, m_BufferSize, &pBuffer, &Length, NULL, NULL, 0);
		if(FAILED(hr)){
			PbgError("CWaveMusic::Play()  Lock() に失敗");
			Initialize();
			return FALSE;
		}
	}

	// 最初のデータを書き込んでみようか //
	if(FALSE == WriteData((BYTE *)pBuffer, Length)){
		PbgError("CWaveMusic::Play()  FillBuffer() に失敗");
		Initialize();
		return FALSE;
	}

	// 書き込んだら、後始末  //
	hr = m_lpDSBuffer->Unlock(pBuffer, Length, NULL, 0);
	if(FAILED(hr)){
		PbgError("CWaveMusic::Play()  Unlock() に失敗");
		Initialize();
		return FALSE;
	}

	m_NextWriteOffset  = Length;		// 次のデータ書き込み先
	m_NextWriteOffset %= m_BufferSize;

	if(FALSE == CreateThread()){
		PbgError("CWaveMusic::Play()  CreateThread() に失敗");
		Initialize();
		return FALSE;
	}

	return TRUE;
}


// 再生場所の移動 //
BOOL CWaveMusic::Seek(DWORD Pos)
{
	BOOL	IsPlaying = FALSE;	// 再生中にシークしたいのかな

	// ファイルをロードしていない //
	if(NULL == m_pFile) return FALSE;

	// 内部時間(すなわちバイト数)に変更する //
	Pos *= m_pFile->GetFormat()->nBlockAlign;

	// 範囲外はループ先頭に移動 //
	if(Pos >= m_LoopEnd){
		Pos = m_LoopStart;	// 先頭
	}

	// 再生中なら、停止 //
	if(IsActiveThread()){
		IsPlaying = TRUE;	// 再生中
		Stop();				// 止まって
	}

	// 現在位置をずらして、ポーズ状態にする //
	m_Progress = Pos;
	m_IsPaused = TRUE;

	// 再生中だった場合は再び再生に戻ることになる //
	if(IsPlaying){
		Play();
	}

	return TRUE;
}


// 曲を停止する //
void CWaveMusic::Stop(void)
{
	// 一時停止状態を無効化する //
	m_IsPaused = FALSE;

	if(NULL == m_lpDSBuffer) return;

	DisableThread();		// スレッド無効化
	PulseEvent(m_hEvent);	// イベント待ちを解除
	StopThread();			// スレッドの停止まで待つ

	PulseEvent(m_hEvent);	// イベント待ちを解除
}


// 曲を一時停止する //
void CWaveMusic::Pause(void)
{
	// 通常の停止要求で止める //
	Stop();

	// 一時停止状態にする //
	m_IsPaused = TRUE;
}


BOOL CWaveMusic::Restore(void)
{
	HRESULT		hr;
	DWORD		Status;

	if(NULL == m_lpDSBuffer) return FALSE;

	hr = m_lpDSBuffer->GetStatus(&Status);
	if(FAILED(hr)) return FALSE;

	// ロストしてる //
	if(Status & DSBSTATUS_BUFFERLOST){
		while(1){
			hr = m_lpDSBuffer->Restore();
			if(SUCCEEDED(hr)) break;
			if(hr != DSERR_BUFFERLOST) return FALSE;

			Sleep(10);	// SDK のサンプルより
		}
	}

	// 成功したのね //
	return TRUE;
}


// ループタイミングのセット //
BOOL CWaveMusic::SetLoopPosition(DWORD Start, DWORD End)
{
	short nBlockAlign;

	// ファイルが読み込まれていない //
	if(NULL == m_pFile){
		return FALSE;
	}

	// 「ステレオ・モノラル」「８ビット・１６ビット」を考慮にいれる //
	nBlockAlign = m_pFile->GetFormat()->nBlockAlign;

	// 位置の補正を行う //
	Start *= nBlockAlign;		// 開始位置
	End   *= nBlockAlign;		// 終了位置

	// 再生中の場合、変更できない //
	if(IsActiveThread()){
		return FALSE;
	}

	if(Start >= End){
		return FALSE;
	}

	// 終了時刻がデータサイズより大きい //
	if(End > m_pFile->GetSize()){
		End = m_pFile->GetSize();	// 自動補正
	}

	m_LoopStart = Start;	// 開始位置
	m_LoopEnd   = End;		// 終了位置

	// 終了位置をこれでセットする                                       //
	// →開始位置はこのクラスで扱うが、終了位置はCWaveFileRead に任せる //
	m_pFile->SetEnd(m_LoopEnd);

	return TRUE;
}


// ループタイミングの取得 //
BOOL CWaveMusic::GetLoopPosition(DWORD *Start, DWORD *End)
{
	short BlockAlign;

	// ファイル開いてません //
	if(NULL == m_pFile) return FALSE;

	BlockAlign = m_pFile->GetFormat()->nBlockAlign;
	if(BlockAlign == 0){	// ゼロ除算はだめ
		*Start = 0;
		*End   = 0;

		return FALSE;
	}

	*Start = m_LoopStart / BlockAlign;	// クラス外の値に変更
	*End   = m_LoopEnd   / BlockAlign;	// クラス外の値に変更

	// ここに来ないのは呼び出し側に問題がある //
	return TRUE;
}


// 現在位置の取得 //
DWORD CWaveMusic::GetCurrentPosition(void)
{
	short BlockAlign;

	// ファイルが開かれていない //
	if(NULL == m_pFile) return 0;

	// ゼロ除算防止 //
	BlockAlign = m_pFile->GetFormat()->nBlockAlign;
	if(0 == BlockAlign) return 0;

	// クラス外の値に変更 //
	return m_Progress / BlockAlign;
}


// ファイル終端位置の取得 //
DWORD CWaveMusic::GetEofPosition(void)
{
	short BlockAlign;

	// ファイルが開かれていない //
	if(NULL == m_pFile) return 0;

	// ゼロ除算防止 //
	BlockAlign = m_pFile->GetFormat()->nBlockAlign;
	if(0 == BlockAlign) return 0;

	return m_pFile->GetSize() / BlockAlign;
}


// 周波数の取得 //
DWORD CWaveMusic::GetFrequency(void)
{
	// ファイルが開かれていない //
	if(NULL == m_pFile) return 0;

	return m_pFile->GetFormat()->nSamplesPerSec;
}


// 現在の波形を表示する(Stereo / 8Bit に変換される) //
void CWaveMusic::GetCurrentScope(BYTE Table[128+128])
{
	ZEROMEM(Table);
}


// ボリュームをセットする //
void CWaveMusic::SetVolume(BYTE Volume)
{
	HRESULT		hr;
	LONG		DSoundVolume;

	m_Volume     = Volume;
	DSoundVolume = DSVolumeTable[Volume];

	if(m_lpDSBuffer){
		// 実際にボリュームをセットする //
		hr = m_lpDSBuffer->SetVolume(DSoundVolume);
		if(FAILED(hr)){
			PbgDebugLog("CWaveMusic::SetVolume()  SetVolume() に失敗");
		}
	}
}


// 曲をロードする //
BOOL CWaveMusic::Load(char *FileName)
{
	HRESULT				hr;
	DWORD				i;
	DWORD				nBlockAlign;
	DWORD				LoopTime;
	INT					nSamplesPerSec;
	DSBUFFERDESC		dsbd;
	BOOL				ret;


	// 初期化 //
	Initialize();

	m_pFile = &m_RawFile;

	ret = m_pFile->Open(FileName);

	// ファイルをオープンする                                                  //
	// 開けないか(RIFF ヘッダ無し) ＰＣＭで無ければ、DirectShow の使用を試みる //
	if((FALSE == ret) || (m_pFile->GetFormat()->wFormatTag != WAVE_FORMAT_PCM)){
		// DirectShow を使用してオープンを試みる //
		m_pFile = &m_DShowFile;
		if(FALSE == m_pFile->Open(FileName)){
			PbgError("CWaveMusic::Load()  ファイルのオープンに失敗");
			m_pFile = NULL;
			return FALSE;
		}
//#ifdef PBG_DEBUG
//		else{
//			PbgLog("Uses DirectShow...");
//		}
//#endif
	}

	nBlockAlign    = m_pFile->GetFormat()->nBlockAlign;	// ひとかたまり
	nSamplesPerSec = m_pFile->GetFormat()->nSamplesPerSec;// １秒あたりのSize

	// 通知を行う間隔を計算する //
	m_NotifySize  = nSamplesPerSec * nBlockAlign / NUM_WAVMUSIC_NOTIFY;
	m_NotifySize -= m_NotifySize % nBlockAlign;

	// バッファのサイズは通知サイズと通知数の積となる //
	m_BufferSize = m_NotifySize * NUM_WAVMUSIC_NOTIFY;

	m_Progress        = 0;		// 再生バイト数は０
	m_LastPos         = 0;		// 前回の書き込み箇所
	m_NextWriteOffset = 0;		// 次の書き込み地点
	m_IsPaused        = FALSE;	// 停止中ではない

	// 要求するバッファを記述する //
	ZEROMEM(dsbd);								// ０めもりぃ
	dsbd.dwSize        = sizeof(DSBUFFERDESC);	// 構造体のサイズそのもの
	dsbd.dwBufferBytes = m_BufferSize;			// バッファのサイズ
	dsbd.lpwfxFormat   = m_pFile->GetFormat();// 波ファイルのフォーマット

	// ボリューム・新型位置通知・どこに配置してもＯＫ・位置通知可能 グローバルフォーカス
	dsbd.dwFlags       = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2
                       | DSBCAPS_LOCDEFER   | DSBCAPS_CTRLPOSITIONNOTIFY
					   | DSBCAPS_GLOBALFOCUS;

	// サウンドバッファを作成する //
	hr = m_lpDS->CreateSoundBuffer(&dsbd, &m_lpDSBuffer, NULL);
	if(FAILED(hr)){
		PbgError("CWaveMusic::Load()  CreateSoundBuffer() に失敗");
		Initialize();
		return FALSE;
	}

	// DirectSoundNotify オブジェクトを作成する //
	hr = m_lpDSBuffer->QueryInterface(IID_IDirectSoundNotify
											, (LPVOID *)&m_lpDSNotify);
	if(FAILED(hr)){
		PbgError("CWaveMusic::Load()  QueryInterface() に失敗");
		Initialize();
		return FALSE;
	}

	// 通知イベントセット用構造体を記述 //
	for(i=0; i<NUM_WAVMUSIC_NOTIFY; i++){
		// 通知地点 //
		m_PosNotify[i].dwOffset = (m_NotifySize * i) + m_NotifySize - 1;
		m_PosNotify[i].hEventNotify = m_hEvent;
	}

	// 停止用イベントをセット //
	m_PosNotify[i].dwOffset     = DSBPN_OFFSETSTOP;	// 停止用オフセット
	m_PosNotify[i].hEventNotify = m_hEvent;

	// ループ位置をセットする //
	LoopTime = m_pFile->GetSize() / nBlockAlign;	// 基本的には終端でループ
	if(FALSE == SetLoopPosition(0, LoopTime)){
		PbgError("CWaveMusic::Load()  ループ位置の確定に失敗");
		Initialize();
		return FALSE;
	}

	SetVolume(m_Volume);

	return TRUE;
}


// DSound オブジェクトを破棄する //
void CWaveMusic::Initialize(void)
{
	// これは、スレッドの停止を意味する //
	Stop();

	SAFE_RELEASE(m_lpDSNotify);		// 通知イベント
	SAFE_RELEASE(m_lpDSBuffer);		// サウンドバッファ

	// WAVE ファイルを閉じる //
	if(m_pFile){
		m_pFile->Close();
	}

	// 通知イベント構造体をゼロ初期化 //
	ZEROMEM(m_PosNotify);

	m_BufferSize      = 0;	// バッファのサイズ
	m_NotifySize      = 0;	// 通知イベントの間隔
	m_NextWriteOffset = 0;	// 次に書き込むところ
	m_Progress        = 0;	// 現在の再生バイト数
	m_LastPos         = 0;	// 前回の書き込み地点

	m_LoopStart = 0;	// ループの開始位置
	m_LoopEnd   = 0;	// ループの終了位置

	m_IsPaused = FALSE;	// ポーズ中かどうか

	// 読み込み対象クラスのポイント用 //
	m_pFile = NULL;
}


// どこまで再生したか示す変数を更新する //
void CWaveMusic::UpdateProgress(void)
{
	HRESULT		hr;
	DWORD		PlayPos;
	DWORD		WritePos;
	DWORD		Played;

	// 今の場所を教えて //
	hr = m_lpDSBuffer->GetCurrentPosition(&PlayPos, &WritePos);
	if(FAILED(hr)) return;

	// 再生完了したバイト数（前回との差分）を求める //
	if(PlayPos < m_LastPos) Played = m_BufferSize - m_LastPos + PlayPos;
	else                    Played = PlayPos - m_LastPos;

	m_Progress += Played;	// 再生完了バイト数
	m_LastPos   = PlayPos;	// 現在位置は次回の読み込みで使用する

	// ループ再生対策 //
	if(m_Progress >= m_LoopEnd){
		// 終端を過ぎていたら、はみ出した分だけループの先頭位置から進める //
		// ただし、さらに大きい場合を考えてループを行う範囲の大きさで     //
		// 余りを求めている                                               //
		m_Progress -= m_LoopEnd;
		m_Progress %= (m_LoopEnd - m_LoopStart);
		m_Progress += m_LoopStart;
	}
}


// 次のデータを補充する //
BOOL CWaveMusic::FillNextSoundData(void)
{
	HRESULT			hr;
	VOID			*pBuffer;
	DWORD			Length;

	UpdateProgress();	// 状況を更新

	hr = m_lpDSBuffer->Lock(m_NextWriteOffset, m_NotifySize,
								&pBuffer, &Length, NULL, NULL, 0);

	// ダメな場合は、修復 //
	if(FAILED(hr)){
		// 復旧もダメ //
		if(FALSE == Restore()) return FALSE;

		hr = m_lpDSBuffer->Lock(m_NextWriteOffset, m_NotifySize,
									&pBuffer, &Length, NULL, NULL, 0);
		if(FAILED(hr)) return FALSE;
	}

	// データを書き込んで //
	WriteData((BYTE *)pBuffer, Length);

	// 書き込み終わったから、閉じて //
	hr = m_lpDSBuffer->Unlock(pBuffer, Length, NULL, 0);
	if(FAILED(hr)) return FALSE;

	// 次に書き込みを行うオフセットを更新 //
	m_NextWriteOffset += Length;
	m_NextWriteOffset %= m_BufferSize;

	return TRUE;
}


// 指定だけデータを書き込み //
BOOL CWaveMusic::WriteData(BYTE *pBuffer, DWORD BufferLength)
{
	UINT		nActualBytesWritten;
	UINT		nWritten;

	// ファイルから読み込んで、読み込めたサイズを取得する //
	nActualBytesWritten = m_pFile->Read(pBuffer, BufferLength);

	// 要求されたサイズより、書き込めたサイズが小さかった場合 //
	if(nActualBytesWritten < BufferLength){
		// すでに書き込んだサイズを保存する //
		nWritten = nActualBytesWritten;

		// 全部書き込むまで //
		while(nWritten < BufferLength){
			m_pFile->Seek(m_LoopStart);	// ループ先頭まで巻き戻し

			// 今回、書き込めたサイズを求める //
			nActualBytesWritten =
				m_pFile->Read(pBuffer+nWritten, BufferLength-nWritten);

			// 読み込み失敗 //
			if(nActualBytesWritten < 0) return FALSE;

			nWritten += nActualBytesWritten;
		}
	}

	return TRUE;
}


// 曲の再生用スレッド //
void CWaveMusic::ThreadFunction(void)
{
	HRESULT			hr;

	if(NULL == this) return;
	if(NULL == m_lpDSBuffer) return;

	hr = m_lpDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if(FAILED(hr)){
		if(FALSE == Restore()) return;

		hr = m_lpDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
		if(FAILED(hr)) return;
	}

	// スレッドが有効な間、実行する //
	while(IsEnableThread()){
		// イベント待ち //
		WaitForSingleObject(m_hEvent, INFINITE);

		if(FALSE == FillNextSoundData()) break;
	}

	// 後始末 //
	hr = m_lpDSBuffer->Stop();
}



} // namespace Pbg
