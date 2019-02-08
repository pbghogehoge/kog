/*
 *   RndCtrl.h   : 乱数管理
 *
 */

#ifndef RNDCTRL_INCLUDED
#define RNDCTRL_INCLUDED "乱数管理     : Version 0.01 : Update 2002/02/06"

/*  [更新履歴]
 *    Version 0.01 : 2002/02/06 : 制作開始
 */



#include "PbgMath.h"



/***** [関数プロトタイプ] *****/
FVOID  SetRndClass(Pbg::CRnd *pRnd);

#ifdef PBG_DEBUG
	#define RndEx()		RndExDebug(__FILE__, __LINE__)
	FDWORD RndExDebug(char *pFileName, int line);
#else
	FDWORD RndEx();
#endif


#endif
