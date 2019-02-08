/*
 *   CTitleProc.cpp   : タイトル画面の処理
 *
 */

#include "CTitleProc.h"
#include "CGameInfo.h"


static PBG_ITEMINFO g_MainMenuInfo[5] = {
	{"Game Start", "", 0, PBGITM_PARENT,  NULL},
	{"2P Start",   "", 1, PBGITM_PARENT,  NULL},
	{"Config",     "", 2, PBGITM_PARENT,  NULL},
	{"Music Room", "", 3, PBGITM_DEFAULT, NULL},
	{"Exit",       "", 4, PBGITM_PARENT,  NULL}
};

#define ITEM_1PSTART		0
#define ITEM_2PSTART		1
#define ITEM_CONFIG			2
#define ITEM_MUSICROOM		3
#define ITEM_EXIT			4


static PBG_ITEMINFO g_1PMode[3] = {
	{"Story Mode",      "", 42, PBGITM_DEFAULT, NULL},
	{"VS CPU",          "", 45, PBGITM_DEFAULT, NULL},
	{"BossAttack Mode", "", 43, PBGITM_DEFAULT, NULL},
};

#define ITEM_STORYMODE		0
#define ITEM_VS_CPU			1
#define ITEM_BOSSMODE		2


static PBG_ITEMINFO g_ExitYesNoInfo[2] = {
	{"いいえ", "", 6, PBGITM_DEFAULT, NULL},
	{"はい",   "", 5, PBGITM_DEFAULT, NULL},
};

#define ITEM_EXITYES		1
#define ITEM_EXITNO			0

static PBG_ITEMINFO g_2PDevice[5] = {
	{"キーボード ＶＳ キーボード",  "", 35, PBGITM_DEFAULT, NULL},
	{"  パッド   ＶＳ キーボード",  "", 36, PBGITM_DEFAULT, NULL},
	{"キーボード ＶＳ   パッド  ",  "", 37, PBGITM_DEFAULT, NULL},
	{"ＴＣＰ/ＩＰ 接続",            "", 44, PBGITM_DEFAULT, NULL},
	{"▼ 戻る ▼",                  "", 39, PBGITM_EXIT,    NULL},
};

#define ITEM_KEY_VS_KEY		0
#define ITEM_PAD_VS_KEY		1
#define ITEM_KEY_VS_PAD		2
#define ITEM_VS_TCPIP		3

static PBG_ITEMINFO g_ConfigInfo[5] = {
	{"難易度",       "", 7,  PBGITM_PARENT, NULL},
	{"グラフィック", "", 8,  PBGITM_PARENT, NULL},
	{"サウンド",     "", 9,  PBGITM_PARENT, NULL},
	{"入力",         "", 10, PBGITM_PARENT, NULL},
	{"▼ 戻る ▼",   "", 11, PBGITM_EXIT,   NULL},
};

#define ITEM_DIFFICULTY		0
#define ITEM_GRAPHIC		1
#define ITEM_SOUND			2
#define ITEM_INPUT			3

static PBG_ITEMINFO g_GraphicInfo[3] = {
	{"",           "", 12, PBGITM_PARENT,  NULL},				// ＦＰＳの設定
	{"γ補正",     "", 13, PBGITM_RANGE,   NULL, {50, 200, 100}},
	{"▼ 戻る ▼", "", 14, PBGITM_EXIT,    NULL},
};

#define ITEM_FPS		0
#define ITEM_GAMMA		1

static PBG_ITEMINFO g_InputInfo[5] = {
	{"１Ｐ用キーボード", "", 15, PBGITM_PARENT, NULL},
	{"２Ｐ用キーボード", "", 17, PBGITM_PARENT, NULL},
	{"パッド",           "", 19, PBGITM_PARENT, NULL},
	{"設定を初期化する", "", 40, PBGITM_PARENT, NULL},
	{"▼ 戻る ▼",       "", 20, PBGITM_EXIT,   NULL},
};

#define ITEM_1PKEY		0
#define ITEM_2PKEY		1
#define ITEM_PAD		2
#define ITEM_INP_INIT	3


static PBG_ITEMINFO g_InputInitYesNo[2] = {
	{"いいえ", "", 41, PBGITM_DEFAULT, NULL},
	{"はい",   "", 41, PBGITM_DEFAULT, NULL},
};

#define ITEM_INPUTINIT_NO		0
#define ITEM_INPUTINIT_YES		1

//static PBG_ITEMINFO g_DiffInfo[3] = {
static PBG_ITEMINFO g_DiffInfo[2] = {
	{"",           "", 46, PBGITM_PARENT, NULL},
//	{"hoge",           "", 47, PBGITM_PARENT, NULL},
	{"▼ 戻る ▼", "", 25, PBGITM_EXIT,   NULL},
};

#define ITEM_DIF_DIFFICULTY		0
//#define ITEM_DIF_BLUE			1

static PBG_ITEMINFO g_Difficulty[4] = {
	{"Easy",    "", 50, PBGITM_DEFAULT, NULL},
	{"Normal",  "", 51, PBGITM_DEFAULT, NULL},
	{"Hard",    "", 52, PBGITM_DEFAULT, NULL},
	{"Lunatic", "", 53, PBGITM_DEFAULT, NULL},
};

#define ITEM_EASY		0
#define ITEM_NORMAL		1
#define ITEM_HARD		2
#define ITEM_LUNATIC	3

static PBG_ITEMINFO g_SoundInfo[5] = {
	{"",                 "", 21, PBGITM_PARENT,  NULL},						// ＢＧＭ
	{"ＢＧＭの音量",     "", 23, PBGITM_RANGE,   NULL, {0, 255, 255}},
	{"効果音の音量",     "", 24, PBGITM_RANGE,   NULL, {0, 255, 255}},
	{"ＭＩＤＩデバイス", "", 22, PBGITM_PARENT,  NULL},
	{"▼ 戻る ▼",       "", 25, PBGITM_EXIT,    NULL},
};

#define ITEM_BGM			0
#define ITEM_MUSICVOLUME	1
#define ITEM_SOUNDVOLUME	2
#define ITEM_MIDIDEV		3

static PBG_ITEMINFO g_BGMDevInfo[3] = {
	{"ＭＩＤＩ",   "", 26, PBGITM_DEFAULT, NULL},
	{"ＷＡＶＥ",   "", 27, PBGITM_DEFAULT, NULL},
	{"使用しない", "", 28, PBGITM_DEFAULT, NULL},
};

#define ITEM_BGM_MIDI		0
#define ITEM_BGM_WAVE		1
#define ITEM_BGM_NONE		2

static PBG_ITEMINFO g_FPSInfo[7] = {
	{"自動調整",     "", 30, PBGITM_DEFAULT, NULL},
	{"６０ｆｐｓ",   "", 31, PBGITM_DEFAULT, NULL},
	{"３０ｆｐｓ",   "", 32, PBGITM_DEFAULT, NULL},
	{"２０ｆｐｓ",   "", 33, PBGITM_DEFAULT, NULL},
	{"１５ｆｐｓ",   "", 48, PBGITM_DEFAULT, NULL},
	{"１０ｆｐｓ",   "", 49, PBGITM_DEFAULT, NULL},
	{"ウェイト無し", "", 34, PBGITM_DEFAULT, NULL},
};

#define ITEM_FPS_AUTO		0
#define ITEM_FPS_60			1
#define ITEM_FPS_30			2
#define ITEM_FPS_20			3
#define ITEM_FPS_15			4
#define ITEM_FPS_10			5
#define ITEM_FPS_NONE		6

static PBG_ITEMINFO	g_MidiInfo[20];


static PBG_PROCID	g_TitleProcID        = PBGPROC_OK;
static BOOL			g_bMusicDevChanged   = TRUE;
static BOOL			g_bFpsChanged        = TRUE;
static BOOL			g_bMidiDevChanged    = TRUE;
static BOOL			g_bDifficultyChanged = TRUE;



PBGWND_RET MainMenuFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	switch(ItemID){
	case ITEM_1PSTART:
/*		if(KeyCode == KEY_SHOT){
			pSys->SetGameMode(GMODE_STORY);
			g_TitleProcID = PBGPROC_SELECT;
			return RET_OK;
		}*/
	break;

	case ITEM_2PSTART:
	break;

	case ITEM_CONFIG:
	break;

	case ITEM_MUSICROOM:
		if(KeyCode == KEY_SHOT){
			g_TitleProcID = PBGPROC_MUSIC;
			return RET_OK;
		}
	break;
	}

	return RET_DEFAULT;
}


PBGWND_RET Sel1PModeFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	if(KeyCode != KEY_SHOT) return RET_DEFAULT;

	switch(ItemID){
		case ITEM_STORYMODE:	pSys->SetGameMode(GMODE_STORY);			break;
		case ITEM_VS_CPU:		pSys->SetGameMode(GMODE_VS_CPU);		break;
		case ITEM_BOSSMODE:		pSys->SetGameMode(GMODE_BOSSATTACK);	break;

//		case ITEM_KEY_VS_KEY:	pSys->SetInputMode(IMODE_2PDEFAULT);	break;
//		case ITEM_PAD_VS_KEY:	pSys->SetInputMode(IMODE_PADKEY);		break;
//		case ITEM_KEY_VS_PAD:	pSys->SetInputMode(IMODE_KEYPAD);		break;

		default:
		return RET_DEFAULT;
	}

	pSys->SetCharacterID(CHARACTER_VIVIT, CHARACTER_VIVIT);
	g_TitleProcID = PBGPROC_SELECT;
	return RET_OK;

//	pSys->SetGameMode(GMODE_VS_2P);	// ２Ｐモードで
//	g_TitleProcID = PBGPROC_SELECT;	// 選択画面へと

//	return RET_OK;
}


PBGWND_RET Sel2PDevFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	if(KeyCode != KEY_SHOT) return RET_DEFAULT;

	switch(ItemID){
		case ITEM_KEY_VS_KEY:	pSys->SetInputMode(IMODE_2PDEFAULT);	break;
		case ITEM_PAD_VS_KEY:	pSys->SetInputMode(IMODE_PADKEY);		break;
		case ITEM_KEY_VS_PAD:	pSys->SetInputMode(IMODE_KEYPAD);		break;
		case ITEM_VS_TCPIP:		pSys->SetInputMode(IMODE_TCPIP);		break;

		default:
		return RET_DEFAULT;
	}

	pSys->SetCharacterID(CHARACTER_VIVIT, CHARACTER_VIVIT);
	pSys->SetGameMode(GMODE_VS_2P);	// ２Ｐモードで
	g_TitleProcID = PBGPROC_SELECT;	// 選択画面へと

	return RET_OK;
}


PBGWND_RET ExitYesNoFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	switch(ItemID){
	case ITEM_EXITYES:
		if(KeyCode == KEY_SHOT){
			g_TitleProcID = PBGPROC_CLOSE;
			return RET_OK;
		}
	break;

	case ITEM_EXITNO:
		if(KeyCode == KEY_SHOT){
			return RET_CLOSE;
		}
	break;
	}

	return RET_DEFAULT;
}


PBGWND_RET ConfigFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	return RET_DEFAULT;
}


PBGWND_RET DiffFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	switch(ItemID){
	case ITEM_DIF_DIFFICULTY:
		if(g_bDifficultyChanged){
			strcpy(g_DiffInfo[ITEM_DIF_DIFFICULTY].m_ItemName, "難易度：");
			switch(pSys->GetBaseDifficulty()){
			case GDIFF_EASY:
				strcat(g_DiffInfo[ITEM_DIF_DIFFICULTY].m_ItemName, " Easy   ");
			break;
			case GDIFF_NORMAL:
				strcat(g_DiffInfo[ITEM_DIF_DIFFICULTY].m_ItemName, " Normal ");
			break;
			case GDIFF_HARD:
				strcat(g_DiffInfo[ITEM_DIF_DIFFICULTY].m_ItemName, " Hard   ");
			break;
			case GDIFF_LUNATIC:
				strcat(g_DiffInfo[ITEM_DIF_DIFFICULTY].m_ItemName, " Lunatic");
			break;
			}

			g_bDifficultyChanged = FALSE;
			return RET_UPDATE;
		}
	break;

//	case ITEM_DIF_BLUE:
//	break;
	}

	return RET_DEFAULT;
}


PBGWND_RET DifficultyFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	DWORD	Dif;

	if(!(KeyCode & KEY_SHOT))   return RET_DEFAULT;

	Dif = pSys->GetBaseDifficulty();

	switch(ItemID){
	case ITEM_EASY:
		if(GDIFF_EASY != Dif){
			g_bDifficultyChanged = TRUE;
			pSys->SetBaseDifficulty(GDIFF_EASY);
		}
	break;
	case ITEM_NORMAL:
		if(GDIFF_NORMAL != Dif){
			g_bDifficultyChanged = TRUE;
			pSys->SetBaseDifficulty(GDIFF_NORMAL);
		}
	break;
	case ITEM_HARD:
		if(GDIFF_HARD != Dif){
			g_bDifficultyChanged = TRUE;
			pSys->SetBaseDifficulty(GDIFF_HARD);
		}
	break;
	case ITEM_LUNATIC:
		if(GDIFF_LUNATIC != Dif){
			g_bDifficultyChanged = TRUE;
			pSys->SetBaseDifficulty(GDIFF_LUNATIC);
		}
	break;
	}

	return RET_CLOSE;
}


PBGWND_RET GraphicFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	int		Gamma;
	int		dir, Current;

	pSys->EnableGammaWindow(FALSE);

	switch(ItemID){
	case ITEM_FPS:
		if(g_bFpsChanged){
			strcpy(g_GraphicInfo[ITEM_FPS].m_ItemName, "描画間隔：");
			switch(pSys->GetFPS()){
			case PBGFPS_60:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, " ６０ｆｐｓ ");
			break;

			case PBGFPS_30:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, " ３０ｆｐｓ ");
			break;

			case PBGFPS_20:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, " ２０ｆｐｓ ");
			break;

			case PBGFPS_15:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, " １５ｆｐｓ ");
			break;

			case PBGFPS_10:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, " １０ｆｐｓ ");
			break;

			case PBGFPS_AUTO:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, "    自動    ");
			break;

			case PBGFPS_WITHOUT:
				strcat(g_GraphicInfo[ITEM_FPS].m_ItemName, "ウェイト無し");
			break;
			}

			g_bFpsChanged = FALSE;
			return RET_UPDATE;
		}
	break;

	case ITEM_GAMMA:
		pSys->EnableGammaWindow(TRUE);

		Gamma = pSys->GetGamma();
		dir   = 0;

		switch(KeyCode){
		case KEY_LEFT:				dir = -6;	break;
		case KEY_RIGHT:				dir = +6;	break;
		case KEY_LEFT  | KEY_SHIFT:	dir = -2;	break;
		case KEY_RIGHT | KEY_SHIFT:	dir = +2;	break;

		// 同時押しでリセット //
		case KEY_LEFT | KEY_RIGHT:	Gamma = 100;	break;

//		case KEY_CANCEL:
		case KEY_BOMB:	case KEY_MENU:
			pSys->EnableGammaWindow(FALSE);
		return RET_DEFAULT;

		default:
			Current = g_GraphicInfo[ITEM_GAMMA].m_RangeInfo.m_Current;
			if(Gamma != Current){
				Gamma = Current;
				break;
			}
		return RET_DEFAULT;
		}

		g_GraphicInfo[ITEM_GAMMA].m_RangeInfo.m_Current = Gamma;
		g_GraphicInfo[ITEM_GAMMA].m_RangeInfo.SeekR(dir);
		pSys->SetGamma(Gamma + dir);
	return RET_OK;

	default:
	break;
	}

	return RET_DEFAULT;
}


PBGWND_RET InputFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	BOOL	bIsActive = pSys->IsActiveInputWindow();
	WORD	Keyboard  = pSys->GetSystemKeyCode();


	// 入力ウィンドウが非アクティブな場合 //
	if(FALSE == bIsActive){
		if((KEY_MENU == KeyCode) || (KEY_BOMB == KeyCode)){
//			pSys->EnableInputWindow(CFGW_DISABLE);
			return RET_CLOSE;
		}

		if(KEY_SHOT != KeyCode) return RET_DEFAULT;

		// どの項目が選択されたか //
		switch(ItemID){
			case ITEM_1PKEY:
//				g_InputInfo[ITEM_1PKEY].m_HelpID = bIsActive ? 16 : 15;
//				pSys->GetInputHelpString(g_InputInfo[ITEM_1PKEY].m_HelpOption);
				pSys->EnableInputWindow(CFGW_FULLKEY);
			break;

			case ITEM_2PKEY:
//				g_InputInfo[ITEM_2PKEY].m_HelpID = bIsActive ? 18 : 17;
//				pSys->GetInputHelpString(g_InputInfo[ITEM_2PKEY].m_HelpOption);
				pSys->EnableInputWindow(CFGW_HALFKEY);
			break;

			case ITEM_PAD:
//				g_InputInfo[ITEM_PAD].m_HelpID = 19;//bIsActive ? 18 : 17;
//				pSys->GetInputHelpString(g_InputInfo[ITEM_2PKEY].m_HelpOption);
				pSys->EnableInputWindow(CFGW_PAD);
			break;

			case ITEM_INP_INIT:
			break;

			default:
				pSys->EnableInputWindow(CFGW_DISABLE);
			break;
		}

		return RET_DEFAULT;
	}

	// 入力ウィンドウがアクティブな場合 //
	if(KEY_MENU == KeyCode){
		// パッドの場合、パッドの MENU ボタンでの脱出は出来ない //
		if((ITEM_PAD == ItemID) && (pSys->IsPadInput())){
			return RET_NONUPDATE;
		}

		pSys->EnableInputWindow(CFGW_DISABLE);
		return RET_ACTIVATE;
	}

	return RET_NONUPDATE;
}


PBGWND_RET InputInitFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	if(KEY_SHOT != KeyCode) return RET_DEFAULT;

	switch(ItemID){
		case ITEM_INPUTINIT_NO:
		return RET_CLOSE;

		case ITEM_INPUTINIT_YES:
			pSys->InitInputCfgData();
		return RET_CLOSE;
	}

	return RET_DEFAULT;
}


PBGWND_RET SoundFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	int			dir = 0;
	int			Current;
	int			Volume;

	if(KeyCode & KEY_LEFT)  dir -= 12;
	if(KeyCode & KEY_RIGHT) dir += 12;
	if(KeyCode & KEY_SHIFT) dir /= 4;


	switch(ItemID){
	case ITEM_BGM:
		if(g_bMusicDevChanged){
			switch(g_pMusic->GetMusicDev()){
			case MDEV_MIDI:
				strcpy(g_SoundInfo[ITEM_BGM].m_ItemName, "ＢＧＭ： ＭＩＤＩ ");
			break;

			case MDEV_WAVE:
				strcpy(g_SoundInfo[ITEM_BGM].m_ItemName, "ＢＧＭ： ＷＡＶＥ ");
			break;

			case MDEV_NULL:
				strcpy(g_SoundInfo[ITEM_BGM].m_ItemName, "ＢＧＭ：再生しない");
			break;
			}

			g_bMusicDevChanged = FALSE;

			return RET_UPDATE;
		}
	break;

	case ITEM_MUSICVOLUME:
		Volume = g_pMusic->GetMusicVolume();
		g_SoundInfo[ITEM_MUSICVOLUME].m_RangeInfo.SeekR(dir);
		Current = g_SoundInfo[ITEM_MUSICVOLUME].m_RangeInfo.m_Current;

		if(Volume != Current){
			g_pMusic->SetMusicVolume(Current);
		}
	break;

	case ITEM_SOUNDVOLUME:
		if(g_pMusic){
			Volume = g_pMusic->GetSoundVolume();
			g_SoundInfo[ITEM_SOUNDVOLUME].m_RangeInfo.SeekR(dir);
			Current = g_SoundInfo[ITEM_SOUNDVOLUME].m_RangeInfo.m_Current;

			if(Volume != Current){
				g_pMusic->SetSoundVolume(Current);
			}
		}
/*
		if(g_pSnd){
			Volume = g_pSnd->GetMasterVolume();
			g_SoundInfo[ITEM_SOUNDVOLUME].m_RangeInfo.SeekR(dir);
			Current = g_SoundInfo[ITEM_SOUNDVOLUME].m_RangeInfo.m_Current;

			if(Volume != Current){
				g_pSnd->SetMasterVolume(Current);
			}
		}
*/
	break;

	case ITEM_MIDIDEV:
		if(g_bMidiDevChanged){
			BYTE	n = g_pMusic->GetCurrentMidDev();
			strcpy(g_SoundInfo[ITEM_MIDIDEV].m_HelpOption, g_MidiInfo[n].m_ItemName);
			g_bMidiDevChanged = FALSE;
			return RET_UPDATE;
		}
	}

	return RET_DEFAULT;
}


PBGWND_RET BGMDevFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	if(!(KeyCode & KEY_SHOT))   return RET_DEFAULT;

	switch(ItemID){
	case ITEM_BGM_MIDI:
		if(MDEV_MIDI != g_pMusic->GetMusicDev()){
			g_bMusicDevChanged = TRUE;
			g_pMusic->ChangeMusicDev(MDEV_MIDI);
		}
	break;

	case ITEM_BGM_WAVE:
		if(MDEV_WAVE != g_pMusic->GetMusicDev()){
			g_bMusicDevChanged = TRUE;
			g_pMusic->ChangeMusicDev(MDEV_WAVE);
		}
	break;

	case ITEM_BGM_NONE:
		if(MDEV_NULL != g_pMusic->GetMusicDev()){
			g_bMusicDevChanged = TRUE;
			g_pMusic->ChangeMusicDev(MDEV_NULL);
		}
	break;
	}

	return RET_CLOSE;
}


PBGWND_RET MidiDevFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	BYTE		MidiID;

	if(!(KeyCode & KEY_SHOT))   return RET_DEFAULT;

	MidiID = g_pMusic->GetCurrentMidDev();

	if(ItemID != MidiID){
		g_bMidiDevChanged = TRUE;
		g_pMusic->ChangeMidDev(ItemID);
	}

	return RET_CLOSE;
}


PBGWND_RET FpsFunc(CGameInfo *pSys, BYTE ItemID, WORD KeyCode)
{
	DWORD	Fps;

	if(!(KeyCode & KEY_SHOT))   return RET_DEFAULT;

	Fps = pSys->GetFPS();

	switch(ItemID){
	case ITEM_FPS_60:
		if(PBGFPS_60 != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_60);
		}
	break;

	case ITEM_FPS_30:
		if(PBGFPS_30 != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_30);
		}
	break;

	case ITEM_FPS_20:
		if(PBGFPS_20 != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_20);
		}
	break;

	case ITEM_FPS_15:
		if(PBGFPS_15 != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_15);
		}
	break;

	case ITEM_FPS_10:
		if(PBGFPS_10 != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_10);
		}
	break;

	case ITEM_FPS_AUTO:
		if(PBGFPS_AUTO != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_AUTO);
		}
	break;

	case ITEM_FPS_NONE:
		if(PBGFPS_WITHOUT != Fps){
			g_bFpsChanged = TRUE;
			pSys->SetFPS(PBGFPS_WITHOUT);
		}
	break;
	}

	return RET_CLOSE;
}


// コンストラクタ //
CTitleProc::CTitleProc(CGameInfo *pSys)
	: CScene(pSys)
	, m_MainMenu(pSys), m_ExitYesNo(pSys), m_Config(pSys)
	, m_Diff(pSys), m_Graphic(pSys), m_Input(pSys), m_Sound(pSys)
	, m_Difficulty(pSys)
	, m_BGMDev(pSys), m_Fps(pSys), m_MidiDev(pSys)
	, m_2PDev(pSys), m_1PMode(pSys), m_InputInit(pSys)
{
	BOOL	bRet;
	int		i, n;

	m_bInitialized = FALSE;
	m_pBack        = NULL;

	// メインメニュー //
	bRet = m_MainMenu.SetItem(NULL				// 親ウィンドウ
							, MainMenuFunc		// コールバック関数
							, g_MainMenuInfo	// 作成情報
							, 5					// 項目数
							, 125				// 横幅
							, TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// コンフィグ用ウィンドウ //
	bRet = m_Config.SetItem(&m_MainMenu		// 親ウィンドウ
						  , ConfigFunc		// コールバック関数
						  , g_ConfigInfo	// 作成情報
						  , 5				// 項目数
						  , 125				// 横幅
						  , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// 難易度設定用ウィンドウ //
	bRet = m_Diff.SetItem(&m_Config			// 親ウィンドウ
					    , DiffFunc			// コールバック関数
					    , g_DiffInfo		// 作成情報
					    , 2					// 項目数
					    , 225				// 横幅
					    , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// グラフィック用ウィンドウ //
	bRet = m_Graphic.SetItem(&m_Config			// 親ウィンドウ
						   , GraphicFunc		// コールバック関数
						   , g_GraphicInfo		// 作成情報
						   , 3					// 項目数
						   , 225				// 横幅
						   , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// 入力コンフィグ用ウィンドウ //
	bRet = m_Input.SetItem(&m_Config		// 親ウィンドウ
						 , InputFunc		// コールバック関数
						 , g_InputInfo		// 作成情報
						 , 5				// 項目数
						 , 190				// 横幅
						 , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// 入力・設定初期化ウィンドウ //
	bRet = m_InputInit.SetItem(&m_Input				// 親ウィンドウ
							,  InputInitFunc		// コールバック関数
							,  g_InputInitYesNo		// 作成情報
							,  2					// 項目数
							,  125					// 横幅
							,  TWIN_MAINMENU);		// ヘルプＩＤ
	if(FALSE == bRet) return;

	// サウンド用ウィンドウ //
	bRet = m_Sound.SetItem(&m_Config		// 親ウィンドウ
						 , SoundFunc		// コールバック関数
						 , g_SoundInfo		// 作成情報
						 , 5				// 項目数
						 , 250				// 横幅
						 , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// ＢＧＭデバイス選択ウィンドウ //
	bRet = m_BGMDev.SetItem(&m_Sound		// 親ウィンドウ
						  , BGMDevFunc		// コールバック関数
						  , g_BGMDevInfo	// 作成情報
						  , 3				// 項目数
						  , 125				// 横幅
						  , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// ＦＰＳ設定ウィンドウ //
	bRet = m_Fps.SetItem(&m_Graphic			// 親ウィンドウ
					   , FpsFunc			// コールバック関数
					   , g_FPSInfo			// 作成情報
					   , 7					// 項目数
					   , 125				// 横幅
					   , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// 難易度設定ウィンドウ //
	bRet = m_Difficulty.SetItem(&m_Diff				// 親ウィンドウ
							   , DifficultyFunc		// コールバック関数
							   , g_Difficulty		// 作成情報
							   , 4					// 項目数
							   , 96					// 横幅
							   , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// ＭＩＤＩデバイス選択ウィンドウ //
	n = g_pMusic->GetNumMidDev();
	for(i=0; i<n; i++){
		g_pMusic->GetMidDevName(g_MidiInfo[i].m_ItemName, i);
		g_MidiInfo[i].m_ItemFlag = PBGITM_DEFAULT;
		g_MidiInfo[i].m_HelpID   = 29;
	}

	if(FALSE == g_pMusic->IsEnableMid()){
		g_SoundInfo[ITEM_MIDIDEV].m_ItemFlag = PBGITM_DEFAULT;
	}

	bRet = m_MidiDev.SetItem(&m_Sound			// 親ウィンドウ
						   , MidiDevFunc		// コールバック関数
						   , g_MidiInfo			// 作成情報
						   , n					// 項目数
						   , 275				// 横幅
						   , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// ２Ｐ対戦に使用する入力デバイス //
	bRet = m_2PDev.SetItem(&m_MainMenu		// 親ウィンドウ
						 , Sel2PDevFunc		// コールバック関数
						 , g_2PDevice		// 作成情報
						 , 5				// 項目数
						 , 275				// 横幅
						 , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// １Ｐモード選択に使用する入力デバイス //
	bRet = m_1PMode.SetItem(&m_MainMenu		// 親ウィンドウ
						 , Sel1PModeFunc	// コールバック関数
						 , g_1PMode			// 作成情報
						 , 3				// 項目数
						 , 180				// 横幅
						 , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// 終了時のＹｅｓ/Ｎｏ判別用ウィンドウ //
	bRet = m_ExitYesNo.SetItem(&m_MainMenu		// 親ウィンドウ
							 , ExitYesNoFunc	// コールバック関数
							 , g_ExitYesNoInfo	// 作成情報
							 , 2				// 項目数
							 , 125				// 横幅
							 , TWIN_MAINMENU);	// ヘルプＩＤ
	if(FALSE == bRet) return;

	// 親ウィンドウと子ウィンドウを接続する //
	g_MainMenuInfo[ITEM_CONFIG].m_pChild   = &m_Config;
	g_MainMenuInfo[ITEM_EXIT].m_pChild     = &m_ExitYesNo;
	g_MainMenuInfo[ITEM_1PSTART].m_pChild  = &m_1PMode;
	g_MainMenuInfo[ITEM_2PSTART].m_pChild  = &m_2PDev;
	g_ConfigInfo[ITEM_DIFFICULTY].m_pChild = &m_Diff;
	g_ConfigInfo[ITEM_GRAPHIC].m_pChild    = &m_Graphic;
	g_ConfigInfo[ITEM_INPUT].m_pChild      = &m_Input;
	g_ConfigInfo[ITEM_SOUND].m_pChild      = &m_Sound;
	g_SoundInfo[ITEM_BGM].m_pChild         = &m_BGMDev;
	g_SoundInfo[ITEM_MIDIDEV].m_pChild     = &m_MidiDev;
	g_GraphicInfo[ITEM_FPS].m_pChild       = &m_Fps;
	g_InputInfo[ITEM_INP_INIT].m_pChild    = &m_InputInit;
	g_DiffInfo[ITEM_DIF_DIFFICULTY].m_pChild = &m_Difficulty;

	// 範囲系メンバの初期化を行う //
	g_GraphicInfo[ITEM_GAMMA].m_RangeInfo.m_Current     = pSys->GetGamma();
	g_SoundInfo[ITEM_MUSICVOLUME].m_RangeInfo.m_Current = g_pMusic->GetMusicVolume();
	g_SoundInfo[ITEM_SOUNDVOLUME].m_RangeInfo.m_Current = g_pMusic->GetSoundVolume();
	g_DiffInfo[ITEM_DIFFICULTY].m_RangeInfo.m_Current   = 128;

	m_bInitialized = TRUE;
}


// デストラクタ //
CTitleProc::~CTitleProc()
{
	if(m_pBack){
		g_pGrp->ReleaseOffsSurface(m_pBack);
	}
}


// 初期化をする //
IBOOL CTitleProc::Initialize(void)
{
	const int	WindowOx = 70;
	const int	WindowOy = 120;
	DWORD		NumPads;

	if(FALSE == m_bInitialized) return FALSE;

	// 入力モードをデフォルトに //
	m_pSys->SetInputMode(IMODE_1P);

	if(g_pMusic){
		g_pMusic->Load(0);
		g_pMusic->Play();
	}

	m_MainMenu.SetWindowPos(WindowOx, WindowOy);
//	m_MainMenu.EnableItem(1, FALSE);
//	m_MainMenu.EnableItem(3, FALSE);

	if(g_pInp) NumPads = g_pInp->GetNumJoyDriver();
	else       NumPads = 0;

	// パッドが存在しない場合、以下の項目は選択出来ない //
	if(0 == NumPads){
		// ２Ｐスタート・パッドを含む //
		m_2PDev.EnableItem(ITEM_KEY_VS_PAD, FALSE);
		m_2PDev.EnableItem(ITEM_PAD_VS_KEY, FALSE);
//		m_2PDev.EnableItem(ITEM_PAD_VS_PAD, FALSE);

		// 入力デバイス設定・パッド //
		m_Input.EnableItem(ITEM_PAD, FALSE);
	}
//	m_Input.EnableItem(ITEM_1PKEY, FALSE);
//	m_Input.EnableItem(ITEM_2PKEY, FALSE);

	m_Fps.EnableItem(ITEM_FPS_AUTO, FALSE);

	if(FALSE == g_pMusic->IsEnableMid()){
		m_BGMDev.EnableItem(ITEM_BGM_MIDI, FALSE);
		m_Sound.EnableItem(ITEM_MIDIDEV, FALSE);
	}

	if(FALSE == g_pMusic->IsEnableWave()){
		m_BGMDev.EnableItem(ITEM_BGM_WAVE, FALSE);
		m_Sound.EnableItem(ITEM_SOUNDVOLUME, FALSE);
	}

//	m_Config.EnableItem(0, FALSE);

	//m_1PMode.EnableItem(2, FALSE);

	const int FontSize = 20;
	m_Config.SetWindowPos(   15, FontSize * 3 + 5);
	m_ExitYesNo.SetWindowPos(15, FontSize * 5 + 5);

	m_Diff.SetWindowPos(   15, FontSize * 1 + 5);
	m_Graphic.SetWindowPos(15, FontSize * 2 + 5);
	m_Sound.SetWindowPos(  15, FontSize * 3 + 5);
	m_Input.SetWindowPos(  15, FontSize * 4 + 5);

	m_MidiDev.SetWindowPos(250, 0);
	m_BGMDev.SetWindowPos(250, FontSize * 0 + 5);
	m_Fps.SetWindowPos(225,    FontSize * 0 + 5);
	m_Difficulty.SetWindowPos(225, FontSize * 0 + 5);

	m_2PDev.SetWindowPos(15, FontSize * 2 + 5);
	m_InputInit.SetWindowPos(190, FontSize * 3 + 5);

	m_1PMode.SetWindowPos(16, FontSize * 1 + 5);

	m_Difficulty.OnClose();
	m_Fps.OnClose();
	m_MidiDev.OnClose();
	m_BGMDev.OnClose();
	m_Sound.OnClose();
	m_Graphic.OnClose();
	m_Diff.OnClose();
	m_InputInit.OnClose();
	m_Input.OnClose();
	m_Config.OnClose();
	m_ExitYesNo.OnClose();
	m_2PDev.OnClose();
	m_1PMode.OnClose();			////////////
	m_MainMenu.OnActivate();


	// 鳩祭補正！ //
//	m_MainMenu.EnableItem(4, FALSE);

	g_TitleProcID = PBGPROC_OK;

	// マウスカーソルを有効化 //
	m_pSys->EnableMouseCursor(TRUE);
	g_pCursor->SetPosition(WindowOx+90, WindowOy+8);

	if(NULL == m_pBack){
		if(g_pGrp->CreateSurface(&m_pBack, 640, 480)){
			Pbg::CGrpLoader::LoadP(m_pBack->Lock(), "GRAPH.DAT", "GRP/タイトル");
			m_pBack->Unlock();
//			m_pBack->LoadP("GRAPH.DAT", "GRP/タイトル", 0, 0);
		}
		else{
			return FALSE;
		}
	}

	return TRUE;
}







//BYTE	buf[128+128];
//BYTE	buf2[128+128];


// １フレーム分だけ動作させる //
PBG_PROCID CTitleProc::Move(void)
{
	static BOOL KeyFlag = FALSE;
	WORD		KeyCode;
	BYTE		MouseCode;
	POINT		Pos;

	KeyCode   = m_pSys->GetSystemKeyCode();
	MouseCode = g_pCursor->GetButtonState();
	g_pCursor->GetPosition(&Pos);

	m_MainMenu.OnKeyDown(KeyCode);
	m_MainMenu.OnMouseMove(&Pos, MouseCode);

	m_MainMenu.Move();

	if(PBGPROC_OK != g_TitleProcID){
		if(m_pBack){
			g_pGrp->ReleaseOffsSurface(m_pBack);
			m_pBack = NULL;
		}

		m_MainMenu.OnClose();
	}
//	g_pMusic->GetWaveScope(buf);

	return g_TitleProcID;
}


// 描画する //
IVOID CTitleProc::Draw(void)
{
	RECT		src = {0, 0, 640, 480};

	if(m_pBack){
		g_pGrp->BltN(&src, 0, 0, m_pBack);
	}

	m_MainMenu.Draw();
	g_pTextWindow->Draw();	// 上に同じ...
}
/*
	int				i, x, y, y1, y2;
	int				dy, dy1, dy2, dy3, dy4;
	DWORD			c, c1, c2, c3, c4;
	D3DTLVERTEX		tlv[256*2];

	g_pGrp->SetRenderStateEx(GRPST_NORMAL);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);

	if(g_pGrp->Begin3DScene()){
		for(x=0; x<128; x++){
			i = x*2;
//			dy1 = ((char)buf[i*2+0]/4);
//			dy2 = ((char)buf[i*2+1]/4);

//			Set2DPointC(tlv+i  , i*2+320, 200+(dy1+dy2)/2, 0, 0, RGB_MAKE(255,   0,   0));
//			Set2DPointC(tlv+i+128  , i*2+320, 200+dy, 0, 0, RGB_MAKE(  0,   0, 255));

			y1  = (char)buf[i];
			y2  = (char)buf2[i];
			y   = y1;//(y1*2 + y2*1) / 4;

			if(y > 0) dy1 =  ((double)log( y * 2));//((char)buf[i]/2);
			else      dy1 = -((double)log(-y * 2));
			c1  = 196;//255-min(255, abs((char)y) * 2);
			c   = RGB_MAKE(c1, c1, c1);
			Set2DPointC(tlv+x, 80+x*2, 450+dy1, 0, 0, c);//RGB_MAKE(  0,   0, 255));
//			Set2DPointC(tlv+i*2+1 , 180+i*2, 100, 0, 0, c);//RGB_MAKE(255, 255, 255));

//			dy = ((char)buf[i*2+1]/3);
//			c  = 255 - abs((char)buf[i*2+1]) * 3 / 2;
//			c  = RGB_MAKE(c, c, c);
//			Set2DPointC(tlv+i*2+256  , 80+i*3, 300+dy,   0, 0, c);//RGB_MAKE(255,   0,   0));
//			Set2DPointC(tlv+i*2+257  , 80+i*3, 300+dy/5, 0, 0, c);//RGB_MAKE(255, 255, 255));

			buf2[i] = buf[i];
		}
		g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv,     x);
//		g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv+256, 256);

		g_pGrp->End3DScene();
	}
*/
