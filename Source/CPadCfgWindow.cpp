/*
 *   CPadCfgWindow.cpp   : パッドコンフィグ窓
 *
 */

#include "CPadCfgWindow.h"



// コンストラクタ //
CPadCfgWindow::CPadCfgWindow(CPadInput *pDevice) : CInputCfgWindow(pDevice)
{
	m_pPadInput = pDevice;
	m_pSurface  = NULL;

	OnDeviceChanged();
}


// デストラクタ //
CPadCfgWindow::~CPadCfgWindow()
{
	// 名前格納用 Surface を解放する //
	if(m_pSurface) g_pGrp->ReleaseOffsSurface(m_pSurface);
}


// 動作させる //
FVOID CPadCfgWindow::Move(void)
{
	if(g_pCursor->GetButtonState() & PBGMS_LEFTDOWN){
		OnLButtonDown();
	}

	MoveBaseWindow();

	if(IsActive()){
		if(m_pPadInput->OnChangeSetting(GetActiveItem())){
			ProceedNextItem();
		}
	}
}


// 描画する //
FVOID CPadCfgWindow::Draw(void)
{
	Pbg::CGrpSurface	*pSurface;
	RECT				src;
	int					sx, sy;

	// Surface が取得出来ないなら、 DrawBaseWindow も実行できないので //
	// ここでリターンするものとする                                   //
	pSurface = GetSurface();
	if(NULL == pSurface) return;

	// まずは、バックグラウンド＆ボタンを描画しましょう //
	DrawBaseWindow();

	sx     = GetWindowOx();		// ウィンドウ左上のＸ座標
	sy     = GetWindowOy();		// ウィンドウ左上のＹ座標

	SetRect2(&src, 0, 272, 264, 32);
	g_pGrp->BltC(&src, sx, sy, pSurface);

	if(m_pSurface){
		SetRect2(&src, 0, 0, m_CFont.GetWidth(), m_CFont.GetHeight());

		sx += (GetWindowWidth()-12)/2-m_CFont.GetWidth()/2;
		sy += 6;
		g_pGrp->BltC(&src, sx, sy, m_pSurface);
	}
}


// アクティブにする //
FVOID CPadCfgWindow::OnActivate(BOOL bActive)
{
	int			sx, sy;

	// 変更不能状態に推移させる //
	m_pPadInput->DisableChangeSetting();

	if(bActive){
		sx = GetWindowOx();
		sy = GetWindowOy();

		SetActiveItem(c_ButtonShot);
		g_pCursor->SetPosition(sx+149+78, sy+1+40+7);
	}
	else{
		SetActiveItem(-1);
	}
}


static const int g_KeyWindowWidth   = 264 + 12;	// ウィンドウの幅
static const int g_KeyWindowHeight  = 128;		// ウィンドウの高さ
static const int g_KeyWindowButtons = 4;		// 設定可能なボタンの数


// ウィンドウの幅 //
FINT CPadCfgWindow::GetWindowWidth(void)
{
	return g_KeyWindowWidth;
}


// ウィンドウの高さ //
FINT CPadCfgWindow::GetWindowHeight(void)
{
	return g_KeyWindowHeight;
}


// 設定可能なボタンの数 //
FINT CPadCfgWindow::GetNumButtons(void)
{
	return g_KeyWindowButtons;
}


// 次の項目に進む //
FVOID CPadCfgWindow::ProceedNextItem(void)
{
	int		ItemID;

	switch(GetActiveItem()){
		case c_ButtonShot:	ItemID = c_ButtonBomb;	break;	// ショット
		case c_ButtonBomb:	ItemID = c_ButtonShift;	break;	// ボム
		case c_ButtonShift:	ItemID = c_ButtonMenu;	break;	// 低速移動
		case c_ButtonMenu:	ItemID = c_ButtonShot;	break;	// メニュー

		default:
		return;
	}

	SetActiveItem(ItemID);
}


// 左ボタンが押し下げられた //
FVOID CPadCfgWindow::OnLButtonDown(void)
{
	POINT		Mouse;

	g_pCursor->GetPosition(&Mouse);

	Mouse.x -= GetWindowOx();
	Mouse.y -= GetWindowOy();

	if((Mouse.x < 0) || (Mouse.y < 0)
	|| (Mouse.x >= GetWindowWidth()) || (Mouse.y >= GetWindowHeight())){
		SetActiveItem(-1);
		return;
	}
}


// デバイスが変更された //
FVOID CPadCfgWindow::OnDeviceChanged(void)
{
	char		Name[128];

	// フォントに関する情報を初期化する //
	m_CFont.SetColor(RGB(255, 255, 255));			// 色
	m_CFont.SetBkBlendColor(RGB(196, 64, 64));		// 背景の合成色
	m_CFont.SetFontID(FONTID_MS_GOTHIC);			// フォント
	m_CFont.SetSize(12);							// サイズ

	m_pPadInput->GetDeviceName(Name);
	m_CFont.SetText(Name);

	if(NULL == m_pSurface){
		if(FALSE == g_pGrp->CreateSurface(&m_pSurface, 12*16, 12)){
			return;
		}
	}

	m_pSurface->LoadDIB32(&m_CFont, 0, 0);
	m_pSurface->SetColorKey(0);
}
