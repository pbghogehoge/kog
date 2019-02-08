/*
 *   PbgProcID.h   : プロシージャ定数の定義
 *
 */

#ifndef PBGPROCID_INCLUDED
#define PBGPROCID_INCLUDED "プロシージャ定数の定義 : Version 0.01 : Update 2001/06/12"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/12 : 制作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
typedef enum tagPBG_PROCID {
	PBGPROC_OK      = 0x0000,	// 正常終了
	PBGPROC_TITLE   = 0x0001,	// タイトルに移行
	PBGPROC_MAIN    = 0x0002,	// メインに移行
	PBGPROC_MUSIC   = 0x0003,	// 音楽室に移行
	PBGPROC_LOGO    = 0x0004,	// 西方ロゴに移行
	PBGPROC_SELECT  = 0x0005,	// キャラ選択画面に移行
	PBGPROC_ENDING  = 0x0006,	// エンディング

//	PBGPROC_DISPWON1P = 0x007,	// １Ｐ側の勝ち表示
//	PBGPROC_DISPWON2P = 0x008,	// ２Ｐ側の勝ち表示

	PBGPROC_CLOSE   = 0xffff,	// 閉じ
} PBG_PROCID;



#endif
