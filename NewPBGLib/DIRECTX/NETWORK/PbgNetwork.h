/*
 *   PbgNetwork.h   : 通信関連
 *
 */

#ifndef PBGNETWORK_INCLUDED
#define PBGNETWORK_INCLUDED "通信関連     : Version 0.01 : Update 2001/09/15"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/15 : 制作開始
 */



#include "PbgType.h"
#include "CThread.h"



/***** [ 定数 ] *****/
#define ENUMSESSIONS_MAX		20		// セッションの列挙最大数



namespace Pbg {



/***** [クラス定義] *****/

// 通信関連 //
class CPbgNetwork : public CThread {
public:
	FBOOL CreateSessionTCPIP(char *pSessionName);	// セッションを作成
	FBOOL JoinSessionTCPIP(DWORD SessionID);		// セッションに参加
	FVOID Close(void);								// 接続を閉じる

	FBOOL  BeginEnumSessions(void);		// セッションの列挙を開始する
	FBOOL  IsEnumSessionsStopped(void);	// セッションの列挙が停止していれば真
	FDWORD GetNumSessions(void);							// セッションの数を取得
	FBOOL  GetSessionName(char *pName, DWORD SessionID);	// セッション名を取得

	// データを 送信/受信 する //
	FBOOL SendData(VOID *pData, DWORD DataSize);	// 送信
	FBOOL RecvData(VOID *pData, DWORD MaxSize);		// 受信

	FBOOL IsOpen(void);				// 開いていれば真を返す
	FBOOL IsSessionCreated(void);	// セッションが作成されていれば真を返す

	CPbgNetwork(const GUID &rGuidApp);	// コンストラクタ
	~CPbgNetwork();						// デストラクタ


private:
	// セッション探索用スレッド //
	virtual void ThreadFunction(void);

	// プレイヤーを作成する //
	FBOOL CreatePlayer(void);

	// 使用できる接続方法を調べ上げる //
	FVOID CheckConnections(void);

	// インターフェースを初期化する //
	FBOOL InitializeInterfaces(void);

	// ＴＣＰ/ＩＰでの初期化を行う                        //
	// 成功すると DirectPlay インターフェースが生成される //
	FBOOL InitializeTCPIP(char *pIPAddr, int nPort);

	// IP アドレスとポート番号から DirectPlay アドレスを作成する //
	// 得られたポインタは MemFree() で解放する必要がある         //
	VOID *CreateTCPIPAddr(char *pIPAddr, int nPort);


	// 接続方法の列挙の為に使用するコールバック関数     //
	// このクラスでサポートする接続方法に対応していれば //
	// それに対するフラグを真にする                     //
	static BOOL FAR PASCAL EnumConnectionsCallback(
				const GUID		*pSPGUID			// サービスプロバイダのGUID
			,	VOID			*pConnection		// DirectPlayアドレス格納先
			,	DWORD			dwConnectionSize	// DirectPlayアドレスのサイズ
			,	const DPNAME	*pName				// サービスプロバイダ名
			,	DWORD			dwFlags				// 接続のタイプ
			,	VOID			*pContext);			// アプリケーション定義

	static BOOL FAR PASCAL EnumSessionsCallback(
				const DPSESSIONDESC2	*pSessionDesc	// セッション情報
			,	DWORD					*pdwTimeOut		// タイムアウト値
			,	DWORD					dwFlags			// セッションのフラグ
			,	VOID					*pContext);		// アプリケーション定義


private:
	LPDIRECTPLAY4A			m_pDPlay;		// DirectPlay      Interface
	LPDIRECTPLAYLOBBY3A		m_pDPLobby;		// DirectPlayLobby Interface

	DPID					m_PlayerID;		// このプレイヤーのＩＤ
	HANDLE					m_hMsgEvent;	// 受信用イベント

	DPSESSIONDESC2		m_SessionDesc[ENUMSESSIONS_MAX];	// セッションの情報
	char				m_SessionName[ENUMSESSIONS_MAX][256];	// セッション名
	DWORD				m_NumSessions;						// 現在のセッションの数

	const GUID			m_GuidApplication;	// このアプリケーションの GUID

	BOOL	m_bConnect;			// 接続していれば真

	BOOL	m_bEnableTCPIP;		// TCP/IP が使用できれば真
	BOOL	m_bEnableModem;		// モデム が使用できれば真
};



}	// namespace Pbg



#endif
