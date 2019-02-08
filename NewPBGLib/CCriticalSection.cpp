/*
 *   CCriticalSection.cpp   : クリティカルセクション制御
 *
 */

#include "CCriticalSection.h"



namespace Pbg {



// コンストラクタ //
CCriticalSection::CCriticalSection()
{
	// 失敗すると例外を投げちゃうらしい //
	InitializeCriticalSection(&m_CriticalSection);
}


// デストラクタ //
CCriticalSection::~CCriticalSection()
{
	// クリティカルセクションオブジェクトを削除する //
	DeleteCriticalSection(&m_CriticalSection);
}


// クリティカルセクションに入る //
void CCriticalSection::EnterCS(void)
{
//	if(bEnableBlock){
		// ここでブロッキングが起こる可能性があるので注意！！ //
		EnterCriticalSection(&m_CriticalSection);

		// 抜けた時点で、成功している //
		//return TRUE;
//	}
}


// クリティカルセクションから抜ける
void CCriticalSection::LeaveCS(void)
{
	LeaveCriticalSection(&m_CriticalSection);
}



} // namespace Pbg
