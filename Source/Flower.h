/*
 *   Flower.h   : 花
 *
 */

#ifndef FLOWER_INCLUDED
#define FLOWER_INCLUDED "花           : Version 0.01 : Update 2001/11/30"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/30 : 制作開始
 */



#include "PbgType.h"




/***** [クラス定義] *****/

// 花の管理 //
// 敵用特殊ショットはこの構造体にキャストして使用すること！！ //
typedef struct tagFlowerData {
	int		m_X;			// 中心となるＸ座標
	int		m_Y;			// 中心となるＹ座標
	int		m_vx;			// Ｘ方向の速度成分
	int		m_vy;			// Ｙ方向の速度成分
	int		m_Radius;		// 半径
	DWORD	m_Count;		// カウンタ

	BYTE	m_Color;		// 花の色(0..3)
	BYTE	m_Angle;		// 花の傾き
	char	m_Omega;		// 角速度
} FlowerData;



#endif
