/*
 *   CEndingProc.cpp   : エンディング処理
 *
 */

#include "CEndingProc.h"
#include "CGameInfo.h"



#define EDPST_OPEN		0x00	// 初期状態
#define EDPST_WAIT		0x01	// 待ち状態
#define EDPST_CLOSE		0x02	// 閉じ状態



// コンストラクタ //
CEndingProc::CEndingProc(CGameInfo *pSys) : CScene(pSys)
{
}


// デストラクタ //
CEndingProc::~CEndingProc()
{
}


// 初期化をする //
IBOOL CEndingProc::Initialize(void)
{
	m_State = EDPST_OPEN;
	m_Count = 0;

	m_pSys->OnClearGame();

	if(FALSE == m_LogoDIB.Create(640, 480)) return FALSE;

	return Pbg::CGrpLoader::LoadP(&m_LogoDIB, "GRAPH.DAT", "GRP/エンディング");
//	return m_LogoDIB.LoadP("GRAPH.DAT", "GRP/エンディング");
}


// １フレーム分だけ動作させる //
PBG_PROCID CEndingProc::Move(void)
{
	DWORD		t;

	switch(m_State){
	case EDPST_OPEN:	// 初期状態
		if(m_Count > 50){
			m_Count = 0;
			m_State = EDPST_WAIT;
		}
	break;

	case EDPST_WAIT:	// 待ち状態
		if(KEY_SHOT == m_pSys->GetSystemKeyCode()){
			m_Count = 0;
			m_State = EDPST_CLOSE;
		}
	break;

	case EDPST_CLOSE:	// 閉じ状態
		t = m_Count;
		m_LogoDIB.EfcMosaic(NULL, t, -2*t);
//		m_LogoDIB.EfcMotionBlur(NULL, 320/*-250*/, 240/*+110*/, t);
		m_LogoDIB.Update();

		if(t > 35){
			return PBGPROC_TITLE;
		}
	break;
	}

	m_Count++;

	return PBGPROC_OK;
}


// 描画する //
IVOID CEndingProc::Draw(void)
{
	RECT			src = {0, 0, 640, 480};
	D3DTLVERTEX		tlv[10];
	DWORD			c;

	g_pGrp->SetViewport(&src);
	g_pGrp->BitBltEx(&m_LogoDIB, 0, 0);

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

	if(EDPST_OPEN != m_State) return;

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
