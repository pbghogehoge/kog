/*
 *   CTriEffect.cpp   : 三角形エフェクト
 *
 */

#include "CTriEffect.h"
#include "SECtrl.h"

#include "RndCtrl.h"



// コンストラクタ //
CTriEffect::CTriEffect()
{
	Initialize();
}


// デストラクタ //
CTriEffect::~CTriEffect()
{
}


// 初期化を行う //
FVOID CTriEffect::Initialize(void)
{
	// リストを初期化する //
	DeleteAllData();
}


// ガードブレイク用エフェクトをセット //
FVOID CTriEffect::SetGuardBreak(int x256, int y256)
{
	TriEfcData		*pTri;	// 三角形エフェクトデータへのポインタ
	int				i;		// いわゆるループ用変数(記述の必要無し)
	int				v;		// 三角エフェクトの移動速度
	BYTE			d;		// 三角エフェクトの移動角度


	for(i=0; i<120; i++){
		pTri = InsertBack(TRIEFC_GBREAK);
		if(NULL == pTri) return;

		d = RndEx() >> 1;				// 発射角
		v = RndEx() % (256*3) + 256*2;	// 進行方向

		// 破片１個の初期化 //
		pTri->ox = x256;			// 現在のＸ座標
		pTri->oy = y256;			// 現在のＹ座標
		pTri->vx = CosL(d, v);		// 速度のＸ成分
		pTri->vy = SinL(d, v);		// 速度のＹ成分
		pTri->r  = 64*256;

		// 現在の回転角 //
		pTri->rx = RndEx() >> 1;		// Ｘ軸
		pTri->ry = RndEx() >> 1;		// Ｙ軸
		pTri->rz = RndEx() >> 1;		// Ｚ軸

		// 角速度 //
		pTri->dx = RndEx()%9 - 4;		// Ｘ軸
		pTri->dy = RndEx()%9 - 4;		// Ｙ軸
		pTri->dz = RndEx()%9 - 4;		// Ｚ軸

		// カウンタのゼロ初期化 //
		pTri->Count = 0;
	}

	PlaySE(SNDID_GBREAK, x256);
}


// ボス用ガードブレイクエフェクトをセット //
FVOID CTriEffect::SetBossGB(int x256, int y256)
{
	TriEfcData		*pTri;	// 三角形エフェクトデータへのポインタ
	int				i;		// いわゆるループ用変数(記述の必要無し)
	int				v;		// 三角エフェクトの移動速度
	BYTE			d;		// 三角エフェクトの移動角度


	for(i=0; i<180; i++){
		pTri = InsertBack(TRIEFC_BOSS_GBREAK);
		if(NULL == pTri) return;

		d = RndEx() >> 1;				// 発射角
		v = RndEx() % (256+128) + 64;	// 進行方向

		// 破片１個の初期化 //
		pTri->ox = x256;			// 現在のＸ座標
		pTri->oy = y256;			// 現在のＹ座標
		pTri->vx = CosL(d, v);		// 速度のＸ成分
		pTri->vy = SinL(d, v);		// 速度のＹ成分
		pTri->r  = 38*256;

		// 現在の回転角 //
		pTri->rx = RndEx() >> 1;		// Ｘ軸
		pTri->ry = RndEx() >> 1;		// Ｙ軸
		pTri->rz = RndEx() >> 1;		// Ｚ軸

		// 角速度 //
		pTri->dx = RndEx()%9 - 4;		// Ｘ軸
		pTri->dy = RndEx()%9 - 4;		// Ｙ軸
		pTri->dz = RndEx()%9 - 4;		// Ｚ軸

		// カウンタのゼロ初期化 //
		pTri->Count = 0;
	}

	PlaySE(SNDID_GBREAK, x256);
}


// １フレーム進める //
FVOID CTriEffect::Move(void)
{
	Iterator		it;
	TriEfcData		*pTri;
	int				Target[2] = {TRIEFC_GBREAK, TRIEFC_BOSS_GBREAK};
	int				i, current;

	// ボス用ガードブレイクでも、自機用ガードブレイクでも
	// 基本的な動作は同じである
	for(i=0; i<2; i++){
		current = Target[i];

		ThisForEachFront(current, it){
			pTri = it.GetPtr();

			pTri->Count++;

			pTri->r -= 128;
			if(pTri->r <= 0) ThisDelContinue(it);

			pTri->ox += pTri->vx;	// Ｘ座標の更新を行う
			pTri->oy += pTri->vy;	// Ｙ座標の更新を行う

			// 三角形の回転を行う //
			pTri->rx += pTri->dx;	// Ｘ軸回転
			pTri->ry += pTri->dy;	// Ｙ軸回転
			pTri->rz += pTri->dz;	// Ｚ軸回転
		}
	}
}


// 三角エフェクトを描画する //
FVOID CTriEffect::Draw(void)
{
	D3DTLVERTEX		tlv[3];
	DWORD			c[3];

	Iterator		it;
	TriEfcData		*pTri;
	int				i, x, y, r;
	BYTE			a;


	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);


	// 自機用ガードブレイクの描画を行う //
	ThisForEachFront(TRIEFC_GBREAK, it){
		pTri = it.GetPtr();

		a    = (BYTE)max(0, 128 - pTri->Count);
		c[0] = RGBA_MAKE(a, a, a, 0);
		c[1] = RGBA_MAKE(0, 0, a, 0);
		c[2] = RGBA_MAKE(0, 0, a/2, 0);

		for(i=0; i<3; i++){
			r = pTri->r >> 8;
			x = CosL(i * (256/3), r);
			y = SinL(i * (256/3), r);

			Set2DPointC(tlv+i, x, y, 0, 0, c[i]);
		}

		RotateTLV(tlv, 3, pTri->rx, pTri->ry, pTri->rz);

		for(i=0; i<3; i++){
			tlv[i].sx = D3DVAL(tlv[i].sx + D3DVAL(pTri->ox) / 256.0);
			tlv[i].sy = D3DVAL(tlv[i].sy + D3DVAL(pTri->oy) / 256.0);
		}

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, tlv, 3);
	}


	// ボス用ガードブレイク描画を行う //
	ThisForEachFront(TRIEFC_BOSS_GBREAK, it){
		pTri = it.GetPtr();

		a    = (BYTE)max(0, 128 - pTri->Count);
		c[0] = RGBA_MAKE(a, a, a, 0);
		c[1] = RGBA_MAKE(a/2, 0, a, 0);
		c[2] = RGBA_MAKE(a/4, 0, a/2, 0);

		for(i=0; i<3; i++){
			r = pTri->r >> 8;
			x = CosL(i * (256/3), r);
			y = SinL(i * (256/3), r);

			Set2DPointC(tlv+i, x, y, 0, 0, c[i]);
		}

		RotateTLV(tlv, 3, pTri->rx, pTri->ry, pTri->rz);

		for(i=0; i<3; i++){
			tlv[i].sx = D3DVAL(tlv[i].sx + D3DVAL(pTri->ox) / 256.0);
			tlv[i].sy = D3DVAL(tlv[i].sy + D3DVAL(pTri->oy) / 256.0);
		}

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, tlv, 3);
	}
}
