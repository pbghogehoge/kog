/*
 *   CEnemyLSphere.cpp   : らいとにんぐすふぃあ
 *
 */

#include "CEnemyLSphere.h"

#include "RndCtrl.h"


static const int ELS_WAIT_COUNT   = 60;
static const int ELS_NORMAL_COUNT = 160;
static const int ELS_DELETE_COUNT = 20;

static const int LSPHERE_DAMAGE  = 56789;		// 約５％
static const int LSPHERE_HITSIZE = 10 * 256;	// 当たり判定サイズ



// コンストラクタ //
CEnemyLSphere::CEnemyLSphere(
							RECT			*rcTargetX256	// 対象矩形
						,	int				*pX				// 当たり判定Ｘ座標へのポインタ
						,	int				*pY				// 当たり判定Ｙ座標へのポインタ
						,	CShaveEffect	*pShaveEfc)		// カスりエフェクト発動用クラス
{
//PbgError("コンストラクタ");
	m_pX = pX;		// 当たり判定Ｘへのポインタ
	m_pY = pY;		// 当たり判定Ｙへのポインタ

	m_pShaveEfc = pShaveEfc;	// カスり管理クラスへのポインタ

	m_XMin = rcTargetX256->left;	// 画面左端
	m_YMin = rcTargetX256->top;		// 画面上端
	m_XMax = rcTargetX256->right;	// 画面右端
	m_YMax = rcTargetX256->bottom;	// 画面下端

	m_XMid = (m_XMin + m_XMax) >> 1;
	m_YMid = (m_YMin + m_YMax) >> 1;
/*
char hoge[100];
wsprintf(hoge, "%u %u", pX, pY);
PbgError(hoge);
*/
}


// デストラクタ //
CEnemyLSphere::~CEnemyLSphere()
{
	// ここでは、特になにもしません //
}


// セットする //
FVOID CEnemyLSphere::Set(int AtkLv)
{
	LSphere		*pSp;
	POINT		*pTarget;
	int			i, n, t;
	int			dx, dy;
	int			*pLength;
	BYTE		*pAngle;

#define set_point(index, tx, ty)	\
{									\
	pTarget[index].x = tx;			\
	pTarget[index].y = ty;			\
}

	pSp = InsertBack(ELSPHERE_WAIT);// 挿入＆ポインタ取得
	if(NULL == pSp) return;			// 空き領域が存在せず

	pTarget           = pSp->m_CurrentPos;
	pSp->m_StateCount = 0;
	pSp->m_AnmCount   = 0;

	switch(AtkLv){
	// Level Z E R O //
	case 0:		// 大きな矩形
		pSp->m_NumSphere = 8;
		set_point(0, m_XMin+32*256, m_YMin+64*256);
		set_point(1, m_XMid,        m_YMin+64*256);
		set_point(2, m_XMax-32*256, m_YMin+64*256);
		set_point(3, m_XMax-32*256, m_YMid       );
		set_point(4, m_XMax-32*256, m_YMax-64*256);
		set_point(5, m_XMid,        m_YMax-64*256);
		set_point(6, m_XMin+32*256, m_YMax-64*256);
		set_point(7, m_XMin+32*256, m_YMid       );
	break;

	// Level 1 //
	case 1:		// 左側に矩形
		pSp->m_NumSphere = 6;
		set_point(0, m_XMin+16*256, m_YMin+64*256);
		set_point(1, m_XMid       , m_YMin+64*256);
		set_point(2, m_XMid       , m_YMid       );
		set_point(3, m_XMid       , m_YMax-64*256);
		set_point(4, m_XMin+16*256, m_YMax-64*256);
		set_point(5, m_XMin+16*256, m_YMid       );
	break;

	// Level 2 //
	case 2:
		if(RndEx()%256 < 128){
			// 右側に矩形
			pSp->m_NumSphere = 6;
			set_point(0, m_XMid       , m_YMin+64*256);
			set_point(1, m_XMax-16*256, m_YMin+64*256);
			set_point(2, m_XMax-16*256, m_YMid       );
			set_point(3, m_XMax-16*256, m_YMax-64*256);
			set_point(4, m_XMid       , m_YMax-64*256);
			set_point(5, m_XMid       , m_YMid       );
		}
		else{
			// 中央に矩形
			pSp->m_NumSphere = 6;
			set_point(0, m_XMid-64*256, m_YMin+64*256);
			set_point(1, m_XMid+64*256, m_YMin+64*256);
			set_point(2, m_XMid+64*256, m_YMid       );
			set_point(3, m_XMid+64*256, m_YMax-64*256);
			set_point(4, m_XMid-64*256, m_YMax-64*256);
			set_point(5, m_XMid-64*256, m_YMid       );
		}
	break;

	// Level 3 //
	case 3:		// 中央に邪魔な三角形
		pSp->m_NumSphere = 3;
		n = RndEx();
		for(i=0; i<3; i++, n+=256/3){
			set_point(i, m_XMid+CosL(n, 192*256), m_YMid+SinL(n, 192*256));
		}
	break;

	// Level 4 //
	case 4:
		if(RndEx()%256 < 128){
			// 下底のほうが小さい台形
			pSp->m_NumSphere = 7;
			set_point(0, m_XMin+16*256, m_YMin+64*256);
			set_point(1, m_XMid,        m_YMin+64*256);
			set_point(2, m_XMax-16*256, m_YMin+64*256);
			set_point(3, m_XMax-16*256, m_YMid       );
			set_point(4, m_XMid+64*256, m_YMax-96*256);
			set_point(5, m_XMid-64*256, m_YMax-96*256);
			set_point(6, m_XMin+16*256, m_YMid       );
		}
		else{
			// 画面下に矩形
			pSp->m_NumSphere = 6;
			set_point(0, m_XMin+16*256, m_YMid+96*256);
			set_point(1, m_XMid,        m_YMid+96*256);
			set_point(2, m_XMax-16*256, m_YMid+96*256);
			set_point(3, m_XMax-16*256, m_YMax-64*256);
			set_point(4, m_XMid,        m_YMax-64*256);
			set_point(5, m_XMin+16*256, m_YMax-64*256);
		}
	break;

	// Level 5 //
	case 5: default:	// 中央に「とっても」邪魔なお星様
		pSp->m_NumSphere = 5;
		n = RndEx();
		for(i=0; i<5; i++, n+=(256*2)/5){
			set_point(i, m_XMid+CosL(n, 256*256), m_YMid+SinL(n, 256*256));
		}
	break;
	}

	n       = pSp->m_NumSphere;
	pAngle  = pSp->m_AngleTable;
	pLength = pSp->m_LengthTable;
	for(i=0; i<n; i++, pAngle++, pLength++){
		// すなわち、t = i % n //
		if(i >= n-1) t = 0;
		else         t = i + 1;

		dx = pTarget[t].x - pTarget[i].x;
		dy = pTarget[t].y - pTarget[i].y;

		*pAngle  = Atan8(dx, dy);
		*pLength = Isqrt((dx * dx) + (dy * dy));
	}
}


// 初期化する //
FVOID CEnemyLSphere::Initialize(void)
{
	// リスト内のデータを解放する //
	DeleteAllData();
}


// びりびり球体全てに消去エフェクトをセットする //
FVOID CEnemyLSphere::Clear(void)
{
#pragma message("<<<<<<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>")
#pragma message("このコードは、後に変更を行う必要があります")
	Initialize();
#pragma message("<<<<<<<<<<<< W A R N I N G >>>>>>>>>>>>>>>>>>")
}


// びりびり球体を移動させる //
FVOID CEnemyLSphere::Move(void)
{
	Iterator		it;
	LSphere			*pSp;

	g_pGrp->SetTexture(TEXTURE_ID_HLASER);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	// 発動前の予備動作中 //
	ThisForEachFront(ELSPHERE_WAIT, it){
		pSp = it.GetPtr();
		if(pSp->m_StateCount >= ELS_WAIT_COUNT){
			pSp->m_StateCount = 0;
			MoveBack(it, ELSPHERE_NORMAL);
			continue;
		}

		pSp->m_StateCount++;
		pSp->m_AnmCount++;
	}

	// びりびり発動中 //
	ThisForEachFront(ELSPHERE_NORMAL, it){
		pSp = it.GetPtr();
		if(pSp->m_StateCount >= ELS_NORMAL_COUNT){
			pSp->m_StateCount = 0;
			MoveBack(it, ELSPHERE_DELETE);
			continue;
		}

		pSp->m_StateCount++;
		pSp->m_AnmCount++;
	}

	// へこへこ消去中 //
	ThisForEachFront(ELSPHERE_DELETE, it){
		pSp = it.GetPtr();
		if(pSp->m_StateCount >= ELS_DELETE_COUNT){
			ThisDelContinue(it);
		}

		pSp->m_StateCount++;
		pSp->m_AnmCount++;
	}
}


// びりびり球体を描画する //
FVOID CEnemyLSphere::Draw(void)
{
	Iterator		it;
	LSphere			*pSp;
	BYTE			a;
//static const int ELS_WAIT_COUNT   = 60;
//static const int ELS_NORMAL_COUNT = 120;
//static const int ELS_DELETE_COUNT = 60;

	// 発動前の予備動作中 //
	ThisForEachFront(ELSPHERE_WAIT, it){
		pSp = it.GetPtr();

		a = pSp->m_StateCount * 255 / ELS_WAIT_COUNT;
		DrawThunder(pSp, a);
		DrawSphere(pSp, a);
	}

	// びりびり発動中 //
	ThisForEachFront(ELSPHERE_NORMAL, it){
		pSp = it.GetPtr();
		DrawThunder(pSp, 255);
		DrawSphere(pSp, 255);
	}

	// へこへこ消去中 //
	ThisForEachFront(ELSPHERE_DELETE, it){
		pSp = it.GetPtr();

		a = 255-pSp->m_StateCount * 255 / ELS_DELETE_COUNT;
		DrawThunder(pSp, a);
		DrawSphere(pSp, a);
	}
}


// 当たり判定を行う               //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemyLSphere::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	Iterator		it;
	int				mx, my;
	int				tx, ty;
	int				i, n;
	int				w, l;
	BYTE			*pAngle;
	BYTE			d;
	POINT			*pPoint;
	int				*pLength;
	LSphere			*pSp;

	mx = (*m_pX);		// 自機のＸ座標
	my = (*m_pY);		// 自機のＹ座標

	// びりびり発動中 //
	ThisForEachFront(ELSPHERE_NORMAL, it){
		pSp     = it.GetPtr();

		pAngle  = pSp->m_AngleTable;
		pLength = pSp->m_LengthTable;
		pPoint  = pSp->m_CurrentPos;
		n       = pSp->m_NumSphere;

		// もう少し、高速化できる筈 //
		for(i=0; i<n; i++, pAngle++, pPoint++, pLength++){
			tx = mx - (pPoint->x);
			ty = my - (pPoint->y);
			d  = *pAngle;

			l =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
			w = -SinL(d, tx) + CosL(d, ty);	// 幅方向

			if( (abs(w) < LSPHERE_HITSIZE) && (l > 0) && (l < (*pLength)) ){
				(*pDamage) += LSPHERE_DAMAGE;
				break;		// for 抜け
			}
		}
	}

	/*
	pLaser = it.GetPtr();	// レーザー構造体へのポインタを取得する
	tx = mx - pLaser->x;	// ベクトルのＸ成分
	ty = my - pLaser->y;	// ベクトルのＹ成分
	d  = pLaser->Angle;		// レーザーの進行方向

	// レーザーと自機との距離を求める //
	length =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
	width  = -SinL(d, tx) + CosL(d, ty);	// 幅方向
	awidth = abs(width);					// 幅の絶対値
*/
}


// びりびり球体の描画 //
FVOID CEnemyLSphere::DrawSphere(LSphere *pSp, BYTE a)
{
	D3DVALUE		u0, u1, v0, v1;
	D3DTLVERTEX		tlv[32];
	int				i, n, x, y, dx, dy, t, l;
	DWORD			c;
	POINT			*pt;

	// ここから下は、ホーミングレーザー頭の描画となる //
	u0  = D3DVAL( 0) / D3DVAL(256);
	u1  = D3DVAL(64) / D3DVAL(256);
	v0  = D3DVAL( 0) / D3DVAL(256);
	v1  = D3DVAL(64) / D3DVAL(256);

	n  = pSp->m_NumSphere;
	pt = pSp->m_CurrentPos;
	t  = pSp->m_AnmCount;
	c  = RGBA_MAKE(a, a, a, a);

	l  = SinL(t*12, 9*256) + 22*256;
	dx = CosL(-t*29, l);
	dy = SinL(-t*29, l);

	for(i=0; i<n; i++, pt++){
		x = pt->x;
		y = pt->y;

		Set2DPointC(tlv+0, (x+dx)>>8, (y+dy)>>8, u0, v0, c);
		Set2DPointC(tlv+1, (x+dy)>>8, (y-dx)>>8, u1, v0, c);
		Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v1, c);
		Set2DPointC(tlv+3, (x-dy)>>8, (y+dx)>>8, u0, v1, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// 雷部の描画・仮実装版 //
static IVOID DrawThSub(POINT *p1, POINT *p2, BYTE a)
{
	D3DVALUE		u0, u1, v0, v1;
	D3DTLVERTEX		tlv[32];
	int				i, n;
	int				x1, y1, x2, y2;
	int				dx, dy, t, l;
	DWORD			c;
	BYTE			d;
	POINT			*pt;

	// ここから下は、ホーミングレーザー頭の描画となる //
	u0  = D3DVAL(32) / D3DVAL(256);
	u1  = D3DVAL(32) / D3DVAL(256);
	v0  = D3DVAL(64) / D3DVAL(256);
	v1  = D3DVAL(128) / D3DVAL(256);

	c  = RGBA_MAKE(a, a, a, a);

	x1 = p1->x;
	y1 = p1->y;
	x2 = p2->x;
	y2 = p2->y;

	d  = Atan8(x2-x1, y2-y1) - 64;
	l  = 4*256;//(RndEx() % 10) * 256;
	dx = CosL(d, l);
	dy = SinL(d, l);

	Set2DPointC(tlv+0, (x1+dx)>>8, (y1+dy)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x2+dx)>>8, (y2+dy)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (x2-dx)>>8, (y2-dy)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (x1-dx)>>8, (y1-dy)>>8, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 雷の描画 //
FVOID CEnemyLSphere::DrawThunder(LSphere *pSp, BYTE a)
{
	int		i, n;
	POINT	*p;

	n = pSp->m_NumSphere;
	if(n <= 1) return;

	p = pSp->m_CurrentPos;
	for(i=0; i<n-1; i++, p++){
		DrawThSub(p, p+1, a);
	}

	DrawThSub(p, pSp->m_CurrentPos, a);
}
