/*
 *   CEnemyLLaser.cpp   : 敵用太レーザー
 *
 */

#include "CEnemyLLaser.h"

#define ELL_SHAVE		(16*256)		// カスり判定の許容幅
#define ELL_DAMAGE		(123456 / 2)	// 約５％




// コンストラクタ //
CEnemyLLaser
	::CEnemyLLaser(int			*pX				// 自機のＸ座標へのポインタ
				 , int			*pY				// 自機のＹ座標へのポインタ
				 , CShaveEffect	*pShaveEfc)		// カスりエフェクト発動用クラス
{
	// ポインタの接続を行う //
	m_pX        = pX;			// 当たり判定Ｘ座標
	m_pY        = pY;			// 当たり判定Ｙ座標
	m_pShaveEfc = pShaveEfc;	// カスりエフェクト管理クラス
}


// デストラクタ //
CEnemyLLaser::~CEnemyLLaser()
{
	// 何もしません //
}


// 開き状態に移行する //
FVOID CEnemyLLaser::OnOpen(EnemyAtkCtrl *pAtkCtrl, int w)
{
	LLaserData		*pLaser;

	pLaser = Connect(pAtkCtrl);
	if(NULL == pLaser) return;

	pLaser->m_TargetWidth = w;
	pLaser->m_State       = LLST_RESIZE;
}


// 閉じ状態に移行する //
FVOID CEnemyLLaser::OnClose(EnemyAtkCtrl *pAtkCtrl)
{
	AttackCommand		*pCmd;
	LLaserData			*pLaser;

	pLaser = Connect(pAtkCtrl);
	if(NULL == pLaser) return;

	pLaser->m_TargetWidth = 0;		// 目標とする幅は、もちろん０
	pLaser->m_State = LLST_CLOSE;	// ここではまだ消去しない

	pCmd = &(pLaser->m_pTarget->m_Cmd);

	pLaser->m_x     = pCmd->ox;		// 発射元Ｘ座標
	pLaser->m_y     = pCmd->oy;		// 発射元Ｙ座標
	pLaser->m_Angle = pCmd->Angle;	// 発射角
	pLaser->m_Speed = pCmd->Speed;	// レーザー展開
	pLaser->m_Count = 0;			// カウンタの初期化

	// 攻撃オブジェクトとの関係を切り離す //
	pLaser->m_pTarget->DisableLLaserPtr();
	pLaser->m_pTarget = NULL;
}


// 溜め状態に移行する //
FVOID CEnemyLLaser::OnCharge(EnemyAtkCtrl *pAtkCtrl)
{
	LLaserData			*pLaser;

	// 相互の接続を確立する //
	pLaser = Connect(pAtkCtrl);
	if(NULL == pLaser) return;

	pLaser->m_TargetWidth = 0;		// 目標とする幅は、もちろん０
	pLaser->m_Count       = 0;		// ちょっとだけ重要ね


	// すでにクローズ中の場合は、溜め状態に直接移行する //
	// そうでなければ、溜め状態移行用のクローズとする   //
	if(0 == pLaser->m_Width) pLaser->m_State = LLST_CHARGE;
	else                     pLaser->m_State = LLST_CLOSEL;
}


// 初期化する //
FVOID CEnemyLLaser::Initialize(void)
{
	Iterator		it;
	LLaserData		*pLaser;

	// 攻撃オブジェクトとの接続を強制的に切り離す //
	ThisForEachFront(ELLASER_NORMAL, it){
		pLaser = it.GetPtr();

		pLaser->m_pTarget->DisableLLaserPtr();
		pLaser->m_pTarget = NULL;
	}

	// リスト内のデータを解放する //
	DeleteAllData();
}


// 太レーザー全てに消去エフェクトをセットする //
FVOID CEnemyLLaser::Clear(void)
{
	Iterator			it;

	// 攻撃オブジェクトとの接続を切り離し＆クローズ要求 //
	ThisForEachFront(ELLASER_NORMAL, it){
		OnClose(it.GetPtr()->m_pTarget);
	}
}


// 太レーザーを移動させる //
FVOID CEnemyLLaser::Move(void)
{
	LLaserData			*pLaser;
	AttackCommand		*pCmd;
	Iterator			it;
	int					w, wmax, v;

	// 通常状態のレーザー //
	ThisForEachFront(ELLASER_NORMAL, it){
		pLaser = it.GetPtr();

		if(pLaser->m_pTarget){
			pCmd = &(pLaser->m_pTarget->m_Cmd);
			pLaser->m_x     = pCmd->ox;		// Ｘ座標更新
			pLaser->m_y     = pCmd->oy;		// Ｙ座標更新
			pLaser->m_Angle = pCmd->Angle;	// 基本角度
			pLaser->m_Color = pCmd->Color;	// レーザーの色
		}

		switch(pLaser->m_State){
		case LLST_NONE:		// 通常の待ち
			// 特別に行う処理はありません //
		break;

		case LLST_RESIZE:	// サイズ変更中
			w    = pLaser->m_Width;				// 現在の幅
			wmax = pLaser->m_TargetWidth;		// 目標幅
			v    = max(32, abs(pCmd->Speed));	// 展開速度

			// 現在の状態よりも太くする場合 //
			if(w < wmax){
				w = w + v;		// 細くして

				if(w >= wmax){	// 展開完了
					pLaser->m_Width = wmax;
					pLaser->m_State = LLST_NONE;
				}
				else{
					pLaser->m_Width = w;
				}
			}
			// 現在の状態よりも細くする場合 //
			else{
				w = w - v;		// 太くして

				if(w <= wmax){	// 展開完了
					pLaser->m_Width = wmax;
					pLaser->m_State = LLST_NONE;
				}
				else{
					pLaser->m_Width = w;
				}
			}
		break;

		case LLST_CLOSEL:	// クローズ(溜め状態に移行中)
			v = max(64, abs(pCmd->Speed));
			pLaser->m_Width -= v;
			if(pLaser->m_Width <= 0){
				pLaser->m_Width = 0;
				pLaser->m_State = LLST_CHARGE;
			}
		break;

		case LLST_CHARGE:	// 溜め状態
			pLaser->m_Count++;
		break;

		case LLST_CLOSE:	// 消去用リストに移動要求
			MoveBack(it, ELLASER_DELETE);
		continue;
		}
	}

	// クローズ状態のレーザー //
	ThisForEachFront(ELLASER_DELETE, it){
		pLaser = it.GetPtr();

		// レーザーを細くする //
		v = max(32, abs(pLaser->m_Speed)/2);
		pLaser->m_Width -= v;

		if(pLaser->m_Width <= 0){	// ０以下になったら
			ThisDelContinue(it);	// 削除する
		}

		pLaser->m_Count++;
	}
}


// 太レーザーを描画する //
FVOID CEnemyLLaser::Draw(void)
{
	Iterator		it;
	LLaserData		*pLaser;

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);

	// 通常状態のレーザー //
	ThisForEachFront(ELLASER_NORMAL, it){
		pLaser = it.GetPtr();

		switch(pLaser->m_State){
			case LLST_NONE:		// 通常の待ち
			case LLST_RESIZE:	// サイズ変更中
			case LLST_CLOSEL:	// クローズ(溜め状態に移行中)
			case LLST_CLOSE:	// 消去用リストに移動要求
				DrawNormal(pLaser);	// 通常の描画です
			break;

			case LLST_CHARGE:	// 溜め状態
				DrawCharge(pLaser);	// 溜め状態の描画です
			break;

			default:
			continue;			// 何も描画しません
		}
	}

	// クローズ状態のレーザー //
	ThisForEachFront(ELLASER_DELETE, it){
		DrawNormal(it.GetPtr());		// 通常の描画です
	}
}


// 当たり判定を行う //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemyLLaser::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	Iterator		it;
	LLaserData		*pLaser;
	CShaveEffect	*pEfc;
	int				mx, my;
	int				tx, ty;
	int				width, length;
	int				awidth;
	BYTE			d;

	// 初期化は無しです(2001/09/14) //
	// ゼロ初期化する //
//	(*pShave)  = 0;		// かすった回数
//	(*pDamage) = 0;		// ダメージ量

	// 自機の座標の参照を高速化する //
	mx = (*m_pX);	// 自機のＸ座標
	my = (*m_pY);	// 自機のＹ座標

	// カスりエフェクト管理クラスに接続する //
	pEfc = m_pShaveEfc;

	// 通常状態のレーザーに対してだけ当たり判定を発行する //
	ThisForEachFront(ELLASER_NORMAL, it){
		pLaser = it.GetPtr();	// レーザー構造体へのポインタを取得する

		switch(pLaser->m_State){
			case LLST_NONE:		// 通常の待ち
			case LLST_RESIZE:	// サイズ変更中
				// この場合は、必ず当たり判定を行うべし //
			break;

			case LLST_CLOSEL:	// クローズ(溜め状態に移行中)
			case LLST_CHARGE:	// 溜め状態
			case LLST_CLOSE:	// 消去用リストに移動要求
			continue;	// 当たり判定の対象ではありません
		}

		tx = mx - pLaser->m_x;	// ベクトルのＸ成分
		ty = my - pLaser->m_y;	// ベクトルのＹ成分
		d  = pLaser->m_Angle;	// レーザーの進行方向

		// レーザーと自機との距離を求める //
		length =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
		width  = -SinL(d, tx) + CosL(d, ty);	// 幅方向
		awidth = abs(width);					// 幅の絶対値

		// カスり判定を行う //
		if((awidth < (pLaser->m_Width + ELL_SHAVE)) && (length > 0)){
			// 当たり判定を行う //
			// 太レーザーは削り系なので、消去しない //
			if(awidth < pLaser->m_Width){
				(*pDamage) += ELL_DAMAGE;
			}

			if(width > 0) d += 64;
			else          d -= 64;

			awidth >>= 1;
			tx = mx - CosL(d, awidth);
			ty = my - SinL(d, awidth);

			pEfc->Set(tx, ty, d);
			(*pShave)++;
		}
	}
}


// ＳＬ用当たり判定 //
// IN  : x y  当たり判定の対象座標
//     : dsl  ＳＬの進行方向
//     : c    反射するレーザーの色(上位６ビットをビットマスクとする)
// RET : == d ならば反射していない
//     : != d ならば反射している
FBYTE CEnemyLLaser::HitCheckEx(int x, int y, BYTE dsl, BYTE c)
{
	Iterator		it;
	LLaserData		*pLaser;
	int				tx, ty;
	int				width, length;
	int				awidth;
	BYTE			d;

	// 通常状態のレーザーに対してだけ当たり判定を発行する //
	ThisForEachFront(ELLASER_NORMAL, it){
		pLaser = it.GetPtr();	// レーザー構造体へのポインタを取得する

		switch(pLaser->m_State){
			case LLST_NONE:		// 通常の待ち
			case LLST_RESIZE:	// サイズ変更中
				// この場合は、必ず当たり判定を行う //
			break;

			default:
			continue;	// 当たり判定の対象ではありません
		}

		// 色が違う場合 //
		if(0 == ((c & pLaser->m_Color) & (~3))) continue;

		tx = x - pLaser->m_x;	// ベクトルのＸ成分
		ty = y - pLaser->m_y;	// ベクトルのＹ成分
		d  = pLaser->m_Angle;	// レーザーの進行方向

		// ＬＬとＳＬとの距離を求める //
		length =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
		width  = -SinL(d, tx) + CosL(d, ty);	// 幅方向
		awidth = abs(width);					// 幅の絶対値

		// 当たり判定を行う //
		if((awidth < pLaser->m_Width) && (length > 0)){
			// - (lp->d) + ((ll->d)<<1)
			return -dsl + ((pLaser->m_Angle) << 1);
		}
	}

	// 反射しない場合 //
	return dsl;
}


// 接続が出来ていればそのポインタを返し、出来ていなければ //
// 接続し、そのポインタを返す。失敗時は NULL を返す       //
LLaserData *CEnemyLLaser::Connect(EnemyAtkCtrl *pAtkCtrl)
{
	LLaserData		*pLaser;
//	AttackCommand	*pCmd;

	// OnClose() に対応させるために必要！ //
	if(NULL == pAtkCtrl) return NULL;

	// すでに接続が確立出来ているかを調べる //
	pLaser = pAtkCtrl->GetLLaserPtr();		// ポインタ取得
	if(pLaser) return pLaser;				// すでに接続済みの場合

	pLaser = InsertBack(ELLASER_NORMAL);	// 挿入＆ポインタ取得
	if(NULL == pLaser) return NULL;			// 空き領域が存在せず

/*
	pCmd = &(pAtkCtrl->m_Cmd);		// ポインタを取得

	pLaser->m_x     = pCmd->ox;		// 発射元Ｘ座標
	pLaser->m_y     = pCmd->oy;		// 発射元Ｙ座標
	pLaser->m_Angle = pCmd->Angle;	// 発射角
*/

	pLaser->m_Width = 0;			// 初期幅は０
	pLaser->m_Count = 0;			// カウンタを初期化
	pLaser->m_State = LLST_CHARGE;	// 溜め状態へと

	// 攻撃オブジェクトとの接続を確立する //
	pAtkCtrl->ConnectLLaser(pLaser);

	return pLaser;
}


// 通常状態の描画 //
FVOID CEnemyLLaser::DrawNormal(LLaserData *pLaser)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				x1, y1, x2, y2, dx, dy;
	DWORD			c, lc;

	// 頂点色は念のため、真白で //
	c  = RGBA_MAKE(255, 255, 255, 255);

	// 基本座標をセットする //
	x1 = pLaser->m_x;	// 始点のＸ座標
	y1 = pLaser->m_y;	// 始点のＹ座標
	x2 = x1 + CosL(pLaser->m_Angle, (800) * 256);	// 終点のＸ座標
	y2 = y1 + SinL(pLaser->m_Angle, (800) * 256);	// 終点のＹ座標

	// 差分をセットする //
	dx = CosL(pLaser->m_Angle - 64, pLaser->m_Width);	// 幅方向のベクトルのＸ成分
	dy = SinL(pLaser->m_Angle - 64, pLaser->m_Width);	// 幅方向のベクトルのＹ成分

	// テクスチャ座標をセットする //
	lc = (pLaser->m_Color & 3) * 40;
	u1 = D3DVAL(208+8) / D3DVAL(256);
	u2 = D3DVAL(240-8) / D3DVAL(256);
	v1 = D3DVAL(lc+  8) / D3DVAL(256);
	v2 = D3DVAL(lc+ 40) / D3DVAL(256);

	// 頂点をセット //
	Set2DPointC(tlv+0, (x1+dx)>>8, (y1+dy)>>8, u1, v1, c);
	Set2DPointC(tlv+1, (x2+dx)>>8, (y2+dy)>>8, u2, v1, c);
	Set2DPointC(tlv+2, (x2-dx)>>8, (y2-dy)>>8, u2, v2, c);
	Set2DPointC(tlv+3, (x1-dx)>>8, (y1-dy)>>8, u1, v2, c);

	// 書き込み //
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// 差分をセットする //
	dx = pLaser->m_Width * 3 / 2;

	// テクスチャ座標をセットする //
	lc = (pLaser->m_Color & 3) * (64+8);
	u1 = D3DVAL(   136) / D3DVAL(256);
	v1 = D3DVAL(lc+  8) / D3DVAL(256);
	u2 = D3DVAL(   200) / D3DVAL(256);
	v2 = D3DVAL(lc+ 72) / D3DVAL(256);

	// いつものように、頂点をセット //
	Set2DPointC(tlv+0, (x1-dx)>>8, (y1-dx)>>8, u1, v1, c);
	Set2DPointC(tlv+1, (x1+dx)>>8, (y1-dx)>>8, u2, v1, c);
	Set2DPointC(tlv+2, (x1+dx)>>8, (y1+dx)>>8, u2, v2, c);
	Set2DPointC(tlv+3, (x1-dx)>>8, (y1+dx)>>8, u1, v2, c);

	// まぁるい部分の描画 //
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 溜め状態の描画 //
FVOID CEnemyLLaser::DrawCharge(LLaserData *pLaser)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				x1, y1, x2, y2, w;
	DWORD			c, lc;

	// 頂点色は念のため、真白で //
	c  = RGBA_MAKE(255, 255, 255, 255);

	// 基本座標をセットする //
	x1 = pLaser->m_x;	// 始点のＸ座標
	y1 = pLaser->m_y;	// 始点のＹ座標
	x2 = x1 + CosL(pLaser->m_Angle, (640+480) * 256);	// 終点のＸ座標
	y2 = y1 + SinL(pLaser->m_Angle, (640+480) * 256);	// 終点のＹ座標

	// テクスチャ座標をセットする //
	lc = (pLaser->m_Color & 3) * 40;
	u1 = D3DVAL(   208) / D3DVAL(256);
	v1 = D3DVAL(lc+  8) / D3DVAL(256);
	u2 = D3DVAL(   240) / D3DVAL(256);
	v2 = D3DVAL(lc+ 40) / D3DVAL(256);

	// 頂点をセット //
	Set2DPointC(tlv+0, x1>>8, y1>>8, u1, (v1+v2)/2, c);
	Set2DPointC(tlv+1, x2>>8, y2>>8, u2, (v1+v2)/2, c);

	// 書き込み //
	g_pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, 2);

	// 差分をセットする //
	w = SinL((BYTE)pLaser->m_Count*3, 10*256);

	// テクスチャ座標をセットする //
	lc = (pLaser->m_Color & 3) * (64+8);
	u1 = D3DVAL(   136) / D3DVAL(256);
	v1 = D3DVAL(lc+  8) / D3DVAL(256);
	u2 = D3DVAL(   200) / D3DVAL(256);
	v2 = D3DVAL(lc+ 72) / D3DVAL(256);

	// 頂点をセット //
	Set2DPointC(tlv+0, (x1-w)>>8, (y1-w)>>8, u1, v1, c);
	Set2DPointC(tlv+1, (x1+w)>>8, (y1-w)>>8, u2, v1, c);
	Set2DPointC(tlv+2, (x1+w)>>8, (y1+w)>>8, u2, v2, c);
	Set2DPointC(tlv+3, (x1-w)>>8, (y1+w)>>8, u1, v2, c);

	// まぁるい部分の描画 //
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
