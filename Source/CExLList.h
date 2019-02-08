/*
 *   CExLList.h   : 特殊な線形リスト
 *
 */

#ifndef CEXLLIST_INCLUDED
#define CEXLLIST_INCLUDED "特殊な線形リスト : Version 0.02 : Update 2001/03/04"

/*  [更新履歴]
 *    Version 0.02 : 2001/03/04 : InsertBack(), MoveBack() のバグを修正
 *    Version 0.01 : 2001/03/03 : 雛祭り
 */



#include "PbgType.h"



/***** [マクロ] *****/

// 先頭からの for_each マクロ(クラス内で使用) //
#define ThisForEachFront(n, IT)		\
	for(GetFrontIterator(IT, n);	\
		IT != NULL;					\
		IT ++)

// 終端からの for_each マクロ(クラス内で使用) //
#define ThisForEachBack(n, IT)		\
	for(GetBackIterator(IT, n);		\
		IT != NULL;					\
		IT --)

// 先頭からの for_each マクロ //
#define ForEachFront(TYPE, n, IT)		\
	for(TYPE.GetFrontIterator(IT, n);	\
		IT != NULL;						\
		IT ++)

// 終端からの for_each マクロ //
#define ForEachBack(TYPE, n, IT)		\
	for(TYPE.GetBackIterator(IT, n);	\
		IT != NULL;						\
		IT --)

// 先頭からの for_each マクロ(ポインタ版) //
#define ForEachFrontPtr(TYPE, n, IT)	\
	for(TYPE->GetFrontIterator(IT, n);	\
		IT != NULL;						\
		IT ++)

// 終端からの for_each マクロ(ポインタ版) //
#define ForEachBackPtr(TYPE, n, IT)		\
	for(TYPE->GetBackIterator(IT, n);	\
		IT != NULL;						\
		IT --)

// 削除して break する //
#define ThisDelBreak(IT)	\
	{						\
		Delete(IT);			\
		break;				\
	}

// 削除して continue する //
#define ThisDelContinue(IT)	\
	{						\
		Delete(IT);			\
		continue;			\
	}

// 削除して break する(ポインタ版) //
#define DelBreakPtr(pTYPE, IT)	\
	{							\
		pTYPE->Delete(IT);		\
		break;					\
	}

// 削除して continue する(ポインタ版) //
#define DelContinuePtr(pTYPE, IT)	\
	{								\
		pTYPE->Delete(IT);			\
		continue;					\
	}

// 削除して break する //
#define DelBreak(TYPE, IT)	\
	{						\
		TYPE.Delete(IT);	\
		break;				\
	}

// 削除して continue する //
#define DelContinue(TYPE, IT)	\
	{							\
		TYPE.Delete(IT);		\
		continue;				\
	}


// 評価値が NULL の場合、エラーを吐いてリターンする //
#ifdef PBG_DEBUG
	#define NullCheck(expr, comment)	\
		if(NULL == (expr)){				\
			PbgError(comment);			\
			return;						\
		}
#else
	#define NullCheck(expr, comment)
#endif



/***** [クラス定義] *****/

// 相互参照の為... //
template <class TYPE, DWORD NUM_ELEMENT>
class CExLList;


// 双方向線形リスト化テンプレート //
template <class TYPE>
class CDualLinearList : public TYPE {
public:
	CDualLinearList		*m_pPrev;	// 一つ前のデータ
	CDualLinearList		*m_pNext;	// 次のデータ
};


// 線形リスト反復子テンプレート //
template <class TYPE, DWORD NUM_ELEMENT>
class Iterator {
	typedef CDualLinearList<TYPE>		TargetType;	// 格納するデータ型
	typedef CExLList<TYPE, NUM_ELEMENT>	ParentType;	// 親クラスの型
	friend ParentType;		// 親クラスをフレンドクラスとする


public:
	// 前進(後置・戻り値無し) //
	inline void operator ++(int){
		if(m_bDelete){
			m_bDelete = FALSE;
			return;
		}
		NullCheck(m_Target, "Iterator++ : NULL イテレータへの演算");
		m_Target = m_Target->m_pNext;
		NullCheck(m_Target, "Iterator++ : NULL イテレータへの演算");
		if(NULL == m_Target->m_pNext){
			m_Target = NULL;
		}
	};

	// 後退(後置・戻り値無し) //
	inline void operator --(int){
		if(m_bDelete){
			m_bDelete = FALSE;
			return;
		}
		NullCheck(m_Target, "Iterator-- : NULL イテレータへの演算");
		m_Target = m_Target->m_pPrev;
		NullCheck(m_Target, "Iterator-- : NULL イテレータへの演算");
		if(NULL == m_Target->m_pPrev){
			m_Target = NULL;
		}
	};

	// 等しい：ポインタ比較 //
	inline BOOL operator ==(void *ptr){
		if(m_Target == ptr) return TRUE;
		else                return FALSE;
	};

	// 等しくない：ポインタ比較 //
	inline BOOL operator !=(void *ptr){
		if(m_Target != ptr) return TRUE;
		else                return FALSE;
	};

	// 等しい：イテレータ比較 //
	inline BOOL operator ==(Iterator &it){
		if(m_Target == it.m_Target) return TRUE;
		else                        return FALSE;
	}

	// 等しくない：イテレータ比較 //
	inline BOOL operator !=(Iterator &it){
		if(m_Target != it.m_Target) return TRUE;
		else                        return FALSE;
	}

	// メンバへのポインタを返す //
	inline TYPE *GetPtr(void){
		return (TYPE *)m_Target;
	};

	// メンバへのポインタを返す(演算子版) //
	inline TYPE *operator ->(){
		return (TYPE *)m_Target;
	};

	// メンバへの参照を返す //
	inline TYPE &operator *(){
		return *(TYPE *)m_Target;
	}


private:
	// 反復子の初期化を行う //
	void Initialize(TargetType *t, ParentType *p, int dir){
		if(t && t->m_pNext && t->m_pPrev)
			m_Target  = t;			// 初期ポイントデータ
		else
			m_Target = NULL;

		m_Parent  = p;			// 親クラスの this ポインタ
		m_bDelete = FALSE;		// 最初は削除していない

		// 進行方向を決める //
		if(dir < 0) m_Direction = -1;	// 後退反復子
		else        m_Direction = +1;	// 前進反復子
	}

	TargetType		*m_Target;		// 対象となるデータ
	ParentType		*m_Parent;		// 親クラス

	// 真ならポインタを動かさないでこのフラグを偽にする //
	BOOL	m_bDelete;

	// 消去時の進行方向 //
	int		m_Direction;
};


// 特殊線形リストテンプレート //
template <class TYPE, DWORD NUM_ELEMENT>
class CExLList {
public:
	// クラス上のイテレータ //
	typedef Iterator<TYPE, NUM_ELEMENT> Iterator;
	typedef CDualLinearList<TYPE>		DList;

	// 確保・初期化 //
	BOOL Initialize(DWORD BufferSize);	// バッファを確保する
	void Cleanup(void);					// 全データを解放する

	// イテレータ取得 //
	IVOID GetFrontIterator(Iterator &it, DWORD n);	// ｎ番リスト先頭を取得
	IVOID GetBackIterator (Iterator &it, DWORD n);	// ｎ番リスト終端を取得

	// 挿入(失敗時は NULL を返す) //
	inline TYPE *InsertFront(DWORD n);	// ｎ番リスト先頭にデータを追加
	inline TYPE *InsertBack (DWORD n);	// ｎ番リスト終端にデータを追加

	// 移動・削除 //
	inline TYPE *MoveFront(Iterator &it, DWORD n);	// ｎ番リスト先頭に移動
	inline TYPE *MoveBack (Iterator &it, DWORD n);	// ｎ番リスト終端に移動
	inline void  Delete   (Iterator &it);			// 削除する

	// 全データを解放済みリストに接続 //
	void DeleteAllData(void);

	// サイズ取得 //
	inline DWORD GetBufferSize(void);	// 現在のバッファサイズを取得
	inline DWORD GetActiveData(void);	// 現在のアクティブデータ数を取得

	CExLList();				// コンストラクタ
	virtual ~CExLList();	// デストラクタ


private:
	DList	*m_pDataBuffer;				// データの一括確保用
	DList	*m_pFreeData;				// 内部解放済みデータ
	DList	m_FrontNode[NUM_ELEMENT];	// リスト先頭ダミーノード
	DList	m_BackNode[NUM_ELEMENT];	// リスト終端ダミーノード

	DWORD		m_BufferSize;	// バッファのサイズ
	DWORD		m_NumActive;	// アクティブなデータ数
};



// コンストラクタ //
template <class TYPE, DWORD NUM_ELEMENT>
CExLList<TYPE, NUM_ELEMENT>
	::CExLList()
{
	m_pDataBuffer = NULL;	// データの一括確保用
	m_pFreeData   = NULL;	// 解放済みデータリスト
	m_BufferSize  = 0;		// バッファサイズ
	m_NumActive   = 0;		// アクティブなデータ数

	ZEROMEM(m_FrontNode);	// リスト先頭ダミーノード
	ZEROMEM(m_BackNode);	// リスト終端ダミーノード
}


// デストラクタ //
template <class TYPE, DWORD NUM_ELEMENT>
CExLList<TYPE, NUM_ELEMENT>
	::~CExLList()
{
	Cleanup();
}


// バッファを確保する //
template <class TYPE, DWORD NUM_ELEMENT>
BOOL CExLList<TYPE, NUM_ELEMENT>
	::Initialize(DWORD BufferSize)
{
	Cleanup();

	// まずは、バッファを生成する //
	m_pDataBuffer = NewEx(DList[BufferSize]);
	if(NULL == m_pDataBuffer) return FALSE;		// メモリ足りん

	m_BufferSize = BufferSize;	// バッファサイズを記憶する
	DeleteAllData();			// リストを初期化する

	return TRUE;
}


// 全データを解放する //
template <class TYPE, DWORD NUM_ELEMENT>
void CExLList<TYPE, NUM_ELEMENT>
	::Cleanup(void)
{
	// バッファが作成されていたら，削除する    //
	// 注意:配列用のデストラクタを呼び出すこと //
	if(NULL != m_pDataBuffer){
		DeleteArray(m_pDataBuffer);
	}

	// 一連の変数を初期化 //
	m_pDataBuffer = NULL;	// データの一括確保用
	m_pFreeData   = NULL;	// 解放済みデータリスト
	m_BufferSize  = 0;		// バッファサイズ
	m_NumActive   = 0;		// アクティブなデータ数

	ZEROMEM(m_FrontNode);	// リスト先頭ダミーノード
	ZEROMEM(m_BackNode);	// リスト終端ダミーノード
}


// ｎ番リストの先頭を取得 //
template <class TYPE, DWORD NUM_ELEMENT>
IVOID CExLList<TYPE, NUM_ELEMENT>
	::GetFrontIterator(Iterator &it, DWORD n)
{
#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CExLList::GetFrontIterator() : 範囲外の要素");
		it.Initialize(NULL, this, +1);
		return;
	}
#endif

	it.Initialize(m_FrontNode[n].m_pNext, this, +1);
}


// ｎ番リストの終端を取得 //
template <class TYPE, DWORD NUM_ELEMENT>
IVOID CExLList<TYPE, NUM_ELEMENT>
	::GetBackIterator(Iterator &it, DWORD n)
{
#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CExLList::GetBackIterator() : 範囲外の要素");
		it.Initialize(NULL, this, -1);
		return;
	}
#endif

	it.Initialize(m_BackNode[n].m_pPrev, this, -1);
}


// ｎ番リスト先頭にデータを追加 //
template <class TYPE, DWORD NUM_ELEMENT>
inline TYPE *CExLList<TYPE, NUM_ELEMENT>
	::InsertFront(DWORD n)
{
	DList		*p;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CExLList::InsertFront() : 範囲外の要素");
		return NULL;
	}
#endif

	// 空きデータが存在しない場合 //
	if(NULL == m_pFreeData) return NULL;

	// 解放済みデータリストから空きデータの取得 //
	p           = m_pFreeData;				// 取得したポインタ
	m_pFreeData = m_pFreeData->m_pNext;		// 次のデータに移動しておく

	// 今回のデータから接続 //
	p->m_pPrev = &m_FrontNode[n];			// 挿入データ -> 前回の先頭
	p->m_pNext = m_FrontNode[n].m_pNext;	// 挿入データ -> 先頭

	// 前回のデータから接続 //
	m_FrontNode[n].m_pNext->m_pPrev = p;	// 前回の先頭 -> 挿入データ
	m_FrontNode[n].m_pNext          = p;	// 先頭       -> 挿入データ

	// アクティブなデータ数をインクリメントする //
	m_NumActive++;

	// アップキャストして、返す //
	return (TYPE *)p;
}


// ｎ番リスト終端にデータを追加 //
template <class TYPE, DWORD NUM_ELEMENT>
inline TYPE *CExLList<TYPE, NUM_ELEMENT>
	::InsertBack(DWORD n)
{
	DList		*p;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CExLList::InsertBack() : 範囲外の要素");
		return NULL;
	}
#endif

	// 空きデータが存在しない場合 //
	if(NULL == m_pFreeData) return NULL;

	// 解放済みデータリストから空きデータの取得 //
	p           = m_pFreeData;				// 取得したポインタ
	m_pFreeData = m_pFreeData->m_pNext;		// 次のデータに移動しておく

	// 今回のデータから接続 //
	p->m_pNext = &m_BackNode[n];			// 挿入データ -> 前回の尻尾
	p->m_pPrev = m_BackNode[n].m_pPrev;		// 挿入データ -> 尻尾

	// 前回のデータから接続 //
	m_BackNode[n].m_pPrev->m_pNext = p;		// 前回の尻尾 -> 挿入データ
	m_BackNode[n].m_pPrev          = p;		// 尻尾       -> 挿入データ

	// アクティブなデータ数をインクリメント //
	m_NumActive++;

	// アップキャストして、返す //
	return (TYPE *)p;
}


// ｎ番リスト先頭に移動 //
template <class TYPE, DWORD NUM_ELEMENT>
inline TYPE *CExLList<TYPE, NUM_ELEMENT>
	::MoveFront(Iterator &it, DWORD n)
{
	DList		*p, *Prev, *Next;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CExLList::MoveFront() : 範囲外の要素");
		return NULL;
	}
#endif

	p    = it.m_Target;		// 移動するデータ
	Prev = p->m_pPrev;		// 前のデータ
	Next = p->m_pNext;		// 次のデータ

	// イテレータを移動する //
	if(it.m_Direction < 0) it--;	// 後退
	else                   it++;	// 前進

	Prev->m_pNext = Next;	// 前→後
	Next->m_pPrev = Prev;	// 後→前

	// 削除フラグを立てる //
	it.m_bDelete = TRUE;

	// 今回のデータから接続 //
	p->m_pPrev = &m_FrontNode[n];			// 挿入データ -> 前回の先頭
	p->m_pNext = m_FrontNode[n].m_pNext;	// 挿入データ -> 先頭

	// 前回のデータから接続 //
	m_FrontNode[n].m_pNext->m_pPrev = p;	// 前回の先頭 -> 挿入データ
	m_FrontNode[n].m_pNext          = p;	// 先頭       -> 挿入データ

	return (TYPE *)p;
}


// ｎ番リスト終端に移動 //
template <class TYPE, DWORD NUM_ELEMENT>
inline TYPE *CExLList<TYPE, NUM_ELEMENT>
	::MoveBack(Iterator &it, DWORD n)
{
	DList		*p, *Prev, *Next;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CExLList::MoveBack() : 範囲外の要素");
		return NULL;
	}
#endif

	p    = it.m_Target;		// 移動するデータ
	Prev = p->m_pPrev;		// 前のデータ
	Next = p->m_pNext;		// 次のデータ

	// イテレータを移動する //
	if(it.m_Direction < 0) it--;	// 後退
	else                   it++;	// 前進

	Prev->m_pNext = Next;	// 前→後
	Next->m_pPrev = Prev;	// 後→前

	// 削除フラグを立てる //
	it.m_bDelete = TRUE;

	// 今回のデータから接続 //
	p->m_pNext = &m_BackNode[n];			// 挿入データ -> 前回の尻尾
	p->m_pPrev = m_BackNode[n].m_pPrev;		// 挿入データ -> 尻尾

	// 前回のデータから接続 //
	m_BackNode[n].m_pPrev->m_pNext = p;		// 前回の尻尾 -> 挿入データ
	m_BackNode[n].m_pPrev          = p;		// 尻尾       -> 挿入データ

	// アップキャストして、返す //
	return (TYPE *)p;
}


// 削除する //
template <class TYPE, DWORD NUM_ELEMENT>
inline void CExLList<TYPE, NUM_ELEMENT>
	::Delete(Iterator &it)
{
	DList		*p, *Prev, *Next;

	p    = it.m_Target;		// 削除するデータ
	Prev = p->m_pPrev;		// 前のデータ
	Next = p->m_pNext;		// 次のデータ

	// イテレータを移動する //
	if(it.m_Direction < 0) it--;	// 後退
	else                   it++;	// 前進

	Prev->m_pNext = Next;	// 前→後
	Next->m_pPrev = Prev;	// 後→前

	// 削除フラグを立てる //
	it.m_bDelete = TRUE;

	// 解放済みリストに挿入する //
	p->m_pNext  = m_pFreeData;		// 前のデータを今回のデータの次へと
	m_pFreeData = p;				// 先頭に削除したデータを

	// アクティブなデータ数をデクリメントする //
	m_NumActive--;
}


// 全データを解放済みリストに接続 //
template <class TYPE, DWORD NUM_ELEMENT>
void CExLList<TYPE, NUM_ELEMENT>
	::DeleteAllData(void)
{
	DWORD		i;
	DWORD		BufferSize;
	DList		*pIt;

	// バッファが生成されていないので、何もしない //
	if(NULL == m_pDataBuffer) return;

	// 少々、高速化を図る //
	BufferSize = m_BufferSize;

// 参考:解放済みリスト内のデータの場合は前のデータが分からなくても良いので  //
//      NULL で無効にしている。データの削除を行うときは、この値は不定となる //

	// リストを繋ぐ //
	for(i=1; i<=BufferSize-2; i++){
		pIt = &m_pDataBuffer[i];

		pIt->m_pPrev = NULL;					// 前のデータは接続しない
		pIt->m_pNext = &m_pDataBuffer[i+1];		// 後ろのデータのみ接続
	}

	// 先頭データを初期化 //
	m_pDataBuffer[0].m_pPrev = NULL;				// 前のデータは存在せず
	m_pDataBuffer[0].m_pNext = &m_pDataBuffer[1];	// 次のデータのみをセット

	// 終端データを初期化する //
	m_pDataBuffer[BufferSize-1].m_pPrev = NULL;	// 前のデータは必要なし
	m_pDataBuffer[BufferSize-1].m_pNext = NULL;	// 次のデータは存在せず

	// 解放済みリストに接続 //
	m_pFreeData = m_pDataBuffer;

	// その他のデータをゼロ初期化する //
	m_NumActive = 0;		// アクティブなデータ数
	ZEROMEM(m_FrontNode);	// リスト先頭ダミーノード
	ZEROMEM(m_BackNode);	// リスト終端ダミーノード

	// ダミーノードの初期化 //
	for(i=0; i<NUM_ELEMENT; i++){
		m_FrontNode[i].m_pNext = &m_BackNode[i];	// 前→後を接続
		m_BackNode[i].m_pPrev  = &m_FrontNode[i];	// 後→前を接続
	}
}


// 現在のバッファサイズを取得
template <class TYPE, DWORD NUM_ELEMENT>
inline DWORD CExLList<TYPE, NUM_ELEMENT>
	::GetBufferSize(void)
{
	return m_BufferSize;
}


// 現在のアクティブデータ数を取得
template <class TYPE, DWORD NUM_ELEMENT>
inline DWORD CExLList<TYPE, NUM_ELEMENT>
	::GetActiveData(void)
{
	return m_NumActive;
}



#endif
