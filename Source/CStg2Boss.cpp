/*
 *   CStg2Boss.cpp   : めい＆まいの定義
 *
 */

#include "CStg2Boss.h"
#include "Gian2001.h"
#include "CEnemyCtrl.h"
#include "SECtrl.h"

#include "RndCtrl.h"



/***** [ 定数 ] *****/
#define STG2_EXSHOT_NORMAL		0x00	// 通常動作モード
#define STG2_EXSHOT_DELETE		0x01	// 消去モード
#define STG2_BOMB_SUBLASER		0x02	// ボム用・サブレーザー
#define STG2_BOMB_BLACKBOX		0x03	// 漆黒の棺桶
#define STG2_BOMB_EXPLOSION		0x04	// 秘密の爆発

#define STG2_EXSHOT_DAMAGE		(256 / 4)	// ツインレーザーのダメージ
#define STG2_SUBLASER_DAMAGE	(256 / 32)	// ボムのサブレーザーのダメージ
#define STG2_BLACKBOX_DAMAGE	(256 / 4)	// 漆黒の棺桶のダメージ

#define STG2_BLACKBOX_COUNT		(60*2)		// 棺桶の追加持続時間

#define	STG2_EXSHOT_DY			(-4 * 256)	// 自機中心からのズレ
#define STG2_EXSHOT_WIDTH		( 5 * 256)	// ツインレーザーの太さ

/*
#define STG2_GST_CENTER			0x00
#define STG2_GST_LEFT1			0x01
#define STG2_GST_RIGHT1			0x02
*/
#define STG2_GST_RANGE			4



/***** [グローバル変数] *****/
CStg2Boss		g_Stg2BossInfo;			// 主人公の情報格納用クラス



// コンストラクタ //
CStg2Boss::CStg2Boss()
{
	m_NormalSpeed = 5 * 256;		// 通常時の早さ
	m_ShiftSpeed  = 3 * 256 / 2;	// シフト移動時の早さ
	m_ChargeSpeed = 256 * 256;		// 溜めの早さ
//	m_BuzzSpeed   = 65536;			// カスり溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_STG2;	// キャラクタＩＤ
	m_NormalShotDx = 0;					// ショットの発射座標
	m_NormalShotDy = 0;			// ショットの発射座標

	m_BombTime = 100;		// ボムの有効時間
}


// デストラクタ //
CStg2Boss::~CStg2Boss()
{
}


// 描画する //
FVOID CStg2Boss::PlayerDraw(
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

/*
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	int				x, y;
	int				dx, dy;
	DWORD			c;
*/
	c  = RGBA_MAKE(255, 255, 255, 245);

	if(0 == State){
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 55) / D3DVAL(256);
		v0 = D3DVAL(  0) / D3DVAL(256);
		v1 = D3DVAL( 40) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG2_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 55) / D3DVAL(256);
			v0 = D3DVAL( 40) / D3DVAL(256);
			v1 = D3DVAL( 80) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 55) / D3DVAL(256);
			v0 = D3DVAL( 80) / D3DVAL(256);
			v1 = D3DVAL(120) / D3DVAL(256);
		}
	}
	else{
		if(STG2_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 55) / D3DVAL(256);
			v0 = D3DVAL(120) / D3DVAL(256);
			v1 = D3DVAL(160) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 55) / D3DVAL(256);
			v0 = D3DVAL(160) / D3DVAL(256);
			v1 = D3DVAL(200) / D3DVAL(256);
		}
	}
/*

	if(0 == State){
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 64) / D3DVAL(256);
		v0 = D3DVAL(  0) / D3DVAL(256);
		v1 = D3DVAL( 48) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG2_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 64) / D3DVAL(256);
			v0 = D3DVAL( 48) / D3DVAL(256);
			v1 = D3DVAL( 96) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 64) / D3DVAL(256);
			v0 = D3DVAL( 96) / D3DVAL(256);
			v1 = D3DVAL(144) / D3DVAL(256);
		}
	}
	else{
		if(STG2_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 64) / D3DVAL(256);
			v0 = D3DVAL(144) / D3DVAL(256);
			v1 = D3DVAL(192) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 64) / D3DVAL(256);
			v0 = D3DVAL(192) / D3DVAL(256);
			v1 = D3DVAL(240) / D3DVAL(256);
		}
	}


*/
	// 4:3
	dx = 20 * 256;
	dy = 15 * 256;
//	dx = 20 * 256;
//	dy = 15 * 256;

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


/*
	c  = RGBA_MAKE(255, 255, 255, 245);

	// 描画状態により、関数の振り分けを行う //
	switch(State){
		case STG2_GST_CENTER:
			u0 = D3DVAL(0)  / D3DVAL(256);
			u1 = D3DVAL(72) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(40) / D3DVAL(256);
		break;

		case STG2_GST_RIGHT1:
			u0 = D3DVAL(0)  / D3DVAL(256);
			u1 = D3DVAL(72) / D3DVAL(256);
			v0 = D3DVAL(48) / D3DVAL(256);
			v1 = D3DVAL(88) / D3DVAL(256);
		break;

		case STG2_GST_LEFT1:
			u0 = D3DVAL(0)   / D3DVAL(256);
			u1 = D3DVAL(72)  / D3DVAL(256);
			v0 = D3DVAL(96)  / D3DVAL(256);
			v1 = D3DVAL(136) / D3DVAL(256);
		break;
	}

//	u0 = D3DVAL(0)  / D3DVAL(256);
//	u1 = D3DVAL(56) / D3DVAL(256);
//	v0 = D3DVAL(0)  / D3DVAL(256);
//	v1 = D3DVAL(40) / D3DVAL(256);

	dx = 23 * 256;
	dy = 13 * 256;

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
/*
	x = (x256 - 23 * 256) >> 8;
	y = (y256 - 13 * 256) >> 8;
	Set2DPointC(tlv+0, x, y, u0, v0, c);

	x = (x256 + 23 * 256) >> 8;
	y = (y256 - 13 * 256) >> 8;
	Set2DPointC(tlv+1, x, y, u1, v0, c);

	x = (x256 + 23 * 256) >> 8;
	y = (y256 + 13 * 256) >> 8;
	Set2DPointC(tlv+2, x, y, u1, v1, c);

	x = (x256 - 23 * 256) >> 8;
	y = (y256 + 13 * 256) >> 8;
	Set2DPointC(tlv+3, x, y, u0, v1, c);
*/
/*
	x = (x256 - 18 * 256) >> 8;
	y = (y256 - 13 * 256) >> 8;
	Set2DPointC(tlv+0, x, y, u0, v0, c);

	x = (x256 + 18 * 256) >> 8;
	y = (y256 - 13 * 256) >> 8;
	Set2DPointC(tlv+1, x, y, u1, v0, c);

	x = (x256 + 18 * 256) >> 8;
	y = (y256 + 13 * 256) >> 8;
	Set2DPointC(tlv+2, x, y, u1, v1, c);

	x = (x256 - 18 * 256) >> 8;
	y = (y256 + 13 * 256) >> 8;
	Set2DPointC(tlv+3, x, y, u0, v1, c);
*/
//	if(bDamaged) g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
//	else         g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
//
//	g_pGrp->SetTexture(TextureID);
//	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 描画用のＩＤを変更する //
FVOID CStg2Boss::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -STG2_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  STG2_GST_RANGE) (*pState)++;
	}

/*	int			dir = 0;


	if(KeyCode & KEY_LEFT)  dir--;
	if(KeyCode & KEY_RIGHT) dir++;

	if(dir){
		if(dir < 0) *pState = STG2_GST_LEFT1;
		else        *pState = STG2_GST_RIGHT1;
	}
	else{
		*pState = STG2_GST_CENTER;
	}
*/
}


// エキストラショットを動作させる //
FVOID CStg2Boss::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my;			// 座標基本値
	int			ox, oy;			// エフェクト中心座標
	DWORD		Charge;			// 溜め具合
	BYTE		d;				// 角度ひねり用
	BOOL		bNeedDelete;	// 消去要求があれば真
	int			i;

	CExtraShot					*pExShot;	// エキストラショットコンテナ
	CExtraShot::Iterator		it;			// 上の奴のイテレータ
	ExtraShot					*pData;		// 上の奴の指すデータの格納先...
	ExtraOption					*pOpt;		// 付随するデータ

	// 構造体へのポインタから諸変数を取得する //
	pExShot = pExShotInfo->pExtraShot;	// エキストラショット格納先
	Charge  = pExShotInfo->Charge;		// 現在の溜め具合

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// キャラクタの傾きに合わせて、レーザーを傾かせる //
	i = pExShotInfo->State;
	if(i){
		if(i > 0) d = ( STG2_GST_RANGE == i) ? 30 : 15;
		else      d = (-STG2_GST_RANGE == i) ? 30 : 15;
	}
	else{
		d = 0;
	}

	// 展開 or 状態維持状態のレーザーの処理 //
	ForEachFrontPtr(pExShot, STG2_EXSHOT_NORMAL, it){
		pData = it.GetPtr();

		if(0 == pData->Count){
			pExShot->MoveBack(it, STG2_EXSHOT_DELETE);
			continue;
		}

		// キャラクタの動きに追従させる //
		pData->x = mx + CosL(d, ((char)pData->d) << 8);
		pData->y = my + STG2_EXSHOT_DY;

		// レーザーを展開する //
		pData->v = min(pData->v + 128, STG2_EXSHOT_WIDTH);

		// カウンタを減少させる //
		pData->Count--;
	}

	// 消去中のレーザーの処理 //
	ForEachFrontPtr(pExShot, STG2_EXSHOT_DELETE, it){
		pData = it.GetPtr();

		// キャラクタの動きに追従させる //
		pData->x  = mx + CosL(d, ((char)pData->d) << 8);
		pData->y  = my + STG2_EXSHOT_DY;

		// 徐々に細くする //
		pData->v -= 128;

		if(pData->v <= 0){
			DelContinuePtr(pExShot, it);
		}
	}

	// ボム用サブレーザー //
	ForEachFrontPtr(pExShot, STG2_BOMB_SUBLASER, it){
		pData = it.GetPtr();

		// 待ち状態 //
		if(pData->Count){
			pData->Count--;
		}
		// オープン＆クローズ //
		else if(pData->d){
			if(     pData->d > 256- 8) pData->v += (512*2);	// open
			else if(pData->d > 256-18) pData->v -= (256*2-128);	// ややclose
			else if(pData->d < 32){
				pData->v = max(0, pData->v - 128);	// close
			}

			pData->d = max(0, pData->d - 4);
		}
		// タスクリストから削除 //
		else{
			DelContinuePtr(pExShot, it);
		}
	}

	// ボム用・黒い棺 //
	ForEachFrontPtr(pExShot, STG2_BOMB_BLACKBOX, it){
		pData = it.GetPtr();

		// 表示中 //
		if(pData->Count < m_BombTime + STG2_BLACKBOX_COUNT){
			if(pData->Count < 40){
				pData->v = min(30 * 256, pData->v + 256 * 2);
				pData->d = min(255, pData->d + 8);
			}
			else if(pData->Count > m_BombTime + STG2_BLACKBOX_COUNT - 70 - 60){
				pData->v = max(0, pData->v - 128);
				pData->d = max(0, pData->d - 12);
			}
//			else if(pData->Count > m_BombTime + STG2_BLACKBOX_COUNT - 70 - 15){
//				pData->d = max(0, pData->d - 12);
//			}

			// 領域内の敵弾をクリアする //
			pEnemy->GetEnemyTamaPtr()->ClearFromRect(
				pData->x, 0, pData->v, 480*256);

			pData->Count++;
		}
		// 消去 //
		else{
			DelContinuePtr(pExShot, it);
		}
	}

	// ボム用・下から迫り来る爆発 //
	ForEachFrontPtr(pExShot, STG2_BOMB_EXPLOSION, it){
		pData = it.GetPtr();

		ox = pData->x;
		oy = pData->y;

		bNeedDelete = TRUE;

		for(i=0; i<EXTRAOPTION_MAX; i++){
			pOpt = pData->Option + i;

			if(pOpt->a){
				bNeedDelete = FALSE;

				if(0 == pOpt->count){
					pOpt->a  = max(0, (int)pOpt->a - 10/2);
					pOpt->x += (pOpt->x - ox) / 24;
					pOpt->y += (pOpt->y - oy) / 24;
				}
				else{
					pOpt->count--;
				}
			}
		}

		if(bNeedDelete){
			DelContinuePtr(pExShot, it);
		}
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CStg2Boss::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot;
	CExtraShot::Iterator	it;

	DWORD		Damage;
	int			x, y, i;
	int			Size;

	Damage = 0;				// ダメージをゼロ初期化
	x      = pEnemy->x;		// 敵のＸ座標
	y      = pEnemy->y;		// 敵のＹ座標

	// 当たり判定サイズ(後で変更のこと) //
	Size = STG2_EXSHOT_WIDTH + pEnemy->size;

	for(i=STG2_EXSHOT_NORMAL; i<=STG2_EXSHOT_DELETE; i++){
		ForEachFrontPtr(pExtraShot, i, it){
			pShot = it.GetPtr();

			if((y < pShot->y) && HitCheckFast(x, (pShot->x), Size)){
				Damage += STG2_EXSHOT_DAMAGE;
			}
		}
	}

	// ボム：サブレーザー //
	ForEachFrontPtr(pExtraShot, STG2_BOMB_SUBLASER, it){
		pShot = it.GetPtr();
		Size = pEnemy->size + pShot->v;

		if(HitCheckFast(x, pShot->x, Size)){
			Damage += STG2_SUBLASER_DAMAGE;
		}
	}

	// ボム：棺桶 //
	ForEachFrontPtr(pExtraShot, STG2_BOMB_BLACKBOX, it){
		pShot = it.GetPtr();
		Size = pEnemy->size + pShot->v;

		if(HitCheckFast(x, pShot->x, Size)){
			Damage += STG2_BLACKBOX_DAMAGE;
		}
	}
	return Damage;
}


// エキストラショットを描画する //
FVOID CStg2Boss::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot					*pExShot;	// エキストラショットコンテナ
	CExtraShot::Iterator		it;			// 上の奴のイテレータ

	ExtraOption		*pOpt;
	D3DTLVERTEX		tlv[20];		// 描画データ
	D3DVALUE		u1, v1, u2, v2;	// テクスチャ座標
	DWORD			c, c2;			// 基本色
	int				i;
	int				x256, y256;		// 基本座標値
	int				x, y;
	int				dx;				// 太さ反映用
	BYTE			a;

	// エキストラショット格納先を取得 //
	pExShot = pExShotInfo->pExtraShot;


	// 使用するテクスチャとレンダリングステートの初期化 //
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);		// 弾から借りて来ましょう
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);	// 描画ステート

//	c  = RGBA_MAKE(200, 190, 255, 255);
	c  = RGBA_MAKE(255, 255, 255, 255);

	for(i=STG2_EXSHOT_NORMAL; i<=STG2_EXSHOT_DELETE; i++){
		ForEachFrontPtr(pExShotInfo->pExtraShot, i, it){
			// 基本となる座標＆角度をセット //
			x256 = it->x;		// Ｘ座標
			y256 = it->y;		// Ｙ座標
			dx   = it->v;		// レーザーの太さ

			u1 = D3DVAL(208) / D3DVAL(256);
			v1 = D3DVAL(  8+4) / D3DVAL(256);
			u2 = D3DVAL(240) / D3DVAL(256);
			v2 = D3DVAL( 40-4) / D3DVAL(256);

			Set2DPointC(tlv+0, (x256-dx)>>8,       0, u1, v1, c);
			Set2DPointC(tlv+1, (x256+dx)>>8,       0, u1, v2, c);
			Set2DPointC(tlv+2, (x256+dx)>>8, y256>>8, u2, v2, c);
			Set2DPointC(tlv+3, (x256-dx)>>8, y256>>8, u2, v1, c);

			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

			dx = it->v << 1;

			u1 = D3DVAL(136) / D3DVAL(256);
			v1 = D3DVAL(  8) / D3DVAL(256);
			u2 = D3DVAL(200) / D3DVAL(256);
			v2 = D3DVAL( 72) / D3DVAL(256);

			Set2DPointC(tlv+0, (x256-dx)>>8, (y256-dx)>>8, u1, v1, c);
			Set2DPointC(tlv+1, (x256+dx)>>8, (y256-dx)>>8, u1, v2, c);
			Set2DPointC(tlv+2, (x256+dx)>>8, (y256+dx)>>8, u2, v2, c);
			Set2DPointC(tlv+3, (x256-dx)>>8, (y256+dx)>>8, u2, v1, c);

			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}
	}

	c  = RGBA_MAKE(255, 255, 255, 255);

	// ボム用のサブレーザー //
	ForEachFrontPtr(pExShotInfo->pExtraShot, STG2_BOMB_SUBLASER, it){
		// 基本となる座標＆角度をセット //
		x256 = it->x;		// Ｘ座標
		y256 = it->y;		// Ｙ座標
		dx   = it->v;		// レーザーの太さ

		u1 = D3DVAL(208)       / D3DVAL(256);
		v1 = D3DVAL(80 +  8+4) / D3DVAL(256);
		u2 = D3DVAL(240)       / D3DVAL(256);
		v2 = D3DVAL(80 + 40-4) / D3DVAL(256);

		Set2DPointC(tlv+0, (x256-dx)>>8,       0, u1, v1, c);
		Set2DPointC(tlv+1, (x256+dx)>>8,       0, u1, v2, c);
		Set2DPointC(tlv+2, (x256+dx)>>8, y256>>8, u2, v2, c);
		Set2DPointC(tlv+3, (x256-dx)>>8, y256>>8, u2, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// ボム用の黒い棺 //
	ForEachFrontPtr(pExShotInfo->pExtraShot, STG2_BOMB_BLACKBOX, it){
		// 基本となる座標＆角度をセット //
		x256 = it->x;		// Ｘ座標
		y256 = it->y;		// Ｙ座標
		dx   = it->v;		// レーザーの太さ

		c  = RGBA_MAKE(0, 0, 0, 96);
		c2 = RGBA_MAKE(0, 0, 0,  0);

		g_pGrp->SetTexture(GRPTEXTURE_MAX);
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);	// 描画ステート
		Set2DPointC(tlv+0, (x256-dx)>>8,       0, 0.0, 0.0, c);
		Set2DPointC(tlv+1, (x256+dx)>>8,       0, 1.0, 0.0, c);
		Set2DPointC(tlv+2, (x256+dx)>>8, y256>>8, 1.0, 1.0, c);
		Set2DPointC(tlv+3, (x256-dx)>>8, y256>>8, 0.0, 1.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		Set2DPointC(tlv+0, (x256-dx)>>8, 0,       0.0, 0.0, c2);
		Set2DPointC(tlv+1, (x256-dx)>>8, 0,       0.0, 0.0, c);
		Set2DPointC(tlv+2, (x256-dx)>>8, y256>>8, 0.0, 0.0, c);
		Set2DPointC(tlv+3, (x256-dx)>>8, y256>>8, 0.0, 0.0, c2);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		Set2DPointC(tlv+0, (x256+dx)>>8, 0,       0.0, 0.0, c);
		Set2DPointC(tlv+1, (x256+dx)>>8, 0,       0.0, 0.0, c2);
		Set2DPointC(tlv+2, (x256+dx)>>8, y256>>8, 0.0, 0.0, c2);
		Set2DPointC(tlv+3, (x256+dx)>>8, y256>>8, 0.0, 0.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		c = max(0, min(255, it->d - abs(SinL(it->Count-64, it->d))));
		c = RGBA_MAKE(c, c, c, 255);

		g_pGrp->SetTexture(TextureID);
		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);	// 描画ステート

		u1 = D3DVAL(256-48) / D3DVAL(256);
		v1 = D3DVAL(0)      / D3DVAL(256);
		u2 = D3DVAL(256)    / D3DVAL(256);
		v2 = D3DVAL(256)    / D3DVAL(256);

		Set2DPointC(tlv+0, (x256-dx)>>8,       0, u1, v1, c);
		Set2DPointC(tlv+1, (x256+dx)>>8,       0, u2, v1, c);
		Set2DPointC(tlv+2, (x256+dx)>>8, y256>>9, u2, v2, c);
		Set2DPointC(tlv+3, (x256-dx)>>8, y256>>9, u1, v2, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		Set2DPointC(tlv+0, (x256-dx)>>8, y256>>9, u1, v1, c);
		Set2DPointC(tlv+1, (x256+dx)>>8, y256>>9, u2, v1, c);
		Set2DPointC(tlv+2, (x256+dx)>>8, y256>>8, u2, v2, c);
		Set2DPointC(tlv+3, (x256-dx)>>8, y256>>8, u1, v2, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	g_pGrp->SetTexture(TEXTURE_ID_EFFECT);		// テクスチャ切り替え
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);	// レンダリングステートをセット

	// 下から迫り来る爆発の描画 //
	ForEachFrontPtr(pExShotInfo->pExtraShot, STG2_BOMB_EXPLOSION, it){
		for(i=0; i<EXTRAOPTION_MAX; i++){
			pOpt = it->Option + i;
			if(pOpt->count) continue;

			a  = max(0, 255-abs(pOpt->a - 128)*2);
			x  = (pOpt->x) >> 8;
			y  = (pOpt->y) >> 8;
			dx = (pOpt->d) * min(128, (pOpt->a * 2)-255) / 255;

			c = RGBA_MAKE(a, a, a, 255);

			Set2DPointC(tlv+0, x+dx, y+dx, 0.0, 0.0, c);
			Set2DPointC(tlv+1, x-dx, y+dx, 1.0, 0.0, c);
			Set2DPointC(tlv+2, x-dx, y-dx, 1.0, 1.0, c);
			Set2DPointC(tlv+3, x+dx, y-dx, 0.0, 1.0, c);

			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}
	}
}


// 通常の溜め攻撃(レベル１) //
FVOID CStg2Boss::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	int				mx, my;
	DWORD			t;
	int				i;

	// この場合は発射できません //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// レーザーの持続時間を求める //
	if(pExShotInfo->Charge < CGAUGE_LEVEL1) t = 20;
	else                                    t = 70;

	// ツインレーザーの音 //
	PlaySE(SNDID_TWIN_LASER, mx);

	// ツインレーザーをセット //
	for(i=-8; i<=8; i+=16){
		// リスト先頭に挿入準備 //
		pData   = pExShot->InsertBack(STG2_EXSHOT_NORMAL);
		if(NULL == pData) return;	// 空きが無い

		// データを初期化する //
		pData->x      = mx + i*256;				// 初期Ｘ座標
		pData->y      = my + STG2_EXSHOT_DY;	// 初期Ｙ座標
		pData->d      = (BYTE)i;				// 発射角(Ｘ軸のズレとして扱う)
		pData->v      = 0;						// 初速度(レーザーの幅として扱う)
		pData->Count  = t;						// カウンタ
	}
}


// ボムアタック(レベル１) //
FBOOL CStg2Boss::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CStg2Boss::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	CExtraShot::Iterator		it;			// 上の奴のイテレータ

	// エキストラショット：オープン //
	ForEachFrontPtr(pExtraShot, STG2_EXSHOT_NORMAL, it){
		return FALSE;
	}

	// エキストラショット：クローズ //
	ForEachFrontPtr(pExtraShot, STG2_EXSHOT_DELETE, it){
		return FALSE;
	}

/*
	// ボム：サブレーザー //
	ForEachFrontPtr(pExtraShot, STG2_BOMB_SUBLASER, it){
		return FALSE;
	}

	// ボム：秘密の爆発 //
	ForEachFrontPtr(pExtraShot, STG2_BOMB_EXPLOSION, it){
		return FALSE;
	}
*/
	return TRUE;
}


// 通常ボム //
FVOID CStg2Boss::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
	int				mx, my;
	int				width, ox;	//, l;
	int				i, j;	//, n;
//	BYTE			d;

	const int SubLaserMax = 10;

	pBGDraw->SetBlendColor(255, 100, 100, m_BombTime-60);

	pExShot = pExShotInfo->pExtraShot;

	// 座標値を取得する //
	ox    = pBGDraw->GetTargetOx();			// 画面中央
	width = pBGDraw->GetTargetWidth() / 2;	// 画面幅 / ２
	mx = *(pExShot->GetXPointer());			// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());			// 現在のＹ座標を取得

	// 黒い棺桶の描画 //
	pData = pExShot->InsertBack(STG2_BOMB_BLACKBOX);
	if(NULL == pData) return;	// 空きが無い
	pData->x     = mx;
	pData->y     = 480 * 256;
	pData->v     = 0;
	pData->Count = 0;
	pData->d     = 0;

	// 演出用サブレーザーのセット //
	for(i=0; i<SubLaserMax; i++){
		pData = pExShot->InsertBack(STG2_BOMB_SUBLASER);
		if(NULL == pData) return;	// 空きが無い

		pData->x     = ox - width + ( (RndEx()%(width >> 7)) << 8 );
		pData->y     = 480 * 256;
		pData->v     = 0;									// レーザー幅
		pData->Count = i * (m_BombTime/2) / SubLaserMax;	// 待ち時間
		pData->d     = 255;									// Open/Closeカウンタ
	}

	// 爆発をセット //
	for(i=0; i<2; i++){
		pData = pExShot->InsertBack(STG2_BOMB_EXPLOSION);
		if(NULL == pData) return;	// 空きが無い

		pData->x = mx;
		pData->y = 480 * 256;

		for(j=0; j<EXTRAOPTION_MAX; j++){
			pOpt = pData->Option + j;

			pOpt->a     = 255;
			pOpt->d     = (BYTE)RndEx()%96 + 16;
			pOpt->count = (j * (m_BombTime-60)) / EXTRAOPTION_MAX;
			pOpt->x     = mx - width/2 + (RndEx()%(width/128/2))*256;
			pOpt->y     = (400 + RndEx()%80) * 256;	//440*256 - y;
		}
	}
}
