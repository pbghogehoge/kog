/*
 *   CInputCtrl.h   : 入力管理クラス
 *
 */

#ifndef CINPUTCTRL_INCLUDED
#define CINPUTCTRL_INCLUDED "入力管理クラス : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.02 : 2002/01/24 : TCP/IP 入力(評価版)を追加
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "PbgType.h"
#include "CInputDevBase.h"
#include "CKeyInput.h"
#include "CPadInput.h"
#include "CTCPIPInput.h"
#include "CKeyCfgWindow.h"
#include "CPadCfgWindow.h"



/***** [ 定数 ] *****/

// 入力モード定数 //
typedef enum tagINPUT_MODE {
	IMODE_1P         = 0x00,	// １Ｐモード(FullKey or 1P-Pad)
	IMODE_2PDEFAULT  = 0x01,	// ２Ｐモード標準(HalfKey VS HalfKey : 1P-Pad VS 2P-Pad)
	IMODE_KEYPAD     = 0x02,	// ２Ｐモード(FullKey VS 1P-Pad)
	IMODE_PADKEY     = 0x03,	// ２Ｐモード(1P-Pad VS FullKey)

	IMODE_TCPIP = 0x04,		// TCP/IP 接続モード
} INPUT_MODE;

typedef enum tagCFG_WINDOW {
	CFGW_DISABLE = 0x00,	// 無効状態
	CFGW_FULLKEY = 0x01,	// フルキーボード
	CFGW_HALFKEY = 0x02,	// キーボード１＆２
	CFGW_PAD     = 0x03,	// パッド１＆２
} CFG_WINDOW;



/***** [クラス定義] *****/

// 入力管理クラス //
class CInputCtrl {
public:
	void UpdateBuffer(void);					// 各データを更新する

	FWORD Get1PInput(void){			// １Ｐ側の入力を取得
		return m_1PInput;
	};

	FWORD Get2PInput(void){			// ２Ｐ側の入力を取得
		return m_2PInput;
	};

	FWORD GetSysKeyInput(void){		// システム側の入力を取得
		return m_SysInput;
	};

	BOOL SelectInputDevice(INPUT_MODE Mode);	// 入力デバイスを選択

	BOOL SelectConfigWindow(CFG_WINDOW CfgWnd);	// コンフィグ窓を選択する
	BOOL IsActiveConfigWindow(void);			// コンフィグ窓がアクティブなら真
	BOOL IsPadInput(void);						// パッド入力があれば真
	void MoveConfigWindow(void);				// コンフィグ窓を動作させる
	void DrawConfigWindow(void);				// コンフィグ窓を描画する

	void InitSetting(void);				// 設定を初期化する
	void SetData(InpCfgData *pData);	// データを書き込む
	void GetData(InpCfgData *pData);	// データを取得する

	CInputCtrl();		// コンストラクタ
	~CInputCtrl();		// デストラクタ


private:
	WORD __Get1PInput(void);				// １Ｐ側の入力を取得
	WORD __Get2PInput(void);				// ２Ｐ側の入力を取得
	WORD __GetSysKeyInput(void);			// システム側の入力を取得

	CKeyInput		m_FullKeyboard;			// フルキーボード配置
	CKeyInput		m_HalfKeyboard_1P;		// ハーフキーボード配置(１Ｐ側)
	CKeyInput		m_HalfKeyboard_2P;		// ハーフキーボード配置(２Ｐ側)

	CPadInput		m_Pad_1P;				// パッド(１Ｐ側)
	CPadInput		m_Pad_2P;				// パッド(２Ｐ側)

	CTCPIPInput		m_TCPIPInput;			// ＴＣＰ/ＩＰ

	CKeyCfgWindow		m_FullKeyWindow;	// フルキーボードコンフィグ窓
	CKeyCfgWindow		m_Key1Window;		// キーボード(１Ｐ)コンフィグ窓
	CKeyCfgWindow		m_Key2Window;		// キーボード(２Ｐ)コンフィグ窓
	CPadCfgWindow		m_Pad1Window;		// パッド(１Ｐ)コンフィグ窓
	CPadCfgWindow		m_Pad2Window;		// パッド(２Ｐ)コンフィグ窓
	CInputCfgWindow		*m_pWindow[2];		// コンフィグ窓

	WORD			m_1PInput;		// １Ｐ側の入力
	WORD			m_2PInput;		// ２Ｐ側の入力
	WORD			m_SysInput;		// システム入力
	BOOL			m_bIsPadInput;	// パッドからの入力があれば真

	INPUT_MODE		m_CurrentMode;			// 現在の入力モード
	BYTE			m_KeyBuffer[256];		// キーバッファ
};



#endif
