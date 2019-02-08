/*
 *   CKeyInput.cpp   : キーボード入力管理
 *
 */

#include "CKeyInput.h"
#include "PbgError.h"
#include "LZUTY\\LzDecode.h"
#include "PbgMem.h"



#define KEYCODE_UNKNOWN		0xff	// 定義されていないキー



// 各方向に対する仮想キーコード //
WORD g_DirVCode[8] = {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	(KEY_UP | KEY_LEFT), (KEY_UP | KEY_RIGHT),
	(KEY_DOWN | KEY_LEFT), (KEY_DOWN | KEY_RIGHT)
};



int				CKeyInput::m_RefCount     = 0;	// 参照カウント
DIK_NameTable	*CKeyInput::m_pKeyNameTbl = 0;	// キーのお名前テーブル



// コンストラクタ //
CKeyInput::CKeyInput(const BYTE *pBuffer, INPUT_DEVICE_ID DevID)
	: m_pKeyBuf(pBuffer)
{
	m_DeviceID = DevID;
	m_pShare   = NULL;

	// 設定を初期化する //
	OnResetSetting();

	// 一つ目のクラスならば、名前テーブルを生成する //
	if(0 == m_RefCount) InitializeNameTable();
	m_RefCount++;
}


// デストラクタ //
CKeyInput::~CKeyInput()
{
	// 最後のクラスならば、名前テーブルを削除する //
	m_RefCount--;
	if(0 == m_RefCount) CleanupNameTable();
}


// 入力コードを取得する //
WORD CKeyInput::GetCode(void)
{
	const BYTE	*pBuf = m_pKeyBuf;
	WORD		VCode = 0;
	DWORD		i;
	BYTE		DirID;

	// ショット //
	if((KEYCODE_UNKNOWN != m_ShotID) && (pBuf[m_ShotID] & 0x80)){
		VCode |= KEY_SHOT;
	}

	// ボム //
	if((KEYCODE_UNKNOWN != m_BombID) && (pBuf[m_BombID] & 0x80)){
		VCode |= KEY_BOMB;
	}

	// 低速移動 //
	if((KEYCODE_UNKNOWN != m_ShiftID) && (pBuf[m_ShiftID] & 0x80)){
		VCode |= KEY_SHIFT;
	}

	// メニュー //
	if(pBuf[DIK_ESCAPE] & 0x80){
		VCode |= KEY_MENU;
	}

	// ８方向への移動 //
	for(i=0; i<8; i++){
		DirID = m_DirID[i];

		// キーの設定が行われていて、キーが押されていたら、その方向の //
		// キーコードに対するビットを立てる                           //
		if((KEYCODE_UNKNOWN != DirID) && (pBuf[DirID] & 0x80)){
			VCode |= g_DirVCode[i];
		}
	}

#pragma message("<<<<<<<<<<<<<<<<<<<<  W A R N I N G  >>>>>>>>>>>>>>>>>>>>>>>")
#pragma message("以下のコードは、鳩山祭専用の仕様です。修正が必要なので注意!!")
#pragma message("<<<<<<<<<<<<<<<<<<<<  W A R N I N G  >>>>>>>>>>>>>>>>>>>>>>>")
	if(IDEVID_FULLKEY == m_DeviceID){
		if(pBuf[DIK_UP]    & 0x80) VCode |= KEY_UP;
		if(pBuf[DIK_DOWN]  & 0x80) VCode |= KEY_DOWN;
		if(pBuf[DIK_LEFT]  & 0x80) VCode |= KEY_LEFT;
		if(pBuf[DIK_RIGHT] & 0x80) VCode |= KEY_RIGHT;
	}

	// 後は、仮想コードを返すのみ //
	return VCode;
}


// 指定されたキーに対して割り当てを行う //
BOOL CKeyInput::OnChangeSetting(BYTE TargetID)
{
	const BYTE	*pBuf = m_pKeyBuf;
	int			i, KeyID;

	// こりゃマズぃでしょ //
	if(NULL == pBuf) return FALSE;

	KeyID = -1;		// ダミーの値を代入

	// 各キーを調べる //
	for(i=0; i<255; i++){
		if(pBuf[i] & 0x80){
			// ２つ以上のキーが押されている場合は偽を返す //
			if(-1 != KeyID) return FALSE;
			else            KeyID = i;
		}
	}

	// 何もキーが押されていない場合 //
	if(-1 == KeyID){
		m_bEnableChange = TRUE;
		return FALSE;
	}

	// 変更不能な場合 //
	if(FALSE == m_bEnableChange) return FALSE;

	switch(KeyID){
		// 以下の case で示されるキーに割り当てることは出来ない //
		case DIK_RETURN:	case DIK_ESCAPE:
		case DIK_F1:	case DIK_F2:	case DIK_F3:	case DIK_F4:
		case DIK_F5:	case DIK_F6:	case DIK_F7:	case DIK_F8:
		case DIK_F9:	case DIK_F10:	case DIK_F11:	case DIK_F12:
		return FALSE;

		default:
		break;
	}

	// どれを変更するのかによって分岐する //
	switch(TargetID){
		case c_DirUp:			//  上  方向への移動
		case c_DirDown:			//  下  方向への移動
		case c_DirRight:		//  右  方向への移動
		case c_DirLeft:			//  左  方向への移動
		case c_DirUpRight:		// 右上 方向への移動
		case c_DirUpLeft:		// 左上 方向への移動
		case c_DirDownRight:	// 右下 方向への移動
		case c_DirDownLeft:		// 左下 方向への移動
			if(FALSE == SwapCode(KeyID, m_DirID[TargetID])){
				if(m_pShare) m_pShare->SwapCode(KeyID, m_DirID[TargetID]);
			}

			m_DirID[TargetID] = KeyID;
		break;

		case c_ButtonShot:		// ショット
			if(FALSE == SwapCode(KeyID, m_ShotID)){
				if(m_pShare) m_pShare->SwapCode(KeyID, m_ShotID);
			}

			m_ShotID = KeyID;
		break;

		case c_ButtonBomb:		// ボム
			if(FALSE == SwapCode(KeyID, m_BombID)){
				if(m_pShare) m_pShare->SwapCode(KeyID, m_BombID);
			}

			m_BombID = KeyID;
		break;

		case c_ButtonShift:		// 低速移動
			if(FALSE == SwapCode(KeyID, m_ShiftID)){
				if(m_pShare) m_pShare->SwapCode(KeyID, m_ShiftID);
			}

			m_ShiftID = KeyID;
		break;

		default:
		return FALSE;
	}

	m_bEnableChange = FALSE;

	return TRUE;
}


// ある機能に割り当てられたボタンorキーの名称を返す //
FVOID CKeyInput::GetButtonName(char *pBuf, BYTE TargetID)
{
	BYTE	KeyID;

	if(NULL == m_pKeyNameTbl){
		strcpy(pBuf, "UNKNOWN");
		return;
	}

	switch(TargetID){
		case c_DirUp:			//  上  方向への移動
		case c_DirDown:			//  下  方向への移動
		case c_DirRight:		//  右  方向への移動
		case c_DirLeft:			//  左  方向への移動
		case c_DirUpRight:		// 右上 方向への移動
		case c_DirUpLeft:		// 左上 方向への移動
		case c_DirDownRight:	// 右下 方向への移動
		case c_DirDownLeft:		// 左下 方向への移動
			KeyID = m_DirID[TargetID];
		break;

		case c_ButtonShot:		// ショット
			KeyID = m_ShotID;
		break;

		case c_ButtonBomb:		// ボム
			KeyID = m_BombID;
		break;

		case c_ButtonShift:		// 低速移動
			KeyID = m_ShiftID;
		break;

		default:
			strcpy(pBuf, "ERROR");
		return;
	}

	strcpy(pBuf, m_pKeyNameTbl[KeyID].Data);
}


// 設定を初期状態にリセットする //
void CKeyInput::OnResetSetting(void)
{
	switch(m_DeviceID){
	default:
#ifdef PBG_DEBUG
		PbgError("CKeyInput::OnResetSetting() : デバイスＩＤがおかしいっすよ");
#endif

	case IDEVID_FULLKEY:
		m_DirID[c_DirUp]        = DIK_NUMPAD8;	//  上  方向への移動
		m_DirID[c_DirDown]      = DIK_NUMPAD2;	//  下  方向への移動
		m_DirID[c_DirRight]     = DIK_NUMPAD6;	//  右  方向への移動
		m_DirID[c_DirLeft]      = DIK_NUMPAD4;	//  左  方向への移動
		m_DirID[c_DirUpRight]   = DIK_NUMPAD9;	// 右上 方向への移動
		m_DirID[c_DirUpLeft]    = DIK_NUMPAD7;	// 左上 方向への移動
		m_DirID[c_DirDownRight] = DIK_NUMPAD3;	// 右下 方向への移動
		m_DirID[c_DirDownLeft]  = DIK_NUMPAD1;	// 左下 方向への移動

		m_ShotID  = DIK_Z;		// ショット
		m_BombID  = DIK_X;		// ボム
		m_ShiftID = DIK_LSHIFT;	// 低速移動
	break;

	case IDEVID_HALFKEY1P:
		m_DirID[c_DirUp]        = DIK_T;	//  上  方向への移動
		m_DirID[c_DirDown]      = DIK_B;	//  下  方向への移動
		m_DirID[c_DirRight]     = DIK_H;	//  右  方向への移動
		m_DirID[c_DirLeft]      = DIK_F;	//  左  方向への移動
		m_DirID[c_DirUpRight]   = DIK_Y;	// 右上 方向への移動
		m_DirID[c_DirUpLeft]    = DIK_R;	// 左上 方向への移動
		m_DirID[c_DirDownRight] = DIK_N;	// 右下 方向への移動
		m_DirID[c_DirDownLeft]  = DIK_V;	// 左下 方向への移動

		m_ShotID  = DIK_Z;		// ショット
		m_BombID  = DIK_X;		// ボム
		m_ShiftID = DIK_LSHIFT;	// 低速移動
	break;

	case IDEVID_HALFKEY2P:
		m_DirID[c_DirUp]        = DIK_NUMPAD8;	//  上  方向への移動
		m_DirID[c_DirDown]      = DIK_NUMPAD2;	//  下  方向への移動
		m_DirID[c_DirRight]     = DIK_NUMPAD6;	//  右  方向への移動
		m_DirID[c_DirLeft]      = DIK_NUMPAD4;	//  左  方向への移動
		m_DirID[c_DirUpRight]   = DIK_NUMPAD9;	// 右上 方向への移動
		m_DirID[c_DirUpLeft]    = DIK_NUMPAD7;	// 左上 方向への移動
		m_DirID[c_DirDownRight] = DIK_NUMPAD3;	// 右下 方向への移動
		m_DirID[c_DirDownLeft]  = DIK_NUMPAD1;	// 左下 方向への移動

		m_ShotID  = DIK_SLASH;		// ショット
		m_BombID  = DIK_BACKSLASH;	// ボム
		m_ShiftID = DIK_PERIOD;		// 低速移動
	break;
	}
}


// SearchCode が一致したボタンに SetCode を割り当てる      //
// 割り当てが成功するか SearchCode == SetCode なら真を返す //
BOOL CKeyInput::SwapCode(BYTE SearchCode, BYTE SetCode)
{
	int				i;

	for(i=0; i<8; i++){
		if(SearchCode == m_DirID[i]){
			m_DirID[i] = SetCode;
			return TRUE;
		}
	}

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

	return FALSE;
}


// 現在の設定を取得する //
void CKeyInput::GetConfigData(BYTE Data[8+3])
{
	int			i;

	for(i=0; i<8; i++) Data[i] = m_DirID[i];

	Data[8]  = m_ShotID;
	Data[9]  = m_BombID;
	Data[10] = m_ShiftID;
}


// 設定を変更する //
void CKeyInput::SetConfigData(BYTE Data[8+3])
{
	int			i;

	for(i=0; i<8; i++) m_DirID[i] = Data[i];

	m_ShotID  = Data[8];
	m_BombID  = Data[9];
	m_ShiftID = Data[10];
}


// キーコードを共有する対象の設定(２Ｐキーボード用) //
void CKeyInput::ShareKeyConfig(CKeyInput *pShare)
{
	m_pShare = pShare;
}


// 名前テーブルを初期化する //
FVOID CKeyInput::InitializeNameTable(void)
{
	Pbg::LzDecode		Decode;
	char				*pInputHelpID = "STRTABLE/INPUTHELP";


	if(TRUE == Decode.Open("ENEMY.DAT")){
		m_pKeyNameTbl = (DIK_NameTable *)Decode.Decode("STRTABLE/INPUTDEF");

#ifdef PBG_DEBUG
		if(NULL == m_pKeyNameTbl){
			PbgError("ENEMY.DAT : 文字列テーブル(Input)のロードに失敗");
		}
#endif
		Decode.Close();
	}
}


// 名前テーブルを解放する //
FVOID CKeyInput::CleanupNameTable(void)
{
	if(m_pKeyNameTbl) MemFree(m_pKeyNameTbl);
}
