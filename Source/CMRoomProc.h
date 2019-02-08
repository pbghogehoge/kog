/*
 *   CMRoomProc.h   : 音楽室の処理
 *
 */

#ifndef CMROOMPROC_INCLUDED
#define CMROOMPROC_INCLUDED "音楽室の処理 : Version 0.01 : Update 2001/06/17"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/17 : 製作開始(間に合うのか？)
 */



#include "Gian2001.h"
#include "CGameMain.h"
#include "CScene.h"



/***** [クラス定義] *****/

// 音楽室処理用クラス //
class CMRoomProc : public CScene {
public:
	IBOOL      Initialize(void);	// 初期化をする
	PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	IVOID      Draw(void);			// 描画する


	CMRoomProc(CGameInfo *pSys);	// コンストラクタ
	~CMRoomProc();					// デストラクタ


private:
	FBOOL Play(int dir);	// 再生する
	FBOOL Fade(void);		// フェードアウト＆停止
	FBOOL Stop(void);		// 停止する
	FBOOL Tempo(int delta);	// テンポを変更する

	FVOID DrawNumber(int x, int y, int n);		// 数字の描画を行う

	// ループ再生用通知関数 //
	static MIDIN_RET MidiNotify(BYTE Message, DWORD Param);


	CTextWindow		m_TitleWindow;	// タイトル表示ウィンドウ

	DWORD		m_State;			// 現在の状態
	DWORD		m_Count;			// カウンタ
	DWORD		m_NumMusic;			// 曲数
	DWORD		m_CurrentMusic;		// 現在再生中の曲

	DWORD		m_LvTable[16];		// レベル表示用テーブル

	char		m_CurrentTempo;		// 現在のテンポ補正値
};



#endif
