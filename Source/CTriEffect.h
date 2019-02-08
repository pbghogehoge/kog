/*
 *   CTriEffect.h   : 三角形エフェクト
 *
 */

#ifndef CTRIEFFECT_INCLUDED
#define CTRIEFFECT_INCLUDED "三角形エフェクト : Version 0.01 : Update 2001/04/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/27 : 
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define TRIEFC_MAX			400		// 三角エフェクトの最大数
#define TRIEFC_KIND			2		// 三角エフェクトの種類

#define TRIEFC_GBREAK			0x00	// ガードブレイク用トライアングル
#define TRIEFC_BOSS_GBREAK		0x01	// ボス・ガードブレイク用トライアングル



/***** [クラス定義] *****/

typedef struct tagTriEfcData {
	int		ox, oy;		// 正三角形の重心の座標
	int		vx, vy;		// 速度のＸ＆Ｙ成分
	int		r;			// 正三角形の重心から頂点までの距離

	DWORD	Count;		// カウンタ

	BYTE	rx, ry, rz;		// 回転角
	char	dx, dy, dz;		// 角速度
} TriEfcData;


// 三角エフェクト管理クラス //
class CTriEffect : public CFixedLList<TriEfcData, TRIEFC_KIND, TRIEFC_MAX> {
public:
	FVOID Initialize(void);						// 初期化を行う
	FVOID SetGuardBreak(int x256, int y256);	// ガードブレイク用エフェクトをセット
	FVOID SetBossGB(int x256, int y256);		// ボス用ガードブレイクエフェクトをセット
	FVOID Move(void);							// １フレーム進める
	FVOID Draw(void);							// 三角エフェクトを描画する

	CTriEffect();	// コンストラクタ
	~CTriEffect();	// デストラクタ


private:
};



#endif
