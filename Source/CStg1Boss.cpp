/*
 *   CStg1Boss.cpp   : ミリアの定義
 *
 */

#include "CStg1Boss.h"
#include "Gian2001.h"
#include "CEnemyCtrl.h"
#include "SECtrl.h"

#include "RndCtrl.h"



#define STG1_EXSHOT_EXPAND	0x00	// 雷・拡大中
#define STG1_EXSHOT_FINISH	0x01	// 雷・消滅中
#define STG1_EXSHOT_SPHERE	0x02	// 電撃球体
#define STG1_EXSHOT_SPHERE2	0x03	// 電撃球体(消滅へと)
#define STG1_EXSHOT_BOMBTH	0x04	// ボム用・雷
#define STG1_EXSHOT_BOMBSP	0x05	// ボム用・びりびり球体

#define STG1_EXSHOT_DAMAGE	(40+60)	// 雷一回ヒットのダメージ量
#define STG1_BOMB_DAMAGE	80		// ボム雷・１ヒットのダメージ量

#define STG1_EXSHOT_COUNT	20		// 雷・拡大中
#define STG1_BOMBTH_COUNT	35		// ボム用雷・発生時間

#define STG1_BOMBTH_WIDTH	(256*16)	// ボム雷のあたり判定幅

#define STG1_EXOPTION_MAX	(EXTRAOPTION_MAX / 2)	// 雷の分割数
#define STG1_BOMBSP_MAX		(EXTRAOPTION_MAX / 6)	// びりびり球体の分割数

#define STG1_GST_RANGE			4



/////////////////////////////////////////////////////////////////////////////
// 参考 STG1_EXSHOT_EXPAND, _FINISH では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y)        : 雷リングの中心
//   v           : 基本展開速度
//   d           : 展開方向
//  count        : 発動からの経過時刻
//  pOpt->(x, y) : 雷の節の座標(絶対座標)
//  pOpt->count  : 雷の節の太さ
//  pOpt->d      : 展開方向
//---------------------------------------------------------------------------



/***** [グローバル変数] *****/
CStg1Boss		g_Stg1BossInfo;			// 主人公の情報格納用クラス



// コンストラクタ //
CStg1Boss::CStg1Boss()
{
	m_NormalSpeed = 4 * 256;		// 通常時の早さ
	m_ShiftSpeed  = 3 * 256 / 2;	// シフト移動時の早さ
	m_ChargeSpeed = 65536*2;		// 溜めの早さ
//	m_BuzzSpeed   = 65536;			// カスり溜めの早さ
	m_BuzzSpeed   = 98000;			// カスり溜めの早さ

	m_CharID       = CHARACTER_STG1;	// キャラクタＩＤ
	m_NormalShotDx = 0;					// ショットの発射座標
	m_NormalShotDy = 0;					// ショットの発射座標

	m_BombTime = 180;		// ボムの有効時間
}


// デストラクタ //
CStg1Boss::~CStg1Boss()
{
}


// 描画する //
FVOID CStg1Boss::PlayerDraw(
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
		u0 = D3DVAL(  0) / D3DVAL(256);
		u1 = D3DVAL( 64) / D3DVAL(256);
		v0 = D3DVAL( 56) / D3DVAL(256);
		v1 = D3DVAL(112) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG1_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL(128) / D3DVAL(256);
			u1 = D3DVAL(192) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL(192) / D3DVAL(256);
			u1 = D3DVAL(256) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}
	else{
		if(STG1_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(0)  / D3DVAL(256);
			u1 = D3DVAL(64) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(56) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(64)  / D3DVAL(256);
			u1 = D3DVAL(128) / D3DVAL(256);
			v0 = D3DVAL(0)   / D3DVAL(256);
			v1 = D3DVAL(56)  / D3DVAL(256);
		}
	}

	dx = 20 * 256;
	dy = 18 * 256;

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
FVOID CStg1Boss::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -STG1_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  STG1_GST_RANGE) (*pState)++;
	}
}


// エキストラショットを動作させる //
FVOID CStg1Boss::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my;//, oldx, oldy;
//	int			YMin, XMin, XMax;
	WORD		KeyCode;
	int			i, v;
	int			d;
	int			ox, oy;
//	BYTE		oldd;
	BOOL		bNeedDelete;

	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	ExtraShot				*pData;
	ExtraOption				*pOpt;

	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());
	my      = *(pExShot->GetYPointer());

	KeyCode = pExShotInfo->KeyCode;

	// 雷・展開中 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_EXPAND, it){
		pData = it.GetPtr();

		if(pData->Count >= STG1_EXSHOT_COUNT){
			pExShot->MoveBack(it, STG1_EXSHOT_FINISH);
		}

//		d    = (BYTE)(pData->d + pData->Count * 3);
		pOpt = pData->Option;
		for(i=0; i<STG1_EXOPTION_MAX; i++, pOpt++){
			d = (int)pOpt->d - (int)pData->d;
			if(     d < -128) d += 256;
			else if(d >  128) d -= 256;

			if(abs(d) > 8) v = -256 * 2 + (RndEx() % (256 * 10));
//			if(abs(d) > 8) v = -256 * 3 + (RndEx() % (256 * 13));
			else            v = -256 * 3 + (RndEx() % (256 * (13 + 18-SinL(-64+abs(d)*7, 18) ) / 2));
//			else            v = -256 * 3 + (RndEx() % (256 * (13 + 28-SinL(-64+abs(d)*7, 28) ) / 2));
//			else            v = -256 * 3 + (RndEx() % (256 * (13 + 66-abs(d)) / 2));

			pOpt->x += CosL(pOpt->d, v);
			pOpt->y += SinL(pOpt->d, v);
		}

		pData->Count++;
	}

	// 雷・消滅中 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_FINISH, it){
		pData = it.GetPtr();

		if(pData->Count >= 1){
			pData->Count -= 1;
		}
		else{
			DelContinuePtr(pExShot, it);
		}

		//d    = (BYTE)(pData->d + pData->Count * 3);
		pOpt = pData->Option;
		for(i=0; i<STG1_EXOPTION_MAX; i++, pOpt++){
//			v = 256 - (RndEx() % (256 * 3));
			v = 256 * 2 / 3;//-256 * 1 + (RndEx() % (256 * 5));

			pOpt->x += CosL(pOpt->d, v);
			pOpt->y += SinL(pOpt->d, v);
		}
	}

	// びりびり球体 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_SPHERE, it){
		pData = it.GetPtr();

		if(pData->v <= 0){
			pData->v = 256 + RndEx()%(256 * 2);
			pExShot->MoveBack(it, STG1_EXSHOT_SPHERE2);
		}

		pData->v--;
	}

	// びりびり球体 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_SPHERE2, it){
		pData = it.GetPtr();

		if(pData->Count >= STG1_EXSHOT_COUNT){
			DelContinuePtr(pExShot, it);
		}

		pData->x += CosL(pData->d, pData->v);
		pData->y += SinL(pData->d, pData->v);
		pData->Count++;
	}

	// ボム用雷・メイン //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_BOMBTH, it){
		pData = it.GetPtr();

		if(pData->v){
			pData->v--;
			if(0 == pData->v) PlaySE(SNDID_LIGHTNING_P, pData->x);
		}
		else{
			if(pData->Count >= STG1_BOMBTH_COUNT){
				DelContinuePtr(pExShot, it);
			}

			ox = pData->x;
			pOpt = pData->Option + 1;	// +1 は重要！

			if(pData->Count > STG1_BOMBTH_COUNT / 2){
				for(i=0; i<STG1_EXOPTION_MAX-1; i++, pOpt++){
					if(pOpt->x > ox) v =  256 / 4;
					else             v = -256 / 4;

					pOpt->x += v;
				}
			}
			else{
				for(i=0; i<STG1_EXOPTION_MAX-1; i++, pOpt++){
					if(pOpt->x > ox) v = -128 + (RndEx() % (256 * 2));
					else             v =  128 - (RndEx() % (256 * 2));

					pOpt->x += v;
				}
			}

			pEnemy->GetEnemyTamaPtr()->ClearFromRect(
				pData->x, 0, STG1_BOMBTH_WIDTH, pData->y);

			pData->Count++;
		}
	}

	// ボム用雷・びりびり球体 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_BOMBSP, it){
		pData = it.GetPtr();

		if(pData->Count){
			pData->Count--;
		}
		else{
			bNeedDelete = TRUE;

			pOpt = pData->Option;
			for(i=0; i<STG1_BOMBSP_MAX; i++, pOpt++){
				if(pOpt->count){
					pOpt->count--;
				}
				else{
					if(pOpt->a != 255){
						bNeedDelete = FALSE;
						pOpt->x += CosL(pOpt->d, 256+128);
						pOpt->y += SinL(pOpt->d, 256+128) + pOpt->a * 3;
						pOpt->a = min(255, pOpt->a + 4);
					}
				}
			}

			if(bNeedDelete) DelContinuePtr(pExShot, it);
		}
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CStg1Boss::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	CExtraShot::Iterator	it;
	ExtraShot				*pData;
	DWORD					damage;

	// ダメージ合計をゼロ初期化 //
	damage = 0;

	// 展開中の雷との当たり判定を行う //
	ForEachFrontPtr(pExtraShot, STG1_EXSHOT_EXPAND, it){
		damage += HitCheckThunder(pEnemy, it.GetPtr());
	}

	// 消滅中の雷との当たり判定を行う //
	ForEachFrontPtr(pExtraShot, STG1_EXSHOT_FINISH, it){
		damage += HitCheckThunder(pEnemy, it.GetPtr());
	}

	// ボム雷のあたり判定を行う //
	ForEachFrontPtr(pExtraShot, STG1_EXSHOT_BOMBTH, it){
		pData = it.GetPtr();

		// 雷は上方向に無限の当たり判定がある //
		if((abs(pData->x - pEnemy->x) <= STG1_BOMBTH_WIDTH)
		&& (pEnemy->y <= pData->y) && (pData->Count > STG1_BOMBTH_COUNT / 4)){
			damage += STG1_BOMB_DAMAGE;
		}
	}

	// ダメージを返す //
	return damage;
}


// エキストラショットを描画する //
FVOID CStg1Boss::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;

	// エキストラショット格納クラスに接続 //
	pExShot = pExShotInfo->pExtraShot;

	// 描画の前準備 //
	g_pGrp->SetTexture(TextureID);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	// 展開中の雷を描画する //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_EXPAND, it){
		DrawThunder(it.GetPtr());
	}

	// 消滅中の雷を描画する //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_FINISH, it){
		DrawThunder(it.GetPtr());
	}

	// びりびり球体・出現 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_SPHERE, it){
		DrawSphere(it.GetPtr());
	}

	// びりびり球体・消滅 //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_SPHERE2, it){
		DrawSphere(it.GetPtr());
	}

	// ボム用・雷の描画を行う //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_BOMBTH, it){
		DrawBombThunder(it.GetPtr());
	}

	// ボム用・びりびり球体の描画を行う //
	ForEachFrontPtr(pExShot, STG1_EXSHOT_BOMBSP, it){
		DrawBombSphere(it.GetPtr());
	}
}


// 通常の溜め攻撃(レベル１) //
FVOID CStg1Boss::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
	WORD			KeyCode;
	int				mx, my;
	int				i, j, n, v, delta;
	BYTE			d, angle;
	int				l;

	// １／２まで溜まっていないので発動不可 //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());	// 現在のＸ座標
	my      = *(pExShot->GetYPointer());	// 現在のＹ座標
	KeyCode = pExShotInfo->KeyCode;

	// 雷を構成するデータをの初期状態 //
	n = (pExShotInfo->Charge < CGAUGE_LEVEL1) ? 2 : 4;
	v = 256 * 5;

	// のびのびする方向を決める//
	switch(KeyCode & (KEY_UP | KEY_LEFT | KEY_RIGHT)){
		case KEY_LEFT:
			if(KeyCode & KEY_SHIFT) angle = 0;
			else                    angle = -64+32;
		break;

		case KEY_RIGHT:
			if(KeyCode & KEY_SHIFT) angle = 128;
			else                    angle = -64-32;
		break;

		case KEY_UP:
			if(KeyCode & KEY_SHIFT) angle =  64;
			else                    angle = -64;
		break;

		case KEY_UP|KEY_LEFT:	angle =  64-32;		break;
		case KEY_UP|KEY_RIGHT:	angle =  64+32;		break;
		default:				angle = -64;		break;
	}
/*
	if(KeyCode & KEY_UP)	angle += 128;
	if(KeyCode & KEY_LEFT)  angle += 32;
	if(KeyCode & KEY_RIGHT) angle -= 32;
*/
	PlaySE(SNDID_LIGHTNING_P, mx);

	for(i=0; i<n; i++){
		pData = pExShot->InsertBack(STG1_EXSHOT_EXPAND);
		if(NULL == pData) return;

		pData->v = v;	// 初速度

		pData->Count = 0;		// 発動からの経過カウント
		pData->d     = angle;	// 基本展開方向
		pData->x     = mx;		// Ｘ座標の中心
		pData->y     = my;		// Ｙ座標の中心

		delta = 7 + (i * 5);
//		delta = 1 + (i << 2);
		d     = i + i + i - 5;

		pOpt = pData->Option;
		for(j=0; j<STG1_EXOPTION_MAX; j++, pOpt++){
			pOpt->a     = 0;
			pOpt->x     = mx;
			pOpt->y     = my;
			pOpt->count = (4 + RndEx()%delta) * 256;
			pOpt->d     = d + (255 * j) / STG1_EXOPTION_MAX;
		}
	}

	n <<= 1;

	// 電撃球体を発生させる //
	for(i=0; i<n; i++){
		pData = pExShot->InsertBack(STG1_EXSHOT_SPHERE);
		if(NULL == pData) return;

		d = (BYTE)RndEx();
		l = (RndEx() % 64) << 8;
		pData->Count = 0;
		pData->d     = d;
		pData->v     = RndEx() % 20;
		pData->x     = mx + CosL(d, l);
		pData->y     = my + SinL(d, l);
	}
}


// ボムアタック(レベル１) //
FBOOL CStg1Boss::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CStg1Boss::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	return TRUE;
}


// 通常ボム //
FVOID CStg1Boss::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
	WORD			KeyCode;
	int				mx, my, ox, oy;
	int				i, j, n, v, delta, t;
	BYTE			d, angle;
	int				l, width;

//	pBGDraw->SetBlendColor(180, 50, 200, m_BombTime-60);
	pBGDraw->SetBlendColor(200, 150, 200, m_BombTime-60);
//	pBGDraw->SetBlendColor(168, 168, 255, m_BombTime-60);

	pExShot = pExShotInfo->pExtraShot;
//	mx      = *(pExShot->GetXPointer());	// 現在のＸ座標
//	my      = *(pExShot->GetYPointer());	// 現在のＹ座標

	mx    = pExShot->GetXMin() + 32 * 256;
	my    = pExShot->GetYMax() - 64 * 256;
	width = (pExShot->GetXMax() - pExShot->GetXMin() - 64*256) >> 8;

	// 縦方向雷を発生させる //
	for(n=0; n<16; n++){
		ox = mx + ((RndEx() % width) << 8);
		oy = my - (RndEx() % 256) * 256;
		t  = n * 120 / 16;

		for(i=0; i<3; i++){
			pData = pExShot->InsertBack(STG1_EXSHOT_BOMBTH);
			if(NULL == pData) return;

			pData->Count = 0;
			pData->d     = 0;
			pData->v     = t;
			pData->x     = ox;
			pData->y     = oy;

			pOpt = pData->Option;
			for(j=0; j<STG1_EXOPTION_MAX; j++, pOpt++){
				pOpt->a     = 0;
				pOpt->x     = ox + ((RndEx() & 1) ? -1 : 1);
				pOpt->y     = oy - (j * 480 * 256 / STG1_EXOPTION_MAX);
				pOpt->count = 0;
				pOpt->d     = 2 + RndEx() % (1 + (i*8));	// 太さパラメータ
			}
		}

		// ボム専用・びりびり球体のセット
		pData = pExShot->InsertBack(STG1_EXSHOT_BOMBSP);
		if(NULL == pData) return;

		pData->x     = ox;
		pData->y     = oy;
		pData->Count = t;
		pData->d     = 0;
		pData->v     = 0;

		pOpt = pData->Option;
		for(j=0; j<STG1_BOMBSP_MAX; j++, pOpt++){
			pOpt->a     = 0;			// 発動後のカウンタ(Inc)
			pOpt->count = j * 20 / STG1_BOMBSP_MAX;	// 発動前のカウンタ(Dec)
			pOpt->d     = RndEx();		// 移動方向
			pOpt->x     = ox;
			pOpt->y     = oy;
		}
	}
/*
	// 電撃球体を発生させる //
	for(i=0; i<20; i++){
		pData = pExShot->InsertBack(STG1_EXSHOT_SPHERE);
		if(NULL == pData) return;

		d = (BYTE)RndEx();
//		l = (RndEx() % 48) << 8;
		pData->Count = 0;
		pData->d     = d;
		pData->v     = RndEx() % 30;
		pData->x     = mx;
		pData->y     = my - 256 * 96;
	}
*/
}


// 雷の当たり判定を行う             //
// arg  pEnemy : 対象となる敵データ //
//      pShot  : 対象となる雷データ //
// ret         : ダメージ           //
FDWORD CStg1Boss::HitCheckThunder(EnemyData *pEnemy, ExtraShot *pShot)
{
	int				i, w, l;
	ExtraOption		*pOpt;
	int				tx, ty, ex, ey, ox, oy;
	int				size;
	BYTE			d;

	ex   = pEnemy->x;
	ey   = pEnemy->y;
	size = pEnemy->size + 256 * 2;

	pOpt = pShot->Option;
	for(i=0; i<STG1_EXOPTION_MAX; i++){
		ox = -pOpt->x;
		oy = -pOpt->y;
		tx = ex + ox;	// ベクトルのＸ成分
		ty = ey + oy;	// ベクトルのＹ成分

		// 次の頂点を参照する準備 //
		if(i <= STG1_EXOPTION_MAX-2) pOpt++;
		else                         pOpt = pShot->Option;

		ox += pOpt->x;
		oy += pOpt->y;
		d   = Atan8(ox, oy);

		// レーザーと自機との距離を求める //
		l =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
		w = -SinL(d, tx) + CosL(d, ty);	// 幅方向

		if(abs(w) < size && l > 0 && l < Isqrt(ox*ox + oy*oy)){
			return STG1_EXSHOT_DAMAGE;
		}
	}
	return 0;
/*
	// 以下のループは、最適化されていません //
	for(i=0; i<STG1_EXOPTION_MAX; i++){
		// P(i) -> X のベクトルを求める //
		vp.x = -pOpt->x;
		vp.y = -pOpt->y;
		vx.x = ex + vp.x;
		vx.y = ey + vp.y;

		// 次の頂点を参照する準備 //
		if(i <= STG1_EXOPTION_MAX-2) pOpt++;
		else                         pOpt = pShot->Option;

		// P(i) -> P(i+1) のベクトルを求める //
		vp.x += pOpt->x;
		vp.y += pOpt->y;

		// P(i)->X と P(i)->P(i+1) の内積を求める //
		t = (vp.x * vx.x) + (vp.y * vx.y);

		// P(i)->X の長さ と P(i)->P(i+1) の長さを求める //
		// ISqrt は一回にしたいが、桁あふれの危険性有り  //
		lx = Isqrt((vx.x * vx.x) + (vx.y * vx.y));
		if(0 == lx) return STG1_EXSHOT_DAMAGE;		// 距離が０ならば当たっている

		lp = Isqrt((vp.x * vp.x) + (vp.y * vp.y));
		if(0 == lp) continue;		// ひとまず、保留

		// cos(angle) >= 0 ならば、角度(256)が 0..64 となる //
		// 従って、この値が負ならば、ヒットする事は無い     //
		if(t / (lp * lx) < 0) continue;

		vx.x = ex - pOpt->x;
		vx.y = ey - pOpt->y;
		vp.x = -vp.x;
		vp.y = -vp.y;

		// P(i+1)->X と P(i+1)->P(i) の内積を求める //
		t = (vp.x * vx.x) + (vp.y * vx.y);

		// P(i+1)->X の長さを求める //
		lx = Isqrt((vx.x * vx.x) + (vx.y * vx.y));
		if(0 == lx) return STG1_EXSHOT_DAMAGE;		// 距離が０ならば当たっている

		// cos(angle) >= 0 ならば、角度(256)が 0..64 となる //
		// 従って、この値が負ならば、ヒットする事は無い     //
		if(lx < hitsize && t / (lp * lx) >= 0){
			return STG1_EXSHOT_DAMAGE;
		}
	}
	return 0;
*/
}


// 雷を描画する //
FVOID CStg1Boss::DrawThunder(ExtraShot *pShot)
{
	ExtraOption		*pOpt;
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y, i;//, ox, oy, i;
	int				dx, dy;
	DWORD			c, t;

	u0 = D3DVAL(  0) / D3DVAL(256);
	u1 = D3DVAL( 32) / D3DVAL(256);
	v0 = D3DVAL(128+13+32) / D3DVAL(256);
	v1 = D3DVAL(160-13+32) / D3DVAL(256);
	t  = (pShot->Count * 256) / STG1_EXSHOT_COUNT;
	c  = RGBA_MAKE((210*t)>>8, (180*t)>>8, (255*t)>>8, 255);

	pOpt = pShot->Option;
	for(i=0; i<STG1_EXOPTION_MAX; i++){
		dx = CosL(pOpt->d, pOpt->count);
		dy = SinL(pOpt->d, pOpt->count);
		x  = pOpt->x;
		y  = pOpt->y;
		Set2DPointC(tlv+0, (x-dx)>>8, (y-dy)>>8, u0, v0, c);
		Set2DPointC(tlv+1, (x+dx)>>8, (y+dy)>>8, u1, v0, c);

		if(i <= STG1_EXOPTION_MAX-2) pOpt++;
		else                         pOpt = pShot->Option;

		dx = CosL(pOpt->d, pOpt->count);
		dy = SinL(pOpt->d, pOpt->count);
		x  = pOpt->x;
		y  = pOpt->y;
		Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v1, c);
		Set2DPointC(tlv+3, (x+dx)>>8, (y+dy)>>8, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// ボム用雷を描画する //
FVOID CStg1Boss::DrawBombThunder(ExtraShot *pShot)
{
	ExtraOption		*pOpt;
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y, i;//, ox, oy, i;
	int				dx, dy;
	DWORD			c, t;

	if(pShot->v) return;

	u0 = D3DVAL(  0) / D3DVAL(256);
	u1 = D3DVAL( 32) / D3DVAL(256);
	v0 = D3DVAL(128+13+32) / D3DVAL(256);
	v1 = D3DVAL(160-13+32) / D3DVAL(256);
	t  = 256 - (pShot->Count * 256) / STG1_BOMBTH_COUNT;
	c  = RGBA_MAKE((210*t)>>8, (180*t)>>8, (255*t)>>8, 255);

	pOpt = pShot->Option;
	for(i=0; i<STG1_EXOPTION_MAX-1; i++){
		dx = pOpt->d;
		x  = (pOpt->x) >> 8;
		y  = (pOpt->y) >> 8;
		Set2DPointC(tlv+0, x-dx, y, u0, v0, c);
		Set2DPointC(tlv+1, x+dx, y, u1, v0, c);

		pOpt++;

		dx = pOpt->d;
		x  = (pOpt->x) >> 8;
		y  = (pOpt->y) >> 8;
		Set2DPointC(tlv+2, x-dx, y, u1, v1, c);
		Set2DPointC(tlv+3, x+dx, y, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// びりびり球体を描画する //
FVOID CStg1Boss::DrawSphere(ExtraShot *pShot)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y;
	DWORD			c;
	int				t;

	x = ((pShot->x) >> 8);
	y = ((pShot->y) >> 8);

	u0 = D3DVAL(  0) / D3DVAL(256);
	u1 = D3DVAL( 32) / D3DVAL(256);
	v0 = D3DVAL(128+32) / D3DVAL(256);
	v1 = D3DVAL(160+32) / D3DVAL(256);

	t = (pShot->Count - (STG1_EXSHOT_COUNT / 2)) << 1;
	t = 255 - min(255, abs(t) * 255 / STG1_EXSHOT_COUNT);
	c = RGBA_MAKE(t, t, t, 255);

	t = 24+8 + CosL(-(int)pShot->Count<<3, 8+4);
	Set2DPointC(tlv+0, x-t, y-t, u0, v0, c);
	Set2DPointC(tlv+1, x+t, y-t, u1, v0, c);
	Set2DPointC(tlv+2, x+t, y+t, u1, v1, c);
	Set2DPointC(tlv+3, x-t, y+t, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// ボム用球体を描画する //
FVOID CStg1Boss::DrawBombSphere(ExtraShot *pShot)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, u1, v0, v1;
	int				x, y;
	DWORD			c;
	int				t, i;
	ExtraOption		*pOpt;

	if(pShot->Count) return;

	u0 = D3DVAL(  0) / D3DVAL(256);
	u1 = D3DVAL( 32) / D3DVAL(256);
	v0 = D3DVAL(128+32) / D3DVAL(256);
	v1 = D3DVAL(160+32) / D3DVAL(256);

	pOpt = pShot->Option;
	for(i=0; i<STG1_BOMBSP_MAX; i++, pOpt++){
		if(pOpt->count) continue;
		if(255 == pOpt->a) continue;

		t  = 256 - pOpt->a;
		c  = RGBA_MAKE((210*t)>>8, (180*t)>>8, (255*t)>>8, 255);

		x = pOpt->x >> 8;
		y = pOpt->y >> 8;
		t = (255 - pOpt->a) / 8;//10;

		Set2DPointC(tlv+0, x-t, y-t, u0, v0, c);
		Set2DPointC(tlv+1, x+t, y-t, u1, v0, c);
		Set2DPointC(tlv+2, x+t, y+t, u1, v1, c);
		Set2DPointC(tlv+3, x-t, y+t, u0, v1, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
