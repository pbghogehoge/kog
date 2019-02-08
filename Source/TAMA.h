/*
 *   Tama.h   : 弾管理構造体
 *
 */

#ifndef TAMA_INCLUDED
#define TAMA_INCLUDED "弾管理構造体 : Version 0.03 : Update 2001/03/08"

/*  [更新履歴]
 *    Version 0.03 : 2001/03/08 : アニメーション系の微調整 //
 *    Version 0.02 : 2001/03/06 : 角度ずらし＆サイズ指定機能の追加
 *    Version 0.01 : 2001/03/05 : 定数名が大幅に変更されているので注意
 */



#include "PbgType.h"



/***** [ 定数 ] *****/

// 弾の種類(BYTE) //
#define TAMA_NORM		0x00	// 通常弾
#define TAMA_NORM_A		0x01	// 加速弾
#define TAMA_HOMING		0x02	// ｎ回ホーミング
#define TAMA_HOMING_M	0x03	// ｎ％ホーミング
#define TAMA_ROLL		0x04	// 回転弾
#define TAMA_ROLL_A		0x05	// 回転弾(加速)
#define TAMA_ROLL_R		0x06	// 回転弾(反転)
#define TAMA_GRAVITY	0x07	// 落下弾
#define TAMA_CHANGE		0x08	// 角度変更弾


// 弾オプション定数(BYTE) : 下位は繰り返し回数 //
#define TOPT_NONE		0x00	// オプションなし
#define TOPT_REFX		0x10	// 反射Ｘ
#define TOPT_REFY		0x20	// 反射Ｙ
#define TOPT_REFXY		0x30	// 反射ＸＹ
#define TOPT_REFX2		0x40	// 反射Ｘ + 自機セット



// 弾発射コマンド定数(WORD)                     //
// 参考：上位が角度指定、下位がオプションとなる //
#define TCMD_WAY		0x0000		// 扇状発射				0000 ...
#define TCMD_ALL		0x0100		// 全方向発射			0001 ...
#define TCMD_RND		0x0200		// ランダム方向発射		0010 ...
#define TCMD_ONE		0x0300		// １方向発射			0011 ...

#define TCMD_SET		0x0001		// 自機セット属性			0000 0001
#define TCMD_NUM		0x0002		// 連弾属性					0000 0010
#define TCMD_NUMSHIFT	0x0006		// 連弾 + 角度ずらし有効	0000 0110
#define TCMD_RNDSPD		0x0008		// 速度ランダム属性			0000 1000
#define TCMD_NODIF		0x0010		// 難易度反映無効化フラグ	0001 0000

#define TCMD_SHIFT		0x0004		// 角度ズラシ有効



/***** [構造体] *****/

// 弾データ構造体 //
typedef struct tagTamaData {
	int		x, y;		// 現在の座標
	int		vx, vy;		// 速度成分(x, y)

	int		v;			// 速度
	int		v0;			// 初速度
	int		a;			// 加速度

	DWORD	Count;		// 発射から何フレーム経過したか(移動単位)

	BYTE	Angle;		// 進行角度
	char	vAngle;		// 角速度

	BYTE	Color;		// 外見の指定(上位が形状、下位が色)
	BYTE	Rep;		// 繰り返し回数
	BYTE	Type;		// 移動タイプ
	BYTE	Option;		// 属性

	char	AnmSpd;		// アニメーションスピード
	BYTE	Anime;		// 現在のパターン(角度アニメ時)

	BYTE	Size;		// 大きさ(x256)
	BYTE	IsShaved;	// すでにカスったものか

	BYTE	IsClipped;	// クリッピングされていれば非ゼロ
	BYTE	Score;		// 得点があるならば、非ゼロ
} TamaData;



#endif
