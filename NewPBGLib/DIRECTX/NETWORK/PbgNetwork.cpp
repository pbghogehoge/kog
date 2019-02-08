/*
 *   PbgNetwork.cpp   : 通信関連
 *
 */

#include "PbgNetwork.h"
#include "DPError.h"
#include "PbgError.h"
#include "PbgMem.h"



namespace Pbg {



// コンストラクタ //
CPbgNetwork::CPbgNetwork(const GUID &rGuidApp)
	: m_GuidApplication(rGuidApp)	// アプリケーションの GUID
{
	m_pDPlay   = NULL;		// DirectPlay      Interface
	m_pDPLobby = NULL;		// DirectPlayLobby Interface

	m_PlayerID = DPID_UNKNOWN;	// プレイヤーＩＤ

	m_hMsgEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_bConnect = FALSE;		// 接続していれば真

	m_bEnableTCPIP = FALSE;		// TCP/IP が使用できれば真
	m_bEnableModem = FALSE;		// モデム が使用できれば真
	m_NumSessions  = 0;			// セッションの数

	// インターフェースの初期化を行う //
	if(FALSE == InitializeInterfaces()){
		PbgLog("\tDirectPlay は使用できません");
	}

	// 使用できるサービスプロバイダを検出する //
	CheckConnections();

	// DirectPlay Interface を殺す //
	Close();
}


// デストラクタ //
CPbgNetwork::~CPbgNetwork()
{
//PbgError("Enter Close()");
	Close();		// DirectPlay インターフェースを破棄

	SAFE_RELEASE(m_pDPLobby);
	SAFE_RELEASE(m_pDPlay);

	CloseHandle(m_hMsgEvent);

	if(m_bEnableTCPIP) PbgLog("\tTCP/IP 使用可能");
//PbgError("Leave Close()");
//	if(m_bEnableModem) PbgLog("Modem 使用可能");
}


// セッションを作成 //
BOOL CPbgNetwork::CreateSessionTCPIP(char *pSessionName)
{
	DPSESSIONDESC2		dpsd;
	HRESULT				hr;

	// ＴＣＰ/ＩＰ接続の初期化を試みる //
	if(FALSE == InitializeTCPIP("", 0)){
		return FALSE;
	}

	ZEROMEM(dpsd);
	dpsd.dwSize           = sizeof(DPSESSIONDESC2);
	dpsd.dwFlags          = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
	dpsd.guidApplication  = m_GuidApplication;
	dpsd.lpszSessionNameA = pSessionName;
	dpsd.dwMaxPlayers     = 2;

	// -- 接続待ちへと移行する -- //
	// 注意：この場合の Open はブロッキング関数なので、必要に応じて //
	//       スレッドを適用する事！！                               //
	hr = m_pDPlay->Open(&dpsd, DPOPEN_CREATE | DPOPEN_RETURNSTATUS);
	if(FAILED(hr)){
		Close();
		return FALSE;
	}

	if(!CreatePlayer()){
		Close();
		return FALSE;
	}

	return TRUE;
}


// セッションに参加 //
BOOL CPbgNetwork::JoinSessionTCPIP(DWORD SessionID)
{
	DPSESSIONDESC2		Desc;
	HRESULT				hr;

	// 存在しないセッションへのアクセス抑制 //
	if(SessionID >= m_NumSessions){
		Close();
		return FALSE;
	}
/*
	// ＴＣＰ/ＩＰ接続の初期化を試みる //
	if(FALSE == InitializeTCPIP("", 0)){
		MessageBox(NULL, "TCPIP Initialize Failed.", "", MB_OK);
		return FALSE;
	}
*/
	ZEROMEM(Desc);
	Desc.dwSize          = sizeof(DPSESSIONDESC2);
	Desc.guidApplication = m_GuidApplication;
	Desc.guidInstance    = m_SessionDesc[SessionID].guidInstance;
	Desc.dwMaxPlayers    = 2;

	hr = m_pDPlay->Open(&Desc, DPOPEN_JOIN | DPOPEN_RETURNSTATUS);
	if(FAILED(hr)){
		DPlayError(hr);
		Close();
		return FALSE;
	}

	if(!CreatePlayer()){
		Close();
		return FALSE;
	}

	// 接続完了 //
	m_bConnect = TRUE;

	return TRUE;
}


// 接続を閉じる //
void CPbgNetwork::Close(void)
{
//OutputDebugString("000\n");
	// ここでセッション列挙スレッドの停止を待つ //
	DisableThread();
//OutputDebugString("001\n");
	while(IsActiveThread());
//OutputDebugString("002\n");

	m_bConnect = FALSE;
//OutputDebugString("003\n");

	// インターフェースが生きていれば、閉じる //
	if(m_pDPlay){
//OutputDebugString("004\n");
		// プレイヤーを削除する //
		if(DPID_UNKNOWN != m_PlayerID){
			m_pDPlay->DestroyPlayer(m_PlayerID);
//OutputDebugString("005\n");
			m_PlayerID = DPID_UNKNOWN;
		}

		m_pDPlay->Close();
//OutputDebugString("006\n");
		SAFE_RELEASE(m_pDPlay);
//OutputDebugString("007\n");
	}
}


// セッションの列挙を開始する //
BOOL CPbgNetwork::BeginEnumSessions(void)
{
	// スレッドを止めて、接続を切る //
	Close();

	// セッションの数を初期化 //
	m_NumSessions = 0;

	// 初期化に失敗した場合 //
	if(FALSE == InitializeTCPIP("", 0)){
		return FALSE;
	}

	return CreateThread();
}


// セッションの列挙が停止していれば真 //
BOOL CPbgNetwork::IsEnumSessionsStopped(void)
{
	if(IsActiveThread()) return FALSE;
	else                 return TRUE;
}


// セッションの数を取得 //
DWORD CPbgNetwork::GetNumSessions(void)
{
	if(IsActiveThread()) return 0;
	else                 return m_NumSessions;
}


// セッション名を取得 //
BOOL CPbgNetwork::GetSessionName(char *pName, DWORD SessionID)
{
	if(SessionID >= m_NumSessions) return FALSE;

	strcpy(pName, m_SessionName[SessionID]);
	return TRUE;
}


// データを 送信 する //
BOOL CPbgNetwork::SendData(VOID *pData, DWORD DataSize)
{
	HRESULT		hr;

	if(NULL == m_pDPlay) return FALSE;

	hr = m_pDPlay->Send(
					m_PlayerID,
					DPID_ALLPLAYERS,
					DPSEND_GUARANTEED,
					pData,
					DataSize);

	if(FAILED(hr)) return FALSE;
	else           return TRUE;
}


// データを 受信 する //
BOOL CPbgNetwork::RecvData(VOID *pData, DWORD MaxSize)
{
	HRESULT				hr;
	DPID				from, to;
	LPDPMSG_GENERIC		lpSys;


	if(NULL == m_pDPlay) return FALSE;

	while(1){
		hr = m_pDPlay->Receive(
							&from,
							&to,
							DPRECEIVE_ALL,
							pData,
							&MaxSize);
		if(FAILED(hr)) return FALSE;

		if(DPID_SYSMSG == from){
			lpSys = (DPMSG_GENERIC *)pData;

			switch(lpSys->dwType){
			case DPSYS_CREATEPLAYERORGROUP:
				m_bConnect = TRUE;

//				MessageBox(NULL, "CONNECT", "", MB_OK);
			break;

			case DPSYS_HOST:
			case DPSYS_SESSIONLOST:
			case DPSYS_DESTROYPLAYERORGROUP:
//				MessageBox(NULL, "DISCONNECT", "", MB_OK);
				Close();
			return FALSE;	// ここで戻らないと危険！！
			}
		}
		else{
			return TRUE;
		}
	}
}


// 開いていれば真を返す //
FBOOL CPbgNetwork::IsOpen(void)
{
	return m_bConnect;
}


// セッションが作成されていれば真を返す //
FBOOL CPbgNetwork::IsSessionCreated(void)
{
	if(DPID_UNKNOWN != m_PlayerID) return TRUE;
	else                           return FALSE;
}


// セッション探索用スレッド //
void CPbgNetwork::ThreadFunction(void)
{
	HRESULT				hr;
	DPSESSIONDESC2		Desc;
	DWORD				start;

	if(NULL == m_pDPlay){
		return;
	}

	ZEROMEM(Desc);
	Desc.dwSize          = sizeof(DPSESSIONDESC2);
	Desc.guidApplication = m_GuidApplication;

	start = timeGetTime();

	do{
		hr = m_pDPlay->EnumSessions(	&Desc
									,	0
									,	EnumSessionsCallback
									,	this
									,	DPENUMSESSIONS_AVAILABLE
									|	DPENUMSESSIONS_ASYNC
									//|	DPENUMSESSIONS_STOPASYNC
									|	DPENUMSESSIONS_RETURNSTATUS);

		if(timeGetTime() - start > 1000) break;	// タイムアウト

		m_NumSessions = 0;
	}
	while(/*(hr == DPERR_CONNECTING) &&*/ IsEnableThread());

//	m_pDPlay->Close();
//	SAFE_RELEASE(m_pDPlay);
}


// プレイヤーを作成する //
FBOOL CPbgNetwork::CreatePlayer(void)
{
	HRESULT		hr;

	if(NULL == m_pDPlay) return FALSE;

	hr = m_pDPlay->CreatePlayer(
							&m_PlayerID,
							NULL,
							m_hMsgEvent,
							NULL,
							0,
							0);

	if(FAILED(hr)){
		m_PlayerID = DPID_UNKNOWN;
		return FALSE;
	}

	return TRUE;
}


// 使用できる接続方法を調べ上げる //
FVOID CPbgNetwork::CheckConnections(void)
{
	// インターフェースの初期化が行われていない場合 //
	if(FALSE == m_pDPlay) return;

	// サービスプロバイダを列挙してみる //
	m_pDPlay->EnumConnections(NULL, EnumConnectionsCallback, this, 0);
}


// インターフェースを初期化する //
FBOOL CPbgNetwork::InitializeInterfaces(void)
{
	HRESULT			hr;

	if(NULL == m_pDPlay){
		hr = CoCreateInstance(	CLSID_DirectPlay
							,	NULL
							,	CLSCTX_INPROC_SERVER
							,	IID_IDirectPlay4A
							,	(VOID**)&m_pDPlay);
		if(FAILED(hr)){
			PbgLog("CoCreateInstance(IID_IDirectPlay4A) Failed.");
			return FALSE;
		}
	}

	// DirectPlayLobby オブジェクトを作成する //
	if(NULL == m_pDPLobby){
		hr = CoCreateInstance(	CLSID_DirectPlayLobby
							,	NULL
							,	CLSCTX_INPROC_SERVER
							,	IID_IDirectPlayLobby3A
							,	(VOID **)&m_pDPLobby);
		if(FAILED(hr)){
			PbgLog("CoCreateInstance(IID_IDirectPlayLObby3A) Failed.");
			SAFE_RELEASE(m_pDPlay);
			return FALSE;
		}
	}

	// 正常終了 //
	return TRUE;
}


// ＴＣＰ/ＩＰでの初期化を行う                        //
// 成功すると DirectPlay インターフェースが生成される //
FBOOL CPbgNetwork::InitializeTCPIP(char *pIPAddr, int nPort)
{
	VOID				*pAddr;
	HRESULT				hr;

	// すでに接続されている場合に備えて、まずは閉じる //
	Close();

	// どうやら TCP/IP 接続が行えない //
	if(FALSE == m_bEnableTCPIP){
		return FALSE;
	}

	// -- インターフェースを作成する -- //
	if(FALSE == InitializeInterfaces()){
		return FALSE;
	}

	// TCP/IP アドレスから DirectPlay アドレスを作成する //
	pAddr = CreateTCPIPAddr(pIPAddr, nPort);
	if(NULL == pAddr){
		Close();
		return FALSE;
	}

	// 接続を初期化する //
	hr = m_pDPlay->InitializeConnection(pAddr, 0);

	// 失敗かどうかの判定の前に、メモリは解放しておく //
	MemFree(pAddr);

	// 接続に失敗している場合 //
	if(FAILED(hr)){
		DPlayError(hr);
		Close();
		return FALSE;
	}

	return TRUE;
}


// IP アドレスとポート番号から DirectPlay アドレスを作成する //
// 得られたポインタは MemFree() で解放する必要がある         //
VOID *CPbgNetwork::CreateTCPIPAddr(char *pIPAddr, int nPort)
{
	DPCOMPOUNDADDRESSELEMENT		Elem[3];
	HRESULT							hr;
	WORD							wPort;
	DWORD							AddrSize;
	VOID							*pAddr;
	int								NumElements;

	if(NULL == m_pDPLobby) return NULL;

	Elem[0].guidDataType = DPAID_ServiceProvider;
	Elem[0].dwDataSize   = sizeof(GUID);
	Elem[0].lpData       = (VOID *)&DPSPGUID_TCPIP;

	Elem[1].guidDataType = DPAID_INet;
	Elem[1].dwDataSize   = strlen(pIPAddr) + 1;
	Elem[1].lpData       = pIPAddr;

	if(nPort > 0){
		wPort = (WORD)nPort;

		Elem[2].guidDataType = DPAID_INetPort;
		Elem[2].dwDataSize   = sizeof(WORD);
		Elem[2].lpData       = &wPort;

		NumElements = 3;
	}
	else{
		NumElements = 2;
	}

	// アドレスの格納に必要なメモリを求める //
	hr = m_pDPLobby->CreateCompoundAddress(Elem, NumElements, NULL, &AddrSize);
	if(DPERR_BUFFERTOOSMALL != hr){
		PbgComError(hr, "CreateCompoundAddress() Failed.");
		return NULL;
	}

	// 実際にメモリを確保する //
	pAddr = MemAlloc(AddrSize);
	if(NULL == pAddr) return NULL;

	// アドレスを作成 //
	hr = m_pDPLobby->CreateCompoundAddress(Elem, NumElements, pAddr, &AddrSize);
	if(FAILED(hr)){
		PbgComError(hr, "CreateCompoundAddress() Failed.");
		return NULL;
	}

	return pAddr;
}


// 接続方法の列挙の為に使用するコールバック関数     //
// このクラスでサポートする接続方法に対応していれば //
// それに対するフラグを真にする                     //
BOOL FAR PASCAL CPbgNetwork::EnumConnectionsCallback(
			const GUID		*pSPGUID			// サービスプロバイダのGUID
		,	VOID			*pConnection		// DirectPlayアドレス格納先
		,	DWORD			dwConnectionSize	// DirectPlayアドレスのサイズ
		,	const DPNAME	*pName				// サービスプロバイダ名
		,	DWORD			dwFlags				// 接続のタイプ
		,	VOID			*pContext)			// アプリケーション定義
{
	CPbgNetwork		*pThis;

	pThis = (CPbgNetwork *)pContext;

	if(DPSPGUID_TCPIP == *pSPGUID){
		pThis->m_bEnableTCPIP = TRUE;
	}
	else if(DPSPGUID_MODEM == *pSPGUID){
		pThis->m_bEnableModem = TRUE;
	}

	return TRUE;
}


BOOL FAR PASCAL CPbgNetwork::EnumSessionsCallback(
			const DPSESSIONDESC2	*pSessionDesc	// セッション情報
		,	DWORD					*pdwTimeOut		// タイムアウト値
		,	DWORD					dwFlags			// セッションのフラグ
		,	VOID					*pContext)		// アプリケーション定義
{
	CPbgNetwork		*pThis;

	pThis = (CPbgNetwork *)pContext;

	if(pThis->m_NumSessions >= ENUMSESSIONS_MAX) return FALSE;
    if(dwFlags & DPESC_TIMEDOUT) return FALSE;

	// このセッションには参加できない //
	if(pSessionDesc->dwCurrentPlayers >= pSessionDesc->dwMaxPlayers) return TRUE;

	strcpy(pThis->m_SessionName[pThis->m_NumSessions], pSessionDesc->lpszSessionNameA);
	pThis->m_SessionDesc[pThis->m_NumSessions] = *pSessionDesc;

	pThis->m_NumSessions++;

	return TRUE;
}



} // namespace Pbg
