/*
 *   CEnemyLLaser.h   : 敵用太レーザー
 *
 */

#ifndef CENEMYLLASER_INCLUDED
#define CENEMYLLASER_INCLUDED "敵用太レーザー : Version 0.01 : Update 2001/07/18"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/18 : 制作開始
 */



#include "Gian2001.h"
#include "CShaveEffect.h"
#include "EAtkCtrl.h"
#include "Laser.h"



/***** [ 定数 ] *****/

#define ELLASER_KIND		2		// 太レーザーの種類
#define ELLASER_MAX			100		// 太レーザー同時発生可能数

// 状態定数 //
#define ELLASER_NORMAL		0x00	// 通常の状態
#define ELLASER_DELETE		0x01	// 消去中



/***** [クラス定義] *****/

// 太レーザー管理クラス //
class CEnemyLLaser : public CFixedLList<LLaserData, ELLASER_KIND, ELLASER_MAX> {
public:
	FVOID OnOpen(EnemyAtkCtrl *pAtkCtrl, int w);	// 開き状態に移行する
	FVOID OnClose(EnemyAtkCtrl *pAtkCtrl);			// 閉じ状態に移行する
	FVOID OnCharge(EnemyAtkCtrl *pAtkCtrl);			// 溜め状態に移行する

	FVOID Initialize(void);		// 初期化する
	FVOID Clear(void);			// 太レーザー全てに消去エフェクトをセットする
	FVOID Move(void);			// 太レーザーを移動させる
	FVOID Draw(void);			// 太レーザーを描画する

	// 当たり判定を行う //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// ＳＬ用当たり判定 //
	// IN  : x y  当たり判定の対象座標
	//     : dsl  ＳＬの進行方向
	//     : c    反射するレーザーの色
	// RET : == d ならば反射していない
	//     : != d ならば反射している
	FBYTE CEnemyLLaser::HitCheckEx(int x, int y, BYTE dsl, BYTE c);

	// コンストラクタ //
	CEnemyLLaser(int			*pX				// 自機のＸ座標へのポインタ
			   , int			*pY				// 自機のＹ座標へのポインタ
			   , CShaveEffect	*pShaveEfc);	// カスりエフェクト発動用クラス

	// デストラクタ //
	~CEnemyLLaser();


private:
	// 接続が出来ていればそのポインタを返し、出来ていなければ //
	// 接続し、そのポインタを返す。失敗時は NULL を返す       //
	LLaserData *Connect(EnemyAtkCtrl *pAtkCtrl);

	// 以下の関数の呼び出しを行う前に、レンダリングステートと //
	// テクスチャの設定を済ませておく必要があります           //
	static FVOID DrawNormal(LLaserData *pLaser);	// 通常状態の描画
	static FVOID DrawCharge(LLaserData *pLaser);	// 溜め状態の描画


	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// エフェクト関連 //
	CShaveEffect	*m_pShaveEfc;
};



#endif
