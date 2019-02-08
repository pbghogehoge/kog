/*
 *   CGameProc.h   : 1P/2P用ゲーム進行
 *
 */

#ifndef CGAMEPROC_INCLUDED
#define CGAMEPROC_INCLUDED "1P/2P用ゲーム進行 : Version 0.01 : Update 2001/05/22"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/22 : 製作開始
 */

#include "Gian2001.h"
#include "CGameMain.h"
#include "CSelectWindow.h"
#include "CScene.h"
#include "CDispWonProc.h"



#define NUM_STAGE		5		// 最大ステージ数
#define NUM_ROUND		3		// 最大ラウンド数(後で変数に変更のこと)



/***** [クラス定義] *****/

// ゲーム進行用クラス //
class CGameProc : public CScene {
public:
	virtual IBOOL      Initialize(void);	// 初期化をする
	virtual PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	virtual IVOID      Draw(void);			// 描画する

	CGameProc(CGameInfo *pSys);	// コンストラクタ
	virtual ~CGameProc();		// デストラクタ


private:
	PBG_PROCID MoveGameMain(void);		// １フレーム分だけ動作させる
	IVOID      DrawGameMain(void);		// 描画する

	// ステージの初期化を行う //
	BOOL InitializeStage(int nStage, BOOL bLoadMusic);

	// 次のステージ or ラウンドに進む //
	PBG_PROCID GotoNextStage(void);

	// プレイヤーの残機を描画する //
	FVOID DrawPlayerLeft(int sx, int n, CHARACTER_ID CharID);

	// 窓管理用関数 //
	static PBGWND_RET ExitWindowFunc(CGameInfo	*pSys
								   , BYTE		ItemID
								   , WORD		KeyCode);

private:
	CGameMain		*m_pPlayer1;			// プレイヤー１のゲーム管理
	CGameMain		*m_pPlayer2;			// プレイヤー２のゲーム管理

	CDispWonProc	m_DispWon;				// 勝敗表示の委譲先

	CHARACTER_ID	m_Player1_ID;			// プレイヤー１のＩＤ
	CHARACTER_ID	m_Player2_ID;			// プレイヤー２のＩＤ
	CHARACTER_ID	m_CharID[NUM_STAGE];	// ステージに対応するキャラクタＩＤ

	__int64			m_Player1Score;			// プレイヤー１のスコア
	__int64			m_Player2Score;			// プレイヤー２のスコア

	Pbg::CGrpSurface	*m_pFrame;			// 画面の外枠
	CSelectWindow		m_ExitWindow;		// 終了問い合わせ窓

	int		m_Player1AtkLv;		// プレイヤー１の攻撃レベル
	int		m_Player2AtkLv;		// プレイヤー２の攻撃レベル

	DWORD	m_Player1CGaugeMax;	// プレイヤー１の最大溜め
	DWORD	m_Player2CGaugeMax;	// プレイヤー１の最大溜め

	int		m_CurrentStage;		// 現在のステージorラウンド
	int		m_Player1Left;		// １Ｐモード時の残機数
	int		m_Player1Won;		// １Ｐ側の勝利数
	int		m_Player2Won;		// ２Ｐ側の勝利数
	BOOL	m_Is2PMode;			// ２Ｐモードなら真

	DWORD	m_DoorWait;			// ドアを閉じたあとのウェイト

	// うむ、確かに気持ち悪いな //
	static BYTE		m_State;	// 現在の状態
	static BOOL		m_bPaused;	// 停止中なら真


#ifdef PBG_DEBUG
	static BOOL	m_bNeedDebugReset;
#endif
};





#endif
