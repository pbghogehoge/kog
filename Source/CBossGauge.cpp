/*
 *   CBossGauge.cpp   : ボス用各種ゲージ
 *
 */

#include "CBossGauge.h"
#include "Gian2001.h"

#define BGST_DISABLE	0x00	// 無効状態
#define BGST_MOVE		0x01	// オープンorクローズ中
#define BGST_NORMAL		0x02	// 通常状態



// コンストラクタ //
CBossGauge::CBossGauge(int MaxHP, RECT *pTarget)
	: m_MaxHP(MaxHP)
	, m_DispOx(pTarget->right - 198*256)
	, m_DispOy(pTarget->top + 24 * 256)
{
	Initialize();
}


// デストラクタ //
CBossGauge::~CBossGauge()
{
}


// 初期化時に呼び出す //
FVOID CBossGauge::Initialize(void)
{
	m_CurrentHP = 0;		// 現在のＨＰ
	m_DisplayHP = 0;		// 表示用ＨＰ

	m_Alpha = 0;			// 表示用α値
	m_State = BGST_DISABLE;	// 現在の状態
	m_Count = 0;			// アニメーション動作用カウンタ
}


// 動作時に呼び出す //
FVOID CBossGauge::Move(int CurrentHP)
{
	int			temp;
	int			delta;

	m_CurrentHP = CurrentHP;	// 現在のＨＰを更新すべし

	// >>>>>   状態推移   <<<<< //
	// ここでは State と Count の推移を行う //
	// DisplayHP の値にかかわらず、推移する //
	switch(m_State){
	case BGST_DISABLE:
		if(CurrentHP){
			m_Alpha     = 0;			// すなわち、初期状態
			m_Count     = 0;			// カウンタ初期化
			m_DisplayHP = -m_MaxHP;		// 表示用ＨＰ
			m_State     = BGST_MOVE;	// 動作せよ
		}
		else{
			m_DisplayHP = -m_MaxHP;
		}
	break;

	case BGST_MOVE:
		if(CurrentHP){		// 再生せよ
			m_Alpha = min(255, m_Alpha+4);
			if(255 == m_Alpha) m_State = BGST_NORMAL;

		}
		else{				// 死に逝く時
			m_Alpha = max(0, m_Alpha-6);
			if(0 == m_Alpha) m_State = BGST_DISABLE;
		}
	break;

	case BGST_NORMAL:
		if(0 == CurrentHP) m_State = BGST_MOVE;
		else               m_Alpha = 255;		// まだ生存中

		m_Count++;		// カウンタ更新
	break;
	}


	// >>>>>   ゲージの表示値を推移させる   <<<<< //
	// ここでは、CurrentHP の値にしたがって DisplayHP の //
	// 値を推移させる                                    //
	temp  = (CurrentHP + m_DisplayHP*14) / 15;
	delta = abs(temp - m_DisplayHP);
	if(delta < (m_MaxHP/196)){
//		m_DisplayHP = CurrentHP;
		if(CurrentHP > m_DisplayHP) m_DisplayHP = min(CurrentHP, m_DisplayHP + m_MaxHP/196);
		else                        m_DisplayHP = max(CurrentHP, m_DisplayHP - m_MaxHP/196);
	}
	else{
		m_DisplayHP = temp;
	}
}


// 描画時に呼び出す //
FVOID CBossGauge::Draw(BOOL IsMadnessMode)
{
	D3DTLVERTEX			tlv[20];
	D3DVALUE			u0, v0, u1, v1;
	DWORD				c1, c2;
	int					ox, oy, w, h;
	int					ty;
	BYTE				a;

	if(0 == m_Alpha) return;

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);

	ox = (m_DispOx >> 8) + max(0, (128 - m_Alpha*3/2))*0;
	oy = (m_DispOy >> 8);
	w  = max(0, (196 * m_DisplayHP) / m_MaxHP);

	a  = (m_Alpha * 200) / 255;
	if(IsMadnessMode) c1 = RGBA_MAKE(255, 128, 200, a);
	else              c1 = RGBA_MAKE(128, 128, 255, a);
//	c1 = RGBA_MAKE(128, 128, 255, a);
	c2 = RGBA_MAKE(255, 255, 255, a);

	Set2DPointC(tlv+0, ox,   oy+1, 0, 0, c1);
	Set2DPointC(tlv+1, ox+w, oy+1, 0, 0, c1);
	Set2DPointC(tlv+2, ox+w, oy+3, 0, 0, c2);
	Set2DPointC(tlv+3, ox,   oy+3, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	Set2DPointC(tlv+0, ox,   oy+3, 0, 0, c2);
	Set2DPointC(tlv+1, ox+w, oy+3, 0, 0, c2);
	Set2DPointC(tlv+2, ox+w, oy+5, 0, 0, c1);
	Set2DPointC(tlv+3, ox,   oy+5, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	c1 = RGBA_MAKE(255, 0, 0, a/4);
	Set2DPointC(tlv+0, ox+w,   oy+1, 0, 0, c1);
	Set2DPointC(tlv+1, ox+196, oy+1, 0, 0, c1);
	Set2DPointC(tlv+2, ox+196, oy+5, 0, 0, c1);
	Set2DPointC(tlv+3, ox+w,   oy+5, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	a  = m_Alpha;
	w  = 196;

	if(IsMadnessMode) c1 = RGBA_MAKE(128, 64, 64, a);
	else              c1 = RGBA_MAKE( 64, 64, 64, a);
//	c1 = RGBA_MAKE(64, 64, 64, a);

	Set2DPointC(tlv+0, ox,     oy+5, 0, 0, c1);
	Set2DPointC(tlv+1, ox+w/2, oy+5, 0, 0, c2);
	Set2DPointC(tlv+2, ox+w/2, oy+5, 0, 0, c2);
	Set2DPointC(tlv+3, ox+w,   oy+5, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, 4);

	Set2DPointC(tlv+0, ox,     oy, 0, 0, c1);
	Set2DPointC(tlv+1, ox+w/2, oy, 0, 0, c2);
	Set2DPointC(tlv+2, ox+w/2, oy, 0, 0, c2);
	Set2DPointC(tlv+3, ox+w,   oy, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, 4);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_FONT);
	c1 = RGBA_MAKE(a, a, a, a);

	w  = (8 * 9 + max(0, (120 - a)) * 6) / 2;
	h  = 4 - max(0, min(3, (122 - a)));
	ox = ox + 196 / 2 + 64;

	if(IsMadnessMode) ty = 16;
	else              ty = 0;

	if(IsMadnessMode && (255 == a) && (m_Count % 8 < 4)){
		u0 = D3DVAL( 16)    / D3DVAL(256);
		v0 = D3DVAL(200+ty) / D3DVAL(256);
		u1 = D3DVAL(128)    / D3DVAL(256);
		v1 = D3DVAL(216+ty) / D3DVAL(256);
		w -= 8;
	}
	else{
		u0 = D3DVAL(  0)    / D3DVAL(256);
		v0 = D3DVAL(200+ty) / D3DVAL(256);
		u1 = D3DVAL(128+16) / D3DVAL(256);
		v1 = D3DVAL(216+ty) / D3DVAL(256);
	}

	Set2DPointC(tlv+0, ox-w, oy+9-h, u0, v0, c1);
	Set2DPointC(tlv+1, ox+w, oy+9-h, u1, v0, c1);
	Set2DPointC(tlv+2, ox+w, oy+9+h, u1, v1, c1);
	Set2DPointC(tlv+3, ox-w, oy+9+h, u0, v1, c1);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
