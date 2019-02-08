/*
 *   CExtraShot.h   : Ｌｖ１・溜め管理
 *
 */

#ifndef CEXTRASHOT_INCLUDED
#define CEXTRASHOT_INCLUDED "Ｌｖ１・溜め管理 : Version 0.01 : Update 2001/04/16"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/16 :
 */



#include "Gian2001.h"
//#include "CCharacterBase.h"



/***** [ 定数 ] *****/
#define EXTRASHOT_MAX		150		// エキストラショットの数
#define EXTRAOPTION_MAX		50		// エキストラショット演出の数
#define EXTRASHOT_KIND		12		// エキストラショットの種類



/***** [クラス定義] *****/

class ExtraOption {
public:
	int		x, y;		// 現在の座標
	WORD	count;		// カウンタ
	BYTE	d;			// 現在の角度
	BYTE	a;			// 現在の不透明度
};


class ExtraShot {
public:
	int		x, y;		// 現在の基本座標
	int		v;			// 速度
	DWORD	Count;		// カウンタ

	BYTE	d;			// 進行角


	// 演出用データ //
	ExtraOption		Option[EXTRAOPTION_MAX];
};


// 相互参照用 //
class CCharacterBase;


class CExtraShot : public CFixedLList<ExtraShot, EXTRASHOT_KIND, EXTRASHOT_MAX> {
public:
	// 初期化する //
	FVOID Initialize(CCharacterBase *pCharInfo);

	// 現在の座標へのポインタを取得する //
	int *GetXPointer(void){ return m_pX; };		// Ｘ座標へのポインタ
	int *GetYPointer(void){ return m_pY; };		// Ｙ座標へのポインタ

	// 描画矩形を取得する //
	int GetXMin(void){ return m_XMin; };	// 左端
	int GetYMin(void){ return m_YMin; };	// 上端
	int GetXMax(void){ return m_XMax; };	// 右端
	int GetYMax(void){ return m_YMax; };	// 下端

	CExtraShot(RECT *rcTargetX256, int *pX, int *pY);	// コンストラクタ
	~CExtraShot(void);									// デストラクタ


private:
	// キャラクター固有の情報管理 //
	CCharacterBase		*m_pCharacter;

	// 座標関連の情報 //
	int			*m_pX;	// 発射時の基本Ｘ座標
	int			*m_pY;	// 発射時の基本Ｙ座標

	// 画面外判定用(x256) //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標
};



#endif
