/*
 *   CFpsCtrl.h   : FPS計測・設定
 *
 */

#ifndef CFPSCTRL_INCLUDED
#define CFPSCTRL_INCLUDED "FPS計測・設定 : Version 0.01 : Update 2001/06/12"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/12 : 製作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/

// ＦＰＳ平均取得用サンプル数 //
#define FPS_SAMPLES			32


// ＦＰＳ定数 //
#define PBGFPS_60			60				// ６０ＦＰＳモード
#define PBGFPS_30			30				// ３０ＦＰＳモード
#define PBGFPS_20			20				// ２０ＦＰＳモード
#define PBGFPS_15			15				// １５ＦＰＳモード
#define PBGFPS_10			10				// １０ＦＰＳモード
#define PBGFPS_AUTO			0				// 自動設定モード
#define PBGFPS_WITHOUT		0xffffffff		// 制限無し



/***** [クラス定義] *****/

// ＦＰＳ調整用クラス //
class CFPSCtrl {
public:
	void  SetFPS(DWORD fps);	// ＦＰＳを設定する
	DWORD GetFPS(void);			// 設定されたＦＰＳ値を取得する
	DWORD GetRealFPS(void);		// 実際のＦＰＳ値を取得する

	BOOL Update(void);			// 内部状態を更新する
	BOOL IsNeedMove(void);		// 移動部を更新する必要があるか
	BOOL IsNeedDraw(void);		// 描画部を更新する必要があるか
	void Reset(void);			// 各種リセット

	CFPSCtrl();		// コンストラクタ
	~CFPSCtrl();	// デストラクタ


private:
	DWORD	m_FPS;			// ＦＰＳ設定値
	DWORD	m_WaitTime;		// 描画用の待ち時間
	DWORD	m_LastCalled;	// 前回呼び出し時の時刻

	DWORD	m_MoveCount;	// 移動用カウンタ
	DWORD	m_DrawCount;	// 描画用カウンタ

	BOOL	m_bNeedMove;	// 移動部の更新が必要ならば真
	BOOL	m_bNeedDraw;	// 描画部の更新が必要ならば真

	DWORD	m_PassedTime[FPS_SAMPLES];	// ＦＰＳ計測用
	DWORD	m_Count;					// リングバッファ挿入先
	BOOL	m_bRealFPSEnable;			// ＦＰＳ計測値が有効なら真
};



#endif
