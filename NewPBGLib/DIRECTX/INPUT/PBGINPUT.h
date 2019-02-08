/*
 *   PbgInput.h   : 入力デバイス管理
 *
 */

#ifndef PBGINPUT_INCLUDED
#define PBGINPUT_INCLUDED "入力デバイス管理 : Version 0.03 : Update 2001/09/21"

/*  [更新履歴]
 *    Version 0.03 : 2001/09/21 : ロストした時の処理が抜けていたので追加
 *    Version 0.02 : 2001/02/09 : 仕様を少々変更する(DirectInput 専用クラスへと)
 *                              : ぷるぷる非対応版の完成
 *
 *    Version 0.01 : 2001/02/05 : 大まかなクラス設計
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
#define ENUM_INPUT_DEVICE	5		// パッドの列挙数
#define JOY_RANGE			1000	// パッドの範囲指定



/***** [クラス定義] *****/
/*
// マウス情報格納用構造体 //
typedef struct tagPBG_MOUSESTATE {
	int			x;			// Ｘ座標(絶対)
	int			y;			// Ｙ座標(絶対)
	BYTE		Right;		// 右ボタンが押されていれば非ゼロ
	BYTE		Left;		// 左ボタンが押されていれば非ゼロ
} PBG_MOUSESTATE;
*/


namespace Pbg {



/***** [クラス定義] *****/

// パッド列挙情報・格納用構造体 //
typedef struct tagJoyDrvEnumInfo {
	GUID	m_Guid;					// ドライバＧＵＩＤ
	char	m_strGuid[32+4+1];		// ＧＵＩＤをＣ文字列にしたもの

	char	m_strInst[MAX_PATH];		// インスタンスの通称名
//	char	m_strProduct[MAX_PATH];		// 製品の名前??

	BOOL	m_bEnableFF;	// フォースフィードバックに対応しているか
} JoyDrvEnumInfo;


// パッドの状態格納用構造体 //
typedef struct tagPadState {
	JoyDrvEnumInfo			*m_pInfo;		// デバイス情報へのポインタ
	LPDIRECTINPUTDEVICE7	m_lpJoy;		// 入力デバイス
	LPDIRECTINPUTEFFECT		m_lpEffect;		// エフェクトオブジェクト
} JoyState;


// 入力デバイス管理クラス //
class CInput {
public:
	BOOL GetKeyboardInput(BYTE *pKeyBuffer);				// キーボードの状態を取得
//	BOOL GetMouseInput(PBG_MOUSESTATE *pMouseState);		// マウスの状態を取得
	BOOL GetJoyInput(DIJOYSTATE *pJoyInfo, BYTE JoyID);		// パッドの状態を取得

//	void SetMouseRange(RECT *rc);			// マウスの移動範囲を変更する
//	void SetMousePos(int x, int y);			// マウスの現在位置を変更する

	BOOL Initialize(HWND hWnd);		// インターフェース初期化
	void Cleanup();					// インターフェース解放

	// DirectInput を有効にするかどうか(TRUE : 使用する) //
//	BOOL EnableDirectInput(BOOL IsUseDInput);

	DWORD GetNumJoyDriver(void);						// ドライバ数を取得
	BOOL  ChangeJoyDriver(BYTE JoyID, DWORD DeviceID);	// ドライバを変更
	BOOL  GetJoyDriverName(char *pName, DWORD JoyID);	// ドライバ名を取得

	CInput();		// コンストラクタ
	~CInput();		// デストラクタ

private:
	BOOL InitSysKeyboard(HWND hWnd);				// キーボードの初期化

	// ジョイスティックの初期化 //
	BOOL InitJoystick(HWND hWnd, BYTE JoyID, DWORD DeviceID);

	// ジョイスティックの解放 //
	void ReleaseJoystick(BYTE JoyID);

//	BOOL InitSysMouse(HWND hWnd);		// システムマウスの初期化

	// パッド系のデバイスを列挙する為のコールバック関数 //
	static BOOL CALLBACK
		EnumJoyDevices(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	LPDIRECTINPUT7			m_lpDI;				// DirectInput インターフェース
	LPDIRECTINPUTDEVICE7	m_lpSysKeyboard;	// システムキーボード(Default)
//	LPDIRECTINPUTDEVICE7	m_lpSysMouse;		// システムマウス(Default)
	JoyState				m_JoyState[2];		// 選択されているパッドの情報

//	BOOL		m_bEnableDI;			// DirectInput が有効かどうか
	HWND		m_hWindow;				// ウィンドウハンドル
	DWORD		m_NumJoyDevice;			// 列挙されたパッド系デバイスの数

//	RECT		m_rcMouse;		// マウスの移動可能領域
//	int			m_MouseX;		// マウスのＸ座標
//	int			m_MouseY;		// マウスのＹ座標

	// デバイス情報の格納先 //
	JoyDrvEnumInfo	m_JoyInfo[ENUM_INPUT_DEVICE];
};



} // namespace Pbg



#endif
