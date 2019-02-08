/*
 *   PbgSound.cpp   : WAVE 管理クラス
 *
 */

#include "PbgSound.h"
#include "DSTable.h"
#include "PbgError.h"
#include "PbgMem.h"
#include "..\DxUtil.h"



namespace Pbg {


// コンストラクタ //
CSound::CSound()
{
	HRESULT		hr;
	DWORD		i;

	m_lpDS            = NULL;		// DirectSound インターフェース
	m_lpPrim          = NULL;		// プライマリサウンドバッファ
	m_hWindow         = NULL;		// 対象となるウィンドウハンドル
	m_NumDriver       = 0;			// 列挙されたドライバ数
	m_CurrentDriverID = 0;			// 現在のドライバＩＤ(０はデフォルト)

	m_MasterVolume    = 255;		// マスターボリューム
	m_ppWaveMusic     = NULL;		// 曲のストリーム再生(１つだけ)

	// ドライバ情報構造体をゼロ初期化 //
	ZEROMEM(m_DrvInfo);

	// 所有者管理用ポインタの配列の初期化 //
	for(i=0; i<SOUND_BUFFER_SIZE; i++){
		m_ppSoundBuffer[i] = NULL;
	}

	// ドライバを列挙する //
	hr = DirectSoundEnumerate(EnumDrivers, (LPVOID)this);
	if(FAILED(hr)){
		// 列挙に失敗(見たこと無いけど...) //
		PbgError("\tCSound::CSound  DirectSoundEnumerate() に失敗");
	}
}


// デストラクタ //
CSound::~CSound()
{
	int			i;
	char		temp[1024];

	if(m_NumDriver){
		// 列挙したデバイスを表示します   //
		wsprintf(temp, "\tSound :  ドライバ %d つみっけ", m_NumDriver);
		PbgLog(temp);

		// 全てのドライバを見せる //
		for(i=0; i<m_NumDriver; i++){
			wsprintf(temp, "\t\t%d : %s (%s)\r\n\t\t\tGuid { %s }\r\n", i+1,
				m_DrvInfo[i].m_strDesc, m_DrvInfo[i].m_DrvName,
				m_DrvInfo[i].m_strGuid
			);

			// ログ吐き //
			PbgLog(temp);
		}
	}

	Cleanup();
}


// デフォルト(ID=0)のドライバで初期化 //
BOOL CSound::Initialize(HWND hWnd)
{
	// DirectSound で使用できるドライバが存在しない場合 //
	if(0 == m_NumDriver) return FALSE;

	// ウィンドウハンドルをセット //
	m_hWindow = hWnd;

	return ChangeDriver(0);
}


// サウンドに関係するオブジェクトの解放 //
void CSound::Cleanup(void)
{
	m_CurrentDriverID = 0;		// 一応、デフォルトのＩＤにしておく

	// セカンダリバッファを全て解放し、所有権も奪う //
	DeleteAllSoundBuffer();

	SAFE_RELEASE(m_lpPrim);		// プライマリサウンドバッファ
	SAFE_RELEASE(m_lpDS);		// DirectSound インターフェース
}


// 全ての音を停止する(所有権も奪う) //
void CSound::StopAllSoundBuffer(void)
{
	DWORD		i;

	for(i=0; i<SOUND_BUFFER_SIZE; i++){
		// 「サウンドバッファ」の所有者が存在するならば //
		if(NULL != m_ppSoundBuffer[i]){
			// そのサウンドバッファに対して停止要求を送る //
			(*m_ppSoundBuffer[i])->StopAll();
		}
	}
}


// マスターボリュームを変更する //
void CSound::SetMasterVolume(BYTE Volume)
{
	DWORD			i;

	// マスターボリュームを覚えておく //
	m_MasterVolume = Volume;

	// 曲のバッファには手を出さない当たりがポイントね  //
	// -> 効果音と曲では別のボリューム管理を行っている //
	for(i=0; i<SOUND_BUFFER_SIZE; i++){
		// バッファが存在するならば、ボリューム変更要求を送出 //
		// ポインタのポインタである事に注意                   //
		if(m_ppSoundBuffer[i] && (*(m_ppSoundBuffer[i]))){
			(*m_ppSoundBuffer[i])->OnMasterVolumeChanged();
		}
	}
}


// マスターボリュームを取得する //
BYTE CSound::GetMasterVolume(void)
{
	// 現在のマスターボリューム(効果音)を返す //
	return m_MasterVolume;
}


// WAVE ファイルからサウンドバッファを生成する //
BOOL CSound::CreateSoundBuffer(CSoundBuffer **ppBuffer)
{
	CSoundBuffer	*pTemp;
	DWORD			i;

	// まずは、ポインタを無効化しておく //
	*ppBuffer = NULL;

	// 初期化が出来ていない場合 //
	if(NULL == m_lpDS) return FALSE;

	// 空いている格納先を検索する //
	for(i=0; i<SOUND_BUFFER_SIZE; i++){
		if(NULL == m_ppSoundBuffer[i]) break;
	}

	// 空きが無い場合 //
	if(i >= SOUND_BUFFER_SIZE) return FALSE;

	// サウンドバッファを作成する //
	pTemp = NewEx(CSoundBuffer(m_lpDS, m_MasterVolume));
	if(NULL == pTemp) return FALSE;

	// 作成できたので、ポインタを接続 //
	m_ppSoundBuffer[i] = ppBuffer;		// 実際の格納先へのポインタ
	*ppBuffer          = pTemp;			// 実際のデータを格納

	return TRUE;
}


// 曲再生用のスタティックバッファを生成する //
BOOL CSound::CreateWaveMusicBuffer(CWaveMusic **ppMusic)
{
	CWaveMusic		*pTemp;

	if(NULL != m_ppWaveMusic) return FALSE;

	// ポインタを NULL で初期化 //
	*ppMusic = NULL;

	// 実際に作成する //
	pTemp = NewEx(CWaveMusic(m_lpDS));
	if(NULL == pTemp) return FALSE;			// 失敗

	m_ppWaveMusic = ppMusic;	// ポインタの格納される先を覚える
	*ppMusic      = pTemp;		// オブジェクトの実体を結びつける

	return TRUE;
}


// 作成したサウンドバッファを削除する <<全て削除 >> //
void CSound::DeleteAllSoundBuffer(void)
{
	DWORD			i;
	CSoundBuffer	*Target;
	CWaveMusic		*Music;

	for(i=0; i<SOUND_BUFFER_SIZE; i++){
		if(NULL != m_ppSoundBuffer[i]){		// 格納先のポインタが有効ならば
			Target = *(m_ppSoundBuffer[i]);		// 実際のオブジェクトを取得して
			DeleteEx(Target);					// 解放

			*m_ppSoundBuffer[i] = NULL;
			m_ppSoundBuffer[i] = NULL;
		}
	}

	// 曲バッファの破棄 //
	if(NULL != m_ppWaveMusic){		// 格納先が存在するならば
		Music = *(m_ppWaveMusic);	// ポインタ取得
		DeleteEx(Music);			// 解放

		*m_ppWaveMusic = NULL;		// オブジェクトへのポインタを無効化
		m_ppWaveMusic  = NULL;		// ポインタのポインタを無効化
	}
}


// ドライバを変更する //
BOOL CSound::ChangeDriver(DWORD DrvID)
{
	HRESULT			hr;			// 戻り値の格納先
	LPGUID			lpDSGuid;	// ドライバＧＵＩＤへのポインタ格納先
	DSBUFFERDESC	dsbd;		// サウンドバッファ記述子
	WAVEFORMATEX	wfx;		// プライマリバッファのフォーマット

	// まずは、初期化を行う //
	Cleanup();

	// 範囲外のドライバＩＤの指定 //
	if(DrvID >= m_NumDriver) return FALSE;

	// 対象となるドライバのＧＵＩＤへのポインタを取得 //
	lpDSGuid = &(m_DrvInfo[DrvID].m_Guid);

	// 初期化中に失敗したら、エラーメッセージを投げます //
	try{
		// DirectSound インターフェースを生成する //
		hr = DirectSoundCreate(lpDSGuid, &m_lpDS, NULL);
		if(FAILED(hr)) throw("DirectSoundCreate() に失敗");

		// 強調レベルをセットする(優先レベル)                         //
		// 参考：優先レベルにするのは、SetFormat() が呼び出せるように //
		hr = m_lpDS->SetCooperativeLevel(m_hWindow, DSSCL_PRIORITY);
		if(FAILED(hr)) throw("DSound->SetCooperativeLevel() に失敗");

		// 作成するサウンドバッファを記述する //
		ZEROMEM(dsbd);
		dsbd.dwSize        = sizeof(DSBUFFERDESC);	// 構造体のサイズ
		dsbd.dwBufferBytes = 0;						// プライマリだから０
		dsbd.lpwfxFormat   = NULL;					// プライマリだからNULL

		// プライマリバッファ＆ボリュームのコントロールをサポート //
		dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRLVOLUME;

		// プライマリサウンドバッファを取得する //
		hr = m_lpDS->CreateSoundBuffer(&dsbd, &m_lpPrim, NULL);
		if(FAILED(hr)) throw("DSound->CreateSoundBuffer() に失敗");

		ZEROMEM(wfx);
		wfx.wFormatTag     = WAVE_FORMAT_PCM;	// ＰＣＭ
		wfx.nChannels      = 2;					// ステレオ
		wfx.nSamplesPerSec = 44100;				// サンプリングレート
		wfx.wBitsPerSample = 16;				// １６ビット

		wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

		// プライマリサウンドバッファのフォーマットをセットする //
		hr = m_lpPrim->SetFormat(&wfx);
		if(FAILED(hr)) throw("Primary->SetFormat() に失敗");

		// マスターボリュームを最大値にセットする //
		SetMasterVolume(255);
	}
	catch(char *Msg){
		// エラーメッセージの一時格納用バッファ //
		char			MsgBuf[256];

		// エラーメッセージをエラー出力に渡す //
		strcpy(MsgBuf, "\tCSound::Initialize()  ");	// 関数名指定
		strcat(MsgBuf, Msg);						// エラー内容結合
		PbgError(MsgBuf);

		// オブジェクト解放 //
		Cleanup();

		return FALSE;	// 失敗したので FALSE を返す
	}

	// 初期化に成功したのでドライバＩＤを現在のものに変更しておく //
	m_CurrentDriverID = DrvID;

	return TRUE;	// 成功したので TRUE を返す
}


// DirectSound デバイスの数を取得する //
DWORD CSound::GetNumDevices(void)
{
	return m_NumDriver;
}


// DirectSound ドライバを列挙する //
BOOL CALLBACK CSound::EnumDrivers(
	GUID *pGuid, LPCSTR strDesc, LPCSTR strDrvName, VOID *pContext)
{
	CSound		*pThis;		// 静的メンバ関数なので、こんな形で this ポインタ
	SndDrvInfo	*pDrvInfo;	// ドライバ情報の書き込み用にポインタを

	pThis = (CSound *)pContext;

	// ドライバ数が列挙できる範囲を超えた場合、列挙を中止する    //
	// 参考：２０枚もサウンドカードを差すような環境は稀ですが... //
	if(pThis->m_NumDriver >= ENUM_SOUND_DRIVER) return FALSE;

	// 列挙された情報格納先をポインタで指定 //
	pDrvInfo = &(pThis->m_DrvInfo[pThis->m_NumDriver]);

	// 列挙されたドライバのＧＵＩＤ を格納する //
	if(pGuid) pDrvInfo->m_Guid = *pGuid;

	// ＧＵＩＤを文字列として格納する //
	GuidToString(&(pDrvInfo->m_Guid), pDrvInfo->m_strGuid);

	// デバイスのテキスト記述 //
	strncpy(pDrvInfo->m_strDesc, strDesc, SNDDEV_NAME_LENGTH-3);
	if(strlen(strDesc) > SNDDEV_NAME_LENGTH-3){
		// 長いから、以下は省略 //
		strcat(pDrvInfo->m_strDesc, "...");
	}

	// ドライバのモジュール名 //
	strncpy(pDrvInfo->m_DrvName, strDrvName, SNDDEV_NAME_LENGTH-3);
	if(strlen(strDrvName) > SNDDEV_NAME_LENGTH-3){
		// こちらも、長すぎるので、省略 //
		strcat(pDrvInfo->m_DrvName, "...");
	}

	// 列挙されたドライバ数をインクリメント //
	pThis->m_NumDriver += 1;

	return TRUE;	// さらにデバイス列挙を求める
}



} // namespace Pbg
