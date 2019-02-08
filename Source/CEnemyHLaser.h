/*
 *   CEnemyHLaser.h   : ホーミングレーザー管理
 *
 */

#ifndef CENEMYHLASER_INCLUDED
#define CENEMYHLASER_INCLUDED "ホーミングレーザー管理 : Version 0.01 : Update 2001/09/14"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/14 : 製作開始(エーリッヒ専用なのかな？)
 */



#include "Gian2001.h"
#include "CShaveEffect.h"
#include "EAtkCtrl.h"
#include "HLaser.h"



/***** [ 定数 ] *****/

// 最大値 //
#define EHLASER_KIND	5		// ホーミングレーザーの種類
#define EHLASER_MAX		50		// 同時に発射可能なホーミングレーザーの本数

// 状態定数 //
#define EHLASER_SEARCH	0x00	// ホーミングＬ：目標探索中
#define EHLASER_ACC		0x01	// ホーミングＬ：加速状態
#define EHLASER_FINISH	0x02	// ホーミングＬ：画面外へと
#define EHLASER_DELETE	0x03	// ホーミングＬ：ボムなどによる消去中
#define EHLASER_PLAYER	0x04	// ホーミングＬ：自機を探索中



/***** [クラス定義] *****/

// ホーミングレーザー管理クラス //
class CEnemyHLaser : public CFixedLList<HLaserData, EHLASER_KIND, EHLASER_MAX> {
public:
	// レーザーをセットする(引数がポインタでないことに注意) //
	FVOID Set(AttackCommand Command);

	FVOID Initialize(void);		// 初期化する
	FVOID Clear(void);			// レーザー全てに消去エフェクトをセットする
	FVOID Move(void);			// レーザーを移動させる
	FVOID Draw(void);			// レーザーを描画する

	// 当たり判定を行う               //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// コンストラクタ //
	CEnemyHLaser(RECT			*rcTargetX256	// 対象となる矩形
			   , int			*pX				// 自機のＸ座標へのポインタ
			   , int			*pY				// 自機のＹ座標へのポインタ
			   , CShaveEffect	*pShaveEfc);	// カスりエフェクト発動用クラス

	// デストラクタ //
	~CEnemyHLaser();


private:
	// ホーミングレーザー描画の本体 //
	static FVOID DrawNormalHLaser(HLaserData *pLaser);	// 通常状態
	static FVOID DrawDeleteHLaser(HLaserData *pLaser);	// 消去状態

	// 難易度による書き換え //
	static IVOID EasyCommand(AttackCommand *pCmd);	// 難易度 Easy 用にセット
	static IVOID HardCommand(AttackCommand *pCmd);	// 難易度 Hard 用にセット
	static IVOID LunaCommand(AttackCommand *pCmd);	// 難易度 Luna 用にセット

	// 当たり判定 //
	FVOID HitCheckHLaser(	HLaserData	*pLaser		// 対象となるレーザー
						,	DWORD		*pShave		// カスリ回数
						,	DWORD		*pDamage);	// ダメージ

private:
	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標

	// エフェクト関連 //
	CShaveEffect	*m_pShaveEfc;
};



#endif
