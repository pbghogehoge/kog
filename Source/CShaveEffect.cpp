/*
 *   CShaveEffect.cpp   : カスりエフェクト管理
 *
 */

#include "CShaveEffect.h"
#include "SECtrl.h"
#include "FontDraw.h"

#include "RndCtrl.h"



#define FRAGMENT_LENGTH			(100 * 256)
#define FRAGMENT_SPEED			(6  * 256)
#define SHAVE_INTERVALSPEED		2


// コンストラクタ //
CShaveEffect::CShaveEffect(RECT *rcTargetX256)
{
	m_XMin = rcTargetX256->left;	// 左端の座標
	m_YMin = rcTargetX256->top;		// 上端の座標
	m_XMax = rcTargetX256->right;	// 右端の座標
	m_YMax = rcTargetX256->bottom;	// 下端の座標

	Initialize();
}


// デストラクタ //
CShaveEffect::~CShaveEffect()
{
}


// 初期化を行う //
FVOID CShaveEffect::Initialize(void)
{
	m_ShaveCombo = 0;
	m_Timer      = 0;

	// リストを初期化する //
	DeleteAllData();
}


// エフェクトをセットする //
FVOID CShaveEffect::Set(int x, int y, BYTE d)
{
	int			i;
	int			Angle;
	ShaveData	*pFragment;


	m_Timer = 255;
	m_ShaveCombo++;

	// ４桁なら５６、３桁なら４８ //
	const int FontSize = 48 * 256;

	if(     x < m_XMin + FontSize) x = m_XMin + FontSize;
	else if(x > m_XMax - FontSize) x = m_XMax - FontSize;

	m_ComboX = x;
	m_ComboY = y - 256 * 70;


	for(i=0; i<2; i++){
		pFragment = InsertBack(0);
		if(NULL == pFragment) return;

		// まずは、発射角を求める //
		Angle = d + (RndEx() % 20 - 10);

		pFragment->Length = 0;		// 現在の長さ
		pFragment->Count  = 0;		// 現在の経過時間
		pFragment->Angle  = Angle;	// 発射角
		pFragment->x      = x;		// 現在のＸ座標
		pFragment->y      = y;		// 現在のＹ座標

		pFragment->vx     = CosL(Angle, FRAGMENT_SPEED);
		pFragment->vy     = SinL(Angle, FRAGMENT_SPEED);
	}

	PlaySE(SNDID_SHAVE, x);
}


// エフェクトを１フレーム動作させる //
FVOID CShaveEffect::Move(void)
{
	Iterator			it;
	ShaveData			*pFragment;
	int					mx, my;
	int					XMin, YMin, XMax, YMax;

	// メンバ変数 -> ローカル変数に変換して、高速化を図る //
	XMin = m_XMin;		// 左端の座標
	YMin = m_YMin;		// 上端の座標
	XMax = m_XMax;		// 右端の座標
	YMax = m_YMax;		// 下端の座標


	ThisForEachFront(0, it){
		pFragment = it.GetPtr();

		if(pFragment->Length < FRAGMENT_LENGTH){
			pFragment->Length += (FRAGMENT_SPEED*3);
		}
		else{
			pFragment->x += pFragment->vx;
			pFragment->y += pFragment->vy;
		}

		mx = pFragment->x;
		my = pFragment->y;

		if(mx < XMin || mx > XMax || my < YMin  || my > YMax){
			ThisDelContinue(it);
		}

		pFragment->Count++;
	}

	if(m_Timer > SHAVE_INTERVALSPEED){
		m_Timer -= SHAVE_INTERVALSPEED;
	}
	else{
		m_Timer      = 0;
		m_ShaveCombo = 0;
	}
}


// 破片を描画する //
FVOID CShaveEffect::DrawFragment(void)
{
	Iterator			it;
	ShaveData			*pFragment;
	D3DTLVERTEX			tlv[SHAVE_MAX * 2], *ptlv;
	int					x, y, l, n;
	BYTE				d;

	ptlv = tlv;

	ThisForEachFront(0, it){
		pFragment = it.GetPtr();

		x = pFragment->x;
		y = pFragment->y;
		Set2DPointC(ptlv+0, x>>8, y>>8, 0, 0, RGBA_MAKE(150, 150, 255,  32));

		d  = pFragment->Angle;
		l  = pFragment->Length;
		x += CosL(d, l);
		y += SinL(d, l);
		Set2DPointC(ptlv+1, x>>8, y>>8, 0, 0, RGBA_MAKE(225, 225, 255, 230));

		ptlv += 2;
	}

	n = ptlv - tlv;

	if(n){
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
		g_pGrp->SetTexture(GRPTEXTURE_MAX);
		g_pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, n);
	}
}


// コンボ数を描画する //
FVOID CShaveEffect::DrawCombo(void)
{
	BYTE		alpha;
	char		buf[10];

	if(m_Timer){
		if(m_Timer > 223)     alpha = (255-m_Timer) * 8;
		else if(m_Timer < 32) alpha = (m_Timer)     * 8;
		else                  alpha = 255;

		wsprintf(buf, "%d", m_ShaveCombo);
		g_Font.DrawCombo(m_ComboX>>8, m_ComboY>>8, buf, alpha);
	}
}


// 強制リセット                                              //
// Initialize() とは異なり、ラインエフェクトの消去は行わない //
FVOID CShaveEffect::ForceReset(void)
{
	m_Timer      = 0;
	m_ShaveCombo = 0;
}
