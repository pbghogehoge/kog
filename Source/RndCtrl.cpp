/*
 *   RndCtrl.cpp   : 乱数管理
 *
 */

#include "PbgMath.h"


static Pbg::CRnd	*g_pRnd = NULL;



FVOID SetRndClass(Pbg::CRnd *pRnd)
{
	g_pRnd = pRnd;
}


FDWORD RndEx(void)
{
	return g_pRnd->Get();
}


#ifdef PBG_DEBUG
#include "PbgError.h"
FDWORD RndExDebug(char *pFileName, int line)
{
	if(NULL == g_pRnd){
		char	buf[256];
		wsprintf(buf, "乱数発生クラスが無効です : %s in %d", pFileName, line);
		PbgError(buf);
		return 0;
	}
	else{
		return RndEx();
	}
}
#endif
