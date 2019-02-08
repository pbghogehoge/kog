/*
 *   Cross.h   : 十字架
 *
 */

#ifndef CROSS_INCLUDED
#define CROSS_INCLUDED "十字架       : Version 0.01 : Update 2001/11/29"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/29 : 制作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// 敵用特殊ショットはこの構造体にキャストして使用する //
typedef struct tagCrossData {
	int		m_X, m_Y;		// 現在の座標
	int		m_TargetY;		// 目標とするＹ座標
	int		m_Speed;		// 落下速度
	int		m_Accel;		// 加速度
	DWORD	m_Count;		// カウンタ

	BYTE	m_Angle;		// 進行方向(1-127 の筈)
} CrossData;



#endif
