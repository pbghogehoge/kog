/*
 *   KOG_GUID.h   : 稀翁玉用ＧＵＩＤ
 *
 */

#ifndef CKOG_GUID_INCLUDED
#define CKOG_GUID_INCLUDED "稀翁玉用ＧＵＩＤ : Version 0.01 : Update 2002/02/22"

#include "PbgType.h"



/***** [ 定数 ] *****/
#define KIOHDATA_KEY		0x00	// キーボードメッセージ
#define KIOHDATA_SYNC		0x01	// 同期メッセージ

extern const GUID guidKIOH;



/***** [クラス定義] *****/
typedef struct tagKIOH_DATA {
	WORD	m_DataType;		// 拡張用・この値で構造体の判別を行う
	WORD	m_KeyData;		// キーコード格納先
} KIOH_DATA;




#endif
