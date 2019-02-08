/*
 *   CYuka.cpp   : 幽香の定義
 *
 */

#include "CYuka.h"
#include "Gian2001.h"
#include "SECtrl.h"

#include "RndCtrl.h"



/***** [グローバル変数] *****/
CYuka		g_YukaInfo;			// 幽香の情報格納用クラス



/***** [ 定数 ] *****/
#define YUKA_GST_RANGE		4			// 横アニメ推移の早さ
#define YUKA_EXSHOT_SIZE	(10*256)	// エキストラショットの当たりサイズ
#define YUKA_EXSHOT_DAMAGE	(64*2)		// エキストラショットのダメージ

#define YUKA_EXSHOT_SET		0x00	// ワイドショットセット(直進部)
#define YUKA_EXSHOT_SET2	0x01	// ワイドショットセット(ワイド部)
#define YUKA_EXSHOT_NORMAL	0x02	// ワイドショット移動中
#define YUKA_EXSHOT_DELETE	0x03	// ワイドショット消滅中

#define YUKA_BOMB_SET		0x04	// ボム・セット用タスク
#define YUKA_BOMB_WAIT		0x05	// ボム・待ち状態(拡大中！)
#define YUKA_BOMB_NORMAL	0x06	// ボム・通常状態



// コンストラクタ //
CYuka::CYuka()
{
	m_NormalSpeed =  3 * 256-64;		// 通常時の早さ
	m_ShiftSpeed  = (3 * 256-64) / 2;	// シフト移動時の早さ
	m_ChargeSpeed = 160 * 256;			// 溜めの早さ
//	m_BuzzSpeed   = 65536;				// カスり溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_YUKA;	// キャラクタＩＤ
	m_NormalShotDx = 0 * 256;			// ショットの発射座標
	m_NormalShotDy = 0 * 256;			// ショットの発射座標

	m_BombTime = 180;		// ボムの有効時間
}


// デストラクタ //
CYuka::~CYuka()
{
}


// キャラクター固有の描画を行う //
FVOID CYuka::PlayerDraw(int		x256			// Ｘ座標中心
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
		u1 = D3DVAL( 40) / D3DVAL(256);
		v0 = D3DVAL(  0) / D3DVAL(256);
		v1 = D3DVAL( 56) / D3DVAL(256);
	}
	else if(State < 0){
		if(-YUKA_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL( 40) / D3DVAL(256);
			u1 = D3DVAL( 80) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL( 80) / D3DVAL(256);
			u1 = D3DVAL(120) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}
	else{
		if(YUKA_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(120) / D3DVAL(256);
			u1 = D3DVAL(160) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(160) / D3DVAL(256);
			u1 = D3DVAL(200) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}

	dx = (40/2) * 256 * 6 / 7;
	dy = (56/2) * 256 * 6 / 7;

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
FVOID CYuka::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -YUKA_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  YUKA_GST_RANGE) (*pState)++;
	}
}


// エキストラショットを動作させる //
FVOID CYuka::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int				mx, my;			// 座標基本値
	DWORD			Charge;			// 溜め具合
	WORD			KeyCode;		// キーコード
	int				YMin;			// 消去Ｙ座標
	int				i, v, n;

	CExtraShot					*pExShot;	// エキストラショットコンテナ
	CExtraShot::Iterator		it;			// 上の奴のイテレータ
	ExtraShot					*pData;		// 上の奴の指すデータの格納先...

	pExShot = pExShotInfo->pExtraShot;	// エキストラショット格納先
	Charge  = pExShotInfo->Charge;		// 現在の溜め具合
	KeyCode = pExShotInfo->KeyCode;		// キーコード

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// Ｙ消去座標を確定する //
	YMin = pExShot->GetYMin() - (50 * 256);

	// エキストラショットの動作 //
	ForEachFrontPtr(pExShot, YUKA_EXSHOT_NORMAL, it){
		pData = it.GetPtr();

		pData->x += CosL(pData->d, pData->v);	// Ｘ座標の更新
		pData->y += SinL(pData->d, pData->v);	// Ｙ座標の更新

		// 消去すべき座標に突入した場合 //
		if(pData->y < YMin){
			DelContinuePtr(pExShot, it);
		}

		// カウンタを増加させる //
		pData->Count++;
	}

	// エキストラショットの動作 //
	ForEachFrontPtr(pExShot, YUKA_EXSHOT_DELETE, it){
		pData = it.GetPtr();

		// カウンタを減少させる //
		if(pData->Count > 8) pData->Count -= 8;
		else                 DelContinuePtr(pExShot, it);

		pData->x += CosL(pData->d, pData->v);	// Ｘ座標の更新
		pData->y += SinL(pData->d, pData->v);	// Ｙ座標の更新

		// 消去すべき座標に突入した場合 //
		if(pData->y < YMin){
			DelContinuePtr(pExShot, it);
		}
	}

	// ボム・待ち状態の動作 //
	ForEachFrontPtr(pExShot, YUKA_BOMB_WAIT, it){
		pData = it.GetPtr();

		//pData->d += 8;

		if(pData->Count>2) pData->Count-=2;
		else{
			pData->Count = 255;
			pExShot->MoveBack(it, YUKA_BOMB_NORMAL);
		}
	}

	// ボム・通常状態の動作 //
	ForEachFrontPtr(pExShot, YUKA_BOMB_NORMAL, it){
		pData = it.GetPtr();

		pData->x += CosL(pData->d, pData->v);	// Ｘ座標の更新
		pData->y += SinL(pData->d, pData->v);	// Ｙ座標の更新

		pData->d += 1;

		if(pData->Count > 8) pData->Count-= 8;
		else                 DelContinuePtr(pExShot, it);
	}

	// エキストラショットセット用タスク(直進部) //
	ForEachFrontPtr(pExShot, YUKA_EXSHOT_SET, it){
		if(it->Count & 1){
			v = it->v;

			pData = pExShot->InsertBack(YUKA_EXSHOT_NORMAL);
			if(NULL == pData) break;

			pData->x     = mx;
			pData->y     = my;
			pData->d     = -64 - 5 + RndEx() % 11;
			pData->v     = v;
			pData->Count = 0;
		}

		if(it->Count) it->Count--;
		else          DelContinuePtr(pExShot, it);
	}

	// エキストラショットセット用タスク(ワイド部) //
	ForEachFrontPtr(pExShot, YUKA_EXSHOT_SET2, it){
		v = it->v;

		if(it->Count % 2 == 0){
			n = it->d;

			for(i=-n; i<=n; i++){
				pData = pExShot->InsertBack(YUKA_EXSHOT_NORMAL);
				if(NULL == pData) break;

				pData->x     = mx;
				pData->y     = my;
				pData->d     = (BYTE)(-64 + (i * (it->Count-10)) / n);
				pData->v     = v;
				pData->Count = 0;
			}
		}

		if(it->Count) it->Count--;
		else          DelContinuePtr(pExShot, it);
	}


	// ボム花発生用タスク(ワイド部) //
	ForEachFrontPtr(pExShot, YUKA_BOMB_SET, it){
		if(it->Count % 2 == 0){
			n = it->d;

			pData = pExShot->InsertBack(YUKA_BOMB_WAIT);
			if(NULL == pData) break;
			pData->x     = mx + CosL(n, (140-it->Count)<<8);
			pData->y     = my + SinL(n, (140-it->Count)<<8);
			pData->d     = n;
			pData->v     = 256 * 8;
			pData->Count = 32;

			it->d -= 16;
		}

		if(it->Count) it->Count--;
		else          DelContinuePtr(pExShot, it);
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CYuka::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot;
	CExtraShot::Iterator	it;

	DWORD		Damage;
	int			x, y;
	int			Size;

	Damage = 0;				// ダメージをゼロ初期化
	x      = pEnemy->x;		// 敵のＸ座標
	y      = pEnemy->y;		// 敵のＹ座標

	// 当たり判定サイズ(後で変更のこと) //
	Size = YUKA_EXSHOT_SIZE + pEnemy->size;

	ForEachFrontPtr(pExtraShot, YUKA_EXSHOT_NORMAL, it){
		pShot = it.GetPtr();

		if(HitCheckFast(y, (pShot->y), Size)
		&& HitCheckFast(x, (pShot->x), Size)){
			Damage += YUKA_EXSHOT_DAMAGE;

			// ヒット時に消去する //
			pShot->Count = 255;
			pShot->d     = -64 - 16 + RndEx()%32;
			pShot->v   >>= 2;
			pExtraShot->MoveBack(it, YUKA_EXSHOT_DELETE);
		}
	}

	return Damage;
}


// エキストラショットを描画する //
FVOID CYuka::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot					*pExShot;	// エキストラショット
	CExtraShot::Iterator		it;			// 上の奴のイテレータ

	g_pGrp->SetTexture(TextureID);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	// 構造体へのポインタから諸変数を取得する //
	pExShot = pExShotInfo->pExtraShot;	// エキストラショット格納先

	// エキストラショット(移動中)の描画 //
	ForEachFrontPtr(pExShot, YUKA_EXSHOT_NORMAL, it){
		DrawFlowerShot(it.GetPtr(), 255);
	}

	// エキストラショット(消去中)の描画 //
	ForEachFrontPtr(pExShot, YUKA_EXSHOT_DELETE, it){
		DrawFlowerShot(it.GetPtr(), (BYTE)it->Count);
	}

	// ボム(待ち状態)の描画 //
	ForEachFrontPtr(pExShot, YUKA_BOMB_WAIT, it){
		DrawFlowerBomb(it.GetPtr(), 255);
	}

	// ボム(稼動中)の描画 //
	ForEachFrontPtr(pExShot, YUKA_BOMB_NORMAL, it){
		DrawFlowerBomb(it.GetPtr(), it->Count);
	}
}


// 通常の溜め攻撃 //
FVOID CYuka::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	WORD			KeyCode;
	int				mx, my;
	int				n, v;

	// １／２まで溜まっていないので発動不可 //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());	// 現在のＸ座標
	my      = *(pExShot->GetYPointer());	// 現在のＹ座標
	KeyCode = pExShotInfo->KeyCode;

	n = (pExShotInfo->Charge < CGAUGE_LEVEL1) ? 20 : 40;
	v = 256 * 12;

	pData = pExShot->InsertBack(YUKA_EXSHOT_SET);
	if(NULL == pData) return;

	pData->Count = n + 10;
	pData->d     = 0;
	pData->v     = v;
	pData->x     = 0;
	pData->y     = 0;

//	if(n <= 20) return;

	pData = pExShot->InsertBack(YUKA_EXSHOT_SET2);
	if(NULL == pData) return;

	pData->Count = n;
	pData->d     = n / 20;
	pData->v     = v;
	pData->x     = 0;
	pData->y     = 0;
}


// ボムアタック //
FBOOL CYuka::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CYuka::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	return TRUE;
}


// 通常ボム //
FVOID CYuka::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	int				mx, my;

	pBGDraw->SetBlendColor(100, 200, 100, m_BombTime-60);

	pExShot = pExShotInfo->pExtraShot;

	mx = *(pExShot->GetXPointer());	// 現在のＸ座標
	my = *(pExShot->GetYPointer());	// 現在のＹ座標

	pData = pExShot->InsertBack(YUKA_BOMB_SET);
	if(NULL == pData) return;

	pData->Count = m_BombTime-60;
	pData->d     = (BYTE)RndEx();
	pData->v     = 0;
	pData->x     = mx;
	pData->y     = my;
}


FVOID CYuka::DrawFlowerShot(ExtraShot *pExShot, BYTE a)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y;
	int				dx, dy;
	DWORD			c;
	int				t;
	BYTE			d;

	x = ((pExShot->x) >> 8);
	y = ((pExShot->y) >> 8);

	u0 = D3DVAL(256-24) / D3DVAL(256);
	v0 = D3DVAL(     0) / D3DVAL(256);
	u1 = D3DVAL(   256) / D3DVAL(256);
	v1 = D3DVAL(    24) / D3DVAL(256);

	c = RGBA_MAKE(a, a, a, 255);

	t = (14 - (255 - a) / 26) << 8;

	d  = pExShot->d + 64 - 32;
	dx = CosL(d, t) >> 8;
	dy = SinL(d, t) >> 8;

	Set2DPointC(tlv+0, x+dx, y+dy, u0, v0, c);
	Set2DPointC(tlv+1, x+dy, y-dx, u1, v0, c);
	Set2DPointC(tlv+2, x-dx, y-dy, u1, v1, c);
	Set2DPointC(tlv+3, x-dy, y+dx, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


FVOID CYuka::DrawFlowerBomb(ExtraShot *pExShot, BYTE a)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y;
	int				dx, dy;
	DWORD			c;
	int				t;
	BYTE			d;

	x = ((pExShot->x) >> 8);
	y = ((pExShot->y) >> 8);

	u0 = D3DVAL(     0) / D3DVAL(256);
	v0 = D3DVAL(   128) / D3DVAL(256);
	u1 = D3DVAL(    64) / D3DVAL(256);
	v1 = D3DVAL(128+64) / D3DVAL(256);

	c = RGBA_MAKE(a, a, a, 255);

	t = min(32, (32-pExShot->Count)*3) << 8;
	d = pExShot->Count << 2;

	dx = CosL(d, t) >> 8;
	dy = SinL(d, t) >> 8;

	Set2DPointC(tlv+0, x+dx, y+dy, u0, v0, c);
	Set2DPointC(tlv+1, x+dy, y-dx, u1, v0, c);
	Set2DPointC(tlv+2, x-dx, y-dy, u1, v1, c);
	Set2DPointC(tlv+3, x-dy, y+dx, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
