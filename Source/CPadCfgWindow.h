/*
 *   CPadCfgWindow.h   : パッドコンフィグ窓
 *
 */

#ifndef CPADCFGWINDOW_INCLUDED
#define CPADCFGWINDOW_INCLUDED "パッドコンフィグ窓 : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "PbgType.h"
#include "CInputCfgWindow.h"
#include "CPadInput.h"



/***** [クラス定義] *****/

class CPadCfgWindow : public CInputCfgWindow {
public:
	FVOID Move(void);				// 動作させる
	FVOID Draw(void);				// 描画する
	FVOID OnActivate(BOOL bActive);	// アクティブにする

	CPadCfgWindow(CPadInput *pDevice);	// コンストラクタ
	~CPadCfgWindow();					// デストラクタ


protected:
	virtual FINT  GetWindowWidth(void);		// ウィンドウの幅
	virtual FINT  GetWindowHeight(void);	// ウィンドウの高さ
	virtual FINT  GetNumButtons(void);		// 設定可能なボタンの数
	virtual FVOID ProceedNextItem(void);	// 次の項目に進む


private:
	FVOID OnLButtonDown(void);		// 左ボタンが押し下げられた
	FVOID OnDeviceChanged(void);	// デバイスが変更された

	CPadInput	*m_pPadInput;		// パッド入力管理クラス

	Pbg::CFont			m_CFont;		// フォント管理クラス
	Pbg::CGrpSurface	*m_pSurface;	// 透過転送用Surface
};



#endif
