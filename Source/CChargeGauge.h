/*
 *   CChargeGauge.h   : 溜めゲージ管理
 *
 */

#ifndef CCHARGEGAUGE_INCLUDED
#define CCHARGEGAUGE_INCLUDED "溜めゲージ管理 : Version 0.05 : Update 2001/11/17"

/*  [更新履歴]
 *    Version 0.05 : 2001/11/17 : CGaugeInitialize() の仕様変更
 *                              : 初期攻撃レベルを設定可能にした
 *
 *    Version 0.04 : 2001/07/30 : キャラ描画、背景管理の追加
 *    Version 0.03 : 2001/07/27 : 各種エフェクトの追加
 *    Version 0.02 : 2001/04/04 : 復帰
 *    Version 0.01 : 2001/02/27 : クラス設計
 */



#include "Gian2001.h"
#include "CCharacterBase.h"	// キャラクタ基底
#include "CNormalShot.h"	// ノーマルショット
#include "CExtraShot.h"		// エキストラショット
#include "ChargeDef.h"		// 溜め定義
#include "CStdAttack.h"		// 攻撃送り
#include "ChargeEfc.h"		// 溜めエフェクト
#include "CDeadEfc.h"		// 死亡エフェクト
#include "CBGDraw.h"		// 背景描画
#include "CAtkGrpDraw.h"	// キャラ描画
#include "CStateWindow.h"



/***** [クラス定義] *****/

// 溜めゲージ管理・基底クラス //
class CChargeGauge {
public:
	// 死亡エフェクト完了なら真を返す //
	FBOOL IsEffectFinished(void);

	// 現在溜められる最大値を求める //
	IDWORD GetCGaugeMax(void){ return m_CGaugeMax; };

	// 現在、どこまでチャージしたかを調べる //
	IDWORD CGaugeGetCurrent(void){ return m_CGaugeCurrent; };

	// アクティブなエキストラショットオブジェクト数を取得 //
	IDWORD GetNumExShot(void){ return m_ExtraShot.GetActiveData(); };

	// 現在の攻撃レベルを取得する //
	IINT GetCurrentAtkLv(void){ return m_CGaugeAtkLv; };

#ifdef PBG_DEBUG
	FVOID DebugAddAtkLv(int delta){
		if(delta > 0 && m_CGaugeAtkLv < 25){
			m_CGaugeAtkLv++;
		}
		else if(delta < 0 && m_CGaugeAtkLv > 1){
			m_CGaugeAtkLv--;
		}
	}
#endif

	CChargeGauge(RECT *rcTargetX256, int *pX, int *pY);	// コンストラクタ
	~CChargeGauge();									// デストラクタ


protected:
	FBOOL  CGaugeInitialize(int InitAtkLv, DWORD CGaugeCurrent);	// 溜めゲージを初期化する
	FVOID  CGaugeReset(void);				// ガードブレイク時の動作
	FVOID  CGaugeBuzz(DWORD nBuzz);			// カスりにより溜めの最大値を上昇させる
	FVOID  CGaugeUpdate(WORD KeyCode);		// 溜めゲージの状態を更新する
	FDWORD CGaugeGetAtkLv(void);			// 攻撃レベルを取得する

	// 溜めゲージを描画する //
	FVOID CGaugeDraw(int ox, int oy, Pbg::CGrpSurface *pSurface);

	// シールドをＯＮにする //
	FVOID ShieldOn(int Count);

	CCharacterBase	*m_pCharInfo;			// キャラクタ情報クラスへのポインタ
	CStdAttack		*m_pStdAttack;			// 攻撃送り管理クラスへのポインタ
	CSCLDecoder		*m_pRivalSCLDecoder;	// 相手側のＳＣＬデコーダ
	CAtkGrpDraw		*m_pRivalAtkGrpDraw;	// 相手側のキャラ描画クラス
	CBGDraw			*m_pRivalBGDraw;		// 相手側の背景描画クラス
	CBGDraw			*m_pThisBGDraw;			// コイツの背景描画クラス
	CShaveEffect	*m_pShaveEffect;		// カスリエフェクト
	CEnemyCtrl		*m_pEnemyCtrl;			// 敵管理(特殊・敵弾消去用)

	CNormalShot		m_NormalShot;			// ノーマルショット管理クラス
	CExtraShot		m_ExtraShot;			// エキストラショット管理クラス
	CChargeEfc		m_ChargeEfc;			// 溜め完了エフェクト
	CDeadEfc		m_DeadEfc;				// 死亡エフェクト

	CStateWindow	*m_pThisState;			// 自分の状態表示窓
	CStateWindow	*m_pRivalState;			// 相手の状態表示窓

	int		m_GrpState;			// グラフィックの状態

	int		m_ShieldStart;		// シールド開始カウント
	int		m_ShieldCount;		// シールドの有効期間
	BOOL	m_bNeedETClear;		// 敵弾消去の必要があれば真
	BOOL	m_bBombKeyPushed;	// ボムキーが押されていれば真


private:
	// 以下の関数は CGaugeUpdate から呼び出される //
	FVOID StateNormal(WORD KeyCode);	// 通常の状態
	FVOID StateCharge(WORD KeyCode);	// 溜め状態
	FBOOL BombAttack( WORD KeyCode);	// ボムアタック(発動したらTRUE)

	BOOL	m_bCGaugeEnable;	// 溜め動作に入ることが出来るか
	DWORD	m_CGaugeCurrent;	// 現在どこまで溜めたか
	DWORD	m_CGaugeMax;		// 溜められる最大値
	DWORD	m_CGaugeAuto;		// 溜め攻撃の自動発動までの残り時間
	DWORD	m_CGaugeBombLeft;	// 残りのボム数
	DWORD	m_CGaugeAtkLv;		// 攻撃レベル

	BYTE	m_Count;			// 描画エフェクト用カウンタ
};



#endif
