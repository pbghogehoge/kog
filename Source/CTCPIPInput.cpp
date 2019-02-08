/*
 *   CTCPIPInput.cpp   : TCP/IP 入力
 *
 */

#include "CTCPIPInput.h"
#include "KOG_Guid.h"
#include "PbgError.h"
#include "VKeyCode.h"



//------------------------------------------------------------------------
//   名称  | CTCPIPInput::CTCPIPInput()
//   説明  | コンストラクタ
//------------------------------------------------------------------------
CTCPIPInput::CTCPIPInput()
	: m_DPlay(guidKIOH)
{
//	ReConnect();	// 遅延初期化とする
}


//------------------------------------------------------------------------
//   名称  | CTCPIPInput::~CTCPIPInput()
//   説明  | デストラクタ
//------------------------------------------------------------------------
CTCPIPInput::~CTCPIPInput()
{
	m_DPlay.Close();
}


//------------------------------------------------------------------------
//   名称  | CTCPIPInput::GetCode()
//   説明  | 入力コードを取得する
//  戻り値 | キーコードを返す
//------------------------------------------------------------------------
WORD CTCPIPInput::GetCode(void)
{
	BYTE		readbuf[400];
	KIOH_DATA	*p  = (KIOH_DATA *)readbuf;
	WORD		key = 0;
	int			timeout;
	BOOL		bRecv;

	if(FALSE == m_DPlay.IsSessionCreated()){
		ReConnect();
		return 0;
	}

	m_DPlay.RecvData(readbuf, 400);
	if(FALSE == m_DPlay.IsOpen()) return FALSE;

	// 同期メッセージを送る //
	p->m_DataType = KIOHDATA_SYNC;
	p->m_KeyData  = 0;
	m_DPlay.SendData(p, sizeof(KIOH_DATA));

	timeout = 1000;
	bRecv   = FALSE;
	while(FALSE == bRecv){
		while(m_DPlay.RecvData(readbuf, 400)){
			key |= p->m_KeyData;
			bRecv = TRUE;
		}

		timeout = timeout - 1;
		if(timeout <= 0){
			PbgLog("RecvData() TimeOut...");
			return 0;
		}

		Sleep(1);
	}

	return key;
}


//------------------------------------------------------------------------
//   名称  | CTCPIPInput::OnChangeSetting()
//   説明  | 指定されたキーに対して割り当てを行う
//  戻り値 | この実装では、常に TRUE を返します
//------------------------------------------------------------------------
BOOL CTCPIPInput::OnChangeSetting(BYTE TargetID)
{
	return TRUE;
}


//------------------------------------------------------------------------
//   名称  | CTCPIPInput::GetButtonName()
//   説明  | ある機能に割り当てられたボタンorキーの名称を返す
//------------------------------------------------------------------------
FVOID CTCPIPInput::GetButtonName(char *pBuf, BYTE TargetID)
{
}


//------------------------------------------------------------------------
//   名称  | CTCPIPInput::OnResetSetting()
//   説明  | 設定を初期状態にリセットする
//------------------------------------------------------------------------
void CTCPIPInput::OnResetSetting(void)
{
}


//------------------------------------------------------------------------
//   名称  | CTCPIPInput::ReConnect()
//   説明  | 再接続待ちに移行する
//------------------------------------------------------------------------
void CTCPIPInput::ReConnect(void)
{
	TCHAR		name[1024] = "???";
	char		buf[1024]  = "稀翁玉＠";
	DWORD		size       = sizeof(TCHAR) * 1024;

	GetComputerName(name, &size);
	strcat(buf, (char *)name);

	if(m_DPlay.CreateSessionTCPIP(buf)){
		PbgLog("セッションの作成完了");
	}
}
