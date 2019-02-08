/*
 *   CInputCfgWindow.cpp   : 入力コンフィグ窓
 *
 */

#include "CInputCfgWindow.h"



// 描画用 Surface //
Pbg::CGrpSurface	**CInputCfgWindow::m_ppSurface = NULL;



// コンストラクタ //
CInputCfgWindow::CInputCfgWindow(CInputDevBase *pTarget)
{
	m_WindowOx   = 0;		// ウィンドウ右上のＹ座標
	m_WindowOy   = 0;		// ウィンドウ左上のＹ座標
	m_pTargetDev = pTarget;	// 対象となるデバイス
}


// デストラクタ //
CInputCfgWindow::~CInputCfgWindow()
{
	// 特に何もしません //
}


// ウィンドウ左上の座標を変更する //
FVOID CInputCfgWindow::SetWindowPos(int ox, int oy)
{
	m_WindowOx   = ox;		// ウィンドウ左上のＸ座標
	m_WindowOy   = oy;		// ウィンドウ左上のＹ座標
}


// コンフィグ窓用の Surface をセットする //
FVOID CInputCfgWindow::SetSurface(Pbg::CGrpSurface **ppSurface)
{
	m_ppSurface = ppSurface;
}


// 8x8フォントで文字列描画 //
FVOID CInputCfgWindow::Draw8x8Fonts(int dx, int dy, char *pStr)
{
	Pbg::CGrpSurface	*pSurface;
	RECT				src;

	pSurface = GetSurface();
	if(NULL == pSurface) return;

	// センタリングをかける //
	dx += m_WindowOx + 2 + (37 - strlen(pStr)*3);
	dy += m_WindowOy + 3;

	for(; pStr[0]!='\0'; pStr++, dx+=6){
		if((pStr[0] >= 'A') && (pStr[0] <= 'Z')){
			SetRect2(&src, 0+(pStr[0]-'A')*8, 152, 8, 8);
		}
		else if((pStr[0] >= '0' && pStr[0] <= '9')){
			SetRect2(&src, 208+(pStr[0]-'0')*8, 152, 8, 8);
		}
		else{
			continue;
		}

		g_pGrp->BltC(&src, dx, dy, pSurface);
	}
}


// アクティブな項目を描画 //
FVOID CInputCfgWindow::DrawActiveItemRect(int dx, int dy)
{
	D3DTLVERTEX		tlv[20];
	int				x1, x2, y, delta;
	DWORD			c1, c2;

	if(FALSE == g_pGrp->Begin3DScene()) return;

	// 選択中の項目をマークする //
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

	x1    = dx + m_WindowOx;
	x2    = dx + m_WindowOx + 79;
	y     = dy + m_WindowOy;
	delta = SinL(m_CursorAnime, 120) + 128;

	c2 = RGBA_MAKE(50, 100, 255, 248*2/3);
	c1 = RGBA_MAKE(50, 100, 255, delta*2/3);

	Set2DPointC(tlv+0, x1       , y,    0, 0, c1);
	Set2DPointC(tlv+1, (x1+x2)/2, y,    0, 0, c2);
	Set2DPointC(tlv+2, (x1+x2)/2, y+14, 0, 0, c2);
	Set2DPointC(tlv+3, x1       , y+14, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	Set2DPointC(tlv+0, (x1+x2)/2, y,    0, 0, c2);
	Set2DPointC(tlv+1, x2       , y,    0, 0, c1);
	Set2DPointC(tlv+2, x2       , y+14, 0, 0, c1);
	Set2DPointC(tlv+3, (x1+x2)/2, y+14, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	g_pGrp->End3DScene();
}


static RECT g_ShotRect  = {149,  1, 79, 14};
static RECT g_BombRect  = {157, 25, 79, 14};
static RECT g_ShiftRect = {165, 49, 79, 14};
static RECT g_MenuRect  = {173, 73, 79, 14};


// 窓の基本部分の描画 //
FVOID CInputCfgWindow::DrawBaseWindow(void)
{
	// 外枠の描画 //
	if(g_pGrp->Begin3DScene()){
		DrawClient();

		g_pGrp->End3DScene();
	}

	// デフォルトのボタンがアクティブなら、描画する //
	switch(m_ActiveItem){
		case c_ButtonShot:		// ショット
			DrawActiveButton(g_ShotRect.left, g_ShotRect.top);
		break;

		case c_ButtonBomb:		// ボム
			DrawActiveButton(g_BombRect.left, g_BombRect.top);
		break;

		case c_ButtonShift:		// 低速移動
			DrawActiveButton(g_ShiftRect.left, g_ShiftRect.top);
		break;

		case c_ButtonMenu:		//メニュー
			DrawActiveButton(g_MenuRect.left, g_MenuRect.top);
		break;
	}

	// デフォルト項目に割り当てられた キー or ボタン 名を描画 //
	DrawContents();
}


// 窓の基本部分の動作 //
FVOID CInputCfgWindow::MoveBaseWindow(void)
{
	POINT		Mouse;
	int			Button, dy, sx, sy;

	m_CursorAnime += 8;

	if(g_pCursor->GetButtonState() & PBGMS_LEFTDOWN){
		g_pCursor->GetPosition(&Mouse);

		Button = GetNumButtons();
		dy     = GetWindowHeight() - (Button * 24 - 8);

		Mouse.x -= m_WindowOx;
		Mouse.y -= (m_WindowOy + dy);

		sx = Mouse.x - g_ShotRect.left;
		sy = Mouse.y - g_ShotRect.top;
		if((sx >= 0) && (sy >= 0)
		&& (sx < g_ShotRect.right) && (sy < g_ShotRect.bottom)){
			SetActiveItem(c_ButtonShot);
			return;
		}

		sx = Mouse.x - g_BombRect.left;
		sy = Mouse.y - g_BombRect.top;
		if((sx >= 0) && (sy >= 0)
		&& (sx < g_BombRect.right) && (sy < g_BombRect.bottom)){
			SetActiveItem(c_ButtonBomb);
			return;
		}

		sx = Mouse.x - g_ShiftRect.left;
		sy = Mouse.y - g_ShiftRect.top;
		if((sx >= 0) && (sy >= 0)
		&& (sx < g_ShiftRect.right) && (sy < g_ShiftRect.bottom)){
			SetActiveItem(c_ButtonShift);
			return;
		}

		sx = Mouse.x - g_MenuRect.left;
		sy = Mouse.y - g_MenuRect.top;
		if((sx >= 0) && (sy >= 0)
		&& (sx < g_MenuRect.right) && (sy < g_MenuRect.bottom)){
			SetActiveItem(c_ButtonMenu);
			return;
		}
	}
}


// 窓部分の描画 //
FVOID CInputCfgWindow::DrawClient(void)
{
	D3DTLVERTEX	tlv[10];
	DWORD		c1, c2;
	int			x1, y1, x2, y2;

	x1 = m_WindowOx;
	y1 = m_WindowOy;
	x2 = x1 + GetWindowWidth();
	y2 = y1 + GetWindowHeight();


	c1 = RGBA_MAKE(0, 0, 64, 128+32);
	c2 = RGBA_MAKE(64, 64, 128, 128+32);
//	c1 = RGBA_MAKE(0, 0, 128, 128);
//	c2 = RGBA_MAKE(128, 128, 255, 128);

	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c1);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c2);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	c1 = RGBA_MAKE(196, 196, 196, 255);
	c2 = RGBA_MAKE(128, 128, 128, 255);

	x1--;	y1--;
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c2);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c1);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	Set2DPointC(tlv+4, x1, y1, 0, 0, c1);
	g_pGrp->SetRenderStateEx(GRPST_NORMAL);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

	x1-=2;	y1-=2;
	x2+=2;	y2+=2;
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c2);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c1);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	Set2DPointC(tlv+4, x1, y1, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

	Set2DPointC(tlv+0, x2  , y2 , 0, 0, c1);
	Set2DPointC(tlv+1, x2-2, y2-2, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_POINTLIST, tlv, 2);
}


// 内容物の描画 //
FVOID CInputCfgWindow::DrawContents(void)
{
	Pbg::CGrpSurface	*pSurface;
	RECT				src;
	int					sx, sy, DeltaY;
	char				ButtonName[128];

	if(NULL == m_pTargetDev) return;

	pSurface = GetSurface();
	if(NULL == pSurface) return;

	DeltaY = GetNumButtons() * 24 - 8;
	sx     = m_WindowOx;
	sy     = m_WindowOy + GetWindowHeight() - DeltaY;

	SetRect2(&src, 0, 312, 264, DeltaY);
	g_pGrp->BltC(&src, sx, sy, pSurface);

	switch(m_pTargetDev->GetDeviceID()){
	case IDEVID_FULLKEY:	SetRect2(&src, 280, 160, 8, 112);	break;
	case IDEVID_HALFKEY1P:	SetRect2(&src, 272, 160, 8, 112);	break;
	case IDEVID_HALFKEY2P:	SetRect2(&src, 264, 160, 8, 112);	break;
	case IDEVID_PAD1P:		SetRect2(&src, 296, 160, 8, 112);	break;
	case IDEVID_PAD2P:		SetRect2(&src, 288, 160, 8, 112);	break;
	}

	sx = m_WindowOx +  GetWindowWidth() - 12;
	sy = m_WindowOy + (GetWindowHeight() / 2) - (112 / 2);
	g_pGrp->BltC(&src, sx, sy, pSurface);


	sy = GetWindowHeight() - DeltaY;

	m_pTargetDev->GetButtonName(ButtonName, c_ButtonShot);
	Draw8x8Fonts(g_ShotRect.left, sy+g_ShotRect.top, ButtonName);

	m_pTargetDev->GetButtonName(ButtonName, c_ButtonBomb);
	Draw8x8Fonts(g_BombRect.left, sy+g_BombRect.top, ButtonName);

	m_pTargetDev->GetButtonName(ButtonName, c_ButtonShift);
	Draw8x8Fonts(g_ShiftRect.left, sy+g_ShiftRect.top, ButtonName);

	if(GetNumButtons() >= 4){
		m_pTargetDev->GetButtonName(ButtonName, c_ButtonMenu);
		Draw8x8Fonts(g_MenuRect.left, sy+g_MenuRect.top, ButtonName);
	}
}


// アクティブなボタンを描画 //
FVOID CInputCfgWindow::DrawActiveButton(int dx, int dy)
{
	int			DeltaY;

	DeltaY = GetWindowHeight() - (GetNumButtons() * 24 - 8);
	DrawActiveItemRect(dx, dy + DeltaY);
}
