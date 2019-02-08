/*
 *   CScoreCtrl.h   : スコア管理
 *
 */

#ifndef CSCORECTRL_INCLUDED
#define CSCORECTRL_INCLUDED "スコア管理   : Version 0.01 : Update 2001/07/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/27 : 製作開始
 */



#include "Gian2001.h"
#include "CShaveEffect.h"



/***** [クラス定義] *****/

// スコア管理クラス //
class CScoreCtrl {
public:
	FVOID Initialize(__int64 Score);	// 初期化を行う
	FVOID Update(void);					// 更新する(カウンタまわし)
	FVOID Draw(void);					// 描画する
	FVOID AddScore(__int64 dScore);		// 現在のスコアに加算する

	__int64 GetScore(void);				// 現在のスコアを取得する

	CScoreCtrl(RECT *rcTargetX256, CShaveEffect *pShaveEfc);	// コンストラクタ
	~CScoreCtrl();												// デストラクタ


private:
	__int64		m_CurrentScore;		// 現在のスコア
	__int64		m_DispScore;		// 表示するスコア

	CShaveEffect	*m_pShave;	// カスりエフェクト
	int				m_ox;		// 表示開始座標(X)
	int				m_oy;		// 表示開始座標(Y)
};



#endif
