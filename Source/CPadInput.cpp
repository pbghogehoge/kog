/*
 *   CPadInput.cpp   : パッド入力管理
 *
 */

#include "CPadInput.h"
#include "Gian2001.h"



// コンストラクタ //
CPadInput::CPadInput(BYTE PlayerID)
	: m_PlayerID(PlayerID)
{
	// パッドＩＤからデバイスＩＤを仕掛ける //
	m_DeviceID = ((0 == PlayerID) ? IDEVID_PAD1P : IDEVID_PAD2P);

	// 設定を初期化する //
	OnResetSetting();
}


// デストラクタ //
CPadInput::~CPadInput()
{
	// 特に何もしません //
}


// 入力コードを取得する //
WORD CPadInput::GetCode(void)
{
	DIJOYSTATE		JoyState;
	WORD			VCode    = 0;
	BYTE			PlayerID = m_PlayerID;

	if(PlayerID >= 2) return 0;		// プレイヤーＩＤがおかしい
	if(NULL == g_pInp) return 0;	// 入力が死んでいる

	// 押し下げ情報の取得 //
	if(FALSE == g_pInp->GetJoyInput(&JoyState, PlayerID)) return 0;

	// Ｘ軸方向の移動 //
	if(     JoyState.lX < 0) VCode |= KEY_LEFT;		// 左方向の移動
	else if(JoyState.lX > 0) VCode |= KEY_RIGHT;	// 右方向の移動

	// Ｙ軸方向の移動 //
	if(     JoyState.lY < 0) VCode |= KEY_UP;		// 上方向の移動
	else if(JoyState.lY > 0) VCode |= KEY_DOWN;		// 下方向の移動

	// 各ボタンの押し下げ状況を反映する //
	if(JoyState.rgbButtons[m_ShotID]  & 0x80) VCode |= KEY_SHOT;	//  ショット
	if(JoyState.rgbButtons[m_BombID]  & 0x80) VCode |= KEY_BOMB;	//    ボム
	if(JoyState.rgbButtons[m_ShiftID] & 0x80) VCode |= KEY_SHIFT;	// シフト移動
	if(JoyState.rgbButtons[m_MenuID]  & 0x80) VCode |= KEY_MENU;	//  メニュー

	return VCode;
}


// 指定されたボタンに対して割り当てを行う //
BOOL CPadInput::OnChangeSetting(BYTE TargetID)
{
	BYTE			PlayerID = m_PlayerID;
	BYTE			ButtonID;
	DIJOYSTATE		JoyState;
	int				i;

	if(PlayerID >= 2) return FALSE;		// プレイヤーＩＤがおかしい
	if(NULL == g_pInp) return FALSE;	// 入力が死んでいる

	// 押し下げ情報の取得 //
	if(FALSE == g_pInp->GetJoyInput(&JoyState, PlayerID)) return FALSE;


	ButtonID = 255;		// ダミーの値を代入

	// 各ボタンの押し下げ状況を調べる //
	for(i=0; i<32; i++){
		if(JoyState.rgbButtons[i] & 0x80){
			// 押し下げられているキーが２つ以上の場合は、失敗 //
			if(ButtonID != 255) return FALSE;
			else                ButtonID = i;
		}
	}

	if(255 == ButtonID){
		m_bEnableChange = TRUE;
		return FALSE;
	}

	if(FALSE == m_bEnableChange) return FALSE;

	switch(TargetID){
		case c_ButtonShot:		// ショット
			SwapCode(ButtonID, m_ShotID);
			m_ShotID = ButtonID;
		break;

		case c_ButtonBomb:		// ボム
			SwapCode(ButtonID, m_BombID);
			m_BombID = ButtonID;
		break;

		case c_ButtonShift:		// 低速移動
			SwapCode(ButtonID, m_ShiftID);
			m_ShiftID = ButtonID;
		break;

		case c_ButtonMenu:		// メニュー
			SwapCode(ButtonID, m_MenuID);
			m_MenuID = ButtonID;
		break;

		default:
		return FALSE;
	}

	m_bEnableChange = FALSE;

	return TRUE;
}


// ある機能に割り当てられたボタンorキーの名称を返す //
FVOID CPadInput::GetButtonName(char *pBuf, BYTE TargetID)
{
	int		ButtonID;

	switch(TargetID){
		case c_ButtonShot:		// ショット
			ButtonID = m_ShotID;
		break;

		case c_ButtonBomb:		// ボム
			ButtonID = m_BombID;
		break;

		case c_ButtonShift:		// 低速移動
			ButtonID = m_ShiftID;
		break;

		case c_ButtonMenu:		// メニュー
			ButtonID = m_MenuID;
		break;

		default:
			strcpy(pBuf, "");
		return;
	}

	wsprintf(pBuf, "BUTTON %d", ButtonID);
}


// 設定を初期状態にリセットする //
void CPadInput::OnResetSetting(void)
{
	m_ShotID  = 0;		// ショットのデフォルト番号
	m_BombID  = 1;		// ボムのデフォルト番号
	m_ShiftID = 2;		// シフト移動のデフォルト番号
	m_MenuID  = 3;		// メニューのデフォルト番号
}


// SearchCode が一致したボタンに SetCode を割り当てる      //
// 割り当てが成功するか SearchCode == SetCode なら真を返す //
BOOL CPadInput::SwapCode(BYTE SearchCode, BYTE SetCode)
{
	if(SearchCode == SetCode) return TRUE;

	if(SearchCode == m_ShotID){
		m_ShotID = SetCode;
		return TRUE;
	}
	if(SearchCode == m_BombID){
		m_BombID = SetCode;
		return TRUE;
	}
	if(SearchCode == m_ShiftID){
		m_ShiftID = SetCode;
		return TRUE;
	}
	if(SearchCode == m_MenuID){
		m_MenuID = SetCode;
		return TRUE;
	}

	return FALSE;
}


// このパッドの名称を取得する //
FVOID CPadInput::GetDeviceName(char *pBuf)
{
	g_pInp->GetJoyDriverName(pBuf, m_PlayerID);
}


// 現在の設定を取得する //
void CPadInput::GetConfigData(BYTE Data[4])
{
	Data[0] = m_ShotID;
	Data[1] = m_BombID;
	Data[2] = m_ShiftID;
	Data[3] = m_MenuID;
}


// 設定を変更する //
void CPadInput::SetConfigData(BYTE Data[4])
{
	m_ShotID  = Data[0];
	m_BombID  = Data[1];
	m_ShiftID = Data[2];
	m_MenuID  = Data[3];
}
