/*
 *   ChargeEfc.cpp   : 溜め完了エフェクト描画
 *
 */

#include "ChargeEfc.h"
#include "RndCtrl.h"

#define CEFC_TIME		90		// 溜めエフェクトにかかる時間



// コンストラクタ //
CChargeEfc::CChargeEfc(int *pX, int *pY)
	: m_pX(pX), m_pY(pY)
{
	Initialize();
}


// デストラクタ //
CChargeEfc::~CChargeEfc()
{
}


// 初期化する //
FVOID CChargeEfc::Initialize(void)
{
	m_Count      = 0;	// エフェクト用カウンタ
	m_GBEfcCount = 0;	// ガードブレイク終了エフェクト(０なら無効)
}


// エフェクトをセットする //
FVOID CChargeEfc::Set(BYTE Level)
{
	m_Count = CEFC_TIME;
	m_Level = Level;

	m_rx = (BYTE)RndEx();
	m_ry = (BYTE)RndEx();
	m_rz = (BYTE)RndEx();
}


// ガードブレイク終了エフェクトをセットする //
FVOID CChargeEfc::SetGBFinished(void)
{
	m_GBEfcCount = CEFC_TIME;
}


// １フレーム分動作させる //
FVOID CChargeEfc::Move(void)
{
	// エフェクトは発動していない //
	if(m_Count){
		m_rx += 4;
		m_ry += 2;

		m_Count--;
	}

	// ガードブレイクエフェクト //
	if(m_GBEfcCount){
		m_GBEfcCount--;
	}
}


// 描画する //
FVOID CChargeEfc::Draw(void)
{
	D3DTLVERTEX			tlv[20];
	int					t, d, l;
	int					x, y;
	DWORD				c;

	if(m_Count){
		DrawChargeEfc();	// 正四面体の描画
		DrawExtraString();	// 文字列の描画
	}

	if(m_GBEfcCount){
		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
		g_pGrp->SetTexture(TEXTURE_ID_GUARD);

		x = *m_pX;
		y = *m_pY;

		t = (m_GBEfcCount * 220) / CEFC_TIME;
		d = 110 - abs(t - 110);
		c = RGB_MAKE(d, d, d*2);
		l = t << 7;

		Set2DPointC(tlv+0, (x-l)>>8, (y-l)>>8, 0.0, 0.0, c);
		Set2DPointC(tlv+1, (x+l)>>8, (y-l)>>8, 1.0, 0.0, c);
		Set2DPointC(tlv+2, (x+l)>>8, (y+l)>>8, 1.0, 1.0, c);
		Set2DPointC(tlv+3, (x-l)>>8, (y+l)>>8, 0.0, 1.0, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}



// 溜め完了エフェクト描画 //
FVOID CChargeEfc::DrawChargeEfc(void)
{
	D3DTLVERTEX		tlv[50];
	D3DTLVERTEX		temp[8];
	int				i, ox, oy, l, t;
	DWORD			c0, c1, c2;
	BYTE			rx, ry, rz;
	BYTE			r, g, b;

	static int TriIndex[24] = {
		0, 1, 5, 4,
		0, 2, 6, 4,
		0, 3, 7, 4,
		1, 2, 6, 5,
		2, 3, 7, 6,
		3, 1, 5, 7,
	};

	rx = m_rx;	// Ｘ軸に対する回転角
	ry = m_ry;	// Ｙ軸に対する回転角
	rz = m_rz;	// Ｚ軸に対する回転角

	ox = (*m_pX) >> 8;				// 中心となるＸ座標
	oy = (*m_pY) >> 8;				// 中心となるＹ座標
	t  = m_Count;					// 残りカウント
	l  = (t * 120) / CEFC_TIME;		// 中心からの距離

	t = CEFC_TIME - t;

	r = (32 * t) / CEFC_TIME;
	c1 = RGBA_MAKE(r, r, r, 255);

	switch(m_Level){
		case 2:	default:
			r = ( 83 * t) / CEFC_TIME;
			g = (228 * t) / CEFC_TIME;
			b = (152 * t) / CEFC_TIME;

			c0 = RGBA_MAKE(r, g, b, min(128, (255 * t) / CEFC_TIME));
			c2 = RGBA_MAKE(r, g, b, 255);
		break;

		case 3:
			r = (231 * t) / CEFC_TIME;
			g = (162 * t) / CEFC_TIME;
			b = ( 44 * t) / CEFC_TIME;

			c0 = RGBA_MAKE(r, g, b, min(128, (255 * t) / CEFC_TIME));
			c2 = RGBA_MAKE(r, g, b, 255);
		break;

		case 4:
			r = (238 * t) / CEFC_TIME;
			g = ( 74 * t) / CEFC_TIME;
			b = (113 * t) / CEFC_TIME;

			c0 = RGBA_MAKE(r, g, b, min(128, (255 * t) / CEFC_TIME));
			c2 = RGBA_MAKE(r, g, b, 255);
		break;
	}

	Setup3DPoint(temp, ox, oy, l, c0, rx, ry, rz);
	Setup3DPoint(temp+4, ox, oy, l, c0, rx+16, ry+8, rz);

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

	for(i=0; i<24; i+=4){
		tlv[0] = temp[TriIndex[i+0]];
		tlv[1] = temp[TriIndex[i+1]];
		tlv[2] = temp[TriIndex[i+2]];
		tlv[3] = temp[TriIndex[i+3]];
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	for(i=0; i<4; i++){
		temp[i+0].color = c1;
		temp[i+4].color = c2;
	}

	for(i=0; i<24; i+=4){
		tlv[0] = temp[TriIndex[i+0]];
		tlv[1] = temp[TriIndex[i+1]];
		tlv[2] = temp[TriIndex[i+2]];
		tlv[3] = temp[TriIndex[i+3]];
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// 文字列 (LvXX CHARGE OK) の表示 //
FVOID CChargeEfc::DrawExtraString(void)
{
	D3DVALUE		u1, v1, u2, v2;
	D3DTLVERTEX		tlv[20];
	DWORD			c;
	int				ox, oy;
	int				dx;//, dy;
	int				t;

	ox = ((*m_pX) >> 8);
	oy = ((*m_pY) >> 8);

	c = RGBA_MAKE(0, 255, 0, 32);
	Set2DPointC(tlv+0, ox, oy, 0, 0, c);

	c = RGBA_MAKE(0, 255, 0, 64);
	Set2DPointC(tlv+1, ox+20, oy+30, 0, 0, c);

	c = RGBA_MAKE(0, 255, 0, 96);
	Set2DPointC(tlv+2, ox+30, oy+30, 0, 0, c);

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 3);

	t = min(CEFC_TIME, 2 * (CEFC_TIME - m_Count));
	dx = (144 * t) / CEFC_TIME;
	dx -= (dx%16);

	u1 = D3DVAL(  0) / D3DVAL(256);
	v1 = D3DVAL(152) / D3DVAL(256);
	u2 = D3DVAL( dx) / D3DVAL(256);
	v2 = D3DVAL(168) / D3DVAL(256);

	ox += 30;
	oy += 26;

	c = RGBA_MAKE(255, 255, 255, 255);

	Set2DPointC(tlv+0, ox     , oy     , u1, v1, c);
	Set2DPointC(tlv+1, ox+dx/3, oy     , u2, v1, c);
	Set2DPointC(tlv+2, ox+dx/3, oy+16/3, u2, v2, c);
	Set2DPointC(tlv+3, ox     , oy+16/3, u1, v2, c);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_FONT);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


	if((t < CEFC_TIME) || (m_Count % 8 > 3)){
		u1 = D3DVAL(144) / D3DVAL(256);
		v1 = D3DVAL(152) / D3DVAL(256);
		u2 = D3DVAL(160) / D3DVAL(256);
		v2 = D3DVAL(168) / D3DVAL(256);

		Set2DPointC(tlv+0, ox+dx/3,   oy     , u1, v1, c);
		Set2DPointC(tlv+1, ox+dx/3+8, oy     , u2, v1, c);
		Set2DPointC(tlv+2, ox+dx/3+8, oy+16/3, u2, v2, c);
		Set2DPointC(tlv+3, ox+dx/3,   oy+16/3, u1, v2, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	u1 = D3DVAL(m_Level*32-64) / D3DVAL(256);
	v1 = D3DVAL(168)           / D3DVAL(256);
	u2 = D3DVAL(m_Level*32-32) / D3DVAL(256);
	v2 = D3DVAL(184)           / D3DVAL(256);

	c = (255 * t) / CEFC_TIME;
	c = RGBA_MAKE(c, c, c, 255);
	Set2DPointC(tlv+0, ox,    oy-16/2, u1, v1, c);
	Set2DPointC(tlv+1, ox+16, oy-16/2, u2, v1, c);
	Set2DPointC(tlv+2, ox+16, oy     , u2, v2, c);
	Set2DPointC(tlv+3, ox,    oy     , u1, v2, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 頂点をセットする //
FVOID CChargeEfc
	::Setup3DPoint(D3DTLVERTEX	*tlv	// 頂点バッファ
				, int			x		// 中心となるＸ座標(非x256)
				, int			y		// 中心となるＹ座標(非x256)
				, int			l		// 中心からの距離(非x256)
				, DWORD			c		// 頂点色
				, BYTE			rx		// Ｘ軸に対する回転
				, BYTE			ry		// Ｙ軸に対する回転
				, BYTE			rz)		// Ｚ軸に対する回転
{
	int			dx, dy, l2, i;

	l <<= 8;					// x256 に変換
	l2 = CosL((256*30)/360, l);

	Set2DPointC(tlv+0, 0, 0, 0, 0, c);

	dx = CosL(0, l2) >> 8;
	dy = SinL(0, l2) >> 8;
	Set2DPointC(tlv+1, dx, dy, 0, 0, c);

	dx = CosL(256/3, l2) >> 8;
	dy = SinL(256/3, l2) >> 8;
	Set2DPointC(tlv+2, dx, dy, 0, 0, c);

	dx = CosL(256*2/3, l2) >> 8;
	dy = SinL(256*2/3, l2) >> 8;
	Set2DPointC(tlv+3, dx, dy, 0, 0, c);

	tlv[0].sz = (float)(l >> 8);
	tlv[1].sz = tlv[2].sz = tlv[3].sz = -(float)(SinL((256*30)/360, l) >> 8);

	// 後は、指定された角度で回転を行う //
	RotateTLV(tlv, 4, rx, ry, rz);

	for(i=0; i<4; i++){
		tlv[i].sx += x;
		tlv[i].sy += y;
		tlv[i].sz  = 0;
	}
}
