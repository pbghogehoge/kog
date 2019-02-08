/*
 *   PbgInput.cpp   : 入力デバイス管理
 *
 */

#include "PbgInput.h"
#include "DirectX\\DxUtil.h"
#include "PbgError.h"



namespace Pbg {



// コンストラクタ
CInput::CInput()
{
//	m_bEnableDI     = FALSE;	// DirectInput が有効かどうか
	m_hWindow       = NULL;		// ウィンドウハンドル
	m_lpDI          = NULL;		// DirectInput インターフェース
	m_lpSysKeyboard = NULL;		// システム・キーボード
//	m_lpSysMouse    = NULL;		// システム・マウス
	m_NumJoyDevice  = 0;		// パッドの列挙数

	ZEROMEM(m_JoyInfo);		// パッド列挙情報
	ZEROMEM(m_JoyState);	// パッドの状態

	// マウスの移動可能領域 //
//	SetRect(&m_rcMouse, 0, 0, 639, 479);

//	m_MouseX = 320;		// マウスの初期Ｘ座標
//	m_MouseY = 240;		// マウスの初期Ｙ座標
}


// デストラクタ
CInput::~CInput()
{
	char			buf[MAX_PATH * 2];
	DWORD			i;

	if(m_NumJoyDevice){
		wsprintf(buf,
				"\tInput : ドライバ %d 個みっけ",
				m_NumJoyDevice
		);
		PbgLog(buf);

		for(i=0; i<m_NumJoyDevice; i++){
			wsprintf(buf,
					"\t\t%d : インスタンス通称名   \"%s\"\r\n\t\t\tGuid { %s }\r\n",
					i+1,
					m_JoyInfo[i].m_strInst,
					m_JoyInfo[i].m_strGuid
			);
			PbgLog(buf);
		}
	}

	Cleanup();
}

// キーボードの状態を取得 //
BOOL CInput::GetKeyboardInput(BYTE *pKeyBuffer)
{
	HRESULT		hr;

/*
	// DirectInput が無効な場合 //
	if(FALSE == m_bEnableDI){
		// 仮(このままでは、ちょっとマズい) //
		GetKeyboardState(pKeyBuffer);
		return TRUE;
	}
*/

	// 致命的なエラー(内部エラー) //
	if(NULL == m_lpSysKeyboard) return FALSE;

	// キーボードの状態を取得する //
	while(1){
		hr = m_lpSysKeyboard->GetDeviceState(256, pKeyBuffer);

		// ロストしている場合 //
		if(DIERR_INPUTLOST == hr){
			// デバイスのアクセス権を取得する //
			hr = m_lpSysKeyboard->Acquire();
			if(FAILED(hr)) return FALSE;
		}
		// ロスト以外のエラーならば、偽を返す //
		else if(FAILED(hr)){
			return FALSE;
		}
		// 成功したので、真を返す //
		else{
			return TRUE;
		}
	}

//	if(FAILED(hr)){
//		return FALSE;
//	}

//	return TRUE;
}

/*
// マウスの状態を取得 //
BOOL CInput::GetMouseInput(PBG_MOUSESTATE *pMouseState)
{
	HRESULT			hr;
	DIMOUSESTATE	dims;
	int				x, y;


	// 致命的なエラー(内部エラー) //
	if(NULL == m_lpSysMouse) return FALSE;

	// キーボードの状態を取得する //
	hr = m_lpSysMouse->GetDeviceState(sizeof(dims), &dims);
	if(FAILED(hr)){
		return FALSE;
	}

	// マウスの相対移動量から絶対座標を算出する //
	x = m_MouseX + dims.lX;			// Ｘ座標
	y = m_MouseY + dims.lY;			// Ｙ座標

	// Ｘ座標を移動可能範囲に納める //
	if(     x > m_rcMouse.right) x = m_rcMouse.right;	// 右端
	else if(x < m_rcMouse.left ) x = m_rcMouse.left;	// 左端

	// Ｙ座標を移動可能範囲に納める //
	if(     y > m_rcMouse.bottom) y = m_rcMouse.bottom;	// 下端
	else if(y < m_rcMouse.top   ) y = m_rcMouse.top;	// 上端

	// 実際に座標を格納する(クラス内でも保存) //
	pMouseState->x = m_MouseX = x;			// マウスのＸ座標
	pMouseState->y = m_MouseY = y;			// マウスのＹ座標

	// ボタンの情報 //
	pMouseState->Left  = dims.rgbButtons[0] & 0x80;	// 左ボタン
	pMouseState->Right = dims.rgbButtons[1] & 0x80;	// 右ボタン

	return TRUE;
}
*/

// パッドの状態を取得 //
BOOL CInput::GetJoyInput(DIJOYSTATE *pJoyInfo, BYTE JoyID)
{
	LPDIRECTINPUTDEVICE7	lpdid;
	HRESULT					hr;

	while(1){
		// そりゃないっすよ //
		if(JoyID >= 2) break;

		lpdid = m_JoyState[JoyID].m_lpJoy;
		if(NULL == lpdid) break;

		// ポーリング //
		hr = lpdid->Poll();
		if(FAILED(hr)) break;

		hr = lpdid->GetDeviceState(sizeof(DIJOYSTATE), pJoyInfo);
		//if(FAILED(hr)) break;

		// ロストしている場合 //
		if(DIERR_INPUTLOST == hr){
			// デバイスのアクセス権を取得する         //
			// 成功した場合、もう一回まわることになる //
			hr = m_lpSysKeyboard->Acquire();
			if(FAILED(hr)) break;		// 失敗
		}
		// ロスト以外のエラーならば、外に出る //
		else if(FAILED(hr)){
			break;
		}
		// 成功したので、真を返す //
		else{
			if(abs(pJoyInfo->lX) < (JOY_RANGE / 2)) pJoyInfo->lX = 0;
			if(abs(pJoyInfo->lY) < (JOY_RANGE / 2)) pJoyInfo->lY = 0;

			return TRUE;
		}
	}

	ZeroMemory(pJoyInfo, sizeof(DIJOYSTATE));
	return FALSE;
}

/*
// マウスの移動範囲を変更する //
void CInput::SetMouseRange(RECT *rc)
{
	// 矩形を指定して、おしまい //
	m_rcMouse = *rc;
}
*/
/*
// マウスの現在位置を変更する //
void CInput::SetMousePos(int x, int y)
{
	m_MouseX = x;	// マウスのＸ座標
	m_MouseY = y;	// マウスのＹ座標

	// Ｘ座標を移動可能範囲に納める //
	if(     x > m_rcMouse.right) x = m_rcMouse.right;	// 右端
	else if(x < m_rcMouse.left ) x = m_rcMouse.left;	// 左端

	// Ｙ座標を移動可能範囲に納める //
	if(     y > m_rcMouse.bottom) y = m_rcMouse.bottom;	// 下端
	else if(y < m_rcMouse.top   ) y = m_rcMouse.top;	// 上端
}
*/

// インターフェース初期化 //
BOOL CInput::Initialize(HWND hWnd)
{
	HRESULT			hr;

	// まずは、問答無用で初期化 //
	Cleanup();

	// DirectInput インターフェースを作成する //
	hr = DirectInputCreateEx(
			GetModuleHandle(NULL),		// インスタンスハンドル
			DIRECTINPUT_VERSION,		// DirectInput バージョン
			IID_IDirectInput7,			// インターフェース識別子
			(LPVOID *)&m_lpDI,			// インターフェースの格納先
			NULL						// IUnknown インターフェース
		 );
	if(FAILED(hr)){
		PbgError("CInput::Initialize() : DirectInputCreateEx() に失敗");
		return FALSE;	// ダメでした
	}

	// システムキーボードの初期化 //
	if(FALSE == InitSysKeyboard(hWnd)){
		// 失敗時の解放はこの関数で行う //
		Cleanup();
		return FALSE;
	}
/*
	// システムマウスの初期化 //
	if(FALSE == InitSysMouse(hWnd)){
		// 失敗時の解放はこの関数で行う //
		Cleanup();
		return FALSE;
	}
*/

	hr = m_lpDI->EnumDevices(
			DIDEVTYPE_JOYSTICK,
			EnumJoyDevices,
			this,
			DIEDFL_ATTACHEDONLY
		 );
	if(FAILED(hr)){
		PbgError("CInput::Initialize() : EnumDevices() に失敗");
		return FALSE;	// ダメでした
	}

	// この関数は失敗しても構わない //
	InitJoystick(hWnd, 0, 0);		// パッド０
	InitJoystick(hWnd, 1, 1);		// パッド１

	// 成功したので、ウィンドウハンドルを記憶しておく //
	m_hWindow = hWnd;

	// 初期状態は DirectInput 有効 //
//	m_bEnableDI = TRUE;

	return TRUE;
}


// インターフェース解放 //
void CInput::Cleanup()
{
	SAFE_RELEASE(m_lpSysKeyboard);	// システム・キーボード
//	SAFE_RELEASE(m_lpSysMouse);		// システム・マウス

	ReleaseJoystick(0);		// パッド０
	ReleaseJoystick(1);		// パッド１

	SAFE_RELEASE(m_lpDI);	// DirectInput インターフェース

	ZEROMEM(m_JoyInfo);		// パッド列挙情報
	ZEROMEM(m_JoyState);	// パッドの状態

	m_NumJoyDevice = 0;			// 列挙情報も無効化
//	m_bEnableDI    = FALSE;		// DirectInput 無効化
}

/*
// DirectInput を有効にするかどうか(TRUE : 使用する) //
BOOL CInput::EnableDirectInput(BOOL IsUseDInput)
{
	// 有効にする場合 //
	if(TRUE == IsUseDInput){
		// すでに有効になっている場合 //
		if(TRUE == m_bEnableDI) return TRUE;

		// 少なくとも以下のインターフェースが使用できなければいけない //
		if(NULL == m_lpDI)			return FALSE;	// DInput インターフェース
		if(NULL == m_lpSysKeyboard)	return FALSE;	// システムキーボード
		if(NULL == m_lpSysMouse)	return FALSE;	// システムマウス

		// フラグを有効にする //
		m_bEnableDI = TRUE;

		return TRUE;
	}

	// フラグを無効にする //
	m_bEnableDI = FALSE;

	// どんな状況においても無効化は成功する //
	return TRUE;
}
*/

// パッドのドライバ数を取得 //
DWORD CInput::GetNumJoyDriver(void)
{
	return m_NumJoyDevice;
}


// パッドのドライバを変更 //
BOOL CInput::ChangeJoyDriver(BYTE JoyID, DWORD DeviceID)
{
	if(JoyID >= 2) return FALSE;

	// インターフェース解放 //
	ReleaseJoystick(JoyID);

	// 後は、おまかせ //
	return InitJoystick(m_hWindow, JoyID, DeviceID);
}


// ドライバ名を取得 //
BOOL CInput::GetJoyDriverName(char *pName, DWORD JoyID)
{
	if(JoyID >= 2) return FALSE;

	strcpy(pName, m_JoyInfo[JoyID].m_strInst);

	return TRUE;
}


// システムキーボードの初期化 //
BOOL CInput::InitSysKeyboard(HWND hWnd)
{
	HRESULT		hr;

	// システムキーボードのインターフェースを作成 //
	hr = m_lpDI->CreateDeviceEx(
			GUID_SysKeyboard,			// システムキーボードのＧＵＩＤ
			IID_IDirectInputDevice7,	// インターフェース識別子
			(void **)&m_lpSysKeyboard,	// インターフェースの格納先
			NULL						// IUnknown インターフェース
		 );
	if(FAILED(hr)){
		PbgError("CInput::InitSysKeyboard() : CreateDeviceEx() に失敗");
		return FALSE;
	}

	// 定義済みのデータフォーマットで全く問題なし //
	hr = m_lpSysKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr)){
		PbgError("CInput::InitSysKeyboard() : SetDataFormat() に失敗");
		return FALSE;
	}

	// システムキーボードの強調レベルをセットする //
	hr = m_lpSysKeyboard->SetCooperativeLevel(
			hWnd,					// ウィンドウハンドル

			  DISCL_BACKGROUND		// バックグラウンド動作
			| DISCL_NONEXCLUSIVE	// 非排他アクセス権
//			| DISCL_NOWINKEY		// Windows キーを無効化する
		 );
	if(FAILED(hr)){
		PbgError("CInput::InitSysKeyboard() : SetCooperativeLevel() に失敗");
		return FALSE;..
	}

	hr = m_lpSysKeyboard->Acquire();
	if(FAILED(hr)){
		PbgError("CInput::InitSysKeyboard() : Acquire() に失敗");
		return FALSE;
	}

	return TRUE;
}


// ジョイスティックの初期化 //
BOOL CInput::InitJoystick(HWND hWnd, BYTE JoyID, DWORD DeviceID)
{
	HRESULT			hr;
	DIPROPRANGE		dipr;


	// 内部エラー(パッド数の指定がおかしい) //
	if(JoyID >= 2) return FALSE;

	// 列挙されたドライバの範囲に収まっていない //
	if(DeviceID >= m_NumJoyDevice){
		ReleaseJoystick(JoyID);		// インターフェース解放
		return FALSE;
	}

	// ジョイスティックのインターフェースを作成する //
	hr = m_lpDI->CreateDeviceEx(
			m_JoyInfo[DeviceID].m_Guid,			// ジョイスティックのＧＵＩＤ
			IID_IDirectInputDevice7,			// インターフェース識別子
			(void **)&m_JoyState[JoyID].m_lpJoy,// インターフェースの格納先
			NULL								// IUnknown インターフェース
		 );
	if(FAILED(hr)){
		PbgError("CInput::InitJoystick() : CreateDeviceEx() に失敗");
		ReleaseJoystick(JoyID);	// インターフェース解放
		return FALSE;
	}

	// 暫くは、定義済みのデータフォーマットにしておきましょう //
	hr = m_JoyState[JoyID].m_lpJoy->SetDataFormat(&c_dfDIJoystick);
	if(FAILED(hr)){
		PbgError("CInput::InitJoystick() : SetDataFormat() に失敗");
		ReleaseJoystick(JoyID);	// 失敗したので解放する
		return FALSE;
	}

	ZEROMEM(dipr);
	dipr.lMin = -JOY_RANGE;
	dipr.lMax = +JOY_RANGE;
	dipr.diph.dwSize       = sizeof(dipr);
	dipr.diph.dwHow        = DIPH_BYOFFSET;
	dipr.diph.dwHeaderSize = sizeof(dipr.diph);
	dipr.diph.dwObj        = DIJOFS_X;

	hr = m_JoyState[JoyID].m_lpJoy->SetProperty(DIPROP_RANGE, &dipr.diph);
	if(FAILED(hr)){
		PbgError("CInput::InitJoystick() : SetProperty(DIJOFS_X) に失敗");
		ReleaseJoystick(JoyID);	// 失敗したので解放する
		return FALSE;
	}

	dipr.diph.dwObj        = DIJOFS_Y;

	hr = m_JoyState[JoyID].m_lpJoy->SetProperty(DIPROP_RANGE, &dipr.diph);
	if(FAILED(hr)){
		PbgError("CInput::InitJoystick() : SetProperty(DIJOFS_Y) に失敗");
		ReleaseJoystick(JoyID);	// 失敗したので解放する
		return FALSE;
	}

	// ジョイスティックの強調レベルをセットする //
	hr = m_JoyState[JoyID].m_lpJoy->SetCooperativeLevel(
			hWnd,					// ウィンドウハンドル
			  DISCL_BACKGROUND		// バックグラウンド動作
			| DISCL_NONEXCLUSIVE	// 非排他アクセス権
		 );
	if(FAILED(hr)){
		PbgError("CInput::InitJoystick() : SetCooperativeLevel() に失敗");
		ReleaseJoystick(JoyID);
		return FALSE;
	}

	// それじゃ、行ってみましょう //
	hr = m_JoyState[JoyID].m_lpJoy->Acquire();
	if(FAILED(hr)){
		PbgError("CInput::InitJoystick() : Acquire() に失敗");
		ReleaseJoystick(JoyID);
		return FALSE;
	}

// ------------------------------------------------------------------------- //
			// この部分で、エフェクトの初期化を行う //
// ------------------------------------------------------------------------- //

	// 最後に、デバイス情報の格納先を記憶する //
	m_JoyState[JoyID].m_pInfo = &m_JoyInfo[DeviceID];

	return TRUE;
}


// ジョイスティックの解放 //
void CInput::ReleaseJoystick(BYTE JoyID)
{
	// 内部エラー(パッド数の指定がおかしい) //
	if(JoyID >= 2) return;

	SAFE_RELEASE(m_JoyState[JoyID].m_lpEffect);	// エフェクト
	SAFE_RELEASE(m_JoyState[JoyID].m_lpJoy);	// パッドのインターフェース

	// 参照先を無効化する //
	m_JoyState[JoyID].m_pInfo = NULL;
}


// パッド系のデバイスを列挙する為のコールバック関数 //
BOOL CALLBACK CInput::EnumJoyDevices(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	CInput		*pThis;
	DWORD		n;

	// 内部エラー //
	if(NULL == pvRef) return FALSE;

	pThis = (CInput *)pvRef;			// this ポインタへと
	n     = pThis->m_NumJoyDevice;		// これまでに列挙されたデバイス数

	// もうこれ以上列挙できない場合 //
	if(n >= ENUM_INPUT_DEVICE) return FALSE;

	// まず、インスタンス名を保存する //
	strcpy(pThis->m_JoyInfo[n].m_strInst, lpddi->tszInstanceName);

	// インスタンスＧＵＩＤを保存する //
	pThis->m_JoyInfo[n].m_Guid = lpddi->guidInstance;

	// インスタンスＧＵＩＤをＣ文字列で保存する //
	GuidToString(&(lpddi->guidInstance), pThis->m_JoyInfo[n].m_strGuid);

	pThis->m_JoyInfo[n].m_bEnableFF = FALSE;	// 現時点では、無効にしておく
	pThis->m_NumJoyDevice++;					// 列挙されたのでパッド数を追加

	return TRUE;
}



} // namespace Pbg
