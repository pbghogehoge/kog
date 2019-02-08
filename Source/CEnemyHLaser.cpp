/*
 *   CEnemyHLaser.cpp   : ホーミングレーザー管理
 *
 */

#include "CEnemyHLaser.h"

#include "RndCtrl.h"


#define EHL_MAXSIZE			48
#define EHL_DELETETIME		60
#define HLASER_WIDTH		(9 * 256)
//#define HLASER_WIDTH		(8 * 256)

#define EHL_DAMAGE			(56789)		// 約 5％



// コンストラクタ //
CEnemyHLaser::CEnemyHLaser(
			RECT			*rcTargetX256	// 対象となる矩形
		  , int				*pX				// 自機のＸ座標へのポインタ
		  , int				*pY				// 自機のＹ座標へのポインタ
		  , CShaveEffect	*pShaveEfc)		// カスりエフェクト発動用クラス
{
	// 画面端の座標を格納する //
	m_XMin = rcTargetX256->left   - (EHL_MAXSIZE * 128);	// 左端の座標
	m_YMin = rcTargetX256->top    - (EHL_MAXSIZE * 128);	// 上端の座標
	m_XMax = rcTargetX256->right  + (EHL_MAXSIZE * 128);	// 右端の座標
	m_YMax = rcTargetX256->bottom + (EHL_MAXSIZE * 128);	// 下端の座標

	m_pShaveEfc = pShaveEfc;	// カスリエフェクト管理

	m_pX = pX;		// 現在のＸ座標へのポインタ
	m_pY = pY;		// 現在のＹ座標へのポインタ
}


// デストラクタ //
CEnemyHLaser::~CEnemyHLaser()
{
}


// レーザーをセットする(引数がポインタでないことに注意) //
FVOID CEnemyHLaser::Set(AttackCommand Command)
{
	HLaserData		*pLaser;
	HLaserInfo		*pInfo;
	int				i, j;
	int				ox, oy;
	int				Length, d, l;
	int				NumLaser, Target;
	int				dw;

	NumLaser = Command.Num;

	switch(Command.Type){
		case HLASER_RND:
			ox     = (m_XMin + m_XMax) / 2;		// 目標中心のＸ座標
			oy     = (m_YMin + m_YMax) / 2;		// 目標中心のＹ座標
			dw     = 64;
			Length = ox >> 8;					// 中心からの距離(非x256)
			Target = EHLASER_SEARCH;			// 挿入先
		break;

		case HLASER_SETX:
			ox     = Command.ox;		// 目標中心のＸ座標
			oy     = 800 * 256;			// 目標中心のＹ座標
			dw     = 8;
			Length = 1;					// 中心からの距離(非x256)
			Target = EHLASER_SEARCH;	// 挿入先(基本的にはSearchと同じ)
		break;

		case HLASER_PLAYER:
			ox     = *m_pX;				// 目標中心のＸ座標
			oy     = *m_pY;				// 目標中心のＹ座標
			dw     = 64;
			Length = 50;				// 中心からの距離(非x256)
			Target = EHLASER_PLAYER;	// 挿入先
		break;

		default:
#ifdef PBG_DEBUG
			PbgError("だめ");
#endif
		return;
	}

	// ここからが、本番ですぞ //
	for(i=0; i<NumLaser; i++){
		pLaser = InsertBack(Target);	// 挿入して、ポインタを取得
		if(NULL == pLaser) return;		// 空きがありません

		d = RndEx();
		l = (RndEx() % Length) << 8;

		pLaser->a          = Command.Accel;		// 加速度
		pLaser->v          = Command.Speed;		// 速度
		pLaser->v0         = Command.Speed;		// 初速度

		pLaser->TargetX    = ox + CosL(d, l);	// 目標のＸ座標
		pLaser->TargetY    = oy + SinL(d, l);	// 目標のＹ座標
		pLaser->count      = 0;					// カウンタ
		pLaser->queue_head = 0;					// キューの先頭
		pLaser->c          = Command.Color&3;	// レーザーの色

							// 節情報の格納 //
		pInfo = pLaser->HLInfo;
		d = 128+64 - (dw/2) + RndEx() % dw;
		for(j=0; j<EHLASER_LENGTH; j++, pInfo++){
			pInfo->x = Command.ox;		// 初期Ｘ座標
			pInfo->y = Command.oy;		// 初期Ｙ座標
			pInfo->d = d;//Command.Angle;	// 初期角度
		}
	}
}


// 初期化する
FVOID CEnemyHLaser::Initialize(void)
{
	// リスト内のデータを解放する //
	DeleteAllData();
}


// レーザー全てに消去エフェクトをセットする //
FVOID CEnemyHLaser::Clear(void)
{
	Iterator		it;

	// ホーミングレーザー：目標探索中 //
	ThisForEachFront(EHLASER_SEARCH, it){
		it->count = 0;
		it->a    *= 8;
		MoveBack(it, EHLASER_DELETE);
	}

	// ホーミングレーザー：自機探索中 //
	ThisForEachFront(EHLASER_PLAYER, it){
		it->count = 0;
		it->a    *= 8;
		MoveBack(it, EHLASER_DELETE);
	}

	// ホーミングレーザー：目標探索中 //
	ThisForEachFront(EHLASER_ACC, it){
		it->count = 0;
		it->a    *= 8;
		MoveBack(it, EHLASER_DELETE);
	}

	// ホーミングレーザー：目標探索中 //
	ThisForEachFront(EHLASER_FINISH, it){
		it->count = 0;
		it->a    *= 8;
		MoveBack(it, EHLASER_DELETE);
	}
}


// レーザーを移動させる //
FVOID CEnemyHLaser::Move(void)
{
	Iterator			it;
	HLaserData			*pLaser;
	HLaserInfo			*pInfo;
	int					oldx, oldy, oldd, ideg;
	int					tx, ty, delta;
	int					mx, my, dx, dy;
	int					XMin, YMin, XMax, YMax;
	BYTE				d;

	XMin = m_XMin;		// 左端の座標
	YMin = m_YMin;		// 上端の座標
	XMax = m_XMax;		// 右端の座標
	YMax = m_YMax;		// 下端の座標


	// ホーミングレーザー：消去中 //
	ThisForEachFront(EHLASER_FINISH, it){
		pLaser = it.GetPtr();
		pLaser->count++;

		pInfo = pLaser->HLInfo + pLaser->queue_head;
		oldx  = pInfo->x;
		oldy  = pInfo->y;
		oldd  = pInfo->d;

		pLaser->queue_head--;
		if(pLaser->queue_head < 0){
			pLaser->queue_head = EHLASER_LENGTH - 1;
		}

		pInfo = pLaser->HLInfo + pLaser->queue_head;

		if(pInfo->y > YMax){
			ThisDelContinue(it);
		}

		pLaser->v += pLaser->a;

		pInfo->d = oldd;
		pInfo->x = oldx + CosL(oldd, pLaser->v);
		pInfo->y = oldy + SinL(oldd, pLaser->v);
	}


	// ホーミングレーザー：加速中 //
	ThisForEachFront(EHLASER_ACC, it){
		pLaser = it.GetPtr();
		pLaser->count++;

		pInfo = pLaser->HLInfo + pLaser->queue_head;
		oldx  = pInfo->x;
		oldy  = pInfo->y;
		oldd  = pInfo->d;

		pLaser->queue_head--;
		if(pLaser->queue_head < 0){
			pLaser->queue_head = EHLASER_LENGTH - 1;
		}

		pInfo = pLaser->HLInfo + pLaser->queue_head;

		if(pLaser->v < pLaser->v0){
			pLaser->v += (pLaser->a << 2);
		}

		pInfo->d   = oldd;

		tx = pInfo->x = oldx + CosL(pInfo->d, pLaser->v);
		ty = pInfo->y = oldy + SinL(pInfo->d, pLaser->v);

		if(ty > YMax || ty < YMin || tx > XMax || tx < XMin){
			MoveBack(it, EHLASER_FINISH);
		}
	}


	// ホーミングレーザー：目標探索中 //
	ThisForEachFront(EHLASER_SEARCH, it){
		pLaser = it.GetPtr();
		pLaser->count++;

		pInfo = pLaser->HLInfo + pLaser->queue_head;
		oldx  = pInfo->x;
		oldy  = pInfo->y;
		oldd  = pInfo->d;

		pLaser->queue_head--;
		if(pLaser->queue_head < 0){
			pLaser->queue_head = EHLASER_LENGTH - 1;
		}

		pInfo = pLaser->HLInfo + pLaser->queue_head;

		dx = pLaser->TargetX - oldx;
		dy = pLaser->TargetY - oldy;
		if(abs(dx) < 16*256 && abs(dy) < 16*256){
			pInfo->d = oldd;
			MoveBack(it, EHLASER_ACC);
		}
		else{
			d    = Atan8(dx, dy);
			ideg = (int)d - (int)oldd;
			if(     ideg < -128) ideg += 256;
			else if(ideg >  128) ideg -= 256;

			if(pLaser->count >= 120){
				// あきらめ //
				pInfo->d = oldd;
				MoveBack(it, EHLASER_ACC);
			}
			else{
				if(abs(ideg) > 4){
					delta = 255 - (255 * abs(pLaser->v)) / abs(pLaser->v0);
					pInfo->d = oldd + ((ideg * delta) / 255);

					pLaser->v = max(256, (pLaser->v) - (pLaser->a));
				}
				else{
					pInfo->d = d;
					MoveBack(it, EHLASER_ACC);
				}
			}
		}

		pInfo->x = oldx + CosL(pInfo->d, pLaser->v);
		pInfo->y = oldy + SinL(pInfo->d, pLaser->v);
	}


	// ホーミングレーザー：自機探索中 //
	mx = *m_pX;		my = *m_pY;
	ThisForEachFront(EHLASER_PLAYER, it){
		pLaser = it.GetPtr();
		pLaser->count++;

		pInfo = pLaser->HLInfo + pLaser->queue_head;
		oldx  = pInfo->x;
		oldy  = pInfo->y;
		oldd  = pInfo->d;

		pLaser->queue_head--;
		if(pLaser->queue_head < 0){
			pLaser->queue_head = EHLASER_LENGTH - 1;
		}

		pInfo = pLaser->HLInfo + pLaser->queue_head;

		dx = mx - oldx;
		dy = my - oldy;
		if(abs(dx) < 16*256 && abs(dy) < 16*256){
			pInfo->d = oldd;
			MoveBack(it, EHLASER_ACC);
		}
		else{
			d  = Atan8(dx, dy);
			ideg = (int)d - (int)oldd;
			if(     ideg < -128) ideg += 256;
			else if(ideg >  128) ideg -= 256;

			if(pLaser->count >= 120){
				// あきらめ //
				pInfo->d = oldd;
				MoveBack(it, EHLASER_ACC);
			}
			else{
				if(abs(ideg) > 3){
					delta = 255 - (255 * abs(pLaser->v)) / abs(pLaser->v0);
					pInfo->d = oldd + ((ideg * delta) / 255);

					pLaser->v = max(256, (pLaser->v) - (pLaser->a));
				}
				else{
					pInfo->d = d;
					MoveBack(it, EHLASER_ACC);
				}
			}
		}

		pInfo->x = oldx + CosL(pInfo->d, pLaser->v);
		pInfo->y = oldy + SinL(pInfo->d, pLaser->v);
	}


	// ホーミングレーザー：消去エフェクト中 //
	ThisForEachFront(EHLASER_DELETE, it){
		pLaser = it.GetPtr();
		pLaser->count++;
		if(pLaser->count > EHL_DELETETIME){
			ThisDelContinue(it);
		}

		pInfo = pLaser->HLInfo + pLaser->queue_head;
		oldx  = pInfo->x;
		oldy  = pInfo->y;
		oldd  = pInfo->d;

		pLaser->queue_head--;
		if(pLaser->queue_head < 0){
			pLaser->queue_head = EHLASER_LENGTH - 1;
		}

		pInfo = pLaser->HLInfo + pLaser->queue_head;

		pLaser->v += pLaser->a;
		pInfo->d   = oldd;
		pInfo->x   = oldx + CosL(oldd, pLaser->v);
		pInfo->y   = oldy + SinL(oldd, pLaser->v);
	}
}


// レーザーを描画する //
FVOID CEnemyHLaser::Draw(void)
{
	Iterator			it;

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_HLASER);

	// ホーミングレーザー：目標探索中 //
	ThisForEachFront(EHLASER_SEARCH, it){
		DrawNormalHLaser(it.GetPtr());
	}

	// ホーミングレーザー：自機探索中 //
	ThisForEachFront(EHLASER_PLAYER, it){
		DrawNormalHLaser(it.GetPtr());
	}

	// ホーミングレーザー：加速中 //
	ThisForEachFront(EHLASER_ACC, it){
		DrawNormalHLaser(it.GetPtr());
	}

	// ホーミングレーザー：画面外へ //
	ThisForEachFront(EHLASER_FINISH, it){
		DrawNormalHLaser(it.GetPtr());
	}

	// ホーミングレーザー：消去エフェクト中 //
	ThisForEachFront(EHLASER_DELETE, it){
		DrawDeleteHLaser(it.GetPtr());
	}
}


// 当たり判定を行う               //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemyHLaser::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	Iterator		it;

	// ホーミングレーザー：目標探索中 //
	ThisForEachFront(EHLASER_SEARCH, it){
		HitCheckHLaser(it.GetPtr(), pShave, pDamage);
	}

	// ホーミングレーザー：自機探索中 //
	ThisForEachFront(EHLASER_SEARCH, it){
		HitCheckHLaser(it.GetPtr(), pShave, pDamage);
	}

	// ホーミングレーザー：加速中 //
	ThisForEachFront(EHLASER_ACC, it){
		HitCheckHLaser(it.GetPtr(), pShave, pDamage);
	}

	// ホーミングレーザー：画面外へ //
	ThisForEachFront(EHLASER_FINISH, it){
		HitCheckHLaser(it.GetPtr(), pShave, pDamage);
	}
}


// 難易度 Easy 用にセット //
IVOID CEnemyHLaser::EasyCommand(AttackCommand *pCmd)
{
	// まだ実装されていないのです //
}


// 難易度 Hard 用にセット //
IVOID CEnemyHLaser::HardCommand(AttackCommand *pCmd)
{
	// まだ実装されていないのです //
}


// 難易度 Luna 用にセット //
IVOID CEnemyHLaser::LunaCommand(AttackCommand *pCmd)
{
	// まだ実装されていないのです //
}


// 通常状態の描画 //
FVOID CEnemyHLaser::DrawNormalHLaser(HLaserData *pLaser)
{
	D3DVALUE		u0, u1, v0, v1;
	D3DTLVERTEX		tlv[EHLASER_LENGTH * 2];
	HLaserInfo		*pInfo;
	int				i, n, x, y, dx, dy, t, l, t2;
	int				tsx, tw;
	DWORD			c, col;

	c     = RGBA_MAKE(200, 200, 200, 255);
	t     = pLaser->count;
	n     = pLaser->queue_head;

	// ホーミングレーザー尻尾の描画 //
	col = pLaser->c * 64;
	u0  = D3DVAL(col+ 0) / D3DVAL(256);
	u1  = D3DVAL(col+64) / D3DVAL(256);
	v0  = D3DVAL(    64) / D3DVAL(256);
	v1  = D3DVAL(   128) / D3DVAL(256);

	t2  = min(pLaser->count/2+1, EHLASER_LENGTH);
	tw  = (20 * t2) / EHLASER_LENGTH;
	tsx = col + 32 + 20 - tw;

	for(i=0; i<EHLASER_LENGTH*2; i+=2){
		pInfo = pLaser->HLInfo + n;

		// レーザーの太さを変化させるためにテクスチャの座標を変化させる //
		l  = (tw * i) / (EHLASER_LENGTH*2 - 2);
		u1 = D3DVAL(tsx + l) / D3DVAL(256);

//		l  = (20 * i) / (EHLASER_LENGTH*2 - 2);
//		u1 = D3DVAL(col + 32 + l) / D3DVAL(256);

		x    = pInfo->x;
		y    = pInfo->y;

		dx   = CosL(pInfo->d + 64, HLASER_WIDTH);
		dy   = SinL(pInfo->d + 64, HLASER_WIDTH);
//		t    = 255 - (255 * i) / (HLASER_LENGTH * 2);
//		c    = RGBA_MAKE(255, 196, t, 255);

		Set2DPointC(tlv+i+0, (x-dx)>>8, (y-dy)>>8, u1, v0, c);
		Set2DPointC(tlv+i+1, (x+dx)>>8, (y+dy)>>8, u1, v1, c);

		n = n + 1;
		if(n >= EHLASER_LENGTH) n = 0;
	}
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLESTRIP, tlv, EHLASER_LENGTH * 2);


	// ここから下は、ホーミングレーザー頭の描画となる //
	u0  = D3DVAL(col+ 0) / D3DVAL(256);
	u1  = D3DVAL(col+64) / D3DVAL(256);
	v0  = D3DVAL(     0) / D3DVAL(256);
	v1  = D3DVAL(    64) / D3DVAL(256);

	pInfo = pLaser->HLInfo + pLaser->queue_head;

	x = pInfo->x;
	y = pInfo->y;

	l = SinL(t*12, 7*256) + 22*256;

	dx = CosL(pInfo->d, l);
	dy = SinL(pInfo->d, l);
	c  = RGBA_MAKE(255, 255, 255, 255);

	Set2DPointC(tlv+0, (x+dx)>>8, (y+dy)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+dy)>>8, (y-dx)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (x-dy)>>8, (y+dx)>>8, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);




/*
/////////////////////////////////////////////////////////////////////
	n = pLaser->queue_head - 1;
	if(n < 0) n = 0;
	pInfo = pLaser->HLInfo + n;
//	n = pLaser->queue_head + 1;
//	if(n >= EHLASER_LENGTH) n = 0;

	x = pInfo->x;
	y = pInfo->y;

	l = (SinL(t*12, 6*256) + 20*256) * t / EHL_DELETETIME;

	dx = CosL(pInfo->d, l);
	dy = SinL(pInfo->d, l);

	Set2DPointC(tlv+0, (x+dx)>>8, (y+dy)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+dy)>>8, (y-dx)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (x-dy)>>8, (y+dx)>>8, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
*/
}


// 消去状態の描画 //
FVOID CEnemyHLaser::DrawDeleteHLaser(HLaserData *pLaser)
{
	D3DVALUE		u0, u1, v0, v1;
	D3DTLVERTEX		tlv[EHLASER_LENGTH * 2];
	HLaserInfo		*pInfo;
	int				i, n, x, y, dx, dy, t, l;
	DWORD			c, col;
	int				w;
	BYTE			d;

	t = EHL_DELETETIME - pLaser->count;
	d = (255 * t) / EHL_DELETETIME;
	c = RGBA_MAKE(d, d, d, d);
	n = pLaser->queue_head;

	// ホーミングレーザー尻尾の描画 //
	col = pLaser->c * 64;
	u0  = D3DVAL(col+ 0) / D3DVAL(256);
	u1  = D3DVAL(col+64) / D3DVAL(256);
	v0  = D3DVAL(    64) / D3DVAL(256);
	v1  = D3DVAL(   128) / D3DVAL(256);

	for(i=0; i<EHLASER_LENGTH*2; i+=2){
		pInfo = pLaser->HLInfo + n;

		// レーザーの太さを変化させるためにテクスチャの座標を変化させる //
		l  = (20 * i) / (EHLASER_LENGTH*2 - 2);
//		l  = (32 * i) / (EHLASER_LENGTH*2 - 2);
		u1 = D3DVAL(col + 32 + l) / D3DVAL(256);

		x    = pInfo->x;
		y    = pInfo->y;

		w    = (HLASER_WIDTH) * t / EHL_DELETETIME;// + (HLASER_WIDTH/3);
		dx   = CosL(pInfo->d + 64, w);
		dy   = SinL(pInfo->d + 64, w);
//		t    = 255 - (255 * i) / (HLASER_LENGTH * 2);
//		c    = RGBA_MAKE(255, 196, t, 255);

		Set2DPointC(tlv+i+0, (x-dx)>>8, (y-dy)>>8, u1, v0, c);
		Set2DPointC(tlv+i+1, (x+dx)>>8, (y+dy)>>8, u1, v1, c);

		n = n + 1;
		if(n >= EHLASER_LENGTH) n = 0;
	}
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLESTRIP, tlv, EHLASER_LENGTH * 2);


	// ここから下は、ホーミングレーザー頭の描画 //
	u0  = D3DVAL(col+ 0) / D3DVAL(256);
	u1  = D3DVAL(col+64) / D3DVAL(256);
	v0  = D3DVAL(     0) / D3DVAL(256);
	v1  = D3DVAL(    64) / D3DVAL(256);

	pInfo = pLaser->HLInfo + pLaser->queue_head;

	x = pInfo->x;
	y = pInfo->y;

	l = (SinL(t*12, 7*256) + 22*256) * t / EHL_DELETETIME;

	dx = CosL(pInfo->d, l);
	dy = SinL(pInfo->d, l);

	Set2DPointC(tlv+0, (x+dx)>>8, (y+dy)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+dy)>>8, (y-dx)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (x-dy)>>8, (y+dx)>>8, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 当たり判定 //
FVOID CEnemyHLaser::HitCheckHLaser(
						HLaserData	*pLaser		// 対象となるレーザー
					,	DWORD		*pShave		// カスリ回数
					,	DWORD		*pDamage)	// ダメージ
{
	int				mx, my;
	int				tx, ty;
	int				ox, oy;
	int				i, n, w, l, aw;
	BYTE			d;
	HLaserInfo		*pInfo;
	CShaveEffect	*pEfc;

	int				hitSize;
	int				shaveSize;

	mx = *m_pX;
	my = *m_pY;

	pEfc  = m_pShaveEfc;

	n = pLaser->queue_head;

	// 最後のデータは除く //
	for(i=0; i<EHLASER_LENGTH-1; i++){
		pInfo = pLaser->HLInfo + n;

		ox = pInfo->x;
		oy = pInfo->y;

		n = n + 1;
		if(n >= EHLASER_LENGTH) n = 0;
		pInfo = pLaser->HLInfo + n;

		tx  = -pInfo->x;
		ty  = -pInfo->y;
		ox += tx;
		oy += ty;
		tx += mx;
		ty += my;

		d  = pInfo->d;

		// レーザーと自機との距離を求める //
		l  =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
		w  = -SinL(d, tx) + CosL(d, ty);	// 幅方向
		aw = abs(w);

		shaveSize = ((HLASER_WIDTH + 24*256) * (EHLASER_LENGTH - i)) / EHLASER_LENGTH;
		if(aw <= shaveSize && l > 0 && l < Isqrt(ox*ox + oy*oy)){
			pEfc->Set((mx+pInfo->x)>>1, (my+pInfo->y)>>1, Atan8(tx-mx, ty-my));
			(*pShave) += 1;

			hitSize = (HLASER_WIDTH * (EHLASER_LENGTH - i)) / EHLASER_LENGTH;
			if(aw <= hitSize) (*pDamage) += EHL_DAMAGE;

			return;
		}
	}

/*
		if(HitCheckFast(my, tx, shaveSize)
		&& HitCheckFast(mx, ty, shaveSize)){
			(*pShave)++;
			pEfc->Set(tx, ty, Atan8(tx-mx, ty-my));

			if(HitCheckFast(my, tx, hitSize)
			&& HitCheckFast(mx, ty, hitSize)){
				(*pDamage) += EHL_DAMAGE;
			}

			return;
		}
	}
*/
}
