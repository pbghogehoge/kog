/*
 *   CBossGauge.h   : ボス用各種ゲージ
 *
 */

#ifndef CBOSSGAUGE_INCLUDED
#define CBOSSGAUGE_INCLUDED "ボス用各種ゲージ : Version 0.01 : Update 2001/10/22"

/*  [更新履歴]
 *    Version 0.02 : 2001/10/22 : 大幅に修正を加える
 *    Version 0.01 : 2001/09/23 : 制作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// マッドネス＆経験値ゲージ管理 //
class CBossGauge {
public:
	FVOID Draw(BOOL IsMadnessMode);	// 描画時に呼び出す
	FVOID Move(int CurrentHP);		// 動作時に呼び出す
	FVOID Initialize(void);			// 初期化時に呼び出す

	CBossGauge(int MaxHP, RECT *pTarget);	// コンストラクタ
	~CBossGauge();							// デストラクタ


private:
	int			m_CurrentHP;	// 現在のＨＰ
	int			m_DisplayHP;	// 表示用ＨＰ

	const int	m_MaxHP;		// 最大ＨＰ
	const int	m_DispOx;		// 表示Ｘ座標
	const int	m_DispOy;		// 表示Ｙ座標

	DWORD		m_Count;		// アニメーション動作用カウンタ

	BYTE		m_Alpha;		// ゲージのα値(徐々に表示用)
	BYTE		m_State;		// 表示状態
};



#endif
