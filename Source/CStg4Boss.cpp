/*
 *   CStg4Boss.cpp   : マリーの定義
 *
 */

#include "CStg4Boss.h"
#include "Gian2001.h"
#include "SECtrl.h"

#include "RndCtrl.h"


#define STG4_GST_RANGE			6

#define EXSHOT_HITSIZE			(5 * 256)	// ミサイルの当たり判定幅
#define EXBOMB_HITSIZE			(16 * 256)	// ボムの当たり判定幅
#define MISSILE_DAMAGE			512			// ミサイルのダメージ
#define BOMB_DAMAGE				196			// ミサイルのダメージ
#define MISSILE_SPEED			(10*256)	// ミサイルの初速度
#define SMOKE_MAX				16			// 煙同時発生数

#define EXST_MISSILE_BACK		0x00		// ミサイル後ろ方向に移動中
#define EXST_MISSILE_ACC		0x01		// ミサイル加速中
#define EXST_MISSILE_DELETE		0x02		// ミサイル煙の消滅待ち
#define EXST_BOMBWAIT			0x03		// ボム用艦載機・発動待ち
#define EXST_BOMBMOVE			0x04		// ボム用艦載機・移動中
#define EXST_BOMBDELETE			0x05		// 艦載機の残してゆくエフェクト・消滅用

#define STG5_NUMBOMB			60			// ボム用機体の最大発生数



/////////////////////////////////////////////////////////////////////////////
// 参考：MISSILE_BACK では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y)        : 目標の座標
//   v           : 現在の速度
//   d           : 進行方向
//  count        : 発射時刻ディレイ(０以外は描画しないように)
//  pOpt->(x, y) : 煙の座標
//  pOpt->count  : 煙のアニメーション用
//  pOpt->d      : ０ならばその煙は無効である
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 参考：MISSILE_ACC では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y)        : 目標の座標
//   v           : 現在の速度
//  pOpt->(x, y) : 煙の座標
//  pOpt->count  : 煙のアニメーション用
//  pOpt->d      : ０ならばその煙は無効である
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 参考：MISSILE_DELETE では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
//  pOpt->(x, y) : 煙の座標
//  pOpt->count  : 煙のアニメーション用
//  pOpt->d      : ０ならばその煙は無効である
//---------------------------------------------------------------------------



/***** [グローバル変数] *****/
CStg4Boss		g_Stg4BossInfo;			// 主人公の情報格納用クラス



// コンストラクタ //
CStg4Boss::CStg4Boss()
{
	m_NormalSpeed = 2 * 256;		// 通常時の早さ
	m_ShiftSpeed  = 2 * 256 / 2;	// シフト移動時の早さ
	m_ChargeSpeed = 170 * 256;		// 溜めの早さ
//	m_BuzzSpeed   = 65536;			// カスり溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_STG4;	// キャラクタＩＤ
	m_NormalShotDx = 0;					// ショットの発射座標
	m_NormalShotDy = -14*256;			// ショットの発射座標

	m_BombTime = 150;		// ボムの有効時間
}


// デストラクタ //
CStg4Boss::~CStg4Boss()
{
}


// 描画する //
FVOID CStg4Boss::PlayerDraw(
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

	if(0 == State){
		u0 = D3DVAL(0*48) / D3DVAL(256);
		u1 = D3DVAL(1*48) / D3DVAL(256);
		v0 = D3DVAL(   0) / D3DVAL(256);
		v1 = D3DVAL(  64) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG4_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL(1*48) / D3DVAL(256);
			u1 = D3DVAL(2*48) / D3DVAL(256);
			v0 = D3DVAL(   0) / D3DVAL(256);
			v1 = D3DVAL(  64) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL(2*48) / D3DVAL(256);
			u1 = D3DVAL(3*48) / D3DVAL(256);
			v0 = D3DVAL(   0) / D3DVAL(256);
			v1 = D3DVAL(  64) / D3DVAL(256);
		}
	}
	else{
		if(STG4_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(3*48) / D3DVAL(256);
			u1 = D3DVAL(4*48) / D3DVAL(256);
			v0 = D3DVAL(   0) / D3DVAL(256);
			v1 = D3DVAL(  64) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(4*48) / D3DVAL(256);
			u1 = D3DVAL(5*48) / D3DVAL(256);
			v0 = D3DVAL(   0) / D3DVAL(256);
			v1 = D3DVAL(  64) / D3DVAL(256);
		}
	}

/*
	if(0 == State){
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 56) / D3DVAL(256);
		v0 = D3DVAL( 80) / D3DVAL(256);
		v1 = D3DVAL(160) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG4_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL( 56) / D3DVAL(256);
			u1 = D3DVAL(112) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 80) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 56) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 80) / D3DVAL(256);
		}
	}
	else{
		if(STG4_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(112) / D3DVAL(256);
			u1 = D3DVAL(168) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 80) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(168) / D3DVAL(256);
			u1 = D3DVAL(224) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 80) / D3DVAL(256);
		}
	}
*/
	dx = 20 * 256;
	dy = 26 * 256;

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
FVOID CStg4Boss::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -STG4_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  STG4_GST_RANGE) (*pState)++;
	}
}


// エキストラショットを動作させる //
FVOID CStg4Boss::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my;
	int			YMin, YMax, XMin, XMax;
	WORD		KeyCode;
	int			i;

	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	ExtraShot				*pData;
	ExtraOption				*pOpt;


	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());
	my      = *(pExShot->GetYPointer());

	YMin    = pExShot->GetYMin() - (50 * 256);
	XMin    = pExShot->GetXMin() - (50 * 256);
	XMax    = pExShot->GetXMax() + (50 * 256);
	YMax    = pExShot->GetYMax() + (50 * 256);

	KeyCode = pExShotInfo->KeyCode;

	// ホーミングミサイル：後方移動中 //
	ForEachFrontPtr(pExShot, EXST_MISSILE_BACK, it){
		pData = it.GetPtr();

		if(pData->Count){
			pData->Count--;

			if(0 == pData->Count){
				pData->x = mx;
				pData->y = my;

				// ミサイルの煙に関する情報を初期化 //
				pOpt = pData->Option;
				for(i=0; i<SMOKE_MAX; i++, pOpt++){
					pOpt->d     = 0;
					pOpt->x     = mx;
					pOpt->y     = my;
					pOpt->count = (i * 256) / SMOKE_MAX;
				}
			}
		}
		else{
			if(pData->v <= 0){
				// ミサイル加速音 //
				PlaySE(SNDID_MISSILE_ACCEL, mx);

				pData->d = -64;
				pExShot->MoveBack(it, EXST_MISSILE_ACC);
				continue;
			}

			pData->x += CosL(pData->d, pData->v);
			pData->y += SinL(pData->d, pData->v);
			pData->v -= 64;

			// 煙の状態推移＆煙更新 //
			MoveSmoke(pData);
		}
	}

	// ホーミングミサイル：加速中 //
	ForEachFrontPtr(pExShot, EXST_MISSILE_ACC, it){
		pData = it.GetPtr();

		if(pData->y <= YMin){
			pExShot->MoveBack(it, EXST_MISSILE_DELETE);
		}

		pData->y -= pData->v;
		pData->v += 128;

		// 煙の状態推移＆煙更新 //
		MoveSmoke(pData);
	}

	// ホーミングミサイル：煙消滅待ち //
	ForEachFrontPtr(pExShot, EXST_MISSILE_DELETE, it){
		pData = it.GetPtr();

		if(DeleteSmoke(pData)){
			DelContinuePtr(pExShot, it);
		}
	}

	// ボム用機体の待ち時間 //
	ForEachFrontPtr(pExShot, EXST_BOMBWAIT, it){
		if(0 == it->Count){
			pOpt = it->Option;
			for(i=0; i<SMOKE_MAX; i++, pOpt++){
				pOpt->d     = 0;
				pOpt->x     = it->x;
				pOpt->y     = it->y;
				pOpt->count = (i * 256) / SMOKE_MAX;
			}

			pExShot->MoveBack(it, EXST_BOMBMOVE);
			continue;
		}
		else{
			it->Count--;
		}
	}

	// ボム用機体・動作中 //
	ForEachFrontPtr(pExShot, EXST_BOMBMOVE, it){
		pData = it.GetPtr();

		pData->Count++;
		pData->x += CosL(pData->d, 10*256 + (pData->Count << 3));
		pData->y += SinL(pData->d, 10*256 + (pData->Count << 3));
//		pData->v += 64;

		if(pData->Count > 10 && pData->Count < 30){
			pData->d += pData->v;
		}

		if(pData->y <= YMin || pData->y >= YMax
		|| pData->x <= XMin || pData->x >= XMax){
			pExShot->MoveBack(it, EXST_BOMBDELETE);
			continue;
		}

		MoveBombSmoke(pData);
	}

	ForEachFrontPtr(pExShot, EXST_BOMBDELETE, it){
		pData = it.GetPtr();

		if(DeleteSmoke(pData)){
			DelContinuePtr(pExShot, it);
		}
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CStg4Boss::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot;
	CExtraShot::Iterator	it;

	DWORD		Damage;
	int			x, y, i, n;
	int			Size;

	const int	HitChkTarget[2] = {
		EXST_MISSILE_BACK, EXST_MISSILE_ACC
	};

	Damage = 0;				// ダメージをゼロ初期化
	x      = pEnemy->x;		// 敵のＸ座標
	y      = pEnemy->y;		// 敵のＹ座標

	// 当たり判定サイズ(後で変更のこと) //
	Size = EXSHOT_HITSIZE + pEnemy->size;

	for(i=0; i<2; i++){
		n = HitChkTarget[i];

		ForEachFrontPtr(pExtraShot, n, it){
			pShot = it.GetPtr();

			if(HitCheckFast(y, (pShot->y), Size)
			&& HitCheckFast(x, (pShot->x), Size)){
				Damage += MISSILE_DAMAGE;
			}
		}
	}

	// 当たり判定サイズ(後で変更のこと) //
	Size = EXBOMB_HITSIZE + pEnemy->size;
	ForEachFrontPtr(pExtraShot, EXST_BOMBMOVE, it){
		pShot = it.GetPtr();

		if(HitCheckFast(y, (pShot->y), Size)
		&& HitCheckFast(x, (pShot->x), Size)){
			Damage += BOMB_DAMAGE;
		}
	}

	return Damage;
}


// エキストラショットを描画する //
FVOID CStg4Boss::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;

	// エキストラショット格納クラスに接続 //
	pExShot = pExShotInfo->pExtraShot;

	// 描画の前準備 //
	g_pGrp->SetTexture(TextureID);

	ForEachFrontPtr(pExShot, EXST_MISSILE_BACK, it){
		// カウンタが０以外なら発射されていない //
		if(0 == it->Count) DrawMissile(it.GetPtr());
	}

	ForEachFrontPtr(pExShot, EXST_MISSILE_ACC, it){
		DrawMissile(it.GetPtr());
	}

	ForEachFrontPtr(pExShot, EXST_MISSILE_DELETE, it){
		DrawMissile(it.GetPtr());
	}

	// ボム用乗り物の描画 //
	ForEachFrontPtr(pExShot, EXST_BOMBMOVE, it){
		DrawShip(it.GetPtr());
	}
}


// 通常の溜め攻撃(レベル１) //
FVOID CStg4Boss::Level1Attack(ExtraShotInfo *pExShotInfo)
{
// #define EXST_MISSILE_BACK		0x00		// ミサイル後ろ方向に移動中
// #define EXST_MISSILE_ACC		0x01		// ミサイル加速中
	ExtraShot		*pData;
//	ExtraOption		*pOpt;
	CExtraShot		*pExShot;
	WORD			KeyCode;
	int				mx, my;
	int				i, n, delay, v;
	int				dw, d0;

	// １／２まで溜まっていないので発動不可 //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
	my      = *(pExShot->GetYPointer());		// 現在のＹ座標
	KeyCode = pExShotInfo->KeyCode;				// キーコード

	dw = 128;
	d0 = 64;

	if(KeyCode & KEY_DOWN)  dw  = 32;
	if(KeyCode & KEY_LEFT)  d0 -= 56;
	if(KeyCode & KEY_RIGHT) d0 += 56;

	d0 -= dw / 2;

	// ミサイルの本数を決める //
	if(pExShotInfo->Charge < CGAUGE_LEVEL1){
		n     = 10;
		delay = 15;
	}
	else{
		n     = 15;
		delay = 30;
	}

	// ミサイルセット音 //
	PlaySE(SNDID_MISSILE_SET, mx);

	for(i=0; i<n; i++){
		pData = pExShot->InsertBack(EXST_MISSILE_BACK);
		if(NULL == pData) return;

		v = RndEx() % (MISSILE_SPEED);

		pData->x     = mx;				// 目標のＸ座標
		pData->y     = my;				// 目標のＹ座標
		pData->v     = v;				// 初速度
		pData->Count = 1+RndEx()%delay;	// 発動時間のディレイ
		pData->d     = d0 + (i*dw) / n;	// リングバッファ用カーソル
	}
}


// ボムアタック(レベル１) //
FBOOL CStg4Boss::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CStg4Boss::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	CExtraShot::Iterator		it;			// 上の奴のイテレータ

	ForEachFrontPtr(pExtraShot, EXST_BOMBWAIT, it){
		return FALSE;
	}

	ForEachFrontPtr(pExtraShot, EXST_BOMBMOVE, it){
		return FALSE;
	}

	ForEachFrontPtr(pExtraShot, EXST_BOMBDELETE, it){
		return FALSE;
	}

	return TRUE;
}


// 通常ボム //
FVOID CStg4Boss::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
	int				mx, my;
	int				i, j, t, v;
	int				ox, w;
	BYTE			d;

	pBGDraw->SetBlendColor(140, 50, 50, m_BombTime - 60);

	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
	my      = *(pExShot->GetYPointer());		// 現在のＹ座標

	w  = pBGDraw->GetTargetWidth();
	ox = pBGDraw->GetTargetOx() - (w >> 1);
	w  = w >> 8;

	for(i=0; i<STG5_NUMBOMB; i++){
		pData = pExShot->InsertBack(EXST_BOMBWAIT);
		if(NULL == pData) return;

		pData->Count = i * (m_BombTime-60) / STG5_NUMBOMB;
		pData->x     = ox + ((RndEx() % w) << 8);
		pData->y     = 500 * 256;
		pData->d     = RndEx() % 32 + 192 - 16;

		if((RndEx() & 0xff) < 128) pData->v = -3 + RndEx()%7;
		else                       pData->v = 0;
	}
}

/*
// 攻撃送り用グラフィックの状態推移用関数(初期化) //
FVOID CStg4Boss::InitAtkGrp(AtkGrpInfo *pInfo, DWORD AtkLv)
{
	InitDefaultAtkGrp(pInfo);

	switch(AtkLv){
	case 2:
	break;

	case 3:
	break;

	case 4:
	break;
	}
}


// 攻撃送り用グラフィックの状態推移用関数(レベル２) //
FBOOL CStg4Boss::Lv2AtkGrpMove(AtkGrpInfo *pInfo)
{
	return FALSE;
}


// 攻撃送り用グラフィックの状態推移用関数(レベル３) //
FBOOL CStg4Boss::Lv3AtkGrpMove(AtkGrpInfo *pInfo)
{
	return FALSE;
}


// 攻撃送り用グラフィックの状態推移用関数(レベル４) //
FBOOL CStg4Boss::Lv4AtkGrpMove(AtkGrpInfo *pInfo)
{
	return FALSE;
}
*/

// 煙を動作させる //
FVOID CStg4Boss::MoveSmoke(ExtraShot *pShot)
{
	int				i;
	BOOL			bMove;
	ExtraOption		*pOpt;

	pOpt = pShot->Option;
	bMove = (-64 == pShot->d) ? TRUE : FALSE;

	for(i=0; i<SMOKE_MAX; i++, pOpt++){
		pOpt->count += (256 / SMOKE_MAX);
		if(pOpt->count >= 256){
			pOpt->count -= 256;
			pOpt->d      = 255;
			pOpt->x      = pShot->x;
			pOpt->y      = pShot->y + 16 * 256;
		}
		else if(bMove){
			pOpt->y -= (pShot->v / 1);
		}
	}
}


// ボム用の煙を動作させる //
FVOID CStg4Boss::MoveBombSmoke(ExtraShot *pShot)
{
	int				i;
	BOOL			bMove;
	ExtraOption		*pOpt;

	pOpt = pShot->Option;
	bMove = (-64 == pShot->d) ? TRUE : FALSE;

	for(i=0; i<SMOKE_MAX; i++, pOpt++){
		pOpt->count += (256 / SMOKE_MAX);
		if(pOpt->count >= 256){
			pOpt->count -= 256;
			pOpt->d      = 255;
			pOpt->x      = pShot->x;
			pOpt->y      = pShot->y;
		}
		else if(bMove){
			pOpt->y -= (pShot->v / 1);
		}
	}
}


// 煙を消滅に向かわせる(TRUE:消滅完了) //
FBOOL CStg4Boss::DeleteSmoke(ExtraShot *pShot)
{
	int				i, n;
	BOOL			bMove;
	ExtraOption		*pOpt;

	pOpt  = pShot->Option;
	bMove = (-64 == pShot->d) ? TRUE : FALSE;

	for(i=n=0; i<SMOKE_MAX; i++, pOpt++){
		pOpt->count += (256 / SMOKE_MAX);
		if(pOpt->count >= 256){
			pOpt->d     = 0;
			pOpt->count = 255;
			n++;
		}
		else if(bMove){
			pOpt->y -= (pShot->v / 1);
		}
	}

	if(SMOKE_MAX == n) return TRUE;
	else               return FALSE;
}


// ミサイルを描画する(煙含む) //
FVOID CStg4Boss::DrawMissile(ExtraShot *pShot)
{
	ExtraOption		*pOpt;
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y, n, i, w, h;
	DWORD			c;

	x = ((pShot->x) >> 8);
	y = ((pShot->y) >> 8);

	u0 = D3DVAL( 64) / D3DVAL(256);
	v0 = D3DVAL( 80) / D3DVAL(256);
	u1 = D3DVAL( 80) / D3DVAL(256);
	v1 = D3DVAL(112) / D3DVAL(256);

	c = RGBA_MAKE(255, 255, 255, 255);
	Set2DPointC(tlv+0, x-5, y-10, u0, v0, c);
	Set2DPointC(tlv+1, x+5, y-10, u1, v0, c);
	Set2DPointC(tlv+2, x+5, y+10, u1, v1, c);
	Set2DPointC(tlv+3, x-5, y+10, u0, v1, c);

	g_pGrp->SetRenderStateEx(GRPST_COLORKEY);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	pOpt = pShot->Option;


	u0 = D3DVAL(64) / D3DVAL(256);
	v0 = D3DVAL(112)  / D3DVAL(256);
	u1 = D3DVAL(80) / D3DVAL(256);
	v1 = D3DVAL(128)  / D3DVAL(256);

	for(i=0; i<SMOKE_MAX; i++, pOpt++){
		if(0 == pOpt->d) continue;

		x  = (pOpt->x >> 8);
		y  = (pOpt->y >> 8);
		w  = (300 - pOpt->count)/30;
		h  = w << 1;
//		n  = (8 - min(8, pOpt->count/32)) << 4;

		n = (255 - pOpt->count) >> 1;
		c = RGBA_MAKE(n, n, 255, 255);//, 128);
		Set2DPointC(tlv+0, x-w, y-h, u0, v0, c);
		Set2DPointC(tlv+1, x+w, y-h, u1, v0, c);
		Set2DPointC(tlv+2, x+w, y+h, u1, v1, c);
		Set2DPointC(tlv+3, x-w, y+h, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// ボム用乗り物を描画する //
FVOID CStg4Boss::DrawShip(ExtraShot *pShot)
{
	ExtraOption		*pOpt;
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y, n, i, w, h;
	int				dx, dy;
	DWORD			c;
	BYTE			d;

	x = ((pShot->x) >> 8);
	y = ((pShot->y) >> 8);

	u0 = D3DVAL(128) / D3DVAL(256);
	u1 = D3DVAL(176) / D3DVAL(256);
	v0 = D3DVAL(160) / D3DVAL(256);
	v1 = D3DVAL(192) / D3DVAL(256);

	c = RGBA_MAKE(255, 255, 255, 255);

	x  = (pShot->x) >> 8;
	y  = (pShot->y) >> 8;
	d  = pShot->d + 64;
//	w  = 24 * 256;
//	h  = 16 * 256;
	w  = 16 * 256;
	h  = 11 * 256;

	dx = (CosL(d-128, w) + CosL(d-64, h)) >> 8;
	dy = (SinL(d-128, w) + SinL(d-64, h)) >> 8;

	Set2DPointC(tlv+0, x+dx, y+dy, u0, v0, c);
	Set2DPointC(tlv+2, x-dx, y-dy, u1, v1, c);

	dx = (CosL(d, w) + CosL(d-64, h)) >> 8;
	dy = (SinL(d, w) + SinL(d-64, h)) >> 8;

	Set2DPointC(tlv+1, x+dx, y+dy, u1, v0, c);
	Set2DPointC(tlv+3, x-dx, y-dy, u0, v1, c);

//	Set2DPointC(tlv+0, x-24, y-16, u0, v0, c);
//	Set2DPointC(tlv+1, x+24, y-16, u1, v0, c);
//	Set2DPointC(tlv+2, x+24, y+16, u1, v1, c);
//	Set2DPointC(tlv+3, x-24, y+16, u0, v1, c);

//	g_pGrp->SetRenderStateEx(GRPST_COLORKEY);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

//	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	pOpt = pShot->Option;

	u0 = D3DVAL(64) / D3DVAL(256);
	v0 = D3DVAL(112+16)  / D3DVAL(256);
	u1 = D3DVAL(80) / D3DVAL(256);
	v1 = D3DVAL(128+16)  / D3DVAL(256);

	for(i=0; i<SMOKE_MAX; i++, pOpt++){
		if(0 == pOpt->d) continue;

		x  = (pOpt->x >> 8);
		y  = (pOpt->y >> 8);
		w  = (300 - pOpt->count)/20;
//		n  = (8 - min(8, pOpt->count/32)) << 4;

		n = (255 - pOpt->count) >> 1;
		c = RGBA_MAKE(n, 190*n/255, 190*n/255, 255);//, 128);
		Set2DPointC(tlv+0, x-w, y-w, u0, v0, c);
		Set2DPointC(tlv+1, x+w, y-w, u1, v0, c);
		Set2DPointC(tlv+2, x+w, y+w, u1, v1, c);
		Set2DPointC(tlv+3, x-w, y+w, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
