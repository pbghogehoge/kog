/*
 *   Neru.h   : ねる
 *
 */

#ifndef NERU_INCLUDED
#define NERU_INCLUDED "ねる : Version 0.01 : Update 2001/12/25"

/*  [更新履歴]
 *    Version 0.01 : 2001/12/25 : 製作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

typedef struct tagNeruData {
	int		m_Ox;			// 始点のＸ座標
	int		m_Oy;			// 始点のＹ座標
	int		m_Speed;		// 「ねる」の移動速度
	int		m_Length;		// 現在の長さ
	int		m_MaxLength;	// 最大長

	DWORD	m_Count;		// 経過時間

	BYTE	m_Color;		// 「ねる」の色
	BYTE	m_Angle;		// 「ねる」の進行方向
} NeruData;



#endif
