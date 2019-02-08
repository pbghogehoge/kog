/*
 *   CAnmCursor.cpp   : アニメーションカーソル
 *
 */

#include "CAnmCursor.h"
#include "Gian2001.h"



// コンストラクタ //
CAnmCursor::CAnmCursor()
{
	m_Count      = 0;	// アニメーションカウンタ
	m_NumPtn     = 0;	// アニメのパターン数
	m_AnmPtn     = 0;	// 現在のアニメーションパターン
	m_Speed      = 0;	// アニメーションスピード
	m_Allocated  = 0;	// 現在の矩形バッファサイズ

	m_HotSpotX    = 0;	// マウスのホットスポット(x)
	m_HotSpotY    = 0;	// マウスのホットスポット(y)
	m_TargetX     = 0;	// 自動移動先のＸ座標
	m_TargetY     = 0;	// 自動移動先のＹ座標
	m_CurrentX256 = 0;	// 自動移動中のＸ座標(x256)
	m_CurrentY256 = 0;	// 自動移動中のＹ座標(x256)

	m_bAutoMove = FALSE;	// 現在、自動移動中か

	m_ButtonState    = 0;		// ボタンの状態
	m_bLButtonPushed = FALSE;	// 左が押されていたら真
	m_bRButtonPushed = FALSE;	// 右が押されていたら真

	m_aSrcRect   = NULL;	// 対象となる矩形
	m_ppTarget   = NULL;	// 対象となる Surface
}


// デストラクタ //
CAnmCursor::~CAnmCursor()
{
	// 配列作成の形跡があるので、解放 //
	if(m_aSrcRect){
		DeleteArray(m_aSrcRect);
	}
}


// パターンのセット //
BOOL CAnmCursor::Set(Pbg::CGrpSurface	**ppTarget	// 対象となる Surface
				   , RECT				*pPtn		// パターン矩形
				   , BYTE				n			// アニメーションパターン
				   , char				speed		// アニメーションスピード
				   , int				hx			// ホットスポットのＸ座標
				   , int				hy)			// ホットスポットのＹ座標
{
	if(NULL == pPtn)     return FALSE;		// 矩形データ
	if(NULL == ppTarget) return FALSE;		// 対象 Surface

	m_ppTarget = NULL;

	// バッファサイズを変更する //
	if(FALSE == ResizeBuffer(n)) return FALSE;

	// 矩形データを転送 //
	memcpy(m_aSrcRect, pPtn, sizeof(RECT) * n);

	m_Count     = 0;		// アニメーションカウンタ
	m_NumPtn    = n;		// パターン数
	m_AnmPtn    = 0;		// 現在のアニメーションパターン
	m_Speed     = speed;	// アニメーションスピード
	m_bAutoMove = FALSE;	// 自動移動を無効化する

	// マウスのホットスポット //
	m_HotSpotX = hx;	// ホットスポットのＸ座標
	m_HotSpotY = hy;	// ホットスポットのＹ座標

	// Surface を関連づけ //
	m_ppTarget = ppTarget;

	return TRUE;
}


// 現在の座標を取得する //
void CAnmCursor::GetPosition(POINT *pCurrent)
{
	*pCurrent = m_CurrentPos;
}


// マウスを指定座標まで誘導する //
void CAnmCursor::SetPosition(int x, int y)
{
	m_TargetX = x;	// 目標となるＸ座標
	m_TargetY = y;	// 目標となるＹ座標

	// 現在の座標を x256 に変換して格納する //
	m_CurrentX256 = m_CurrentPos.x << 8;
	m_CurrentY256 = m_CurrentPos.y << 8;

	// 自動移動を有効にする //
	m_bAutoMove = TRUE;

	// ボタンの状態を無効化する //
	m_ButtonState = 0;
}


// ボタンの状態を取得する //
BYTE CAnmCursor::GetButtonState(void)
{
	return m_ButtonState;
}


// 更新する //
void CAnmCursor::Update(void)
{
	char	spd;
	BOOL	LButton;
	BOOL	RButton;

	// 自動移動中の場合 //
	if(m_bAutoMove){
		int dx = (m_TargetX<<8) - m_CurrentX256;
		int dy = (m_TargetY<<8) - m_CurrentY256;
		int n  = 0;

		if(abs(dx) > 256){
			if(dx > 0) m_CurrentX256 += max( 256, dx/2);
			else       m_CurrentX256 += min(-256, dx/2);

			m_CurrentPos.x = m_CurrentX256 >> 8;
		}
		else{
			m_CurrentPos.x = m_TargetX;
			n++;
		}

		if(abs(dy) > 256){
			if(dy > 0) m_CurrentY256 += max( 256, dy/2);
			else       m_CurrentY256 += min(-256, dy/2);

			m_CurrentPos.y = m_CurrentY256 >> 8;
		}
		else{
			m_CurrentPos.y = m_TargetY;
			n++;
		}

		if(n >= 2){
			m_bAutoMove = FALSE;
			SetCursorPos(m_TargetX, m_TargetY);
		}

	}
	else{
		// 普通に、カーソル座標を更新 //
		GetCursorPos(&m_CurrentPos);

		LButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		RButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);

		m_ButtonState = 0;

		// 押し続けに対応させる(2001/06/27) //
		if(LButton) m_ButtonState |= PBGMS_LEFTDRAG;
		if(RButton) m_ButtonState |= PBGMS_RIGHTDRAG;

		if(LButton && (!m_bLButtonPushed))      m_ButtonState |= PBGMS_LEFTDOWN;
		else if((!LButton) && m_bLButtonPushed) m_ButtonState |= PBGMS_LEFTUP;

		if(RButton && (!m_bRButtonPushed))      m_ButtonState |= PBGMS_RIGHTDOWN;
		else if((!RButton) && m_bRButtonPushed) m_ButtonState |= PBGMS_RIGHTUP;

		m_bLButtonPushed = LButton ? TRUE : FALSE;
		m_bRButtonPushed = RButton ? TRUE : FALSE;
	}

	// アニメーションスピードを一時格納 //
	spd = m_Speed;

	// カウンタをインクリメントする //
	m_Count += 1;

	// 正方向にアニメーションする //
	if(spd > 0){
		if(m_Count >= spd){		// 更新のタイミング
			m_AnmPtn = (m_AnmPtn + 1) % m_NumPtn;
			m_Count  = 0;
		}
	}
	// 負方向にアニメーションする //
	else if(spd < 0){
		if(m_Count >= (-spd)){	// 更新のタイミング
			// まぁ、基本かね //
			m_AnmPtn = (m_AnmPtn + m_NumPtn - 1) % m_NumPtn;
			m_Count  = 0;
		}
	}
}


// 描画する //
void CAnmCursor::Draw(void)
{
	int			x, y;
	RECT		*pSrc;

	// まだ、準備出来ていない //
	if(NULL == (*m_ppTarget))	return;		// 元画像の格納先
	if(NULL == m_aSrcRect)		return;		// パターン矩形

	pSrc = m_aSrcRect + m_AnmPtn;		// 描画対象矩形

	// ホットスポットが中心になるように補正する //
	x = m_CurrentPos.x - m_HotSpotX;	// Ｙ座標
	y = m_CurrentPos.y - m_HotSpotY;	// Ｘ座標

	// 後は、おまかせ //
	g_pGrp->BltC(pSrc, x, y, (*m_ppTarget));
}


// 自動移動中なら真 //
BOOL CAnmCursor::IsAutoMove(void)
{
	return m_bAutoMove;
}


// バッファサイズの更新 //
BOOL CAnmCursor::ResizeBuffer(BYTE n)
{
	// 要求サイズがおかしい //
	if(0 == n) return FALSE;

	// 必要なサイズを満たしている //
	if(m_Allocated >= n) return TRUE;

	// サイズ更新のために、メモリを解放 //
	if(m_aSrcRect) DeleteArray(m_aSrcRect);

	// 再確保ぉ //
	m_aSrcRect = NewEx(RECT[n]);
	if(NULL == m_aSrcRect) return FALSE;

	// 確保できたので、サイズ更新 //
	m_Allocated = n;

	return TRUE;
}
