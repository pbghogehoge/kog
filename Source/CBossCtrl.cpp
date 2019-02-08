/*
 *   CBossCtrl.cpp   : ボス管理
 *
 */

#include "CBossCtrl.h"

#define BOSS_MAX_HP		(96 * 256)



// コンストラクタ //
CBossCtrl::CBossCtrl(
				RECT		*pTarget,		// 描画対象
				CTriEffect	*pTriEffect,	// ガードブレイク用エフェクト
				CBGDraw		*pBGDraw)		// 背景描画クラス

	: m_MadnessGauge(BOSS_MAX_HP, pTarget)
{
	m_pTriEfc = pTriEffect;		// ガードブレイク用エフェクト
	m_pBGDraw = pBGDraw;		// 背景描画

	Initialize();
}


// デストラクタ //
CBossCtrl::~CBossCtrl()
{
}


// 初期化を行う //
FVOID CBossCtrl::Initialize(void)
{
	m_CurrentHP      = 0;			// 現在のＨＰを０とする
	m_bIsMadnessMode = FALSE;		// 非マッドネスモード
	m_MadnessGauge.Initialize();	// ゲージの初期化
}


// ボスにダメージを与える(ボスが死亡したら真) //
FBOOL CBossCtrl::OnDamage(int Damage)
{
#ifdef PBG_DEBUG
	if(Damage < 0){
		PbgError("CBossCtrl::OnDamage() : ダメージが負の数になっとる");
	}
#endif

	// お前はすでに死んでいる //
	if(0 == m_CurrentHP) return FALSE;

//	// 当たり判定が無い(CEnemyCtrl 内で抑制されているはず) //
//	if(!(m_EnemyData.Flag & EFLG_HIT_ON)) return FALSE;

	Damage = (max(0, (1024 - m_EnemyData.hp)) * Damage) >> 10;
	if(0 == Damage) return FALSE;

	m_EnemyData.DamageFlag = 2;
	m_CurrentHP           -= Damage;

	if(m_CurrentHP <= 0){
		m_CurrentHP      = 0;
		m_bIsMadnessMode = FALSE;

		// 表示されている場合はガードブレイクを、それ以外は「そっと」殺す //
		if(m_EnemyData.Flag & EFLG_DRAW_ON){
			m_pTriEfc->SetBossGB(m_EnemyData.x, m_EnemyData.y);
		}

		m_pBGDraw->SetBossAlive(FALSE);

		return TRUE;
	}

	return FALSE;
}


// 攻撃強化・青玉が送れれば真 //
FBOOL CBossCtrl::OnSendExp(int Exp)
{
	// 死んでいる場合、この関数は呼び出されないハズだが... //
	// →死んでいれば当然、青玉を発生させられる            //
	if(0 == m_CurrentHP) return TRUE;

#pragma message("ここに攻撃レベルアップの処理を記述せよ！！")
	return m_bIsMadnessMode;
}


// ボスアタック(つまりボス発生) //
FVOID CBossCtrl::OnBossAttack(void)
{
	// 現時点で生きているならば //
	if(m_CurrentHP){
		// マッドネスモードに突入！！ //
		m_bIsMadnessMode = TRUE;
	}

	// 完全回復！！ //
	m_CurrentHP = BOSS_MAX_HP;

	// 背景を暗くする //
	m_pBGDraw->SetBossAlive(TRUE);
}


// 強制消去要求 //
FVOID CBossCtrl::OnClear(void)
{
	if(!m_CurrentHP) return;

	m_CurrentHP       = 1;				// 瀕死
//	m_EnemyData.Flag &= (~EFLG_HIT_ON);	// フラグ外し

	OnDamage(1024 * BOSS_MAX_HP);
}


// データを更新する //
FVOID CBossCtrl::MoveBossGauge(void)
{
	m_MadnessGauge.Move(m_CurrentHP);
}


// ゲージを描画する //
FVOID CBossCtrl::DrawBossGauge(void)
{
	m_MadnessGauge.Draw(m_bIsMadnessMode);
}


// 敵データの取得(死亡中ならNULL) //
EnemyData *CBossCtrl::GetEnemyData(void)
{
	if(m_CurrentHP) return &m_EnemyData;
	else            return NULL;
}
