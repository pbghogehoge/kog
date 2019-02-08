/*
 *   DPError.h   : DPlay エラー
 *
 */

#ifndef DPERROR_INCLUDED
#define DPERROR_INCLUDED "DPlay エラー : Version 0.01 : Update 2001/09/17"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/17 : 製作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
#ifdef PBG_DEBUG
	FVOID DPlayError(HRESULT hr);
#else
	#define DPlayError(hr)
#endif



#endif
