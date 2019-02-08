/*
 *   IonRing.h   : イオンリング定義
 *
 */

#ifndef IONRING_INCLUDED
#define IONRING_INCLUDED "イオンリング定義 : Version 0.01 : Update 2001/12/24"

/*  [更新履歴]
 *    Version 0.01 : 2001/12/24 : 制作開始
 */

#include "PbgType.h"



/***** [クラス定義] *****/

// イオンリングの管理 //
// 敵用特殊ショットはこの構造体にキャストして使用すること！！ //
typedef struct tagIonRingData {
	int		x, y;				// 現在の座標(x256 固定小数点数)
	int		vx, vy;				// 現在の速度成分

	int		size;				// 当たり判定サイズ

	// ここまでは、EnemyData と同じ並びになっている //

	DWORD	Count;				// 発生時からの経過カウント
} IonRingData;



#endif
