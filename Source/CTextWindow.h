/*
 *   CTextWindow.h   : テキスト用ウィンドウ
 *
 */

#ifndef CTEXTWINDOW_INCLUDED
#define CTEXTWINDOW_INCLUDED "テキスト用ウィンドウ : Version 0.03 : Update 2001/06/24"

/*  [更新履歴]
 *    Version 0.03 : 2001/06/24 : 入力系ヘルプを ICfgWindow に移動
 *    Version 0.02 : 2001/05/26 : 遅延解放を追加
 *    Version 0.01 : 2001/05/25 : 制作開始
 */



#include "PbgMain.h"



/***** [ 定数 ] *****/
#define TWIN_MAINMENU		0x01	// メインメニュー用のヘルプ
#define TWIN_EXITMENU		0x02	// ゲーム中のメニュー用ヘルプ

#define TINPUT_CFG_UP		0x00	// 上方向
#define TINPUT_CFG_DOWN		0x01	// 下方向
#define TINPUT_CFG_LEFT		0x02	// 左方向
#define TINPUT_CFG_RIGHT	0x03	// 右方向
#define TINPUT_CFG_SHOT		0x04	// ショット
#define TINPUT_CFG_BOMB		0x05	// ボム
#define TINPUT_CFG_SHIFT	0x06	// シフト
#define TINPUT_CFG_KEY		0x07	// キー入力待ち
#define TINPUT_CFG_MOUSE	0x08	// マウス選択待ち



/***** [クラス定義] *****/

// ヘルプ文字列テーブル //
typedef struct tagHelpStrTable {
	char	Data[150];
} HelpStrTable;


// テキスト表示専用ウィンドウ管理 //
class CTextWindow {
public:
	FVOID Draw(void);	// 描画する

	FVOID SetWindowPosA(int sx, int sy);	// 絶対指定で座標を変更
	FVOID SetWindowPosR(int dx, int dy);	// 相対指定で座標を変更
	FBOOL Resize(int width, int height);	// ウィンドウのサイズを変更
	FBOOL SetFontSize(int size);			// フォントのサイズを変更

	// テキストを割り当てる //
	FVOID SetText(char *pString, BOOL bInsert);

	// ヘルプ文字列を割り当てる //
	FVOID SetHelpText(DWORD ItemID, DWORD HelpID, BOOL bInsert);

	// %s 文字列追加タイプでヘルプ文字列の割り当て //
	FVOID SetHelpTextEx(DWORD ItemID, DWORD HelpID, char *pStr);


	CTextWindow();		// コンストラクタ
	~CTextWindow();		// デストラクタ


private:
	FVOID InitializeStaticData(void);	// 静的データの確保
	FVOID CleanupStaticData(void);		// 静的データの解放

	FVOID DrawClient(void);					// クライアント領域を描画
	FVOID DrawContents(void);				// 文字列等の描画(2D 部)

	Pbg::CFont			m_CFont;		// フォント管理クラス
	Pbg::CGrpSurface	*m_pSurface;	// 透過転送用Surface

	int					m_sx;			// ウィンドウ左上のＸ座標
	int					m_sy;			// ウィンドウ左上のＹ座標
	int					m_Width;		// ウィンドウの幅
	int					m_Height;		// ウィンドウの高さ
	int					m_FontSize;		// フォントのサイズ

	static int			m_RefCount;			// 参照数
	static BOOL			m_bInitialized;		// 初期化されているか

	static HelpStrTable *m_pMainMenuHelp;	// TWIN_MAINMENU 系のヘルプ
	static DWORD		m_NumMainMenuHelp;	// TWIN_MAINMENU の項目数

	static HelpStrTable *m_pExitMenuHelp;	// TWIN_EXITMENU 系のヘルプ
	static DWORD		m_NumExitMenuHelp;	// TWIN_EXITMENU の項目数
};



#endif
