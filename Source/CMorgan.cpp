/*
 *   CMorgan.cpp   : モーガンの定義
 *
 */


#include "CMorgan.h"
#include "Gian2001.h"
#include "SECtrl.h"

#include "RndCtrl.h"



/***** [グローバル変数] *****/
CMorgan		g_MorganInfo;			// モーガンの情報格納用クラス



/***** [ 定数 ] *****/
#define MORGAN_GST_RANGE		4		// 横アニメ推移の早さ

#define EXST_SETBOMB		0x00		// 爆発セット用タスク
#define EXST_MGN_SUSPEND	0x01		// 発動前タスク
#define EXST_MGN_COUNTDOWN	0x02		// 発動前のカウントダウン用タスク
#define EXST_MGN_ACTIVE		0x03		// 爆発中タスク
#define EXST_MGN_DELETE		0x04		// 消去中タスク

#define EXST_MGN_BOMB		0x05		// モーガン・ボム用タスク
#define EXST_MGN_BOMBDEL	0x06		// モーガン・ボム消滅用タスク

#define STDBOMB_TIME		120			// ボム・爆発時間

#define SETBOMB_INTERVAL		5			// ボムセットの間隔
#define ACTIVEBOMB_TIME			35			// 爆発時間
#define MGN_EXSHOT_DAMAGE		(256/3)		// エキストラショット・ダメージ量
#define MGN_EXBOMB_DAMAGE		(196)		// ボム・ダメージ量



/////////////////////////////////////////////////////////////////////////////
// 参考：EXST_MGN_ACTIVE では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y)      : 爆発している座標
//   v         : 当たり判定サイズ
//  count      : カウンタ
//---------------------------------------------------------------------------



// コンストラクタ //
CMorgan::CMorgan()
{
//	m_NormalSpeed = 3 * 256 - 64;		// 通常時の早さ
//	m_ShiftSpeed  = 3 * 256 / 2;		// シフト移動時の早さ
	m_NormalSpeed = 6 * 256;			// 通常時の早さ
	m_ShiftSpeed  = 2 * 256;			// シフト移動時の早さ

	m_ChargeSpeed = 180 * 256;			// 溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_MORGAN;	// キャラクタＩＤ
	m_NormalShotDx =  0 * 256;			// ショットの発射座標
	m_NormalShotDy = -4 * 256;			// ショットの発射座標

	m_BombTime = 180;		// ボムの有効時間
}


// デストラクタ //
CMorgan::~CMorgan()
{
}


// キャラクター固有の描画を行う //
FVOID CMorgan::PlayerDraw(	int		x256			// Ｘ座標中心
						,	int		y256			// Ｙ座標中心
						,	int		State			// 状態(角度)
						,	BOOL	bDamaged		// ダメージを受けていれば真
						,	int		Transform		// 変形等をするときは非ゼロ
						,	int		TextureID)		// テクスチャ番号
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	int				x, y;
	int				dx, dy;
	DWORD			c;

	c  = RGBA_MAKE(255, 255, 255, 245);

	if(0 == State){
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 48) / D3DVAL(256);
		v0 = D3DVAL(  0) / D3DVAL(256);
		v1 = D3DVAL( 40) / D3DVAL(256);
	}
	else if(State < 0){
		if(-MORGAN_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL( 48) / D3DVAL(256);
			u1 = D3DVAL( 96) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 40) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL( 96) / D3DVAL(256);
			u1 = D3DVAL(144) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 40) / D3DVAL(256);
		}
	}
	else{
		if(MORGAN_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(144) / D3DVAL(256);
			u1 = D3DVAL(192) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 40) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(192) / D3DVAL(256);
			u1 = D3DVAL(240) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 40) / D3DVAL(256);
		}
	}

	// 4:3
	dx = 24 * 5 / 7 * 256;
	dy = 20 * 5 / 7 * 256;
//	dx = 20 * 256;
//	dy = 15 * 256;
/*
	if(0 == State){
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 48) / D3DVAL(256);
		v0 = D3DVAL(  0) / D3DVAL(256);
		v1 = D3DVAL( 56) / D3DVAL(256);
	}
	else if(State < 0){
		if(-MORGAN_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL( 48) / D3DVAL(256);
			u1 = D3DVAL( 96) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL( 96) / D3DVAL(256);
			u1 = D3DVAL(144) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}
	else{
		if(MORGAN_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(144) / D3DVAL(256);
			u1 = D3DVAL(192) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(192) / D3DVAL(256);
			u1 = D3DVAL(240) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}

	// 4:3
	dx = 24 * 5 / 7 * 256;
	dy = 28 * 5 / 7 * 256;
//	dx = 20 * 256;
//	dy = 15 * 256;
*/
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
FVOID CMorgan::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -MORGAN_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  MORGAN_GST_RANGE) (*pState)++;
	}
}


// エキストラショットを動作させる //
FVOID CMorgan::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my;
	WORD		KeyCode;

//	int			v;
	BYTE		d, a;
	BOOL		bFinished;

	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	ExtraShot				*pData;
	ExtraOption				*pOpt;

	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());
	my      = *(pExShot->GetYPointer());
	KeyCode = pExShotInfo->KeyCode;

	// 爆発セット用タスク //
	if(KeyCode & KEY_SHOT){
		pExShot->GetFrontIterator(it, EXST_SETBOMB);
		if(it != NULL){
			if(it->v){
				it->v--;
			}
			else{
				it->Count++;

				// セット //
				if(0 == it->d){
					it->d = SETBOMB_INTERVAL;

					pData = pExShot->InsertFront(EXST_MGN_SUSPEND);
					if(pData){
						PlaySE(SNDID_LOCKON, mx);

						pData->x     = mx;
						pData->y     = my;
						pData->d     = 0;
						pData->Count = it->Count / 5;	// EXST_MGN_COUNTDOWN にて使用
					}
					else{	// 強制的に自動発動させる
						Level1Attack(pExShotInfo);
					}
				}
				else{
					it->d--;
				}
			}
		}
		else{
			pData = pExShot->InsertFront(EXST_SETBOMB);
			if(pData){
				pData->Count = 0;					// タスク発生からの相対時間
				pData->d     = SETBOMB_INTERVAL;	// 爆発タスクの発生間隔
				pData->v     = SETBOMB_INTERVAL*2;	// 初期待ち時間
			}
		}
	}
	// ショットキーが押されていない場合は消し去る //
	else{
		ForEachFrontPtr(pExShot, EXST_SETBOMB, it){
			DelContinuePtr(pExShot, it);
		}
	}

	// 発動前、アニメ //
	ForEachFrontPtr(pExShot, EXST_MGN_SUSPEND, it){
		if(it->d < 10) it->d++;
	}

	// 発動はまだ？ //
	ForEachFrontPtr(pExShot, EXST_MGN_COUNTDOWN, it){
		if(it->Count){
			if(it->d < 10) it->d++;
			it->Count--;
		}
		else{
			PlaySE(SNDID_ENEMY_DESTROY, it->x);
			pExShot->MoveBack(it, EXST_MGN_ACTIVE);
		}
	}

	// 焼き尽くせ //
	ForEachFrontPtr(pExShot, EXST_MGN_ACTIVE, it){
		it->Count++;

		it->v = (28 * 256 * it->Count) / ACTIVEBOMB_TIME;

		if(it->Count >= ACTIVEBOMB_TIME){
			pExShot->MoveBack(it, EXST_MGN_DELETE);
		}
	}

	// 後始末 //
	ForEachFrontPtr(pExShot, EXST_MGN_DELETE, it){
		if(it->Count >= 2){
			it->Count -= 2;
		}
		else{
			DelContinuePtr(pExShot, it);
		}
	}

	// ボム用爆発 //
	ForEachFrontPtr(pExShot, EXST_MGN_BOMB, it){
		pData = it.GetPtr();
		if(pData->Count >= STDBOMB_TIME){
			pOpt = pData->Option;
			for(int i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
				pOpt->count = pOpt->count = (STDBOMB_TIME/8 * i) / EXTRAOPTION_MAX;
				pOpt->d     = RndEx() % 256;
			}

			pExShot->MoveBack(it, EXST_MGN_BOMBDEL);
			continue;
		}

		pOpt = pData->Option;
		for(int i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
			if(pOpt->count) pOpt->count--;
			else{
				if(pOpt->a){
					d = pOpt->d;

					pOpt->a  = max(0, pOpt->a-4);
					pOpt->x += CosL(d, 6*256);
					pOpt->y += SinL(d, 6*256);
				}
			}
		}

		pData->Count++;
	}

	// ボム消滅 //
	ForEachFrontPtr(pExShot, EXST_MGN_BOMBDEL, it){
		pData = it.GetPtr();

		bFinished = TRUE;
		pOpt = pData->Option;
		for(int i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
			if(pOpt->count) pOpt->count--;
			else{
				a = pOpt->a;

				if(a != 255){
					d = pOpt->d;

					if(pOpt->a < 196){
						if(i & 1) pOpt->d = d + 6;
						else      pOpt->d = d - 6;
					}

					pOpt->a  = min(255, pOpt->a+4);
					pOpt->x -= CosL(d, a*96);
					pOpt->y -= SinL(d, a*96);

					bFinished = FALSE;
				}
			}
		}

		if(pData->Count <= 4){
			pData->Count = 0;

			if(bFinished){
				DelContinuePtr(pExShot, it);
			}
		}
		else{
			pData->Count-=4;
		}
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CMorgan::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot;
	CExtraShot::Iterator	it;
	ExtraOption				*pOpt;

	DWORD		Damage;
	int			x, y;
	int			Size;
	int			w;

	Damage = 0;				// ダメージをゼロ初期化
	x      = pEnemy->x;		// 敵のＸ座標
	y      = pEnemy->y;		// 敵のＹ座標

	// 当たり判定サイズ(後で変更のこと) //
	Size = pEnemy->size;

	ForEachFrontPtr(pExtraShot, EXST_MGN_ACTIVE, it){
		pShot = it.GetPtr();

		w = Size + pShot->v;
		if(HitCheckFast(y, (pShot->y), w)
		&& HitCheckFast(x, (pShot->x), w)){
			Damage += MGN_EXSHOT_DAMAGE;
		}
	}

	w = Size + 8 * 256;
	ForEachFrontPtr(pExtraShot, EXST_MGN_BOMBDEL, it){
		pShot = it.GetPtr();

		pOpt = pShot->Option;
		for(int i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
			if(HitCheckFast(y, (pOpt->y), w)
			&& HitCheckFast(x, (pOpt->x), w)){
				Damage += MGN_EXBOMB_DAMAGE;
			}
		}
	}

	return Damage;
}


// エキストラショットを描画する //
FVOID CMorgan::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;

	// エキストラショット格納クラスに接続 //
	pExShot = pExShotInfo->pExtraShot;

	// 描画の前準備 //
	g_pGrp->SetTexture(TextureID);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	// 爆発中のタスクを描画すべし //
	ForEachFrontPtr(pExShot, EXST_MGN_ACTIVE, it){
		DrawExplosion(it.GetPtr());
	}

	// 消去中のタスクを描画すべし //
	ForEachFrontPtr(pExShot, EXST_MGN_DELETE, it){
		DrawExplosion(it.GetPtr());
	}

	// ロックされている個所を描画すべし(サスペンド中) //
	ForEachFrontPtr(pExShot, EXST_MGN_SUSPEND, it){
		DrawExpLockon(it.GetPtr());
	}

	// ロックされている個所を描画すべし(カウントダウン中) //
	ForEachFrontPtr(pExShot, EXST_MGN_COUNTDOWN, it){
		DrawExpLockon(it.GetPtr());
	}

	// ボムの描画 //
	ForEachFrontPtr(pExShot, EXST_MGN_BOMB, it){
		DrawBombExp(it.GetPtr());
	}

	// ボムの描画 //
	ForEachFrontPtr(pExShot, EXST_MGN_BOMBDEL, it){
		DrawBombExpDel(it.GetPtr());
	}
}


// 通常の溜め攻撃 //
FVOID CMorgan::Level1Attack(ExtraShotInfo *pExShotInfo)
{
//	ExtraShot				*pData;
//	ExtraOption				*pOpt;
	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;

	// １／２まで溜まっていないので発動不可 //
//	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

//	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
//	my      = *(pExShot->GetYPointer());		// 現在のＹ座標

	// 爆発セット用タスクを削除する //
	ForEachFrontPtr(pExShot, EXST_SETBOMB, it){
		DelContinuePtr(pExShot, it);
	}

	// サスペンド状態のタスクを稼動させる //
	ForEachFrontPtr(pExShot, EXST_MGN_SUSPEND, it){
		pExShot->MoveBack(it, EXST_MGN_COUNTDOWN);
	}
}


// ボムアタック //
FBOOL CMorgan::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CMorgan::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	return TRUE;
}


// 通常ボム //
FVOID CMorgan::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	pBGDraw->SetBlendColor(200, 200, 255, m_BombTime-60);

	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
//	int				width, ox;
	int				i, l;
	int				mx, my;
	BYTE			d;

	pExShot = pExShotInfo->pExtraShot;

	// 座標値を取得する //
//	ox    = pBGDraw->GetTargetOx();			// 画面中央
//	width = pBGDraw->GetTargetWidth() / 2;	// 画面幅 / ２
	mx = *(pExShot->GetXPointer());			// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());			// 現在のＹ座標を取得
	my = max(120*256, my - 128*256);

	// 爆発を仕掛ける //
	pData = pExShot->InsertBack(EXST_MGN_BOMB);
	if(NULL == pData) return;	// 空きが無い
	pData->x     = mx;
	pData->y     = my;
	pData->Count = 0;
	pData->v     = 0;

	// 集まる光の玉 //
	pOpt = pData->Option;
	for(i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
		d = 128 + RndEx()%128;
		l = (RndEx()%50 + 350) * 256;

		pOpt->a     = 255;
		pOpt->count = (STDBOMB_TIME/2 * i) / EXTRAOPTION_MAX;
		pOpt->x     = mx + CosL(d, l);
		pOpt->y     = my + SinL(d, l);
		pOpt->d     = d + 128;
	}
}


// 爆発の描画 //
FVOID CMorgan::DrawExplosion(ExtraShot *pExShot)
{
	D3DTLVERTEX			tlv[20];
	DWORD				c;
	int					w, h;
	int					x, y;
	int					t;

	const D3DVALUE	u0 = D3DVAL(0     ) / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(128   ) / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(64    ) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(128+56) / D3DVAL(256);

	x = pExShot->x >> 8;
	y = pExShot->y >> 8;
	t = pExShot->Count;

	w = (32 * t) / ACTIVEBOMB_TIME;
	h = (28 * t) / ACTIVEBOMB_TIME;

	t = (255 * t) / ACTIVEBOMB_TIME;
	c = RGBA_MAKE(t, t, t, 255);

	Set2DPointC(tlv+0, x-w, y-h, u0, v0, c);
	Set2DPointC(tlv+1, x+w, y-h, u1, v0, c);
	Set2DPointC(tlv+2, x+w, y+h, u1, v1, c);
	Set2DPointC(tlv+3, x-w, y+h, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// ロックしてある個所を描画 //
FVOID CMorgan::DrawExpLockon(ExtraShot *pExShot)
{
	D3DTLVERTEX			tlv[20];
	DWORD				c;
	int					w, h;
	int					x, y, sx, sy;
	int					d;
	D3DVALUE			u0, v0, u1, v1;

	x = pExShot->x >> 8;
	y = pExShot->y >> 8;
	d = 2 + ((10 - pExShot->d) << 2);
	w = 8;//(32 * t) / ACTIVEBOMB_TIME;
	h = 8;//(28 * t) / ACTIVEBOMB_TIME;

	if(pExShot->d != 10) c = 64 + (pExShot->d << 2);
	else                 c = 255;

	c = RGBA_MAKE(c, c, c, 255);

	// 左上 //
	u0 = D3DVAL(  0) / D3DVAL(256);	v0 = D3DVAL(192) / D3DVAL(256);
	u1 = D3DVAL( 24) / D3DVAL(256);	v1 = D3DVAL(216) / D3DVAL(256);

	sx = x - d;		sy = y - d;
	Set2DPointC(tlv+0, sx-w, sy-h, u0, v0, c);
	Set2DPointC(tlv+1, sx+w, sy-h, u1, v0, c);
	Set2DPointC(tlv+2, sx+w, sy+h, u1, v1, c);
	Set2DPointC(tlv+3, sx-w, sy+h, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// 右上 //
	u0 = D3DVAL( 24) / D3DVAL(256);	v0 = D3DVAL(192) / D3DVAL(256);
	u1 = D3DVAL( 48) / D3DVAL(256);	v1 = D3DVAL(216) / D3DVAL(256);

	sx = x + d;		sy = y - d;
	Set2DPointC(tlv+0, sx-w, sy-h, u0, v0, c);
	Set2DPointC(tlv+1, sx+w, sy-h, u1, v0, c);
	Set2DPointC(tlv+2, sx+w, sy+h, u1, v1, c);
	Set2DPointC(tlv+3, sx-w, sy+h, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// 左下 //
	u0 = D3DVAL(  0) / D3DVAL(256);	v0 = D3DVAL(216) / D3DVAL(256);
	u1 = D3DVAL( 24) / D3DVAL(256);	v1 = D3DVAL(240) / D3DVAL(256);

	sx = x - d;		sy = y + d;
	Set2DPointC(tlv+0, sx-w, sy-h, u0, v0, c);
	Set2DPointC(tlv+1, sx+w, sy-h, u1, v0, c);
	Set2DPointC(tlv+2, sx+w, sy+h, u1, v1, c);
	Set2DPointC(tlv+3, sx-w, sy+h, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	// 右下 //
	u0 = D3DVAL( 24) / D3DVAL(256);	v0 = D3DVAL(216) / D3DVAL(256);
	u1 = D3DVAL( 48) / D3DVAL(256);	v1 = D3DVAL(240) / D3DVAL(256);

	sx = x + d;		sy = y + d;
	Set2DPointC(tlv+0, sx-w, sy-h, u0, v0, c);
	Set2DPointC(tlv+1, sx+w, sy-h, u1, v0, c);
	Set2DPointC(tlv+2, sx+w, sy+h, u1, v1, c);
	Set2DPointC(tlv+3, sx-w, sy+h, u0, v1, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

}


// ボム爆発の描画 //
FVOID CMorgan::DrawBombExp(ExtraShot *pExShot)
{
	ExtraOption			*pOpt;
	D3DTLVERTEX			tlv[20];
	DWORD				c;
	int					w, h;
	int					x, y;
	int					t;

	D3DVALUE	u0 = D3DVAL(0     ) / D3DVAL(256);
	D3DVALUE	v0 = D3DVAL(128   ) / D3DVAL(256);
	D3DVALUE	u1 = D3DVAL(64    ) / D3DVAL(256);
	D3DVALUE	v1 = D3DVAL(128+56) / D3DVAL(256);

	x = pExShot->x >> 8;
	y = pExShot->y >> 8;
	t = pExShot->Count;

	w = (32*3 * t) / STDBOMB_TIME;
	h = (28*3 * t) / STDBOMB_TIME;

//	t = (255 * t) / STDBOMB_TIME;
//	c = RGBA_MAKE(t, t, t, 255);
	c = RGBA_MAKE(255, 255, 255, 255);

	Set2DPointC(tlv+0, x-w, y-h, u0, v0, c);
	Set2DPointC(tlv+1, x+w, y-h, u1, v0, c);
	Set2DPointC(tlv+2, x+w, y+h, u1, v1, c);
	Set2DPointC(tlv+3, x-w, y+h, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


	u0 = D3DVAL(0    ) / D3DVAL(256);
	v0 = D3DVAL(40   ) / D3DVAL(256);
	u1 = D3DVAL(32   ) / D3DVAL(256);
	v1 = D3DVAL(40+32) / D3DVAL(256);

	pOpt = pExShot->Option;
	for(int i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
		if(255 == pOpt->a) continue;

		x = pOpt->x >> 8;
		y = pOpt->y >> 8;
		w = 12 - (pOpt->a * 12 / 256);

		c = min(pOpt->a+64, 255);
		c = RGBA_MAKE(c, c, c, 255);

		Set2DPointC(tlv+0, x-w, y-w, u0, v0, c);
		Set2DPointC(tlv+1, x+w, y-w, u1, v0, c);
		Set2DPointC(tlv+2, x+w, y+w, u1, v1, c);
		Set2DPointC(tlv+3, x-w, y+w, u0, v1, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// 消去用ボム爆発の描画 //
FVOID CMorgan::DrawBombExpDel(ExtraShot *pExShot)
{
	ExtraOption			*pOpt;
	D3DTLVERTEX			tlv[20];
	DWORD				c;
	int					w, h;
	int					x, y;
	int					t;

	D3DVALUE	u0 = D3DVAL(0     ) / D3DVAL(256);
	D3DVALUE	v0 = D3DVAL(128   ) / D3DVAL(256);
	D3DVALUE	u1 = D3DVAL(64    ) / D3DVAL(256);
	D3DVALUE	v1 = D3DVAL(128+56) / D3DVAL(256);

	x = pExShot->x >> 8;
	y = pExShot->y >> 8;
	t = pExShot->Count;

	if(t){
		w = (32*3 * t) / STDBOMB_TIME;
		h = (28*3 * t) / STDBOMB_TIME;

	//	t = (255 * t) / STDBOMB_TIME;
	//	c = RGBA_MAKE(t, t, t, 255);
		c = RGBA_MAKE(255, 255, 255, 255);

		Set2DPointC(tlv+0, x-w, y-h, u0, v0, c);
		Set2DPointC(tlv+1, x+w, y-h, u1, v0, c);
		Set2DPointC(tlv+2, x+w, y+h, u1, v1, c);
		Set2DPointC(tlv+3, x-w, y+h, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}


	u0 = D3DVAL(0    ) / D3DVAL(256);
	v0 = D3DVAL(40   ) / D3DVAL(256);
	u1 = D3DVAL(32   ) / D3DVAL(256);
	v1 = D3DVAL(40+32) / D3DVAL(256);

	pOpt = pExShot->Option;
	for(int i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
//		if(255 == pOpt->a) continue;

		x = pOpt->x >> 8;
		y = pOpt->y >> 8;
		w = pOpt->a / 3;

//		c = 255;	//min(pOpt->a+64, 255);
		c = RGBA_MAKE(255, 255, 255, 255);

		Set2DPointC(tlv+0, x-w, y-w, u0, v0, c);
		Set2DPointC(tlv+1, x+w, y-w, u1, v0, c);
		Set2DPointC(tlv+2, x+w, y+w, u1, v1, c);
		Set2DPointC(tlv+3, x-w, y+w, u0, v1, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
