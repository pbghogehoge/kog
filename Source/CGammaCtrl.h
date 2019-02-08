/*
 *   CGammaCtrl.h   : γ補正処理
 *
 */

#ifndef CGAMMACTRL_INCLUDED
#define CGAMMACTRL_INCLUDED "γ補正処理   : Version 0.01 : Update 2001/06/24"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/24 : 製作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// γ補正管理クラス //
class CGammaCtrl {
public:
	FBOOL SetGamma(int Gamma);	// 補正値をセットする
	FBYTE GetGamma(void);		// 補正値を取得する

	FVOID Move(void);		// ウィンドウ周りの動作
	FVOID Draw(void);		// ウィンドウ周りの描画

	FVOID SetWindowPos(int sx, int sy);	// 絶対指定で座標を変更

	CGammaCtrl();	// コンストラクタ
	~CGammaCtrl();	// デストラクタ


private:
	// 現在のγ値を反映させる //
	FVOID SetGammaTable(void);

	DDGAMMARAMP		m_DefaultGamma;		// γランプ補正後の値
	DDGAMMARAMP		m_CurrentGamma;		// 現在のγランプ

	BOOL	m_bInitialized;		// 初期化されているか
	int		m_Gamma;			// 現在のγ補正値 * 100

	int		m_sx;	// ウィンドウ左上のＸ座標
	int		m_sy;	// ウィンドウ左上のＹ座標
};



#endif
