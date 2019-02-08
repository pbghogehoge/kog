/*
 *   CalcStack.h   : 演算スタック
 *
 */

#ifndef CALCSTACK_INCLUDED
#define CALCSTACK_INCLUDED "演算スタック : Version 0.01 : Update 2001/03/12"

/*  [更新履歴]
 *    Version 0.02 : 2001/03/29 : Pop / Top 周辺のデバッグ
 *    Version 0.01 : 2001/03/12 : つくりはじめる
 */



#include "PbgType.h"
#include "PbgError.h"
#include "PbgMath.h"

#include "RndCtrl.h"



/***** [ 定数 ] *****/
#define CALCSTACK_SIZE		64			// スタックサイズ

#ifdef PBG_DEBUG
	#define STACK_OVER_CHECK()						\
	if((m_StkPtr - m_Buffer) >= CALCSTACK_SIZE){	\
		PbgError("CalcStack : StackOverflow");		\
		return;										\
	}

	#define STACK_UNDER_CHECK()							\
		if(m_StkPtr < m_Buffer){						\
			PbgError("CalcStack : StackUnderflow");		\
			return;										\
		}

#else
	#define STACK_OVER_CHECK()
	#define STACK_UNDER_CHECK()
#endif


/***** [クラス定義] *****/

// ScrBase 用・演算スタック //
class CalcStack {
public:
	static IVOID Push(int Value);	// スタックに値を積む
	static IINT  Top(void);			// スタックの一番下を返す
	static IINT  Pop(void);			// Top + Reset
	static IVOID Reset(void);		// スタックの初期化


	// 単項＆２項演算子 //
	static IVOID Add(void);		// Push(Pop(1) + Pop(0))
	static IVOID Sub(void);		// Push(Pop(1) - Pop(0))
	static IVOID Mul(void);		// Push(Pop(1) * Pop(0))
	static IVOID Div(void);		// Push(Pop(1) / Pop(0))
	static IVOID Mod(void);		// Push(Pop(1) % Pop(0))
	static IVOID Neg(void);		// Push(-Pop(0))

	// 関数 //
	static IVOID SinL(void);		// Push(sinl(Pop(1), Pop(0))
	static IVOID CosL(void);		// Push(cosl(Pop(1), Pop(0))
	static IVOID Rnd(void);			// Push(rnd() % Pop(0))
	static IVOID Atan(void);		// Push(atan(Pop(1), Pop(0))
	static IVOID Max(void);			// Push(max(Pop(0), Pop(1)))
	static IVOID Min(void);			// Push(min(Pop(0), Pop(1)))

	// 比較演算子 //
	static IVOID Equal(void);		// Push(Pop(1) == Pop(0))
	static IVOID NotEq(void);		// Push(Pop(1) != Pop(0))
	static IVOID Above(void);		// Push(Pop(1) >  Pop(0))
	static IVOID Less(void);		// Push(Pop(1) <  Pop(0))
	static IVOID AboveEq(void);		// Push(Pop(1) >= Pop(0))
	static IVOID LessEq(void);		// Push(Pop(1) <= Pop(0))

	// デバッグ用 //
	static IVOID DebugOut(void);	// スタックの中身を表示する


	CalcStack();		// コンストラクタ
	~CalcStack();		// デストラクタ


private:
	static int		m_Buffer[CALCSTACK_SIZE];	// 固定長スタック
	static int		*m_StkPtr;					// スタックポインタ
};



/***** [グローバル変数] *****/
extern CalcStack	g_CalcStack;



// スタックに値を積む //
IVOID CalcStack::Push(int Value)
{
	*m_StkPtr = Value;

	m_StkPtr++;
	STACK_OVER_CHECK();
}


// スタックの一番下を返す //
IINT CalcStack::Top(void)
{
#ifdef PBG_DEBUG
	if(m_StkPtr-1 != m_Buffer){
		PbgError(	"CalcStack::Top() : 演算スタック内にデータが"
					"残っている状態で呼び出されました");
		m_StkPtr = 0;
	}
#endif

	return *(m_Buffer - 1);
}


// Top + Reset //
IINT CalcStack::Pop(void)
{
	m_StkPtr--;

#ifdef PBG_DEBUG
	if(m_StkPtr != m_Buffer){
		PbgError(	"CalcStack::Pop() : 演算スタック内にデータが"
					"残っている状態で呼び出されました");
		m_StkPtr = 0;
	}
#endif

	return *m_Buffer;
}


// スタックの初期化 //
IVOID CalcStack::Reset(void)
{
	m_StkPtr = m_Buffer;
}


// Push(Pop(1) + Pop(0)) //
IVOID CalcStack::Add(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) += (*m_StkPtr);
}


// Push(Pop(1) - Pop(0)) //
IVOID CalcStack::Sub(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = (*(m_StkPtr-1)) - (*m_StkPtr);
}


// Push(Pop(1) * Pop(0)) //
IVOID CalcStack::Mul(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) *= (*m_StkPtr);
}


// Push(Pop(1) / Pop(0)) //
IVOID CalcStack::Div(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

#ifdef PBG_DEBUG
	if(0 == (*m_StkPtr)){
		PbgError("CalcStack::Div() : ゼロ除算");
		return;
	}
#endif

	*(m_StkPtr-1) = (*(m_StkPtr-1)) / (*m_StkPtr);
}


// Push(Pop(1) % Pop(0)) //
IVOID CalcStack::Mod(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

#ifdef PBG_DEBUG
	if(0 == (*m_StkPtr)){
		PbgError("CalcStack::Mod() : ゼロ除算");
		return;
	}
#endif

	*(m_StkPtr-1) = (*(m_StkPtr-1)) % (*m_StkPtr);
}


// Push(-Pop(0))
IVOID CalcStack::Neg(void)
{
	*((m_StkPtr-1)) = -(*(m_StkPtr-1));
}


// Push(sinl(Pop(1), Pop(0))
IVOID CalcStack::SinL(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ::SinL((*(m_StkPtr-1)), (*m_StkPtr));
}


// Push(cosl(Pop(1), Pop(0))
IVOID CalcStack::CosL(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ::CosL((*(m_StkPtr-1)), (*m_StkPtr));
}


// Push(rnd() % Pop(0))
IVOID CalcStack::Rnd(void)
{
#ifdef PBG_DEBUG
	if(0 == (*(m_StkPtr-1))){
		PbgError("CalcStack::Rnd() : ゼロ除算");
		return;
	}
#endif

	*(m_StkPtr-1) = ::RndEx() % (*(m_StkPtr-1));
}


// Push(atan(Pop(1), Pop(0))
IVOID CalcStack::Atan(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ::Atan8((*(m_StkPtr-1)), (*m_StkPtr));
}


// Push(max(Pop(0), Pop(1)))
IVOID CalcStack::Max(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = max((*(m_StkPtr-1)), (*m_StkPtr));
}


// Push(min(Pop(0), Pop(1)))
IVOID CalcStack::Min(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = min((*(m_StkPtr-1)), (*m_StkPtr));
}


// Push(Pop(1) == Pop(0))
IVOID CalcStack::Equal(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ( (*(m_StkPtr-1)) == (*m_StkPtr) );
}


// Push(Pop(1) != Pop(0))
IVOID CalcStack::NotEq(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ( (*(m_StkPtr-1)) != (*m_StkPtr) );
}


// Push(Pop(1) >  Pop(0))
IVOID CalcStack::Above(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ( (*(m_StkPtr-1)) > (*m_StkPtr) );
}


// Push(Pop(1) <  Pop(0))
IVOID CalcStack::Less(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ( (*(m_StkPtr-1)) < (*m_StkPtr) );
}


// Push(Pop(1) >= Pop(0))
IVOID CalcStack::AboveEq(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ( (*(m_StkPtr-1)) >= (*m_StkPtr) );
}


// Push(Pop(1) <= Pop(0))
IVOID CalcStack::LessEq(void)
{
	m_StkPtr--;
	STACK_UNDER_CHECK();

	*(m_StkPtr-1) = ( (*(m_StkPtr-1)) <= (*m_StkPtr) );
}


// スタックの中身を表示する //
IVOID CalcStack::DebugOut(void)
{
	int			i;
	char		buf[1024];
	char		temp[80];

	strcpy(buf, "(上)");

	for(i=m_StkPtr-m_Buffer-1; i>=0; i--){
		wsprintf(temp, " %d ", m_Buffer[i]);
		strcat(buf, temp);
	}

	strcat(buf, "(下)");
	PbgLog(buf);
}



#endif
