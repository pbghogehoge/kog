/*
 *   HLaser.h   : ホーミングレーザーの定義
 *
 */

#ifndef HLASER_INCLUDED
#define HLASER_INCLUDED "ホーミングレーザーの定義 : Version 0.01 : Update 2001/09/14"

/* [ 参考 ]
 *   このホーミングレーザーは、次のような動作をする
 *
 *   ① 捕捉中      : 減速＆目標に向かって角度補正
 *   ② 加速中      : 目標に向かって加速、微妙に角度補正
 *   ③ ボム消去中  : 基本動作は②と同じだが、消去ＥＦＣがかかる
 */

/*  [更新履歴]
 *    Version 0.01 : 2001/09/14 : 制作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
#define EHLASER_LENGTH	22			// ホーミングレーザーの長さ

// AttackCommand.Type の値 //
#define HLASER_RND		0x00		// ランダムロックオン属性
#define HLASER_PLAYER	0x01		// 自機周辺ロックオン属性
#define HLASER_SETX		0x02		// 発射位置とＸ座標を一致させる



/***** [ 構造体 ] *****/

// ホーミングレーザーの各節のデータ //
typedef struct tagHLaserInfo {
	int		x, y;	// 現在の座標
	BYTE	d;		// 現在の角度
} HLaserInfo;


// ホーミングレーザー用構造体                           //
// 注意：状態(捕捉中・加速中・ボム消去中)はＬＬにて管理 //
typedef struct tagHLaserData {
	int			TargetX, TargetY;		// ロックオン対象の座標
	int			v;						// 速度
	int			v0;						// 初速度
	int			a;						// 加速度

	DWORD		count;					// カウンタ

	HLaserInfo	HLInfo[EHLASER_LENGTH];	// ホーミングレーザーの節情報キュー
	char		queue_head;				// 節情報キューの先頭
	BYTE		c;						// 色
} HLaserData;



#endif
