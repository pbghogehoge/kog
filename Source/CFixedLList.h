/*
 *   CFixedLList.h   : 線形リスト(固定サイズ版)
 *
 */

#ifndef CFIXEDLLIST_INCLUDED
#define CFIXEDLLIST_INCLUDED "線形リスト(固定サイズ版) : Version 0.02 : Update 2001/03/10"

/*  [更新履歴]
 *    Version 0.02 : 2001/03/10 : リスト内データ全体の移動を追加
 *
 *    Version 0.01 : 2001/03/05 : 固定サイズの奴が欲しい
 *                              : ちなみにCExLListとは違うので注意
 */



#include "CExLList.h"
#include "PbgError.h"



/***** [クラス定義] *****/

// 相互参照の為... //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
class CFixedLList;


// 線形リスト反復子テンプレート //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
class FIterator {
	// 格納するデータ型 //
	typedef CDualLinearList<TYPE>	TargetType;

	// 親クラスの型 //
	typedef CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>	ParentType;

	friend ParentType;


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
		// アップキャストする //
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
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
class CFixedLList {
public:
	// クラス上のイテレータ //
	typedef FIterator<TYPE, NUM_ELEMENT, BUFFER_SIZE>	Iterator;
	typedef CDualLinearList<TYPE>						DList;

	// 全データを解放する //
	void Cleanup(void);

	// TYPE 型のオブジェクトに対する固有の値を返す //
	IINT GetUniqueID(TYPE *p);

	// イテレータ取得 //
	IVOID GetFrontIterator(Iterator &it, DWORD n);	// ｎ番リスト先頭を取得
	IVOID GetBackIterator (Iterator &it, DWORD n);	// ｎ番リスト終端を取得

	// 挿入(失敗時は NULL を返す) //
	inline TYPE *InsertFront(DWORD n);	// ｎ番リスト先頭にデータを追加
	inline TYPE *InsertBack (DWORD n);	// ｎ番リスト終端にデータを追加

	// 全体を移動 //
	IVOID MoveFrontAll(DWORD NewList, DWORD OldList);	// Old全体->New 先頭
	IVOID MoveBackAll (DWORD NewList, DWORD OldList);	// Old全体->New 終端

	// 削除 //
	IVOID Delete(Iterator &it);

	// 一つだけ移動 //
	inline TYPE *MoveFront(Iterator &it, DWORD n);	// ｎ番リスト先頭に移動
	inline TYPE *MoveBack (Iterator &it, DWORD n);	// ｎ番リスト終端に移動


	// 全データを解放済みリストに接続 //
	void DeleteAllData(void);

	// サイズ取得 //
	inline DWORD GetActiveData(void);	// 現在のアクティブデータ数を取得

	CFixedLList();				// コンストラクタ
	virtual ~CFixedLList();		// デストラクタ


private:
	DList	m_pDataBuffer[BUFFER_SIZE];	// 確保済みデータ配列
	DList	*m_pFreeData;				// 内部解放済みデータ
	DList	m_FrontNode[NUM_ELEMENT];	// リスト先頭ダミーノード
	DList	m_BackNode[NUM_ELEMENT];	// リスト終端ダミーノード

	DWORD		m_NumActive;	// アクティブなデータ数
};



// コンストラクタ //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::CFixedLList()
{
	m_pFreeData   = NULL;	// 解放済みデータリスト
	m_NumActive   = 0;		// アクティブなデータ数

	ZEROMEM(m_pDataBuffer);	// 確保済みデータ配列
	ZEROMEM(m_FrontNode);	// リスト先頭ダミーノード
	ZEROMEM(m_BackNode);	// リスト終端ダミーノード

	DeleteAllData();
}


// デストラクタ //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::~CFixedLList()
{
	Cleanup();
}


// 全データを解放する //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
void CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::Cleanup(void)
{
	m_pFreeData   = NULL;	// 解放済みデータリスト
	m_NumActive   = 0;		// アクティブなデータ数

	ZEROMEM(m_pDataBuffer);	// 確保済みデータ配列
	ZEROMEM(m_FrontNode);	// リスト先頭ダミーノード
	ZEROMEM(m_BackNode);	// リスト終端ダミーノード
}


// イテレータの指すオブジェクトに対する固有の値を返す //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
IINT CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::GetUniqueID(TYPE *p)
{
	return static_cast<DList *>(p) - m_pDataBuffer;
}


// ｎ番リストの先頭を取得 //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
IVOID CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::GetFrontIterator(Iterator &it, DWORD n)
{
#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CFixedLList::GetFrontIterator() : 範囲外の要素");
		it.Initialize(NULL, this, +1);
		return;
	}
#endif

	it.Initialize(m_FrontNode[n].m_pNext, this, +1);
}


// ｎ番リストの終端を取得 //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
IVOID CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::GetBackIterator(Iterator &it, DWORD n)
{
#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CFixedLList::GetBackIterator() : 範囲外の要素");
		it.Initialize(NULL, this, -1);
		return;
	}
#endif

	it.Initialize(m_BackNode[n].m_pPrev, this, -1);
}


// ｎ番リスト先頭にデータを追加 //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
inline TYPE *CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::InsertFront(DWORD n)
{
	DList		*p;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CFixedLList::InsertFront() : 範囲外の要素");
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
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
inline TYPE *CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::InsertBack(DWORD n)
{
	DList		*p;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CFixedLList::InsertBack() : 範囲外の要素");
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

	// アクティブなデータ数をインクリメントする //
	m_NumActive++;

	return (TYPE *)p;
}


// 全体を移動(Old全体->New 先頭) //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
IVOID CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::MoveFrontAll(DWORD NewList, DWORD OldList)
{
	DList		*Front, *Back;

#ifdef PBG_DEBUG
	if(NewList >= NUM_ELEMENT){
		PbgError("CFixedLList::MoveFront() : 範囲外の要素(NewList)");
		return;
	}
	if(OldList >= NUM_ELEMENT){
		PbgError("CFixedLList::MoveFront() : 範囲外の要素(OldList)");
		return;
	}
#endif

	// リスト内にデータが存在しない場合 //
	if(&m_FrontNode[OldList] == m_BackNode[OldList].m_pPrev) return;

	Front = m_FrontNode[OldList].m_pNext;	// 移動元データ先頭
	Back  = m_BackNode [OldList].m_pPrev;	// 移動元データ終端

	// 古いリストの中身を空っぽにする //
	m_FrontNode[OldList].m_pNext = &m_BackNode [OldList];	// 前→後を接続
	m_BackNode [OldList].m_pPrev = &m_FrontNode[OldList];	// 後→前を接続

	// 移動元データリストの前後を接続する //
	Front->m_pBack = &m_FrontNode[NewList];			// 挿入データ先頭
	Back->m_pPrev  =  m_FrontNode[NewList].m_pNext;	// 挿入データ終端

	// 移動先データリストの先頭に挿入する //
	m_FrontNode[NewList].m_pNext->m_pPrev = Back;	// データ挿入部(終端)
	m_FrontNode[NewList].m_pNext          = Front;	// データ挿入部(先頭)
}


// 全体を移動(Old全体->New 終端) //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
IVOID CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::MoveBackAll(DWORD NewList, DWORD OldList)
{
	DList		*Front, *Back;

#ifdef PBG_DEBUG
	if(NewList >= NUM_ELEMENT){
		PbgError("CFixedLList::MoveBack() : 範囲外の要素(NewList)");
		return;
	}
	if(OldList >= NUM_ELEMENT){
		PbgError("CFixedLList::MoveBack() : 範囲外の要素(OldList)");
		return;
	}
#endif

	// リスト内にデータが存在しない場合 //
	if(&m_FrontNode[OldList] == m_BackNode[OldList].m_pPrev) return;

	Front = m_FrontNode[OldList].m_pNext;	// 移動元データ先頭
	Back  = m_BackNode [OldList].m_pPrev;	// 移動元データ終端

	// 古いリストの中身を空っぽにする //
	m_FrontNode[OldList].m_pNext = &m_BackNode [OldList];	// 前→後を接続
	m_BackNode [OldList].m_pPrev = &m_FrontNode[OldList];	// 後→前を接続

	// 移動元データリストの前後を接続する //
	Back->m_pNext  = &m_BackNode[NewList];			// 挿入データ終端
	Front->m_pPrev =  m_BackNode[NewList].m_pPrev;	// 挿入データ先頭

	// 移動先データリストの先頭に挿入する //
	m_BackNode[NewList].m_pPrev->m_pNext = Front;	// データ挿入部(先頭)
	m_BackNode[NewList].m_pPrev          = Back;	// データ挿入部(終端)
}


// 削除する //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
inline void CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
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


// ｎ番リスト先頭に移動 //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
inline TYPE *CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::MoveFront(Iterator &it, DWORD n)
{
	DList		*p, *Prev, *Next;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CFixedLList::MoveFront() : 範囲外の要素");
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

	// アップキャストして、返す //
	return (TYPE *)p;
}


// ｎ番リスト終端に移動 //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
inline TYPE *CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::MoveBack(Iterator &it, DWORD n)
{
	DList		*p, *Prev, *Next;

#ifdef PBG_DEBUG
	if(n >= NUM_ELEMENT){
		PbgError("CFixedLList::MoveBack() : 範囲外の要素");
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


// 全データを解放済みリストに接続 //
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
void CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::DeleteAllData(void)
{
	DWORD		i;
	DList		*pIt;

// 参考:解放済みリスト内のデータの場合は前のデータが分からなくても良いので  //
//      NULL で無効にしている。データの削除を行うときは、この値は不定となる //

	// リストを繋ぐ //
	for(i=1; i<=BUFFER_SIZE-2; i++){
		pIt = &m_pDataBuffer[i];

		pIt->m_pPrev = NULL;					// 前のデータは接続しない
		pIt->m_pNext = &m_pDataBuffer[i+1];		// 後ろのデータのみ接続
	}

	// 先頭データを初期化する //
	m_pDataBuffer[0].m_pPrev = NULL;				// 前のデータは存在せず
	m_pDataBuffer[0].m_pNext = &m_pDataBuffer[1];	// 次のデータのみをセット

	// 終端データを初期化する //
	m_pDataBuffer[BUFFER_SIZE-1].m_pPrev = NULL;	// 前のデータは必要なし
	m_pDataBuffer[BUFFER_SIZE-1].m_pNext = NULL;	// 次のデータは存在せず

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


// 現在のアクティブデータ数を取得
template <class TYPE, DWORD NUM_ELEMENT, DWORD BUFFER_SIZE>
inline DWORD CFixedLList<TYPE, NUM_ELEMENT, BUFFER_SIZE>
	::GetActiveData(void)
{
	return m_NumActive;
}



#endif
