/*
 *   CCriticalSection.h   : クリティカルセクション制御
 *
 */

#ifndef CCRITICALSECTION_INCLUDED
#define CCRITICALSECTION_INCLUDED "クリティカルセクション制御 : Version 0.02 : Update 2001/09/10"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/10 : 微調整
 *    Version 0.01 : 2001/05/30 : 製作開始
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/

// クリティカルセクション管理クラス //
class CCriticalSection {
public:
	void EnterCS(void);		// クリティカルセクションに入る
	void LeaveCS(void);		// クリティカルセクションから抜ける

	CCriticalSection();		// コンストラクタ
	~CCriticalSection();	// デストラクタ


private:
	CRITICAL_SECTION	m_CriticalSection;
};



} // namespace Pbg



#endif
