/*
 *   PbgUtil.h   : 汎用テンプレート等
 *
 */

#ifndef PBGUTIL_INCLUDED
#define PBGUTIL_INCLUDED "汎用テンプレート等 : Version 0.02 : Update 2001/05/16"

/*  [更新履歴]
 *    Version 0.02 : 2001/05/16 : アプリケーション多重起動防止関数の追加
 *    Version 0.01 : 2001/04/25 : 製作開始
 */



#include "PbgType.h"
#include "CRnd.h"



namespace Pbg {



/***** [関数プロトタイプ] *****/

// すでにアプリケーションが起動していれば、そのウィンドウを //
// アクティブにして、ハンドルを返す                         //
HWND IsRunningApp(LPCTSTR lpClassName, LPCTSTR lpWindowName);



/***** [関数テンプレート] *****/

// Data1 と Data2 の内容を交換する //
template <class TYPE>
IVOID Swap(TYPE &Data1, TYPE &Data2)
{
	TYPE	Temp;

	Temp  = Data1;
	Data1 = Data2;
	Data2 = Temp;
}


// Data[0..Range-1] のデータをシャッフルする //
// 乱数管理クラス版                          //
template <class TYPE>
IVOID Shuffle(TYPE *pData, int Range, CRnd *pRnd)
{
	int		i, j;

	for(i=Range-1; i>0; i--){
		j = pRnd->Get() % i;		// 入れ替える要素
		Swap(pData[i], pData[j]);	// スワップ実行
	}
}



} // namespace Pbg



/***** [インライン関数] *****/

// 相対指定の SetRect() //
inline void SetRect2(RECT *pRect, int x, int y, int w, int h)
{
	SetRect(pRect, x, y, x+w, y+h);
}



#endif
