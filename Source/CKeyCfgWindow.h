/*
 *   CKeyCfgWindow.h   : キーボードコンフィグ窓
 *
 */

#ifndef CKEYCFGWINDOW_INCLUDED
#define CKEYCFGWINDOW_INCLUDED "キーボードコンフィグ窓 : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "PbgType.h"
#include "CInputCfgWindow.h"
#include "CKeyInput.h"



/***** [クラス定義] *****/

class CKeyCfgWindow : public CInputCfgWindow {
public:
	FVOID Move(void);				// 動作させる
	FVOID Draw(void);				// 描画する
	FVOID OnActivate(BOOL bActive);	// アクティブにする

	CKeyCfgWindow(CKeyInput *pDevice);	// コンストラクタ
	~CKeyCfgWindow();					// デストラクタ


protected:
	virtual FINT  GetWindowWidth(void);		// ウィンドウの幅
	virtual FINT  GetWindowHeight(void);	// ウィンドウの高さ
	virtual FINT  GetNumButtons(void);		// 設定可能なボタンの数
	virtual FVOID ProceedNextItem(void);	// 次の項目に進む


private:
	// 左ボタンが下がったときの処理 //
	FVOID OnLButtonDown(int mx, int my);

	CKeyInput	*m_pKeyInput;
};



#endif
