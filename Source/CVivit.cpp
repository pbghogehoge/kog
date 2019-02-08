/*
 *   CVivit.cpp   : びびっとさんの定義
 *
 */

#include "CVivit.h"
#include "Gian2001.h"
#include "SECtrl.h"

#include "RndCtrl.h"



/***** [グローバル変数] *****/
CVivit		g_VivitInfo;			// 主人公の情報格納用クラス


//#define VIV_EXSHOT_WIDTH	(28*256)	// エキストラショットの幅
//#define VIV_EXSHOT_HEIGHT	(50*256)	// エキストラショットの高さ
//#define VIV_EXSHOT_WIDTH	(32*256)	// エキストラショットの幅
//#define VIV_EXSHOT_HEIGHT	(32*256)	// エキストラショットの高さ

/***** [ 定数 ] *****/
#define VIV_EXSHOT_NORMAL	0x00		// エキストラショットのＩＤ
#define VIV_EXSHOT_BOMB01	0x01		// ボム・第１段階(発射待ち)
#define VIV_EXSHOT_BOMB02	0x02		// ボム・第２段階(移動中)
#define VIV_EXSHOT_BOMB03	0x03		// ボム・その他の謎爆発

#define VIV_BOMB02_SPEED	(18*256)	// ボム用星のスピード
#define VIV_BOMB02_COUNT	70			// ボム、移動完了カウント

#define VIV_EXSHOT_SPEED	(10*256)	// エキストラショットのスピード
#define VIV_EXSHOT_WIDTH	(24*256)	// エキストラショットの幅
#define VIV_EXSHOT_HEIGHT	(24*256)	// エキストラショットの高さ

#define VIV_EXSHOT_DAMAGE	(256+96)				// エキストラショットのダメージ
//#define VIV_EXSHOT_DAMAGE	256					// エキストラショットのダメージ
#define VIV_BOMB_DAMAGE		24					// ボムのダメージ
#define VIV_EXSHOT_SIZE		VIV_EXSHOT_WIDTH	// エキストラショットのサイズ

#define VIV_GST_RANGE			4
//#define VIV_GST_CENTER		0x00		// 中央表示のグラフィック
//#define VIV_GST_LEFT1		0x01		// 左移動のグラフィック
//#define VIV_GST_RIGHT1		0x02		// 右移動のグラフィック



// コンストラクタ //
CVivit::CVivit()
{
	m_NormalSpeed = 4 * 256 - 64;		// 通常時の早さ
	m_ShiftSpeed  = 3 * 256 / 2;		// シフト移動時の早さ
	m_ChargeSpeed = 150 * 256;			// 溜めの早さ
//	m_BuzzSpeed   = 65536;				// カスり溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_VIVIT;	// キャラクタＩＤ
	m_NormalShotDx =  0 * 256;			// ショットの発射座標
	m_NormalShotDy = -4 * 256;			// ショットの発射座標

	m_BombTime = 180;		// ボムの有効時間
}


// デストラクタ //
CVivit::~CVivit()
{
}


// 描画する //
FVOID CVivit::PlayerDraw(
						int		x256			// Ｘ座標中心
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

	// 描画状態により、関数の振り分けを行う //
	if(0 == State){
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 24) / D3DVAL(256);
		v0 = D3DVAL(  0) / D3DVAL(256);
		v1 = D3DVAL( 56) / D3DVAL(256);
	}
	else if(State < 0){
		if(-VIV_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL( 24) / D3DVAL(256);
			u1 = D3DVAL( 48) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL( 48) / D3DVAL(256);
			u1 = D3DVAL( 72) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}
	else{
		if(VIV_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(72) / D3DVAL(256);
			u1 = D3DVAL(96) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(56) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(96)  / D3DVAL(256);
			u1 = D3DVAL(120) / D3DVAL(256);
			v0 = D3DVAL(0)   / D3DVAL(256);
			v1 = D3DVAL(56)  / D3DVAL(256);
		}
	}
/*	switch(State){
		case VIV_GST_CENTER:
			u0 = D3DVAL(32) / D3DVAL(256);
			u1 = D3DVAL(64) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(64) / D3DVAL(256);
		break;

		case VIV_GST_LEFT1:
			u0 = D3DVAL(0)  / D3DVAL(256);
			u1 = D3DVAL(32) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(64) / D3DVAL(256);
		break;

		case VIV_GST_RIGHT1:
			u0 = D3DVAL(64) / D3DVAL(256);
			u1 = D3DVAL(96) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(64) / D3DVAL(256);
		break;
	}
*/
	dx = 12 * 256 * 9 / 10;
	dy = 28 * 256 * 9 / 10;

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
FVOID CVivit::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -VIV_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  VIV_GST_RANGE) (*pState)++;
	}
}


// エキストラショットを動作させる //
FVOID CVivit::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int				mx, my;			// 座標基本値
	DWORD			Charge;			// 溜め具合
	WORD			KeyCode;		// キーコード
	int				YMin;			// 消去Ｙ座標
	int				j;
//	int				t;				// 残りカウント計測用
	BYTE			d;				// 角度ひねり用
	BOOL			bNeedDelete;	// 消去要求

	CExtraShot					*pExShot;	// エキストラショット
	CExtraShot::Iterator		it;			// 上の奴のイテレータ
	ExtraShot					*pData;		// 上の奴の指すデータの格納先...
	ExtraOption					*pOpt;		// きらきらおほしさま

	pExShot = pExShotInfo->pExtraShot;	// エキストラショット格納先
	Charge  = pExShotInfo->Charge;		// 現在の溜め具合
	KeyCode = pExShotInfo->KeyCode;		// キーコード

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// Ｙ消去座標を確定する //
	YMin = pExShot->GetYMin() - (50 * 256);

	// エキストラショットの動作 //
	ForEachFrontPtr(pExShot, VIV_EXSHOT_NORMAL, it){
		pData = it.GetPtr();

		// キーコードに従い、発射方向を変更 //
		if(pData->Count & 1){
			d = pData->d;		// 基本角は上方向
			if(KeyCode & KEY_LEFT){
				// 左にちょっと曲げる //
				if(d > (196-48)) pData->d-=1;
			}
			if(KeyCode & KEY_RIGHT){
				// 右にちょっと曲げる //
				if(d < (196+48)) pData->d+=1;
			}
		}

		pData->x += CosL(pData->d, pData->v);	// Ｘ座標の更新
		pData->y += SinL(pData->d, pData->v);	// Ｙ座標の更新

		// 消去すべき座標に突入した場合 //
		if(pData->y < YMin){
			bNeedDelete = TRUE;
		}

		for(j=0; j<EXTRAOPTION_MAX; j++){
			pOpt = pData->Option + j;

			if(pOpt->a){
				bNeedDelete = FALSE;

				if(0 == pOpt->count){
					pOpt->x += CosL(pOpt->d, pOpt->a * 8);
					pOpt->y += SinL(pOpt->d, pOpt->a * 8);

					pOpt->a = max(0, (int)pOpt->a - 12);
				}
				else{
					pOpt->x = pData->x;
					pOpt->y = pData->y;
					pOpt->count--;

					if(0 == pOpt->count) PlaySE(SNDID_TRI_STAR_SMALL, pData->x);
				}
			}
		}

		if(bNeedDelete){
			DelContinuePtr(pExShot, it);
		}

		// カウンタを増加させる //
		pData->Count++;
	}

	// ボム・初期状態の動作 //
	ForEachFrontPtr(pExShot, VIV_EXSHOT_BOMB01, it){
		pData = it.GetPtr();

		if(pData->Count) pData->Count--;
		else{
			pExShot->MoveBack(it, VIV_EXSHOT_BOMB02);
		}
	}

	// ボム・移動中 //
	ForEachFrontPtr(pExShot, VIV_EXSHOT_BOMB02, it){
		pData = it.GetPtr();

		pData->x += CosL(pData->d, pData->v);	// Ｘ座標の更新
		pData->y += SinL(pData->d, pData->v);	// Ｙ座標の更新
		pData->d += 1;

		for(j=0; j<EXTRAOPTION_MAX; j++){
			pOpt = pData->Option + j;

			if(pOpt->a){
				if(0 == pOpt->count){
					pOpt->x += CosL(pOpt->d, 256);//pOpt->a * 8);
					pOpt->y += SinL(pOpt->d, 256);//pOpt->a * 8);

					pOpt->a = max(0, (int)pOpt->a - 10/2);
				}
				else{
				pOpt->x = pData->x;
					pOpt->y = pData->y;

					pOpt->count--;
				}
			}
			else{
				pOpt->a     = 255;//0;
				pOpt->d     = pData->d + 128*0 - 32 + RndEx()%64;//(BYTE)(RndEx()>>1);
				pOpt->count = VIV_BOMB02_COUNT / 2 / EXTRAOPTION_MAX;
				pOpt->x = pData->x;
				pOpt->y = pData->y;
			}
/*

			pOpt->x = pData->x;
			pOpt->y = pData->y;

			t = pOpt->count;
			if(t){
				pOpt->x += CosL(pOpt->d, t * 2048);
				pOpt->y += SinL(pOpt->d, t * 2048);
				//pOpt->d -= 2;
				pOpt->a = max(0, (255 - pOpt->count*5));
				pOpt->count--;	// こちらのカウンタは逆方向へと
			}
*/
		}

		if(pData->Count >= VIV_BOMB02_COUNT){
			DelContinuePtr(pExShot, it);
		}

		// カウンタを増加させる //
		pData->Count++;
	}

	// ボム・補助爆発 //
	ForEachFrontPtr(pExShot, VIV_EXSHOT_BOMB03, it){
		pData = it.GetPtr();

		bNeedDelete = TRUE;

		for(j=0; j<EXTRAOPTION_MAX; j++){
			pOpt = pData->Option + j;

			if(pOpt->a){
				bNeedDelete = FALSE;

				if(0 == pOpt->count) pOpt->a = max(0, (int)pOpt->a - 10/2);
				else                 pOpt->count--;
			}
		}

		if(bNeedDelete){
			DelContinuePtr(pExShot, it);
		}
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CVivit::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
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
	Size = VIV_EXSHOT_SIZE + pEnemy->size;

	ForEachFrontPtr(pExtraShot, VIV_EXSHOT_NORMAL, it){
		pShot = it.GetPtr();

		// びびっとさんの弾は、貫通弾なので、消去は行わない //
		// なお、後で８角形判定に変更すること               //
		if(HitCheckFast(y, (pShot->y), Size)
		&& HitCheckFast(x, (pShot->x), Size)){
			Damage += VIV_EXSHOT_DAMAGE;
		}
	}

	// ボムの描画(爆発) : 全ての敵に一定ダメージ //
	ForEachFrontPtr(pExtraShot, VIV_EXSHOT_BOMB03, it){
		return Damage + VIV_BOMB_DAMAGE;
	}

	return Damage;
}


// エキストラショットを描画する //
FVOID CVivit::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	int							mx, my;		// 座標基本値
	DWORD						Charge;		// 溜め具合
	WORD						KeyCode;	// キーコード
	BYTE						d;			// 移動方向

	CExtraShot					*pExShot;	// エキストラショットコンテナ
	CExtraShot::Iterator		it;			// 上の奴のイテレータ

	D3DTLVERTEX		tlv[20];			// 描画データ
	D3DVALUE		u0, v0, u1, v1;		// テクスチャ座標
	DWORD			c;
	int				x, y;				// 描画座標値
	int				x256, y256;			// 基本座標値
	int				dx, dy;				// 回転用
	int				w, h;				// 幅＆高さ


	// 構造体へのポインタから諸変数を取得する //
	pExShot = pExShotInfo->pExtraShot;	// エキストラショット格納先
	Charge  = pExShotInfo->Charge;		// 現在の溜め具合
	KeyCode = pExShotInfo->KeyCode;		// キーコード

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// テクスチャ座標をセットする //
	u0 = D3DVAL(0)   / D3DVAL(256);
	u1 = D3DVAL(64)  / D3DVAL(256);
	v0 = D3DVAL(136) / D3DVAL(256);
	v1 = D3DVAL(200) / D3DVAL(256);
//	u0 = D3DVAL(0)   / D3DVAL(256);
//	u1 = D3DVAL(56)  / D3DVAL(256);
//	v0 = D3DVAL(160) / D3DVAL(256);
//	v1 = D3DVAL(256) / D3DVAL(256);

	// 使用するテクスチャとレンダリングステートの初期化 //
	g_pGrp->SetTexture(TextureID);				// テクスチャ
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);	// 描画ステート


	// エキストラショットの描画 //
	ForEachFrontPtr(pExShotInfo->pExtraShot, VIV_EXSHOT_NORMAL, it){
		x256 = it->x;					// Ｘ座標
		y256 = it->y;					// Ｙ座標
		d    = (BYTE)(it->Count * 7);	// 角度

		w  = min(VIV_EXSHOT_WIDTH,  it->Count * 512);
		h  = min(VIV_EXSHOT_HEIGHT, it->Count * 512);
		dx = CosL(d, h) + CosL(d-64, w);
		dy = SinL(d, h) + SinL(d-64, w);
		c  = RGBA_MAKE(255, 255, 255, 255);

		x = (x256 + dx) >> 8;
		y = (y256 + dy) >> 8;
		Set2DPointC(tlv+0, x, y, u0, v0, c);

		x = (x256 - dx) >> 8;
		y = (y256 - dy) >> 8;
		Set2DPointC(tlv+2, x, y, u1, v1, c);

		dx = CosL(d, h) + CosL(d+64, w);
		dy = SinL(d, h) + SinL(d+64, w);

		x = (x256 + dx) >> 8;
		y = (y256 + dy) >> 8;
		Set2DPointC(tlv+1, x, y, u1, v0, c);

		x = (x256 - dx) >> 8;
		y = (y256 - dy) >> 8;
		Set2DPointC(tlv+3, x, y, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		// 小さいお星様を描く //
		DrawSmallStar(it->Option);
	}

	// ボムの描画(星) //
	ForEachFrontPtr(pExShotInfo->pExtraShot, VIV_EXSHOT_BOMB02, it){
		DrawBombStar(it->Option);
	}

	// ボムの描画(爆発) //
	ForEachFrontPtr(pExShotInfo->pExtraShot, VIV_EXSHOT_BOMB03, it){
		DrawExBomb(it->Option);
	}
}


// 通常の溜め攻撃(レベル１) //
FVOID CVivit::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
	WORD			KeyCode;
	int				mx, my;
	int				i, j;
	int				d;
	int				n;

	// この場合は発射できません //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	// ExShot 格納先のポインタを取得 //
	pExShot = pExShotInfo->pExtraShot;

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// キーコードを取得する //
	KeyCode = pExShotInfo->KeyCode;

	// ショットを撃ち分ける //
	d = 6;							// 基本差分角
	if(KeyCode & KEY_DOWN) d-=4;	// 密集系ショット
	if(KeyCode & KEY_UP)   d+=10;	// 拡散系ショット

	if(pExShotInfo->Charge < CGAUGE_LEVEL1){
		i = 1;
		n = 2;
	}
	else{
		i = 0;
		n = 3;
	}

	PlaySE(SNDID_TRI_STAR_LARGE, mx);

	for(; i<n; i++){
		pData   = pExShot->InsertBack(VIV_EXSHOT_NORMAL);
		if(NULL == pData) return;	// 空きが無い

		// データを初期化する //
		pData->x      = mx;					// 初期Ｘ座標
		pData->y      = my;					// 初期Ｙ座標
		pData->d      = -64 + i*d - d*1;	// 発射角
		pData->v      = VIV_EXSHOT_SPEED;	// 初速度
		pData->Count  = 0;					// カウンタ

		for(j=0; j<EXTRAOPTION_MAX; j++){
			pOpt = pData->Option + j;

			pOpt->d = (128 + pData->d) - 9 + RndEx()%19;
			pOpt->x = mx;
			pOpt->y = my;
			pOpt->a = 255;
			pOpt->count = RndEx() % 60;
		}
	}
}


// ボムアタック(レベル１) //
FBOOL CVivit::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CVivit::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	CExtraShot::Iterator		it;

	// ボムの描画(爆発) //
	ForEachFrontPtr(pExtraShot, VIV_EXSHOT_BOMB03, it){
		return FALSE;
	}

	return TRUE;
}


// 通常ボム //
FVOID CVivit::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	pBGDraw->SetBlendColor(168, 168, 255, m_BombTime-60);

	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
//	int				mx, my;
	int				width, ox;
	int				i, j;
//	BYTE			d;

	const int BombStarMax = 24;

	pExShot = pExShotInfo->pExtraShot;

	// 座標値を取得する //
	ox    = pBGDraw->GetTargetOx();			// 画面中央
	width = pBGDraw->GetTargetWidth() / 2;	// 画面幅 / ２
//	mx = *(pExShot->GetXPointer());			// 現在のＸ座標を取得
//	my = *(pExShot->GetYPointer());			// 現在のＹ座標を取得

	// その他爆発(仮 をセット //
	pData = pExShot->InsertBack(VIV_EXSHOT_BOMB03);
	if(NULL == pData) return;	// 空きが無い
	for(j=0; j<EXTRAOPTION_MAX; j++){
		pOpt = pData->Option + j;
		pOpt->a     = 255;
		pOpt->d     = (BYTE)RndEx()%96 + 16;
		pOpt->count = RndEx()%(m_BombTime-60);//(j * m_BombTime) / EXTRAOPTION_MAX;
		pOpt->x     = ox - width + (RndEx()%(width/128))*256;
		pOpt->y     = (RndEx()%380 + 50) * 256;
	}

	for(i=0; i<BombStarMax; i++){
		pData   = pExShot->InsertBack(VIV_EXSHOT_BOMB01);
		if(NULL == pData) return;	// 空きが無い

		pData->Count = i * (m_BombTime - VIV_BOMB02_COUNT/2) / BombStarMax;
		pData->y     = (RndEx() % 380 + 50) * 256;
		pData->v     = VIV_BOMB02_SPEED;

		if(i & 1){
			pData->x = ox - width - 16 * 256;
			if(pData->y < 240 * 256) pData->d =  8 + RndEx()%48;
			else                     pData->d = -8 - RndEx()%48;
		}
		else{
			pData->x = ox + width + 16 * 256;
			if(pData->y < 240 * 256) pData->d = 128 - 8 - RndEx()%48;
			else                     pData->d = 128 + 8 + RndEx()%48;
		}

		// 放出される星屑 //
		for(j=0; j<EXTRAOPTION_MAX; j++){
			pOpt = pData->Option + j;
			pOpt->a     = 255;//0;
			pOpt->d     = pData->d + 128*0 - 32 + RndEx()%64;//(BYTE)(RndEx()>>1);
			pOpt->count = (j * VIV_BOMB02_COUNT/2) / EXTRAOPTION_MAX;
			pOpt->x     = pData->x;
			pOpt->y     = pData->y;
		}
	}
}


// 小さい「お星様」の描画 //
FVOID CVivit::DrawSmallStar(ExtraOption *pOpt)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
	DWORD			c;
	BYTE			a;
	int				x, y;				// 描画座標値
	int				i;
	int				dx, dy;				// 回転用
	int				w;//, h;				// 幅＆高さ

	const D3DVALUE	u0 = D3DVAL(0)   / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(64)  / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(136) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(200) / D3DVAL(256);

	// きらきらおほしさまの描画 //
	for(i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
		if((pOpt->count) || (0 == pOpt->a)) continue;

		a  = pOpt->a;
		c  = RGBA_MAKE(a, a, a, 255);
		x  = pOpt->x >> 8;
		y  = pOpt->y >> 8;
		w  = a * 12;
		dx = CosL(a, w) >> 8;
		dy = SinL(a, w) >> 8;

		Set2DPointC(tlv+0, x+dx, y+dy, u0, v0, c);
		Set2DPointC(tlv+1, x-dy, y+dx, u1, v0, c);
		Set2DPointC(tlv+2, x-dx, y-dy, u1, v1, c);
		Set2DPointC(tlv+3, x+dy, y-dx, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// 小さい星描画(ボム用) //
FVOID CVivit::DrawBombStar(ExtraOption *pOpt)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
	DWORD			c;
	BYTE			a;
	int				x, y;				// 描画座標値
	int				i;
	int				dx, dy;				// 回転用
	int				w;//, h;			// 幅＆高さ

	const D3DVALUE	u0 = D3DVAL(0)   / D3DVAL(256);
	const D3DVALUE	u1 = D3DVAL(64)  / D3DVAL(256);
	const D3DVALUE	v0 = D3DVAL(136) / D3DVAL(256);
	const D3DVALUE	v1 = D3DVAL(200) / D3DVAL(256);

	// きらきらおほしさまの描画 //
	for(i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
		a  = pOpt->a;
		x  = (pOpt->x) >> 8;
		y  = (pOpt->y) >> 8;
		w  = a * 30;
		dx = CosL(a, w) >> 8;
		dy = SinL(a, w) >> 8;

		if(a <= 196){
			a = (255 * a)/196;
		}
		else{
			a = 255 - 255 * (a - 196) / (255-196);
		}
//		a = 255min(255, abs(128 - a) << 1);
		c = RGBA_MAKE(a, a, a, 255);

		Set2DPointC(tlv+0, x+dx, y+dy, u0, v0, c);
		Set2DPointC(tlv+1, x-dy, y+dx, u1, v0, c);
		Set2DPointC(tlv+2, x-dx, y-dy, u1, v1, c);
		Set2DPointC(tlv+3, x+dy, y-dx, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// ボム用爆発描画 //
FVOID CVivit::DrawExBomb(ExtraOption *pOpt)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
	DWORD			c;
	BYTE			a;
	int				x, y;				// 描画座標値
	int				i;
//	int				dx, dy;				// 回転用
	int				w;//, h;			// 幅＆高さ
/*
#define TEXTURE_ID_ATKEFC	0x1f	// 攻撃予告エフェクト用テクスチャ
#define TEXTURE_ID_BONUS	0x20	// ボーナス表示エフェクト用テクスチャ
#define TEXTURE_ID_GUARD	0x21	// ガードゲージ用テクスチャ
#define TEXTURE_ID_SHIELD	0x22	// シールド用テクスチャ
*/
	g_pGrp->SetTexture(TEXTURE_ID_SHIELD);	// テクスチャ切り替え
//	g_pGrp->SetTexture(TEXTURE_ID_ATKEFC);	// テクスチャ切り替え

	// 爆発まるの描画 //
	for(i=0; i<EXTRAOPTION_MAX; i++, pOpt++){
		if(pOpt->count) continue;

		a = max(0, 255-abs(pOpt->a - 128)*2);
		x  = (pOpt->x) >> 8;
		y  = (pOpt->y) >> 8;
//		w  = (pOpt->d) * (pOpt->a) / 255;
		w  = (pOpt->d) * min(255, (pOpt->a * 2)-255) / 255;

		c = RGBA_MAKE(a, a, a, 255);

		Set2DPointC(tlv+0, x+w, y+w, 0.0, 0.0, c);
		Set2DPointC(tlv+1, x-w, y+w, 1.0, 0.0, c);
		Set2DPointC(tlv+2, x-w, y-w, 1.0, 1.0, c);
		Set2DPointC(tlv+3, x+w, y-w, 0.0, 1.0, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
