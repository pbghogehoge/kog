/*
 *   CGameProc.cpp   : 1P/2P用ゲーム進行
 *
 */

#include "CGameProc.h"
#include "CGameInfo.h"
#include "SECtrl.h"
#include "GameMainMenu.h"



// 以下は、一時的なグローバル変数 //
RECT	rcScreen1 = { 24, 0, 292, 480};
RECT	rcScreen2 = {348, 0, 616, 480};
//RECT	rcScreen1 = {  4, 0, 276, 440};
//RECT	rcScreen2 = {364, 0, 636, 440};



BYTE	CGameProc::m_State   = GAMEST_PLAY;
BOOL	CGameProc::m_bPaused = FALSE;


#ifdef PBG_DEBUG
BOOL	CGameProc::m_bNeedDebugReset = FALSE;
#endif




// コンストラクタ //
CGameProc::CGameProc(CGameInfo *pSys)
	 : CScene(pSys), m_ExitWindow(pSys), m_DispWon(pSys)
{
	m_Player1_ID = CHARACTER_VIVIT;
	m_Player2_ID = CHARACTER_VIVIT;

	m_pPlayer1  = NULL;		// プレイヤー１のゲーム管理
	m_pPlayer2  = NULL;		// プレイヤー２のゲーム管理

	m_pFrame    = NULL;		// 画面外枠の描画用

	m_Player1Score = 0;		// プレイヤー１のスコア
	m_Player2Score = 0;		// プレイヤー２のスコア
	m_CurrentStage = 0;		// 現在のステージ or ラウンド

	m_Player1Left = 0;		// １Ｐモード時の残機数
	m_Player1Won  = 0;		// １Ｐ側の勝利数
	m_Player2Won  = 0;		// ２Ｐ側の勝利数

	m_Player1AtkLv = 1;		// プレイヤー１の初期攻撃レベル
	m_Player2AtkLv = 1;		// プレイヤー２の初期攻撃レベル

	m_Player1CGaugeMax = CGAUGE_LEVEL1;
	m_Player2CGaugeMax = CGAUGE_LEVEL1;

	m_DoorWait = 0;		// 扉閉じ用ウェイト

	m_pPlayer1 = NewEx(CGameMain(&rcScreen1, SCENE_PLAYER_1));
	if(NULL == m_pPlayer1){
		PbgError("シーンの作成(Player1)に失敗");
		return;
	}

	m_pPlayer2 = NewEx(CGameMain(&rcScreen2, SCENE_PLAYER_2));
	if(NULL == m_pPlayer2){
		PbgError("シーンの作成(Player2)に失敗");
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
CGameProc::~CGameProc()
{
	DeleteEx(m_pPlayer1);
	DeleteEx(m_pPlayer2);

	if(m_pFrame){
		g_pGrp->ReleaseOffsSurface(m_pFrame);
		m_pFrame = NULL;
	}
}


// 初期化をする //
IBOOL CGameProc::Initialize(void)
{
	if(NULL == m_pPlayer1) return FALSE;
	if(NULL == m_pPlayer2) return FALSE;

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

	// ゲームモードの取得を行う //
	switch(m_pSys->GetGameMode()){
		case GMODE_STORY:		// ストーリーモード
			m_Is2PMode = FALSE;
		break;

		case GMODE_VS_2P:		// ２Ｐと対戦
		case GMODE_VS_CPU:		// ＣＰＵと対戦
		case GMODE_DEMO:		// デモ
			m_Is2PMode = TRUE;
		break;

		default:
			PbgError("ゲームモード指定ＩＤが不正");
		return FALSE;
	}

	m_bPaused = FALSE;


	// スコアの初期化 //
	m_Player1Score = 0;		// プレイヤー１のスコア
	m_Player2Score = 0;		// プレイヤー２のスコア

	// 残機＆勝敗数 //
	m_Player1Left = 2;		// １Ｐモード時の残機数
	m_Player1Won  = 0;		// １Ｐ側の勝利数
	m_Player2Won  = 0;		// ２Ｐ側の勝利数

	// 初期攻撃レベル //
	if(GMODE_VS_CPU == m_pSys->GetGameMode()){
		int			dif;

		switch(m_pSys->GetBaseDifficulty()){
		case GDIFF_EASY:	dif = 1;	break;

		default:
		case GDIFF_NORMAL:	dif = 2;	break;

		case GDIFF_HARD:	dif = 4;	break;
		case GDIFF_LUNATIC:	dif = 5;	break;
		}

		m_Player1AtkLv = dif;	// プレイヤー１の初期攻撃レベル
		m_Player2AtkLv = dif;	// プレイヤー２の初期攻撃レベル
	}
	else{
		m_Player1AtkLv = 1;	// プレイヤー１の初期攻撃レベル
		m_Player2AtkLv = 1;	// プレイヤー２の初期攻撃レベル
	}

	m_Player1CGaugeMax = CGAUGE_LEVEL1;
	m_Player2CGaugeMax = CGAUGE_LEVEL1;

	// キャラクタの出現順序をシャッフルする //
	CHARACTER_ID	CharID[6] = {
		CHARACTER_VIVIT, CHARACTER_STG1, CHARACTER_STG2,
		CHARACTER_STG3, CHARACTER_STG4, CHARACTER_STG5,
	};

//	m_CharID[0] = CHARACTER_VIVIT;
//	m_CharID[1] = CHARACTER_STG2;
//	m_CharID[2] = CHARACTER_STG3;

	Pbg::CRnd		MakeRnd;
	Pbg::Shuffle(CharID, 6, &MakeRnd);		// おまかせ
	Pbg::Shuffle(CharID, 6, &MakeRnd);		// おまかせ

	for(int i=0; i<3; i++){
		m_CharID[i] = CharID[i];
	}

	m_CharID[3] = CHARACTER_MORGAN;
	m_CharID[4] = CHARACTER_MUSE;

//////////////////////////////////////
//	m_CharID[0] = CHARACTER_MUSE;
//////////////////////////////////////



	// ステージ情報の初期化を行う //
	if(FALSE == InitializeStage(1, TRUE)) return FALSE;

	// マウスカーソルを無効化 //
	m_pSys->EnableMouseCursor(FALSE);

	m_ExitWindow.OnClose();

	m_State = GAMEST_PLAY;

	return TRUE;
}


// １フレーム分だけ動作させる //
PBG_PROCID CGameProc::Move(void)
{
	PBG_PROCID		ret;

	// 現在、勝敗表示中の場合 //
	if(GAMEST_DISPWONLOST == m_State){
		if(!m_DispWon.Move(&ret)){
			if(ret != PBGPROC_OK) return ret;

			// 最終ステージまで行った場合         //
			// 注意：完成したらエンディングに接続 //
			if(m_CurrentStage > NUM_STAGE) return PBGPROC_ENDING;

			// ステージを初期状態にする or 次のステージへと進む //
			if(!InitializeStage(m_CurrentStage, TRUE)){
				return PBGPROC_CLOSE;	// ？おかしいぞ
			}

			m_State = GAMEST_PLAY;
		}

		return ret;
	}
	// ゲーム中 //
	else{
		return MoveGameMain();
	}
}


// 描画する //
IVOID CGameProc::Draw(void)
{
	// 現在、勝敗表示中の場合 //
	if(GAMEST_DISPWONLOST == m_State){
		m_DispWon.Draw();
	}
	// ゲーム中 //
	else{
		DrawGameMain();
	}
}


// 次のステージ or ラウンドに進む //
PBG_PROCID CGameProc::GotoNextStage(void)
{
	CHARACTER_ID		p1, p2;
	BOOL				bInitMusic = FALSE;
	BYTE				col;

	m_Player1CGaugeMax = CGAUGE_LEVEL1;
	m_Player2CGaugeMax = CGAUGE_LEVEL1;

	// まずは、スコアの取得を行っておきましょう //
	m_Player1Score = m_pPlayer1->GetCurrentScore();
	m_Player2Score = m_pPlayer2->GetCurrentScore();

	// キャラクタＩＤの取得 //
	m_pSys->GetCharacterID(&p1, &p2);

	// ２Ｐ対戦モードの場合 //
	if(m_Is2PMode){
		// デモの場合、タイトルに戻る処理をここに記述 //

		// ここで、勝敗カウンタを更新する //
		switch(m_State){
			case GAMEST_1PWON:	m_Player1Won++;		break;
			case GAMEST_2PWON:	m_Player2Won++;		break;

			// 引き分けは、両者負け扱い //
			case GAMEST_DRAW:	break;
		}

		// 次のラウンドに進む //
		m_CurrentStage++;

		// 前回の攻撃レベルは保持する //
		m_Player1AtkLv = m_pPlayer1->GetCurrentAtkLv();
		m_Player2AtkLv = m_pPlayer2->GetCurrentAtkLv();

		m_Player1CGaugeMax = m_pPlayer1->GetCGaugeMax();
		m_Player2CGaugeMax = m_pPlayer2->GetCGaugeMax();

		// ステージ数が規定の範囲を超えた場合、もしくは //
		// 勝った回数が２回を超えるような場合           //
		if(m_Player1Won >= 2){
			m_State = GAMEST_DISPWONLOST;
			if(m_DispWon.Initialize(PBGPROC_SELECT, p1, p2, 1)) return PBGPROC_OK;
			else                                                return PBGPROC_CLOSE;
		}
		else if(m_Player2Won >= 2){
			m_State = GAMEST_DISPWONLOST;
			if(m_DispWon.Initialize(PBGPROC_SELECT, p2, p1, 2)) return PBGPROC_OK;
			else                                                return PBGPROC_CLOSE;
		}
		// 本来、これは引き分け表示をすべきである //
		else if(m_CurrentStage > NUM_ROUND) return PBGPROC_SELECT;

		m_State = GAMEST_PLAY;
	}
	// ストーリーモードの場合 //
	else{
		// プレイヤー側の勝利なら、次のステージ   //
		// そうでなければ残機によって、処理が変化 //
		switch(m_State){
			// 勝った場合、次のステージへと進むので、相手キャラは //
			// 別のキャラとなるため、スコアは初期化しておく       //
			case GAMEST_1PWON:
				m_Player2Score = 0;	// ＣＰＵ側のスコアはリセットする
				m_CurrentStage++;	// 次のステージへと

				// 次のステージ用に攻撃レベルを補正する //
				m_Player1AtkLv = m_CurrentStage;
				m_Player2AtkLv = m_CurrentStage;

				bInitMusic = TRUE;	// 曲は再ロード

				// 使用キャラの勝ちメッセージを表示する //
				m_State = GAMEST_DISPWONLOST;
				if(!m_DispWon.Initialize(PBGPROC_OK, p1, p2, 1)){
					return PBGPROC_CLOSE;
				}

				return PBGPROC_OK;
			break;

			// 参考：「１Ｐ時の引き分け」は負け扱いとなる //
			case GAMEST_DRAW: case GAMEST_2PWON:
				if(0 == m_Player1Left){
					// 相手キャラの勝ちメッセージを表示する     //
					// ->あとでゲームオーバー処理に飛ばすように //
					if(p1 == p2) col = 2;	// 同キャラ対戦
					else         col = 1;	// 別キャラ
					if(!m_DispWon.Initialize(PBGPROC_TITLE, p2, p1, col)){
						return PBGPROC_CLOSE;
					}
					m_State = GAMEST_DISPWONLOST;

					return PBGPROC_OK;
				}
				else{
					// 前回の攻撃レベルは保持する //
					m_Player1AtkLv = m_pPlayer1->GetCurrentAtkLv();
					m_Player2AtkLv = m_pPlayer2->GetCurrentAtkLv();

					m_Player1CGaugeMax = m_pPlayer1->GetCGaugeMax();
					m_Player2CGaugeMax = m_pPlayer2->GetCGaugeMax();

					// 残機を減らす //
					m_Player1Left--;

					// 元に戻す //
					m_State = GAMEST_PLAY;
				}
			break;
		}
/*
		// 最終ステージまで行った場合         //
		// 注意：完成したらエンディングに接続 //
		if(m_CurrentStage > NUM_STAGE) return PBGPROC_ENDING;
*/
	}

	// ステージを初期状態にする or 次のステージへと進む //
	if(InitializeStage(m_CurrentStage, bInitMusic)){
		return PBGPROC_OK;		// 初期化完了
	}
	else{
		return PBGPROC_CLOSE;	// ？おかしいぞ
	}
}


// プレイヤーの残機を描画する //
FVOID CGameProc::DrawPlayerLeft(int sx, int n, CHARACTER_ID CharID)
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


// １フレーム分だけ動作させる //
PBG_PROCID CGameProc::MoveGameMain(void)
{
	WORD		KeyCode1;//, KeyCode2;
	GMAIN_RET	Ret1, Ret2;

#ifdef PBG_DEBUG
	if(GetAsyncKeyState(VK_SPACE) & 0x8000){
		Sleep(200);
	}
#endif

//PbgError("CGameProc::Move()");
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
		KeyCode1 = m_pSys->GetSystemKeyCode();

		if(KEY_MENU == KeyCode1){
#ifdef PBG_DEBUG
			extern HWND g_hMainWindow;
			if(CGameMain::IsDebugMode()){
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

//PbgError("CGameProc::switch()");

	BOOL		f1, f2;
	switch(m_State){
	case GAMEST_1PWON:	case GAMEST_2PWON:	case GAMEST_DRAW:
//PbgError("On Game Finished");
		// 両者の完了を待つ //
		f1 = m_pPlayer1->IsGameFinished();
		f2 = m_pPlayer2->IsGameFinished();

		if(f1 && f2){
			if(0 == m_DoorWait){
				PlaySE(SNDID_METEORIC_BLAST_CRASH);
				PlaySE(SNDID_METEORIC_BLAST_CRASH);
			}

			if(m_DoorWait < 64) m_DoorWait++;
			else                return GotoNextStage();
		}

	case GAMEST_PLAY:
		switch(m_pSys->GetGameMode()){
		case GMODE_STORY:	// ストーリーモード
		case GMODE_VS_CPU:	// ＣＰＵと対戦
//			Ret1 = m_pPlayer1->Move(m_pSys->GetDefaultKeyCode());
//PbgError("VS CPU");
//PbgError("Ret1");
			Ret1 = m_pPlayer1->Move(m_pSys->GetPlayer1_KeyCode());
			Ret2 = m_pPlayer2->Move(KEY_CPUMOVE);
//PbgError("Ret2");
		break;

		case GMODE_VS_2P:	// ２Ｐと対戦
			Ret1 = m_pPlayer1->Move(m_pSys->GetPlayer1_KeyCode());
			Ret2 = m_pPlayer2->Move(m_pSys->GetPlayer2_KeyCode());
		break;

		case GMODE_DEMO:	// デモ
			Ret1 = m_pPlayer1->Move(KEY_CPUMOVE);
			Ret2 = m_pPlayer2->Move(KEY_CPUMOVE);
		break;
		}

//PbgError("On Moved");
		if((GMAIN_ERROR == Ret1) || (GMAIN_ERROR == Ret2)){
			PbgError("エラーが発生しました");
			return PBGPROC_CLOSE;
		}

		// １Ｐ側が死亡している場合 //
		if(GMAIN_DEAD == Ret1){
			if(GMAIN_DEAD == Ret2){	// 引き分けの場合
				m_State = GAMEST_DRAW;

				m_pPlayer1->OnWonLost(DWL_DRAW);
				m_pPlayer2->OnWonLost(DWL_DRAW);
			}
			else{							// ２Ｐ側の勝ちの場合
				m_State = GAMEST_2PWON;

				m_pPlayer1->OnWonLost(DWL_LOST);
				m_pPlayer2->OnWonLost(DWL_WON);
			}

			if(g_pMusic){
				g_pMusic->Fade(250, 160);
			}
		}
		// ２Ｐ側が死亡している場合 //
		else if(GMAIN_DEAD == Ret2){		// １Ｐ側の勝ちの場合
			m_State = GAMEST_1PWON;

			m_pPlayer1->OnWonLost(DWL_WON);
			m_pPlayer2->OnWonLost(DWL_LOST);

			if(g_pMusic){
				g_pMusic->Fade(250, 160);
			}
		}
	break;
/*
	case GAMEST_PAUSE:
		m_ExitWindow.OnKeyDown(m_pSys->GetSystemKeyCode());
		m_ExitWindow.Move();

		if(GAMEST_EXIT == m_State){
			if(m_pFrame){
				g_pGrp->ReleaseOffsSurface(m_pFrame);
			}
			return PBGPROC_TITLE;
		}
	break;*/
	}

	return PBGPROC_OK;
}


// 描画する //
IVOID CGameProc::DrawGameMain(void)
{
	RECT	src = {0, 0, 640, 480};
//RECT	rcScreen1 = { 24, 0, 292, 480};
//RECT	rcScreen2 = {348, 0, 616, 480};
	int		dx;

	// ３Ｄシーンの開始 //
	if(FALSE == g_pGrp->Begin3DScene()) return;

	dx = SinL((BYTE)(m_DoorWait*16), 24*(64-m_DoorWait)) >> 8;
	m_pPlayer1->Draw(dx);
	m_pPlayer2->Draw(dx);

	g_pGrp->SetViewport(&src);

	// ３Ｄシーンの終了 //
	g_pGrp->End3DScene();


	SetRect(&src, 32, 0, 56, 480);		// 左枠
	g_pGrp->BltN(&src,  dx, 0, m_pFrame);

	SetRect(&src, 0, 0, 56, 480);		// 中央枠
	g_pGrp->BltN(&src, 292+dx, 0, m_pFrame);

	SetRect(&src, 0, 0, 24, 480);		// 右枠
	g_pGrp->BltN(&src, 616+dx, 0, m_pFrame);

	// 残機の表示 //
	if(FALSE == m_Is2PMode){
		DrawPlayerLeft(320-24+dx, m_Player1Left, m_Player1_ID);
//		DrawPlayerLeft(320-32, m_Player1Left, m_Player1_ID);
	}
	else{
		DrawPlayerLeft(320-24+dx, m_Player1Won, m_Player1_ID);
//		DrawPlayerLeft(320-32, m_Player1Won, m_Player1_ID);
		DrawPlayerLeft(320+dx,    m_Player2Won, m_Player2_ID);
	}

	// ３Ｄシーンの開始 //
	if(FALSE == g_pGrp->Begin3DScene()) return;

	m_pPlayer1->DrawInfo(m_pFrame);
	m_pPlayer2->DrawInfo(m_pFrame);

	// ３Ｄシーンの終了 //
	g_pGrp->End3DScene();

	// ＦＰＳなどの情報を描画する //
	m_pSys->DrawSystemInfo(m_pFrame, dx);

//	if(m_State == GAMEST_PAUSE){
	if(m_bPaused){
		m_ExitWindow.Draw();
		g_pTextWindow->Draw();
	}
}


// ステージの初期化を行う //
BOOL CGameProc::InitializeStage(int nStage, BOOL bLoadMusic)
{
	// 現在のステージをセット //
	m_CurrentStage = nStage;

	// 現在設定されているキャラクタＩＤを取得する //
	m_pSys->GetCharacterID(&m_Player1_ID, &m_Player2_ID);

	// １Ｐモードの場合、正しいキャラクタＩＤに変更し、 //
	// システム側に通知する                             //
	if(FALSE == m_Is2PMode){
		m_Player2_ID = m_CharID[nStage-1];
		m_pSys->SetCharacterID(m_Player1_ID, m_Player2_ID);

		///////////////////////////
//		m_Player2_ID = CHARACTER_STG4;
		///////////////////////////
	}

	// ２Ｐ側のキャラクタに合わせて曲を選択する //
	if(bLoadMusic){
		if(g_pMusic){
			g_pMusic->Stop();

			switch(m_Player2_ID){
				default:
				case CHARACTER_VIVIT:	g_pMusic->Load(1);	break;
				case CHARACTER_STG2:	g_pMusic->Load(3);	break;
				case CHARACTER_STG3:	g_pMusic->Load(2);	break;
/*
				// ここから、鳩祭追加部 //
#pragma message("<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>>")
#pragma message("鳩で追加されていますな")
#pragma message("<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>>")
*/
				case CHARACTER_STG1:	g_pMusic->Load(4);	break;
				case CHARACTER_STG4:	g_pMusic->Load(5);	break;
				case CHARACTER_STG5:	g_pMusic->Load(6);	break;

				case CHARACTER_MORGAN:	g_pMusic->Load(8);	break;
				case CHARACTER_MUSE:	g_pMusic->Load(9);	break;
				case CHARACTER_YUKA:	g_pMusic->Load(7);	break;
			}
			g_pMusic->Play();
		}
	}
	else{
		// フェードイン //
		if(g_pMusic) g_pMusic->Fade(250, 255);
	}

//PbgError("CGameProc::InitializeStage.MusicInitialized");

//PbgError("CGameProc::InitializeStage.TextureLoaded");


	char *pStgID[8] = {
		"SCRIPT/STAGE01", "SCRIPT/STAGE05", "SCRIPT/STAGE03",
		"SCRIPT/STAGE02", "SCRIPT/STAGE04", "SCRIPT/STAGE03",
		"SCRIPT/STAGE01", "SCRIPT/STAGE02"
	};

	if(FALSE == CSCLDecoder::Load(pStgID[(m_CurrentStage-1) % 8]
								, m_Player1_ID
								, m_Player2_ID)){
		PbgError("スクリプトのロードに失敗");
		return FALSE;
	}
	Pbg::CRnd		MakeRnd;
	CSCLBADecoder::ShuffleSCLData(&MakeRnd);
//PbgError("CGameProc::InitializeStage.ScriptLoaded");

	BYTE		P1DmgWeight, P2DmgWeight;

	if(m_Is2PMode){
		if(GMODE_VS_CPU == m_pSys->GetGameMode()){
			P1DmgWeight = 255;

			switch(m_pSys->GetBaseDifficulty()){
			case GDIFF_EASY:	P2DmgWeight = 255;	break;

			default:
			case GDIFF_NORMAL:	P2DmgWeight = 128;	break;

			case GDIFF_HARD:	P2DmgWeight = 64;	break;
			case GDIFF_LUNATIC:	P2DmgWeight = 15;	break;
			}
		}
		else{
			P1DmgWeight = P2DmgWeight = 255;
		}
	}
	else{
		P1DmgWeight = 255;
		P2DmgWeight = 255 - (240 * m_CurrentStage) / NUM_STAGE;
	}

	BOOL	b1PColor, b2PColor;

	if(!m_Is2PMode){
		b1PColor = FALSE;

		if(m_Player1_ID == m_Player2_ID) b2PColor = TRUE;
		else                             b2PColor = FALSE;
	}
	else{
		b1PColor = FALSE;
		b2PColor = TRUE;
	}

//	Pbg::CRnd	MakeRnd;
	int			RndSeed = MakeRnd.Get();

	// １Ｐ(左側)の初期化を行う //
	if(FALSE == m_pPlayer1->Initialize(
								m_Player1_ID		// １Ｐ側のキャラクタＩＤ
							  , m_Player2_ID		// ２Ｐ側のキャラクタＩＤ
							  , m_pPlayer2			// 相手側
							  , m_Is2PMode			// ２Ｐモードなら真
							  , m_CurrentStage		// 現在のステージorラウンド
							  , 1					// ＳＣＬ初期化レベル
							   , m_Player1AtkLv		// 初期攻撃レベル
							  , m_Player1Score		// 前回のスコアを反映
							  , m_Player1CGaugeMax
							  , P1DmgWeight
							  , b1PColor
							  , RndSeed)){
		PbgError("シーンの初期化に失敗");
		return FALSE;
	}
//PbgError("CGameProc::InitializeStage.1PInitialized");

	// ２Ｐ(右側)の初期化を行う //
	if(FALSE == m_pPlayer2->Initialize(
								m_Player2_ID		// ２Ｐ側のキャラクタＩＤ
							  , m_Player1_ID		// １Ｐ側のキャラクタＩＤ
							  , m_pPlayer1			// 相手側
							  , m_Is2PMode			// ２Ｐモードなら真
							  , m_CurrentStage		// 現在のステージorラウンド
							  , 1					// ＳＣＬ初期化レベル
							  , m_Player2AtkLv		// 初期攻撃レベル
							  , m_Player2Score		// 前回のスコアを反映
							  , m_Player2CGaugeMax
							  , P2DmgWeight
							  , b2PColor
							  , RndSeed)){
		PbgError("シーンの初期化に失敗");
		return FALSE;
	}
//PbgError("CGameProc::InitializeStage.2PInitialized");

	// 通常状態に推移する //
//	m_State = GAMEST_PLAY;

	// 扉閉じ用ウェイト //
	m_DoorWait = 0;

	return TRUE;
}


PBGWND_RET CGameProc::ExitWindowFunc(CGameInfo	*pSys
								   , BYTE		ItemID
								   , WORD		KeyCode)
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
