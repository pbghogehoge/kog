/*
 *   Laser.h   : レーザーの定義
 *
 */

#ifndef LASER_INCLUDED
#define LASER_INCLUDED "レーザーの定義 : Version 0.01 : Update 2001/04/01"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/01 : 制作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/

// ショートレーザーの種類 //
#define SLASER_NORM		0x00	// 通常動作
#define SLASER_REF		0x01	// 反射レーザー
#define SLASER_LLREF	0x02	// LL反射
#define SLASER_SEARCH	0x03	// 探索反射Ｌ


// レーザー発射コマンド定数 //
// 弾コマンドと互換性有り   //
#define LCMD_WAY		0x0000		// 扇状発射				0000 ...
#define LCMD_ALL		0x0100		// 全方向発射			0001 ...
#define LCMD_RND		0x0200		// ランダム方向発射		0010 ...
#define LCMD_ONE		0x0300		// １方向発射			0011 ...

#define LCMD_SET		0x0001		// 自機セット属性			0000 0001
#define LCMD_RNDSPD		0x0008		// 速度ランダム属性			0000 1000
#define LCMD_NODIF		0x0010		// 難易度反映無効化フラグ	0001 0000


// 反射レーザー状態定数 //
#define LRST_NONE		0x00	// 反射レーザー：通常の移動中
#define LRST_SHOT		0x01	// 反射レーザー：発射中
#define LRST_HIT		0x02	// 反射レーザー：ヒット中
#define LRST_NMOVE		0x03	// 反射レーザー：反射＆ヒット中(SHOT|HIT = 3)


// 太レーザーの状態定数 //
#define LLST_NONE		0x00	// 太レーザー：通常の待ち
#define LLST_RESIZE		0x01	// 太レーザー：サイズ変更中
#define LLST_CLOSEL		0x02	// 太レーザー：クローズ(溜め状態に移行中)
#define LLST_CHARGE		0x03	// 太レーザー：溜め状態
#define LLST_CLOSE		0x04	// 太レーザー：消去用リストに移動要求



/***** [ 構造体 ] *****/

// ショートレーザー用構造体 //
typedef struct tagSLaserData {
	int		x, y;		// 現在の始点の座標
	int		vx, vy;		// 速度成分(x, y)

	int		v;			// 速度
	int		l;			// 現在の長さ
	int		lmax;		// 最大長
	int		lwait;		// 仮の長さ(時間待ち用)

	DWORD	Count;		// カウンタ

	BYTE	Angle;		// 角度
	BYTE	Color;		// 色
	BYTE	Rep;		// 繰り返し回数
	BYTE	Type;		// 種類定数

	BYTE	IsShaved;	// カスっているか
	BYTE	RefState;	// 反射ステート
} SLaserData;


class EnemyAtkCtrl;


// 太レーザー管理用構造体 //
typedef struct tagLLaserData {
	int		m_x, m_y;		// 始点の座標
	int		m_Width;		// レーザーの現在の幅
	int		m_TargetWidth;	// レーザーの目標とする幅
	int		m_Speed;		// サイズ変更の早さ

	DWORD	m_Count;		// カウンタ

	EnemyAtkCtrl		*m_pTarget;		// 対象となる攻撃オブジェクト
	BYTE				m_State;		// 現在の状態
	BYTE				m_Angle;		// 発射角度
	BYTE				m_Color;		// レーザーの色
} LLaserData;



#endif
