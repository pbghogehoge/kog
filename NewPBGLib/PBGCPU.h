/*
 *   PbgCPU.h   : ＣＰＵ情報取得
 *
 */

#ifndef PBGCPU_INCLUDED
#define PBGCPU_INCLUDED "ＣＰＵ情報取得 : Version 0.01 : Update 2001/03/04"

/*  [更新履歴]
 *    Version 0.01 : 2001/03/04 : CGraphic から移動。
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/
class CCPUCheck {
public:
	// ＭＭＸが使用できるかを調べる //
	static IBOOL IsEnableMMX(void){ return m_bEnableMMX; };

	CCPUCheck();		// コンストラクタ
	~CCPUCheck();		// デストラクタ


private:
	static BOOL		m_bEnableMMX;		// MMX が使用できれば真
};



} // namespace Pbg



#endif
