/*
 *   CSelectWindow.cpp   : 選択窓
 *
 */

#include "CSelectWindow.h"
#include "SECtrl.h"

#define PBGWND_FONTSIZE			(16+4)
#define PBGWND_DEFAULTWIDTH		256




// コンストラクタ //
CSelectWindow::CSelectWindow(CGameInfo *pSys)
{
	m_ActiveItem  = 0;		// 現在アクティブな項目
	m_NumItems    = 0;		// 全項目数
	m_sx          = 0;		// 左上のＸ座標
	m_sy          = 0;		// 左上のＹ座標
	m_CursorAnime = 0;		// カーソルアニメ用カウンタ

	m_pInfo          = NULL;	// 情報構造体へのポインタ
	m_pCallBack      = NULL;	// コールバック関数へのポインタ
	m_pIsEnableItem  = NULL;	// 項目が選択できれば真
	m_aCFont         = NULL;	// フォント管理クラスの配列
	m_pSurface       = NULL;	// 項目表示用 Surface

	// コールバック関数のシステム呼び出しの為に接続 //
	m_pSys = pSys;

	// デフォルトの横幅 //
	m_Width = PBGWND_DEFAULTWIDTH;

	// ウィンドウは閉じている //
	m_WindowState = PBGWND_CLOSED;

	m_bNeedUpdate    = FALSE;	// 更新フラグ
	m_bMouseCaptured = FALSE;	// マウスキャプチャーフラグ
}


// デストラクタ //
CSelectWindow::~CSelectWindow()
{
	// 注意： m_pInfo は解放してはならない //

	// これをやらんと、死ぬ //
	if(m_pSurface && g_pGrp){
		g_pGrp->ReleaseOffsSurface(m_pSurface);
	}

	// 選択可能フラグを破棄する //
	if(m_pIsEnableItem) DeleteEx(m_pIsEnableItem);

	// フォント管理クラスを破棄する //
	if(m_aCFont) DeleteArray(m_aCFont);
}


// １フレームごとの更新 //
void CSelectWindow::Move(void)
{
	if(PBGWND_DISABLE == m_WindowState){
		if(m_ActiveItem >= m_NumItems) return;
		if(m_pInfo[m_ActiveItem].m_pChild){
			m_pInfo[m_ActiveItem].m_pChild->Move();
		}
		return;
	}

	m_CursorAnime += 8;
}


// このウィンドウとその子ウィンドウを描画する //
void CSelectWindow::Draw(void)
{
	int		i, n;

	// 閉じている場合は、何もしません //
	if(PBGWND_CLOSED == m_WindowState) return;

	if(g_pGrp->Begin3DScene()){
		DrawClient();
		g_pGrp->End3DScene();
	}
	DrawContents();

	n = m_NumItems;

	for(i=0; i<n; i++){
		// 親項目の場合、描画が必要かもしれん //
		if(PBGITM_PARENT == m_pInfo[i].m_ItemFlag){
			// ポインタが有効なら、呼び出し //
			if(m_pInfo[i].m_pChild) m_pInfo[i].m_pChild->Draw();
		}
	}
}


// 親ウィンドウからの相対指定で移動する //
void CSelectWindow::SetWindowPos(int sx, int sy)
{
	m_sx = sx;		// 左上のＸ座標
	m_sy = sy;		// 左上のＹ座標
}


// マウスが動いたとき //
BOOL CSelectWindow::OnMouseMove(POINT *pMouse, BYTE MouseState)
{
	if(PBGWND_DISABLE == m_WindowState){
		if(m_ActiveItem >= m_NumItems) return FALSE;
		if(m_pInfo[m_ActiveItem].m_pChild){
			return m_pInfo[m_ActiveItem].m_pChild->OnMouseMove(pMouse, MouseState);
		}

		if(MouseState & PBGMS_RIGHTDOWN) return OnRButtonDown();
		return FALSE;
	}

	// ドラッグ状態の推移 //
	if(!(MouseState & PBGMS_LEFTDRAG)) m_bMouseCaptured = FALSE;

	// 以下の動作は、キャプチャ中の動作を認めない //
	if(FALSE == m_bMouseCaptured){
		if(MouseState & PBGMS_RIGHTDOWN) return OnRButtonDown();
		OnLButtonCheck(pMouse);
	}

	if(MouseState & PBGMS_LEFTDRAG){
		return OnLButtonDrag(pMouse);
	}

	return TRUE;
}


// 通知用関数 : 何かキーが押されて、離されたとき //
BOOL CSelectWindow::OnKeyDown(WORD KeyCode)
{
	PBG_ITEMINFO	*pInfo;
	PBGWND_RET		ret;
	BYTE			ItemID;
	BYTE			NumItems;
	int				i, x, y;
	WORD			MouseState;

	// キャプチャ中の動作は認めない //
	if(m_bMouseCaptured) return FALSE;

	if(NULL == m_pInfo) return FALSE;			// 内部エラー
	if(NULL == m_pCallBack) return FALSE;		// コールバック関数が使用できず
	if(0 == m_NumItems) return FALSE;			// 項目数がおかしい

	// 状態に応じて推移する //
	switch(m_WindowState){
	case PBGWND_DISABLE:
		if(m_ActiveItem >= m_NumItems) return FALSE;
		if(m_pInfo[m_ActiveItem].m_pChild){
			return m_pInfo[m_ActiveItem].m_pChild->OnKeyDown(KeyCode);
		}
	break;
//	return FALSE;

	case PBGWND_CLOSED:
	return FALSE;

	case PBGWND_ACTIVE:
		// ウィンドウはアクティブだが、項目がアクティブじゃない //
		if(m_ActiveItem < m_NumItems) break;	// このときは必要なし

		// キーボードの入力の場合 //
		if(0 == g_pCursor->GetButtonState()){
			if(0 == KeyCode) return FALSE;

			m_ActiveItem = 0;
			OnActivate();

			if(KeyCode != KEY_MENU) KeyCode = 0;
			//KeyCode &= ~(KEY_UP | KEY_DOWN);
		}
		// マウスの入力で、かつキャンセルの場合 //
		else{
			if(KeyCode & (KEY_MENU|KEY_BOMB)){
				if(m_pParent){
					//PlaySysSound(1);

					// 親ウィンドウをアクティブにする //
					m_pParent->OnActivate();
					OnClose();	// 自分を閉じる
				}
			}

			return TRUE;
		}
	break;
	}

	ItemID   = m_ActiveItem;		// アクティブな項目は？
	NumItems = m_NumItems;			// 全項目数
	pInfo    = m_pInfo + ItemID;	// メッセージの対象を選択！

	// 項目が有効な場合 //
	if(TRUE == m_pIsEnableItem[ItemID]){
		// ウィンドウ固有の処理が受理できたなら、正常終了 //
		ret = m_pCallBack(m_pSys, ItemID, KeyCode);

		switch(ret){
		case RET_UPDATE:
			UpdateItemFont(ItemID);

		case RET_OK:
//			if(KeyCode & KEY_OK) PlaySysSound(0);
			if(KeyCode & KEY_SHOT) PlaySE(SNDID_CANCEL);
			UpdateHelpStr();

		case RET_NONUPDATE:
		return TRUE;

		case RET_CLOSE:
			if(NULL == m_pParent){
				if(KeyCode & KEY_SHOT)     PlaySE(SNDID_CANCEL);
//				if(KeyCode & KEY_OK)     PlaySysSound(0);
//				if(KeyCode & KEY_MENUL)  PlaySysSound(1);
			}

			KeyCode = KEY_MENU;
		break;

		case RET_ACTIVATE:
			OnActivate();
		return TRUE;
		}
	}

	// ＥＸＩＴ用項目の場合、横取り... //
	if(PBGITM_EXIT == pInfo->m_ItemFlag){
		if(KEY_SHOT == KeyCode) KeyCode = KEY_MENU;
	}

	// ここからは、デフォルトの動作である         //
	// break 抜けすると、ヘルプ文字列が更新される //
	switch(KeyCode){
	case KEY_UP:	// 前の項目へ
//		PlaySysSound(2);
		for(i=0; i<NumItems; i++){	// 無効化されている項目はスキップ
			ItemID = (ItemID + NumItems - 1) % NumItems;	// 前へ
			if(TRUE == m_pIsEnableItem[ItemID]) break;
		}
		m_ActiveItem = ItemID;
	break;

	case KEY_DOWN:	// 次の項目へ
//		PlaySysSound(2);
		for(i=0; i<NumItems; i++){	// 無効化されている項目はスキップ
			ItemID = (ItemID + 1) % NumItems;	// 次へ
			if(TRUE == m_pIsEnableItem[ItemID]) break;
		}
		m_ActiveItem = ItemID;
	break;

	case KEY_SHOT:	// 決定
//		PlaySysSound(0);
		PlaySE(SNDID_CANCEL);

		// 利用不可能な項目に対するアクセス //
		if(FALSE == m_pIsEnableItem[ItemID]) break;

		// 親項目なら、子ウィンドウをアクティブにする ... //
		if(PBGITM_PARENT == pInfo->m_ItemFlag){
			// 通常の親の場合(子が存在する) //
			if(pInfo->m_pChild){
				NumItems = pInfo->m_pChild->m_NumItems;
				for(i=0; i<NumItems; i++){
					// 子項目に空メッセージを送る //
					if(RET_UPDATE == pInfo->m_pChild->m_pCallBack(m_pSys, i, 0)){
						//PbgError(pInfo->m_pChild->m_pInfo[i].m_ItemName);
						pInfo->m_pChild->UpdateItemFont(i);
					}
				}

				pInfo->m_pChild->OnActivate();
			}

			OnDisable();	// 自分を無効化する
		}
	return TRUE;		// ヘルプ文字列の更新を抑制

	case KEY_BOMB:	case KEY_MENU:	// キャンセル
		if(m_pParent){
//			PlaySysSound(1);
			PlaySE(SNDID_CANCEL);

			// コールバック関数が生きていれば、空メッセージを送る //
			if(m_pParent->m_pCallBack){
				ItemID = m_pParent->m_ActiveItem;
				m_pParent->m_pCallBack(m_pSys, ItemID, 0);
				m_pParent->UpdateItemFont(ItemID);
			}

			// 親ウィンドウをアクティブにする //
			m_pParent->OnActivate();
			OnClose();	// 自分を閉じる
		}
	return TRUE;	// ヘルプ文字列を更新してはイカン

	default:		// それ以外
	return TRUE;
	}

	if(KeyCode & (KEY_UP | KEY_DOWN)){
		MouseState = g_pCursor->GetButtonState();
		if(0 == MouseState || (MouseState & PBGMS_LEFTDOWN)){
			x = GetCurrentX() + m_Width - PBGWND_FONTSIZE/2;
			y = GetCurrentY() + m_ActiveItem * PBGWND_FONTSIZE + PBGWND_FONTSIZE/2;

			g_pCursor->SetPosition(x, y);
		}
	}

	UpdateHelpStr();

	return TRUE;
}


// そのままの設定でアクティブにする //
void CSelectWindow::OnActivate(void)
{
	int		i, n;
	int		x, y;

	n = m_NumItems;

	// 選択可能な項目のうち、一番上の奴を探索 //
	for(i=0; i<n; i++){
		if(m_pIsEnableItem[m_ActiveItem]) break;
		m_ActiveItem = (m_ActiveItem + 1) % n;
	}

	m_WindowState = PBGWND_ACTIVE;

	// ヘルプ文字列を更新する //
	UpdateHelpStr();

	x = GetCurrentX() + m_Width - PBGWND_FONTSIZE/2;
	y = GetCurrentY() + m_ActiveItem * PBGWND_FONTSIZE + PBGWND_FONTSIZE/2;

	g_pCursor->SetPosition(x, y);
}


// ウィンドウの選択を無効化する //
void CSelectWindow::OnDisable(void)
{
	m_WindowState    = PBGWND_DISABLE;
	m_bMouseCaptured = FALSE;
}


// このウィンドウを閉じる //
void CSelectWindow::OnClose(void)
{
	// この部分に「ウィンドウを閉じている途中」という //
	// 状態を追加したいのだが、今のところはいい       //
	if(m_pParent){
		m_pParent->OnActivate();
	}

	m_ActiveItem     = 0;
	m_WindowState    = PBGWND_CLOSED;
	m_bMouseCaptured = FALSE;
}


// 項目をセットする //
BOOL CSelectWindow::
	SetItem(CSelectWindow		*pParent	// 親ウィンドウ
		  , PBGWND_CALLBACK		pCallBack	// コールバック関数
		  , PBG_ITEMINFO		*pItemInfo	// 項目情報
		  , BYTE				NumItems	// 項目数
		  , int					Width		// 横幅
		  , DWORD				ItemID)		// ヘルプ項目ＩＤ
{
	int			i;

	// 動的確保オブジェクトの破棄 //
	if(m_pIsEnableItem) DeleteEx(m_pIsEnableItem);	// 選択可能フラグ
	if(m_aCFont)        DeleteArray(m_aCFont);		// フォント管理

	// 項目情報が存在しない場合は、無効化する //
	if(NULL == pItemInfo){
		m_pInfo     = pItemInfo;	// 項目情報
		NumItems    = 0;			// 項目数
		m_pParent   = NULL;			// 親ウィンドウ
		m_pCallBack = NULL;			// コールバック関数へのポインタ

		OnDisable();	// 無効化..
		return TRUE;	// 正常終了
	}

	// 項目数がおかしい //
	if(0 == NumItems) return FALSE;

	// コールバック関数は必ず必要である //
	if(NULL == pCallBack) return FALSE;

	if(FALSE == CreateSurface(Width, NumItems)) return FALSE;

	// 選択可能フラグ用のメモリを確保する //
	m_pIsEnableItem = NewEx(BOOL[NumItems]);
	if(NULL == m_pIsEnableItem) return FALSE;

	// フォント管理クラス用のメモリを確保する //
	m_aCFont = NewEx(Pbg::CFont[NumItems]);
	if(NULL == m_aCFont) return FALSE;

	// 基本パラメータの設定 //
	m_pParent    = pParent;		// 親ウィンドウ
	m_pCallBack  = pCallBack;	// コールバック関数
	m_NumItems   = NumItems;	// 現在の項目数
	m_ActiveItem = 0;			// 現在アクティブな項目
	m_sx         = 0;			// 左上のＸ座標
	m_sy         = 0;			// 右上のＹ座標
	m_pInfo      = pItemInfo;	// 項目の情報
	m_ItemID     = ItemID;		// 項目格納先のＩＤ

	// 全アイテムを有効にし、フォントの接続を行う //
	for(i=0; i<NumItems; i++){
		EnableItem(i, TRUE);
	}

	// ヘルプ文字列を更新する //
	UpdateHelpStr();

	// 閉じた状態にしておく //
	OnClose();

	return TRUE;
}


// 項目フォントを更新する                                  //
// PBG_ITEMINFO::m_ItemName を変更したら呼び出す必要がある //
void CSelectWindow::UpdateItemFont(DWORD ItemID)
{
	Pbg::CFont	*pFont;

	if(NULL == m_pIsEnableItem) return;	// 配列がありません
	if(ItemID >= m_NumItems) return;	// 項目番号が不正です
	if(NULL == m_aCFont) return;		// フォントがありません
	if(NULL == m_pInfo) return;			// 表示情報がない

	// 作業対象をローカルなポインタへ //
	pFont = &m_aCFont[ItemID];

	// フォントに関する情報を初期化する //
	if(m_pIsEnableItem[ItemID]){
		pFont->SetColor(RGB(255, 255, 255));
	}
	else{	// 無効になっている場合 //
		pFont->SetColor(RGB(128, 128, 128));
	}

	pFont->SetBkBlendColor(RGB(196, 64, 64));		// 背景の合成色
	//pFont->SetBkBlendColor(RGB(64, 0, 0));	// 背景の合成色
	pFont->SetFontID(FONTID_MS_GOTHIC);		// フォント
	pFont->SetSize(PBGWND_FONTSIZE);		// サイズ

	pFont->SetText(m_pInfo[ItemID].m_ItemName);
	pFont->Update();

	m_bNeedUpdate = TRUE;
}


// 項目を 有効(TRUE) or 無効(FALSE) にする //
void CSelectWindow::EnableItem(DWORD ItemID, BOOL bSetEnable)
{
	if(NULL == m_pIsEnableItem) return;	// 配列がありません
	if(ItemID >= m_NumItems) return;	// 項目番号が不正

	// フラグをセット //
	m_pIsEnableItem[ItemID] = bSetEnable;

	// フォントを更新 //
	UpdateItemFont(ItemID);
}


// 通知用関数 :マウスの右ボタンが離されたとき //
BOOL CSelectWindow::OnLButtonCheck(POINT *pMouse)
{
	int		x, y;
	int		w, h;

	x = pMouse->x - GetCurrentX();
	y = pMouse->y - GetCurrentY();
	w = m_Width;
	h = PBGWND_FONTSIZE * m_NumItems;

	// 完全に範囲外 //
	if(x < 0 || x > w || y < 0 || y > h){
		// 前回、アクティブだったなら、キャンセルメッセージを送出する //
		if(m_ActiveItem < m_NumItems){
			if(m_pCallBack) m_pCallBack(m_pSys, m_ActiveItem, KEY_MENU);
			g_pTextWindow->SetText("", FALSE);
			m_ActiveItem = m_NumItems;
		}
		return FALSE;
	}

	y /= PBGWND_FONTSIZE;
	if((y >= m_NumItems) || (FALSE == m_pIsEnableItem[y])){
		m_ActiveItem = m_NumItems;
		g_pTextWindow->SetText("", FALSE);
		return FALSE;
	}

	if(y != m_ActiveItem){
		// 自動移動中の選択は行えない //
		if(g_pCursor->IsAutoMove()) return FALSE;

		m_ActiveItem = y;

		// ヘルプ文字列を更新する //
		UpdateHelpStr();

		return TRUE;
	}

	if((g_pCursor->GetButtonState()) & PBGMS_LEFTUP){
		return OnKeyDown(KEY_SHOT);
	}

	return TRUE;
}


// 左ボタンドラッグ //
BOOL CSelectWindow::OnLButtonDrag(POINT *pMouse)
{
	int		x, y;
	int		w;//, h;
	int		rmin, rmax;
	int		cx, sx;//, dx;

	sx = GetCurrentX() + m_Width - (80+4) - 4;
	x  = pMouse->x - sx;
	y  = m_ActiveItem;
	w  = 80 + 4;

	// 完全に範囲外 //
	if(FALSE == m_bMouseCaptured){
		if(x < 0 || x > w || y >= m_NumItems){
			return FALSE;
		}

		m_bMouseCaptured = TRUE;
	}

	rmin = m_pInfo[y].m_RangeInfo.m_Min;
	rmax = m_pInfo[y].m_RangeInfo.m_Max;

	cx = rmin + (pMouse->x - sx) * (rmax - rmin) / (80+4);

	if((cx >= rmin) && (cx <= rmax)){
		if(abs(cx - m_pInfo[y].m_RangeInfo.m_Current) > 8){
			// ４：１２に内分する点を求める //
			cx = (4*cx + 12*m_pInfo[y].m_RangeInfo.m_Current) / 16;
		}
	}

	m_pInfo[y].m_RangeInfo.m_Current = cx;
	m_pInfo[y].m_RangeInfo.SeekR(0);

	if(m_pCallBack) m_pCallBack(m_pSys, y, 0);

	return TRUE;
}


// マウスの右ボタンが離されたとき //
BOOL CSelectWindow::OnRButtonDown(void)
{
	// 結局、キーダウンと同じ //
	return OnKeyDown(KEY_MENU);
}


// ウィンドウの横幅を決める (CreateSurface を含みます) //
BOOL CSelectWindow::CreateSurface(int Width, int NumItems)
{
	int		w, h;

	if(0 == NumItems) return FALSE;

	if(Width <= 0) m_Width = PBGWND_DEFAULTWIDTH;
	else           m_Width = Width;

	if(m_pSurface){	// すでに作成されている場合 //
		g_pGrp->ReleaseOffsSurface(m_pSurface);
		m_pSurface = NULL;
	}

	// 幅＆高さ //
	w = m_Width;
	h = PBGWND_FONTSIZE * NumItems;

	if(FALSE == g_pGrp->CreateSurface(&m_pSurface, m_Width, h)){
		return FALSE;
	}

	m_pSurface->SetColorKey(0);

	return TRUE;
}


// ヘルプ文字列を更新する //
void CSelectWindow::UpdateHelpStr(void)
{
	g_pTextWindow->SetHelpTextEx(m_ItemID
							   , m_pInfo[m_ActiveItem].m_HelpID
							   , m_pInfo[m_ActiveItem].m_HelpOption);
}


// クライアント領域の描画 //
void CSelectWindow::DrawClient(void)
{
	D3DTLVERTEX	tlv[10];
	DWORD		c1, c2, c3;
	int			x1, y1, x2, y2;
	int			sx, sy;
	int			rmax, rmin;
	int			i, n;

	if(0 == m_NumItems) return;

	// 状態に応じて描画する //
	switch(m_WindowState){
		case PBGWND_CLOSED:
		return;

		case PBGWND_DISABLE:
		case PBGWND_ACTIVE:
		break;
	}

	c1 = RGBA_MAKE(0, 0, 64, 128+32);
	c2 = RGBA_MAKE(0, 64, 128, 128+32);

	sx = GetCurrentX();
	sy = GetCurrentY();
	x1 = sx;
	y1 = sy;
	x2 = sx + m_Width;
	y2 = sy + PBGWND_FONTSIZE * m_NumItems;

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);

	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c1);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c2);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	c1 = RGBA_MAKE(196, 196, 196, 255);
	c2 = RGBA_MAKE(128, 128, 128, 255);

	g_pGrp->SetRenderStateEx(GRPST_NORMAL);

	x1--;y1--;
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c2);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c1);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	Set2DPointC(tlv+4, x1, y1, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

	x1-=2;y1-=2;
	x2+=2;y2+=2;
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c2);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c1);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	Set2DPointC(tlv+4, x1, y1, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

	Set2DPointC(tlv+0, x2  , y2 , 0, 0, c1);
	Set2DPointC(tlv+1, x2-2, y2-2, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_POINTLIST, tlv, 2);

	if((m_ActiveItem < m_NumItems)
	&& (TRUE == m_pIsEnableItem[m_ActiveItem])){
		// 選択中の項目をマークする //
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

		y1 = sy + PBGWND_FONTSIZE * m_ActiveItem + PBGWND_FONTSIZE/2;
		x1 = sx + 1;
		x2 = sx + m_Width - 1;
		y2 = SinL(m_CursorAnime, 120) + 128;

		if(PBGWND_ACTIVE != m_WindowState){
			c2 = RGBA_MAKE(255, 120, 25, 200*2/3);
			c1 = RGBA_MAKE(255, 120, 25, 96*2/3);
		}
		else{
			c2 = RGBA_MAKE(50, 100, 255, 248*2/3);
			c1 = RGBA_MAKE(50, 100, 255, y2*2/3);
		}

		Set2DPointC(tlv+0, x1-1     , y1-9, 0, 0, c1);
		Set2DPointC(tlv+1, (x1+x2)/2, y1-9, 0, 0, c2);
		Set2DPointC(tlv+2, (x1+x2)/2, y1+9, 0, 0, c2);
		Set2DPointC(tlv+3, x1-1     , y1+9, 0, 0, c1);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		Set2DPointC(tlv+0, (x1+x2)/2, y1-9, 0, 0, c2);
		Set2DPointC(tlv+1, x2+1     , y1-9, 0, 0, c1);
		Set2DPointC(tlv+2, x2+1     , y1+9, 0, 0, c1);
		Set2DPointC(tlv+3, (x1+x2)/2, y1+9, 0, 0, c2);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->SetRenderStateEx(GRPST_NORMAL);

	n = m_NumItems;
	for(i=0; i<n; i++){
		if(PBGITM_RANGE != m_pInfo[i].m_ItemFlag) continue;

		y1 = sy + PBGWND_FONTSIZE * i + PBGWND_FONTSIZE/2;
		x1 = sx + m_Width - (80 + 4) - 4;
		x2 = sx + m_Width - 4;

		if(m_pIsEnableItem[i]){
			c1 = RGBA_MAKE(  0,   0, 128, 255);
			c2 = RGBA_MAKE(220, 220, 255, 255);
			c3 = RGBA_MAKE(200, 200, 200, 255);
		}
		else{
			c1 = RGBA_MAKE( 64,  64,  64, 255);
			c2 = RGBA_MAKE(120, 120, 120, 255);
			c3 = RGBA_MAKE(100, 100, 100, 255);
		}

		Set2DPointC(tlv+0, x1, y1-4, 0, 0, c1);
		Set2DPointC(tlv+1, x2, y1-4, 0, 0, c2);
		Set2DPointC(tlv+2, x2, y1+4, 0, 0, c2);
		Set2DPointC(tlv+3, x1, y1+4, 0, 0, c1);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		rmax = m_pInfo[i].m_RangeInfo.m_Max;
		rmin = m_pInfo[i].m_RangeInfo.m_Min;
		x2 = (m_pInfo[i].m_RangeInfo.m_Current - rmin) * 80;
		x2 = x1 + x2 / max(1, abs(rmax - rmin));

		Set2DPointC(tlv+0, x2+0, y1-9, 0, 0, c3);
		Set2DPointC(tlv+1, x2+0, y1+9, 0, 0, c3);
		Set2DPointC(tlv+2, x2+4, y1-9, 0, 0, c3);
		Set2DPointC(tlv+3, x2+4, y1+9, 0, 0, c3);
		g_pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, 4);
	}
}


// 内容物の描画 //
void CSelectWindow::DrawContents(void)
{
	RECT		src;
	int			i, dx, ox;
	int			sx, sy;

	if(NULL == m_pSurface) return;	// Surface 管理
	if(NULL == m_aCFont) return;	// フォント管理

	// 閉じているなら、表示の必要が無い //
	if(PBGWND_CLOSED == m_WindowState) return;

	sx = GetCurrentX();
	sy = GetCurrentY();

	// 更新要求フラグが立っている場合 //
	if(m_bNeedUpdate){
		m_pSurface->Cls();

		ox = m_Width / 2;
		for(i=0; i<m_NumItems; i++){
			if(PBGITM_RANGE == m_pInfo[i].m_ItemFlag){
				dx = 2;
			}
			else{
				dx = ox - m_aCFont[i].GetWidth() / 2;
			}

			m_pSurface->LoadDIB32(&m_aCFont[i], dx, i * PBGWND_FONTSIZE);
		}

		m_bNeedUpdate = FALSE;
	}

	SetRect(&src, 0, 0, m_Width, m_NumItems * PBGWND_FONTSIZE);
	g_pGrp->BltC(&src, sx, sy, m_pSurface);
}


// 現在のＸ座標を取得する //
int CSelectWindow::GetCurrentX(void)
{
	if(m_pParent) return m_sx + m_pParent->GetCurrentX();
	else          return m_sx;
}


// 現在のＹ座標を取得する //
int CSelectWindow::GetCurrentY(void)
{
	if(m_pParent) return m_sy + m_pParent->GetCurrentY();
	else          return m_sy;
}
