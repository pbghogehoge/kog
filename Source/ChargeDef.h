/*
 *   ChargeDef.h   : 溜め定数定義
 *
 */

#ifndef CHARGEDEF_INCLUDED
#define CHARGEDEF_INCLUDED "溜め定数定義 : Version 0.01 : Update 2001/04/17"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/17 : 制作開始
 */




/***** [ 定数 ] *****/
#define CGAUGE_ATATK_TIME	(60*8)		// 溜め攻撃・自動発動までの時間
#define CGAUGE_WEIGHT		65536		// 溜めゲージの重み

#define CGAUGE_LV1HALF	((CGAUGE_WEIGHT/2) * 64)	// レベル１/２アタック発動
#define CGAUGE_LEVEL1	(CGAUGE_WEIGHT * 64 * 1)	// レベル１アタック発動時刻
#define CGAUGE_LEVEL2	(CGAUGE_WEIGHT * 64 * 2)	// レベル２アタック発動時刻
#define CGAUGE_LEVEL3	(CGAUGE_WEIGHT * 64 * 3)	// レベル３アタック発動時刻
#define CGAUGE_LEVEL4	(CGAUGE_WEIGHT * 64 * 4)	// レベル４アタック発動時刻



#endif
