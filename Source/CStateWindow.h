/*
 *   CStateWindow.h   : 状態表示窓
 *
 */

#ifndef CSTATEWINDOW_INCLUDED
#define CSTATEWINDOW_INCLUDED "状態表示窓   : Version 0.01 : Update 2002/01/30"

/*  [更新履歴]
 *    Version 0.01 : 2002/01/30 : 制作開始
 */



#include "CRingBuffer.h"



/***** [クラス定義] *****/

// 表示文字列情報 //
class CStateTxtInfo {
public:
	inline const char *getTxt(void) const;	// 文字列を取得
	IDWORD getColor(void)     const;		// 色を取得
	IDWORD getFlag(void)      const;		// フラグを取得

	// 初期化用コンストラクタ .//
	CStateTxtInfo(char *pTxt, DWORD col, DWORD flag);

	// コンストラクタ＆デストラクタ //
	CStateTxtInfo(){};
	~CStateTxtInfo(){};


private:
	char		m_Txt[256];		// 文字列の格納先
	DWORD		m_Color;		// 文字列色
	DWORD		m_Flag;			// 属性フラグ
};


// 状態表示窓管理クラス //
class CStateWindow {
public:
	FVOID Initialize(void);	// 初期化を行う
	FVOID Move(void);		// １フレーム動作させる
	FVOID Draw(void);		// 描画を行う

	// 文字列を挿入する //
	FVOID InsertString(BYTE StrID, int value);
	FVOID InsertString(char *pStr, DWORD col, DWORD flag);

	CStateWindow(RECT *rcTarget, int *pX, int *pY);
	~CStateWindow();


private:
	// ウィンドウの描画を行う //
	FVOID DrawStateWindow(int sx, BYTE a);
	FVOID DrawString(const CStateTxtInfo *pInfo, int sx, int sy, BYTE a);

	int		m_XMin;			// 左端の座標
	int		m_XMax;			// 右端の座標
	int		m_XMid256;		// 中央の座標
	int		m_Y;			// 表示開始Ｙ座標

	const int		*m_pMx;		// 自機のＸ座標
	const int		*m_pMy;		// 自機のＹ座標

	#define LINE		3

	// 表示する情報の格納先 //
	CFixedRingBuffer<CStateTxtInfo, LINE>	m_DispInfo;

	// 左端・右端ウィンドウのα値 //
	BYTE	m_LeftAlpha;		// 左窓
	BYTE	m_RightAlpha;		// 右窓
};



#endif
