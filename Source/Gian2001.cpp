/*
 *   Gian2001.cpp   : ゲームの定数定義
 *
 */

#include "Gian2001.h"
#include "WinMain.h"
#include "PbgMain.cpp"
#include "CGameInfo.h"
#include "CSelectWindow.h"



// グローバル変数 //
Pbg::CGraphic		*g_pGrp        = NULL;
Pbg::CGrpSurface	*g_pSystemSurf = NULL;
Pbg::CInput			*g_pInp        = NULL;

CAnmCursor			*g_pCursor       = NULL;
CTextWindow			*g_pTextWindow   = NULL;
CGameInfo			*g_pGameInfo     = NULL;

CMusicCtrl			*g_pMusic = NULL;

//CCfgIO				*g_pConfig = NULL;



extern void XMainLoop(void)
{
	if(GetAsyncKeyState(VK_F12) & 0x8000){
//	if((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON))& 0x8000){
//PbgError("Capture");
		g_pGameInfo->OnCapture();
	}

	if(FALSE == g_pGameInfo->UpdateGame()){
		PostMessage(g_hMainWindow, WM_CLOSE, 0, 0);
		return;
	}
}


// 初期化関数 //
extern BOOL XInitialize(CCfgIO *pConfig)
{
/*
	g_pConfig = NewEx(CCfgIO);
	if(NULL == g_pConfig){
		PbgError("メモリがたりないのぉ(Config)");
		return FALSE;
	}
*/
	g_pInp = NewEx(Pbg::CInput);
	if(NULL == g_pInp){
		PbgError("メモリが足りないよ(Input)");
		return FALSE;
	}
	if(FALSE == g_pInp->Initialize(g_hMainWindow)){
		PbgError("初期化に失敗(Input)");
		return FALSE;
	}

	g_pMusic = NewEx(CMusicCtrl(g_hMainWindow, pConfig));
	if(NULL == g_pMusic){
		PbgError("メモリが足りないよ(Music)");
		return FALSE;
	}

	g_pGrp = NewEx(Pbg::CGraphic);
	if(NULL == g_pGrp){
		PbgError("メモリが足りないよ(Graphic)");
		return FALSE;
	}


//	for(int i=g_pGrp->GetNumEnumeratedDevices()-1; i>=0; i--){
		if(FALSE == g_pGrp->Initialize(g_hMainWindow, 0)){
			PbgError("初期化に失敗（Graphic）");
			return FALSE;
		}
/*
		DDGAMMARAMP		Gamma;
		int				j;
		char			buf[1024];
		g_pGrp->GetGammaRamp(&Gamma);
		for(j=0; j<256; j++){
			wsprintf(buf, "%3d : %5d %5d %5d\n", j, Gamma.red[j], Gamma.green[j], Gamma.blue[j]);
			PbgLog(buf);
		}
	}
	return FALSE;
*/

	if(FALSE == g_pGrp->CreateSurface(&g_pSystemSurf, 640, 480)){
		PbgError("Surface の作成に失敗（Graphic）");
		return FALSE;
	}

	if(FALSE == g_pSystemSurf->LoadP("GRAPH.DAT", "GRP/キーコンフィグ")){
		PbgError("ＢＭＰが見つからないの");
		return FALSE;
	}

	if(FALSE == g_pSystemSurf->SetColorKey(0)){
		PbgError("カラーキーがセットできない");
		return FALSE;
	}

	g_pCursor = NewEx(CAnmCursor);
	if(NULL == g_pCursor) return FALSE;

	RECT	ptn[9];
/*
	for(int i=0; i<9; i++){
		SetRect2(ptn+i, 176+i*16, 264, 16, 16);
	}
	g_pCursor->Set(&g_pSystemSurf, ptn, 9, 4);

	SetRect2(ptn+0, 176, 264, 24, 24);
	SetRect2(ptn+1, 200, 264, 24, 24);
	SetRect2(ptn+2, 224, 264, 24, 24);
	SetRect2(ptn+3, 248, 264, 24, 24);
	SetRect2(ptn+4, 224, 264, 24, 24);
	SetRect2(ptn+5, 200, 264, 24, 24);
*/
	SetRect2(ptn+0,  0, 128, 24, 24);
	SetRect2(ptn+1, 24, 128, 24, 24);
	SetRect2(ptn+2, 48, 128, 24, 24);
	SetRect2(ptn+3, 72, 128, 24, 24);
	SetRect2(ptn+4, 48, 128, 24, 24);
	SetRect2(ptn+5, 24, 128, 24, 24);
	g_pCursor->Set(&g_pSystemSurf, ptn, 6, 4);



	//////////////////////////////////////

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_TAMA, "GRAPH.DAT", "GRP/敵弾", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(1)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_EXTAMA, "GRAPH.DAT", "GRP/特殊弾", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(2)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_HLASER, "GRAPH.DAT", "GRP/れざ", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(3)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_FONT, "GRAPH.DAT", "GRP/フォント", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(4)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_EFFECT, "GRAPH.DAT", "GRP/エフェクト", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(5)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_ATKEFC, "GRAPH.DAT", "GRP/攻撃予告", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(6)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_STAGE, "GRAPH.DAT", "GRP/ステージ", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(7)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_BONUS, "GRAPH.DAT", "GRP/ボーナス", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(8)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_GUARD, "GRAPH.DAT", "GRP/ガードゲージ", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(9)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_SHIELD, "GRAPH.DAT", "GRP/ガードまる", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(A)");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(	TEXTURE_ID_STFONT, "GRAPH.DAT", "GRP/StFont", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ(B)");
		return FALSE;
	}


	g_pTextWindow = NewEx(CTextWindow);
	if(NULL == g_pTextWindow) return FALSE;

	const int FontSize = 24;
	g_pTextWindow->Resize(FontSize*23, FontSize*3);
	g_pTextWindow->SetFontSize(FontSize);
	g_pTextWindow->SetWindowPosA((640-FontSize*23)/2, 480-FontSize*3-10);


//	CKeyCfgWindow::SetSurface(&g_pSystemSurf);
	CInputCfgWindow::SetSurface(&g_pSystemSurf);

	g_pGameInfo = NewEx(CGameInfo(pConfig));
	if(NULL == g_pGameInfo) return FALSE;
	if(FALSE == g_pGameInfo->Initialize()) return FALSE;


//	PbgError("作為的な強制終了ね");
//	return FALSE;

	return TRUE;
}


// 終了処理関数 //
extern void XCleanup(void)
{
//PbgError("G_INFO");
	DeleteEx(g_pGameInfo);		// ゲーム情報管理

//PbgError("CURSOR");
	DeleteEx(g_pCursor);		// カーソル管理
//PbgError("TXT_WNd");
	DeleteEx(g_pTextWindow);	// テキスト窓管理

//PbgError("G_Dst");
	DeleteEx(g_pGrp);			// グラフィック管理
//PbgError("I_Dst");
	DeleteEx(g_pInp);			// 入力系管理
//PbgError("M_Dst");
	DeleteEx(g_pMusic);			// 曲の管理

//	DeleteEx(g_pConfig);		// 設定ファイル入出力
}
