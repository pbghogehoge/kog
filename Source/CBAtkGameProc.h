/*
 *   CBAtkGameProc.h   : ボスアタックモード
 *
 */

#ifndef CBATKGAMEPROC_INCLUDED
#define CBATKGAMEPROC_INCLUDED "ボスアタックモード : Version 0.01 : Update 2002/01/22"

/*  [更新履歴]
 *    Version 0.01 : 2002/01/22 : 制作開始
 */



#include "Gian2001.h"
#include "CGameMainBA.h"
#include "CSelectWindow.h"
#include "CScene.h"
#include "CDispWonProc.h"



/***** [ 定数 ] *****/
#define BA_NUM_STAGE		9



/***** [クラス定義] *****/

// ゲーム進行用クラス //
class CBAGameProc : public CScene {
public:
	IBOOL      Initialize(void);	// 初期化をする
	PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	IVOID      Draw(void);			// 描画する

	CBAGameProc(CGameInfo *pSys);	// コンストラクタ
	virtual ~CBAGameProc();			// デストラクタ


private:
	// プレイヤーの残機を描画する //
	FVOID DrawPlayerLeft(int sx, int n, CHARACTER_ID CharID);

	// 窓管理用関数 //
	static PBGWND_RET ExitWindowFunc(CGameInfo	*pSys
								   , BYTE		ItemID
								   , WORD		KeyCode);


private:
	CGameMainBA		*m_pGameMain;			// ゲーム管理

	CHARACTER_ID	m_Player_ID;			// プレイヤー１のＩＤ
//	CHARACTER_ID	m_CharID[BA_NUM_STAGE];	// ステージに対応するキャラクタＩＤ

	Pbg::CGrpSurface	*m_pFrame;			// 画面の外枠
	CSelectWindow		m_ExitWindow;		// 終了問い合わせ窓

	int		m_CurrentStage;		// 現在のステージorラウンド
	int		m_Player1Left;		// １Ｐモード時の残機数

	DWORD	m_DoorWait;			// ドアを閉じたあとのウェイト

	static BYTE		m_State;	// 現在の状態
	static BOOL		m_bPaused;	// 停止中なら真


#ifdef PBG_DEBUG
	static BOOL	m_bNeedDebugReset;
#endif
};



#endif
