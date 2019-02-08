/*
 *   CSoundBuffer.cpp   : サウンドバッファ管理
 *
 */

#include "CSoundBuffer.h"
#include "CSoundState.h"
#include "PbgError.h"
#include "PbgMem.h"



namespace Pbg {



// コンストラクタ //
CSoundBuffer::CSoundBuffer(LPDIRECTSOUND lpDS, BYTE &rMasterVolume)
	: m_rMasterVolume(rMasterVolume)
{
	m_aBuffer   = NULL;		// DirectSoundBuffer
	m_lpDS      = lpDS;		// 親クラスで生成したインターフェース
	m_NumBuffer = 0;		// バッファ数
}


// デストラクタ //
CSoundBuffer::~CSoundBuffer()
{
	Initialize();
}


// デフォルト再生 //
void CSoundBuffer::Play(void)
{
	if(NULL == this) return;

	Play(0, DSBFREQUENCY_ORIGINAL, 200);
}


// 特殊再生 //
void CSoundBuffer::Play(LONG Pan, DWORD Frequency, BYTE Volume)
{
	DWORD					n, Target, MaxPos;
	DWORD					Status;
	BOOL					IsPlaying;
	HRESULT					hr;
	LPDIRECTSOUNDBUFFER		lpBuf;

	if(NULL == this) return;

	Target    = m_NumBuffer;	// 再生対象
	MaxPos    = 0;				// 最大の再生時間
	IsPlaying = TRUE;			// 再生中かどうかのフラグ

	for(n=0; n<m_NumBuffer; n++){
		// 一応、高速化を図る //
		lpBuf = m_aBuffer[n].m_lpBuffer;

		// 再生中かどうかを調べる //
		hr = lpBuf->GetStatus(&Status);
		if(FAILED(hr)) return;

		// 再生中でないバッファが見つかった場合、このバッファに再生要求を //
		// 出す。(占有中であれば、必ず再生中のはずである)                 //
		if(0 == (Status & DSBSTATUS_PLAYING)){
			Target    = n;		// 再生対象を確定
			IsPlaying = FALSE;	// 再生中ではない
			break;
		}

		if(NULL == m_aBuffer[n].m_ppOwner){
			hr = lpBuf->GetCurrentPosition(&Status, NULL);
			if(FAILED(hr)) return;

			// 再生時間の最も長いバッファを削除対象とする //
			if(Status > MaxPos){
				MaxPos = Status;	// 最大値格納
				Target = n;			// 再生対象を一時的に確定
			}
		}
	}

	// 全てのバッファが占有されている場合 //
	if(Target == m_NumBuffer) return;

	// そのバッファが再生中の場合 //
	if(IsPlaying){
		m_aBuffer[Target].Stop();		// 停止要求
	}

	m_aBuffer[Target].SetPan(Pan);					// パン
	m_aBuffer[Target].SetFrequency(Frequency);		// 周波数
	m_aBuffer[Target].SetVolume(Volume);			// ボリューム

	// 引数は、(予約済み, 優先順位(０は最低), フラグ)となる //
	hr = m_aBuffer[Target].m_lpBuffer->Play(0, 0, 0);

	// バッファがロストしていた場合 //
	if(DSERR_BUFFERLOST == hr){
		// リストアして、もう一度再生を試みる //
		if(TRUE == Restore()){
			// 失敗しても特別な処理は行わない //
			m_aBuffer[Target].m_lpBuffer->Play(0, 0, 0);
		}
	}
}


// 占有ループ再生 //
void CSoundBuffer::PlayEx(CSoundState *&lprState)
{
	DWORD					n, Target, MaxPos;
	DWORD					Status;
	BOOL					IsPlaying;
	HRESULT					hr;
	LPDIRECTSOUNDBUFFER		lpBuf;

	// 参照元のポインタを初期化する(失敗時に備える) //
	lprState = NULL;

	if(NULL == this) return;

	Target    = m_NumBuffer;	// 再生対象
	MaxPos    = 0;				// 最大の再生時間
	IsPlaying = TRUE;			// 再生中かどうかのフラグ

	for(n=0; n<m_NumBuffer; n++){
		lpBuf = m_aBuffer[n].m_lpBuffer;

		// 再生中かどうかを調べる //
		hr = lpBuf->GetStatus(&Status);
		if(FAILED(hr)) return;

		// 再生中でないバッファが見つかった場合、このバッファに再生要求を //
		// 出す。(占有中であれば、必ず再生中のはずである)                 //
		if(0 == (Status & DSBSTATUS_PLAYING)){
			Target    = n;		// 再生対象を確定
			IsPlaying = FALSE;	// 再生中ではない
			break;
		}

		// 所有者がいない場合、奪い去ることができる //
		if(NULL == m_aBuffer[n].m_ppOwner){
			hr = lpBuf->GetCurrentPosition(&Status, NULL);
			if(FAILED(hr)) return;

			// 再生時間の最も長いバッファを削除対象とする //
			if(Status > MaxPos){
				MaxPos = Status;	// 最大値格納
				Target = n;			// 再生対象を一時的に確定
			}
		}
	}

	// 全てのバッファが占有されている場合 //
	if(Target == m_NumBuffer) return;

	// そのバッファが再生中の場合 //
	if(IsPlaying){
		m_aBuffer[Target].Stop();		// 停止要求
	}

	m_aBuffer[Target].SetPan(0);								// パン
	m_aBuffer[Target].SetFrequency(DSBFREQUENCY_ORIGINAL);		// 周波数
	m_aBuffer[Target].SetVolume(0);								// ボリューム

	// 引数は、(予約済み, 優先順位(０は最低), フラグ)となる //
	hr = m_aBuffer[Target].m_lpBuffer->Play(0, 0, DSBPLAY_LOOPING);

	// バッファがロストしていた場合 //
	if(DSERR_BUFFERLOST == hr){
		// リストアして、もう一度再生を試みる //
		if(TRUE == Restore()){
			// 失敗しても特別な処理は行わない //
			m_aBuffer[Target].m_lpBuffer->Play(0, 0, DSBPLAY_LOOPING);
		}
	}

	// 所有権の割り当て //
	m_aBuffer[Target].m_ppOwner = &lprState;
	lprState                    = &m_aBuffer[Target];
}


// 全バッファの停止＆バッファ占有権を奪う //
void CSoundBuffer::StopAll(void)
{
	DWORD		n;

	if(NULL == this) return;

	for(n=0; n<m_NumBuffer; n++){
		m_aBuffer[n].Stop();
	}
}


// ファイルをオープンする //
BOOL CSoundBuffer::Load(char *FileName, DWORD NumBuffer)
{
	if(NULL == this) return FALSE;

	// バッファを初期化する //
	Initialize();

	if(FALSE == m_WaveFile.Open(FileName)){
		PbgError("CSoundBuffer::Load()  ファイルのオープンに失敗");
		return FALSE;
	}

	// バッファを作成 //
	return CreateBuffer(NumBuffer);
}


// 圧ファイルをオープンする[FileID] //
BOOL CSoundBuffer::LoadP(char *pPackFileName, char *pFileID, DWORD NumBuffer)
{
	if(NULL == this) return FALSE;

	// バッファを初期化する //
	Initialize();

	// 圧ファイルをオープンする //
	if(FALSE == m_WaveFile.OpenP(pPackFileName, pFileID)){
		PbgError("CSoundBuffer::LoadP()  ファイルのオープンに失敗");
		return FALSE;
	}

	// バッファを作成してさようなら //
	return CreateBuffer(NumBuffer);
}


// 圧ファイルをオープンする[FileNo] //
BOOL CSoundBuffer::LoadP(char *pPackFileName, DWORD FileNo, DWORD NumBuffer)
{
	if(NULL == this) return FALSE;

	// バッファを初期化する //
	Initialize();

	// ファイルをオープンする //
	if(FALSE == m_WaveFile.OpenP(pPackFileName, FileNo)){
		PbgError("CSoundBuffer::LoadP()  ファイルのオープンに失敗");
		return FALSE;
	}

	return CreateBuffer(NumBuffer);
}


// マスターボリュームが変更された //
void CSoundBuffer::OnMasterVolumeChanged(void)
{
	DWORD			i;

	if(NULL == this) return;

	for(i=0; i<m_NumBuffer; i++){
		m_aBuffer[i].OnMasterVolumeChanged(m_rMasterVolume);
	}
}


// オブジェクトを解放する //
void CSoundBuffer::Initialize(void)
{
	if(NULL == this) return;

	// 全効果音を停止する //
	StopAll();

	// バッファが有効なら、全て削除する //
	if(NULL != m_aBuffer){
		DeleteArray(m_aBuffer);
	}

	m_NumBuffer = 0;		// バッファ数をゼロ初期化
	m_WaveFile.Close();		// 波ファイルをクローズする
}


// バッファを復旧する //
BOOL CSoundBuffer::Restore(void)
{
	DWORD					i;
	DWORD					Status;
	LPDIRECTSOUNDBUFFER		lpBuf;
	HRESULT					hr;

	if(NULL == this) return FALSE;

	if(NULL == m_aBuffer){
		PbgError("CSoundBuffer::Restore()  バッファの復旧に失敗");
	}

	// 作成した全てのバッファに対して //
	for(i=0; i<m_NumBuffer; i++){
		// 参照回数を減らす為にポインタに代入                                //
		// 注意：m_aBuffer は配列として確保されているのでm_NumBuffer 個だけ  //
		//       存在する事が保証されている                                  //
		lpBuf = m_aBuffer[i].m_lpBuffer;

		if(lpBuf != NULL){
			// 現在のバッファの状態を調べる //
			hr = lpBuf->GetStatus(&Status);
			if(FAILED(hr)) return FALSE;

			// ロストしていた場合 //
			if(Status & DSBSTATUS_BUFFERLOST){
				while(1){
					hr = lpBuf->Restore();
					if(SUCCEEDED(hr)) break;
					if(hr != DSERR_BUFFERLOST) return FALSE;

					Sleep(10);	// 何でだろうね？<-SDK のサンプルより
				}

				// バッファにＷＡＶＥファイルを取り込む //
				if(FALSE == FillBuffer(&m_aBuffer[i])) return FALSE;
			}
		}
	}

	return TRUE;
}


// バッファを作成する //
BOOL CSoundBuffer::CreateBuffer(DWORD NumBuffer)
{
	HRESULT					hr;
	DSBUFFERDESC			dsbd;
	LPDIRECTSOUNDBUFFER		lpBuf;
	DWORD					i;

	if(NULL == this) return FALSE;

	// 配列にて作成する
	m_aBuffer = NewEx(CSoundState[NumBuffer]);
	if(NULL == m_aBuffer){
		PbgError("CSoundBuffer::Load()  バッファ作成用のメモリが足りない");
		Initialize();
		return FALSE;
	}

	// サウンドバッファの作成に必要なフラグをセットする //
	ZEROMEM(dsbd);
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwBufferBytes = m_WaveFile.GetSize();
	dsbd.lpwfxFormat   = m_WaveFile.GetFormat();
	dsbd.dwFlags       = DSBCAPS_STATIC  | DSBCAPS_CTRLVOLUME
                       | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;

	// 一つ目のバッファは素直に作成する //
	hr = m_lpDS->CreateSoundBuffer(&dsbd, &lpBuf, NULL);
	if(FAILED(hr)){
		PbgError("CSoundBuffer::Load()  CreateSoundBuffer()に失敗");
		Initialize();
		return FALSE;
	}

	// ポインタを割り当てる //
	m_aBuffer[0].m_lpBuffer = lpBuf;
	if(FALSE == FillBuffer(&m_aBuffer[0])){		// 読み込み
		PbgError("CSoundBuffer::FillBuffer()  ファイルの読み込みに失敗");
		Initialize();
		return FALSE;
	}

	// ２つ目以降の効果音を作成する                     //
	// 出来るならばデュプリケートすることでメモリを節約 //
	for(i=1; i<NumBuffer; i++){

		// まずは、バッファの複製を試みる //
		hr = m_lpDS->DuplicateSoundBuffer(lpBuf, &m_aBuffer[i].m_lpBuffer);

		// 複製に失敗したので、新規に作成する //
		if(FAILED(hr)){
			PbgDebugLog("デュプリケートに失敗");

			// 新たにサウンドバッファの作成 //
			hr = m_lpDS->CreateSoundBuffer(&dsbd, &m_aBuffer[i].m_lpBuffer, NULL);
			if(FAILED(hr)){
				PbgError("CSoundBuffer::Load()  CreateSoundBuffer()に失敗(2)");
				Initialize();
				return FALSE;
			}

			// バッファを埋める //
			if(FALSE == FillBuffer(&m_aBuffer[i])){
				PbgError("CSoundBuffer::Load()  ファイルの読み込みに失敗(2)");
				Initialize();
				return FALSE;
			}
		}
	}

	// 読み込み完了 //
	m_NumBuffer = NumBuffer;

	return TRUE;
}


// バッファにデータをロードする //
BOOL CSoundBuffer::FillBuffer(CSoundState *pTarget)
{
	HRESULT			hr;
	BYTE			*pbWavData;
	VOID			*pbData  = NULL;
	VOID			*pbData2 = NULL;
	DWORD			dwLength;
	DWORD			dwLength2;
	UINT			nWaveFileSize;

	if(NULL == this) return FALSE;

	nWaveFileSize = m_WaveFile.GetSize();		// ファイルサイズ
	pbWavData     = NewEx(BYTE[nWaveFileSize]);
	if(NULL == pbWavData) return FALSE;

	// 実際に読み込む //
	if(nWaveFileSize != m_WaveFile.Read(pbWavData, nWaveFileSize)){
		DeleteArray(pbWavData);
		return FALSE;
	}

	// 読み出し元を初期状態に戻す //
	m_WaveFile.Reset();

	// バッファを先頭からロックする //
	hr = pTarget->m_lpBuffer->Lock(0, nWaveFileSize,
								&pbData, &dwLength, &pbData2, &dwLength2, 0);
	if(FAILED(hr)){
		DeleteArray(pbWavData);
		return FALSE;
	}

	// 一時メモリからバッファへと書き込み //
	memcpy(pbData, pbWavData, nWaveFileSize);

	// アンロックし //
	pTarget->m_lpBuffer->Unlock(pbData, nWaveFileSize, NULL, 0);

	// 波ファイルのデータそのものを解放 //
	DeleteArray(pbWavData);

	return TRUE;
}



} // namespace Pbg
