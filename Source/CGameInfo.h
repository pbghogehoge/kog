/*
 *   CGameInfo.h   : ゲーム情報管理
 *
 */

#ifndef CGAMEINFO_INCLUDED
#define CGAMEINFO_INCLUDED "ゲーム情報管理 : Version 0.01 : Update 2001/05/22"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/22 : 製作開始
 */



#include "CScene.h"
#include "CGameProc.h"
#include "CBAtkGameProc.h"
#include "CTitleProc.h"
#include "CMRoomProc.h"
#include "CLogoProc.h"
#include "CSelectProc.h"
//#include "CDispWonProc.h"
#include "CEndingProc.h"

#include "CFpsCtrl.h"
#include "CGammaCtrl.h"
#include "CCfgIO.h"
#include "CInputCtrl.h"



/***** [ 定数 ] *****/

// 動作モード定数 //
#define GMODE_STORY			0x00	// ストーリーモード
#define GMODE_VS_2P			0x01	// ２Ｐと対戦
#define GMODE_VS_CPU		0x02	// ＣＰＵと対戦
#define GMODE_DEMO			0x03	// デモ
#define GMODE_BOSSATTACK	0x04	// ボスアタックモード

// 難易度定数 //
#define GDIFF_EASY			0x00	// 難易度 : Ｅａｓｙ
#define GDIFF_NORMAL		0x01	// 難易度 : Ｎｏｒｍａｌ
#define GDIFF_HARD			0x02	// 難易度 : Ｈａｒｄ
#define GDIFF_LUNATIC		0x03	// 難易度 : Ｌｕｎａｔｉｃ



/***** [クラス定義] *****/

// ゲーム情報管理クラス //
class CGameInfo {
public:
	FBOOL Initialize(void);		// ゲーム全体の初期化
	FBOOL UpdateGame(void);		// ゲームを進行させる(FALSE : 終了要求)

	FWORD GetSystemKeyCode(void);		// システム用キーコードを取得(オートリピート付)
//	FWORD GetDefaultKeyCode(void);		// プレイヤー１用のデフォルトキーコードを取得
	FWORD GetPlayer1_KeyCode(void);		// プレイヤー１のキーコードを取得
	FWORD GetPlayer2_KeyCode(void);		// プレイヤー２のキーコードを取得
	FBOOL IsPadInput(void);				// パッドからの入力があれば真

	FVOID SetInputMode(INPUT_MODE Mode);	// 入力モードを変更する
	FVOID WriteInputCfgData(void);			// 入力コンフィグを書き込む
	FVOID InitInputCfgData(void);			// 入力コンフィグを全初期化する

	FVOID  SetFPS(DWORD Fps);				// ＦＰＳをセットする
	FDWORD GetFPS(void);					// ＦＰＳを取得する
	FVOID  SetGamma(BYTE Gamma);			// γをセットする
	FBYTE  GetGamma(void);					// γを取得する
	FVOID  EnableGammaWindow(BOOL bEnable);	// γ設定ウィンドウを有効にする
	FVOID  EnableMouseCursor(BOOL bEnable);	// マウスカーソルを有効にする

	FVOID SetGameMode(BYTE Mode);				// ゲームの動作モードをセットする
	FBYTE GetGameMode(void);					// ゲームの動作モードを取得する
	FVOID SetBaseDifficulty(BYTE Difficulty);	// ゲームの難易度をセットする
	FBYTE GetBaseDifficulty(void);				// ゲームの難易度を取得する

	FVOID DrawSystemInfo(Pbg::CGrpSurface *pSurf, int dx=0);	// システム情報を描画する

	// 各プレイヤーの選択したキャラクタ //
	FVOID SetCharacterID(CHARACTER_ID Player1, CHARACTER_ID Player2);		// セットする
	FVOID GetCharacterID(CHARACTER_ID *pPlayer1, CHARACTER_ID *pPlayer2);	// 取得する

	FVOID  OnClearGame(void);		// ゲームが完了したときに呼び出す
	FDWORD GetNumCharacters(void);	// 使用できるキャラクタ数を返す

	FVOID OnCapture(void);						// キャプチャー要求を送出


	// 入力設定ウィンドウを有効にする    //
	FVOID EnableInputWindow(CFG_WINDOW WindowID);

	// 入力設定ウィンドウがアクティブなら真を返す //
	FBOOL IsActiveInputWindow(void);

	// 入力ヘルプ文字列を取得し、変更要求を出力 //
	FVOID GetInputHelpString(char *pBuf);


	CGameInfo(CCfgIO *pCfgIO);		// コンストラクタ
	~CGameInfo();					// デストラクタ


private:
	FVOID DrawExtraWindow(void);					// 特殊なウィンドウを描画する
	FVOID MoveExtraWindow(void);					// 特殊なウィンドウを動作させる
//	FVOID MoveInputWindow(BYTE *KeyBuf);			// 入力コンフィグ窓を動作させる
	FBOOL ChangeProcedure(PBG_PROCID ProcID);		// 動作プロシージャを変更する

	FVOID CaptureScreen(void);					// キャプチャー要求に対応する


	CGameProc			*m_pGameProc;		// ゲーム本体の管理
	CBAGameProc			*m_pGameProcBA;		// ボスアタックモード用ゲーム本体
	CTitleProc			*m_pTitleProc;		// タイトル画面の管理(コンフィグ含む)
	CMRoomProc			*m_pMRoomProc;		// 音楽室の管理
	CLogoProc			*m_pLogoProc;		// 西方ロゴの管理
	CSelectProc			*m_pSelectProc;		// 使用キャラ選択画面
	CEndingProc			*m_pEndingProc;		// エンディング
//	CDispWonProc		*m_pDispWonProc;	// 勝ちキャラ表示処理

	CCfgIO				*m_pCfgIO;			// コンフィグ管理
	GrpCfgData			m_GrpCfg;			// グラフィックの設定

	Pbg::CDIB32Effect	m_EffectDIB;		// フェードエフェクト用

	CGammaCtrl			m_GammaCtrl;		// γコントロール

	CInputCtrl			m_InputCtrl;		// 入力デバイス管理

	Pbg::CDIB32			m_SaveDIB;

/*
	// 撤去 : 2001/09/02 //
	CKeyCfgWindow		m_Player1KeyCfg;	// プレイヤー１のキーコンフィグ
	CKeyCfgWindow		m_Player2KeyCfg;	// プレイヤー２のキーコンフィグ
	CKeyCfgWindow		m_SystemKeyCfg;		// システム用のキーコンフィグ
*/
	BOOL				m_bGammaEnable;		// γ設定用ウィンドウが有効か
	BOOL				m_bMouseEnable;		// マウスカーソルが有効か

	// 撤去 : 2001/09/04 //
//	int					m_InputWindowID;	// 入力設定用ウィンドウが有効か

	WORD				m_Player1_KeyCode;	// プレイヤー１の押下キーコード
	WORD				m_Player2_KeyCode;	// プレイヤー２の押下キーコード
//	WORD				m_Default_KeyCode;	// デフォルトのキーコード
	WORD				m_System_KeyCode;	// システム用キーコード

	BOOL				m_IsPadInput;		// パッド入力があれば真

	CHARACTER_ID		m_Player1_CharID;	// プレイヤー１のキャラクタＩＤ
	CHARACTER_ID		m_Player2_CharID;	// プレイヤー１のキャラクタＩＤ

	BYTE				m_KeyWait;			// キー入力用ウェイト

	CScene				*m_pCurrentProc;	// 現在動作中のプロシージャ
	CFPSCtrl			m_FpsCtrl;			// ＦＰＳ管理

	BYTE				m_GameMode;			// ゲームの動作モード
	BYTE				m_Difficulty;		// ゲーム全体の難易度

	BOOL				m_bCapture;			// キャプチャー要求フラグ
};



#endif
