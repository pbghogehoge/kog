/*
 *   CNormalShot.cpp   : 自機の通常ショット
 *
 */

#include "CNormalShot.h"
#include "Gian2001.h"

#include "RndCtrl.h"



/***** [プライベートな定数] *****/
#define NS_SPEED		(12 * 256)	// 通常ショットの移動速度
#define NS_MAXSIZE		(16 * 256)	// 通常ショットの最大サイズ
#define NS_HITSIZE		( 8 * 256)	// 通常ショットの当たり判定サイズ
#define NS_MAX_TIME		24			// 通常ショットの自動連射時間
#define NS_AUTO_TIME	6			// 通常ショットの連射間隔
#define NS_DAMAGE		256			// 通常ショットのダメージ量



// コンストラクタ //
CNormalShot::CNormalShot(RECT *rcTargetX256, int *pX, int *pY)
{
	// Ｙ方向の消去座標を確定する //
	m_YMin = rcTargetX256->top - NS_MAXSIZE;

	// ポインタを初期化 //
	m_pX = pX;				// 発射元のＸ座標へのポインタ
	m_pY = pY;				// 発射元のＹ座標へのポインタ

	m_Count = 0;	// カウンタ
	m_Type  = 0;	// 形状
}


// デストラクタ //
CNormalShot::~CNormalShot()
{
	// 特に何もしませんよ //
}


// 初期化する //
FVOID CNormalShot::Initialize(CCharacterBase *pCharInfo)
{
	// 各変数の初期化を行う //
	m_dx    = pCharInfo->GetNShotDx();	// Ｘ座標をどれだけズラすか
	m_dy    = pCharInfo->GetNShotDy();	// Ｙ座標をどれだけズラすか
	m_Count = 0;						// カウンタ

	// 弾の種類を確定する //
	switch(pCharInfo->GetCharID()){
		case CHARACTER_VIVIT:	m_Type = 0;		break;	// びびっと
		case CHARACTER_STG1:	m_Type = 1;		break;	// ミリア
		case CHARACTER_STG2:	m_Type = 2;		break;	// めい＆まい
		case CHARACTER_STG3:	m_Type = 3;		break;	// ゲイツ
		case CHARACTER_STG4:	m_Type = 4;		break;	// マリー
		case CHARACTER_STG5:	m_Type = 5;		break;	// エーリッヒ
		case CHARACTER_MORGAN:	m_Type = 6;		break;	// モーガン
		case CHARACTER_MUSE:	m_Type = 7;		break;	// ミューズ
		case CHARACTER_YUKA:	m_Type = 8;		break;	// 幽香
	}

	// リスト内のデータを解放する //
	DeleteAllData();
}


// セットする //
FVOID CNormalShot::Set(void)
{
	// カウンタが非ゼロならば、セットできない //
	if(m_Count) return;

	// カウンタを初期化する //
	m_Count = NS_MAX_TIME;
}


// １フレームだけ動かす //
FVOID CNormalShot::Move(void)
{
	NormalShot		*pShot;
	Iterator		it;
	int				YMin;

	// 自動発射を行う場合 //
	if(m_Count){
		// セットする //
		if(0 == (m_Count % NS_AUTO_TIME)){
			pShot = InsertBack(NSHOT_NORMAL);	// 前から挿入
			pShot->x     = (*m_pX) + m_dx;		// 初期Ｘ座標
			pShot->y     = (*m_pY) + m_dy;		// 初期Ｙ座標
			pShot->count = 0;					// カウンタ
		}

		// カウンタをデクリメントする //
		m_Count--;
	}


	// 消去座標をローカル変数に移動 //
	YMin = m_YMin;

	// 通常動作中の弾 //
	ThisForEachFront(NSHOT_NORMAL, it){
		pShot     = it.GetPtr();	// イテレータ to ポインタ
		pShot->y -= NS_SPEED;		// 上に進むのみ

		// 画面外にでていたら、消去する //
		if(pShot->y < YMin) ThisDelContinue(it);

		// カウンタをインクリメント
		pShot->count++;
	}

	// 消去中の弾 //
	ThisForEachFront(NSHOT_DELETE, it){
		pShot     = it.GetPtr();	// イテレータ to ポインタ
		pShot->x += CosL(pShot->d, (NS_SPEED/4));	// Ｘ方向の移動
		pShot->y += SinL(pShot->d, (NS_SPEED/4));	// Ｙ方向の移動

		// 消去のタイミング //
		if(pShot->count >= 31) ThisDelContinue(it);

		// カウンタをインクリメント
		pShot->count++;
	}
}


// 描画する //
FVOID CNormalShot::Draw(void)
{
	NormalShot		*pShot;
	Iterator		it;
	int				x, y, n;
	int				tvx, tvy;
	int				size;
	DWORD			c;
	D3DVALUE		u0, v0, u1, v1;
	D3DTLVERTEX		tlv[NORMALSHOT_MAX * 6], *ptlv;


	// テクスチャ座標を確定する //
	tvx =  ((m_Type & 7)  << 5);
	tvy = -((m_Type >> 3) << 5);
	u0 = D3DVAL( 8  + tvx) / D3DVAL(256);	// 左
	u1 = D3DVAL( 32 + tvx) / D3DVAL(256);	// 右
	v0 = D3DVAL(224 + tvy) / D3DVAL(256);	// 上
	v1 = D3DVAL(248 + tvy) / D3DVAL(256);	// 下

	// レンダリングステート＆テクスチャのセット //
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);

	// 諸変数の初期化 //
	ptlv = tlv;								// 挿入先
	c    = RGBA_MAKE(255, 255, 255, 235);	// 色の指定


	// 通常動作中の弾 //
	ThisForEachFront(NSHOT_NORMAL, it){
		pShot = it.GetPtr();		// イテレータ to ポインタ
		x     = (pShot->x) >> 8;	// ｘ座標中心
		y     = (pShot->y) >> 8;	// ｙ座標中心

		Set2DPointC(ptlv+0, x-12, y-12, u0, v0, c);
		Set2DPointC(ptlv+5, x-12, y-12, u0, v0, c);

		Set2DPointC(ptlv+1, x+12, y-12, u1, v0, c);

		Set2DPointC(ptlv+2, x+12, y+12, u1, v1, c);
		Set2DPointC(ptlv+3, x+12, y+12, u1, v1, c);

		Set2DPointC(ptlv+4, x-12, y+12, u0, v1, c);

		ptlv += 6;
	}


	// 消去中の弾 //
	ThisForEachFront(NSHOT_DELETE, it){
		pShot = it.GetPtr();				// イテレータ to ポインタ
		x     = (pShot->x) >> 8;			// ｘ座標中心
		y     = (pShot->y) >> 8;			// ｙ座標中心
		n     = 255 - (pShot->count) * 8;	// α値
		c     = RGBA_MAKE(255, 255, 255, n);	// 色の指定
		size  = 12 - pShot->count/3;

		Set2DPointC(ptlv+0, x-size, y-size, u0, v0, c);
		Set2DPointC(ptlv+5, x-size, y-size, u0, v0, c);

		Set2DPointC(ptlv+1, x+size, y-size, u1, v0, c);

		Set2DPointC(ptlv+2, x+size, y+size, u1, v1, c);
		Set2DPointC(ptlv+3, x+size, y+size, u1, v1, c);

		Set2DPointC(ptlv+4, x-size, y+size, u0, v1, c);

		ptlv += 6;
	}

	n = ptlv - tlv;
	if(n){
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, tlv, n);
	}
}


// 当たり判定 //
FDWORD CNormalShot::HitCheck(EnemyData *pEnemy)
{
	Iterator		it;
	NormalShot		*pShot;
	DWORD			Damage;
	DWORD			Size;
	int				x, y;

	// 当たり判定のサイズは、後で変更すること！！ //
	x      = pEnemy->x;					// 敵のＸ座標
	y      = pEnemy->y;					// 敵のＹ座標
	Size   = NS_HITSIZE + pEnemy->size;	// 当たり判定
	Damage = 0;							// ダメージ総量の初期化

	// 通常ショットはヒットした時点で消去状態に移行する //
	ThisForEachFront(NSHOT_NORMAL, it){
		pShot = it.GetPtr();

		// ヒットしていたら、その弾に消去エフェクトをかける //
		if(HitCheckFast(y, (pShot->y), Size)
		&& HitCheckFast(x, (pShot->x), Size)){
			Damage      += NS_DAMAGE;					// ダメージ増加
			pShot        = MoveFront(it, NSHOT_DELETE);	// 先頭に挿入
			pShot->count = 0;							// カウンタをリセット
			pShot->d     = 128+32 + RndEx() % 64;		// 角度をランダムでセット
			continue;
		}
	}


	// ダメージを返す //
	return Damage;
}
