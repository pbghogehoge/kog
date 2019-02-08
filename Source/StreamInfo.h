/*
 *   StreamInfo.h   : ストリーム情報
 *
 */

#ifndef STREAMINFO_INCLUDED
#define STREAMINFO_INCLUDED "ストリーム情報 : Version 0.01 : Update 2002/02/07"

/*  [更新履歴]
 *    Version 0.01 : 2002/02/07 : 制作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
#define KIOH_VERSION		"Version 1.009"



/***** [クラス定義] *****/

// ストリーム情報 //
typedef struct tagStreamInfoHeader {
	DWORD	m_RndSeed;			// 乱数の種
	char	m_VersionStr[32];	// バージョン文字列
} StreamInfoHeader;



#endif
