/*
 *   CInputCtrl.cpp   : 入力管理クラス
 *
 */

#include "CInputCtrl.h"
#include "Gian2001.h"



// コンストラクタ //
CInputCtrl::CInputCtrl() :
	m_FullKeyboard(m_KeyBuffer, IDEVID_FULLKEY),		// フルキーボード配置
	m_HalfKeyboard_1P(m_KeyBuffer, IDEVID_HALFKEY1P),	// １Ｐ側
	m_HalfKeyboard_2P(m_KeyBuffer, IDEVID_HALFKEY2P),	// ２Ｐ側
	m_Pad_1P(0), m_Pad_2P(1),							// パッド

	m_FullKeyWindow(&m_FullKeyboard),	// フルキーボードコンフィグ窓
	m_Key1Window(&m_HalfKeyboard_1P),	// キーボード(１Ｐ)コンフィグ窓
	m_Key2Window(&m_HalfKeyboard_2P),	// キーボード(２Ｐ)コンフィグ窓
	m_Pad1Window(&m_Pad_1P),			// パッド(１Ｐ)コンフィグ窓
	m_Pad2Window(&m_Pad_2P)				// パッド(２Ｐ)コンフィグ窓
{
	m_pWindow[0] = NULL;
	m_pWindow[1] = NULL;

	SelectInputDevice(IMODE_1P);
	SelectConfigWindow(CFGW_DISABLE);

	// キーボードコンフィグ共有の設定 //
	m_HalfKeyboard_1P.ShareKeyConfig(&m_HalfKeyboard_2P);
	m_HalfKeyboard_2P.ShareKeyConfig(&m_HalfKeyboard_1P);

	m_FullKeyWindow.SetWindowPos(350, 150);

	m_Key1Window.SetWindowPos(350, 20);
	m_Key2Window.SetWindowPos(350, 210);

	m_Pad1Window.SetWindowPos(350, 100);
	m_Pad2Window.SetWindowPos(350, 250);
}


// デストラクタ //
CInputCtrl::~CInputCtrl()
{
	// 特に何もしない //
}


// 各データを更新する //
void CInputCtrl::UpdateBuffer(void)
{
	g_pInp->GetKeyboardInput(m_KeyBuffer);

	m_bIsPadInput = FALSE;

	m_1PInput  = __Get1PInput();
	m_2PInput  = __Get2PInput();
	m_SysInput = __GetSysKeyInput();
}


// 入力デバイスを選択 //
BOOL CInputCtrl::SelectInputDevice(INPUT_MODE Mode)
{
	switch(Mode){
		case IMODE_1P:			// １Ｐモード(FullKey or 1P-Pad)
		case IMODE_2PDEFAULT:	// ２Ｐモード標準(HalfKey VS HalfKey : 1P-Pad VS 2P-Pad)
		case IMODE_KEYPAD:		// ２Ｐモード(FullKey VS 1P-Pad)
		case IMODE_PADKEY:		// ２Ｐモード(1P-Pad VS FullKey)
		case IMODE_TCPIP:		// ＴＣＰ/ＩＰモード
		break;

		default:	// 不明なモードの指定
#ifdef PBG_DEBUG
		PbgError("CInputCtrl::SelectInputDevice() : デバＩＤが変だ");
#endif
		return FALSE;
	}

	// 実際に入力デバイス選択モードを変更する //
	m_CurrentMode = Mode;

	return TRUE;
}


BOOL CInputCtrl::IsActiveConfigWindow(void)
{
	if(m_pWindow[0] && m_pWindow[0]->IsActive()) return TRUE;
	if(m_pWindow[1] && m_pWindow[1]->IsActive()) return TRUE;

	return FALSE;
}


// パッド入力があれば真 //
BOOL CInputCtrl::IsPadInput(void)
{
	return m_bIsPadInput;
}


// コンフィグ窓を選択する //
BOOL CInputCtrl::SelectConfigWindow(CFG_WINDOW CfgWnd)
{
	if(m_pWindow[0]) m_pWindow[0]->OnActivate(FALSE);
	if(m_pWindow[1]) m_pWindow[1]->OnActivate(FALSE);

	m_pWindow[0] = NULL;
	m_pWindow[1] = NULL;

	switch(CfgWnd){
		case CFGW_FULLKEY:	// フルキーボード
			m_pWindow[0] = &m_FullKeyWindow;
		break;

		case CFGW_HALFKEY:	// キーボード１＆２
			m_pWindow[0] = &m_Key1Window;
			m_pWindow[1] = &m_Key2Window;
		break;

		case CFGW_PAD:		// パッド１＆２
			m_pWindow[0] = &m_Pad1Window;

			if(g_pInp->GetNumJoyDriver() >= 2){
				m_pWindow[1] = &m_Pad2Window;
			}
		break;

		case CFGW_DISABLE:	// 無効状態
		return TRUE;

		default:
		return FALSE;
	}

	if(m_pWindow[0]) m_pWindow[0]->OnActivate(TRUE);
	if(m_pWindow[1]) m_pWindow[1]->OnActivate(FALSE);

	return TRUE;
}


// コンフィグ窓を動作させる //
void CInputCtrl::MoveConfigWindow(void)
{
	int		ActivateTarget = 0;

	// この時点でアクティブなウィンドウが無ければ、即座に戻る //
	if(FALSE == IsActiveConfigWindow()) return;

	// 現在アクティブなウィンドウを調べる //
	if(m_pWindow[1] && m_pWindow[1]->IsActive()) ActivateTarget = 1;

	// 各ウィンドウを動作させる //
	if(m_pWindow[0]) m_pWindow[0]->Move();
	if(m_pWindow[1]) m_pWindow[1]->Move();

	// 両方のウィンドウが非アクティブになっていたら、   //
	// 前回アクティブだったウィンドウをアクティブにする //
	if(FALSE == IsActiveConfigWindow()){
		m_pWindow[ActivateTarget]->OnActivate(TRUE);
	}
}


// コンフィグ窓を描画する //
void CInputCtrl::DrawConfigWindow(void)
{
	if(m_pWindow[0]) m_pWindow[0]->Draw();
	if(m_pWindow[1]) m_pWindow[1]->Draw();
}


// 設定を初期化する //
void CInputCtrl::InitSetting(void)
{
	m_FullKeyboard.OnResetSetting();
	m_HalfKeyboard_1P.OnResetSetting();
	m_HalfKeyboard_2P.OnResetSetting();

	m_Pad_1P.OnResetSetting();
	m_Pad_2P.OnResetSetting();

	m_TCPIPInput.OnResetSetting();
}


// データを書き込む //
void CInputCtrl::SetData(InpCfgData *pData)
{
	m_FullKeyboard.SetConfigData(pData->FullKeyCfg);
	m_HalfKeyboard_1P.SetConfigData(pData->HalfKey1PCfg);
	m_HalfKeyboard_2P.SetConfigData(pData->HalfKey2PCfg);

	m_Pad_1P.SetConfigData(pData->Pad1PCfg);
	m_Pad_2P.SetConfigData(pData->Pad2PCfg);
}


// データを取得する //
void CInputCtrl::GetData(InpCfgData *pData)
{
	m_FullKeyboard.GetConfigData(pData->FullKeyCfg);
	m_HalfKeyboard_1P.GetConfigData(pData->HalfKey1PCfg);
	m_HalfKeyboard_2P.GetConfigData(pData->HalfKey2PCfg);

	m_Pad_1P.GetConfigData(pData->Pad1PCfg);
	m_Pad_2P.GetConfigData(pData->Pad2PCfg);
}


// １Ｐ側の入力を取得 //
WORD CInputCtrl::__Get1PInput(void)
{
	WORD		PadCode;

	switch(m_CurrentMode){
		case IMODE_1P:
		case IMODE_TCPIP:
			PadCode = m_Pad_1P.GetCode();
			if(PadCode) m_bIsPadInput = TRUE;
		return m_FullKeyboard.GetCode() | PadCode;

		case IMODE_KEYPAD:
		return m_FullKeyboard.GetCode();

		case IMODE_PADKEY:		// 仕様変更の可能性有り
			PadCode = m_Pad_1P.GetCode() | m_Pad_2P.GetCode();
			if(PadCode) m_bIsPadInput = TRUE;
		return PadCode;

		case IMODE_2PDEFAULT:	// 仕様変更の可能性有り
			PadCode = m_Pad_1P.GetCode();
			if(PadCode) m_bIsPadInput = TRUE;
		return m_HalfKeyboard_1P.GetCode() | PadCode;

		default:
		return 0;
	}
}


// ２Ｐ側の入力を取得 //
WORD CInputCtrl::__Get2PInput(void)
{
	WORD		PadCode;

	switch(m_CurrentMode){
		case IMODE_PADKEY:
		return m_FullKeyboard.GetCode();

		case IMODE_KEYPAD:		// 仕様変更の可能性有り
			PadCode = m_Pad_1P.GetCode() | m_Pad_2P.GetCode();
			if(PadCode) m_bIsPadInput = TRUE;
		return PadCode;

		case IMODE_2PDEFAULT:	// 仕様変更の可能性有り
			PadCode = m_Pad_2P.GetCode();
			if(PadCode) m_bIsPadInput = TRUE;
		return m_HalfKeyboard_2P.GetCode() | PadCode;

		case IMODE_TCPIP:
		return m_TCPIPInput.GetCode();

		case IMODE_1P:	default:
		return 0;
	}
}


// システム側の入力を取得 //
WORD CInputCtrl::__GetSysKeyInput(void)
{
	BYTE		*pBuf = m_KeyBuffer;
	WORD		VCode = 0;

	// 決定 //
	if(pBuf[DIK_RETURN] & 0x80){
		VCode |= KEY_SHOT;
	}

	return VCode;
}
