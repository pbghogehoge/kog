/*
 *   CRnd.h   : 乱数発生
 *
 */

#ifndef CRND_INCLUDED
#define CRND_INCLUDED "乱数発生     : Version 0.01 : Update 2001/09/21"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/21 : 制作開始
 */



#include "PbgType.h"



namespace Pbg {



/***** [ 定数 ] *****/
#define MTVECTOR_SIZE		624



/***** [クラス定義] *****/

// 乱数発生用クラス //
class CRnd {
public:
	FDWORD Get(void);				// 乱数を取得
	FVOID  SetSeed(DWORD Seed);		// 種をセットする
	FVOID  Randomize(void);			// タイマによる種のセット

	inline operator ()(void){
		return Get();
	};

	CRnd();		// コンストラクタ
	~CRnd();	// デストラクタ

private:
	DWORD		m_MTVector[MTVECTOR_SIZE];
	int			m_MTCounter;
};



} // namespace Pbg



#endif
