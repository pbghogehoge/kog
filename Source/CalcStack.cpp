/*
 *   CalcStack.cpp   : 演算スタック
 *
 */

#include "CalcStack.h"



/***** [グローバル変数] *****/
CalcStack		g_CalcStack;



/***** [スタティックメンバ] *****/
int CalcStack::m_Buffer[CALCSTACK_SIZE];	// 固定長スタック
int *CalcStack::m_StkPtr;					// スタックポインタ



// コンストラクタ //
CalcStack::CalcStack()
{
	m_StkPtr = m_Buffer;	// スタックポインタ
	ZEROMEM(m_Buffer);		// スタックをクリアする
}


// デストラクタ //
CalcStack::~CalcStack()
{
	// 何もしません //
}
