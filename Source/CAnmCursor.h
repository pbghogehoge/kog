/*
 *   CAnmCursor.h   : アニメーションカーソル
 *
 */

#ifndef CANMCURSOR_INCLUDED
#define CANMCURSOR_INCLUDED "アニメーションカーソル : Version 0.02 : Update 2001/06/27"

/*  [更新履歴]
 *    Version 0.02 : 2001/06/27 : 押し続け対応(PBGMS_DRAG??)
 *    Version 0.01 : 2001/05/24 : 製作開始
 */



#include "PbgMain.h"



/***** [ 定数 ] *****/
#define PBGMS_LEFTDOWN		0x01	// 左ボタンが押された
#define PBGMS_LEFTUP		0x02	// 左ボタンが離された
#define PBGMS_RIGHTDOWN		0x04	// 右ボタンが押された
#define PBGMS_RIGHTUP		0x08	// 右ボタンが離された
#define PBGMS_LEFTDRAG		0x10	// 左ボタン押し続け
#define PBGMS_RIGHTDRAG		0x20	// 右ボタン押し続け



/***** [クラス定義] *****/

class CAnmCursor {
public:
	// パターンのセット //
	BOOL Set(Pbg::CGrpSurface	**ppTarget	// 対象となる Surface
		   , RECT				*pPtn		// パターン矩形
		   , BYTE				n			// アニメーションパターン
		   , char				speed		// アニメーションスピード
		   , int				hx = 0		// ホットスポットのＸ座標
		   , int				hy = 0);	// ホットスポットのＹ座標

	void GetPosition(POINT *pCurrent);	// 現在の座標を取得する
	void SetPosition(int x, int y);		// マウスを指定座標まで誘導する
	BYTE GetButtonState(void);			// ボタンの状態を取得する
	void Update(void);					// 更新する
	void Draw(void);					// 描画する
	BOOL IsAutoMove(void);				// 自動移動中なら真

	CAnmCursor();	// コンストラクタ
	~CAnmCursor();	// デストラクタ


private:
	BOOL ResizeBuffer(BYTE n);	// バッファサイズの更新


	// マウスポインタの画像格納先(ポインタ変数への参照) //
	Pbg::CGrpSurface **m_ppTarget;

	POINT		m_CurrentPos;		// 現在のマウスの座標
	RECT		*m_aSrcRect;		// 矩形パターン格納先
	BOOL		m_bAutoMove;		// 現在、自動移動中なら真

	int			m_HotSpotX;			// マウスのホットスポット(x)
	int			m_HotSpotY;			// マウスのホットスポット(y)
	int			m_TargetX;			// 自動移動先のＸ座標
	int			m_TargetY;			// 自動移動先のＹ座標
	int			m_CurrentX256;		// 自動移動中のＸ座標(x256)
	int			m_CurrentY256;		// 自動移動中のＹ座標(x256)

	BYTE		m_Count;		// アニメーション用カウンタ
	char		m_Speed;		// アニメーションスピード
	BYTE		m_NumPtn;		// アニメーションパターン数
	BYTE		m_AnmPtn;		// 現在、何番目のパターンか
	BYTE		m_Allocated;	// 矩形パターン配列の現在のサイズ

	BYTE		m_ButtonState;		// ボタンの状態
	BOOL		m_bLButtonPushed;	// 左が押されていたら真
	BOOL		m_bRButtonPushed;	// 右が押されていたら真
};



#endif
