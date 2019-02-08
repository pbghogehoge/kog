/*
 *   CStg5Boss.cpp   : エーリッヒの定義
 *
 */

#include "CStg5Boss.h"
#include "Gian2001.h"
#include "SECtrl.h"

#include "RndCtrl.h"



#define STG5_GST_RANGE		4

#define HLASER_LENGTH		16			// ホーミングレーザーのバッファ長
#define HLASER_SPEED		(256*16)	// ホーミングレーザーの初速度(けっこ速い)
#define HLASER_DAMAGE		(48*2)		// ホーミングレーザーのダメージ
#define HLASERBOMB_DAMAGE	128			// ボム用ホーミングレーザーのダメージ

#define LOCKON_INTERVAL		10			// 次のロックオンまでの間隔
#define LOCKONSITE_TIME		20			// ロックオンサイトの収束速度

#define EXST_FINDTARGET			0x00		// 目標検出中
#define EXST_ACCEL				0x01		// ターゲットに向かって加速中
#define EXST_DELETE				0x02		// 消去準備中(尻尾が画面外なら消去)
#define EXST_LOCKONSITE			0x03		// ロックオン枠
#define EXST_LOCKED				0x04		// ロック中
#define EXST_BOMBFINDTARGET		0x05		// ボムの目標検出中
#define EXST_BOMBACCEL			0x06		// ボムの加速中
#define EXST_BOMBDELETE			0x07		// ボムの消去準備中


/////////////////////////////////////////////////////////////////////////////
// 参考：FINDTARGET, ACCEL, DELETE では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y)      : 目標の座標
//   v         : 現在の速度
//  count      : カウンタ
//   d         : リングバッファ用カーソル(Head を指す)
// pOpt->(x,y) : ホーミングレーザーの節?の座標
// pOpt->d     : ホーミングレーザーの各節の角度
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 参考：LOCKONSITE では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y) : ロックオンサイトの座標
//  count : カウンタ(０ならばロック可能、それ以外は待ち->超高速ロックの禁止)
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 参考：LOCKED では ExtraShot を以下のように使用する
//---------------------------------------------------------------------------
// (x, y) : ロックオン対象の座標
// count  : アニメーション用(くるくる回る)
//---------------------------------------------------------------------------



/***** [グローバル変数] *****/
CStg5Boss		g_Stg5BossInfo;			// 主人公の情報格納用クラス



// コンストラクタ //
CStg5Boss::CStg5Boss()
{
	m_NormalSpeed = 3 * 256;		// 通常時の早さ
	m_ShiftSpeed  = 3 * 256 / 2;	// シフト移動時の早さ
	m_ChargeSpeed = 180 * 256;		// 溜めの早さ
//	m_BuzzSpeed   = 65536;			// カスり溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_STG5;	// キャラクタＩＤ
	m_NormalShotDx = 0;					// ショットの発射座標
	m_NormalShotDy = -8 * 256;			// ショットの発射座標

	m_BombTime = 80;		// ボムの有効時間
}


// デストラクタ //
CStg5Boss::~CStg5Boss()
{
}


// 描画する //
FVOID CStg5Boss::PlayerDraw(
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
	v0 = D3DVAL( 0) / D3DVAL(256);
	v1 = D3DVAL(56) / D3DVAL(256);

	// 描画状態により、関数の振り分けを行う //
	if(0 == State){
		u0 = D3DVAL(48*2) / D3DVAL(256);
		u1 = D3DVAL(48*3) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG5_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL(48*1) / D3DVAL(256);
			u1 = D3DVAL(48*2) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL(48*0) / D3DVAL(256);
			u1 = D3DVAL(48*1) / D3DVAL(256);
		}
	}
	else{
		if(STG5_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(48*3) / D3DVAL(256);
			u1 = D3DVAL(48*4) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(48*4) / D3DVAL(256);
			u1 = D3DVAL(48*5) / D3DVAL(256);
		}
	}
/*
	u0 = D3DVAL(0)  / D3DVAL(256);
	u1 = D3DVAL(56) / D3DVAL(256);
	v0 = D3DVAL(0)  / D3DVAL(256);
	v1 = D3DVAL(40) / D3DVAL(256);
*/
	dx = 18 * 256;
	dy = 21 * 256;

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
	x = (x256 - 18 * 256) >> 8;
	y = (y256 - 21 * 256) >> 8;
	Set2DPointC(tlv+0, x, y, u0, v0, c);

	x = (x256 + 18 * 256) >> 8;
	y = (y256 - 21 * 256) >> 8;
	Set2DPointC(tlv+1, x, y, u1, v0, c);

	x = (x256 + 18 * 256) >> 8;
	y = (y256 + 21 * 256) >> 8;
	Set2DPointC(tlv+2, x, y, u1, v1, c);

	x = (x256 - 18 * 256) >> 8;
	y = (y256 + 21 * 256) >> 8;
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

	if(bDamaged) g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	else         g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);

	g_pGrp->SetTexture(TextureID);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 描画用のＩＤを変更する //
FVOID CStg5Boss::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -STG5_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  STG5_GST_RANGE) (*pState)++;
	}
}



// エキストラショットを動作させる //
FVOID CStg5Boss::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my, oldx, oldy;
	int			YMin, XMin, XMax, YMax;
	WORD		KeyCode;
	int			ideg;
	BYTE		d;
	BYTE		oldd;

	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	ExtraShot				*pData;
	ExtraOption				*pOpt;


	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());
	my      = *(pExShot->GetYPointer());

	XMin    = pExShot->GetXMin() - (20 * 256);
	XMax    = pExShot->GetXMax() + (20 * 256);
	YMin    = pExShot->GetYMin() - (20 * 256);
	YMax    = pExShot->GetYMax() + (20 * 256);

	KeyCode = pExShotInfo->KeyCode;

	// ロックオンサイト //
	if(KeyCode & KEY_SHOT){
		pExShot->GetFrontIterator(it, EXST_LOCKONSITE);
		if(it != NULL){
			it->x     = mx;
			it->y     = my - 130 * 256;

			if(it->Count){
				it->Count--;
			}
		}
		else{
			pData = pExShot->InsertFront(EXST_LOCKONSITE);
			if(pData){
				pData->x     = mx;
				pData->y     = my - 130 * 256;
				pData->Count = LOCKON_INTERVAL;
			}
		}
	}
	// ショットキーが押されていない場合は消し去る //
	else{
		ForEachFrontPtr(pExShot, EXST_LOCKONSITE, it){
			DelContinuePtr(pExShot, it);
		}
	}

	// ロックオンマーカー //
	ForEachFrontPtr(pExShot, EXST_LOCKED, it){
		if(it->Count < LOCKONSITE_TIME) it->Count++;

		if(!(KeyCode & KEY_SHOT)){
			DelContinuePtr(pExShot, it);
		}
	}

	// 加速状態 //
	ForEachFrontPtr(pExShot, EXST_ACCEL, it){
		pData = it.GetPtr();

		pData->Count++;	// カウンタを更新

		// 前回の頭の座標を取得する //
		pOpt = pData->Option + pData->d;
		oldx = pOpt->x;
		oldy = pOpt->y;
		oldd = pOpt->d;

		// リングバッファ用カーソルを前方向に移動する //
		pData->d = (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;

		// レーザー頭へのポインタ : pData->d はリングバッファ先頭 //
		pOpt = pData->Option + pData->d;

		// 加速する //
		pData->v = min(HLASER_SPEED * 3, (pData->v) + 256);

//		d    = Atan8((pData->x) - oldx, (pData->y) - oldy);
		d    = Atan8((pData->x) - (pOpt->x), (pData->y) - (pOpt->y));
		ideg = (int)d - (int)oldd;
		if(     ideg < -128) ideg += 256;
		else if(ideg >  128) ideg -= 256;

		// レーザー本体の座標を更新する //
//		pOpt->d = oldd + ((ideg * 2) / 255);	// 角度差の何割かを補正する
		pOpt->d = oldd + ((ideg * 128) / 255);	// 角度差の何割かを補正する
//		pOpt->d = oldd + ((ideg * 48) / 255);	// 角度差の何割かを補正する
		pOpt->x = oldx + CosL(oldd, pData->v);
		pOpt->y = oldy + SinL(oldd, pData->v);

		// 頭が画面外に出たら、消去待ち状態に移行する //
		if((pOpt->y < YMin) || (pOpt->y > YMax)
		|| (pOpt->x < XMin) || (pOpt->x > XMax)){
			// 消去待ちへと移行する //
			pExShot->MoveBack(it, EXST_DELETE);
		}
	}

	// 目標探索状態 //
	ForEachFrontPtr(pExShot, EXST_FINDTARGET, it){
		pData = it.GetPtr();

		pData->Count++;	// カウンタを更新

		// 前回の頭の座標を取得する //
		pOpt = pData->Option + pData->d;
		oldx = pOpt->x;
		oldy = pOpt->y;
		oldd = pOpt->d;

		// リングバッファ用カーソルを前方向に移動する //
		pData->d = (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;

		// レーザー頭へのポインタ : pData->d はリングバッファ先頭 //
		pOpt = pData->Option + pData->d;

		// 減速する //
		pData->v = max(256, (pData->v) - (256+128+196));

		// 角度補正を行う //
		d    = Atan8((pData->x) - oldx, (pData->y) - oldy);
//		d    = Atan8((pData->x) - (pOpt->x), (pData->y) - (pOpt->y));
		ideg = (int)d - (int)oldd;
		if(     ideg < -128) ideg += 256;
		else if(ideg >  128) ideg -= 256;

		if((HitCheckFast((pData->y), (pOpt->y), 12*256) && HitCheckFast((pData->x), (pOpt->x), 12*256))
		|| (pData->Count > 60)){
			PlaySE(SNDID_H_L_B, pOpt->x);
			pExShot->MoveBack(it, EXST_ACCEL);
		}
		else if(abs(ideg) > 6){
//		if(abs(ideg) > 32){
			pOpt->d = oldd + ((ideg * 64) / 255);	// 角度差の何割かを補正する
//			pOpt->d = oldd + ((ideg * 24) / 255);	// 角度差の何割かを補正する
		}
		else{
			// 加速モードへと移行する //
			pOpt->d = d;//oldd;
			PlaySE(SNDID_H_L_B, pOpt->x);
			pExShot->MoveBack(it, EXST_ACCEL);
		}

		// レーザー本体の座標を更新する //
		pOpt->x = oldx + CosL(pOpt->d, pData->v);
		pOpt->y = oldy + SinL(pOpt->d, pData->v);
	}

	// 消去待ち状態 //
	ForEachFrontPtr(pExShot, EXST_DELETE, it){
		pData = it.GetPtr();

		pData->Count++;	// カウンタを更新

		// 前回の尻尾の座標を取得する //
		pOpt = pData->Option + (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;
		oldx = pOpt->x;
		oldy = pOpt->y;

		// 尻尾が画面外に出たら、消去する //
		if((oldy < YMin)){
//		|| (oldx < XMin) || (oldx > XMax)){
			// 実際に消し去る //
			DelContinuePtr(pExShot, it);
		}

		// 前回の頭の座標を取得する(今回の取得は "頭" ) //
		pOpt = pData->Option + pData->d;
		oldx = pOpt->x;
		oldy = pOpt->y;
		oldd = pOpt->d;

		// リングバッファ用カーソルを前方向に移動する //
		pData->d = (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;

		// レーザー頭へのポインタ : pData->d はリングバッファ先頭 //
		pOpt = pData->Option + pData->d;

		// 加速する //
		pData->v = min(HLASER_SPEED * 3, (pData->v) + 256);

		// レーザー本体の座標を更新する //
		pOpt->d = oldd;
		pOpt->x = oldx + CosL(oldd, pData->v);
		pOpt->y = oldy + SinL(oldd, pData->v);
	}



//////////////////////////////////////////////////////////////////////
	// 加速状態 //
	ForEachFrontPtr(pExShot, EXST_BOMBACCEL, it){
		pData = it.GetPtr();

		pData->Count++;	// カウンタを更新

		// 前回の頭の座標を取得する //
		pOpt = pData->Option + pData->d;
		oldx = pOpt->x;
		oldy = pOpt->y;
		oldd = pOpt->d;

		// リングバッファ用カーソルを前方向に移動する //
		pData->d = (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;

		// レーザー頭へのポインタ : pData->d はリングバッファ先頭 //
		pOpt = pData->Option + pData->d;

		// 加速する //
		pData->v = min(HLASER_SPEED * 3, (pData->v) + 256);

//		d    = Atan8((pData->x) - oldx, (pData->y) - oldy);
		d    = Atan8((pData->x) - (pOpt->x), (pData->y) - (pOpt->y));
		ideg = (int)d - (int)oldd;
		if(     ideg < -128) ideg += 256;
		else if(ideg >  128) ideg -= 256;

		// レーザー本体の座標を更新する //
//		pOpt->d = oldd + ((ideg * 2) / 255);	// 角度差の何割かを補正する
		pOpt->d = oldd + ((ideg * 128) / 255);	// 角度差の何割かを補正する
//		pOpt->d = oldd + ((ideg * 48) / 255);	// 角度差の何割かを補正する
		pOpt->x = oldx + CosL(oldd, pData->v);
		pOpt->y = oldy + SinL(oldd, pData->v);

		// 頭が画面外に出たら、消去待ち状態に移行 //
		if((pOpt->y < YMin) || (pOpt->y > YMax)
		|| (pOpt->x < XMin) || (pOpt->x > XMax)){
			// 消去待ちへと移行 //
			pExShot->MoveBack(it, EXST_BOMBDELETE);
		}
	}

	// 目標探索状態 //
	ForEachFrontPtr(pExShot, EXST_BOMBFINDTARGET, it){
		pData = it.GetPtr();

		pData->Count++;	// カウンタを更新

		// 前回の頭の座標を取得する //
		pOpt = pData->Option + pData->d;
		oldx = pOpt->x;
		oldy = pOpt->y;
		oldd = pOpt->d;

		// リングバッファ用カーソルを前方向に移動する //
		pData->d = (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;

		// レーザー頭へのポインタ : pData->d はリングバッファ先頭 //
		pOpt = pData->Option + pData->d;

		// 減速する //
		pData->v = max(256, (pData->v) - (256+128+196));

		// 角度補正を行う //
		d    = Atan8((pData->x) - oldx, (pData->y) - oldy);
//		d    = Atan8((pData->x) - (pOpt->x), (pData->y) - (pOpt->y));
		ideg = (int)d - (int)oldd;
		if(     ideg < -128) ideg += 256;
		else if(ideg >  128) ideg -= 256;

		if((HitCheckFast((pData->y), (pOpt->y), 12*256) && HitCheckFast((pData->x), (pOpt->x), 12*256))
		|| (pData->Count > 60)){
			PlaySE(SNDID_H_L_B, pOpt->x);
			pExShot->MoveBack(it, EXST_BOMBACCEL);
		}
		else if(abs(ideg) > 6){
//		if(abs(ideg) > 32){
			pOpt->d  = oldd + ((ideg * 64) / 255);	// 角度差の何割かを補正する
			pData->v = max(0, pData->v - 32);
//			pOpt->d = oldd + ((ideg * 24) / 255);	// 角度差の何割かを補正する
		}
		else{
			// 加速モードへと移行する //
			pOpt->d = d;//oldd;
			PlaySE(SNDID_H_L_B, pOpt->x);
			pExShot->MoveBack(it, EXST_BOMBACCEL);
		}

		// レーザー本体の座標を更新する //
		pOpt->x = oldx + CosL(pOpt->d, pData->v);
		pOpt->y = oldy + SinL(pOpt->d, pData->v);
	}

	// 消去待ち状態 //
	ForEachFrontPtr(pExShot, EXST_BOMBDELETE, it){
		pData = it.GetPtr();

		pData->Count++;	// カウンタを更新

		// 前回の尻尾の座標を取得する //
		pOpt = pData->Option + (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;
		oldx = pOpt->x;
		oldy = pOpt->y;

		// 尻尾が画面外に出たら、消去する //
		if((oldy < YMin) || (oldy > YMax)
		|| (oldx < XMin) || (oldx > XMax)){
			// 実際に消し去る //
			DelContinuePtr(pExShot, it);
		}

		// 前回の頭の座標を取得する(今回の取得は "頭") //
		pOpt = pData->Option + pData->d;
		oldx = pOpt->x;
		oldy = pOpt->y;
		oldd = pOpt->d;

		// リングバッファ用カーソルを前方向に移動する //
		pData->d = (pData->d + HLASER_LENGTH - 1) % HLASER_LENGTH;

		// レーザー頭へのポインタ : pData->d はリングバッファ先頭 //
		pOpt = pData->Option + pData->d;

		// 加速する //
		pData->v = min(HLASER_SPEED * 3, (pData->v) + 256);

		// レーザー本体の座標を更新 //
		pOpt->d = oldd;
		pOpt->x = oldx + CosL(oldd, pData->v);
		pOpt->y = oldy + SinL(oldd, pData->v);
	}
}


// エキストラショットの当たり判定を行う //
FDWORD CStg5Boss::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot, *pLockMark;
	ExtraOption				*pOpt;
	CExtraShot::Iterator	it;
	int						x, y, i, j, n;
	int						Size;
	DWORD					Damage = 0;

	const int HLHitChkID[3] = {
		EXST_FINDTARGET, EXST_ACCEL, EXST_DELETE
	};

	const int HLBombHitChkID[3] = {
		EXST_BOMBFINDTARGET, EXST_BOMBACCEL, EXST_BOMBDELETE
	};

	x    = pEnemy->x;				// 敵のＸ座標
	y    = pEnemy->y;				// 敵のＹ座標
	Size = pEnemy->size + 16 * 256;	// ホーミングレーザーの当たり判定幅

	for(i=0; i<3; i++){
		n = HLHitChkID[i];
		ForEachFrontPtr(pExtraShot, n, it){
			pShot = it.GetPtr();
			pOpt  = pShot->Option;

			for(j=0; j<HLASER_LENGTH; j++){
				// レーザーの一部が当たっていたら抜ける //
				if(HitCheckFast(y, (pOpt[j].y), Size)
				&& HitCheckFast(x, (pOpt[j].x), Size)){
					Damage += HLASER_DAMAGE;
					break;
				}
			}
		}

		n = HLBombHitChkID[i];
		ForEachFrontPtr(pExtraShot, n, it){
			pShot = it.GetPtr();
			pOpt  = pShot->Option;

			for(j=0; j<HLASER_LENGTH; j++){
				// レーザーの一部が当たっていたら抜ける //
				if(HitCheckFast(y, (pOpt[j].y), Size)
				&& HitCheckFast(x, (pOpt[j].x), Size)){
					Damage += HLASERBOMB_DAMAGE;
					break;
				}
			}
		}
	}


	Size = pEnemy->size + 8 * 256;	// ロックオンサイトの当たり判定幅

	// ロックオンサイトと敵の当たり判定 //
	pExtraShot->GetFrontIterator(it, EXST_LOCKONSITE);
	if(it == NULL) return Damage;

	pShot = it.GetPtr();
	if(pShot->Count) return Damage;

	if(HitCheckFast(y, (pShot->y), Size)
	&& HitCheckFast(x, (pShot->x), Size)){
		PlaySE(SNDID_LOCKON, x);
		pShot->Count = LOCKON_INTERVAL;

		pLockMark = pExtraShot->InsertFront(EXST_LOCKED);

		if(pLockMark){
			pLockMark->x     = pShot->x;
			pLockMark->y     = pShot->y;
			pLockMark->Count = 0;
		}
	}

	return Damage;
}


// エキストラショットを描画する //
FVOID CStg5Boss::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	D3DVALUE				u0, v0, u1, v1;
	D3DTLVERTEX				tlv[20];
	int						x, y;
	DWORD					c;

	// エキストラショット格納クラスに接続 //
	pExShot = pExShotInfo->pExtraShot;

	// 描画の前準備 //
	g_pGrp->SetTexture(TextureID);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	// ロックオンサイト //
	ForEachFrontPtr(pExShot, EXST_LOCKONSITE, it){
		if(it->Count) continue;

		x = ((it->x) >> 8);
		y = ((it->y) >> 8);

		u0 = D3DVAL( 16) / D3DVAL(256);
		v0 = D3DVAL(120) / D3DVAL(256);
		u1 = D3DVAL( 40) / D3DVAL(256);
		v1 = D3DVAL(144) / D3DVAL(256);

		c = RGBA_MAKE(255, 255, 255, 255);
		Set2DPointC(tlv+0, x-16, y-16, u0, v0, c);
		Set2DPointC(tlv+1, x+16, y-16, u1, v0, c);
		Set2DPointC(tlv+2, x+16, y+16, u1, v1, c);
		Set2DPointC(tlv+3, x-16, y+16, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// 目標探し //
	ForEachFrontPtr(pExShot, EXST_FINDTARGET, it){
		DrawHLaser(it.GetPtr());
	}

	// 加速中 //
	ForEachFrontPtr(pExShot, EXST_ACCEL, it){
		DrawHLaser(it.GetPtr());
	}

	// 消去待ち //
	ForEachFrontPtr(pExShot, EXST_DELETE, it){
		DrawHLaser(it.GetPtr());
	}

	// 目標探し //
	ForEachFrontPtr(pExShot, EXST_BOMBFINDTARGET, it){
		DrawBombHLaser(it.GetPtr());
	}

	// 加速中 //
	ForEachFrontPtr(pExShot, EXST_BOMBACCEL, it){
		DrawBombHLaser(it.GetPtr());
	}

	// 消去待ち //
	ForEachFrontPtr(pExShot, EXST_BOMBDELETE, it){
		DrawBombHLaser(it.GetPtr());
	}

	// ロックオンマーカー //
	ForEachFrontPtr(pExShot, EXST_LOCKED, it){
		if(it->Count < LOCKONSITE_TIME) g_pGrp->SetTexture(GRPTEXTURE_MAX);
		else                            g_pGrp->SetTexture(TextureID);

		DrawLockOnMarker(it.GetPtr());
	}
}


// 通常の溜め攻撃(レベル１) //
FVOID CStg5Boss::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	ExtraOption		*pOpt;
	CExtraShot		*pExShot;
	WORD			KeyCode;
	int				mx, my, tx, ty;
	int				j;
	BYTE			d;

	// １／２まで溜まっていないので発動不可 //
//	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
	my      = *(pExShot->GetYPointer());		// 現在のＹ座標

	// 仮のホーミング目標 //
	tx = (pExShot->GetXMin() + pExShot->GetXMax()) / 2;
	ty = 0;

	KeyCode = pExShotInfo->KeyCode;

	// ロックオンマーカー //
	CExtraShot::Iterator	it;
	ForEachFrontPtr(pExShot, EXST_LOCKED, it){
		pData = it.GetPtr();

//		pData->x     = tx;				// 目標のＸ座標
//		pData->y     = ty;				// 目標のＹ座標
		pData->v     = HLASER_SPEED;	// 初速度
		pData->Count = 0;				// カウンタ
		pData->d     = 0;				// リングバッファ用カーソル

		// ホーミングレーザーの発射角を決める //
		d = 64 - 32 + RndEx() % 64;

		// 実際のレーザー格納域の処理 //
		pOpt = pData->Option;
		for(j=0; j<HLASER_LENGTH; j++, pOpt++){
			pOpt->x = mx;
			pOpt->y = my;
			pOpt->d = d;
		}

		pExShot->MoveBack(it, EXST_FINDTARGET);
	}
	return;
/*
	// ホーミングレーザーの本数を決める //
	n = (pExShotInfo->Charge < CGAUGE_LEVEL1) ? 2 : 6;

	for(i=0; i<n; i++){
		pData = pExShot->InsertBack(EXST_FINDTARGET);
		if(NULL == pData) return;

		pData->x     = tx;				// 目標のＸ座標
		pData->y     = ty;				// 目標のＹ座標
		pData->v     = HLASER_SPEED;	// 初速度
		pData->Count = 0;				// カウンタ
		pData->d     = 0;				// リングバッファ用カーソル

		// ホーミングレーザーの発射角を決める //
		d = 64 + ((i - n / 2) * 32 + 16);
//		d = 64 + ((i - n / 2) * 12 + 6);

		// 実際のレーザー格納域の処理 //
		pOpt = pData->Option;
		for(j=0; j<HLASER_LENGTH; j++, pOpt++){
			pOpt->x = mx;
			pOpt->y = my;
			pOpt->d = d;
		}
	}
*/
}


// ボムアタック(レベル１) //
FBOOL CStg5Boss::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CStg5Boss::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	CExtraShot::Iterator		it;

	// 目標探し //
	ForEachFrontPtr(pExtraShot, EXST_BOMBFINDTARGET, it){
		return FALSE;
	}

	// 加速中 //
	ForEachFrontPtr(pExtraShot, EXST_BOMBACCEL, it){
		return FALSE;
	}

	// 消去待ち //
	ForEachFrontPtr(pExtraShot, EXST_BOMBDELETE, it){
		return FALSE;
	}

	return TRUE;
}


// 通常ボム //
FVOID CStg5Boss::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	ExtraOption		*pOpt;
	CExtraShot		*pExShot;
	WORD			KeyCode;
	int				mx, my, tx, ty;
	int				i, j;
	BYTE			d;

	pBGDraw->SetBlendColor(50, 130, 50, m_BombTime - 60);

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
	my      = *(pExShot->GetYPointer());		// 現在のＹ座標

	// 仮のホーミング目標 //
	tx = (pExShot->GetXMin() + pExShot->GetXMax()) / 2;
	ty = 0;

	KeyCode = pExShotInfo->KeyCode;

	for(i=0; i<50; i++){
		pData = pExShot->InsertFront(EXST_BOMBFINDTARGET);
		if(NULL == pData) return;

		pData->v     = HLASER_SPEED;	// 初速度
		pData->Count = 0;				// カウンタ
		pData->d     = 0;				// リングバッファ用カーソル
		pData->x     = mx+CosL(i*256/50, 150*256);
		pData->y     = my+SinL(i*256/50, 150*256);

		// ホーミングレーザーの発射角を決める //
		d = (BYTE)RndEx();

		// 実際のレーザー格納域の処理 //
		pOpt = pData->Option;
		for(j=0; j<HLASER_LENGTH; j++, pOpt++){
			pOpt->x = mx;
			pOpt->y = my;
			pOpt->d = d;
		}
	}
}

/*
// 攻撃送り用グラフィックの状態推移用関数(初期化) //
FVOID CStg5Boss::InitAtkGrp(AtkGrpInfo *pInfo, DWORD AtkLv)
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
FBOOL CStg5Boss::Lv2AtkGrpMove(AtkGrpInfo *pInfo)
{
	return FALSE;
}


// 攻撃送り用グラフィックの状態推移用関数(レベル３) //
FBOOL CStg5Boss::Lv3AtkGrpMove(AtkGrpInfo *pInfo)
{
	return FALSE;
}


// 攻撃送り用グラフィックの状態推移用関数(レベル４) //
FBOOL CStg5Boss::Lv4AtkGrpMove(AtkGrpInfo *pInfo)
{
	return FALSE;
}
*/

// ホーミングレーザーを描画する //
FVOID CStg5Boss::DrawHLaser(ExtraShot *pExShot)
{
	D3DTLVERTEX		tlv[HLASER_LENGTH * 2 + 2];
	ExtraOption		*pOpt, *pBase;
	D3DVALUE		u0, v0, u1, v1;
	int				i, n, t, delta;
	int				x, y, dx, dy, lx,ly;
	DWORD			c;

#define HLASER_WIDTH	(8 * 256)

	n     = pExShot->d;
	pBase = pExShot->Option;
	c     = RGBA_MAKE(255, 255, 255, 255);

	// テクスチャ座標を初期化する //
	u0 = D3DVAL(16)    / D3DVAL(256);
	v0 = D3DVAL(80)    / D3DVAL(256);
	u1 = D3DVAL(16+16) / D3DVAL(256);
	v1 = D3DVAL(80+32) / D3DVAL(256);

	// 頭の座標をセットする //
	pOpt  = pBase + n;
	x     = pOpt->x;
	y     = pOpt->y;
	dx    = CosL(pOpt->d + 64, HLASER_WIDTH);
	dy    = SinL(pOpt->d + 64, HLASER_WIDTH);
	lx    = CosL(pOpt->d, HLASER_WIDTH * 2);
	ly    = SinL(pOpt->d, HLASER_WIDTH * 2);
	Set2DPointC(tlv+0, (x-dx+lx)>>8, (y-dy+ly)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+dx+lx)>>8, (y+dy+ly)>>8, u0, v1, c);
	Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v0, c);
	Set2DPointC(tlv+3, (x+dx)>>8, (y+dy)>>8, u1, v1, c);

	// ２番目以降の尻尾の座標をセットする //
	for(i=2; i<HLASER_LENGTH*2; i+=2){
		// n = (n + 1) % HLASER_LENGTH;
		if((++n) >= HLASER_LENGTH) n = 0;

		// レーザーの太さを変化させるためにテクスチャの座標を変化させる //
		delta = (16 * i) / (HLASER_LENGTH * 2);
		u1    = D3DVAL(16+ (32/2) + delta) / D3DVAL(256);

		pOpt = pBase + n;
		x    = pOpt->x;
		y    = pOpt->y;
		dx   = CosL(pOpt->d + 64, HLASER_WIDTH);
		dy   = SinL(pOpt->d + 64, HLASER_WIDTH);
		t    = 255 - (255 * i) / (HLASER_LENGTH * 2);
		c    = RGBA_MAKE(255, 196, t, 255);

		Set2DPointC(tlv+i+2, (x-dx)>>8, (y-dy)>>8, u1, v0, c);
		Set2DPointC(tlv+i+3, (x+dx)>>8, (y+dy)>>8, u1, v1, c);
	}

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLESTRIP, tlv, HLASER_LENGTH * 2 + 2);
}


// ホーミングレーザーを描画する //
FVOID CStg5Boss::DrawBombHLaser(ExtraShot *pExShot)
{
	D3DTLVERTEX		tlv[HLASER_LENGTH * 2 + 2];
	ExtraOption		*pOpt, *pBase;
	D3DVALUE		u0, v0, u1, v1;
	int				i, n, t, delta;
	int				x, y, dx, dy, lx,ly;
	DWORD			c;

#define HLASERBOMB_WIDTH	(4 * 256)

	n     = pExShot->d;
	pBase = pExShot->Option;
	c     = RGBA_MAKE(255, 255, 255, 255);

	// テクスチャ座標を初期化する //
	u0 = D3DVAL(16)    / D3DVAL(256);
	v0 = D3DVAL(80)    / D3DVAL(256);
	u1 = D3DVAL(16+16) / D3DVAL(256);
	v1 = D3DVAL(80+32) / D3DVAL(256);

	// 頭の座標をセットする //
	pOpt  = pBase + n;
	x     = pOpt->x;
	y     = pOpt->y;
	dx    = CosL(pOpt->d + 64, HLASERBOMB_WIDTH);
	dy    = SinL(pOpt->d + 64, HLASERBOMB_WIDTH);
	lx    = CosL(pOpt->d, HLASERBOMB_WIDTH * 2);
	ly    = SinL(pOpt->d, HLASERBOMB_WIDTH * 2);
	Set2DPointC(tlv+0, (x-dx+lx)>>8, (y-dy+ly)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (x+dx+lx)>>8, (y+dy+ly)>>8, u0, v1, c);
	Set2DPointC(tlv+2, (x-dx)>>8, (y-dy)>>8, u1, v0, c);
	Set2DPointC(tlv+3, (x+dx)>>8, (y+dy)>>8, u1, v1, c);

	// ２番目以降の尻尾の座標をセットする //
	for(i=2; i<HLASER_LENGTH*2; i+=2){
		// n = (n + 1) % HLASER_LENGTH;
		if((++n) >= HLASER_LENGTH) n = 0;

		// レーザーの太さを変化させるためにテクスチャの座標を変化させる //
		delta = (16 * i) / (HLASER_LENGTH * 2);
		u1    = D3DVAL(16+ (32/2) + delta) / D3DVAL(256);

		pOpt = pBase + n;
		x    = pOpt->x;
		y    = pOpt->y;
		dx   = CosL(pOpt->d + 64, HLASERBOMB_WIDTH);
		dy   = SinL(pOpt->d + 64, HLASERBOMB_WIDTH);
		t    = 255 - (255 * i) / (HLASER_LENGTH * 2);
		c    = RGBA_MAKE(t, 200, t, 255);

		Set2DPointC(tlv+i+2, (x-dx)>>8, (y-dy)>>8, u1, v0, c);
		Set2DPointC(tlv+i+3, (x+dx)>>8, (y+dy)>>8, u1, v1, c);
	}

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLESTRIP, tlv, HLASER_LENGTH * 2 + 2);
}


// ロックオンマーカーを描画する //
FVOID CStg5Boss::DrawLockOnMarker(ExtraShot *pExShot)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	BYTE			d;
	int				l, i;
	int				ox, oy, x, y;
	DWORD			c;

	ox = pExShot->x;
	oy = pExShot->y;

	// ぐるぐるまわる三角形の枠 //
	if(pExShot->Count < LOCKONSITE_TIME){
		d  = (BYTE)pExShot->Count * 6;
		l  = (100 - pExShot->Count * 5) * 256;
		c  = RGBA_MAKE(0, 0, 255, 255);

		for(i=0; i<3; i++, d+=85){
			x = (ox + CosL(d, l)) >> 8;
			y = (oy + SinL(d, l)) >> 8;
			Set2DPointC(tlv+i, x, y, 0, 0, c);
		}
		tlv[3] = tlv[0];

		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
		g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 4);
	}
	// ロック終了の証 //
	else{
		u0 = D3DVAL( 56) / 256;
		v0 = D3DVAL(120) / 256;
		u1 = D3DVAL(104) / 256;
		v1 = D3DVAL(144) / 256;
		c  = RGBA_MAKE(200, 200, 210, 255);

		ox >>= 8;
		oy >>= 8;

		Set2DPointC(tlv+0, ox-12, oy-12, u0, v0, c);
		Set2DPointC(tlv+1, ox+36, oy-12, u1, v0, c);
		Set2DPointC(tlv+2, ox+36, oy+12, u1, v1, c);
		Set2DPointC(tlv+3, ox-12, oy+12, u0, v1, c);

		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
