/*
 *   CEnemyExShot.cpp   : 敵用特殊攻撃
 *
 */

#include "CEnemyExShot.h"

#include "RndCtrl.h"


#define ERL_MAXDAMAGE	(123456/2)	// 結構痛い
#define ECROSS_DAMAGE	(212345)	// 十字架１個のダメージ量
#define EFLOWER_DAMAGE	(123456)	// ちょいとばかし痛い
#define GFIRE_MAXDAMAGE	(123456/8)	// かなり痛い
#define IONRING_DAMAGE	(123456*2)	// イオンリングのダメージ

#define ECROSS_HITSIZE	(8*256)		// 十字架先端の当たり判定サイズ
#define ECROSS_RADIUS	40			// 十字架の落下範囲/256


// コンストラクタ //
CEnemyExShot::CEnemyExShot(
			RECT			*rcTargetX256	// 対象矩形
		  , int				*pX				// 当たり判定Ｘ座標へのポインタ
		  , int				*pY				// 当たり判定Ｙ座標へのポインタ
		  , CShaveEffect	*pShaveEfc)		// カスりエフェクト発動用クラス
{
	// 反射座標 //
	m_XMin = rcTargetX256->left;	// 左端の反射座標
	m_YMin = rcTargetX256->top;		// 上端の反射座標
	m_XMax = rcTargetX256->right;	// 右端の反射座標
	m_YMax = rcTargetX256->bottom;	// 下端の反射座標

	m_Width = m_XMax - m_XMin;		// 横幅

	// ポインタの接続 //
	m_pX        = pX;			// 当たり判定Ｘ座標
	m_pY        = pY;			// 当たり判定Ｙ座標
	m_pShaveEfc = pShaveEfc;	// カスりエフェクト管理

#ifdef PBG_DEBUG
	// タスクのサイズをチェック //
	if(sizeof(EExShotTask) < sizeof(RLaserData)){
		PbgError("RLaserData タスクのサイズが EExShotTask よりも大きいぞぃ");
	}

	// タスクのサイズをチェック //
	if(sizeof(EExShotTask) < sizeof(CrossData)){
		PbgError("CrossData タスクのサイズが EExShotTask よりも大きいぞぃ");
	}

	// タスクのサイズをチェック //
	if(sizeof(EExShotTask) < sizeof(GFireData)){
		PbgError("GFireData タスクのサイズが EExShotTask よりも大きいぞぃ");
	}

	////////////////////////////////////////////////////////////////////////////
	// タスクを追加する場合は、この部分に上のようなコードを追加してくださいな //
	////////////////////////////////////////////////////////////////////////////
#endif
}


// デストラクタ //
CEnemyExShot::~CEnemyExShot()
{
}


// --- ラウンドレーザーをセットする ---
// IN : .ox .oy ( ラウンドレーザーの中心位置 )
//    : .Length ( 目標とする半径 )
//    : .Speed  ( 展開速度 )
FVOID CEnemyExShot::SetRLaser(AttackCommand Command)
{
	RLaserData		*p;

	p = (RLaserData *)InsertBack(EEXTSK_RLASER_LOCK);
	if(NULL == p) return;

	p->m_X            = Command.ox;		// 中心となるＸ座標
	p->m_Y            = Command.oy;		// 中心となるＹ座標
	p->m_TargetRadius = Command.Length;	// 目標とする半径
	p->m_Speed        = Command.Speed;	// 展開速度
	p->m_Count        = 0;				// カウンタ
	p->m_Radius       = Command.Length;	// 現在の半径(For Effect)
}


// --- 十字架をセットする ---
// IN : .Num    ( 十字架の数 )
//    : .Speed  ( 初速度 )
//    : .Accel  ( 加速度 )
FVOID CEnemyExShot::SetCross(AttackCommand Command)
{
	CrossData		*p;
	int				i;
	int				l, rl;
	int				mx, my, ty;
	BYTE			d, rd;

	mx = Command.ox;
	my = Command.oy;
//	mx = *m_pX;			// 目標とする基本Ｘ座標
//	my = *m_pY;			// 目標とする基本Ｙ座標
	l  = 540 * 256;		// 初期位置から目標までの距離

	// 十字架発生位置に警告を発動させる //
	p = (CrossData *)InsertBack(EEXTSK_CROSS_DANGER);
	if(NULL == p) return;
	p->m_X     = mx;
	p->m_Y     = my;
	p->m_Count = 255;
	p->m_Angle = (BYTE)RndEx();

	// 十字架をセットする //
	for(i=0; i<Command.Num; i++, l+=(30*256)){
		p = (CrossData *)InsertBack(EEXTSK_CROSS);
		if(NULL == p) return;

		d  = (BYTE)(-64 - 8 + (RndEx() % 16));	// 落下角度+128
		rd = (BYTE)(RndEx()&0xff);
		rl = (RndEx() % (ECROSS_RADIUS * 2 / 3)) << 8;

		// 初期座標を決定する //
		// (m_X, m_Y) = 現在の自機の位置から rl だけ離れた位置に
		//              落下するための初期位置
		p->m_X       = mx + CosL(rd, rl) + CosL(d, l);
		ty           = my + SinL(rd, rl);
		p->m_TargetY = ty;				// 目標とするＹ座標
		p->m_Y       = ty + SinL(d, l);	// 現在位置

		// その他の諸パラメータをセットする //
		p->m_Speed = Command.Speed;	// 落下速度
		p->m_Accel = Command.Accel;	// 加速度
		p->m_Angle = d + 128;		// 落下角度
		p->m_Count = 0;				// カウンタ
	}
}


// --- 花をセットする --
// IN : .ox .oy (初期座標)
//    : .Angle  (進行方向)
//    : .Length (花の大きさ)
//    : .Speed  (移動速度:遅いほうが強力！)
//    : .Color  (花の色)
FVOID CEnemyExShot::SetFlower(AttackCommand Command)
{
	FlowerData		*p;
//	int				delta;
	BYTE			d;

//PbgError("SetFlower Called");

	p = (FlowerData *)InsertBack(EEXTSK_FLOWER);
	if(NULL == p) return;

	// 花に関する諸パラメータをセットする //
//	delta  = m_Width >> 8;					// 画面横幅
//	p->m_X = m_XMin + ((RndEx() % delta)<<8);	// 初期Ｘ座標
//	p->m_Y = m_YMin - Command.Length;		// 初期Ｙ座標
//	d = 64 - 5 + RndEx() % 11;
	p->m_X = Command.ox;
	p->m_Y = Command.oy;
	d      = Command.Angle;

	p->m_vx = CosL(d, Command.Speed);	// 花の移動速度(x)
	p->m_vy = SinL(d, Command.Speed);	// 花の移動速度(y)

	p->m_Angle  = (BYTE)RndEx();	// 花の傾き
	p->m_Radius = Command.Length;	// 花の大きさ
	p->m_Color  = Command.Color;	// 花の色
	p->m_Count  = 0;				// カウンタ

	p->m_Omega  = -2 + ((RndEx()&1)<<2);	// 角速度
}


// --- ゲイツ・なにがしをセットする --
// IN : .ox .oy (初期座標)
//    : .Angle  (発射方向)
//    : .Accel  (速度減衰)
//    : .Speed  (移動速度:遅いほうが強力！)
FVOID CEnemyExShot::SetGFire(AttackCommand Command)
{
	GFireData		*p;
	int				i, j;
	int				v;//vx, vy, v;

	v = Command.Speed;

	for(j=0; j<8; j++){
		for(i=0; i<5; i++){
			p = (GFireData *)InsertBack(EEXTSK_GFIRE_INIT);
			if(NULL == p) return;

			p->m_X     = Command.ox;	// 初期Ｘ座標
			p->m_Y     = Command.oy;	// 初期Ｙ座標
			p->m_v         = v;
			p->m_a         = Command.Accel;
			p->m_BaseAngle = Command.Angle;
			p->m_Count = j * 2;				// 汎用カウンタ
			p->m_Size  = 0;

			p->m_Alpha = 16 + RndEx() % 96;					// α値
			p->m_Angle = Command.Angle - 16 + RndEx() % 33;	// 進行方向
		}
	}
}


// --- イオンリングをセットする ---
// IN : .ox .oy (初期座標)
//    : .Angle  (発射方向)
//    : .Speed  (移動速度)
//    : .Length (大きさ)
FVOID CEnemyExShot::SetIonRing(AttackCommand Command)
{
	IonRingData		*p;

	p = (IonRingData *)InsertBack(EEXTSK_IONRING);
	if(NULL == p) return;

	p->x     = Command.ox;
	p->y     = Command.oy;
	p->vx    = CosL(Command.Angle, Command.Speed);
	p->vy    = SinL(Command.Angle, Command.Speed);
	p->size  = Command.Length;
	p->Count = 0;
}


// 初期化する //
FVOID CEnemyExShot::Initialize(void)
{
	// リスト内のデータを解放する //
	DeleteAllData();
}


// 特殊ショットに消去エフェクトをセットする //
FVOID CEnemyExShot::Clear(void)
{
	OnClearRLaser();
	OnClearCross();
	OnClearFlower();
	OnClearGFire();
	OnClearIonRing();
}


// 特殊ショットを移動させる
FVOID CEnemyExShot::Move(void)
{
	Iterator		it;


//-------------------------------------------------------------------
//    以下、ラウンドレーザーに関する処理
//-------------------------------------------------------------------
	// ラウンドレーザー・ロック中 //
	ThisForEachFront(EEXTSK_RLASER_LOCK, it){
		if(FALSE == MoveLockRLaser((RLaserData *)it.GetPtr())){
			MoveBack(it, EEXTSK_RLASER);
		}
	}

	// ラウンドレーザー・通常状態 //
	ThisForEachFront(EEXTSK_RLASER, it){
		if(FALSE == MoveRLaser((RLaserData *)it.GetPtr())){
			MoveBack(it, EEXTSK_RLASER_DELETE);
		}
	}

	// ラウンドレーザー・消去中 //
	ThisForEachFront(EEXTSK_RLASER_DELETE, it){
		if(FALSE == MoveDelRLaser((RLaserData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}


//-------------------------------------------------------------------
//    以下、十字架に関する処理
//-------------------------------------------------------------------
	// 十字架・落下中 //
	ThisForEachFront(EEXTSK_CROSS, it){
		if(FALSE == MoveCross((CrossData *)it.GetPtr())){
			MoveBack(it, EEXTSK_CROSS_DELETE);
		}
	}

	// 十字架・消去中 //
	ThisForEachFront(EEXTSK_CROSS_DELETE, it){
		if(FALSE == MoveDelCross((CrossData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}

	// 十字架・警告表示 //
	ThisForEachFront(EEXTSK_CROSS_DANGER, it){
		if(FALSE == MoveCrossDanger((CrossData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}


//-------------------------------------------------------------------
//    以下、花に関する処理
//-------------------------------------------------------------------
	// 花・落下中 //
	ThisForEachFront(EEXTSK_FLOWER, it){
		// このタスクが死亡するのは、画面外に出た場合である     //
		// したがって、チェンジタスクではなく、タスク殺しとなる //
		if(FALSE == MoveFlower((FlowerData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}

	// 花・消去中 //
	ThisForEachFront(EEXTSK_FLOWER_DELETE, it){
		if(FALSE == MoveDelFlower((FlowerData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}


//-------------------------------------------------------------------
//    以下、ゲイツなにがしに関する処理
//-------------------------------------------------------------------
	// 火炎放射・通常動作 //
	ThisForEachFront(EEXTSK_GFIRE, it){
		if(FALSE == MoveGFire((GFireData *)it.GetPtr())){
			MoveBack(it, EEXTSK_GFIRE_DELETE);
		}
	}

	// 火炎放射・消去中 //
	ThisForEachFront(EEXTSK_GFIRE_DELETE, it){
		if(FALSE == MoveDelGFire((GFireData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}

	// 火炎放射・準備中 //
	ThisForEachFront(EEXTSK_GFIRE_INIT, it){
		if(FALSE == MoveInitGFire((GFireData *)it.GetPtr())){
			MoveBack(it, EEXTSK_GFIRE);
		}
	}


//-------------------------------------------------------------------
//    以下、イオンリングに関する処理
//-------------------------------------------------------------------
	// イオンリング・通常状態 //
	ThisForEachFront(EEXTSK_IONRING, it){
		if(FALSE == MoveIonRing((IonRingData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}

	// イオンリング・消去中 //
	ThisForEachFront(EEXTSK_IONRING_DELETE, it){
		if(FALSE == MoveDelIonRing((IonRingData *)it.GetPtr())){
			ThisDelContinue(it);
		}
	}
}


// 特殊ショットを描画する //
FVOID CEnemyExShot::Draw(void)
{
	Iterator		it;

//-------------------------------------------------------------------
//    以下、ラウンドレーザーに関する処理
//-------------------------------------------------------------------
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);

	// ラウンドレーザー・ロック中 //
	ThisForEachFront(EEXTSK_RLASER_LOCK, it){
		DrawLockRLaser((RLaserData *)it.GetPtr());
	}

	// テクスチャをセット //
	g_pGrp->SetTexture(TEXTURE_ID_EXTAMA);

	// ラウンドレーザー・通常状態 //
	ThisForEachFront(EEXTSK_RLASER, it){
		DrawDelRLaser((RLaserData *)it.GetPtr());
	}

	// ラウンドレーザー・消去中 //
	ThisForEachFront(EEXTSK_RLASER_DELETE, it){
		DrawDelRLaser((RLaserData *)it.GetPtr());
	}


//-------------------------------------------------------------------
//    以下、十字架に関する処理
//-------------------------------------------------------------------
	// 十字架・警告表示 //
	ThisForEachFront(EEXTSK_CROSS_DANGER, it){
		DrawCrossDanger((CrossData *)it.GetPtr());
	}

	// 十字架・落下中 //
	ThisForEachFront(EEXTSK_CROSS, it){
		DrawCross((CrossData *)it.GetPtr());
	}

	// 十字架・消去中 //
	ThisForEachFront(EEXTSK_CROSS_DELETE, it){
		DrawDelCross((CrossData *)it.GetPtr());
	}


//-------------------------------------------------------------------
//    以下、花に関する処理
//-------------------------------------------------------------------
	// 花・落下中 //
	ThisForEachFront(EEXTSK_FLOWER, it){
		DrawFlower((FlowerData *)it.GetPtr(), 255);
	}

	// 花・消去中 //
	ThisForEachFront(EEXTSK_FLOWER_DELETE, it){
		FlowerData *pFl = (FlowerData *)it.GetPtr();

		DrawFlower(pFl, (BYTE)(pFl->m_Count));
	}


//-------------------------------------------------------------------
//    以下、ゲイツなにがしに関する処理
//-------------------------------------------------------------------
//	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_HLASER);

	// ゲイツなにがし・通常状態 //
	ThisForEachFront(EEXTSK_GFIRE, it){
		DrawGFire((GFireData *)it.GetPtr());
	}

	// ゲイツなにがし・消去中 //
	ThisForEachFront(EEXTSK_GFIRE_DELETE, it){
		DrawGFire((GFireData *)it.GetPtr());
	}


//-------------------------------------------------------------------
//    以下、イオンリングに関する処理
//-------------------------------------------------------------------
	// イオンリング・通常状態 //
	ThisForEachFront(EEXTSK_IONRING, it){
		DrawIonRing((IonRingData *)it.GetPtr());
	}

	// イオンリング・消去中 //
	ThisForEachFront(EEXTSK_IONRING_DELETE, it){
		DrawDelIonRing((IonRingData *)it.GetPtr());
	}
}


// 当たり判定を行う               //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemyExShot::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	Iterator		it;
	int				mx, my;
	int				tx, ty;
	int				size, size2;
	int				len;
	int				rad;
	RLaserData		*pr;
	CrossData		*pc;
	FlowerData		*pf;
	GFireData		*pg;
	IonRingData		*pi;

	mx = (*m_pX);		// 自機のＸ座標
	my = (*m_pY);		// 自機のＹ座標

//-------------------------------------------------------------------
//    以下、ラウンドレーザーに関する処理
//-------------------------------------------------------------------
	ThisForEachFront(EEXTSK_RLASER, it){
		pr = (RLaserData *)it.GetPtr();

		if(0 == pr->m_Count) continue;

		rad = pr->m_Radius;
//		if(rad <= 16*256) continue;

		tx  = pr->m_X;
		ty  = pr->m_Y;

		// まずは、バウンディングボックスとの当たり判定を行う //
		if(HitCheckFast(my, ty, rad) && HitCheckFast(mx, tx, rad)){
			// 次に円との当たり判定を行う //
			size  = (rad *  4) >> 6;				// 円の正確な半径を求める
			rad   = (rad * 44) >> 6;				// 半径からの許容範囲
//			size2 = (rad * 48) >> 6;

//			size  = size * size;
//			size2 = size2 * size2;
//			rad = rad * rad;					// rad = 半径^2

			tx -= mx;	len  = tx * tx;			// len = dx^2
			ty -= my;	len += ty * ty;			// len = dx^2 + dy^2

			len = abs(Isqrt(len) - rad);

			// 円の内側にある場合 //
			if(len < size){
				(*pDamage) += (ERL_MAXDAMAGE * min(255, pr->m_Count)) / 255;
//				break;			// 同時にヒットするのは１個だけ
			}
		}
	}


//-------------------------------------------------------------------
//    以下、十字架に関する処理
//-------------------------------------------------------------------
	ThisForEachFront(EEXTSK_CROSS, it){
		pc = (CrossData *)it.GetPtr();

		tx  = pc->m_X;
		ty  = pc->m_Y;

		if(HitCheckFast(my, ty, ECROSS_HITSIZE)
		&& HitCheckFast(mx, tx, ECROSS_HITSIZE)){
			(*pDamage) += ECROSS_DAMAGE;
			MoveBack(it, EEXTSK_CROSS_DELETE);
		}
	}


//-------------------------------------------------------------------
//    以下、花に関する処理
//-------------------------------------------------------------------
	ThisForEachFront(EEXTSK_FLOWER, it){
		pf = (FlowerData *)it.GetPtr();

		tx    = abs(pf->m_X - mx);
		ty    = abs(pf->m_Y - my);
		size  = (pf->m_Radius) / 3;
		size2 = (size * 3) >> 1;

		// 八角形の内側にあるかね？ //
		if((tx < size) && (ty < size) && (tx+ty < size2)){
			(*pDamage) += EFLOWER_DAMAGE;
		}
	}


//-------------------------------------------------------------------
//    以下、ゲイツなにがしに関する処理
//-------------------------------------------------------------------
	// 火炎放射・通常動作 //
	ThisForEachFront(EEXTSK_GFIRE, it){
		pg = (GFireData *)it.GetPtr();

		tx    = abs(pg->m_X - mx);
		ty    = abs(pg->m_Y - my);
		size  = (pg->m_Size) / 3;
		size2 = (size * 3) >> 1;

		// 八角形の内側にあるかね？ //
		if((tx < size) && (ty < size) && (tx+ty < size2)){
			(*pDamage) += ((GFIRE_MAXDAMAGE * pg->m_Count) / 255);
		}
	}

	// 火炎放射・消去中 //
	ThisForEachFront(EEXTSK_GFIRE_DELETE, it){
		pg = (GFireData *)it.GetPtr();

		tx    = abs(pg->m_X - mx);
		ty    = abs(pg->m_Y - my);
		size  = (pg->m_Size) / 3;
		size2 = (size * 3) >> 1;

		// 八角形の内側にあるかね？ //
		if((tx < size) && (ty < size) && (tx+ty < size2)){
			(*pDamage) += ((GFIRE_MAXDAMAGE * pg->m_Count) / 255);
		}
	}


//-------------------------------------------------------------------
//    以下、イオンリングに関する処理
//-------------------------------------------------------------------
	ThisForEachFront(EEXTSK_IONRING, it){
		pi = (IonRingData *)it.GetPtr();

		tx    = abs(pi->x - mx);
		ty    = abs(pi->y - my);
		size  = (pi->size) / 3;
		size2 = (size * 3) >> 1;

		// 八角形の内側にあるかね？ //
		if((tx < size) && (ty < size) && (tx+ty < size2)){
			(*pDamage) += IONRING_DAMAGE;

			pi->Count = 255;
			MoveBack(it, EEXTSK_IONRING_DELETE);
		}
	}
}


// 破壊可能な敵弾に対して DamageCallback を呼び出す //
FVOID CEnemyExShot::EnumBreakableExShot(DamageCallback Callback, void *pParam)
{
	Iterator		it;
	IonRingData		*pi;

	// イオンリング //
	ThisForEachFront(EEXTSK_IONRING, it){
		pi = (IonRingData *)it.GetPtr();
		if(Callback(pParam, (EnemyData *)pi)){
			pi->Count = 255;
			MoveBack(it, EEXTSK_IONRING_DELETE);
		}
	}
}


// ラウンドレーザーに対する消去要求 //
FVOID CEnemyExShot::OnClearRLaser(void)
{
	RLaserData		*p;
	Iterator		it;

	// ラウンドレーザー・ロック中 //
	ThisForEachFront(EEXTSK_RLASER_LOCK, it){
		ThisDelContinue(it);		// 強制的に殺す
	}

	// ラウンドレーザー・通常状態 //
	ThisForEachFront(EEXTSK_RLASER, it){
		p = (RLaserData *)it.GetPtr();
		p->m_Count = 255;		// これはα値を示す
		p->m_Speed >>= 1;		// 速度減衰
	}

	// 通常状態のラウンドレーザーをすべて消去リストに移動 //
	MoveBackAll(EEXTSK_RLASER_DELETE, EEXTSK_RLASER);
}


// 十字架に対する消去要求 //
FVOID CEnemyExShot::OnClearCross(void)
{
	CrossData		*p;
	Iterator		it;

	// 十字架・落下中 //
	ThisForEachFront(EEXTSK_CROSS, it){
		p = (CrossData *)it.GetPtr();
		p->m_Count = 255;
	}
	// 落下中の十字架をすべて消去リストに移動 //
	MoveBackAll(EEXTSK_CROSS_DELETE, EEXTSK_CROSS);
}


// 花に対する消去要求 //
FVOID CEnemyExShot::OnClearFlower(void)
{
	FlowerData		*p;
	Iterator		it;

	// 花・落下中 //
	ThisForEachFront(EEXTSK_FLOWER, it){
		p = (FlowerData *)it.GetPtr();
		p->m_Count = 255;
		p->m_Omega <<= 1;
	}
	// 落下中の花をすべて消去リストに移動 //
	MoveBackAll(EEXTSK_FLOWER_DELETE, EEXTSK_FLOWER);
}


// ゲイツなにがし //
FVOID CEnemyExShot::OnClearGFire(void)
{
	Iterator		it;

	// 移動するだけでＯＫ //
	MoveBackAll(EEXTSK_GFIRE_DELETE, EEXTSK_GFIRE);

	ThisForEachFront(EEXTSK_GFIRE_INIT, it){
		ThisDelContinue(it);		// 強制的に殺す
	}
}


// イオンリング //
FVOID CEnemyExShot::OnClearIonRing(void)
{
	Iterator		it;
	IonRingData		*pI;

	ThisForEachFront(EEXTSK_IONRING, it){
		pI        = (IonRingData *)it.GetPtr();
		pI->Count = 255;
	}

	MoveBackAll(EEXTSK_IONRING_DELETE, EEXTSK_IONRING);
}


// ロック中ラウンドレーザーを動作させる //
FBOOL CEnemyExShot::MoveLockRLaser(RLaserData *pRL)
{
	pRL->m_Radius -= pRL->m_Speed;	// 速度を更新
	pRL->m_Count++;					// カウンタをインクリメント

	if(pRL->m_Radius <= 0){
		pRL->m_Radius = 0;		// 半径は０とする
		pRL->m_Count  = 0;
		return FALSE;			// 移動要求
	}

	return TRUE;
}


// ラウンドレーザーを動作させる //
FBOOL CEnemyExShot::MoveRLaser(RLaserData *pRL)
{
	pRL->m_Count += 16;		// カウンタをインクリメント

	// 最終半径に達している場合 //
	if(pRL->m_Radius > pRL->m_TargetRadius){
		if(pRL->m_Count > 256+32*32){
			pRL->m_Count = 255;		// これはα値を示す
			pRL->m_Speed >>= 1;		// 速度減衰
			return FALSE;			// 移動要求
		}
	}
	else{
		if(pRL->m_Count > 128) pRL->m_Count = 128;
		pRL->m_Radius += pRL->m_Speed;		// 半径を更新
	}

	return TRUE;
}


// 消去中ラウンドレーザーを動作させる //
FBOOL CEnemyExShot::MoveDelRLaser(RLaserData *pRL)
{
	pRL->m_Radius += pRL->m_Speed;	// 速度を更新

	if(pRL->m_Count >= 16){
		pRL->m_Count -= 16;
		return TRUE;
	}

	return FALSE;	// 消去要求
}


// 落下中の十字架を動作させる //
FBOOL CEnemyExShot::MoveCross(CrossData *pCr)
{
	// 座標を更新する //
	pCr->m_X += CosL(pCr->m_Angle, pCr->m_Speed);
	pCr->m_Y += SinL(pCr->m_Angle, pCr->m_Speed);

	// 消去の時は訪れた //
	if(pCr->m_Y >= pCr->m_TargetY){
		pCr->m_Count = 255;
		return FALSE;
	}

	pCr->m_Speed += pCr->m_Accel;	// 加速
	pCr->m_Count++;					// カウンタ更新

	return TRUE;
}


// 消去中の十字架を動作させる //
FBOOL CEnemyExShot::MoveDelCross(CrossData *pCr)
{
	if(pCr->m_Count >= 16){
		pCr->m_Count -= 16;

		// 座標を更新する //
		if(pCr->m_Y < pCr->m_TargetY){
			pCr->m_X += CosL(pCr->m_Angle, pCr->m_Speed);
			pCr->m_Y += SinL(pCr->m_Angle, pCr->m_Speed);
		}

		return TRUE;
	}

	return FALSE;	// 消去要求
}


// 十字架落下位置警告を動作させる //
FBOOL CEnemyExShot::MoveCrossDanger(CrossData *pCr)
{
	if(pCr->m_Count >= 3){
		pCr->m_Count -= 3;
		pCr->m_Angle++;
		return TRUE;
	}

	return FALSE;	// 消去要求
}


// 落下中の花を動作させる //
FBOOL CEnemyExShot::MoveFlower(FlowerData *pFl)
{
	// 死ね //
	if(pFl->m_Count > 60){	// 少なくとも１秒間の間、死ぬことは無い筈 //
		if(pFl->m_Y - pFl->m_Radius > m_YMax) return FALSE;
		if(pFl->m_Y + pFl->m_Radius < m_YMin) return FALSE;
		if(pFl->m_X - pFl->m_Radius > m_XMax) return FALSE;
		if(pFl->m_X + pFl->m_Radius < m_XMin) return FALSE;
	}

	// 座標を更新する //
	pFl->m_X += pFl->m_vx;
	pFl->m_Y += pFl->m_vy;

	pFl->m_Angle += pFl->m_Omega;	// 回転
	pFl->m_Count++;					// カウンタ更新

	return TRUE;
}


// 消去の花を動作させる //
FBOOL CEnemyExShot::MoveDelFlower(FlowerData *pFl)
{
	if(pFl->m_Count >= 4){
		pFl->m_Count -= 4;
		pFl->m_Angle += pFl->m_Omega;
		pFl->m_Radius = max(0, pFl->m_Radius - 256*2);
		return TRUE;
	}

	return FALSE;		// 消去要求
}


// 動作中の炎 //
FBOOL CEnemyExShot::MoveGFire(GFireData *pF)
{
	int			v, vx, vy;
	BYTE		d;

	d  = pF->m_BaseAngle;
	v  = pF->m_v;

	pF->m_v += pF->m_a;

	vx = CosL(d, v);
	vy = SinL(d, v);

	d = pF->m_Angle;
	v = 4 * pF->m_Alpha;

	pF->m_X += (vx + CosL(d, v));
	pF->m_Y += (vy + SinL(d, v));

	if(pF->m_Alpha) pF->m_Alpha--;

	pF->m_Count += 8;

	if(pF->m_Count > 256){
		pF->m_Size = (8 + ((24 * 255) >> 8)) << 8;
		return FALSE;
	}

	pF->m_Size = (8 + ((24 * pF->m_Count) >> 8)) << 8;

	return TRUE;
}


// 消去中の炎 //
FBOOL CEnemyExShot::MoveDelGFire(GFireData *pF)
{
	int		v, vx, vy;
	BYTE	d;

	if(pF->m_Count <= 8) return FALSE;

	d  = pF->m_BaseAngle;
	v  = pF->m_v;

	pF->m_v += pF->m_a;

	vx = CosL(d, v);
	vy = SinL(d, v);

	d = pF->m_Angle;
	v = 3 * pF->m_Alpha;

	pF->m_X += (vx - CosL(d, v));
	pF->m_Y += (vy - SinL(d, v));

	pF->m_Count -= 8;
	pF->m_Size = (8 + ((24 * pF->m_Count) >> 8)) << 8;

	return TRUE;
}


// 準備中の炎 //
FBOOL CEnemyExShot::MoveInitGFire(GFireData *pF)
{
	if(pF->m_Count){
		pF->m_Count--;
		return TRUE;
	}

	return FALSE;
}


// 動作中のイオンリング //
FBOOL CEnemyExShot::MoveIonRing(IonRingData *pI)
{
	pI->x += pI->vx;
	pI->y += pI->vy;

	pI->Count++;

	if(pI->y - pI->size > m_YMax) return FALSE;
	if(pI->y + pI->size < m_YMin) return FALSE;
	if(pI->x - pI->size > m_XMax) return FALSE;
	if(pI->x + pI->size < m_XMin) return FALSE;

	return TRUE;
}


// 消去中のイオンリング //
FBOOL CEnemyExShot::MoveDelIonRing(IonRingData *pI)
{
	if(pI->Count <= 24) return FALSE;

	pI->Count -= 24;
	pI->size  += 256;

	return TRUE;
}


// ロック中ラウンドレーザーを描画 //
FVOID CEnemyExShot::DrawLockRLaser(RLaserData *pRL)
{
	D3DTLVERTEX			tlv[20];
	int					x, y, i, n;
	int					rad, ox, oy;
	DWORD				c;
	BYTE				d;

	// テクスチャのｕｖ座標を定める //
//	const D3DVALUE	u0 = D3DVAL(64) / D3DVAL(256);
//	const D3DVALUE	v0 = D3DVAL( 0) / D3DVAL(256);
//	const D3DVALUE	u1 = D3DVAL(96) / D3DVAL(256);
//	const D3DVALUE	v1 = D3DVAL(32) / D3DVAL(256);

	rad = pRL->m_Radius;
	ox  = pRL->m_X;
	oy  = pRL->m_Y;
	d   = (BYTE)(pRL->m_Count * 5);

	for(n=0; n<6; n++){
		c = 255 - (n << 5);
		c = RGBA_MAKE(c>>1, c, c>>1, 255);

		for(i=0; i<3; i++, d+=256/3){
			x = (ox + CosL(d, rad) ) >> 8;
			y = (oy + SinL(d, rad) ) >> 8;

			Set2DPointC(tlv+i, x, y, 0, 0, c);
		}
		tlv[3] = tlv[0];

		g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 4);

		d   += 12;
		rad  = (rad * 90) / 100;
	}
}

/*
// ラウンドレーザーを描画 //
FVOID CEnemyExShot::DrawRLaser(RLaserData *pRL)
{
	D3DTLVERTEX			tlv[20];
	int					rad, x, y;
	DWORD				c;

	// テクスチャのｕｖ座標を定める //
	const D3DVALUE	u0 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(  0) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(256) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(128) / D3DVAL(256);

	rad = pRL->m_Radius >> 8;
	x   = pRL->m_X >> 8;
	y   = pRL->m_Y >> 8;

	c   = min(255, (255 * pRL->m_Radius) / pRL->m_TargetRadius);
	c   = RGBA_MAKE(c, c, c, 255);

	Set2DPointC(tlv+0, x-rad, y-rad, u0, v0, c);
	Set2DPointC(tlv+1, x+rad, y-rad, u1, v0, c);
	Set2DPointC(tlv+2, x+rad, y+rad, u1, v1, c);
	Set2DPointC(tlv+3, x-rad, y+rad, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
*/

// 消去中ＲＬを描画 //
FVOID CEnemyExShot::DrawDelRLaser(RLaserData *pRL)
{
	D3DTLVERTEX			tlv[20];
	int					rad, x, y;
	DWORD				c;

	// テクスチャのｕｖ座標を定める //
	const D3DVALUE	u0 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(  0) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(256) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(128) / D3DVAL(256);

	rad = pRL->m_Radius >> 8;
	x   = pRL->m_X >> 8;
	y   = pRL->m_Y >> 8;
	c   = min(255, pRL->m_Count);
	c   = RGBA_MAKE(c, c, c, 255);

	Set2DPointC(tlv+0, x-rad, y-rad, u0, v0, c);
	Set2DPointC(tlv+1, x+rad, y-rad, u1, v0, c);
	Set2DPointC(tlv+2, x+rad, y+rad, u1, v1, c);
	Set2DPointC(tlv+3, x-rad, y+rad, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 落下中の十字架を描画させる //
FVOID CEnemyExShot::DrawCross(CrossData *pCr)
{
	D3DTLVERTEX			tlv[20];
	int					x, y;
	int					lx, ly, wx, wy;
	DWORD				c;
	BYTE				d;

	// --- テクスチャのｕｖ座標を定める ---
	//
	//              vect(wx, wy) : L(24/2)
	//                          |
	// vect(lx, ly) : L(40)---+-----------  -> d
	//                          |
	//
	const D3DVALUE	u0 = D3DVAL(  8) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(  0) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL( 48) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL( 24) / D3DVAL(256);

	x  = pCr->m_X;
	y  = pCr->m_Y;

	d  = pCr->m_Angle + 128;
	lx = CosL(d, 40 * 256);
	ly = SinL(d, 40 * 256);

	d -= 64;
	wx = CosL(d, (24/2)*256);
	wy = SinL(d, (24/2)*256);

	c   = RGBA_MAKE(255, 255, 255, 255);

	Set2DPointC(tlv+0, (x+wx+lx)>>8, (y+wy+ly)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+wx)>>8,    (y+wy)>>8,    u1, v0, c);
	Set2DPointC(tlv+2, (x-wx)>>8,    (y-wy)>>8,    u1, v1, c);
	Set2DPointC(tlv+3, (x-wx+lx)>>8, (y-wy+ly)>>8, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 消去中の十字架を描画させる //
FVOID CEnemyExShot::DrawDelCross(CrossData *pCr)
{
	D3DTLVERTEX			tlv[20];
	int					x, y;
	int					lx, ly, wx, wy;
	DWORD				c;
	BYTE				d;

	// --- テクスチャのｕｖ座標を定める ---
	//
	//              vect(wx, wy) : L(24/2)
	//                          |
	// vect(lx, ly) : L(40)---+-----------  -> d
	//                          |
	//
	const D3DVALUE	u0 = D3DVAL(  8) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(  0) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL( 48) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL( 24) / D3DVAL(256);

	x  = pCr->m_X;
	y  = pCr->m_Y;

	d  = pCr->m_Angle + 128;
	lx = CosL(d, 40 * 256);
	ly = SinL(d, 40 * 256);

	d -= 64;
	wx = CosL(d, (24/2)*256);
	wy = SinL(d, (24/2)*256);

	c  = pCr->m_Count;
	c  = RGBA_MAKE(c, c, c, 255);

	Set2DPointC(tlv+0, (x+wx+lx)>>8, (y+wy+ly)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+wx)>>8,    (y+wy)>>8,    u1, v0, c);
	Set2DPointC(tlv+2, (x-wx)>>8,    (y-wy)>>8,    u1, v1, c);
	Set2DPointC(tlv+3, (x-wx+lx)>>8, (y-wy+ly)>>8, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 十字架(警告)を描画させる //
FVOID CEnemyExShot::DrawCrossDanger(CrossData *pCr)
{
	D3DTLVERTEX			tlv[20];
	int					rx, ry, x, y;
	int					l;
	DWORD				c;
	BYTE				d;

	// テクスチャのｕｖ座標を定める //
	const D3DVALUE	u0 = D3DVAL(  8) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL( 32) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL( 72) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL( 96) / D3DVAL(256);

	x  = pCr->m_X >> 8;
	y  = pCr->m_Y >> 8;

	c  = pCr->m_Count;
	l  = min(ECROSS_RADIUS, (255 - c));

	d  = pCr->m_Angle;
	rx = CosL(d, l);
	ry = SinL(d, l);

	c  = RGBA_MAKE(c, c, c, 255);

	Set2DPointC(tlv+0, x+rx, y+ry, u0, v0, c);
	Set2DPointC(tlv+1, x-ry, y+rx, u1, v0, c);
	Set2DPointC(tlv+2, x-rx, y-ry, u1, v1, c);
	Set2DPointC(tlv+3, x+ry, y-rx, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 落下中の花を描画させる //
FVOID CEnemyExShot::DrawFlower(FlowerData *pFl, BYTE a)
{
	D3DTLVERTEX			tlv[20];
	int					rx, ry, x, y;
	int					l, tsx;
	DWORD				c;
	BYTE				d;

	// テクスチャのｕｖ座標を定める //
	tsx = (pFl->m_Color & 3) << 6;
	const D3DVALUE	u0 = D3DVAL(  0 + tsx) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL( 64 + tsx) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(192      ) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(256      ) / D3DVAL(256);

	x  = pFl->m_X >> 8;
	y  = pFl->m_Y >> 8;

	l  = pFl->m_Radius;
	d  = pFl->m_Angle;
	rx = CosL(d, l) >> 8;
	ry = SinL(d, l) >> 8;

	c  = RGBA_MAKE(a, a, a, 255);

	Set2DPointC(tlv+0, x+rx, y+ry, u0, v0, c);
	Set2DPointC(tlv+1, x-ry, y+rx, u1, v0, c);
	Set2DPointC(tlv+2, x-rx, y-ry, u1, v1, c);
	Set2DPointC(tlv+3, x+ry, y-rx, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 火炎放射・通常状態 //
FVOID CEnemyExShot::DrawGFire(GFireData *pF)
{
	D3DTLVERTEX			tlv[20];
	int					x, y;
	int					w;
	DWORD				c;
	BYTE				a;

	// テクスチャのｕｖ座標を定める //
	const D3DVALUE	u0 = D3DVAL(  0) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL( 64) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(192) / D3DVAL(256);

	x = pF->m_X >> 8;
	y = pF->m_Y >> 8;

	a = (BYTE)(40 * pF->m_Count / 256);
	c = RGBA_MAKE(a, a, a, 255);
//	c = RGBA_MAKE(255, 255, 255, 255);

//	w = 8 + ((24 * pF->m_Count) >> 8);
	w = pF->m_Size >> 8;

	Set2DPointC(tlv+0, x+w, y+w, u0, v0, c);
	Set2DPointC(tlv+1, x-w, y+w, u1, v0, c);
	Set2DPointC(tlv+2, x-w, y-w, u1, v1, c);
	Set2DPointC(tlv+3, x+w, y-w, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// イオンリング・通常状態 //
FVOID CEnemyExShot::DrawIonRing(IonRingData *pI)
{
	D3DTLVERTEX			tlv[20];
	int					x, y;
	int					w;
	DWORD				c;
	BYTE				a;

	// テクスチャのｕｖ座標を定める //
	const D3DVALUE	u0 = D3DVAL( 64) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(192) / D3DVAL(256);

	x = pI->x >> 8;
	y = pI->y >> 8;
	w = pI->size >> 8;

	a = 255;
	c = RGBA_MAKE(a, a, a, 255);

	Set2DPointC(tlv+0, x+w, y+w, u0, v0, c);
	Set2DPointC(tlv+1, x-w, y+w, u1, v0, c);
	Set2DPointC(tlv+2, x-w, y-w, u1, v1, c);
	Set2DPointC(tlv+3, x+w, y-w, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// イオンリング・さよなら中 //
FVOID CEnemyExShot::DrawDelIonRing(IonRingData *pI)
{
	D3DTLVERTEX			tlv[20];
	int					x, y;
	int					w;
	DWORD				c;
	BYTE				a;

	// テクスチャのｕｖ座標を定める //
	const D3DVALUE	u0 = D3DVAL( 64) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(128) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(192) / D3DVAL(256);

	x = pI->x >> 8;
	y = pI->y >> 8;
	w = pI->size >> 8;

	a = min(255, pI->Count);
	c = RGBA_MAKE(a, a, a, 255);

	Set2DPointC(tlv+0, x+w, y+w, u0, v0, c);
	Set2DPointC(tlv+1, x-w, y+w, u1, v0, c);
	Set2DPointC(tlv+2, x-w, y-w, u1, v1, c);
	Set2DPointC(tlv+3, x+w, y-w, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
