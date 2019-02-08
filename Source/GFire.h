/*
 *   GFire.h   : ゲイツなにがし
 *
 */

#ifndef GFIRE_INCLUDED
#define GFIRE_INCLUDED "ゲイツなにがし : Version 0.01 : Update 2001/12/20"

/*  [更新履歴]
 *    Version 0.01 : 2001/12/20 : 制作開始(いいのか？)
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// ゲイツなにがし管理 //
// 敵用特殊ショットはこの構造体にキャストして使用すること！！ //
typedef struct tagGFireData {
	int		m_X;		// 現在のＸ座標
	int		m_Y;		// 現在のＹ座標
	int		m_v;		// 移動速度
	int		m_a;		// 加速度
	int		m_Size;		// 当たり判定サイズ

	DWORD	m_Count;		// カウンタ

	BYTE	m_BaseAngle;	// 基本角
	BYTE	m_Angle;		// 拡散するための値
	BYTE	m_Alpha;		// 拡散速度 & α値
} GFireData;



#endif
