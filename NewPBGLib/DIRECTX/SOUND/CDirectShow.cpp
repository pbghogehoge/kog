/*
 *   CDirectShow.cpp   : DirectShow によるデコード
 *
 */


#include "CDirectShow.h"
#include "PbgError.h"



namespace PbgEx {



// コンストラクタ //
CDirectShow::CDirectShow()
{
	m_pAMStream     = NULL;	// マルチメディアストリーム
	m_pMMStream     = NULL;	// MMStream のコントロール用
	m_pStream       = NULL;	// メディアストリーム
	m_pSample       = NULL;	// AudioData からのデータ取得
	m_pAudioStream  = NULL;	// 出力する波フォーマット取得用
	m_pAudioData    = NULL;	// 元となる波データフォーマット用
	m_pGraphBuilder = NULL;	// フィルターグラフなにがし

	CoInitialize(NULL);		// ＣＯＭの初期化
}


// デストラクタ //
CDirectShow::~CDirectShow()
{
	Cleanup();			// インターフェースの解放
//	CoUninitialize();	// ＣＯＭの終了処理
}


// ファイルをオープンする //
BOOL CDirectShow::Open(char *pFileName)
{
	HRESULT			hr;
	WCHAR			wszName[MAX_PATH];


	m_Current = 0;
	m_Size    = 0;
	m_End     = 0;

	// -- 初期化が行えていない場合、即座にさよならする -- //
	if(FALSE == CreateMMStream()){
		PbgError("DirectShow の初期化に失敗しました");
		return FALSE;
	}


	// -- 文字列をワイド文字列にマップ -- //
	MultiByteToWideChar(CP_ACP, 0, pFileName, -1, wszName, MAX_PATH);


	// -- ファイルを開く -- //
	hr = m_pAMStream->OpenFile(wszName, AMMSF_RUN | AMMSF_NOCLOCK);
	if(FAILED(hr)){
		PbgComError(hr, "IAMultiMediaStream::OpenFile() に失敗");
		return FALSE;
	}


	// -- MediaStream を取得する -- //
	hr = m_pMMStream->GetMediaStream(MSPID_PrimaryAudio, &m_pStream);
	if(FAILED(hr)){
		PbgComError(hr, "IMultiMediaStream::GetMediaStream() に失敗");
		return FALSE;
	}


	hr = m_pStream->QueryInterface(IID_IAudioMediaStream, (void **)&m_pAudioStream);
	if(FAILED(hr)){
		PbgComError(hr, "IMediaStream::QueryInterface() に失敗");
		return FALSE;
	}


	// -- 出力フォーマットを取得する -- //
	hr = m_pAudioStream->GetFormat(&m_DestFormat);
	if(FAILED(hr)){
		PbgComError(hr, "IAudioMediaStream::GetFormat() に失敗");
		return FALSE;
	}


	// -- IAudioData インターフェースを取得 -- //
	hr = CoCreateInstance(	CLSID_AMAudioData
						,	NULL
						,	CLSCTX_INPROC_SERVER
						,	IID_IAudioData
						,	(void **)&m_pAudioData);
	if(FAILED(hr)){
		PbgComError(hr, "CoCreateInstance(IID_IAudioData) に失敗");
		return FALSE;
	}


	// -- 出力フォーマットを取得する -- //
	hr = m_pAudioData->SetFormat(&m_DestFormat);
	if(FAILED(hr)){
		PbgComError(hr, "IAudioData::SetFormat() に失敗");
		return FALSE;
	}


	// -- IAudioStreamSample インターフェースを作成する -- //
	hr = m_pAudioStream->CreateSample(m_pAudioData, 0, &m_pSample);
	if(FAILED(hr)){
		PbgComError(hr, "IAudioMediaStream::CreateSample() に失敗");
		return FALSE;
	}


	// -- FilterGraph を取得する -- //
	hr = m_pAMStream->GetFilterGraph(&m_pGraphBuilder);
	if(FAILED(hr)){
		PbgComError(hr, "IAMMultiMediaStream::GetFilterGraph() に失敗");
		return FALSE;
	}


	// サイズを取得する //
	m_Size = GetSize();
	m_End  = 0;

	Seek(0);

	return TRUE;
}


// ファイルを閉じる //
BOOL CDirectShow::Close(void)
{
	Cleanup();

	return TRUE;
}


// ファイル先頭に戻る //
BOOL CDirectShow::Reset(void)
{
	return Seek(0);
}


// ファイルを読み込む(Ret:読み込みSize) //
UINT CDirectShow::Read(BYTE *pData, UINT Size)
{
	HRESULT			hr;
	DWORD			write;
	UINT			temp;

	// -- ファイルのロードが行われていない場合 -- //
	if(NULL == m_pAMStream) return -1;


//////////////////////////////////////////

	// ループ終端までの長さを求める //
	temp = m_Current - m_End;
	if(temp < 0) return -1;
	if(temp == 0) return 0;

	// 読み込むサイズを確定する //
	Size = min(Size, temp);
	m_Current -= Size;
//////////////////////////////////////////

	hr = m_pAudioData->SetBuffer(Size, pData, 0);
	if(FAILED(hr)) PbgComError(hr, "SetBuffer");

	m_pSample->Update(0, NULL, NULL, 0);
	if(FAILED(hr)) PbgComError(hr, "Update");

	m_pAudioData->GetInfo(NULL, NULL, &write);
	return (DWORD)write;
	//return Size;
/*
	m_pAudioData->SetBuffer(Size, pData, 0);

	hr = m_pSample->Update(0, NULL, NULL, 0);
	if(MS_S_ENDOFSTREAM == hr){
		m_pMMStream->Seek(0);
		return Read(pData, Size);
	}
	else if(FAILED(hr)){
		m_pMMStream->SetState(STREAMSTATE_STOP);
		return 0;
	}

	m_pAudioData->GetInfo(NULL, NULL, &write);
	return write;
*/
}


// ファイル内の移動(データ部における) //
BOOL CDirectShow::Seek(UINT Offset)
{
	__int64		Pos;

	// -- ファイルのロードが行われていない場合 -- //
	if(NULL == m_pAMStream) return FALSE;

	Pos = (__int64(Offset) * 10000000) / m_DestFormat.nAvgBytesPerSec;
	if(FAILED(m_pMMStream->Seek(Pos))) return FALSE;

	// 現在位置を移動する //
	m_Current = m_Size - Offset;

	return TRUE;
}


// 終端をセットする //
BOOL CDirectShow::SetEnd(UINT Offset)
{

	UINT	temp;

	// -- ファイルのロードが行われていない場合 -- //
	if(NULL == m_pAMStream) return FALSE;

	temp = m_Size - Offset;
	if(temp < 0) return FALSE;

	m_End = temp;

	return TRUE;
}


// フォーマットを取得する //
WAVEFORMATEX *CDirectShow::GetFormat(void)
{
	// -- ファイルのロードが行われていない場合 -- //
	if(NULL == m_pAMStream) return NULL;

	return &m_DestFormat;
}


// バッファのサイズを取得 //
DWORD CDirectShow::GetSize(void)
{
	STREAM_TIME		Duration;
	__int64			Size;

	// -- ファイルのロードが行われていない場合 -- //
	if(NULL == m_pAMStream) return 0;

	if(FAILED(m_pMMStream->GetDuration(&Duration))) return 0;

	Size =(m_DestFormat.nSamplesPerSec
		 * m_DestFormat.nBlockAlign
		 * Duration)
		 / 1000 / 1000 / 10;

	if(m_DestFormat.nBlockAlign){
		Size += (m_DestFormat.nBlockAlign - 1);
		Size &= ~(m_DestFormat.nBlockAlign - 1);
	}

	return (DWORD)Size;
}


// MMStream の作成 //
FBOOL CDirectShow::CreateMMStream(void)
{
	HRESULT			hr;

	// -- まず、各インターフェースを解放する -- //
	Cleanup();


	hr = CoCreateInstance(	CLSID_AMMultiMediaStream	//
						,	NULL						//
						,	CLSCTX_INPROC_SERVER		//
						,	IID_IAMMultiMediaStream		//
						,	(void **)&m_pAMStream);		//
	if(FAILED(hr)){
		PbgComError(hr, "CoCreateInstance(IID_IAMMultiMediaStream) に失敗");

		Cleanup();
		return FALSE;
	}


	// -- IAMultiMediaStream の初期化 -- //
	hr = m_pAMStream->Initialize(STREAMTYPE_READ, 0, NULL);
	if(FAILED(hr)){
		PbgComError(hr, "IAMultiMediaStream::Initialize(READ) に失敗");
		Cleanup();
		return FALSE;
	}


	// -- MediaStream を追加 -- //
	hr = m_pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, 0, NULL);
	if(FAILED(hr)){
		PbgComError(hr, "IAMultiMediaStream::AddMediaStream() に失敗");
		Cleanup();
		return FALSE;
	}


	hr = m_pAMStream->QueryInterface(IID_IMultiMediaStream, (void **)&m_pMMStream);
	if(FAILED(hr)){
		PbgComError(hr, "IAMultiMediaStream::QueryInterface() に失敗");
		Cleanup();
		return FALSE;
	}


	// -- 作成完了 -- //
	return TRUE;
}


FVOID CDirectShow::Cleanup(void)
{
	if(m_pMMStream){
		m_pMMStream->SetState(STREAMSTATE_STOP);
	}

	// 各インターフェースを解放する //
	SAFE_RELEASE(m_pAMStream);		//
	SAFE_RELEASE(m_pMMStream);		//
	SAFE_RELEASE(m_pStream);		//
	SAFE_RELEASE(m_pSample);		//
	SAFE_RELEASE(m_pAudioStream);	//
	SAFE_RELEASE(m_pAudioData);		//
	SAFE_RELEASE(m_pGraphBuilder);	//
}



} // namespace PbgEx
