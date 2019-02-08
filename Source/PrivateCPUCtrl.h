/*
 *   PrivateCPUCtrl.h   : ＣＰＵの動作(非公開)
 *
 */

#ifndef PRIVATECPUCTRL_INCLUDED
#define PRIVATECPUCTRL_INCLUDED "ＣＰＵの動作(非公開) : Version 0.01 : Update 2001/05/10"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/10 : 製作開始
 */



#include "Gian2001.h"



/***** [クラス定義] *****/

// 当たり判定チェック用データ構造体 //
typedef struct tagHitCheckStruct {
	int			x, y;			// 現在の座標
	short		HitSize;		// 当たり判定サイズ
	BYTE		d;				// 進行方向
	BYTE		IsShaved;		// すでにカスっていれば真
} HitCheckStruct;



#endif
