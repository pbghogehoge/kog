/*
 *   CFragmentEfc.cpp   : 破片処理
 *
 */

#include "CFragmentEfc.h"

#include "RndCtrl.h"

#define SMOKE_TIME		(30-5)



// コンストラクタ //
CFragmentEfc::CFragmentEfc(RECT *rcTargetX256)
{
	m_XMin = rcTargetX256->left;	// 左端の座標
	m_YMin = rcTargetX256->top;		// 上端の座標
	m_XMax = rcTargetX256->right;	// 右端の座標
	m_YMax = rcTargetX256->bottom;	// 下端の座標

	Initialize();
}


// デストラクタ //
CFragmentEfc::~CFragmentEfc()
{
}


// 初期化を行う //
FVOID CFragmentEfc::Initialize(void)
{
	// リストを初期化する //
	DeleteAllData();
}


// 指定座標に点破片をセットする //
FVOID CFragmentEfc::SetPointEfc(int x256, int y256, int n)
{
	FragmentData	*pEfc;
	int				i;
	int				v;
	BYTE			d;

	// 指定された数だけ発生させましょう //
	for(i=0; i<n; i++){
		pEfc = InsertBack(FRG_POINT);
		if(NULL == pEfc) return;

		d = 128 + 32 + (RndEx() % 64);	// 発射角
		v = RndEx() % (256*3) + 256*2;	// 速度

		// 破片１個の初期化 //
		pEfc->x     = x256;			// 現在のＸ座標
		pEfc->y     = y256;			// 現在のＹ座標
		pEfc->vx    = CosL(d, v);	// 速度のＸ成分
		pEfc->vy    = SinL(d, v);	// 速度のＹ成分
		pEfc->Count = 0;
	}
}


// 指定座標に煙を発生させる //
FVOID CFragmentEfc::SetSmokeEfc(int x256, int y256, BYTE Target)
{
	Iterator		it;
	FragmentData	*pEfc;
	int				v;
	BYTE			d;

#ifdef PBG_DEBUG
	switch(Target){
		case FRG_SMOKE_W:	case FRG_SMOKE_P:
		break;

		default:
			PbgError("SetSmokeEfc() : 未定義の煙番号");
		return;
	}
#endif

	pEfc = InsertBack(Target);
	if(NULL == pEfc){
//		GetFrontIterator(it, FRG_POINT);
//		if(it == NULL){
			GetFrontIterator(it, Target);
			if(it == NULL) return;
//		}

		pEfc = MoveBack(it, Target);
//		pEfc = it.GetPtr();
	}

	d = (BYTE)RndEx();	// 発射角
	v = RndEx() % (96);	// 速度

	pEfc->x     = x256;			// 現在のＸ座標
	pEfc->y     = y256;			// 現在のＹ座標
	pEfc->vx    = CosL(d, v);	// 速度のＸ成分
	pEfc->vy    = SinL(d, v);	// 速度のＹ成分
	pEfc->Count = SMOKE_TIME;	// 煙の寿命
}


// 破片を動かす //
FVOID CFragmentEfc::Move(void)
{
	Iterator		it;
	FragmentData	*pEfc;

	// 点エフェクト //
	ThisForEachFront(FRG_POINT, it){
		pEfc = it.GetPtr();

		// カウンタをインクリメントして、指定フレーム数が //
		// 経過した場合は消去を行って、continue する      //
		pEfc->Count++;
		if(pEfc->Count >= 64) ThisDelContinue(it);

		// 座標を更新する //
		pEfc->x  += pEfc->vx;	// Ｘ座標の更新を行う
		pEfc->y  += pEfc->vy;	// Ｙ座標の更新を行う
		pEfc->vy += 32;			// Ｙ方向にＧの影響を
	}

	// 煙エフェクト //
	ThisForEachFront(FRG_SMOKE_W, it){
		pEfc = it.GetPtr();

		if(pEfc->Count){
			pEfc->x += pEfc->vx;
			pEfc->y += pEfc->vy;
			pEfc->Count--;
		}
		else{
			ThisDelContinue(it);
		}
	}

	// 煙エフェクト //
	ThisForEachFront(FRG_SMOKE_P, it){
		pEfc = it.GetPtr();

		if(pEfc->Count){
			pEfc->x += pEfc->vx;
			pEfc->y += pEfc->vy;
			pEfc->Count--;
		}
		else{
			ThisDelContinue(it);
		}
	}
}


// 破片を描画する //
FVOID CFragmentEfc::Draw(void)
{
	DrawSmokeW();
	DrawSmokeP();
	DrawPoint();
}


// 点エフェクトを描画する //
FVOID CFragmentEfc::DrawPoint(void)
{
	D3DTLVERTEX			tlv[FRAGMENT_MAX], *ptlv;
	int					x, y, n;
	DWORD				c;
	Iterator			it;
	FragmentData		*pEfc;

	// ポインタをバッファの先頭にセットする //
	ptlv = tlv;

	// バッファにデータを一括挿入 //
	ThisForEachFront(FRG_POINT, it){
		// イテレータからポインタに変換 //
		pEfc = it.GetPtr();

		x = (pEfc->x) >> 8;				// Ｘ座標
		y = (pEfc->y) >> 8;				// Ｙ座標
		c = 255 - (pEfc->Count * 4);	// 色の調整値

		Set2DPointC(ptlv, x, y, 0, 0, RGBA_MAKE(255-c/3, c/2, c/4, 255));

		ptlv++;
	}

	// 描画する点の数を求める //
	n = ptlv - tlv;

	// レンダリングステートをセットして、描画開始！！ //
	if(n){
		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);		// 通常の半透明
		g_pGrp->SetTexture(GRPTEXTURE_MAX);				// テクスチャ無し
		g_pGrp->DrawPrimitive(D3DPT_POINTLIST, tlv, n);	// 点描画モードで
	}
}


// 煙エフェクトを描画する(白) //
FVOID CFragmentEfc::DrawSmokeW()
{
	D3DTLVERTEX			tlv[20];
	D3DVALUE			u1, v1, u2, v2;
	int					x, y;
	int					dx;
	DWORD				c;
	Iterator			it;
	FragmentData		*pEfc;

	u1 = D3DVAL(  8) / D3DVAL(256);
	v1 = D3DVAL(128) / D3DVAL(256);
	u2 = D3DVAL( 24) / D3DVAL(256);
	v2 = D3DVAL(144) / D3DVAL(256);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);		// 通常の半透明
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);			// テクスチャ無し

	// バッファにデータを一括挿入 //
	ThisForEachFront(FRG_SMOKE_W, it){
		pEfc = it.GetPtr();

		x = (pEfc->x) >> 8;						// Ｘ座標
		y = (pEfc->y) >> 8;						// Ｙ座標
		c = (80 * pEfc->Count) / SMOKE_TIME;	// 色の調整値
//		c = RGBA_MAKE(c, c, c+100, 255);
		c = RGBA_MAKE(c, c, c, 255);
		dx = 2 + ((SMOKE_TIME - pEfc->Count) / 3);

		Set2DPointC(tlv+0, x-dx, y-dx, u1, v1, c);
		Set2DPointC(tlv+1, x+dx, y-dx, u2, v1, c);
		Set2DPointC(tlv+2, x+dx, y+dx, u2, v2, c);
		Set2DPointC(tlv+3, x-dx, y+dx, u1, v2, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// 煙エフェクトを描画する(ピンク) //
FVOID CFragmentEfc::DrawSmokeP()
{
	D3DTLVERTEX			tlv[20];
	D3DVALUE			u1, v1, u2, v2;
	int					x, y;
	int					dx;
	DWORD				c;
	Iterator			it;
	FragmentData		*pEfc;

	u1 = D3DVAL(  8) / D3DVAL(256);
	v1 = D3DVAL(128) / D3DVAL(256);
	u2 = D3DVAL( 24) / D3DVAL(256);
	v2 = D3DVAL(144) / D3DVAL(256);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);		// 通常の半透明
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);			// テクスチャ無し

	// バッファにデータを一括挿入 //
	ThisForEachFront(FRG_SMOKE_P, it){
		// イテレータからポインタに変換 //
		pEfc = it.GetPtr();

		x = (pEfc->x) >> 8;						// Ｘ座標
		y = (pEfc->y) >> 8;						// Ｙ座標
		c = (80 * pEfc->Count) / SMOKE_TIME;	// 色の調整値
//		c = RGBA_MAKE(c, c, c+100, 255);
		c = RGBA_MAKE(c*3, c, c*2, 255);
		dx = 6 + ((SMOKE_TIME - pEfc->Count) / 3);

		Set2DPointC(tlv+0, x-dx, y-dx, u1, v1, c);
		Set2DPointC(tlv+1, x+dx, y-dx, u2, v1, c);
		Set2DPointC(tlv+2, x+dx, y+dx, u2, v2, c);
		Set2DPointC(tlv+3, x-dx, y+dx, u1, v2, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
