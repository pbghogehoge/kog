/*
 *   WinMain.h   : ウィンドウ周りの処理
 *
 */

#ifndef WINMAIN_INCLUDED
#define WINMAIN_INCLUDED "ウィンドウ周りの処理 : Version 0.01 : Update 2001/02/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/02/27 : 製作開始
 */



#include "Gian2001.h"
#include "CCfgIO.h"



/***** [関数プロトタイプ] *****/
extern void XMainLoop(void);				// メインループ
extern BOOL XInitialize(CCfgIO *pConfig);	// 初期化関数
extern void XCleanup(void);					// 終了処理関数



/***** [グローバル変数] *****/
extern HWND g_hMainWindow;		// ウィンドウハンドル



#endif
