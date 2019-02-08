/*
 *   CShaveEffect.h   : カスりエフェクト管理
 *
 */

#ifndef CSHAVEEFFECT_INCLUDED
#define CSHAVEEFFECT_INCLUDED "カスりエフェクト管理 : Version 0.01 : Update 2001/04/07"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/07 : 製作開始
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define SHAVE_MAX		50



/***** [クラス定義] *****/

// カスりエフェクト単体管理 //
typedef struct tagShaveData {
	int		x, y;		// 始点の座標
	int		vx, vy;		// 速度成分
	int		Length;		// エフェクトの長さ
	DWORD	Count;		// カウンタ
	BYTE	Angle;		// 進行方向
} ShaveData;


// カスりエフェクト管理クラス //
class CShaveEffect : public CFixedLList<ShaveData, 1, SHAVE_MAX> {
public:
	FVOID Initialize(void);				// 初期化を行う
	FVOID Set(int x, int y, BYTE d);	// エフェクトをセットする
	FVOID Move(void);					// エフェクトを１フレーム動作させる
	FVOID DrawFragment(void);			// 破片を描画する
	FVOID DrawCombo(void);				// コンボ数を描画する

	// 強制リセット                                              //
	// Initialize() とは異なり、ラインエフェクトの消去は行わない //
	FVOID ForceReset(void);

	// カスりコンボ数を返す //
	DWORD GetShaveCombo(void){
		return m_ShaveCombo;
	};

	CShaveEffect(RECT *rcTargetX256);	// コンストラクタ
	~CShaveEffect();					// デストラクタ


private:
	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標

	int		m_ComboX;	// コンボ数の表示Ｘ座標
	int		m_ComboY;	// コンボ数の表示Ｙ座標

	int		m_Timer;		// ウェイト
	DWORD	m_ShaveCombo;	// カスりコンボ数
};



#endif
