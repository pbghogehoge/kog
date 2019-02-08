/*
 *   PbgTimer.h   : タイマー管理
 *
 */

#ifndef PBGTIMER_INCLUDED
#define PBGTIMER_INCLUDED "タイマー管理 : Version 0.01 : Update 2001/05/30"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/30 : 製作開始
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/

// タイマー管理クラス //
class CPbgTimer {
public:
	// インターバルタイマーを開始する //
	BOOL Start(LPTIMECALLBACK	lpFunction	// コールバック関数
			 , DWORD			Interval	// 呼び出し間隔
			 , DWORD			dwUser);	// ユーザ定義

	// インターバルタイマーを停止する //
	void Stop(void);

	// 呼び出し間隔の取得 //
	DWORD GetInterval(void){
		return m_Delay;
	};

	CPbgTimer();		// コンストラクタ
	~CPbgTimer();		// デストラクタ


private:
	UINT		m_hTimer;		// マルチメディアタイマー
	BOOL		m_bStopped;		// 停止中なら真
	DWORD		m_Interval;		// 現在の呼び出し間隔
	DWORD		m_Delay;		// 遅延

	static DWORD	m_IntervalMin;	// インターバルの最小値
	static DWORD	m_RefCount;		// 参照カウンタ
};



} // namespace Pbg



#endif
