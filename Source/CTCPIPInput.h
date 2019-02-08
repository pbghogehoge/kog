/*
 *   CTCPIPInput.h   : TCP/IP 入力
 *
 */

#ifndef CTCPIPINPUT_INCLUDED
#define CTCPIPINPUT_INCLUDED "TCP/IP な入力 : Version 0.01 : Update 2002/02/22"

/*  [更新履歴]
 *    Version 0.01 : 2002/02/22 : 制作開始
 */



#include "CInputDevBase.h"
#include "DIRECTX\NETWORK\PbgNetwork.h"



/***** [クラス定義] *****/

// キーボード入力管理クラス //
class CTCPIPInput : public CInputDevBase {
public:
	// 入力コードを取得する //
	WORD GetCode(void);

	// 指定されたキーに対して割り当てを行う //
	BOOL OnChangeSetting(BYTE TargetID);

	// ある機能に割り当てられたボタンorキーの名称を返す //
	FVOID GetButtonName(char *pBuf, BYTE TargetID);

	// 設定を初期状態にリセットする //
	void OnResetSetting(void);

	CTCPIPInput();		// コンストラクタ
	~CTCPIPInput();		// デストラクタ


private:
	void ReConnect(void);	// 再接続待ちに移行する

	Pbg::CPbgNetwork		m_DPlay;
	DWORD					m_RetryWait;
};



#endif
