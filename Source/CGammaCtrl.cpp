/*
 *   CGammaCtrl.cpp   : γ補正処理
 *
 */

#include "CGammaCtrl.h"
#include "Gian2001.h"
#include "FontDraw.h"



// コンストラクタ //
CGammaCtrl::CGammaCtrl()
{
	m_bInitialized = FALSE;		// 初期化されていない
	m_Gamma        = 100;		// 補正値 100/100 = 1.0

	m_sx = 0;	// 左上のＸ座標
	m_sy = 0;	// 左上のＹ座標
}


// デストラクタ //
CGammaCtrl::~CGammaCtrl()
{
}


// 補正値を増減させる //
FBOOL CGammaCtrl::SetGamma(int Gamma)
{
	// 範囲外チェック //
	if(     Gamma > 200) Gamma = 200;	// 最大値制限
	else if(Gamma <  50) Gamma = 50;	// 最小値制限

	// 実際に代入する //
	m_Gamma = Gamma;

	// 実際に値を反映させる //
	SetGammaTable();

	return TRUE;
}


// 補正値を取得する //
FBYTE CGammaCtrl::GetGamma(void)
{
	return (BYTE)m_Gamma;
}


// ウィンドウ周りの動作 //
FVOID CGammaCtrl::Move(void)
{
}


// ウィンドウ周りの描画 //
FVOID CGammaCtrl::Draw(void)
{
	D3DTLVERTEX	tlv[128*3];
	DWORD		c1, c2;
	int			x1, y1, x2, y2;
	int			i, r, g, b;

	x1 = m_sx;
	y1 = m_sy;
	x2 = x1 + 128 + 16;
	y2 = y1 + 128;


//	c1 = RGBA_MAKE(0, 0, 0, 255);
	c1 = RGBA_MAKE(0, 0, 64, 128+32);
	c2 = RGBA_MAKE(0, 64, 128, 128+32);

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);

	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c1);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c2);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	c1 = RGBA_MAKE(128, 128, 255, 255);

	for(i=0; i<128; i++){
		r = m_CurrentGamma.red[i*2]   / (256 * 2);
		g = m_CurrentGamma.green[i*2] / (256 * 2);
		b = m_CurrentGamma.blue[i*2]  / (256 * 2);
		r = (r + g + b) / 3;

		Set2DPointC(tlv+i, x1+i, y1+127-r, 0, 0, c1);
	}
	g_pGrp->DrawPrimitive(D3DPT_POINTLIST, tlv, 128);


	c1 = RGBA_MAKE(196, 196, 196, 255);
	c2 = RGBA_MAKE(128, 128, 128, 255);

	g_pGrp->SetRenderStateEx(GRPST_NORMAL);

	x1--;y1--;
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c2);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c1);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	Set2DPointC(tlv+4, x1, y1, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

	x1-=2;y1-=2;
	x2+=2;y2+=2;
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c2);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c1);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	Set2DPointC(tlv+4, x1, y1, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

	Set2DPointC(tlv+0, x2  , y2 , 0, 0, c1);
	Set2DPointC(tlv+1, x2-2, y2-2, 0, 0, c1);
	g_pGrp->DrawPrimitive(D3DPT_POINTLIST, tlv, 2);

	x1 = m_sx + 128 + 4;
	y1 = m_sy;
	x2 = x1 + 8;
	y2 = y1 + 128;

	c1 = RGBA_MAKE(255, 255, 255, 255);
	c2 = RGBA_MAKE(  0,   0,   0, 255);
	Set2DPointC(tlv+0, x1, y1, 0, 0, c1);
	Set2DPointC(tlv+1, x2, y1, 0, 0, c1);
	Set2DPointC(tlv+2, x2, y2, 0, 0, c2);
	Set2DPointC(tlv+3, x1, y2, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	char	buf[128];
	wsprintf(buf, "r=%d.%02d", m_Gamma/100, m_Gamma%100);
	g_Font.DrawAlpha(m_sx+32, m_sy+110, buf, 255, 14);
}


// 絶対指定で座標を変更 //
FVOID CGammaCtrl::SetWindowPos(int sx, int sy)
{
	m_sx = sx;		// ウィンドウの左上のＸ座標
	m_sy = sy;		// ウィンドウの左上のＹ座標
}


#include <math.h>

// 現在のγ値を反映させる //
FVOID CGammaCtrl::SetGammaTable(void)
{
	int		i;
	double	r, g, b;
	double	gm;

	// γテーブルが初期化されていないので、ここで取得する //
	if(FALSE == m_bInitialized){
		g_pGrp->GetGammaRamp(&m_DefaultGamma);	// γ値
		m_bInitialized = TRUE;					// 初期化完了
	}

	// γ値を浮動小数点数に変換 //
	gm = 1.0 / ((double)m_Gamma / 100.0);

	for(i=0; i<256; i++){
		// 元の値を [0.0, 1.0] でおさえる //
		r = ((double)m_DefaultGamma.red[i])   / 65535.0;	// 赤γランプ
		g = ((double)m_DefaultGamma.green[i]) / 65535.0;	// 緑γランプ
		b = ((double)m_DefaultGamma.blue[i])  / 65535.0;	// 青γランプ

		// 参考 : Y (YUV) の輝度値とした時、Y = 255 * (Y / 255)^(1 / γ) //
		// ちなみに、γのテーブルは２バイト値である事に注意              //
		m_CurrentGamma.red[i]   = (WORD)(65535.0 * pow(r, gm));	// 赤
		m_CurrentGamma.green[i] = (WORD)(65535.0 * pow(g, gm));	// 緑
		m_CurrentGamma.blue[i]  = (WORD)(65535.0 * pow(b, gm));	// 青
	}

	// 後は任せる //
	g_pGrp->SetGammaRamp(&m_CurrentGamma);
}
