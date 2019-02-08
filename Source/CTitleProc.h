/*
 *   CTitleProc.h   : タイトル画面の処理
 *
 */

#ifndef CTITLEPROC_INCLUDED
#define CTITLEPROC_INCLUDED "タイトル画面の処理 : Version 0.01 : Update 2001/06/12"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/12 : 製作開始(〆切が近い)
 */



#include "Gian2001.h"
#include "CGameMain.h"
#include "CScene.h"
#include "CSelectWindow.h"



/***** [クラス定義] *****/

// ゲーム進行用クラス //
class CTitleProc : public CScene {
public:
	IBOOL      Initialize(void);	// 初期化をする
	PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	IVOID      Draw(void);			// 描画する

	CTitleProc(CGameInfo *pSys);	// コンストラクタ
	~CTitleProc();					// デストラクタ


private:
	CSelectWindow		m_MainMenu;		// メイン
	CSelectWindow		m_ExitYesNo;	// 終了確認
	CSelectWindow		m_InputInit;	// 入力設定の初期化/可否
	CSelectWindow		m_Config;		// コンフィグ

	CSelectWindow		m_Diff;			// 難易度＆残機の設定
	CSelectWindow		m_Graphic;		// グラフィック設定
	CSelectWindow		m_Input;		// インプット設定
	CSelectWindow		m_Sound;		// サウンド設定

	CSelectWindow		m_Difficulty;	// 難易度設定
	CSelectWindow		m_BGMDev;		// ＢＧＭ再生デバイス
	CSelectWindow		m_Fps;			// ＦＰＳの設定
	CSelectWindow		m_MidiDev;		// ＭＩＤＩデバイス

	CSelectWindow		m_2PDev;		// ２Ｐ対戦用デバイス
	CSelectWindow		m_1PMode;		// １Ｐモード(Story or BossAtk)

	Pbg::CGrpSurface	*m_pBack;		// 背景用画像

	BOOL				m_bInitialized;	// 初期化されると真
};



#endif
