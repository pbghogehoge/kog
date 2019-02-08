/*
 *   CNormalShot.h   : 自機の通常ショット
 *
 */

#ifndef CNORMALSHOT_INCLUDED
#define CNORMALSHOT_INCLUDED "自機の通常ショット : Version 0.01 : Update 2001/04/06"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/06 : 制作開始
 */



#include "Gian2001.h"
#include "CCharacterBase.h"
#include "Enemy.h"



/***** [ 定数 ] *****/
#define NORMALSHOT_MAX		50		// ノーマルショットの最大数
#define NSHOT_NORMAL		0x00	// 通常状態
#define NSHOT_DELETE		0x01	// 消去状態



/***** [クラス定義] *****/

// ノーマルショット格納用クラス //
class NormalShot {
public:
	int		x, y;	// 現在の座標
	DWORD	count;	// カウンタ
	BYTE	d;		// 進行角度(エフェクト専用)
};


// ノーマルショット管理用クラス //
class CNormalShot : public CFixedLList<NormalShot, 2, NORMALSHOT_MAX> {
public:
	// 初期化する //
	FVOID Initialize(CCharacterBase *pCharInfo);

	FVOID  Set(void);					// セットする
	FVOID  Move(void);					// １フレームだけ動かす
	FVOID  Draw(void);					// 描画する
	FDWORD HitCheck(EnemyData *pEnemy);	// 当たり判定

	CNormalShot(RECT *rcTargetX256, int *pX, int *pY);	// コンストラクタ
	~CNormalShot();										// デストラクタ


private:
	int			*m_pX;		// セットするＸ座標へのポインタ
	int			*m_pY;		// セットするＹ座標へのポインタ
	int			m_dx;		// Ｘ方向にどれだけズラすか
	int			m_dy;		// Ｙ方向にどれだけズラすか
	int			m_YMin;		// Ｙ方向の消去座標

	int			m_Type;			// 弾の形状
	DWORD		m_Count;		// 自動連射用カウンタ
};



#endif
