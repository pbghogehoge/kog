/*
 *   PbgTimer.cpp   : タイマー管理
 *
 */

#include "PbgTimer.h"



namespace Pbg {



/***** [スタティックメンバ] *****/
DWORD CPbgTimer::m_IntervalMin = 0;		// インターバルの最小値
DWORD CPbgTimer::m_RefCount    = 0;		// 参照カウンタ



// コンストラクタ //
CPbgTimer::CPbgTimer()
{
	TIMECAPS		caps;

	m_hTimer   = NULL;		// タイマー識別子
	m_bStopped = TRUE;		// 現在停止中か

	if(0 == m_RefCount){
		timeGetDevCaps(&caps, sizeof(caps));	// 性能取得
		timeBeginPeriod(caps.wPeriodMin);		// セット

		// 最小値が確定する //
		m_IntervalMin = caps.wPeriodMin;
	}

	// 参照カウンタをインクリメント //
	m_RefCount++;
}


// デストラクタ //
CPbgTimer::~CPbgTimer()
{
	// 強制的に止める //
	Stop();

	// 参照カウンタをデクリメント //
	m_RefCount--;

	// 参照しているオブジェクトが存在しなければ //
	if(m_RefCount <= 0){
		timeEndPeriod(m_IntervalMin);	// 終了する
	}
}


// インターバルタイマーを開始する //
BOOL CPbgTimer::Start(LPTIMECALLBACK	lpFunction	// コールバック関数
					, DWORD				Interval	// 呼び出し間隔
					, DWORD				dwUser)		// ユーザ定義
{
	// 使用できる最小の間隔よりも小さい要求の場合 //
	if(Interval < m_IntervalMin){
		Interval = m_IntervalMin;
	}

	m_Interval = m_IntervalMin;//Interval;	// 呼び出し間隔
	m_bStopped = FALSE;		// 動作中にする

	m_Delay = max(m_Interval, Interval);//(m_Interval * 9) / 10;
//	if(0 == m_Delay) m_Delay++;

	// タイマーをセットする //
	m_hTimer = timeSetEvent(m_Delay				// 遅延
						  , m_Interval			// 呼び出し間隔
						  , lpFunction			// コールバック関数
						  , dwUser				// ユーザ定義
						  , TIME_PERIODIC);		// 呼び出し間隔

	// 失敗した場合 //
	if(NULL == m_hTimer){
		m_bStopped = TRUE;	// 無効にして
		return FALSE;		// 終了
	}

	return TRUE;
}


// インターバルタイマーを停止する //
void CPbgTimer::Stop(void)
{
	// すでに停止中の場合 //
	if(m_bStopped) return;

	timeKillEvent(m_hTimer);	// タイマーを停止して
	m_hTimer = NULL;			// 識別子も無効化する
}



} // namespace Pbg
