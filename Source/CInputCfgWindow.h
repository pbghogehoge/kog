/*
 *   CInputCfgWindow.h   : 入力コンフィグ窓
 *
 */

#ifndef CINPUTCFGWINDOW_INCLUDED
#define CINPUTCFGWINDOW_INCLUDED "入力コンフィグ窓 : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "Gian2001.h"
#include "CInputDevBase.h"



/***** [クラス定義] *****/

// 入力コンフィグ窓 //
class CInputCfgWindow {
public:
	virtual FVOID Move(void) = 0;				// 動作させる
	virtual FVOID Draw(void) = 0;				// 描画する
	virtual FVOID OnActivate(BOOL bActive) = 0;	// アクティブにする

	FBOOL IsActive(void){
		if(m_ActiveItem >= 0) return TRUE;
		else                  return FALSE;
	};

	// ウィンドウ左上の座標を変更する //
	FVOID SetWindowPos(int ox, int oy);

	// コンフィグ窓用の Surface をセットする //
	static FVOID SetSurface(Pbg::CGrpSurface **ppSurface);


protected:
	virtual FINT  GetWindowWidth(void)  = 0;	// ウィンドウの幅
	virtual FINT  GetWindowHeight(void) = 0;	// ウィンドウの高さ
	virtual FINT  GetNumButtons(void)   = 0;	// 設定可能なボタンの数
	virtual FVOID ProceedNextItem(void) = 0;	// 次の項目に進む

	// ウィンドウ左上の座標を取得する //
	FINT  GetWindowOx(void){ return m_WindowOx; };	// Ｘ座標
	FINT  GetWindowOy(void){ return m_WindowOy; };	// Ｙ座標

	// アクティブなアイテムに対する操作 //
	FINT  GetActiveItem(void){ return m_ActiveItem; };	// 取得
	FVOID SetActiveItem(int ItemID){					// セット
		m_ActiveItem = ItemID;
	};

	FVOID Draw8x8Fonts(int dx, int dy, char *pStr);	// 8x8フォントで文字列描画
	FVOID DrawActiveItemRect(int dx, int dy);		// アクティブな項目を描画
	FVOID DrawBaseWindow(void);						// 窓の基本部分の描画
	FVOID MoveBaseWindow(void);						// 窓の基本部分の動作

	// 描画用 Surface を取得する //
	Pbg::CGrpSurface *GetSurface(void){
		if(m_ppSurface) return (*m_ppSurface);
		else            return NULL;
	};

	CInputCfgWindow(CInputDevBase *pTarget);	// コンストラクタ
	~CInputCfgWindow();							// デストラクタ


private:
	FVOID DrawClient(void);					// 窓部分の描画
	FVOID DrawContents(void);				// 内容物の描画
	FVOID DrawActiveButton(int dx, int dy);	// アクティブなボタンを描画

	int				m_WindowOx;		// ウィンドウ左上のＸ座標
	int				m_WindowOy;		// ウィンドウ左上のＹ座標

	CInputDevBase	*m_pTargetDev;	// 対象となるデバイス
	int				m_ActiveItem;	// 現在アクティブな項目

	BYTE			m_CursorAnime;	// アクティブ項目の点滅用

	// 描画用 Surface //
	static Pbg::CGrpSurface		**m_ppSurface;
};



#endif
