/*
 *   CLogoProc.cpp   : 西方ロゴの表示
 *
 */

#include "CLogoProc.h"
#include "PbgProcID.h"
#include "CGameInfo.h"



// コンストラクタ //
CLogoProc::CLogoProc(CGameInfo *pSys) : CScene(pSys)
{
	// 特に何もしません //
}


// デストラクタ //
CLogoProc::~CLogoProc()
{
	// 上に同じ //
}


// 初期化をする //
IBOOL CLogoProc::Initialize(void)
{
	BOOL	bRet;
	m_Count = 0;

	g_pMusic->Load(-1);

	if(MDEV_MIDI == g_pMusic->GetMusicDev()){
		g_pMusic->Play();
	}

	if(FALSE == m_LogoDIB.Create(640, 480)) return FALSE;
	m_LogoDIB.Clear(0);

#pragma message("だめっす")
	bRet = Pbg::CGrpLoader::LoadP(&m_LogoDIB, "GRAPH.DAT", "GRP/西方");
//	bRet = m_LogoDIB.LoadP("GRAPH.DAT", "GRP/西方");

	if(MDEV_WAVE == g_pMusic->GetMusicDev()){
		g_pMusic->Play();
	}

	m_FinishFlag = FALSE;

	return bRet;
}


// １フレーム分だけ動作させる //
PBG_PROCID CLogoProc::Move(void)
{
	DWORD		t;

	if(m_pSys->GetSystemKeyCode() || (m_Count >= 120)){
		if(!m_FinishFlag){
			m_Count      = 256/6;
			m_FinishFlag = TRUE;
		}
	}

	if(FALSE == m_FinishFlag) m_Count++;
	else                      m_Count = max(0, m_Count-2);

	if(m_FinishFlag && (0 == m_Count)){
		m_LogoDIB.Cleanup();
		return PBGPROC_TITLE;
	}


	if(m_Count > 90){
		t = m_Count - 90;
		m_LogoDIB.EfcMotionBlur(NULL, 320/*-250*/, 240/*+110*/, t);
		m_LogoDIB.Update();
	}

	return PBGPROC_OK;
}


// 描画する //
IVOID CLogoProc::Draw(void)
{
	RECT			src = {0, 0, 640, 480};
	D3DTLVERTEX		tlv[10];
	DWORD			c;

	g_pGrp->SetViewport(&src);
	g_pGrp->BitBltEx(&m_LogoDIB, 0, 0);

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

	if(g_pGrp->Begin3DScene()){
		c = RGBA_MAKE(0, 0, 0, 255 - min(255, m_Count * 6));

		Set2DPointC(tlv+0,   0,   0, 0, 0, c);
		Set2DPointC(tlv+1, 640,   0, 0, 0, c);
		Set2DPointC(tlv+2, 640, 480, 0, 0, c);
		Set2DPointC(tlv+3,   0, 480, 0, 0, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->End3DScene();
	}
}
