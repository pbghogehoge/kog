/*
 *   CFpsCtrl.cpp   : FPS計測・設定
 *
 */

#include "CFpsCtrl.h"



// コンストラクタ //
CFPSCtrl::CFPSCtrl()
{
	m_FPS        = 60;			// ＦＰＳ設定値
	m_WaitTime   = 1000/60;		// 待ち時間
	m_LastCalled = 0;			// 前回呼び出し時の時刻

	m_MoveCount  = 0;		// 移動用カウンタ
	m_DrawCount  = 0;		// 描画用カウンタ

	m_bNeedMove = TRUE;		// 移動部の更新が必要ならば真
	m_bNeedDraw = TRUE;		// 描画部の更新が必要ならば真

	m_Count          = 0;		// リングバッファ挿入先
	m_bRealFPSEnable = FALSE;	// ＦＰＳ計測値が有効なら真

	ZEROMEM(m_PassedTime);		// ＦＰＳ計測用
}


// デストラクタ //
CFPSCtrl::~CFPSCtrl()
{
}


// ＦＰＳを設定する //
void CFPSCtrl::SetFPS(DWORD fps)
{
	switch(fps){
		case PBGFPS_AUTO:		// 自動ウェイトモード
			m_FPS      = PBGFPS_AUTO;
			m_WaitTime = 1000 / 60;
		break;

		case PBGFPS_WITHOUT:	// ウェイト無しモード
			m_FPS      = PBGFPS_WITHOUT;
			m_WaitTime = 0;
		break;

		default:				// 通常動作
			m_FPS      = min(60, fps);
			m_WaitTime = 1000 / m_FPS;
		break;
	}

	Reset();
}


// 設定されたＦＰＳ値を取得する //
DWORD CFPSCtrl::GetFPS(void)
{
	return m_FPS;
}


// 実際のＦＰＳ値を取得する //
DWORD CFPSCtrl::GetRealFPS(void)
{
	DWORD	c1, c2, delta;

	// 取得できていなければ、０を返す //
	if(FALSE == m_bRealFPSEnable) return 0;

	c2 = m_Count;								// 数フレーム前の時刻
	c1 = (c2 + FPS_SAMPLES - 1) % FPS_SAMPLES;	// ちょい前の時刻

	// 経過時間を求めましょ //
	delta = m_PassedTime[c1] - m_PassedTime[c2];
	if(delta <= 0) return 0;	// ０除算等の防止

	return (1000 * FPS_SAMPLES) / delta;
}


// 内部状態を更新する //
BOOL CFPSCtrl::Update(void)
{
	DWORD		t, delta;
	DWORD		fps;

	t            = timeGetTime();		// 現在の時刻を取得
	delta        = t - m_LastCalled;	// 前回からの経過時間
	m_LastCalled = t;					// 今回の時刻を保存
	fps          = m_FPS;

	m_MoveCount += delta;	// 動作用カウンタを増やす
	m_DrawCount += delta;	// 描画用カウンタを増やす

	// ウェイト無しモードの動作状態推移 //
	if(PBGFPS_WITHOUT == fps){
		m_bNeedMove = TRUE;
	}
	// それ以外は、(1000/60)s ごとにフラグを立てる //
	else if(m_MoveCount >= (1000 / 60)){
		m_MoveCount %= (1000 / 60);		// カウンタ戻して
		m_bNeedMove  = TRUE;			// 動作要求
	}

	switch(fps){
	case PBGFPS_WITHOUT:	// ウェイト無しモード
		m_bNeedDraw = TRUE;		// 無条件！
	break;

	case PBGFPS_AUTO:		// 自動ウェイトモード

	default:				// 通常動作
		if(m_DrawCount >= m_WaitTime){
			m_DrawCount %= m_WaitTime;	// カウンタ戻して
			m_bNeedDraw  = TRUE;		// 描画要求
		}
	break;
	}

	if(m_bNeedDraw){	// ＦＰＳ計測用リングバッファに挿入
		m_PassedTime[m_Count] = t;
		m_Count = (m_Count + 1) % FPS_SAMPLES;

		if(0 == m_Count){
			m_bRealFPSEnable = TRUE;
		}
	}

	// どちらかのフラグが立っていれば、真となる //
	return (m_bNeedMove || m_bNeedDraw);
}


// 移動部を更新する必要があるか //
BOOL CFPSCtrl::IsNeedMove(void)
{
	if(m_bNeedMove){
		m_bNeedMove = FALSE;
		return TRUE;
	}

	return FALSE;
}


// 描画部を更新する必要があるか //
BOOL CFPSCtrl::IsNeedDraw(void)
{
	if(m_bNeedDraw){
		m_bNeedDraw = FALSE;
		return TRUE;
	}

	return FALSE;
}


// 各種リセット //
void CFPSCtrl::Reset(void)
{
	m_LastCalled = timeGetTime();	// 前回呼び出し時の時刻

	m_MoveCount  = 0;			// 移動用カウンタ
	m_DrawCount  = 0;			// 描画用カウンタ

	m_bNeedMove = TRUE;			// 移動部の更新が必要ならば真
	m_bNeedDraw = TRUE;			// 描画部の更新が必要ならば真

	m_Count          = 0;		// リングバッファ挿入先
	m_bRealFPSEnable = FALSE;	// ＦＰＳ計測値が有効なら真

	ZEROMEM(m_PassedTime);		// ＦＰＳ計測用
}
