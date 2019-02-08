/*
 *   LSphere.h   : 光球とびりびり管理
 *
 */

#ifndef LSPHERE_INCLUDED
#define LSPHERE_INCLUDED "光球とびりびり管理 : Version 0.01 : Update 2001/11/04"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/04 : 制作開始
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define NUM_LSPHERE		12		// ひとつの繋がりにおける光球の最大数



/***** [クラス定義] *****/

// らいとにんぐすふぃあ管理 //
typedef struct tagLSphere {
	POINT	m_CurrentPos[NUM_LSPHERE];	// 現在の位置

	int		m_NumSphere;	// 光球の数
	DWORD	m_StateCount;	// 経過時間
	DWORD	m_AnmCount;		// アニメーションカウンタ

	// ベクトル : p[n] -> p[n+1] に関する情報 //
	int		m_LengthTable[NUM_LSPHERE];	// 長さ
	BYTE	m_AngleTable[NUM_LSPHERE];	// 角度
} LSphere;



#endif
