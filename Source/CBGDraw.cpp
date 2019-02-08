/*
 *   CBGDraw.cpp   : 背景の描画
 *
 */

#include "CBGDraw.h"

#define BGBLEND_FADETIME	30		// 背景ブレンド推移の速度




/***** [スタティックメンバ] *****/
D3DTLVERTEX CBGDraw::m_RenderList[BG_NUM_VERTICES];		// 頂点格納用配列



// コンストラクタ //
CBGDraw::CBGDraw(RECT *rcTargetX256)
{
	// ウィンドウの幅と高さを求める //
	m_WindowWidth  = ((rcTargetX256->right)  - (rcTargetX256->left)) / 256;
	m_WindowHeight = ((rcTargetX256->bottom) - (rcTargetX256->top))  / 256;

	// ウィンドウの中心座標を求める //
	m_WindowOx = ((rcTargetX256->right)  + (rcTargetX256->left)) / 512;
	m_WindowOy = ((rcTargetX256->bottom) + (rcTargetX256->top))  / 512;
	m_WindowOy = 580;

	// カメラの初期化を行う //
	Initialize();
}


// デストラクタ //
CBGDraw::~CBGDraw()
{
	// このクラスでは、動的に確保するオブジェクトが存在しません //
}


// 座標関連の初期化 //
void CBGDraw::Initialize(void)
{
	m_CurrentX = 0;				// 現在のＸ座標
	m_CurrentY = 0;				// 現在のＹ座標
	m_CurrentZ = BG_Z_DEFAULT;	// 現在のＺ座標
	m_TargetZ  = BG_Z_DEFAULT;	// 目標とするＺ座標

	m_Angle  = 0;	// ＸＹ平面の進行方向
	m_Speed  = 0;	// ＸＹ平面の速度
	m_ZSpeed = 0;	// Ｚ軸方向の速度

	// 最初にフェードインがかかる //
	m_CurrentColor = 0;
	SetBlendColor(0, 0, 0, 0);
	m_BlendCount = BGBLEND_FADETIME;

	m_BossFade  = 255;	// ボス出現時の画面フェード用
	m_BossAlive = 0;	// ボスが生きていれば非ゼロ
}


// １フレーム進行させる //
void CBGDraw::Move(void)
{
	BYTE		r, g, b, a;
	int			t;

	// ボスが出現している場合 //
	if(m_BossAlive){
		m_BossFade = max(160, m_BossFade-4);
	}
	// ボスが死亡している場合 //
	else{
		m_BossFade = min(255, m_BossFade+4);
	}

	// フェード要求のある場合 //
	if(m_BlendCount){
		// 目標色から離れる場合 ->(255, 255, 255) へと //
		if(m_BlendCount < BGBLEND_FADETIME){
			a = (BYTE)( (255 * m_BlendCount) / BGBLEND_FADETIME );
			r = ( (m_TargetRed   * a) + (255 * (255 - a)) ) / 255;
			g = ( (m_TargetGreen * a) + (255 * (255 - a)) ) / 255;
			b = ( (m_TargetBlue  * a) + (255 * (255 - a)) ) / 255;
		}
		// 目標色に近づく場合 ->(r, g, b) へと //
		else if(m_BlendCount > m_BlendStart - BGBLEND_FADETIME){
			a = (BYTE)( (255 * (m_BlendStart - m_BlendCount)) / BGBLEND_FADETIME );
			r = ( (m_TargetRed   * a) + (m_StartRed   * (255 - a)) ) / 255;
			g = ( (m_TargetGreen * a) + (m_StartGreen * (255 - a)) ) / 255;
			b = ( (m_TargetBlue  * a) + (m_StartBlue  * (255 - a)) ) / 255;
		}
		// 目標色で維持する場合 //
		else{
			r = m_TargetRed;
			g = m_TargetGreen;
			b = m_TargetBlue;
		}

		t = m_BossFade + 1;
		r = (r * t) >> 8;
		g = (g * t) >> 8;
		b = (b * t) >> 8;

		m_CurrentColor = RGB_MAKE(r, g, b);
		m_BlendCount--;
	}
	// 維持状態の場合 //
	else{
		r              = m_BossFade;
		m_CurrentColor = RGB_MAKE(r, r, r);
//		m_CurrentColor = RGB_MAKE(255, 255, 255);
	}

	// ＸＹ平面上の移動を行う //
	m_CurrentX += CosL(m_Angle, m_Speed);	// Ｘ座標の更新
	m_CurrentY += SinL(m_Angle, m_Speed);	// Ｙ座標の更新

	// ３倍しているのは縦横のテクスチャが３×３のため //
	if(m_CurrentX <= - 3 * 128 * 256) m_CurrentX += (6 * 128 * 256);
	if(m_CurrentY <= - 3 * 128 * 256) m_CurrentY += (6 * 128 * 256);

	if(m_CurrentX >=  3 * 128 * 256) m_CurrentX -= (6 * 128 * 256);
	if(m_CurrentY >=  3 * 128 * 256) m_CurrentY -= (6 * 128 * 256);


	// Ｚ軸方向の移動を行う //
	if(m_ZSpeed){
		m_CurrentZ += m_ZSpeed;	// 速度成分を加算

		// 接近する場合 //
		if(m_ZSpeed < 0 && (m_CurrentZ <= m_TargetZ)){
			m_ZSpeed   = 0;				// 速度を０とする
			m_CurrentZ = m_TargetZ;		// 目標の値に設定する
		}
		// 離れる場合 //
		else if(m_ZSpeed > 0 && m_CurrentZ >= m_TargetZ){
			m_ZSpeed   = 0;				// 速度を０とする
			m_CurrentZ = m_TargetZ;		// 目標の値に設定する
		}
	}
}


// 描画する //
void CBGDraw::Draw(void)
{
	int				width, height, size;
	int				i, j, k, l;
	int				x, y;
	int				tx, ty;
	int				ox, oy;
	int				wx, wy;
	DWORD			c;
	BYTE			d;
	D3DTLVERTEX		*pv;

	// ローカル変数を初期化する //
	width  = BG_WIDTH;		// 画面の横幅
	height = BG_HEIGHT;		// 画面の縦幅

	size = m_CurrentZ;
	ox   = (width  * BG_Z_DEFAULT) / 2;
	oy   = (height * BG_Z_DEFAULT) / 2;

	wx = m_WindowOx;
	wy = m_WindowOy;
	c  = m_CurrentColor;
	d  = -(64 + m_Angle);

/*
	char buf[1200];
	wsprintf(buf, "(%d, %d)", wx, wy);
	PbgLog(buf);
*/
	// レンダリングステートを一括設定 //
	g_pGrp->SetRenderStateEx(GRPST_NORMAL);

	for(j=0; j<height; j++){
		for(i=0; i<width; i++){
			// テクスチャをセットする //
			g_pGrp->SetTexture(TEXTURE_ID_BG01);// + i + j*width);
//			g_pGrp->SetTexture(GRPTEXTURE_MAX);

			pv = m_RenderList;	// ポインタを頂点リストの先頭にセット

			for(k=-3; k<6; k+=3){
				for(l=-3; l<6; l+=3){
					tx = BG_Z_DEFAULT * (i+k) - m_CurrentX - ox;
					ty = BG_Z_DEFAULT * (j+l) - m_CurrentY - oy;
					x = (256 * (CosL(d, tx) - SinL(d, ty))) / size + wx;
					y = (256 * (SinL(d, tx) + CosL(d, ty))) / size + wy;
					Set2DPointC(pv+0, x, y, 0.0, 0.0, c);
					Set2DPointC(pv+5, x, y, 0.0, 0.0, c);

					tx = BG_Z_DEFAULT * (i+k+1) - m_CurrentX - ox;
					ty = BG_Z_DEFAULT * (j+l)   - m_CurrentY - oy;
					x = (256 * (CosL(d, tx) - SinL(d, ty))) / size + wx;
					y = (256 * (SinL(d, tx) + CosL(d, ty))) / size + wy;
					Set2DPointC(pv+1, x, y, 1.0, 0.0, c);

					tx = BG_Z_DEFAULT * (i+k+1) - m_CurrentX - ox;
					ty = BG_Z_DEFAULT * (j+l+1) - m_CurrentY - oy;
					x = (256 * (CosL(d, tx) - SinL(d, ty))) / size + wx;
					y = (256 * (SinL(d, tx) + CosL(d, ty))) / size + wy;
					Set2DPointC(pv+2, x, y, 1.0, 1.0, c);
					Set2DPointC(pv+3, x, y, 1.0, 1.0, c);

					tx = BG_Z_DEFAULT * (i+k)   - m_CurrentX - ox;
					ty = BG_Z_DEFAULT * (j+l+1) - m_CurrentY - oy;
					x = (256 * (CosL(d, tx) - SinL(d, ty))) / size + wx;
					y = (256 * (SinL(d, tx) + CosL(d, ty))) / size + wy;
					Set2DPointC(pv+4, x, y, 0.0, 1.0, c);

					pv+=6;
				}
			}

			g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, 6*9);
		}
	}
}


// 背景のγランプをセットする //
void CBGDraw::SetBlendColor(BYTE r, BYTE g, BYTE b, DWORD Count)
{
	DWORD		c;

	m_TargetRed   = r;
	m_TargetGreen = g;
	m_TargetBlue  = b;

	c            = m_CurrentColor;
	m_StartRed   = (BYTE)RGBA_GETRED(c);
	m_StartGreen = (BYTE)RGBA_GETGREEN(c);
	m_StartBlue  = (BYTE)RGBA_GETBLUE(c);

	m_BlendStart = Count + (BGBLEND_FADETIME * 2);
	m_BlendCount = m_BlendStart;
}


// ボス出現用エフェクト //
void CBGDraw::SetBossAlive(BOOL bBossAlive)
{
	m_BossAlive = bBossAlive;
}


// スクロールスピードをセットする //
void CBGDraw::SetSpeed(int Speed)
{
	m_Speed = abs(Speed);
}


// 進行方向を変化させる(絶対指定) //
void CBGDraw::SetAngleA(BYTE Angle)
{
	m_Angle = Angle;
}


// 進行方向を変化させる(相対指定) //
void CBGDraw::SetAngleR(char dAngle)
{
	m_Angle += dAngle;
}


// Ｚ座標を変化させる //
void CBGDraw::SetZoom(int NewZ, int Speed)
{
	m_TargetZ = max(1, NewZ);

	if(m_TargetZ < m_CurrentZ) m_ZSpeed = -abs(Speed);
	else                       m_ZSpeed =  abs(Speed);
}


// 画面幅の取得 //
FINT CBGDraw::GetTargetWidth(void)
{
	return m_WindowWidth << 8;
}


// 画面中央の取得 //
FINT CBGDraw::GetTargetOx(void)
{
	return m_WindowOx << 8;
}
