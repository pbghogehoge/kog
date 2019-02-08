/*
 *   CStg3Boss.cpp   : ゲイツの定義
 *
 */

#include "CStg3Boss.h"
#include "Gian2001.h"

#include "SECtrl.h"

#include "RndCtrl.h"



/***** [グローバル変数] *****/
CStg3Boss		g_Stg3BossInfo;			// 主人公の情報格納用クラス



/***** [ 定数 ] *****/
#define STG3_EXSHOT_NORMAL	0x00		// エキストラショット動作中
#define STG3_EXSHOT_WAIT	0x01		// エキストラショット発射待ち
#define STG3_EXSHOT_DELETE	0x02		// エキストラショット消去中
#define STG3_EXSHOT_BOMB	0x03		// ボム・発動準備中
#define STG3_EXSHOT_RING	0x04		// ボム・でっかいわっか

#define STG3_EXSHOT_SPEED	(4*256)		// エキストラショットの初速度

#define STG3_EXSHOT_SIZE	(8*256)		// エキストラショットの基本サイズ
#define STG3_EXSHOT_DAMAGE	(256)		// エキストラショットのダメージ

#define STG3_EXOPTION_MAX	(EXTRAOPTION_MAX / 5)	//(EXTRAOPTION_MAX / 3)
#define STG3_EXRING_MAX		3		//(EXTRAOPTION_MAX / 5)

#define STG3_GST_RANGE			4
//#define STG3_GST_CENTER		0x00		// 中央表示のグラフィック
//#define STG3_GST_LEFT1		0x01		// 左移動のグラフィック
//#define STG3_GST_RIGHT1		0x02		// 右移動のグラフィック




// コンストラクタ //
CStg3Boss::CStg3Boss()
{
	m_NormalSpeed = 3 * 256;		// 通常時の早さ
	m_ShiftSpeed  = 3 * 256 / 2;	// シフト移動時の早さ
	m_ChargeSpeed = 200 * 256;		// 溜めの早さ
//	m_BuzzSpeed   = 65536;			// カスり溜めの早さ
	m_BuzzSpeed   = 98000;				// カスり溜めの早さ

	m_CharID       = CHARACTER_STG3;	// キャラクタＩＤ
	m_NormalShotDx = 0;					// ショットの発射座標
	m_NormalShotDy = 0;					// ショットの発射座標

	m_BombTime = 120;		// ボムの有効時間
}


// デストラクタ //
CStg3Boss::~CStg3Boss()
{
}


// 描画する //
FVOID CStg3Boss::PlayerDraw(
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
		u0 = D3DVAL(192) / D3DVAL(256);
		u1 = D3DVAL(256) / D3DVAL(256);
		v0 = D3DVAL( 56) / D3DVAL(256);
		v1 = D3DVAL(112) / D3DVAL(256);
	}
	else if(State < 0){
		if(-STG3_GST_RANGE != State){	// 左移動中
			u0 = D3DVAL(  0) / D3DVAL(256);
			u1 = D3DVAL( 64) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 左ＭＡＸ
			u0 = D3DVAL( 64) / D3DVAL(256);
			u1 = D3DVAL(128) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
	}
	else{
		if(STG3_GST_RANGE != State){	// 右移動中
			u0 = D3DVAL(128) / D3DVAL(256);
			u1 = D3DVAL(192) / D3DVAL(256);
			v0 = D3DVAL(  0) / D3DVAL(256);
			v1 = D3DVAL( 56) / D3DVAL(256);
		}
		else{							// 右ＭＡＸ
			u0 = D3DVAL(192)  / D3DVAL(256);
			u1 = D3DVAL(256) / D3DVAL(256);
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





/*
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	int				x, y;
	int				dx, dy;
	DWORD			c;

	c  = RGBA_MAKE(255, 255, 255, 245);

	// 描画状態により、関数の振り分けを行う //
	switch(State){
		case STG3_GST_CENTER:
			u0 = D3DVAL(0)  / D3DVAL(256);
			u1 = D3DVAL(72) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(56) / D3DVAL(256);
		break;

		case STG3_GST_RIGHT1:
			u0 = D3DVAL(72)  / D3DVAL(256);
			u1 = D3DVAL(144) / D3DVAL(256);
			v0 = D3DVAL(0)   / D3DVAL(256);
			v1 = D3DVAL(56)  / D3DVAL(256);
		break;

		case STG3_GST_LEFT1:
			u0 = D3DVAL(144) / D3DVAL(256);
			u1 = D3DVAL(216) / D3DVAL(256);
			v0 = D3DVAL(0)   / D3DVAL(256);
			v1 = D3DVAL(56)  / D3DVAL(256);
		break;
	}

	dx = 23 * 256;
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
*/
}


// 描画用のＩＤを変更する //
FVOID CStg3Boss::PlayerSetGrp(int *pState, WORD KeyCode)
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
		if(*pState > -STG3_GST_RANGE) (*pState)--;
	}
	// 右方向に移動中 //
	else{
		if(*pState <  STG3_GST_RANGE) (*pState)++;
	}
/*
	int			dir = 0;


	if(KeyCode & KEY_LEFT)  dir--;
	if(KeyCode & KEY_RIGHT) dir++;

	if(dir){
		if(dir < 0) *pState = STG3_GST_LEFT1;
		else        *pState = STG3_GST_RIGHT1;
	}
	else{
		*pState = STG3_GST_CENTER;
	}*/
}


// エキストラショットを動作させる //
FVOID CStg3Boss::MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)
{
	int			mx, my;
	int			i;
	int			YMin;
	WORD		KeyCode;
	BYTE		d;
	char		delta;
	int			dw, t;
	BOOL		bNeedDelete;

	CExtraShot				*pExShot;
	CExtraShot::Iterator	it;
	ExtraShot				*pData;
	ExtraOption				*pOpt;


	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());
	my      = *(pExShot->GetYPointer()) + STG3_EXSHOT_SIZE*2;
	KeyCode = pExShotInfo->KeyCode;
	YMin    = pExShot->GetYMin() - (20 * 256);

	delta = -64;
	d     = 0;

	if(KeyCode & KEY_LEFT)  d -= 2;
	if(KeyCode & KEY_RIGHT) d += 2;

	// エキストラショット発射待ち //
	ForEachFrontPtr(pExShot, STG3_EXSHOT_WAIT, it){
		pData = it.GetPtr();

		if(pData->Count) pData->Count--;
		else{
			pData->x = mx;
			pData->y = my;

			pOpt = pData->Option;
			for(i=0; i<STG3_EXOPTION_MAX; i++){
				pOpt[i].d = delta + (RndEx()%32 - 16);
				pOpt[i].x = 0;
				pOpt[i].y = 0;
				pOpt[i].a = 16 + RndEx()%96;
			}
			pExShot->MoveBack(it, STG3_EXSHOT_NORMAL);
		}

		delta = pData->d;
		if(KeyCode & KEY_LEFT)  delta -= 2;
		if(KeyCode & KEY_RIGHT) delta += 2;
	}

	// ボム発射待ち //
	ForEachFrontPtr(pExShot, STG3_EXSHOT_BOMB, it){
		pData = it.GetPtr();

		if(pData->Count) pData->Count--;
		else{
			pData->x = mx;
			pData->y = my;

			pOpt = pData->Option;

			d = pData->d - 64;
			if(d > 128) dw = 130 - (256 - d) / 2;
			else        dw = 130 - d / 2;

			for(i=0; i<STG3_EXOPTION_MAX; i++){
				pOpt[i].x = 0;
				pOpt[i].y = 0;

				t = 8 + RndEx()%dw;
				pOpt[i].a = t;
				pOpt[i].d = pData->d + RndEx()%t - t/2;
			}
			pExShot->MoveBack(it, STG3_EXSHOT_NORMAL);
		}
	}

	// 通常動作中 //
	ForEachFrontPtr(pExShot, STG3_EXSHOT_NORMAL, it){
		pData = it.GetPtr();

		pData->v -= 16;

		pData->x += CosL(pData->d, pData->v);
		pData->y += SinL(pData->d, pData->v);

		if(pData->y < YMin){
			DelContinuePtr(pExShot, it);
		}

		pOpt = pData->Option;

		pData->Count++;
		if(pData->Count < 32){
			pOpt = pData->Option;
			for(i=0; i<STG3_EXOPTION_MAX; i++){
				d = pData->d - pOpt[i].d;
				if((pData->Count % 4 == 0) && d){
					if((char)d < 0) pOpt[i].d-=1;
					else            pOpt[i].d+=1;
				}

				if(pOpt[i].a){
					//pOpt[i].d += d;
					pOpt[i].x += CosL(pOpt[i].d, pOpt[i].a * 8);
					pOpt[i].y += SinL(pOpt[i].d, pOpt[i].a * 8);
					pOpt[i].a--;
				}
			}
		}
		else{
			pData->Count = 32;
			pExShot->MoveBack(it, STG3_EXSHOT_DELETE);
		}
	}

	// 消去中 //
	ForEachFrontPtr(pExShot, STG3_EXSHOT_DELETE, it){
		pData = it.GetPtr();

		pData->v -= 16;

		pData->x += CosL(pData->d, pData->v);
		pData->y += SinL(pData->d, pData->v);

		if(pData->y < YMin){
			DelContinuePtr(pExShot, it);
		}

		pData->Count--;
		if(0 == pData->Count) DelContinuePtr(pExShot, it);

		pOpt = pData->Option;
		for(i=0; i<STG3_EXOPTION_MAX; i++){
			d = pData->d - pOpt[i].d;
			if(d){
				if((char)d < 0) pOpt[i].d-=1;
				else            pOpt[i].d+=1;
			}

			if(pOpt[i].a){
				pOpt[i].x += CosL(pOpt[i].d, pOpt[i].a * 8);
				pOpt[i].y += SinL(pOpt[i].d, pOpt[i].a * 8);
				pOpt[i].a--;
			}
		}
	}

	// ボム用・でっかいわっか //
	ForEachFrontPtr(pExShot, STG3_EXSHOT_RING, it){
		pData = it.GetPtr();

		bNeedDelete = TRUE;

		for(i=0; i<STG3_EXRING_MAX; i++){
			pOpt = pData->Option + i;

			if(pOpt->a){
				bNeedDelete = FALSE;

				if(0 == pOpt->count){
					pOpt->a  = max(0, (int)pOpt->a - 10/2);
				}
				else{
					pOpt->x = mx;
					pOpt->y = my;

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
FDWORD CStg3Boss::HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy)
{
	ExtraShot				*pShot;
	ExtraOption				*pOpt;
	CExtraShot::Iterator	it;

//	DWORD		Damage;
	int			x, y, ox, oy, tx, ty;
	int			i;
	int			Size;


//	Damage = 0;				// ダメージをゼロ初期化
	x      = pEnemy->x;		// 敵のＸ座標
	y      = pEnemy->y;		// 敵のＹ座標

	// 当たり判定サイズ(後で変更のこと) //
	Size = STG3_EXSHOT_SIZE + pEnemy->size;

	ForEachFrontPtr(pExtraShot, STG3_EXSHOT_NORMAL, it){
		pShot = it.GetPtr();		// ポインタ取得

		pOpt  = pShot->Option;		// 炎それぞれのローカル座標
		ox    = pShot->x;			// 広がり中心のＸ座標
		oy    = pShot->y;			// 広がり中心のＹ座標

		for(i=0; i<STG3_EXOPTION_MAX; i++){
			tx = ox + pOpt[i].x;
			ty = oy + pOpt[i].y;

			if(HitCheckFast(y, ty, Size)
			&& HitCheckFast(x, tx, Size)){
				return STG3_EXSHOT_DAMAGE;
//				Damage += STG3_EXSHOT_DAMAGE;
//				break;		// 一つの塊につき、１ヒットまで
			}
		}
	}

	ForEachFrontPtr(pExtraShot, STG3_EXSHOT_DELETE, it){
		pShot = it.GetPtr();		// ポインタ取得

		pOpt  = pShot->Option;		// 炎それぞれのローカル座標
		ox    = pShot->x;			// 広がり中心のＸ座標
		oy    = pShot->y;			// 広がり中心のＹ座標

		for(i=0; i<STG3_EXOPTION_MAX; i++){
			tx = ox + pOpt[i].x;
			ty = oy + pOpt[i].y;

			if(HitCheckFast(y, ty, Size)
			&& HitCheckFast(x, tx, Size)){
				return STG3_EXSHOT_DAMAGE;
//				Damage += STG3_EXSHOT_DAMAGE;
//				break;		// 一つの塊につき、１ヒットまで
			}
		}
	}

	return 0;
//	return Damage;
}


// エキストラショットを描画する //
FVOID CStg3Boss::DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID)
{
	int				mx, my;		// 座標基本値
	int				i;
	DWORD			count;		// 経過カウント

	CExtraShot					*pExShot;	// エキストラショットコンテナ
	CExtraShot::Iterator		it;			// 上の奴のイテレータ
	ExtraOption					*pOpt;

	int				x256, y256;			// 基本座標値

	// 構造体へのポインタから諸変数を取得する //
	pExShot = pExShotInfo->pExtraShot;	// エキストラショット格納先

	// 座標値を取得する //
	mx = *(pExShot->GetXPointer());		// 現在のＸ座標を取得
	my = *(pExShot->GetYPointer());		// 現在のＹ座標を取得

	// 使用するテクスチャとレンダリングステートの初期化 //
	g_pGrp->SetTexture(TextureID);				// テクスチャセット
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);	// 描画ステートセット

	// 火炎放射：通常状態 //
	ForEachFrontPtr(pExShotInfo->pExtraShot, STG3_EXSHOT_NORMAL, it){
		// 基本となる座標＆角度をセットする //
		x256 = it->x;	// Ｘ座標
		y256 = it->y;	// Ｙ座標

		// テクスチャ座標をセットする //
		count = max(it->Count / 4, 0) * 16;

		//DrawFire(x256, y256, count);

		pOpt = it.GetPtr()->Option;
		for(i=0; i<STG3_EXOPTION_MAX; i++){
			DrawFire(x256+ pOpt[i].x, y256+pOpt[i].y, count);
		}
	}

	// 火炎放射：消去状態 //
	ForEachFrontPtr(pExShotInfo->pExtraShot, STG3_EXSHOT_DELETE, it){
		// 基本となる座標＆角度をセット //
		x256 = it->x;	// Ｘ座標
		y256 = it->y;	// Ｙ座標

		// テクスチャ座標をセット //
		count = max((it->Count / 4), 0) * 16;

		//DrawFire(x256, y256, count);

		pOpt = it.GetPtr()->Option;
		for(i=0; i<STG3_EXOPTION_MAX; i++){
			DrawFire(x256+pOpt[i].x, y256+pOpt[i].y, count);
		}
	}

	g_pGrp->SetTexture(TEXTURE_ID_GUARD);		// テクスチャセット

	// ボム演出用のわっかを描画する //
	ForEachFrontPtr(pExShotInfo->pExtraShot, STG3_EXSHOT_RING, it){
		DrawExRing(it->Option);
	}
}


// 通常の溜め攻撃(レベル１) //
FVOID CStg3Boss::Level1Attack(ExtraShotInfo *pExShotInfo)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	WORD			KeyCode;
	int				mx, my;
	int				i, n, v;

	// １／２まで溜まっていないので発動不可 //
	if(pExShotInfo->Charge < CGAUGE_LV1HALF) return;

	pExShot = pExShotInfo->pExtraShot;

	mx      = *(pExShot->GetXPointer());						// 現在のＸ座標
	my      = *(pExShot->GetYPointer()) + STG3_EXSHOT_SIZE*2;	// 現在のＹ座標
	KeyCode = pExShotInfo->KeyCode;

	// 火炎放射の持続時間を求める //
	n = (pExShotInfo->Charge < CGAUGE_LEVEL1) ? 8 : 19;

	v = STG3_EXSHOT_SPEED;
	if(KeyCode & KEY_UP){
		v -= (256*2);
		n += 5;
	}

	if(KeyCode & KEY_DOWN){
		v += (256*2);
		n -= 5;
	}


	// 火種をセット：ダミー値が多いのは、火炎放射が持続系のショット //
	//               となっている為である。                         //

	PlaySE(SNDID_G_FLAME, mx);

	for(i=0; i<n; i++){
		pData = pExShot->InsertBack(STG3_EXSHOT_WAIT);
		if(NULL == pData) return;

		pData->v = v;	// 初速度

		pData->Count = 2 * i;	// 発射までの待ちカウント
		pData->d     = -64;		// 発射方向(この時点ではダミーだが...)
		pData->x     = mx;		// 初期Ｘ座標(ダミー)
		pData->y     = my;		// 初期Ｙ座標(ダミー)
	}
}


// ボムアタック(レベル１) //
FBOOL CStg3Boss::Level1BombAtk(ExtraShotInfo *pExShotInfo)
{
	return FALSE;
}


// ノーマルショットが撃てれば真を返す //
FBOOL CStg3Boss::IsEnableNormalShot(CExtraShot *pExtraShot)
{
	CExtraShot::Iterator		it;			// 上の奴のイテレータ

	ForEachFrontPtr(pExtraShot, STG3_EXSHOT_WAIT, it){
		return FALSE;
	}

	ForEachFrontPtr(pExtraShot, STG3_EXSHOT_NORMAL, it){
		return FALSE;
	}
/*
	ForEachFrontPtr(pExtraShot, STG3_EXSHOT_DELETE, it){
		return FALSE;
	}
*/
	return TRUE;
}


// 通常ボム //
FVOID CStg3Boss::NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw)
{
	ExtraShot		*pData;
	CExtraShot		*pExShot;
	ExtraOption		*pOpt;
//	WORD			KeyCode;
	int				mx, my;
	int				i, j, t, v;
	BYTE			d;

	pBGDraw->SetBlendColor(255, 128, 128, m_BombTime - 60);

	pExShot = pExShotInfo->pExtraShot;
	mx      = *(pExShot->GetXPointer());		// 現在のＸ座標
	my      = *(pExShot->GetYPointer());		// 現在のＹ座標

	// 効果用リングをセット //
	pData = pExShot->InsertBack(STG3_EXSHOT_RING);
	if(NULL == pData) return;
	for(j=0; j<STG3_EXRING_MAX; j++){
		pOpt = pData->Option + j;

		pOpt->a     = 255;
		pOpt->d     = (BYTE)RndEx()%96 + 16;
		pOpt->count = (j * (m_BombTime-60)) / STG3_EXRING_MAX;
		pOpt->x     = mx;
		pOpt->y     = my;
	}

	// 火種をセット：ダミー値が多いのは、火炎放射が持続系のショット //
	//               となっている為である。                         //
	for(i=0; i<40; i++){
		pData = pExShot->InsertBack(STG3_EXSHOT_BOMB);
		if(NULL == pData) return;

		if(i & 1) d = -64 + 32 + (i/2) * 5;
		else      d = -64 - 32 - (i/2) * 5;
		t = (i/2) * 2;
		v = STG3_EXSHOT_SPEED - (i/2) * 128;

		pData->v     = v;		// 初速度
		pData->Count = t;		// 発射までの待ちカウント
		pData->d     = d;		// 発射方向(この時点ではダミー)
		pData->x     = mx;		// 初期Ｘ座標(ダミー)
		pData->y     = my;		// 初期Ｙ座標(ダミー)
	}
}


// 火炎粒子を描画する //
FVOID CStg3Boss::DrawFire(int x256, int y256, int n)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
	D3DVALUE		u0, v0, u1, v1;		// テクスチャ座標
	int				x, y;				// 描画座標値
	int				size;
	DWORD			c;
	BYTE			t;

	u0 = D3DVAL(n)     / D3DVAL(256);
	u1 = D3DVAL(n+16)  / D3DVAL(256);
	v0 = D3DVAL(64)    / D3DVAL(256);
	v1 = D3DVAL(64+16) / D3DVAL(256);

	size = STG3_EXSHOT_SIZE + (n / 16) * (256 + 128) * 2;
//	t    = min(n/3+16, 255);
	t    = min(n/4+16, 255);
	c    = RGBA_MAKE(t, t, t, 255);

	x = (x256 - size) >> 8;
	y = (y256 - size) >> 8;
	Set2DPointC(tlv+0, x, y, u0, v0, c);

	x = (x256 + size) >> 8;
	y = (y256 + size) >> 8;
	Set2DPointC(tlv+2, x, y, u1, v1, c);

	x = (x256 + size) >> 8;
	y = (y256 - size) >> 8;
	Set2DPointC(tlv+1, x, y, u1, v0, c);

	x = (x256 - size) >> 8;
	y = (y256 + size) >> 8;
	Set2DPointC(tlv+3, x, y, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// ボム演出用のわっかを描画する //
FVOID CStg3Boss::DrawExRing(ExtraOption *pOpt)
{
	D3DTLVERTEX		tlv[20];			// 描画データ
//	D3DVALUE		u0, v0, u1, v1;		// テクスチャ座標
	int				x, y;				// 描画座標値
	int				i;
	int				size;
	DWORD			c;
	BYTE			a;


	for(i=0; i<STG3_EXRING_MAX; i++, pOpt++){
		if(pOpt->count) continue;

		a  = max(0, 255-abs(pOpt->a - 128)*2) / 2;
		x  = (pOpt->x) >> 8;
		y  = (pOpt->y) >> 8;
//		dx = (pOpt->d) * min(128, (pOpt->a * 2)-255) / 255;
		c  = RGBA_MAKE(a, a, a, 255);
		size = (255 - pOpt->a) * 2;

		Set2DPointC(tlv+0, x-size, y-size, 0.0, 0.0, c);
		Set2DPointC(tlv+1, x+size, y-size, 1.0, 0.0, c);
		Set2DPointC(tlv+2, x+size, y+size, 1.0, 1.0, c);
		Set2DPointC(tlv+3, x-size, y+size, 0.0, 1.0, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
