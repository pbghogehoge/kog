/*
 *   CFragmentEfc.h   : 破片処理
 *
 */

#ifndef CFRAGMENTEFC_INCLUDED
#define CFRAGMENTEFC_INCLUDED "破片処理     : Version 0.01 : Update 2001/04/22"



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define FRAGMENT_MAX		200		// 破片の最大数
#define FRAGMENT_KIND		3		// 破片の種類
#define FRG_POINT			0x00	// 点エフェクト
#define FRG_SMOKE_P			0x01	// 煙エフェクト(ぴんく)
#define FRG_SMOKE_W			0x02	// 煙エフェクト(白)


/***** [クラス定義] *****/

// 破片管理構造体 //
typedef struct tagFragmentData {
	int		x, y;		// 現在の座標
	int		vx, vy;		// 現在の速度成分
	DWORD	Count;		// カウンタ
} FragmentData;


// 破片エフェクト管理クラス //
class CFragmentEfc : public CFixedLList<FragmentData, FRAGMENT_KIND, FRAGMENT_MAX> {
public:
	FVOID Initialize(void);								// 初期化を行う
	FVOID SetPointEfc(int x256, int y256, int n);		// 指定座標に点破片をセットする
	FVOID SetSmokeEfc(int x256, int y256, BYTE Target);	// 指定座標に煙を発生させる
	FVOID Move(void);									// 破片を動かす
	FVOID Draw(void);									// 破片を描画する

	CFragmentEfc(RECT *rcTargetX256);	// コンストラクタ
	~CFragmentEfc();					// デストラクタ


private:
	FVOID DrawPoint(void);	// 点エフェクトを描画する
	FVOID DrawSmokeW(void);	// 煙エフェクトを描画する(白)
	FVOID DrawSmokeP(void);	// 煙エフェクトを描画する(ピンク)

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標
};



#endif
