/*
 *   CBAtkGameProc.cpp   : ボスアタックモード
 *
 */

#include "CBAtkGameProc.h"
#include "GameMainMenu.h"
#include "CGameInfo.h"
#include "SECtrl.h"



//RECT	rcScreen = { 24, 0, 292, 480};
RECT rcBAScreen = {320-268/2, 0, (320-268/2)+268, 480};

BYTE CBAGameProc::m_State   = GAMEST_PLAY;
BOOL CBAGameProc::m_bPaused = FALSE;

#ifdef PBG_DEBUG
BOOL	CBAGameProc::m_bNeedDebugReset = FALSE;
#endif



// コンストラクタ //
CBAGameProc::CBAGameProc(CGameInfo *pSys)
	 : CScene(pSys), m_ExitWindow(pSys)
{
	m_CurrentStage = 0;
	m_DoorWait     = 0;
	m_pFrame       = NULL;
	m_pGameMain    = NULL;
	m_Player1Left  = 2;
	m_Player_ID    = CHARACTER_VIVIT;


	m_pGameMain = NewEx(CGameMainBA(&rcBAScreen));
	if(NULL == m_pGameMain){
		PbgError("シーンの作成(BossAttackMode)に失敗");
		return;
	}

	if(FALSE == m_ExitWindow.SetItem(NULL
								   , ExitWindowFunc
								   , g_ExitMenuItem
								   , 3
								   , 150+70
								   , TWIN_EXITMENU)){
		PbgError("CGameProc : ウィンドウの初期化に失敗");
		return;
	}

	m_ExitWindow.SetWindowPos(320-(150+70)/2, 200-16);
}


// デストラクタ //
CBAGameProc::~CBAGameProc()
{
	DeleteEx(m_pGameMain);

	if(m_pFrame){
		g_pGrp->ReleaseOffsSurface(m_pFrame);
		m_pFrame = NULL;
	}
}


// 初期化をする //
IBOOL CBAGameProc::Initialize(void)
{
	// シーンの作成に失敗している //
	if(NULL == m_pGameMain) return FALSE;

	// 画面外枠のロード //
	if(NULL == m_pFrame){
		if(FALSE == g_pGrp->CreateSurface(&m_pFrame, 640, 480)){
			return FALSE;
		}

		if(FALSE == m_pFrame->LoadP("GRAPH.DAT", "GRP/ウィンドウ")){
			return FALSE;
		}

		m_pFrame->SetColorKey(0);
	}

	m_bPaused = FALSE;
	m_State   = GAMEST_PLAY;

	m_CurrentStage = 0;
	m_DoorWait     = 0;
	m_Player1Left  = 2;

	CHARACTER_ID		temp;
	m_pSys->GetCharacterID(&m_Player_ID, &temp);

	// マウスカーソルを無効化 //
	m_pSys->EnableMouseCursor(FALSE);

	m_ExitWindow.OnClose();




////////////////////
	if(g_pMusic){
		g_pMusic->Stop();

		switch(m_Player_ID){
			default:
			case CHARACTER_VIVIT:	g_pMusic->Load(1);	break;
			case CHARACTER_STG2:	g_pMusic->Load(3);	break;
			case CHARACTER_STG3:	g_pMusic->Load(2);	break;
			case CHARACTER_STG1:	g_pMusic->Load(4);	break;
			case CHARACTER_STG4:	g_pMusic->Load(5);	break;
			case CHARACTER_STG5:	g_pMusic->Load(6);	break;

			case CHARACTER_MORGAN:	g_pMusic->Load(8);	break;
			case CHARACTER_MUSE:	g_pMusic->Load(9);	break;
			case CHARACTER_YUKA:	g_pMusic->Load(7);	break;
		}
		g_pMusic->Play();
	}

/*
	char *pStgID[8] = {
		"SCRIPT/STAGE01", "SCRIPT/STAGE05", "SCRIPT/STAGE03",
		"SCRIPT/STAGE02", "SCRIPT/STAGE04", "SCRIPT/STAGE03",
		"SCRIPT/STAGE01", "SCRIPT/STAGE02"
	};
*/
	if(FALSE == CSCLBADecoder::LoadBA("SCRIPT/STAGE01"
									, m_Player_ID)){
		PbgError("スクリプトのロードに失敗");
		return FALSE;
	}
	Pbg::CRnd		MakeRnd;
	CSCLBADecoder::ShuffleSCLData(&MakeRnd);

	return m_pGameMain->Initialize(m_Player_ID, MakeRnd.Get());
//
//	return TRUE;
}


// １フレーム分だけ動作させる //
PBG_PROCID CBAGameProc::Move(void)
{
	WORD		KeyCode;//, KeyCode2;
//	GMAIN_RET	Ret1, Ret2;

#ifdef PBG_DEBUG
	if(GetAsyncKeyState(VK_SPACE) & 0x8000){
		Sleep(200);
	}
#endif

	// 一時停止中の場合 //
	if(m_bPaused){
		m_ExitWindow.OnKeyDown(m_pSys->GetSystemKeyCode());
		m_ExitWindow.Move();

		if(GAMEST_EXIT == m_State || GAMEST_SELECT == m_State){
			if(m_pFrame){
				g_pGrp->ReleaseOffsSurface(m_pFrame);
			}

			if(GAMEST_EXIT == m_State) return PBGPROC_TITLE;
			else                       return PBGPROC_SELECT;
		}

		// フレームの更新を抑制する //
		return PBGPROC_OK;
	}
	else{
		KeyCode = m_pSys->GetSystemKeyCode();

		if(KEY_MENU == KeyCode){
#ifdef PBG_DEBUG
			extern HWND g_hMainWindow;
			if(CGameMainBA::IsDebugMode()){
				ShowWindow(g_hMainWindow, SW_HIDE);//MINIMIZE);
				ShowWindow(g_hMainWindow, SW_SHOWMINNOACTIVE);
				//PostMessage(g_hMainWindow, WM_ACTIVATEAPP, FALSE, 0);
				m_bNeedDebugReset = TRUE;

				return PBGPROC_OK;
			}
#endif
			m_bPaused = TRUE;
			m_ExitWindow.OnClose();		// 一度閉じてから
			m_ExitWindow.OnActivate();	// アクティブにする
			return PBGPROC_OK;
		}
	}

#ifdef PBG_DEBUG
	if(m_bNeedDebugReset){
		Initialize();
		m_bNeedDebugReset = FALSE;
	}
#endif

	if(m_pGameMain) m_pGameMain->Move(m_pSys->GetPlayer1_KeyCode());
/*
	switch(m_State){
	}
*/

	return PBGPROC_OK;
}


// 描画する //
IVOID CBAGameProc::Draw(void)
{
	RECT	src = {0, 0, 640, 480};
//RECT	rcScreen1 = { 24, 0, 292, 480};
//RECT	rcScreen2 = {348, 0, 616, 480};
	int		dx;


	// 仮のぬりつぶし //
	g_pGrp->Cls(0);

	// ３Ｄシーンの開始 //
	if(FALSE == g_pGrp->Begin3DScene()) return;

	dx = SinL((BYTE)(m_DoorWait*16), 24*(64-m_DoorWait)) >> 8;
	m_pGameMain->Draw(dx);

	g_pGrp->SetViewport(&src);

	// ３Ｄシーンの終了 //
	g_pGrp->End3DScene();


//RECT rcBAScreen = {320-268/2+24, 0, (320-268/2+24)+268, 480};
	SetRect(&src, 0, 0, 24, 480);		// 右枠
	g_pGrp->BltN(&src, (320-268/2-24)+dx, 0, m_pFrame);

//	SetRect(&src, 0, 0, 56, 480);		// 中央枠
//	g_pGrp->BltN(&src, 292+dx, 0, m_pFrame);

	SetRect(&src, 32, 0, 56, 480);		// 左枠
	g_pGrp->BltN(&src, (320+268/2+24-24)+dx, 0, m_pFrame);
/*
	// 残機の表示 //
	if(FALSE == m_Is2PMode){
		DrawPlayerLeft(320-24, m_Player1Left, m_Player1_ID);
//		DrawPlayerLeft(320-32, m_Player1Left, m_Player1_ID);
	}
	else{
		DrawPlayerLeft(320-24, m_Player1Won, m_Player1_ID);
//		DrawPlayerLeft(320-32, m_Player1Won, m_Player1_ID);
		DrawPlayerLeft(320,    m_Player2Won, m_Player2_ID);
	}
*/
	DrawPlayerLeft((320+268/2+24-24)+dx, m_Player1Left, m_Player_ID);

	// ３Ｄシーンの開始 //
	if(FALSE == g_pGrp->Begin3DScene()) return;

	m_pGameMain->DrawInfo(m_pFrame);

	// ３Ｄシーンの終了 //
	g_pGrp->End3DScene();

	// ＦＰＳなどの情報を描画する //
	m_pSys->DrawSystemInfo(m_pFrame, 160);

//	if(m_State == GAMEST_PAUSE){
	if(m_bPaused){
		m_ExitWindow.Draw();
		g_pTextWindow->Draw();
	}
}


// プレイヤーの残機を描画する //
FVOID CBAGameProc::DrawPlayerLeft(int sx, int n, CHARACTER_ID CharID)
{
	RECT		src;
	int			i;

	switch(CharID){
		case CHARACTER_VIVIT:
			SetRect2(&src, 64+32*0, 448, 24, 24);
		break;

		case CHARACTER_STG2:
			SetRect2(&src, 64+32*2, 448, 24, 24);
		break;

		case CHARACTER_STG1:
			SetRect2(&src, 64+32*1, 448, 24, 24);
		break;

		case CHARACTER_STG3:
			SetRect2(&src, 64+32*3, 448, 24, 24);
		break;

		case CHARACTER_STG4:
			SetRect2(&src, 64+32*4, 448, 24, 24);
		break;

		case CHARACTER_STG5:
			SetRect2(&src, 64+32*5, 448, 24, 24);
		break;

		case CHARACTER_MORGAN:
			SetRect2(&src, 64+32*6, 448, 24, 24);
		break;

		case CHARACTER_MUSE:
			SetRect2(&src, 64+32*7, 448, 24, 24);
		break;

		case CHARACTER_YUKA:
			SetRect2(&src, 64+32*8, 448, 24, 24);
		break;

		default:
		return;
	}

	for(i=0; i<n; i++){
		g_pGrp->BltC(&src, sx, 16+64+i*24, m_pFrame);
	}
}


// 窓管理用関数 //
PBGWND_RET CBAGameProc::ExitWindowFunc(
								CGameInfo	*pSys,
								BYTE		ItemID,
								WORD		KeyCode)
{
	switch(ItemID){
	case ITEM_EMENU_TITLE:
		if(KEY_SHOT == KeyCode){
			m_State = GAMEST_EXIT;
			return RET_CLOSE;
		}
	break;

	case ITEM_EMENU_SELECT:
		if(KEY_SHOT == KeyCode){
			m_State = GAMEST_SELECT;
			return RET_CLOSE;
		}
	break;

	case ITEM_EMENU_CANCEL:
		if(KEY_SHOT == KeyCode){
			KeyCode = KEY_MENU;
		}
	break;
	}

	if(KEY_MENU == KeyCode){
		m_bPaused = FALSE;
//		m_State = GAMEST_PLAY;
		return RET_CLOSE;
	}

	return RET_DEFAULT;
}
