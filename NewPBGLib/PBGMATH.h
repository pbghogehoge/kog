/*
 *   PbgMath.h   : 数学関数
 *
 */

#ifndef PBGMATH_INCLUDED
#define PBGMATH_INCLUDED "数学関数     : Version 0.02 : Update 2001/03/05"

/*  [更新履歴]
 *    Version 0.02 : 2001/03/05 : 移動して、少々仕様を変更
 *    Version 0.01 : 1999/12/05 : PBG_MATH.c より移植開始
 */



#include "PbgType.h"
#include "CRnd.h"



/***** [ マクロ ] *****/
#define SinM(deg) (SIN256[(deg) & 0xff]) // SINﾃｰﾌﾞﾙ参照用ﾏｸﾛ
#define CosM(deg) (COS256[(deg) & 0xff]) // COSﾃｰﾌﾞﾙ参照用ﾏｸﾛ



/***** [関数プロトタイプ] *****/
FINT SinL(BYTE Deg,int Length);		// SinM(Deg) * Length / 256
FINT CosL(BYTE Deg,int Length);		// CosM(Deg) * Length / 256

FINT SinDiv(BYTE Deg, int Length);	// Length * 256 / SinM(Deg)
FINT CosDiv(BYTE Deg, int Length);	// Length * 256 / SosM(Deg)

BYTE pascal Atan8(int x,int y);	// 一般に言う atan2() の整数版です
int  pascal Isqrt(int s);		// 平方根を求める

FVOID RndSeedSet(DWORD val);	// 乱数の種をセットする
//WORD  pascal Rnd(void);			// 乱数を発生させる



/***** [グローバル変数] *****/
extern const signed int SIN256[256+64];		// サインテーブル
extern const signed int *COS256;			// コサインテーブル



#endif
