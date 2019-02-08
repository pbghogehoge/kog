/*
 *   CDispWonProc.h   : 勝ちキャラ表示
 *
 */

#ifndef CDISPWONPROC_INCLUDED
#define CDISPWONPROC_INCLUDED "勝ちキャラ表示 : Version 0.02 : Update 2001/12/05"

/*  [更新履歴]
 *    Version 0.02 : 2001/12/05 : CScene から切り離し
 *    Version 0.01 : 2001/11/02 : 制作開始
 */



#include "Gian2001.h"
#include "CScene.h"



/***** [ 定数 ] *****/



/***** [クラス定義] *****/

class CDispWonProc {
public:
	// 初期化をする //
	FBOOL Initialize(	PBG_PROCID		RetCode,
						CHARACTER_ID	IdWon,
						CHARACTER_ID	IdLost,
						BYTE			PlayerNo);

	FBOOL Move(PBG_PROCID *pRet);	// １フレーム分だけ動作させる
	FVOID Draw(void);				// 描画する

	CDispWonProc(CGameInfo *pSys);	// コンストラクタ
	~CDispWonProc();				// デストラクタ


private:
	// さようなら //
	FVOID Cleanup(void);

	// 勝ったほうのプレイヤーをセット //
	FBOOL SetCharacterID(CHARACTER_ID IdWon, CHARACTER_ID IdLost, BYTE PlayerNo);

	// キャラクタＩＤから文字列取得 //
	const char *CharID2Str(CHARACTER_ID id);

	Pbg::CGrpSurface	*m_pBG;				// 背景用Surface

	CGameInfo			*m_pSys;

	PBG_PROCID			m_RetCode;			// 終了時に返すべき値
	DWORD				m_Count;			// カウンタ
	int					m_WindowHeight;		// ウィンドウの高さ
	BYTE				m_PlayerNo;			// プレイヤー番号(1 or 2)
};



#endif
