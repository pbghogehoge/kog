/*
 *   CSelectWindow.h   : 選択窓
 *
 */

#ifndef CSELECTWINDOW_INCLUDED
#define CSELECTWINDOW_INCLUDED "選択窓       : Version 0.01 : Update 2001/06/11"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/11 : 製作開始
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define PBGWND_STRMAX		256		// 文字列の最大長



/***** [ 型宣言 ] *****/

// 相互参照用... //
class CSelectWindow;
class CGameInfo;


// コールバック関数の戻り値 //
typedef enum tagPBGWND_RET {
	RET_DEFAULT   = 0x00,		// デフォルトの動作を要求する
	RET_OK        = 0x01,		// 正常終了した
	RET_CLOSE     = 0x02,		// このウィンドウを閉じよ
	RET_UPDATE    = 0x03,		// 更新が必要である
	RET_ACTIVATE  = 0x04,		// アクティブにせよ
	RET_NONUPDATE = 0x05,		// 更新せず、戻れ
} PBGWND_RET;


// アイテム処理コールバック関数                  //
// Args : ItemID  : どの項目に対するメッセージか //
//        KeyCode : 仮想キーコード KEY_XXX       //
// 参考 : マウスの入力がフックされます           //
typedef PBGWND_RET (*PBGWND_CALLBACK)(CGameInfo *pSys, BYTE ItemID, WORD KeyCode);


// ウィンドウの状態定数 //
typedef enum tagPBGWND_STATE {
	PBGWND_DISABLE = 0x00,	// 無効になっている(子ウィンドウが開いている)
	PBGWND_ACTIVE  = 0x01,	// アクティブになっている
	PBGWND_CLOSED  = 0xff,	// 閉じている
} PBGWND_STATE;


// 項目の属性定数 //
typedef enum tagPBG_ITEMFLAG {
	PBGITM_DEFAULT = 0x00,	// 通常の項目
	PBGITM_PARENT  = 0x01,	// 親項目
	PBGITM_RANGE   = 0x02,	// 範囲選択付き項目
	PBGITM_EXIT    = 0x03,	// 一つ前の項目に移動
} PBG_ITEMFLAG;



/***** [クラス定義] *****/

// 範囲選択情報構造体 //
typedef struct tagPBG_RANGEINFO {
	int		m_Min;			// 最小値
	int		m_Max;			// 最大値
	int		m_Current;		// 現在の値

	void SeekR(int n){
		n += m_Current;
		if(n < m_Min) n = m_Min;
		if(n > m_Max) n = m_Max;

		m_Current = n;
	};
} PBG_RANGEINFO;


// 項目設定用構造体 //
typedef struct tagPBG_ITEMINFO {
	char			m_ItemName[PBGWND_STRMAX];		// 項目名
	char			m_HelpOption[PBGWND_STRMAX];	// ヘルプ追加文字列
	DWORD			m_HelpID;						// ヘルプＩＤ
	PBG_ITEMFLAG	m_ItemFlag;						// フラグ(PBGITM_XXX)

	CSelectWindow	*m_pChild;		// PBGITM_PARENT 時の子ウィンドウ

	PBG_RANGEINFO	m_RangeInfo;	// 範囲情報
} PBG_ITEMINFO;



/***** [クラス定義] *****/

// ウィンドウ管理クラス //
// 参考 : 座標は親ウィンドウからの相対値になります                    //
//        なお、トップウィンドウならば(0, 0) からの相対指定となります //
class CSelectWindow {
public:
	void Move(void);	// １フレームごとの更新
	void Draw(void);	// このウィンドウとその子ウィンドウを描画する

	// 親ウィンドウからの相対指定で移動する //
	void SetWindowPos(int sx, int sy);

	// 通知用関数 //
	BOOL OnMouseMove(POINT *pMouse, BYTE MouseState);	// マウスが動いたとき
	BOOL OnKeyDown(WORD KeyCode);		// 何かキーが押されて、離されたとき
	void OnActivate(void);				// そのままの設定でアクティブにする
	void OnDisable(void);				// ウィンドウの選択を無効化する
	void OnClose(void);					// このウィンドウを閉じる

	// 項目をセットする //
	BOOL SetItem(CSelectWindow		*pParent	// 親ウィンドウ
			   , PBGWND_CALLBACK	pCallBack	// コールバック関数
			   , PBG_ITEMINFO		*pItemInfo	// 項目情報
			   , BYTE				NumItems	// 項目数
			   , int				Width		// 横幅
			   , DWORD				ItemID);	// ヘルプ項目ＩＤ

	// 項目フォントを更新する                                  //
	// PBG_ITEMINFO::m_ItemName を変更したら呼び出す必要がある //
	void UpdateItemFont(DWORD ItemID);

	// 項目を 有効(TRUE) or 無効(FALSE) にする //
	void EnableItem(DWORD ItemID, BOOL bSetEnable);


	CSelectWindow(CGameInfo *pSys);	// コンストラクタ
	~CSelectWindow();				// デストラクタ


private:
	BOOL OnLButtonCheck(POINT *pMouse);	// マウスの左ボタンが離されたとき
	BOOL OnLButtonDrag(POINT *pMouse);	// 左ボタンドラッグ
	BOOL OnRButtonDown(void);			// マウスの右ボタンが離されたとき

	// ウィンドウの横幅を決める (CreateSurface を含みます) //
	BOOL CreateSurface(int Width, int NumItems);

	// ヘルプ文字列を更新する //
	void UpdateHelpStr(void);

	void DrawClient(void);		// クライアント領域の描画
	void DrawContents(void);	// 内容物の描画

	int GetCurrentX(void);	// 現在のＸ座標を取得する
	int GetCurrentY(void);	// 現在のＹ座標を取得する

	int		m_sx;		// ウィンドウ表示時の左上のＸ座標
	int		m_sy;		// ウィンドウ表示時の右上のＹ座標
	int		m_Width;	// ウィンドウの横幅

	Pbg::CFont			*m_aCFont;		// フォント管理クラス
	Pbg::CGrpSurface	*m_pSurface;	// 透過転送用Surface

	// 項目情報構造体へのポインタ                                      //
	// 注:配列の初期化はこのクラスで行わない。従って、ポイントするだけ //
	PBG_ITEMINFO	*m_pInfo;

	PBGWND_CALLBACK	m_pCallBack;	// コールバック関数へのポインタ
	PBGWND_STATE	m_WindowState;	// このウィンドウはアクティブか
	CSelectWindow	*m_pParent;		// 親ウィンドウへのポインタ
	CGameInfo		*m_pSys;		// システム情報へのポインタ

	DWORD	m_ItemID;			// ヘルプ項目ＩＤ

	BOOL	*m_pIsEnableItem;	// 項目が選択できれば真
	BOOL	m_bNeedUpdate;		// DIB->VRAM 転送が必要なら真
	BOOL	m_bMouseCaptured;	// マウスのキャプチャー中なら真
	BYTE	m_ActiveItem;		// 現在アクティブな項目
	BYTE	m_NumItems;			// 選択できる項目数
	BYTE	m_CursorAnime;		// カーソルアニメ用カウンタ
};



#endif
