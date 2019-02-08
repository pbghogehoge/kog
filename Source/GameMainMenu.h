/*
 *   GameMainMenu.h   : ゲーム本体用メニュー
 *
 */

#ifndef GAMEMAINMENU_INCLUDED
#define GAMEMAINMENU_INCLUDED "ゲーム本体用メニュー : Version 0.01 : Update 2002/01/22"

/*  [更新履歴]
 *    Version 0.01 : 2002/01/22 : ゲーム本体から移動する
 */



#include "PbgType.h"
#include "CSelectWindow.h"



/***** [ 定数 ] *****/
#define ITEM_EMENU_CANCEL	0		// キャンセル
#define ITEM_EMENU_SELECT	1		// プレイヤーセレクトへ
#define ITEM_EMENU_TITLE	2		// タイトルへ

#define GAMEST_PLAY			0x00	// 通常の動作中
#define GAMEST_DRAW			0x01	// 勝敗が決定：引き分け
#define GAMEST_1PWON		0x02	// 勝敗が決定：１Ｐ勝利
#define GAMEST_2PWON		0x03	// 勝敗が決定：２Ｐ勝利
#define GAMEST_DISPWONLOST	0x04	// 勝敗表示中

//#define GAMEST_PAUSE	0x04		// ＥＳＣメニューの表示中
#define GAMEST_SELECT	0xfe		// キャラクタ選択画面へ
#define GAMEST_EXIT		0xff		// ＥＳＣより終了要求



/***** [グローバル変数] *****/
extern PBG_ITEMINFO		g_ExitMenuItem[3];



#endif
