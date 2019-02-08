/*
 *   CPlayerCtrl.h   : キャラクタの移動処理
 *
 */

#ifndef CPLAYERCTRL_INCLUDED
#define CPLAYERCTRL_INCLUDED "キャラクタの移動処理 : Version 0.01 : Update 2001/02/28"

/*  [注意事項]
 *    CChargeGauge の初期化等はこのクラスが行うものとする
 */

/*  [更新履歴]
 *    Version 0.01 : 2001/02/28 : 制作開始
 */



#include "CChargeGauge.h"
#include "CTriEffect.h"		// ガードブレイクエフェクト



/***** [ 定数 ] *****/
#define PSTATE_NORMAL		0x00		// 通常の状態
#define PSTATE_GBREAK		0x01		// ガードブレイク中
#define PSTATE_WON			0xfe		// 勝ち状態
#define PSTATE_DEAD			0xff		// 死亡状態



/***** [クラス定義] *****/

// プレイヤー管理クラス //
class CPlayerCtrl : public CChargeGauge {
public:
	// 初期化する //
	FBOOL PlayerInitialize(
				CHARACTER_ID	CharID				// キャラクタＩＤ
			  ,	int				AtkLv				// 初期攻撃レベル
			  , CStdAttack		*pStdAttack			// 標準攻撃送り
			  , CSCLDecoder		*pRivalSCLDecoder	// 相手のＳＣＬデコーダ
			  , CBGDraw			*pRivalBGDraw		// 相手の背景描画
			  , CAtkGrpDraw		*pRivalAtkGrpDraw	// 相手側のキャラ描画
			  , CBGDraw			*pThisAtkGrpDraw	// コイツのキャラ描画
			  , CEnemyCtrl		*pEnemy				// 敵管理(ExShot用)
			  , CEnemyCtrl		*pRivalEnemy		// 相手側の敵管理(ボス強化用)
			  , DWORD			CGaugeMax			// 溜めゲージ
			  , CStateWindow	*pThisStWnd			// 自分の状態窓
			  , CStateWindow	*pRivalStWnd);		// 相手の状態窓

	FVOID  PlayerMove(WORD KeyCode);	// 移動する
	FVOID  OnDamage(DWORD Damage);		// ヒットしている場合
	FVOID  OnShave(DWORD Shave);		// カスっている場合
	FVOID  OnPlayerWon(void);			// 勝った時に呼び出す
	FDWORD HitCheck(EnemyData *pEnemy);	// 敵<->自機ショットの当たり判定

	FVOID DrawPlayer(void);							// プレイヤーの描画を行う
	FVOID DrawGuardGauge(void);						// ガードゲージの描画を行う
	FVOID DrawExtra(Pbg::CGrpSurface *pSurface);	// その他・小物の描画を行う

	int *GetX_Ptr(void){ return &m_X; };	// Ｘ座標へのポインタを取得
	int *GetY_Ptr(void){ return &m_Y; };	// Ｙ座標へのポインタを取得

	IINT GetXMin(void){ return m_XMin; };	// 移動可能Ｘ座標の左端
	IINT GetXMax(void){ return m_XMax; };	// 移動可能Ｘ座標の右端
	IINT GetYMin(void){ return m_YMin; };	// 移動可能Ｙ座標の上端
	IINT GetYMax(void){ return m_YMax; };	// 移動可能Ｙ座標の下端

	IINT GetStartY(void){ return m_StartY; };	// 初期Ｙ座標の取得

	IINT GetMaxCombo(void){ return m_MaxBuzzCombo; };	// 最大コンボ数

	IBOOL IsHitOn(void){		// 当たり判定があれば真
		return m_ShieldCount ? FALSE : TRUE;
	};

	IBOOL IsPlayerDead(void){	// 死亡しているなら真
		return (PSTATE_DEAD == m_State) ? TRUE : FALSE;
	};

	IBOOL IsETClear(void){		// 敵弾消去要求があれば真
		return m_bNeedETClear;
	};

	IBOOL IsGBreak(void){		// ガードブレイク中なら真
		if(PSTATE_GBREAK == m_State) return TRUE;
		else                         return FALSE;
	};

	// キーコードによって移動したときの座標を取得する //
	// 引数 rX, rY  : 元の座標への参照                //
	//      KeyCode : キーボード入力                  //
	FVOID SetXYFromKeyCode(int &rX, int &rY, WORD KeyCode);


	// コンストラクタ //
	CPlayerCtrl(RECT			*rcTargetX256	// 移動範囲矩形
			  , DWORD			TextureID		// テクスチャＩＤ
			  , CTriEffect		*pTriEffect		// 三角形エフェクト
			  , CShaveEffect	*pShaveEfc);	// カスリエフェクト

	// デストラクタ //
	~CPlayerCtrl();


private:
	FVOID ChainAttack(void);	// カスリコンボによる攻撃
	FINT  GetShieldSize(void);	// 保護膜のサイズを取得(x256)
	FVOID DrawShield(void);		// 無敵時間の保護膜？描画


	// 座標の範囲と初期値 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標
	int		m_StartX;	// Ｘ座標初期値
	int		m_StartY;	// Ｙ座標初期値
	int		m_ChargeX;	// 溜めゲージの始点Ｘ
	int		m_ChargeY;	// 溜めゲージの始点Ｙ

	// 現在の座標情報 //
	int		m_X;			// 現在のＸ座標 x 256
	int		m_Y;			// 現在のＹ座標 x 256

	// 表示関連 //
	DWORD	m_TextureID;	// テクスチャＩＤ
	DWORD	m_FinCount;		// 勝敗決定時に使用するカウンタ

	// ガード関連 //
	int		m_CurrentGuard;	// ガードゲージの現在の値
	int		m_DisplayGuard;	// ガードゲージの表示値
	int		m_RestoreGuard;	// ガードゲージの最大回復量
	int		m_ChainDamage;	// 連続して受けたダメージ
	int		m_GBreakCount;	// ガードブレイク用カウンタ

	// コンボ関連 //
	DWORD	m_LastBuzzChain;	// 前回のコンボ数
	DWORD	m_MaxBuzzCombo;		// 最大コンボ数

	BYTE	m_IsDamaged;	// ダメージを受けている場合、非ゼロ値
	BYTE	m_State;		// 現在の状態(ボム等)

	CTriEffect		*m_pTriEffect;		// 三角形エフェクト
	CEnemyCtrl		*m_pRivalEnemy;		// 相手側の敵管理
};



#endif
