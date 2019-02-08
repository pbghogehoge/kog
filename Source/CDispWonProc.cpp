/*
 *   CDispWonProc.cpp   : 勝ちキャラ表示
 *
 */

#include "CDispWonProc.h"
#include "CGameInfo.h"

#include "RndCtrl.h"



// コンストラクタ //
CDispWonProc::CDispWonProc(CGameInfo *pSys)
{
	m_pSys         = pSys;

	m_RetCode      = PBGPROC_OK;
	m_Count        = 0;
	m_WindowHeight = 0;		// ウィンドウの高さ

	m_pBG = NULL;
}


// デストラクタ //
CDispWonProc::~CDispWonProc()
{
}


// 初期化をする //
FBOOL CDispWonProc
		::Initialize(	PBG_PROCID		RetCode,
						CHARACTER_ID	IdWon,
						CHARACTER_ID	IdLost,
						BYTE			PlayerNo)
{
	m_RetCode      = RetCode;
	m_Count        = 0;
	m_WindowHeight = 0;		// ウィンドウの高さ

	if(NULL == m_pBG){
		if(g_pGrp->CreateSurface(&m_pBG, 256, 256)){
			Pbg::CGrpLoader::LoadP(m_pBG->Lock(), "GRAPH.DAT", "GRP/WONBG");
			m_pBG->Unlock();
		}
		else return FALSE;
	}
//PbgError("ooo");
	return SetCharacterID(IdWon, IdLost, PlayerNo);
}


// １フレーム分だけ動作させる //
FBOOL CDispWonProc::Move(PBG_PROCID *pRet)
{
	WORD key1 = m_pSys->GetPlayer1_KeyCode();
	WORD key2 = m_pSys->GetPlayer2_KeyCode();

	*pRet = PBGPROC_OK;

	if(m_Count >= 20){
		switch(m_pSys->GetGameMode()){
		case GMODE_VS_2P:
			if( ((1 == m_PlayerNo) && key1)
			||  ((2 == m_PlayerNo) && key2) ){
				break;
			}
		return TRUE;

		default:
			if(key1) break;
		return TRUE;
		}

		Cleanup();
		*pRet = m_RetCode;
		return FALSE;
	}
	else{
		m_Count++;
	}

	return TRUE;
}


// 描画する //
FVOID CDispWonProc::Draw(void)
{
	RECT			rc;
	D3DTLVERTEX		tlv[20];
	DWORD			c, c2;
	int				ox, oy, i, j;

	// ３Ｄシーンの開始 //
	if(FALSE == g_pGrp->Begin3DScene()) return;

	SetRect(&rc, 0, 0, 640, 480);
	g_pGrp->SetViewport(&rc);

	g_pGrp->Cls(0);
	SetRect(&rc, 0, 0, 256, 256);

	for(j=0; j<480; j+=256){
		for(i=-128; i<640; i+=256){
			g_pGrp->BltN(&rc, i, j, m_pBG);
		}
	}

	ox = 640 / 2;
	oy = 256 + 32 + (20 - m_Count) * 12;


	c = RGBA_MAKE(255, 255, 255, 255);
	c2 = RGBA_MAKE(0, 0, 0, 0);

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);

	Set2DPointC(tlv+0, ox-128, oy-256, 0.0, 0.0, c);
	Set2DPointC(tlv+1, ox+128, oy-256, 1.0, 0.0, c);
	Set2DPointC(tlv+2, ox+128, oy,     1.0, 1.0, c);
	Set2DPointC(tlv+3, ox-128, oy,     0.0, 1.0, c);
	g_pGrp->SetTexture(TEXTURE_ID_P1_00);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	Set2DPointC(tlv+0, ox-128, oy,     0.0, 0.0, c);
	Set2DPointC(tlv+1, ox+128, oy,     1.0, 0.0, c);
	Set2DPointC(tlv+2, ox+128, oy+256, 1.0, 1.0, c2);
	Set2DPointC(tlv+3, ox-128, oy+256, 0.0, 1.0, c2);
	g_pGrp->SetTexture(TEXTURE_ID_P1_01);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	g_pGrp->End3DScene();

	// テキスト窓を表示する //
	g_pTextWindow->Draw();
}


// さようなら //
FVOID CDispWonProc::Cleanup(void)
{
	if(m_pBG){
		g_pGrp->ReleaseOffsSurface(m_pBG);
		m_pBG = NULL;
	}

	const int FontSize = 24;
	g_pTextWindow->Resize(FontSize*23, FontSize*3);
	g_pTextWindow->SetFontSize(FontSize);
	g_pTextWindow->SetWindowPosA((640-FontSize*23)/2, 480-FontSize*3-10);
}


// 勝ったほうのプレイヤーをセット //
FBOOL CDispWonProc::SetCharacterID(CHARACTER_ID IdWon, CHARACTER_ID IdLost, BYTE PlayerNo)
{
	static char TextData[320];

	char		*pFileName;
	char		*pTextureName1;
	char		*pTextureName2;
	char		fileid[256];
	char		*dat;
	DWORD		n;

	Pbg::LzDecode		dec;

	m_Count    = 0;
	m_PlayerNo = PlayerNo;

	switch(IdWon){
		case CHARACTER_VIVIT:
			pTextureName1 = "VIVIT/WINU";
			pTextureName2 = "VIVIT/WIND";
		break;

		case CHARACTER_STG1:
			pTextureName1 = "STG1/WINU";
			pTextureName2 = "STG1/WIND";
		break;

		case CHARACTER_STG2:
			pTextureName1 = "STG2/WINU";
			pTextureName2 = "STG2/WIND";
		break;

		case CHARACTER_STG3:
			pTextureName1 = "STG3/WINU";
			pTextureName2 = "STG3/WIND";
		break;

		case CHARACTER_STG4:
			pTextureName1 = "STG4/WINU";
			pTextureName2 = "STG4/WIND";
		break;

		case CHARACTER_STG5:
			pTextureName1 = "STG5/WINU";
			pTextureName2 = "STG5/WIND";
		break;

		case CHARACTER_MORGAN:
			pTextureName1 = "MORGAN/WINU";
			pTextureName2 = "MORGAN/WIND";
		break;

		case CHARACTER_MUSE:
			pTextureName1 = "MUSE/WINU";
			pTextureName2 = "MUSE/WIND";
		break;

		case CHARACTER_YUKA:
			pTextureName1 = "YUKA/WINU";
			pTextureName2 = "YUKA/WIND";
		break;

		default:
		return FALSE;
	}

	if(1 == PlayerNo) pFileName = "GRAPH2.DAT";
	else              pFileName = "GRAPH3.DAT";

	g_pGrp->CreateTextureP(TEXTURE_ID_P1_00, pFileName, pTextureName1, GRPTEX_TRANSBLACK);
	g_pGrp->CreateTextureP(TEXTURE_ID_P1_01, pFileName, pTextureName2, GRPTEX_TRANSBLACK);

	const int FontSize = 24;
	g_pTextWindow->Resize(FontSize*23, FontSize*4);
	g_pTextWindow->SetFontSize(FontSize);
	g_pTextWindow->SetWindowPosA((640-FontSize*23)/2, 480-FontSize*4-10);

	while(1){
		wsprintf(fileid, "T_%s_%s", CharID2Str(IdWon), CharID2Str(IdLost));
		if(FALSE == dec.Open("ENEMY.DAT")) break;

		dat = (char *)dec.Decode(fileid);
		if(NULL == dat) break;

		n = dec.GetFileSize(fileid) / 320;
		if(0 == n){
			MemFree(dat);
			break;
		}

		Pbg::CRnd		MakeRnd;
		n = (MakeRnd.Get()>>1) % n;

		strcpy(TextData, &dat[n*320]);
		g_pTextWindow->SetText(TextData, FALSE);

//		PbgError(fileid);
//		wsprintf(fileid, "TextData(%s)  n = %d", TextData, n);
//		PbgError(fileid);

		MemFree(dat);
		return TRUE;
	}

	g_pTextWindow->SetText("開発蟲ですな", FALSE);
	return FALSE;
}


// キャラクタＩＤから文字列取得 //
const char *CDispWonProc::CharID2Str(CHARACTER_ID id)
{
	switch(id){
		case CHARACTER_VIVIT:	return "VIV";
		case CHARACTER_STG1:	return "STG1";
		case CHARACTER_STG2:	return "STG2";
		case CHARACTER_STG3:	return "STG3";
		case CHARACTER_STG4:	return "STG4";
		case CHARACTER_STG5:	return "STG5";
		case CHARACTER_MORGAN:	return "MORGAN";
		case CHARACTER_MUSE:	return "MUSE";
		case CHARACTER_YUKA:	return "YUKA";
		default:				return "";
	}
}
