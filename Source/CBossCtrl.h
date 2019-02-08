/*
 *   CBossCtrl.h   : ボス管理
 *
 */

#ifndef CBOSSCTRL_INCLUDED
#define CBOSSCTRL_INCLUDED "ボス管理     : Version 0.01 : Update 2001/09/22"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/22 : 制作開始
 */



#include "Enemy.h"
#include "CBossGauge.h"
#include "CTriEffect.h"
#include "CBGDraw.h"



/***** [クラス定義] *****/

// ボス管理 //
class CBossCtrl {
public:
	FVOID Initialize(void);			// 初期化を行う
	FBOOL OnDamage(int Damage);		// ボスにダメージを与える(ボスが死亡したら真)
	FBOOL OnSendExp(int Exp);		// 攻撃強化・青玉が送れれば真
	FVOID OnBossAttack(void);		// ボスアタック(つまりボス発生)
	FVOID OnClear(void);			// 強制消去要求

	FVOID MoveBossGauge(void);		// データを更新する
	FVOID DrawBossGauge(void);		// ゲージを描画する

	EnemyData *GetEnemyData(void);	// 敵データの取得(死亡中ならNULL)

	// コンストラクタ //
	CBossCtrl(	RECT		*pTarget,		// 描画対象
				CTriEffect	*pTriEffect,	// ガードブレイク用エフェクト
				CBGDraw		*pBGDraw);		// 背景描画クラス

	// デストラクタ //
	~CBossCtrl();

private:
	CTriEffect		*m_pTriEfc;			// ガードブレイク用エフェクト
	CBGDraw			*m_pBGDraw;			// 背景描画

	CBossGauge		m_MadnessGauge;		// マッドネスゲージ(委譲)
	EnemyData		m_EnemyData;		// 標準・敵データ

	int				m_CurrentHP;		// 現在のＨＰ
	BOOL			m_bIsMadnessMode;	// ボス中でも青玉が送れれば真
};



#endif
