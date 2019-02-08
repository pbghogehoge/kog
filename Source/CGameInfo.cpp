/*
 *   CGameInfo.cpp   : ゲーム情報管理
 *
 */

#include "CGameInfo.h"
#include "Gian2001.h"
#include "FontDraw.h"
#include "CGameMainBA.h"



// コンストラクタ //
CGameInfo::CGameInfo(CCfgIO *pCfgIO)
//	: m_Player1KeyCfg(ICWIN_KEY1)	// プレイヤー１のキーコンフィグ
//	, m_Player2KeyCfg(ICWIN_KEY2)	// プレイヤー２のキーコンフィグ
//	, m_SystemKeyCfg(ICWIN_SYSTEM)	// システム用キーコンフィグ
{
	InpCfgData		ICfgData;

	m_Player1_KeyCode = 0;		// プレイヤー１のキーコード
	m_Player2_KeyCode = 0;		// プレイヤー２のキーコード
	m_System_KeyCode  = 0;		// システムキーコード
	m_IsPadInput      = FALSE;	// パッド入力があれば真
//	m_Default_KeyCode = 0;		// デフォルトのキーコード
	m_KeyWait         = 0;		// システムキー入力用ウェイト

	m_EffectDIB.Create(640, 480);

	// コンフィグ管理と接続し、初期値を取得 //
	m_pCfgIO = pCfgIO;
	pCfgIO->GetGrpCfg(&m_GrpCfg);

	// 入力系の設定を反映する //
	pCfgIO->GetInpCfg(&ICfgData);
	m_InputCtrl.SetData(&ICfgData);

	// ＦＰＳを初期値にセット //
	m_FpsCtrl.SetFPS(m_GrpCfg.m_Fps);

	// γ値を初期値にセット //
	m_GammaCtrl.SetGamma(m_GrpCfg.m_Gamma);
	m_GammaCtrl.SetWindowPos(350, 200);
	m_bGammaEnable = FALSE;

	m_pGameProc    = NewEx(CGameProc(this));	// ゲーム本体
	m_pGameProcBA  = NewEx(CBAGameProc(this));	// ボスアタックモード用ゲーム本体
	m_pMRoomProc   = NewEx(CMRoomProc(this));	// 音楽室管理
	m_pTitleProc   = NewEx(CTitleProc(this));	// タイトル管理
	m_pLogoProc    = NewEx(CLogoProc(this));	// 西方ロゴ管理
	m_pSelectProc  = NewEx(CSelectProc(this));	// 使用キャラ選択画面
	m_pEndingProc  = NewEx(CEndingProc(this));	// エンディングの処理
//	m_pDispWonProc = NewEx(CDispWonProc(this));	// 勝ちキャラ表示処理

	// マウスカーソルを無効化 //
	m_bMouseEnable = FALSE;

	// キャプチャーフラグを解除 //
	m_bCapture = FALSE;

	// 入力設定ウィンドウ //
//	m_Player1KeyCfg.SetWindowPosA(280, 60);
//	m_Player2KeyCfg.SetWindowPosA(280, 220);
//	m_SystemKeyCfg.SetWindowPosA(280, 60);
//	m_InputWindowID = 0;

	// 難易度などの設定 //
	m_GameMode   = GMODE_STORY;			// ゲームの動作モード
	m_Difficulty = GDIFF_NORMAL;		// ゲーム全体の難易度

	// キャラクタＩＤを初期化 //
	m_Player1_CharID = CHARACTER_VIVIT;
	m_Player2_CharID = CHARACTER_VIVIT;

	// タイトルに接続 //
	m_pCurrentProc = m_pTitleProc;

	m_SaveDIB.Create(640, 480);
}


// デストラクタ //
CGameInfo::~CGameInfo()
{
	WriteInputCfgData();

	DeleteEx(m_pTitleProc);
	DeleteEx(m_pMRoomProc);
	DeleteEx(m_pGameProc);
	DeleteEx(m_pGameProcBA);
	DeleteEx(m_pLogoProc);
	DeleteEx(m_pSelectProc);
	DeleteEx(m_pEndingProc);
//	DeleteEx(m_pDispWonProc);
}


// ゲーム全体の初期化 //
FBOOL CGameInfo::Initialize(void)
{
//	if(NULL == m_pLogoProc)                return FALSE;
//	if(FALSE == m_pLogoProc->Initialize()) return FALSE;

	m_Player1_KeyCode = 0;		// プレイヤー１のキーコード
	m_Player2_KeyCode = 0;		// プレイヤー２のキーコード
//	m_Default_KeyCode = 0;		// デフォルトのキーコード
	m_System_KeyCode  = 0;		// システムキーコード
	m_IsPadInput      = FALSE;	// パッド入力があれば真
	m_KeyWait         = 0;		// システムキー入力用ウェイト

//	m_pCurrentProc = m_pLogoProc;

//	return ChangeProcedure(PBGPROC_DISPWON1P);

//	return ChangeProcedure(PBGPROC_ENDING);
//	return ChangeProcedure(PBGPROC_MUSIC);
//	return ChangeProcedure(PBGPROC_SELECT);

#pragma message(__FILE__"ほげほげほげほげ")
//	return ChangeProcedure(PBGPROC_TITLE);
	return ChangeProcedure(PBGPROC_LOGO);
}


// ゲームを進行させる(FALSE : 終了要求) //
FBOOL CGameInfo::UpdateGame(void)
{
	BYTE		KeyBuf[256];
	WORD		VKeyCode;
	PBG_PROCID	ProcID;

	if(TRUE == g_pInp->GetKeyboardInput(KeyBuf)){
#ifdef PBG_DEBUG
		// この部分にシステムレベルのキーボード処理を記述 //
		// Ex. システムウィンドウ処理、終了処理など       //
		if(m_pGameProc   == m_pCurrentProc
		|| m_pGameProcBA == m_pCurrentProc){
			if(KeyBuf[DIK_F1] & 0x80){
				// リセット処理 //
				if(m_pCurrentProc) m_pCurrentProc->Initialize();
			}

			// デバッグフラグを切り替える //
			if(KeyBuf[DIK_TAB] & 0x80){
				CGameMain::SwitchDebugInfo();
				CGameMainBA::SwitchDebugInfo();
			}
		}
#endif

	}

	// 更新の必要がなければ、ここでおしまい //
	if(FALSE == m_FpsCtrl.Update()) return TRUE;


	// 動作要求があれば、更新 //
	if(m_FpsCtrl.IsNeedMove()){
		// 特殊なシステムウィンドウを描画する //
		MoveExtraWindow();//KeyBuf);

//		VKeyCode = m_SystemKeyCfg.GetKeyCode(KeyBuf);
//		m_Default_KeyCode = m_SystemKeyCfg.GetKeyCode(KeyBuf);
		m_InputCtrl.UpdateBuffer();
		m_Player1_KeyCode = m_InputCtrl.Get1PInput();//m_Player1KeyCfg.GetKeyCode(KeyBuf);
		m_Player2_KeyCode = m_InputCtrl.Get2PInput();//m_Player2KeyCfg.GetKeyCode(KeyBuf);
		m_IsPadInput      = m_InputCtrl.IsPadInput();
		VKeyCode = m_Player1_KeyCode | m_Player2_KeyCode | m_InputCtrl.GetSysKeyInput();


		if(0 == m_KeyWait){
//			VKeyCode        &= ~(KEY_SHOT | KEY_BOMB);
			m_System_KeyCode = VKeyCode;

			if(     VKeyCode & (KEY_SHOT|KEY_MENU|KEY_BOMB))   m_KeyWait = 0xff;
			else if(VKeyCode & (KEY_LEFT|KEY_RIGHT|KEY_SHIFT)) m_KeyWait = 2;
			else if(VKeyCode)                                  m_KeyWait = 8;
		}
		else{
			// 参考までに...                                            //
			// ・ＺＸボタンの押しつづけは、オートリピートしない         //
			// ・全てのボタンが押されていない場合は、待ち時間を解除する //
			if(VKeyCode){
				// ＺＸ以外の待ちならば、待ち時間減少 //
				if(0xff != m_KeyWait) m_KeyWait--;
			}
			else{
				// キーが押されていないので、待ち時間をクリアする //
				m_KeyWait = 0;
			}

			// 前回のキーコードを無効化する //
			m_System_KeyCode = 0;
		}

		if(m_bMouseEnable) g_pCursor->Update();

		ProcID = m_pCurrentProc->Move();
		switch(ProcID){
			case PBGPROC_OK:		// 正常終了
			break;

			case PBGPROC_CLOSE:		// 閉じ
			return FALSE;

			default:				// プロシージャ変更要求
			return ChangeProcedure(ProcID);
		}
	}

	// 描画要求があれば、更新 //
	if(m_FpsCtrl.IsNeedDraw()){
		//g_pGrp->Cls();
		m_pCurrentProc->Draw();
		DrawExtraWindow();

#ifdef PBG_DEBUG
	static BOOL bCaptureLock = FALSE;
	static int  capturewait  = 0;

	if((GetAsyncKeyState(VK_LSHIFT)  & 0x8000)
	&& (GetAsyncKeyState(VK_RSHIFT)  & 0x8000)){
		if(0 == capturewait){
			if(bCaptureLock) bCaptureLock = FALSE;
			else             bCaptureLock = TRUE;
			capturewait  = 1;
		}
	}
	else{
		capturewait = 0;
	}

	if(bCaptureLock) m_bCapture = TRUE;
#endif

		if(FALSE == m_bCapture){
//			DrawSystemInfo();

			// マウスカーソルが有効ならば、描画する //
			if(m_bMouseEnable) g_pCursor->Draw();
		}

		g_pGrp->Flip();
	}

	if(m_bCapture){
		CaptureScreen();
	}

	return TRUE;
}


// システム用キーコードを取得(オートリピート付) //
FWORD CGameInfo::GetSystemKeyCode(void)
{
	return m_System_KeyCode;
}

/*
// プレイヤー１用のデフォルトキーコードを取得 //
FWORD CGameInfo::GetDefaultKeyCode(void)
{
	return m_Default_KeyCode;
}
*/

// プレイヤー１のキーコードを取得 //
FWORD CGameInfo::GetPlayer1_KeyCode(void)
{
	return m_Player1_KeyCode;
}


// プレイヤー２のキーコードを取得 //
FWORD CGameInfo::GetPlayer2_KeyCode(void)
{
	return m_Player2_KeyCode;
}


// パッドからの入力があれば真 //
FBOOL CGameInfo::IsPadInput(void)
{
	return m_IsPadInput;
}


// 入力モードを変更する //
FVOID CGameInfo::SetInputMode(INPUT_MODE Mode)
{
	m_InputCtrl.SelectInputDevice(Mode);
}


// 入力コンフィグを書き込む //
FVOID CGameInfo::WriteInputCfgData(void)
{
	InpCfgData		Cfg;

	m_InputCtrl.GetData(&Cfg);
	m_pCfgIO->SetInpCfg(&Cfg);
}


// 入力コンフィグを全初期化する //
FVOID CGameInfo::InitInputCfgData(void)
{
	m_InputCtrl.InitSetting();
}


// ＦＰＳをセットする //
FVOID CGameInfo::SetFPS(DWORD Fps)
{
	m_GrpCfg.m_Fps = Fps;
	m_pCfgIO->SetGrpCfg(&m_GrpCfg);

	m_FpsCtrl.SetFPS(Fps);
}


// ＦＰＳを取得する //
FDWORD CGameInfo::GetFPS(void)
{
	return m_FpsCtrl.GetFPS();
}


// γをセットする //
FVOID CGameInfo::SetGamma(BYTE Gamma)
{
	m_GrpCfg.m_Gamma = Gamma;
	m_pCfgIO->SetGrpCfg(&m_GrpCfg);

	m_GammaCtrl.SetGamma(Gamma);
}


// γを取得する //
FBYTE CGameInfo::GetGamma(void)
{
	return m_GammaCtrl.GetGamma();
}


// γ設定ウィンドウを有効にする //
FVOID CGameInfo::EnableGammaWindow(BOOL bEnable)
{
	m_bGammaEnable = bEnable;
}


// マウスカーソルを有効にする //
FVOID CGameInfo::EnableMouseCursor(BOOL bEnable)
{
	m_bMouseEnable = bEnable;
}


// ゲームの動作モードをセットする //
FVOID CGameInfo::SetGameMode(BYTE Mode)
{
	m_GameMode = Mode;		// ゲームの動作モード
}


// ゲームの動作モードを取得する //
FBYTE CGameInfo::GetGameMode(void)
{
	return m_GameMode;
}


// ゲームの難易度をセットする //
FVOID CGameInfo::SetBaseDifficulty(BYTE Difficulty)
{
	m_Difficulty = Difficulty;
}


// ゲームの難易度を取得する //
FBYTE CGameInfo::GetBaseDifficulty(void)
{
	return m_Difficulty;
}


// システム情報を描画する //
FVOID CGameInfo::DrawSystemInfo(Pbg::CGrpSurface *pSurf, int dx)
{
	char			buf[100];
	RECT			src = {0, 0, 640, 480}, old;
	SYSTEMTIME		SysTime;

	g_pGrp->GetViewport(&old);
	g_pGrp->SetViewport(&src);

	wsprintf(buf, "%3d Fps", m_FpsCtrl.GetRealFPS());
	Draw2DFont(pSurf, dx+280+15, 0, buf);
//	g_Font.Draw(275, 0, buf, 14);

	GetLocalTime(&SysTime);
	wsprintf(buf, "%02d/%02d/%02d"
				, SysTime.wYear%100, SysTime.wMonth, SysTime.wDay);
	Draw2DFont(pSurf, dx+280+15, 30, buf);
//	g_Font.Draw(280+12, 30, buf, 10);

	wsprintf(buf, "%2d:%02d:%02d"
				, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
//	g_Font.Draw(280+12, 42, buf, 10);
	Draw2DFont(pSurf, dx+280+15, 40, buf);

	g_pGrp->SetViewport(&old);
}


// 各プレイヤーの選択したキャラクタをセットする //
FVOID CGameInfo::SetCharacterID(CHARACTER_ID Player1, CHARACTER_ID Player2)
{
	m_Player1_CharID = Player1;
	m_Player2_CharID = Player2;
}


// 各プレイヤーの選択したキャラクタを取得する //
FVOID CGameInfo::GetCharacterID(CHARACTER_ID *pPlayer1, CHARACTER_ID *pPlayer2)
{
	*pPlayer1 = m_Player1_CharID;
	*pPlayer2 = m_Player2_CharID;
}


// ゲームが完了したときに呼び出す //
FVOID  CGameInfo::OnClearGame(void)
{
	DWORD			n;

	switch(m_Player1_CharID){
		case CHARACTER_MUSE:		n = 9;	break;
		case CHARACTER_MORGAN:		n = 8;	break;
		default:					n = 7;	break;
	}

	if(m_pCfgIO){
		if(m_pCfgIO->GetNumCharacters() < n){
			m_pCfgIO->SetNumCharacters(n);
			m_pCfgIO->SaveConfig();
		}
	}
}


// 使用できるキャラクタ数を返す //
FDWORD CGameInfo::GetNumCharacters(void)
{
	return m_pCfgIO->GetNumCharacters();
}


// キャプチャー要求を送出 //
FVOID CGameInfo::OnCapture(void)
{
	m_FpsCtrl.Reset();
	m_bCapture = TRUE;
}


// 入力設定ウィンドウを有効にする //
FVOID CGameInfo::EnableInputWindow(CFG_WINDOW WindowID)
{
	m_InputCtrl.SelectConfigWindow(WindowID);
}


// 入力設定ウィンドウがアクティブなら真を返す //
FBOOL CGameInfo::IsActiveInputWindow(void)
{
	return m_InputCtrl.IsActiveConfigWindow();
}


// 入力ヘルプ文字列を取得し、変更要求を出力 //
FVOID CGameInfo::GetInputHelpString(char *pBuf)
{
#pragma message(__FILE__" : ここを調整する必要があるぞぃ\r\n")
/*	if(m_SystemKeyCfg.IsActive()){
		m_SystemKeyCfg.GetHelpString(pBuf);
	}
	else if(m_Player1KeyCfg.IsActive()){
		m_Player1KeyCfg.GetHelpString(pBuf);
	}
	else if(m_Player2KeyCfg.IsActive()){
		m_Player2KeyCfg.GetHelpString(pBuf);
	}*/
//	else{
		strcpy(pBuf, "");
//	}
}


// 特殊なウィンドウを描画する //
FVOID CGameInfo::DrawExtraWindow(void)
{
	if(g_pGrp->Begin3DScene()){
		if(m_bGammaEnable) m_GammaCtrl.Draw();
		g_pGrp->End3DScene();
	}

	m_InputCtrl.DrawConfigWindow();
}


// 特殊なウィンドウを動作させる //
FVOID CGameInfo::MoveExtraWindow(void)
{
	m_InputCtrl.MoveConfigWindow();

//	if(FALSE == m_bMouseEnable) return;
/*
	if(m_InputWindowID){
		MoveInputWindow(KeyBuf);
	}
*/
}


// 動作プロシージャを変更する //
FBOOL CGameInfo::ChangeProcedure(PBG_PROCID ProcID)
{
	BOOL		Result = FALSE;

	switch(ProcID){
		case PBGPROC_TITLE:		// タイトルに移行すべし
			m_pCurrentProc = m_pTitleProc;
		break;

		case PBGPROC_MAIN:		// メインに移行すべし
			if(GMODE_BOSSATTACK == m_GameMode) m_pCurrentProc = m_pGameProcBA;
			else                               m_pCurrentProc = m_pGameProc;
		break;

		case PBGPROC_MUSIC:
			m_pCurrentProc = m_pMRoomProc;
		break;

		case PBGPROC_LOGO:
			m_pCurrentProc = m_pLogoProc;
		break;

		case PBGPROC_SELECT:
			m_pCurrentProc = m_pSelectProc;
		break;

		case PBGPROC_ENDING:
			m_pCurrentProc = m_pEndingProc;
		break;
/*
		case PBGPROC_DISPWON1P:
			m_pCurrentProc = m_pDispWonProc;
			m_pDispWonProc->SetCharacterID(m_Player1_CharID, m_Player2_CharID, 1);
		break;

		case PBGPROC_DISPWON2P:
			m_pCurrentProc = m_pDispWonProc;
			m_pDispWonProc->SetCharacterID(m_Player2_CharID, m_Player1_CharID, 2);
		break;
*/
		default:
			PbgError("不正なプロシージャＩＤです。強制終了します");
		return FALSE;
	}

	if(m_pCurrentProc){
		m_FpsCtrl.Reset();
		Result = m_pCurrentProc->Initialize();
	}

	if(FALSE == Result){
		PbgError("初期化に失敗しました");
	}

	g_pGrp->Cls();
	g_pGrp->Flip();
	g_pGrp->Cls();

	return Result;
}


// キャプチャー要求に対応する //
FVOID CGameInfo::CaptureScreen(void)
{
	Pbg::CPbgFile		File;
//	Pbg::CDIB32			SaveDIB;
	char				Name[MAX_PATH];
//	char				OutputFileName[MAX_PATH];
//	char				*p;
	int					i;

	static int			StartID = 0;

	m_bCapture = FALSE;

//	if(FALSE == SaveDIB.Create(640, 480)) return;

	// 書き込み禁止ならば、処理しない //
//	GetCurrentDirectory(MAX_PATH, Name);

//	p = Name + strlen(Name) - 1;
//	if(*p != '\\') strcat(Name, "\\");

//PbgError("出力先は");
//PbgError(Name);
/*	static BOOL bOutputMsg = FALSE;
	if(FILE_ATTRIBUTE_READONLY & GetFileAttributes(Name)){
		if(bOutputMsg){
			PbgError("出力先のディレクトリが読み取り専用になっているため、キャプチャーできません");
		}

		bOutputMsg = TRUE;
		return;
	}*/
//PbgError("属性問題なし");
	if(DRIVE_CDROM == GetDriveType(NULL)) return;

	for(i=StartID; i<10000; i++){
//		wsprintf(OutputFileName, "%sまれ%03d.bmp", Name, i+1);
		wsprintf(Name, "まれ%04d.bmp", i+1);

//PbgError(OutputFileName);
		if(FALSE == File.Open(Name, "r")){
			g_pGrp->Capture(&m_SaveDIB);
//PbgError("ファイルは開かれた");
			if(m_SaveDIB.Save(Name)){
				StartID = i+1;
				return;
			}
		}
		else{
			File.Close();
		}
	}

	StartID = i;
}
