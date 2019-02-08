/*
 *   CEnemyLSphere.h   : らいとにんぐすふぃあ
 *
 */

#ifndef CENEMYLSPHERE_INCLUDED
#define CENEMYLSPHERE_INCLUDED "らいとにんぐすふぃあ : Version 0.01 : Update 2001/11/04"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/04 : 制作開始
 */

#include "Gian2001.h"
#include "LSphere.h"
#include "CShaveEffect.h"

#define ELSPHERE_KIND		3
#define ELSPHERE_MAX		10

#define ELSPHERE_WAIT		0x00		// 発動予備動作
#define ELSPHERE_NORMAL		0x01		// 発動中
#define ELSPHERE_DELETE		0x02		// 消去状態



/***** [クラス定義] *****/

// 光り輝く光球と結合する電気ばちばち //
class CEnemyLSphere : public CFixedLList<LSphere, ELSPHERE_KIND, ELSPHERE_MAX> {
public:
	FVOID Set(int AtkLv);		// セットする
	FVOID Initialize(void);		// 初期化する
	FVOID Clear(void);			// 敵弾全てに消去エフェクトをセットする
	FVOID Move(void);			// 弾を移動させる
	FVOID Draw(void);			// 弾を描画する

	// 当たり判定を行う               //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// コンストラクタ //
	CEnemyLSphere(RECT	*rcTargetX256			// 対象矩形
			 , int	*pX						// 当たり判定Ｘ座標へのポインタ
			 , int	*pY						// 当たり判定Ｙ座標へのポインタ
			 , CShaveEffect *pShaveEfc);	// カスりエフェクト発動用クラス

	~CEnemyLSphere();	// デストラクタ


private:
	FVOID DrawSphere(LSphere *pSp, BYTE a);		// びりびり球体の描画
	FVOID DrawThunder(LSphere *pSp, BYTE a);	// 雷の描画

	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標
	int		m_XMid;		// Ｘ座標中心
	int		m_YMid;		// Ｙ座標中心

	// エフェクト関連 //
	CShaveEffect	*m_pShaveEfc;
};



#endif
