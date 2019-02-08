/*
 *   CMuse.cpp   : ミューズの定義
 *
 */

#include "CMuse.h"
#include "Gian2001.h"
#include "SECtrl.h"

#include "RndCtrl.h"


#define MUSE_GST_WEIGHT		3

#define EXST_SET_CROSS		0x00		// 十字架セット用タスク
#define EXST_MOVE_CROSS		0x01		// 移動中の十字架
#define EXST_HIT_CROSS		0x02		// ヒット中の十字架(一定時間Hitアリ)

#define EXST_BOMB_MSQUARE	0x03		// ボム用・魔方陣(オープン)
#define EXST_BOMB_MSQUARE2	0x04		// ボム用・魔方陣(クローズ中)
#define EXST_BOMB_SETCROSS	0x05		// ボム用・十字架セット用タスク
#define EXST_BOMB_CROSS		0x06		// ボム用・十字架

#define BOMBCROSS_SIZE		(16*256)	// ボム用十字架・サイズ
#define MUSE_EXSHOT_MOVE_DAMAGE	256		// 十字架・１回目のヒット
//#define MUSE_EXSHOT_HIT_DAMAGE	32		// 十字架・削り中
#define MUSE_EXSHOT_HIT_DAMAGE	24		// 十字架・削り中



/***** [グローバル変数] *****/
CMuse		g_MuseInfo;			// ミューズの情報格納用クラス



// コンストラクタ //
CMuse::CMuse()
{
	m_NormalSpeed = 3 * 256 - 128 - 32;		// 通常時の早さ
	m_ShiftSpeed  = 3 * 256 / 2;		// シフト移動時の早さ
	m_ChargeSpeed = 140 * 256;			// 溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_MUSE;	// キャラクタＩＤ
	m_NormalShotDx =  0 * 256;			// ショットの発射座標
	m_NormalShotDy = -4 * 256;			// ショットの発射座標

	m_BombTime = 180;		// ボムの有効時間
}


// デストラクタ //
CMuse::~CMuse()
{
}


// キャラクター固有の描画を行う //
FVOID CMuse::PlayerDraw(	int		x256			// Ｘ座標中心
						,	int		y256			// Ｙ座標中心
						,	int		State			// 状態(角度)
						,	BOOL	bDamaged		// ダメージを受けていれば真
						,	int		Transform		// 変形等をするときは非ゼロ
						,	int		TextureID)		// テクスチャ番号
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	int				x, y;
	int				dx, dy, tx;
	DWORD			c;

	c  = RGBA_MAKE(255, 255, 255, 245);

	// 描画状態により、関数の振り分けを行う //
	if(0 == State){
		u0 = D3DVAL(   0) / D3DVAL(256);
		u1 = D3DVAL(  32) / D3DVAL(256);
		v0 = D3DVAL(48*2) / D3DVAL(256);
		v1 = D3DVAL(48*3) / D3DVAL(256);
	}
	else if(State < 0){
		tx = -(State / MUSE_GST_WEIGHT) * 32;
		u0 = D3DVAL(tx   ) / D3DVAL(256);
		u1 = D3DVAL(tx+32) / D3DVAL(256);
		v0 = D3DVAL(48* 0) / D3DVAL(256);
		v1 = D3DVAL(48* 1) / D3DVAL(256);
	}
	else{
		tx = (State / MUSE_GST_WEIGHT) * 32;
		u0 = D3DVAL(tx   ) / D3DVAL(256);
		u1 = D3DVAL(tx+32) / D3DVAL(256);
		v0 = D3DVAL(48* 1) / D3DVAL(256);
		v1 = D3DVAL(48* 2) / D3DVAL(256);
	}

	dx = 16 * 256 * 9 / 10;
	dy = 24 * 256 * 9 / 10;

	if(Transform){
		dx = dx + Transform * 2048;
		dy = dy / (1 + Transform / 6);

		c  = max(0, 255 - Transform*3);
		c  = RGBA_MAKE(c, c, c, c);
	}

	x = (x256 - dx) >> 8;
	y = (y256 - dy) >> 8;
	Set2DPointC(tlv+0, x, y, u0, v0, c);

	x = (x256 + dx) >> 8;
	y = (y256 - dy) >> 8;
	Set2DPointC(tlv+1, x, y, u1, v0, c);

	x = (x256 + dx) >> 8;
	y = (y256 + dy) >> 8;
	Set2DPointC(tlv+2, x, y, u1, v1, c);

	x = (x256 - dx) >> 8;
	y = (y256 + dy) >> 8;
	Set2DPointC(tlv+3, x, y, u0, v1, c);

	if(bDamaged) g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	else         g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);

	g_pGrp->SetTexture(TextureID);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 描画用のＩＤを変更する //
FVOID CMuse::PlayerSetGrp(int *pState, WORD KeyCode)
{
	int			dir = 0;

	if(KeyCode & KEY_LEFT)  dir--;
	if(KeyCode & KEY_RIGHT) dir++;

	// 左右移動が無い場合 //
	if(0 == dir){
		if(      (*pState) < 0) (*pState)++;
		else if( (*pState) > 0) (*pState)--;
	}
	// 左方向に移動中 //
	else if(dir < 0){
		if(*pState > -MUSE_GST_WEIGHT*7) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  MUSE_GST_WEIGHT*7) (*pState)++;
	}
}


// エキストラショットを動作させる //
FVOID CMuse::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my;
	int			YMin;
	WORD		KeyCode;
	BYTE		d, d2;
	int			t, t2;

	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	ExtraShot				*pData;


	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());
	my      = *(pExShot->GetYPointer());
	KeyCode = pExShotInfo->KeyCode;
	YMin    = pExShot->GetYMin() - (20 * 256);

	// 十字架セット用タスク //
	ForEachFrontPtr(pExShot, EXST_SET_CROSS, it){
		d = 0;
		if(KeyCode & KEY_LEFT)  d -= 1;
		if(KeyCode & KEY_RIGHT) d += 1;

		if(d) it->d += d;
		else{
			t = 192 - (int)it->d;
			if(     t > 0) it->d --;
			else if(t < 0) it->d ++;
		}

		// セットを必要とする時 //
		// ここで pData を作成しているので注意する //
		t  = it->Count;
		t2 = max(1, it->x);
		if(0 == t % t2){
			pData = pExShot->InsertBack(EXST_MOVE_CROSS);
			if(NULL == pData) return;

			pData->x     = mx;
			pData->y     = my;
			pData->Count = (t / (t2*2) + 10) * 256;
			pData->d     = it->d;
			pData->v     = (t / t2 + 8) * 128;

			PlaySE(SNDID_TRI_STAR_LARGE, mx);
		}

		// カウンタが生きていれば、デクリメント
		// それ以外は、死ね
		if(t < it->v) it->Count++;
		else          DelContinuePtr(pExShot, it);
	}

	// 十字架・移動中 //
	ForEachFrontPtr(pExShot, EXST_MOVE_CROSS, it){
		pData = it.GetPtr();

		pData->x += CosL(pData->d, pData->v);
		pData->y += SinL(pData->d, pData->v);

		if(pData->y < YMin) DelContinuePtr(pExShot, it);
	}

	// ボム用・十字架 //
	ForEachFrontPtr(pExShot, EXST_BOMB_CROSS, it){
		pData = it.GetPtr();

		pData->x += CosL(pData->d, 800*256/50);
		pData->y += SinL(pData->d, 800*256/50);
		if(pData->v) pData->v--;
		else{
			pData->v     = BOMBCROSS_SIZE * 2;
			pData->Count = 255;
			pExShot->MoveBack(it, EXST_HIT_CROSS);
		}
	}

	// 十字架・ヒット中 //
	ForEachFrontPtr(pExShot, EXST_HIT_CROSS, it){
		pData = it.GetPtr();

		if(pData->Count > 2) pData->Count -= 2;
		else                 DelContinuePtr(pExShot, it);

		if(pData->v > 0) pData->v -= 256;
		else             DelContinuePtr(pExShot, it);
	}

	// ボム用魔方陣 //
	ForEachFrontPtr(pExShot, EXST_BOMB_MSQUARE, it){
		pData = it.GetPtr();

//		pData->x = mx;
//		pData->y = my;
		pData->d -= 3;
		pData->v += 2;

		if(pData->Count < m_BombTime-90) pData->Count++;
		else pExShot->MoveBack(it, EXST_BOMB_MSQUARE2);
	}

	// ボム用魔方陣 //
	ForEachFrontPtr(pExShot, EXST_BOMB_MSQUARE2, it){
		pData = it.GetPtr();

//		pData->x = mx;
//		pData->y = my;
		pData->d -= 3;

		if(pData->v > 4) pData->v -= 4;
		else DelContinuePtr(pExShot, it);
	}

	// ボム用・十字架セット用タスク //
	ForEachFrontPtr(pExShot, EXST_BOMB_SETCROSS, it){
		pData = pExShot->InsertBack(EXST_BOMB_CROSS);
		if(NULL == pData) return;

		d  = -64 - 8 + RndEx() % 16;
		d2 = (BYTE)RndEx();
		t  = (RndEx()%60) << 8;

		pData->x     = it->x + CosL(d, 800*256) + CosL(d2, t);
		pData->y     = it->y + SinL(d, 800*256) + SinL(d2, t);
		pData->Count = BOMBCROSS_SIZE;	// 大きさ
		pData->d     = d + 128;
		pData->v     = 48;

		// カウンタが生きていれば、デクリメント
		// それ以外は、死ね
		if(it->Count) it->Count--;
		else          DelContinuePtr(pExShot, it);
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CMuse::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot;
	CExtraShot::Iterator	it;

	DWORD		Damage;
	int			x, y;
	int			Size;

	Damage = 0;				// ダメージをゼロ初期化
	x      = pEnemy->x;		// 敵のＸ座標
	y      = pEnemy->y;		// 敵のＹ座標

	ForEachFrontPtr(pExtraShot, EXST_MOVE_CROSS, it){
		pShot = it.GetPtr();

		Size = (pShot->Count>>1) + pEnemy->size;

		if(HitCheckFast(y, (pShot->y), Size)
		&& HitCheckFast(x, (pShot->x), Size)){
			Damage += MUSE_EXSHOT_MOVE_DAMAGE;
			pShot->v     = pShot->Count << 2;
			pShot->Count = 255;
			pExtraShot->MoveBack(it, EXST_HIT_CROSS);
		}
	}

	ForEachFrontPtr(pExtraShot, EXST_HIT_CROSS, it){
		pShot = it.GetPtr();

		Size = (pShot->v>>1) + pEnemy->size;

		if(HitCheckFast(y, (pShot->y), Size)
		&& HitCheckFast(x, (pShot->x), Size)){
			Damage += MUSE_EXSHOT_HIT_DAMAGE;
		}
	}

	return Damage;
}


// エキストラショットを描画する //
FVOID CMuse::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot::Iterator		it;
	CExtraShot					*pExShot;

	pExShot = pExShotInfo->pExtraShot;

	// 使用するテクスチャとレンダリングステートの初期化 //
	g_pGrp->SetTexture(TextureID);				// テクスチャ
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);	// 描画ステート

	// ボム用魔方陣 //
	ForEachFrontPtr(pExShot, EXST_BOMB_MSQUARE, it){
		DrawMSquare(it.GetPtr());
	}

	// ボム用魔方陣(クローズ中) //
	ForEachFrontPtr(pExShot, EXST_BOMB_MSQUARE2, it){
		DrawMSquare(it.GetPtr());
	}

	// 十字架・移動中 //
	ForEachFrontPtr(pExShot, EXST_MOVE_CROSS, it){
		DrawNormalCross(it.GetPtr());
	}

	// 十字架・削り中 //
	ForEachFrontPtr(pExShot, EXST_HIT_CROSS, it){
		DrawHitCross(it.GetPtr());
	}

	// ボム用・十字架 //
	ForEachFrontPtr(pExShot, EXST_BOMB_CROSS, it){
		DrawNormalCross(it.GetPtr());
	}
}


// 通常の溜め攻撃 //
FVOID CMuse::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	WORD			KeyCode;
	int				mx, my;
	int				n;

	// １／２まで溜まっていないので発動不可 //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());	// 現在のＸ座標
	my      = *(pExShot->GetYPointer());	// 現在のＹ座標
	KeyCode = pExShotInfo->KeyCode;

	// 発射する十字架の数を決める //
	n = (pExShotInfo->Charge < CGAUGE_LEVEL1) ? 6 : 15;

	// 十字架セット用タスクを仕掛ける //
	pData = pExShot->InsertBack(EXST_SET_CROSS);
	if(NULL == pData) return;

	pData->Count = 0;		// カウンタ初期値
	pData->d     = -64;		// 発射方向

	int		delta = 2;

	if(KeyCode & KEY_DOWN) delta *= 2;
	if(KeyCode & KEY_UP)   delta /= 2;

	pData->v = n * delta;	// 発射タイミング用カウンタ(終了値)
	pData->x = delta;		// 発射間隔
}


// ボムアタック //
FBOOL CMuse::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CMuse::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	return TRUE;
}


// 通常ボム //
FVOID CMuse::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	int				mx, my;
	BYTE			d;

	pBGDraw->SetBlendColor(255, 64, 64, m_BombTime - 60);

	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
	my      = *(pExShot->GetYPointer());		// 現在のＹ座標

	// 十字架セット用タスク //
	pData = pExShot->InsertBack(EXST_BOMB_SETCROSS);
	if(NULL == pData) return;
	pData->Count = m_BombTime-60;
	pData->x     = mx;
	pData->y     = my;

	// 魔方陣をセット //
	d = (BYTE)RndEx();
	for(int i=0; i<4; i++){
		pData = pExShot->InsertBack(EXST_BOMB_MSQUARE);
		if(NULL == pData) return;

		pData->x     = mx;
		pData->y     = my;
		pData->Count = 0;
		pData->v     = i * 24;
		pData->d     = d + i;
	}
}


// 移動状態の十字架を描画する //
FVOID CMuse::DrawNormalCross(ExtraShot *pc)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
	D3DVALUE		u0, v0, u1, v1;		// テクスチャ座標
	int				x, y;				// 描画座標値
	int				dx, dy;
	int				lx, ly, wx, wy;
	DWORD			c;
	BYTE			d;

	u0 = D3DVAL(0)      / D3DVAL(256);
	u1 = D3DVAL(40)     / D3DVAL(256);
	v0 = D3DVAL(144)    / D3DVAL(256);
	v1 = D3DVAL(144+24) / D3DVAL(256);

	x = pc->x;
	y = pc->y;

	dx = pc->Count * 40 / 24;
	dy = pc->Count;

	d = pc->d;

	lx = CosL(d, dx);
	ly = SinL(d, dx);

	wx = CosL(d-64, dy);
	wy = SinL(d-64, dy);

	c  = RGBA_MAKE(255, 255, 255, 255);

	Set2DPointC(tlv+0, (x+wx   )>>8, (y+wy   )>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+wx+lx)>>8, (y+wy+ly)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (x-wx+lx)>>8, (y-wy+ly)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (x-wx   )>>8, (y-wy   )>>8, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// ヒット状態の十字架を描画する //
FVOID CMuse::DrawHitCross(ExtraShot *pc)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
	D3DVALUE		u0, v0, u1, v1;		// テクスチャ座標
	int				x, y;				// 描画座標値
	int				dx, dy;
	int				lx, ly, wx, wy;
	DWORD			c;
	BYTE			d;

	u0 = D3DVAL(0)      / D3DVAL(256);
	u1 = D3DVAL(40)     / D3DVAL(256);
	v0 = D3DVAL(144)    / D3DVAL(256);
	v1 = D3DVAL(144+24) / D3DVAL(256);

	x = pc->x;
	y = pc->y;

	dx = pc->v * 40 / 24;
	dy = pc->v;

	d = pc->d;

	lx = CosL(d, dx);
	ly = SinL(d, dx);

	wx = CosL(d-64, dy);
	wy = SinL(d-64, dy);

	c = pc->Count;
	c = RGBA_MAKE(c, c, c, 255);

	Set2DPointC(tlv+0, (x+wx   )>>8, (y+wy   )>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+wx+lx)>>8, (y+wy+ly)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (x-wx+lx)>>8, (y-wy+ly)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (x-wx   )>>8, (y-wy   )>>8, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 魔方陣を描画する //
FVOID CMuse::DrawMSquare(ExtraShot *pc)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y;
	DWORD			c;
	int				dx, dy, l;
	BYTE			d;

	x = (pc->x) >> 8;
	y = (pc->y) >> 8;

	u0 = D3DVAL(128) / D3DVAL(256);
	u1 = D3DVAL(256) / D3DVAL(256);
	v0 = D3DVAL(128) / D3DVAL(256);
	v1 = D3DVAL(256) / D3DVAL(256);

//	t = (pShot->Count - ( / 2)) << 1;
//	t = 255 - min(255, abs(t) * 255 / STG1_EXSHOT_COUNT);
//	c = RGBA_MAKE(t, t, t, 255);
	c = RGBA_MAKE(255/4, 255/4, 255/4, 255);

	d  = pc->d;

	l  = 30 * 256 + min(90*256, pc->v * 256);
	dx = CosL(d, l) >> 8;
	dy = SinL(d, l) >> 8;

	Set2DPointC(tlv+0, x+dx, y+dy, u0, v0, c);
	Set2DPointC(tlv+1, x+dy, y-dx, u1, v0, c);
	Set2DPointC(tlv+2, x-dx, y-dy, u1, v1, c);
	Set2DPointC(tlv+3, x-dy, y+dx, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
