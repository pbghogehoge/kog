/*
 *   CEnemyExShot.h   : 敵用特殊攻撃
 *
 */

#ifndef CENEMYEXSHOT_INCLUDED
#define CENEMYEXSHOT_INCLUDED "敵用特殊攻撃 : Version 0.01 : Update 2001/11/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/27 : 制作開始
 */



#include "Gian2001.h"
#include "EAtkCtrl.h"
#include "CShaveEffect.h"

#include "RLaser.h"		// ラウンドレーザーの定義
#include "Cross.h"		// 十字架の定義
#include "Flower.h"		// 花の定義
#include "GFire.h"		// ゲイツなにがしの定義
#include "IonRing.h"	// イオンリングの定義
#include "Enemy.h"



/***** [ 定数 ] *****/
#define EEXSHOT_TASKSIZE	32		// 敵用特殊ショットタスクのサイズ
#define EEXSHOT_MAX			300		// 敵用特殊ショットの同時発生数・最大値
#define EEXSHOT_KIND		20		// 敵用特殊ショットの種類

#define EEXTSK_RLASER_LOCK		0	// ラウンドレーザー予備動作
#define EEXTSK_RLASER			1	// ラウンドレーザー通常状態
#define EEXTSK_RLASER_DELETE	2	// ラウンドレーザー消去中

#define EEXTSK_CROSS			3	// 十字架・落下中
#define EEXTSK_CROSS_DELETE		4	// 十字架・消去中
#define EEXTSK_CROSS_DANGER		5	// 十字架・警告

#define EEXTSK_FLOWER			6	// 花・移動中
#define EEXTSK_FLOWER_DELETE	7	// 花・消去中

#define EEXTSK_GFIRE			8	// ゲイツなにがし
#define EEXTSK_GFIRE_DELETE		9	// ゲイツなにがし・消去中
#define EEXTSK_GFIRE_INIT		10	// ゲイツなにがし・発射準備

#define EEXTSK_IONRING			11	// イオンリング
#define EEXTSK_IONRING_DELETE	12	// イオンリング・消去中



/***** [クラス定義] *****/

// タスク管理構造体 //
typedef struct _tagEExShotTask {
	BYTE	m_Data[EEXSHOT_TASKSIZE];
} EExShotTask;


// 特殊攻撃オブジェクトタスク管理 //
class CEnemyExShot : public CFixedLList<EExShotTask, EEXSHOT_KIND, EEXSHOT_MAX> {
public:
	// --- ラウンドレーザーをセットする ---
	// IN : .ox .oy ( ラウンドレーザーの中心位置 )
	//    : .Length ( 目標とする半径 )
	//    : .Speed  ( 展開速度 )
	FVOID SetRLaser(AttackCommand Command);

	// --- 十字架をセットする ---
	// IN : .Num    ( 十字架の数 )
	//    : .Speed  ( 初速度 )
	//    : .Accel  ( 加速度 )
	FVOID SetCross(AttackCommand Command);

	// --- 花をセットする --
	// IN : .ox .oy (初期座標)
	//    : .Angle  (進行方向)
	//    : .Length (花の大きさ)
	//    : .Speed  (移動速度:遅いほうが強力！)
	//    : .Color  (花の色)
	FVOID SetFlower(AttackCommand Command);

	// --- ゲイツ・なにがしをセットする --
	// IN : .ox .oy (初期座標)
	//    : .Angle  (発射方向)
	//    : .Accel  (速度減衰)
	//    : .Speed  (移動速度:遅いほうが強力！)
	FVOID SetGFire(AttackCommand Command);

	// --- イオンリングをセットする ---
	// IN : .ox .oy (初期座標)
	//    : .Angle  (発射方向)
	//    : .Speed  (移動速度)
	//    : .Length (大きさ)
	FVOID SetIonRing(AttackCommand Command);


	FVOID Initialize(void);		// 初期化する

	FVOID Clear(void);	// 特殊ショットに消去エフェクトをセットする
	FVOID Move(void);	// 特殊ショットを移動させる
	FVOID Draw(void);	// 特殊ショットを描画する

	// 当たり判定を行う               //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// 破壊可能な敵弾に対して DamageCallback を呼び出す //
	FVOID EnumBreakableExShot(DamageCallback Callback, void *pParam);


	// コンストラクタ //
	CEnemyExShot(RECT			*rcTargetX256	// 対象矩形
			   , int			*pX				// 当たり判定Ｘ座標へのポインタ
			   , int			*pY				// 当たり判定Ｙ座標へのポインタ
			   , CShaveEffect	*pShaveEfc);	// カスりエフェクト発動用クラス

	~CEnemyExShot();	// デストラクタ


private:
	// 消去要求時に呼び出される関数 //
	FVOID OnClearRLaser(void);	// ラウンドレーザー
	FVOID OnClearCross(void);	// 十字架
	FVOID OnClearFlower(void);	// 花
	FVOID OnClearGFire(void);	// ゲイツなにがし
	FVOID OnClearIonRing(void);	// イオンリング

	// ラウンドレーザー動作系 //
	FBOOL MoveLockRLaser(RLaserData *pRL);	// ロック中ラウンドレーザーを動作させる
	FBOOL MoveRLaser(RLaserData *pRL);		// ラウンドレーザーを動作させる
	FBOOL MoveDelRLaser(RLaserData *pRL);	// 消去中ラウンドレーザーを動作させる

	// 十字架・動作系 //
	FBOOL MoveCross(CrossData *pCr);		// 落下中の十字架を動作させる
	FBOOL MoveDelCross(CrossData *pCr);		// 消去中の十字架を動作させる
	FBOOL MoveCrossDanger(CrossData *pCr);	// 十字架落下位置警告を動作させる

	// 花・動作系 //
	FBOOL MoveFlower(FlowerData *pFl);		// 落下中の花を動作させる
	FBOOL MoveDelFlower(FlowerData *pFl);	// 消去の花を動作させる

	// 火炎放射・動作系 //
	FBOOL MoveGFire(GFireData *pF);			// 動作中の炎
	FBOOL MoveDelGFire(GFireData *pF);		// 消去中の炎
	FBOOL MoveInitGFire(GFireData *pF);		// 準備中の炎

	// イオンリング・動作系 //
	FBOOL MoveIonRing(IonRingData *pI);		// 動作中のイオンリング
	FBOOL MoveDelIonRing(IonRingData *pI);	// 消去中のイオンリング

	// ラウンドレーザー描画系 //
	static FVOID DrawLockRLaser(RLaserData *pRL);	// ロック中ラウンドレーザーを描画
//	static FVOID DrawRLaser(RLaserData *pRL);		// ラウンドレーザーを描画
	static FVOID DrawDelRLaser(RLaserData *pRL);	// 消去中ＲＬを描画

	// 十字架・描画系 //
	static FVOID DrawCross(CrossData *pCr);		// 落下中の十字架を描画させる
	static FVOID DrawDelCross(CrossData *pCr);	// 消去中の十字架を描画させる
	static FVOID DrawCrossDanger(CrossData *pCr);	// 十字架(警告)を描画させる

	// 花・描画系 //
	static FVOID DrawFlower(FlowerData *pFl, BYTE a);	// 花を描画させる

	// 火炎放射・描画系 //
	static FVOID DrawGFire(GFireData *pF);		// 通常状態

	// イオンリング・描画系 //
	static FVOID DrawIonRing(IonRingData *pI);		// 通常状態
	static FVOID DrawDelIonRing(IonRingData *pI);	// さよなら中


private:
	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標
	int		m_Width;	// 横幅

	// エフェクト関連 //
	CShaveEffect	*m_pShaveEfc;
};



#endif
