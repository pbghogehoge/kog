/*
 *   RLaser.h   : ラウンドレーザー
 *
 */

#ifndef RLASER_INCLUDED
#define RLASER_INCLUDED "ラウンドレーザー : Version 0.01 : Update 2001/11/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/27 : 制作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// ラウンドレーザー管理 //
// 敵用特殊ショットはこの構造体にキャストして使用すること！！ //
typedef struct tagRLaserData {
	int		m_X;			// 中心となるＸ座標
	int		m_Y;			// 中心となるＹ座標
	int		m_Radius;		// 半径
	int		m_TargetRadius;	// 目標半径
	int		m_Speed;		// 展開速度
	DWORD	m_Count;		// カウンタ
} RLaserData;



#endif
