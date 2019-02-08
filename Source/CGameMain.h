/*
 *   CGameMain.h   : ゲーム本体
 *
 */

#ifndef CGAMEMAIN_INCLUDED
#define CGAMEMAIN_INCLUDED "シーンの定義 : Version 0.06 : Update 2001/05/22"

/*  [更新履歴]
 *    Version 0.06 : 2001/05/22 : ファイル名を CSceneCtrl->CGameMain に変更
 *    Version 0.05 : 2001/05/06 : デバッグ情報の表示がちょっとだけ素敵に☆
 *    Version 0.04 : 2001/04/27 : トライアングルエフェクトの追加
 *    Version 0.03 : 2001/04/22 : この時点で破片エフェクト等が追加される
 *    Version 0.02 : 2001/04/07 : カスりエフェクトの追加
 *    Version 0.01 : 2001/03/28 : 制作開始
 */



#include "Gian2001.h"
#include "CPUCtrl.h"
#include "CBGDraw.h"
#include "CBGDoor.h"
#include "CShaveEffect.h"
#include "CFragmentEfc.h"
#include "CTriEffect.h"
#include "CDrawStg.h"
#include "CDrawBonus.h"
#include "CDrawWonLost.h"
#include "SCL.h"
#include "CScoreCtrl.h"		// 得点管理
#include "CAtkGrpDraw.h"	// 攻撃送り時のグラフィック描画
#include "CStateWindow.h"	// 状態表示窓


/***** [ 定数 ] *****/
typedef enum tagGMAIN_RET {
	GMAIN_OK    = 0,	// 通常の終了
	GMAIN_DEAD  = 1,	// プレイヤーが死亡した
	GMAIN_WON   = 2,	// プレイヤーが勝利した
	GMAIN_ERROR = 0xff	// エラーが発生した
} GMAIN_RET;



/***** [クラス定義] *****/

// ゲーム本体の処理 //
class CGameMain : public CCPUCtrl {
public:
	// 初期化を行う //
	FBOOL Initialize(CHARACTER_ID	CharID		// 自分のキャラクタＩＤ
				   , CHARACTER_ID	RivalID		// 相手のキャラクタＩＤ
				   , CGameMain		*pRival		// 相手側のクラス
				   , BOOL			Is2PMode	// ２Ｐモードなら真
				   , int			nStage		// 現在のステージorラウンド数
				   , int			GameLv		// ＳＣＬ初期化レベル
				   , int			AtkLv		// 初期攻撃レベル
				   , __int64		Score		// 初期化スコア
				   , DWORD			CGaugeMax	// 溜めゲージ
				   , BYTE			DamageWeight	// ダメージの減少
				   , BOOL			Is2PColor
				   , DWORD			RndSeed);		// 乱数の種

	GMAIN_RET Move(WORD KeyCode);				// １フレーム進める
	FBOOL Draw(int SyncDx);						// 描画を行う
	FBOOL DrawInfo(Pbg::CGrpSurface *pSurface);	// その他情報の描画を行う

	// ゲーム終了時に各種エフェクトが完了していれば真を返す //
	FBOOL IsGameFinished(void);

	// 全ての敵弾＆レーザーに Clear() 要求を出力する //
	FVOID OnETClear(BOOL bChgScore);

	// 勝敗が決定したときに呼び出される //
	FVOID OnWonLost(BYTE DWL_State);

	// 現在のスコアを取得する //
	__int64 GetCurrentScore(void);

	DWORD GetCGaugeMax(void){
		return m_pPlayerCtrl->GetCGaugeMax();
	};

	// 現在の攻撃レベルを取得する //
	int GetCurrentAtkLv(void);

	// コンストラクタ(矩形は非x２５６) //
	CGameMain(RECT *rcTarget, SCENE_ID SceneID);

	// デストラクタ //
	~CGameMain();

#ifdef PBG_DEBUG
	static IVOID SwitchDebugInfo(void){
		if(( m_Debug_DisplayInfo) && (m_Debug_EffectCount < 128)) return;
		if((!m_Debug_DisplayInfo) && (m_Debug_EffectCount > 128)) return;

		m_Debug_DisplayInfo = !m_Debug_DisplayInfo;
	};

	static IBOOL IsDebugMode(void){
		return m_Debug_DisplayInfo;
	};
#endif


private:
	// ヒットチェック用コールバック関数 //
	static DWORD HitCheckCallback(void *pParam, EnemyData *pEnemy);


private:
	FVOID Cleanup(void);		// 動的確保したオブジェクトの解放を行う

	CGameMain		*m_pRivalInfo;		// 対戦相手の情報
	CSCLDecoder		*m_pSCLDecode;		// ＳＣＬデコーダ

	CBGDraw			*m_pBGDraw;			// 背景描画
	CShaveEffect	*m_pShaveEfc;		// カスりエフェクト
	CFragmentEfc	*m_pFragmentEfc;	// 破片エフェクト
	CTriEffect		*m_pTriangleEfc;	// 三角形エフェクト
	CDrawStg		*m_pDrawStgEfc;		// ステージ表示エフェクト
	CDrawBonus		*m_pDrawBonus;		// ボーナス表示エフェクト
	CDrawWonLost	*m_pDrawWonLost;	// 勝敗表示エフェクト
	CStdAttack		*m_pStdAttack;		// 攻撃送り
	CAtkGrpDraw		*m_pAtkGrpDraw;		// キャラクタ描画クラス
	CBGDoor			*m_pBGDoor;			// 扉の描画
	CStateWindow	*m_pStateWindow;	// 状態表示窓

	CScoreCtrl		*m_pScore;			// スコア管理

	Pbg::CRnd		m_Rnd;				// 乱数管理

	RECT			m_rcTargetX256;		// 対象となる矩形(x256)
	RECT			m_rcViewport;		// 描画対象となる矩形
	SCENE_ID		m_SceneID;			// シーン固有の値
	CHARACTER_ID	m_CharID;			// プレイヤーの種類識別用変数

	DWORD			m_Count;		// カウンタ
	DWORD			m_State;		// 現在の状態(GMAIN_RET)

	DWORD			m_CloseCount;	// 扉クローズ用カウンタ
									// (一定値を超えるとクローズ開始)

	BYTE			m_DamageWeight;	// ダメージの割合(大きければダメージ減少)


#ifdef PBG_DEBUG // デバッグ情報
	static BOOL		m_Debug_DisplayInfo;	// 表示するか否か
	static BYTE		m_Debug_EffectCount;	// 表示フェードエフェクト用カウンタ
#endif
};



#endif
