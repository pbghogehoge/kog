/*
 *   CKeyCfgWindow.cpp   : キーボードコンフィグ窓
 *
 */

#include "CKeyCfgWindow.h"



// 相対指定による方向キー割り当て用矩形 //
static RECT g_DirPosR[8] = {
	{ 93,   1, 79, 14},	{ 93,  89, 79, 14},
	{  6,  45, 79, 14},	{180,  45, 79, 14},
	{ 27,  21, 79, 14},	{159,  21, 79, 14},
	{ 27,  69, 79, 14},	{159,  69, 79, 14}
};



// コンストラクタ //
CKeyCfgWindow::CKeyCfgWindow(CKeyInput *pDevice) : CInputCfgWindow(pDevice)
{
	m_pKeyInput = pDevice;
}


// デストラクタ //
CKeyCfgWindow::~CKeyCfgWindow()
{
}


// 動作させる //
FVOID CKeyCfgWindow::Move(void)
{
	POINT		Mouse;

	if(g_pCursor->GetButtonState() & PBGMS_LEFTDOWN){
		g_pCursor->GetPosition(&Mouse);
		OnLButtonDown(Mouse.x, Mouse.y);
	}

	MoveBaseWindow();

	if(IsActive()){
		if(m_pKeyInput->OnChangeSetting(GetActiveItem())){
			ProceedNextItem();
		}
	}
}


// 描画する //
FVOID CKeyCfgWindow::Draw(void)
{
	Pbg::CGrpSurface	*pSurface;
	RECT				src;
	char				KeyName[128];
	int					sx, sy, ItemID;
	int					i;

	// Surface が取得出来ないなら、 DrawBaseWindow も実行できないので //
	// ここでリターンするものとする                                   //
	pSurface = GetSurface();
	if(NULL == pSurface) return;

	// まずは、バックグラウンド＆ボタンを描画しましょう //
	DrawBaseWindow();

	// アクティブな項目があれば、光らせる //
	ItemID = GetActiveItem();
	if((ItemID >= 0) && (ItemID <= 7)){
		DrawActiveItemRect(g_DirPosR[ItemID].left, g_DirPosR[ItemID].top);
	}

	sx     = GetWindowOx();		// ウィンドウ左上のＸ座標
	sy     = GetWindowOy();		// ウィンドウ左上のＹ座標

	SetRect2(&src, 0, 160, 264, 104);
	g_pGrp->BltC(&src, sx, sy, pSurface);

	for(i=0; i<8; i++){
		m_pKeyInput->GetButtonName(KeyName, i);
		Draw8x8Fonts(g_DirPosR[i].left, g_DirPosR[i].top, KeyName);
	}
}


// アクティブにする //
FVOID CKeyCfgWindow::OnActivate(BOOL bActive)
{
	int			sx, sy;

	// 変更不能状態に推移させる //
	m_pKeyInput->DisableChangeSetting();

	if(bActive){
		sx = GetWindowOx();
		sy = GetWindowOy();

		SetActiveItem(c_DirUp);
		g_pCursor->SetPosition(sx+93+78, sy+1+7);
	}
	else{
		SetActiveItem(-1);
	}
}


static const int g_KeyWindowWidth   = 264+12;	// ウィンドウの幅
static const int g_KeyWindowHeight  = 176;		// ウィンドウの高さ
static const int g_KeyWindowButtons = 3;		// 設定可能なボタンの数


// ウィンドウの幅 //
FINT CKeyCfgWindow::GetWindowWidth(void)
{
	return g_KeyWindowWidth;
}


// ウィンドウの高さ //
FINT CKeyCfgWindow::GetWindowHeight(void)
{
	return g_KeyWindowHeight;
}


// 設定可能なボタンの数 //
FINT CKeyCfgWindow::GetNumButtons(void)
{
	return g_KeyWindowButtons;
}


// 次の項目に進む //
FVOID CKeyCfgWindow::ProceedNextItem(void)
{
	int		ItemID;

	switch(GetActiveItem()){
	case c_DirUp:			ItemID = c_DirDown;			break;	//  上  方向への移動
	case c_DirDown:			ItemID = c_DirLeft;			break;	//  下  方向への移動
	case c_DirLeft:			ItemID = c_DirRight;		break;	//  左  方向への移動
	case c_DirRight:		ItemID = c_DirUpLeft;		break;	//  右  方向への移動
	case c_DirUpLeft:		ItemID = c_DirUpRight;		break;	// 左上 方向への移動
	case c_DirUpRight:		ItemID = c_DirDownLeft;		break;	// 右上 方向への移動
	case c_DirDownLeft:		ItemID = c_DirDownRight;	break;	// 左下 方向への移動
	case c_DirDownRight:	ItemID = c_ButtonShot;		break;	// 右下 方向への移動
	case c_ButtonShot:		ItemID = c_ButtonBomb;		break;	// ショット
	case c_ButtonBomb:		ItemID = c_ButtonShift;		break;	// ボム
	case c_ButtonShift:		ItemID = c_DirUp;			break;	// 低速移動
	}

	SetActiveItem(ItemID);
}


// 左ボタンが上がったときの処理 //
FVOID CKeyCfgWindow::OnLButtonDown(int mx, int my)
{
	int			i;
	int			dx, dy;

	mx -= GetWindowOx();
	my -= GetWindowOy();

	if((mx < 0) || (my < 0)
	|| (mx >= GetWindowWidth()) || (my >= GetWindowHeight())){
		SetActiveItem(-1);
		return;
	}

	for(i=0; i<8; i++){
		dx = mx - g_DirPosR[i].left;
		dy = my - g_DirPosR[i].top;

		// 範囲内に存在していれば、その項目を有効にする //
		if((dx >= 0) && (dy >= 0)
		&& (dx < g_DirPosR[i].right) && (dy < g_DirPosR[i].bottom)){
			SetActiveItem(i);
			return;
		}
	}
}
