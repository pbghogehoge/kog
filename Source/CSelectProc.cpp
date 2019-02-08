/*
 *   CSelectProc.cpp   : 選択画面
 *
 */

#include "CSelectProc.h"
#include "CGameInfo.h"

#include "CVivit.h"			// 主人公の定義
#include "CStg1Boss.h"		// ミリアの定義
#include "CStg2Boss.h"		// めい＆まいの定義
#include "CStg3Boss.h"		// ゲイツの定義
#include "CStg4Boss.h"		// マリーの定義
#include "CStg5Boss.h"		// エーリッヒの定義
#include "CMorgan.h"		// モーガンの定義
#include "CMuse.h"			// ミューズの定義
#include "CYuka.h"			// 幽香の定義
//#include "SECtrl.h"



/***** [ 定数 ] *****/
#define SBT_FAST		0x00	// 高速動作モード
#define SBT_STOP		0x01	// 停止中
#define SBT_NORM		0x02	// 通常動作モード

#define SPP_NUMITEMS	5	// 項目数

#define SPP_SPD		0x00	// 移動速度
#define SPP_SIZE	0x01	// サイズ
#define SPP_STR		0x02	// 攻撃力
#define SPP_DEF		0x03	// 防御力
#define SPP_CHG		0x04	// 溜め速度



/***** [ 静的メンバ ] *****/
BOOL	SPCharInfo::m_bInitialized = FALSE;	// 静的メンバが初期化されていれば真
int		SPCharInfo::m_MaxParam[5];			// 各パラメータの最大値



// コンストラクタ //
SPCharInfo::SPCharInfo()
{
//	m_Rnd.SetSeed(0);

/*
	m_CharName.SetFontID(FONTID_MS_GOTHIC);
	m_CharName.SetColor(RGB(255, 255, 255));
	m_CharName.SetBkBlendColor(RGB(32, 32, 32));
	m_CharName.SetSize(24);
*/
	//バイナリエディタ部の初期化 //
	m_BinTexMode  = SBT_NORM;		// 動作モード
	m_BinTexCount = 10+m_Rnd.Get()%20;	// 状態推移用カウンタ
	m_BinTexY     = m_Rnd.Get() % 512;	// テクスチャのＹ座標

	// 名前格納用 Surface //
	m_pNameSurf = NULL;

	// プレイヤーＩＤを一応初期化しておく //
	m_PlayerID = 0;

	InitCharParams();
}


// デストラクタ //
SPCharInfo::~SPCharInfo()
{
	if(m_pNameSurf){
		g_pGrp->ReleaseOffsSurface(m_pNameSurf);
		m_pNameSurf = NULL;
	}
}


// その名の通り、初期化 //
FVOID SPCharInfo::Initialize(BYTE PlayerID, CHARACTER_ID CharID)
{
	// パラメータ系の初期化 //
	memset(m_DispParams, 0, sizeof(m_DispParams));

	m_PlayerID      = PlayerID;
	m_Angle         = (BYTE)m_Rnd.Get();
	m_AutoRepCount  = 0;
	m_bRandomSelect = FALSE;

	Select(CharID);

	if(CHARACTER_UNKNOWN == CharID) m_SelectedAlpha = 255;
	else                            m_SelectedAlpha = 0;
}


// 更新処理 //
FVOID SPCharInfo::Proceed(WORD KeyCode, int NumCharacters)
{
	CHARACTER_ID	CharID;
	int				i, dx;

	// ぐるぐる回っている奴を更新する //
	if(0 == m_PlayerID) m_Angle += 1;
	else                m_Angle -= 1;

	// 一番下のレイヤーで「うにうに」している奴を更新する //
	switch(m_BinTexMode){
		case SBT_NORM:	m_BinTexY -= 256*6;		break;
		case SBT_FAST:	m_BinTexY -= 256*10;	break;
		case SBT_STOP:	break;
	}
	if(m_BinTexY < -(512 * 256)){
		m_BinTexY += 512 * 256;
	}

	if(m_BinTexCount){
		m_BinTexCount--;
	}
	else{
		// 状態推移 //
		switch(m_BinTexMode){
			case SBT_NORM:	m_BinTexMode = SBT_STOP;	break;
			case SBT_FAST:	m_BinTexMode = SBT_STOP;	break;

			case SBT_STOP:
				m_BinTexMode = ((m_Rnd.Get()>>1)&1) ? SBT_NORM : SBT_FAST;
			break;
		}

		// カウンタの初期化 //
		switch(m_BinTexMode){
			case SBT_NORM:	m_BinTexCount =  80+m_Rnd.Get()%20;	break;
			case SBT_STOP:	m_BinTexCount =  10+m_Rnd.Get()%20;	break;
			case SBT_FAST:	m_BinTexCount = 140+m_Rnd.Get()%20;	break;
		}
	}

	for(i=0; i<SPP_NUMITEMS; i++){
		dx = (m_Params[i] * 4 + m_DispParams[i] * 14) / 18;

		if(abs(dx - m_DispParams[i]) < 1){
			m_DispParams[i] = m_Params[i];
		}
		else{
			m_DispParams[i] = dx;
		}
	}

	CharID = m_CharacterID;

	if(m_SelectedAlpha){
		m_SelectedAlpha = min(255, m_SelectedAlpha+16);

		// ランダムセレクト //
		if(m_bRandomSelect && (m_SelectedAlpha < 16 * 8)){
			Select((CHARACTER_ID)((m_Rnd.Get()>>1) % NumCharacters));
		}
		else{
			m_bRandomSelect = FALSE;
		}
	}
	else if(KeyCode & KEY_SHOT){
		m_SelectedAlpha = 1;

		if(KeyCode & KEY_SHIFT){
			m_bRandomSelect = TRUE;
		}
	}

	// 何も押されていなければ、カウンタをリセットする //
	if(0 == KeyCode){
		m_AutoRepCount = 0;
	}

	// カウンタが残っていれば、デクリメントして、リターン //
	if(m_AutoRepCount){
		m_AutoRepCount--;
		return;
	}


//	NumCharacters = 9;
	switch(KeyCode){
	case KEY_LEFT:
		CharID = (CHARACTER_ID)((CharID + NumCharacters - 1) % NumCharacters);
	break;

	case KEY_RIGHT:
		CharID = (CHARACTER_ID)((CharID + 1) % NumCharacters);
	break;

	case KEY_BOMB:
		m_SelectedAlpha = 0;
		m_bRandomSelect = FALSE;
	return;

	default:
	return;
	}

	if(FALSE == m_SelectedAlpha){
		m_AutoRepCount = 10;
		Select(CharID);
	}
}


// キャラクタ名の描画(左上の座標指定) //
FVOID SPCharInfo::DrawCharName(int ox, int oy)
{
	RECT	rc;
	int		w, h;

	if(NULL == m_pNameSurf) return;

//	w = m_CharName.GetWidth();
//	h = m_CharName.GetHeight();
	w = 160;
	h = 25;

	ox = ox + (320 - w) / 2;

	SetRect(&rc, 0, 0, w, h);
	g_pGrp->BltN(&rc, ox, oy, m_pNameSurf);
}


// キャラクタＩＤを取得する //
CHARACTER_ID SPCharInfo::GetCharacterID(void)
{
	return m_CharacterID;
}


// キャラクタ選択 //
FVOID SPCharInfo::Select(CHARACTER_ID CharID)
{
	DWORD			TextureID;
	char			*pTextureName;
	char			*pName;
	char			*pFileName;
	CCharacterBase	*pInfo;


	m_CharacterID = CharID;

	switch(CharID){
	case CHARACTER_VIVIT:
		pTextureName = "VIVIT/WINU";
		pName        = "GRP/VIV_NAME";
		pInfo        = &g_VivitInfo;

		m_Params[SPP_SIZE] = 255-(60*255)/100;
		m_Params[SPP_STR]  = (75*255)/100;
		m_Params[SPP_DEF]  = (75*255)/100;
	break;

	case CHARACTER_STG1:
		pTextureName = "STG1/WINU";
		pName        = "GRP/STG1_NAME";
		pInfo        = &g_Stg1BossInfo;

		m_Params[SPP_SIZE] = 255-(65*255)/100;
		m_Params[SPP_STR]  = (80*255)/100;
		m_Params[SPP_DEF]  = (40*255)/100;
	break;

	case CHARACTER_STG2:
		pTextureName = "STG2/WINU";
		pName        = "GRP/STG2_NAME";
		pInfo        = &g_Stg2BossInfo;

		m_Params[SPP_SIZE] = 255-(25*255)/100;
		m_Params[SPP_STR]  = (40*255)/100;
		m_Params[SPP_DEF]  = (30*255)/100;
	break;

	case CHARACTER_STG3:
		pTextureName = "STG3/WINU";
		pName        = "GRP/STG3_NAME";
		pInfo        = &g_Stg3BossInfo;

		m_Params[SPP_SIZE] = 255-(70*255)/100;
		m_Params[SPP_STR]  = (100*255)/100;
		m_Params[SPP_DEF]  = (70*255)/100;
	break;

	case CHARACTER_STG4:
		pTextureName = "STG4/WINU";
		pName        = "GRP/STG4_NAME";
		pInfo        = &g_Stg4BossInfo;

		m_Params[SPP_SIZE] = 255-(90*255)/100;
		m_Params[SPP_STR]  = (80*255)/100;
		m_Params[SPP_DEF]  = (85*255)/100;
	break;

	case CHARACTER_STG5:
		pTextureName = "STG5/WINU";
		pName        = "GRP/STG5_NAME";
		pInfo        = &g_Stg5BossInfo;

		m_Params[SPP_SIZE] = 255-(60*255)/100;
		m_Params[SPP_STR]  = (85*255)/100;
		m_Params[SPP_DEF]  = (40*255)/100;
	break;

	case CHARACTER_MORGAN:
		pTextureName = "MORGAN/WINU";
		pName        = "GRP/MORGAN_NAME";
		pInfo        = &g_MorganInfo;

		m_Params[SPP_SIZE] = 255-(60*255)/100;
		m_Params[SPP_STR]  = (70*255)/100;
		m_Params[SPP_DEF]  = (50*255)/100;
	break;

	case CHARACTER_MUSE:
		pTextureName = "MUSE/WINU";
		pName        = "GRP/MUSE_NAME";
		pInfo        = &g_MuseInfo;

		m_Params[SPP_SIZE] = 255-(21*255)/100;
		m_Params[SPP_STR]  = (95*255)/100;
		m_Params[SPP_DEF]  = (92*255)/100;
	break;

	case CHARACTER_YUKA:
		pTextureName = "YUKA/WINU";
		pName        = "GRP/YUKA_NAME";
		pInfo        = &g_MuseInfo;

		m_Params[SPP_SIZE] = 255-(62*255)/100;
		m_Params[SPP_STR]  = (50*255)/100;
		m_Params[SPP_DEF]  = (85*255)/100;
	break;

	default:
		pTextureName = "";
		pName        = "";
		pInfo        = NULL;

		m_Params[SPP_SPD]  = 0;
		m_Params[SPP_SIZE] = 0;
		m_Params[SPP_STR]  = 0;
		m_Params[SPP_DEF]  = 0;
		m_Params[SPP_CHG]  = 0;
	break;
	}

	#pragma message("<<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
	#pragma message("キャラクタのサイズ、防御、攻撃が決定した場合は、以下に変更を加えよ")
	#pragma message("<<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
	if(pInfo){
		m_Params[SPP_SPD]  = 255 * pInfo->GetNormalSpeed() / m_MaxParam[SPP_SPD];
//		m_Params[SPP_SIZE] = 255 * 1 / m_MaxParam[SPP_SIZE];
//		m_Params[SPP_STR]  = 255 * 1 / m_MaxParam[SPP_STR];
//		m_Params[SPP_DEF]  = 255 * 1 / m_MaxParam[SPP_DEF];
		m_Params[SPP_CHG]  = 255 * pInfo->GetChargeSpeed() / m_MaxParam[SPP_CHG];
	}

	if(m_pNameSurf){
		g_pGrp->ReleaseOffsSurface(m_pNameSurf);
		m_pNameSurf = NULL;
	}

	if(g_pGrp->CreateSurface(&m_pNameSurf, 160, 25)){
		if('\0' != pName[0]){
			m_pNameSurf->LoadP("GRAPH.DAT", pName);
			m_pNameSurf->SetColorKey(0);
		}
	}

	switch(m_PlayerID){
		case 0:
			TextureID = TEXTURE_ID_P1_TAIL;
			pFileName  = "GRAPH2.DAT";
		break;

		case 1:
			TextureID = TEXTURE_ID_P2_TAIL;
			pFileName = "GRAPH3.DAT";
		break;

		default:	return;
	}
/*
	PbgError("キャラクタ選択時のテクスチャロードは、現在作成中...");
	return;
*/
	if('\0' != pTextureName[0]){
		g_pGrp->CreateTextureP(TextureID, pFileName, pTextureName, GRPTEX_TRANSBLACK);
	}
}


// パラメータの最大値をセット //
FVOID SPCharInfo::InitCharParams(void)
{
	#pragma message("<<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>>>>>>")
	#pragma message("きゃらくたを追加する場合は、以下に変更を加えよ")
	#pragma message("<<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>>>>>>")
	CCharacterBase *pChar[8] = {
		&g_VivitInfo, &g_Stg1BossInfo, &g_Stg2BossInfo,
		&g_Stg3BossInfo, &g_Stg4BossInfo, &g_Stg5BossInfo,
		&g_MorganInfo, &g_MuseInfo
	};
	int			i;

	// 初期化済みならば、戻る //
	if(m_bInitialized) return;

	// 最大値を求めるのです //
	for(i=0; i<8; i++){
		m_MaxParam[SPP_SPD]  = max(m_MaxParam[SPP_SPD],  pChar[i]->GetNormalSpeed());
		m_MaxParam[SPP_SIZE] = max(m_MaxParam[SPP_SIZE], 1);
		m_MaxParam[SPP_STR]  = max(m_MaxParam[SPP_STR],  1);
		m_MaxParam[SPP_DEF]  = max(m_MaxParam[SPP_DEF],  1);
		m_MaxParam[SPP_CHG]  = max(m_MaxParam[SPP_CHG],  pChar[i]->GetChargeSpeed());
	}

	m_bInitialized = TRUE;
}


// コンストラクタ //
CSelectProc::CSelectProc(CGameInfo *pSys) : CScene(pSys)
{
	m_pFrame = NULL;
	m_pSE    = NULL;
	m_pSys   = pSys;
}


// デストラクタ //
CSelectProc::~CSelectProc()
{
	if(m_pFrame){
		g_pGrp->ReleaseOffsSurface(m_pFrame);
		m_pFrame = NULL;
	}

	if(m_pSE){
		g_pGrp->ReleaseOffsSurface(m_pSE);
		m_pSE = NULL;
	}
}


// 初期化をする //
IBOOL CSelectProc::Initialize(void)
{
	if(NULL == m_pFrame){
		if(g_pGrp->CreateSurface(&m_pFrame, 640, 480)){
			Pbg::CGrpLoader::LoadP(m_pFrame->Lock(), "GRAPH.DAT", "GRP/至高の枠");
			m_pFrame->Unlock();
//			m_pFrame->LoadP("GRAPH.DAT", "GRP/至高の枠", 0, 0);
			m_pFrame->SetColorKey(0);
		}
		else{
			return FALSE;
		}
	}

	if(NULL == m_pSE){
		if(g_pGrp->CreateSurface(&m_pSE, 640, 480)){
			Pbg::CGrpLoader::LoadP(m_pSE->Lock(), "GRAPH.DAT", "GRP/ＳＥ");
			m_pSE->Unlock();
//			m_pSE->LoadP("GRAPH.DAT", "GRP/ＳＥ", 0, 0);
			m_pSE->SetColorKey(0);
		}
		else{
			return FALSE;
		}
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_BG01, "GRAPH.DAT", "GRP/萌Bin", 0)){
		PbgError("テクスチャ読み込めないよぉ");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_BG02, "GRAPH.DAT", "GRP/選択まる", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ");
		return FALSE;
	}

	if(FALSE == g_pGrp->CreateTextureP(TEXTURE_ID_BG03, "GRAPH.DAT", "GRP/選択完了", GRPTEX_TRANSBLACK)){
		PbgError("テクスチャ読み込めないよぉ");
		return FALSE;
	}

	CHARACTER_ID		p1, p2;

	m_pSys->GetCharacterID(&p1, &p2);

	switch(m_pSys->GetGameMode()){
	case GMODE_VS_CPU:
	case GMODE_VS_2P:
		m_CharInfo[0].Initialize(0, p1);
		m_CharInfo[1].Initialize(1, p2);
	break;

	default:
		m_CharInfo[0].Initialize(0, p1);
		m_CharInfo[1].Initialize(1, CHARACTER_UNKNOWN);
	break;
	}

	// マウスカーソルを無効化 //
	m_pSys->EnableMouseCursor(FALSE);

	// キーボードの押し続け対策 //
	m_KeyRep = TRUE;

	return TRUE;
}


// １フレーム分だけ動作させる //
PBG_PROCID CSelectProc::Move(void)
{
	WORD	KeyCode1, KeyCode2;

	switch(m_pSys->GetGameMode()){
		case GMODE_STORY:	// ストーリーモード
		case GMODE_BOSSATTACK:
//			KeyCode1 = m_pSys->GetSystemKeyCode();
			KeyCode1 = m_pSys->GetPlayer1_KeyCode();
			KeyCode2 = 0;
		break;

		case GMODE_VS_CPU:	// ＣＰＵと対戦
			if(255 == m_CharInfo[0].m_SelectedAlpha){
				KeyCode1 = 0;
				KeyCode2 = m_pSys->GetPlayer1_KeyCode();
				if(KEY_BOMB == KeyCode2){
					KeyCode1 = KEY_BOMB;
					KeyCode2 = KEY_BOMB;
				}
			}
			else{
				KeyCode1 = m_pSys->GetPlayer1_KeyCode();
				KeyCode2 = 0;
			}
		break;

		case GMODE_VS_2P:	// ２Ｐと対戦
			KeyCode1 = m_pSys->GetPlayer1_KeyCode();
			KeyCode2 = m_pSys->GetPlayer2_KeyCode();
		break;
	}

	if(m_KeyRep){
		// キーが押し続けられている場合は、強制的に無効化 //
		if(KeyCode1 | KeyCode2){
			KeyCode1 = 0;
			KeyCode2 = 0;
		}
		// そうでなければ、入力を有効化する //
		else{
			m_KeyRep = FALSE;
		}
	}

	int		n = m_pSys->GetNumCharacters();
	m_CharInfo[0].Proceed(KeyCode1, n);
	m_CharInfo[1].Proceed(KeyCode2, n);

	// 選択が完了した場合 //
	if((255 == m_CharInfo[0].m_SelectedAlpha)
	&& (255 == m_CharInfo[1].m_SelectedAlpha)){
		SetCharacterID();

		g_pGrp->ReleaseOffsSurface(m_pFrame);	m_pFrame = NULL;
		g_pGrp->ReleaseOffsSurface(m_pSE);		m_pSE    = NULL;

		return PBGPROC_MAIN;
	}

	switch(KeyCode1 | KeyCode2){
		case KEY_MENU:
			g_pGrp->ReleaseOffsSurface(m_pFrame);	m_pFrame = NULL;
			g_pGrp->ReleaseOffsSurface(m_pSE);		m_pSE    = NULL;
		return PBGPROC_TITLE;

		default:
		return PBGPROC_OK;
	}
}


// 描画する //
IVOID CSelectProc::Draw(void)
{
	RECT			src;
	int				x;

//	g_pGrp->Cls();

	DrawBinTexture();
	DrawCharacter();
	DrawBlackWindow();
	DrawParams();

	m_CharInfo[0].DrawCharName(  0, 22);
	if(m_CharInfo[1].GetCharacterID() != CHARACTER_UNKNOWN){
		m_CharInfo[1].DrawCharName(320, 22);
	}

	SetRect(&src, 0, 0, 640, 480);
	g_pGrp->BltC(&src, 0, 0, m_pFrame);

	DrawWeapons(26, m_CharInfo[0].GetCharacterID());

	if((GMODE_VS_CPU == m_pSys->GetGameMode()) && m_CharInfo[0].m_SelectedAlpha){
		DrawWeapons(640-(26+96), m_CharInfo[1].GetCharacterID());
	}
/*
	switch(m_CharInfo[0].GetCharacterID()){
//		default:					x = 130;	break;
		case CHARACTER_VIVIT:		x = 0;		break;
		case CHARACTER_STG2:		x = 260;	break;
		case CHARACTER_STG3:		x = 390;	break;
	}
	SetRect2(&src, x, 0, 130, 480);
	g_pGrp->BltC(&src, 0, 0, m_pSE);


	switch(m_CharInfo[1].GetCharacterID()){
//		default:	return;
		default:					x = 130;	break;

		case CHARACTER_VIVIT:		x = 0;		break;
		case CHARACTER_STG2:		x = 260;	break;
		case CHARACTER_STG3:		x = 390;	break;
	}
	SetRect2(&src, x, 0, 130, 480);
	g_pGrp->BltC(&src, 640-142, 0, m_pSE);
*/
}


FVOID CSelectProc::DrawWeapons(int sx, CHARACTER_ID CharID)
{
	RECT		rc;

	switch(CharID){
		case CHARACTER_VIVIT:	SetRect2(&rc, 96*0, 56*0, 96, 56);		break;
		case CHARACTER_STG1:	SetRect2(&rc, 96*1, 56*0, 96, 56);		break;
		case CHARACTER_STG2:	SetRect2(&rc, 96*2, 56*0, 96, 56);		break;
		case CHARACTER_STG3:	SetRect2(&rc, 96*3, 56*0, 96, 56);		break;
		case CHARACTER_STG4:	SetRect2(&rc, 96*4, 56*0, 96, 56);		break;
		case CHARACTER_STG5:	SetRect2(&rc, 96*5, 56*0, 96, 56);		break;
		case CHARACTER_MORGAN:	SetRect2(&rc, 96*0, 168,  96, 56);		break;
		case CHARACTER_MUSE:	SetRect2(&rc, 96*1, 168,  96, 56);		break;
		case CHARACTER_YUKA:	SetRect2(&rc, 96*2, 168,  96, 56);		break;

		default:
		return;
	}

	if(sx < 320){
		for(int i=2; i>=0; i--){
			g_pGrp->BltC(&rc, sx-7, 97+76*i, m_pSE);

			rc.top    += 56;
			rc.bottom += 56;
		}
	}
	else{
		for(int i=2; i>=0; i--){
			g_pGrp->BltC(&rc, sx+5, 97+76*i, m_pSE);

			rc.top    += 56;
			rc.bottom += 56;
		}
	}

	switch(CharID){
		case CHARACTER_VIVIT:	SetRect2(&rc, 288,   168, 104, 120);		break;
		case CHARACTER_STG1:	SetRect2(&rc, 392,   168, 104, 120);		break;
		case CHARACTER_STG2:	SetRect2(&rc, 496,   168, 104, 120);		break;
		case CHARACTER_STG3:	SetRect2(&rc, 104*0, 336, 104, 120);		break;
		case CHARACTER_STG4:	SetRect2(&rc, 104*1, 336, 104, 120);		break;
		case CHARACTER_STG5:	SetRect2(&rc, 104*2, 336, 104, 120);		break;
		case CHARACTER_MORGAN:	SetRect2(&rc, 104*3, 336, 104, 120);		break;
		case CHARACTER_MUSE:	SetRect2(&rc, 104*4, 336, 104, 120);		break;
		case CHARACTER_YUKA:	SetRect2(&rc, 104*5, 336, 104, 120);		break;

		default:
		return;
	}

	if(sx < 320) g_pGrp->BltC(&rc, sx-11, 338, m_pSE);
	else         g_pGrp->BltC(&rc, sx, 338, m_pSE);

	SetRect2(&rc, 96*6, 0, 40, 168);
	g_pGrp->BltC(&rc, sx+3, 80, m_pSE);
}


// キャラクタ選択を反映する //
FVOID CSelectProc::SetCharacterID(void)
{
	CHARACTER_ID	Player1, Player2;

	Player1 = m_CharInfo[0].GetCharacterID();
	Player2 = m_CharInfo[1].GetCharacterID();

	switch(m_pSys->GetGameMode()){
		case GMODE_STORY:	// ストーリーモード
		case GMODE_BOSSATTACK:
			m_pSys->SetCharacterID(Player1, CHARACTER_VIVIT);
		break;

		case GMODE_VS_CPU:	// ＣＰＵと対戦
		case GMODE_VS_2P:	// ２Ｐと対戦
			m_pSys->SetCharacterID(Player1, Player2);
		break;
	}
}


// バイナリ文字列の描画を行う //
FVOID CSelectProc::DrawBinTexture(void)
{
	RECT	rc = {0, 0, 640, 480};

	D3DTLVERTEX		tlv[20];
	int				ox, oy, dx, dy;
	int				i, j;
	DWORD			c;
	BYTE			d;

	if(FALSE == g_pGrp->Begin3DScene()) return;

	c  = RGBA_MAKE(255, 255, 255, 255);

	g_pGrp->SetRenderStateEx(GRPST_NORMAL);
	g_pGrp->SetViewport(&rc);

	// テクスチャ１を描画する //
	g_pGrp->SetTexture(TEXTURE_ID_BG01);
	for(i=0; i<2; i++){
		ox = i * 320;

		for(j=m_CharInfo[i].m_BinTexY/256; j<480; j+=256){
			Set2DPointC(tlv+0, ox+  0, j+  0, 0.0, 0.0, c);
			Set2DPointC(tlv+1, ox+320, j+  0, 1.0, 0.0, c);
			Set2DPointC(tlv+2, ox+320, j+256, 1.0, 1.0, c);
			Set2DPointC(tlv+3, ox+  0, j+256, 0.0, 1.0, c);
			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}
	}
/*
	for(i=0; i<2; i++){
		ox = i * 320 + 16 - 3;

		for(j=m_CharInfo[i].m_BinTexY/256; j<480; j+=256){
			Set2DPointC(tlv+0, ox+  0, j+  0, 0.0, 0.0, c);
			Set2DPointC(tlv+1, ox+256+32+8, j+  0, 1.0, 0.0, c);
			Set2DPointC(tlv+2, ox+256+32+8, j+256, 1.0, 1.0, c);
			Set2DPointC(tlv+3, ox+  0, j+256, 0.0, 1.0, c);
			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}
	}
*/
	// テクスチャ２を描画する //
	g_pGrp->SetTexture(TEXTURE_ID_BG02);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	for(i=0; i<2; i++){
		ox = i * 320 + 160;
		oy = 200 + 200;

		d  = m_CharInfo[i].m_Angle;
		dx = CosL(d, 190+50);
		dy = SinL(d, 190+50);

		Set2DPointC(tlv+0, ox+dx, oy+dy, 0.0, 0.0, c);
		Set2DPointC(tlv+1, ox-dy, oy+dx, 1.0, 0.0, c);
		Set2DPointC(tlv+2, ox-dx, oy-dy, 1.0, 1.0, c);
		Set2DPointC(tlv+3, ox+dy, oy-dx, 0.0, 1.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


		d  = 256 - m_CharInfo[i].m_Angle;
		dx = CosL(d, 100 + 50 + SinL(d, 20));
		dy = SinL(d, 100 + 50 + SinL(d, 20));

		Set2DPointC(tlv+0, ox+dx, oy+dy, 0.0, 0.0, c);
		Set2DPointC(tlv+1, ox-dy, oy+dx, 1.0, 0.0, c);
		Set2DPointC(tlv+2, ox-dx, oy-dy, 1.0, 1.0, c);
		Set2DPointC(tlv+3, ox+dy, oy-dx, 0.0, 1.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->End3DScene();
}


// 黒の半透明窓を描画する //
FVOID CSelectProc::DrawBlackWindow(void)
{
	RECT			rc = {0, 0, 640, 480};

	D3DTLVERTEX		tlv[20];
	DWORD			c1, c2;

//	c  = RGBA_MAKE(0, 0, 0, 60);
	c1 = RGBA_MAKE(0, 0, 50, 40);
	c2 = RGBA_MAKE(0, 0, 50, 150);

	if(FALSE == g_pGrp->Begin3DScene()) return;

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetViewport(&rc);

	// １Ｐ：溜め撃ち部 //
	Set2DPointC(tlv+0, 0+  6,  66, 0, 0, c1);
	Set2DPointC(tlv+1, 0+129,  66, 0, 0, c1);
	Set2DPointC(tlv+2, 0+129, 317+4, 0, 0, c2);
	Set2DPointC(tlv+3, 0+  6, 317+4, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// ２Ｐ：溜め撃ち部 //
	Set2DPointC(tlv+0, 640-129,  66, 0, 0, c1);
	Set2DPointC(tlv+1, 640-  6,  66, 0, 0, c1);
	Set2DPointC(tlv+2, 640-  6, 317+4, 0, 0, c2);
	Set2DPointC(tlv+3, 640-129, 317+4, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// １Ｐ：下の奴２つ //
	Set2DPointC(tlv+0, 0+  7, 330, 0, 0, c1);
	Set2DPointC(tlv+1, 0+303, 330, 0, 0, c1);
	Set2DPointC(tlv+2, 0+303, 470, 0, 0, c2);
	Set2DPointC(tlv+3, 0+  7, 470, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// ２Ｐ：下の奴２つ //
	Set2DPointC(tlv+0, 640-303, 330, 0, 0, c1);
	Set2DPointC(tlv+1, 640-  7, 330, 0, 0, c1);
	Set2DPointC(tlv+2, 640-  7, 470, 0, 0, c2);
	Set2DPointC(tlv+3, 640-303, 470, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	g_pGrp->End3DScene();
}


// キャラクタの顔を描画する //
FVOID CSelectProc::DrawCharacter(void)
{
	RECT			rc;
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				ox, oy;
	DWORD			c, c1, c2;

	const POINT texDir[4] = {
		{-1, -1}, {1, -1}, {1, 1}, {-1, 1}
	};

	if(FALSE == g_pGrp->Begin3DScene()) return;

	// プレイヤー１の描画準備 //
	SetRect(&rc, 0, 0, 320, 328);
	g_pGrp->SetViewport(&rc);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);
//	c  = RGBA_MAKE(0, 0, 0, 60);
	c1 = RGBA_MAKE(0, 0, 50, 40);
	c2 = RGBA_MAKE(0, 0, 50, 150);

	Set2DPointC(tlv+0, 141, 64, 0.0, 0.0, c1);
	Set2DPointC(tlv+1, 316, 64, 1.0, 0.0, c1);
	Set2DPointC(tlv+2, 316, 318+4, 1.0, 1.0, c2);
	Set2DPointC(tlv+3, 141, 318+4, 0.0, 1.0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// キャラクタ描画の準備 //
	g_pGrp->SetRenderStateEx(GRPST_COLORKEY);
	c1 = RGBA_MAKE(255, 255, 255, 255);
	c2 = RGBA_MAKE(255, 255, 255, 255);
//	c2 = RGBA_MAKE(180, 180, 180, 255);

	ox = 100;
	oy = 64;
	Set2DPointC(tlv+0, ox+  0, oy+  0, 0.0, 0.0, c1);
	Set2DPointC(tlv+1, ox+256, oy+  0, 1.0, 0.0, c1);
	Set2DPointC(tlv+2, ox+256, oy+256, 1.0, 1.0, c2);
	Set2DPointC(tlv+3, ox+  0, oy+256, 0.0, 1.0, c2);
	g_pGrp->SetTexture(TEXTURE_ID_P1_TAIL);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	if(m_CharInfo[0].m_SelectedAlpha && (FALSE == m_CharInfo[0].m_bRandomSelect)){
		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
		c  = m_CharInfo[0].m_SelectedAlpha - 1;
		c1 = RGBA_MAKE(255-c, 255-c, 255-c, 255);
		c2 = RGBA_MAKE(255, 255, 255, c);
		for(int i=0; i<4; i++){
			tlv[i].color = c1;
			tlv[i].sx += (texDir[i].x * (int)(c)) / 16;
			tlv[i].sy += (texDir[i].y * (int)(c)) / 16;
		}
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

		u1 = D3DVAL(0)   / D3DVAL(256);
		v1 = D3DVAL(110) / D3DVAL(256);
		u2 = D3DVAL(130) / D3DVAL(256);
		v2 = D3DVAL(135) / D3DVAL(256);

		ox = 96  + 63;
		oy = 156 + 128;

		Set2DPointC(tlv+0, ox+  0, oy+ 0, u1, v1, c2);
		Set2DPointC(tlv+1, ox+130, oy+ 0, u2, v1, c2);
		Set2DPointC(tlv+2, ox+130, oy+25, u2, v2, c2);
		Set2DPointC(tlv+3, ox+  0, oy+25, u1, v2, c2);

		g_pGrp->SetTexture(TEXTURE_ID_BG03);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// プレイヤー２の描画準備 //
	SetRect(&rc, 320, 0, 640, 328);
	g_pGrp->SetViewport(&rc);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);
//	c  = RGBA_MAKE(0, 0, 0, 60);
	c1 = RGBA_MAKE(0, 0, 50, 40);
	c2 = RGBA_MAKE(0, 0, 50, 150);

	Set2DPointC(tlv+0, 640-141, 64, 0.0, 0.0, c1);
	Set2DPointC(tlv+1, 640-316, 64, 1.0, 0.0, c1);
	Set2DPointC(tlv+2, 640-316, 318+4, 1.0, 1.0, c2);
	Set2DPointC(tlv+3, 640-141, 318+4, 0.0, 1.0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// 描画しない場合 //
	if(CHARACTER_UNKNOWN == m_CharInfo[1].GetCharacterID()){
		g_pGrp->End3DScene();
		return;
	}

	// キャラクタ描画の準備 //
//	g_pGrp->SetRenderStateEx(GRPST_COLORKEY);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	if(GMODE_VS_CPU == m_pSys->GetGameMode()){
		c  = m_CharInfo[0].m_SelectedAlpha/2 + 128;
		c1 = RGBA_MAKE(255, 255, 255, c);
		c2 = RGBA_MAKE(255, 255, 255, c);
	}
	else{
		c1 = RGBA_MAKE(255, 255, 255, 255);
		c2 = RGBA_MAKE(255, 255, 255, 255);
	}

	ox = 640-100-256;
	oy = 64;
	Set2DPointC(tlv+0, ox+  0, oy+  0, 0.0, 0.0, c1);
	Set2DPointC(tlv+1, ox+256, oy+  0, 1.0, 0.0, c1);
	Set2DPointC(tlv+2, ox+256, oy+256, 1.0, 1.0, c2);
	Set2DPointC(tlv+3, ox+  0, oy+256, 0.0, 1.0, c2);
	g_pGrp->SetTexture(TEXTURE_ID_P2_TAIL);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


	if(m_CharInfo[1].m_SelectedAlpha
	&& (CHARACTER_UNKNOWN != m_CharInfo[1].GetCharacterID())
	&& (FALSE == m_CharInfo[1].m_bRandomSelect)){
		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
		c  = m_CharInfo[1].m_SelectedAlpha - 1;
		c1 = RGBA_MAKE(255-c, 255-c, 255-c, 255);
		c2 = RGBA_MAKE(255, 255, 255, c);

		for(int i=0; i<4; i++){
			tlv[i].color = c1;
			tlv[i].sx += (texDir[i].x * (int)(c)) / 16;
			tlv[i].sy += (texDir[i].y * (int)(c)) / 16;
		}
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

		u1 = D3DVAL(0)   / D3DVAL(256);
		v1 = D3DVAL(110) / D3DVAL(256);
		u2 = D3DVAL(130) / D3DVAL(256);
		v2 = D3DVAL(135) / D3DVAL(256);

		ox = (640-96-256) + 63;
		oy = 156      + 128;

		Set2DPointC(tlv+0, ox+  0, oy+ 0, u1, v1, c2);
		Set2DPointC(tlv+1, ox+130, oy+ 0, u2, v1, c2);
		Set2DPointC(tlv+2, ox+130, oy+25, u2, v2, c2);
		Set2DPointC(tlv+3, ox+  0, oy+25, u1, v2, c2);

		g_pGrp->SetTexture(TEXTURE_ID_BG03);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->End3DScene();
}


// パラメータを描画する //
FVOID CSelectProc::DrawParams(void)
{
	RECT			rc = {0, 0, 640, 480};

	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	DWORD			c1, c2;
	int				i, ox, oy, dx;

	c1 = RGBA_MAKE(10, 255, 10, 255);
	c2 = RGBA_MAKE(0, 32, 0, 255);

	if(FALSE == g_pGrp->Begin3DScene()) return;

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetViewport(&rc);

	// プレイヤー１の表示 //
	for(i=0; i<SPP_NUMITEMS; i++){
		ox = 212;
		oy = 348 + i * 24;
		dx = (m_CharInfo[0].m_DispParams[i] * 80) / 255;

		Set2DPointC(tlv+0, ox,    oy,   0, 0, c1);
		Set2DPointC(tlv+1, ox+dx, oy,   0, 0, c1);
		Set2DPointC(tlv+2, ox+dx, oy+4, 0, 0, c2);
		Set2DPointC(tlv+3, ox,    oy+4, 0, 0, c2);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// プレイヤー２の表示 //
	for(i=0; i<SPP_NUMITEMS; i++){
		ox = 640 - 212;
		oy = 348 + i * 24;
		dx = (m_CharInfo[1].m_DispParams[i] * 80) / 255;

		Set2DPointC(tlv+0, ox-dx, oy,   0, 0, c1);
		Set2DPointC(tlv+1, ox,    oy,   0, 0, c1);
		Set2DPointC(tlv+2, ox,    oy+4, 0, 0, c2);
		Set2DPointC(tlv+3, ox-dx, oy+4, 0, 0, c2);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->SetTexture(TEXTURE_ID_BG03);

	u1 = 0;
	v1 = 0;
	u2 = D3DVAL( 64) / D3DVAL(256);
	v2 = D3DVAL(108) / D3DVAL(256);
	c1 = RGBA_MAKE(255, 255, 255, 255);

	ox = 148;
	oy = 346;

	Set2DPointC(tlv+0, ox,    oy,     u1, v1, c1);
	Set2DPointC(tlv+1, ox+64, oy,     u2, v1, c1);
	Set2DPointC(tlv+2, ox+64, oy+108, u2, v2, c1);
	Set2DPointC(tlv+3, ox,    oy+108, u1, v2, c1);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	if(CHARACTER_UNKNOWN != m_CharInfo[1].GetCharacterID()){
		ox = 430;
		oy = 346;

		Set2DPointC(tlv+0, ox,    oy,     u1, v1, c1);
		Set2DPointC(tlv+1, ox+64, oy,     u2, v1, c1);
		Set2DPointC(tlv+2, ox+64, oy+108, u2, v2, c1);
		Set2DPointC(tlv+3, ox,    oy+108, u1, v2, c1);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->End3DScene();
}
