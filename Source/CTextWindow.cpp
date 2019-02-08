/*
 *   CTextWindow.cpp   : テキスト用ウィンドウ
 *
 */

#include "CTextWindow.h"
#include "Gian2001.h"



/***** [スタティックメンバ] *****/
HelpStrTable *CTextWindow::m_pMainMenuHelp   = NULL;	// TWIN_MAINMENU 系のヘルプ
DWORD         CTextWindow::m_NumMainMenuHelp = 0;		// TWIN_MAINMENU の項目数
HelpStrTable *CTextWindow::m_pExitMenuHelp   = NULL;	// TWIN_EXITMENU 系のヘルプ
DWORD         CTextWindow::m_NumExitMenuHelp = 0;		// TWIN_EXITMENU の項目数

int           CTextWindow::m_RefCount        = 0;		// 参照数
BOOL          CTextWindow::m_bInitialized = FALSE;	// 初期化されているか



// コンストラクタ //
CTextWindow::CTextWindow()
{
	m_sx       = 0;		// ウィンドウ左上のＸ座標
	m_sy       = 0;		// ウィンドウ左上のＹ座標
	m_Width    = 0;		// ウィンドウの幅
	m_Height   = 0;		// ウィンドウの高さ
	m_FontSize = 0;		// フォントのサイズ

	m_pSurface = NULL;

	// フォントに関する情報を初期化する //
	m_CFont.SetColor(RGB(255, 255, 255));		// 色
	m_CFont.SetBkBlendColor(RGB(196, 64, 64));		// 背景の合成色
//	m_CFont.SetBkBlendColor(RGB(64, 0, 0));		// 背景の合成色
	m_CFont.SetFontID(FONTID_MS_GOTHIC);		// フォント
	m_CFont.SetSize(32);						// サイズ

	// ヘルプ文字列のロードを試みる //
	InitializeStaticData();
}


// デストラクタ //
CTextWindow::~CTextWindow()
{
//PbgError("hoge");
	// これをやらんと、死ぬよぉ //
	if(m_pSurface && g_pGrp){
//PbgError("hogehgoe");
		g_pGrp->ReleaseOffsSurface(m_pSurface);
	}
//PbgError("piyo");
	CleanupStaticData();
//PbgError("finished");
}


// 描画する //
FVOID CTextWindow::Draw(void)
{
	if(g_pGrp->Begin3DScene()){
		DrawClient();
		g_pGrp->End3DScene();
	}

	DrawContents();
}


// 絶対指定で座標を変更 //
FVOID CTextWindow::SetWindowPosA(int sx, int sy)
{
	m_sx = sx;		// ウィンドウの左上のＸ座標
	m_sy = sy;		// ウィンドウの左上のＹ座標
}


// 相対指定で座標を変更 //
FVOID CTextWindow::SetWindowPosR(int dx, int dy)
{
	m_sx += dx;		// ウィンドウの左上のＸ座標
	m_sy += dy;		// ウィンドウの左上のＹ座標
}


// ウィンドウのサイズを変更 //
FBOOL CTextWindow::Resize(int width, int height)
{
	// まず、格納用のＤＩＢ領域を確保する //
	if(FALSE == m_CFont.Create(width, height)) return FALSE;

	m_Width  = width;		// 幅
	m_Height = height;		// 高さ

	if(m_pSurface){	// すでに作成されている場合 //
		g_pGrp->ReleaseOffsSurface(m_pSurface);
		m_pSurface = NULL;
	}

	if(FALSE == g_pGrp->CreateSurface(&m_pSurface, width, height)){
		return FALSE;
	}

	m_pSurface->SetColorKey(0);
	return TRUE;
}


// フォントのサイズを変更 //
FBOOL CTextWindow::SetFontSize(int size)
{
	if(FALSE == m_CFont.SetSize(size)) return FALSE;

	m_FontSize = size;

	return TRUE;
}


// テキストを割り当てる //
FVOID CTextWindow::SetText(char *pString, BOOL bInsert)
{
	m_CFont.SetText(pString);
	m_pSurface->Cls();
	m_pSurface->LoadDIB32(&m_CFont);
}


// ヘルプ文字列を割り当てる //
FVOID CTextWindow::SetHelpText(DWORD ItemID, DWORD HelpID, BOOL bInsert)
{
	switch(ItemID){
		case TWIN_MAINMENU:
			if(NULL   == m_pMainMenuHelp)   break;
			if(HelpID >= m_NumMainMenuHelp) break;

			SetText(m_pMainMenuHelp[HelpID].Data, bInsert);
		return;

		case TWIN_EXITMENU:
			if(NULL   == m_pExitMenuHelp)   break;
			if(HelpID >= m_NumExitMenuHelp) break;

			SetText(m_pExitMenuHelp[HelpID].Data, bInsert);
		return;

		default:
		break;
	}

	SetText("Error : ヘルプ文字列の読み込みに失敗", bInsert);
}


// %s 文字列追加タイプでヘルプ文字列の割り当て //
FVOID CTextWindow::SetHelpTextEx(DWORD ItemID, DWORD HelpID, char *pStr)
{
	char	temp[MAX_PATH];
	char	*pExtra = "";

	// pStr のチェック //
	if((NULL == pStr) || ('\0' == pStr[0])){
		pStr = pExtra;
	}

	switch(ItemID){
		case TWIN_MAINMENU:
			if(NULL   == m_pMainMenuHelp)   break;
			if(HelpID >= m_NumMainMenuHelp) break;

			wsprintf(temp, m_pMainMenuHelp[HelpID].Data, pStr);
			SetText(temp, FALSE);
		return;

		case TWIN_EXITMENU:
			if(NULL   == m_pExitMenuHelp)   break;
			if(HelpID >= m_NumExitMenuHelp) break;

			wsprintf(temp, m_pExitMenuHelp[HelpID].Data, pStr);
			SetText(temp, FALSE);
		return;

		default:
		break;
	}

	SetText("Error : ヘルプ文字列の読み込みに失敗", TRUE);
}


// 静的データの確保 //
FVOID CTextWindow::InitializeStaticData(void)
{
	Pbg::LzDecode		Decode;

	char * const pMainMenuHelpID = "STRTABLE/MAINMENUHELP";
	char * const pExitMenuHelpID = "STRTABLE/EXITMENUHELP";

	// 参照カウントをインクリメント //
	m_RefCount += 1;

	// すでに初期化が完了している場合 //
	if(m_bInitialized) return;

	if(FALSE == Decode.Open("ENEMY.DAT")) return;

	// メインメニュー系ヘルプをロード //
	if(NULL == m_pMainMenuHelp){
		// 上から順に、データ解凍、項目数 //
		m_pMainMenuHelp   = (HelpStrTable *)Decode.Decode(pMainMenuHelpID);
		m_NumMainMenuHelp = Decode.GetFileSize(pMainMenuHelpID) / sizeof(HelpStrTable);
	}

	// 終了メニュー系ヘルプをロード //
	if(NULL == m_pExitMenuHelp){
		// 上から順に、データ解凍、項目数 //
		m_pExitMenuHelp   = (HelpStrTable *)Decode.Decode(pExitMenuHelpID);
		m_NumExitMenuHelp = Decode.GetFileSize(pExitMenuHelpID) / sizeof(HelpStrTable);
	}

	//////////////////////////////////////////////////////////////
	// この部分に他のヘルプの読み込みを記述する                 //
	// ただし、 CleanupStaticData() のほうの記述も忘れずに      //
	//////////////////////////////////////////////////////////////

	Decode.Close();

	m_bInitialized = TRUE;
}


// 静的データの解放 //
FVOID CTextWindow::CleanupStaticData(void)
{
	// 参照カウントをデクリメント //
	m_RefCount -= 1;

	// まだ参照しているオブジェクトが存在する //
	if(m_RefCount >= 1) return;

	// では、解放を //
	MemFree(m_pMainMenuHelp);	// メインメニュー用
	m_NumMainMenuHelp = 0;		//

	MemFree(m_pExitMenuHelp);
	m_NumExitMenuHelp = 0;

	m_bInitialized = FALSE;
}



// クライアント領域を描画 //
FVOID CTextWindow::DrawClient(void)
{
	D3DTLVERTEX	tlv[10];
	DWORD		c1, c2;
	int			x1, y1, x2, y2;

	x1 = m_sx;
	y1 = m_sy;
	x2 = x1 + m_Width;
	y2 = y1 + m_Height;


	c1 = RGBA_MAKE(0, 0, 64, 128+32);
	c2 = RGBA_MAKE(64, 64, 128, 128+32);
//	c1 = RGBA_MAKE(0, 0, 128, 128);
//	c2 = RGBA_MAKE(128, 128, 255, 128);

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
}


// 文字列等の描画(2D 部) //
FVOID CTextWindow::DrawContents(void)
{
	RECT		src;

	if(NULL == m_pSurface) return;

	SetRect(&src, 0, 0, m_Width, m_Height);
	g_pGrp->BltC(&src, m_sx, m_sy, m_pSurface);
}
