/*
 *   CRingBuffer.h   : リングバッファテンプレート
 *
 */

#ifndef CRINGBUFFER_INCLUDED
#define CRINGBUFFER_INCLUDED "リングバッファテンプレート : Version 0.01 : Update 2002/01/30"

/*  [更新履歴]
 *    Version 0.01 : 2002/01/30 : 制作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// 固定長リングバッファテンプレート //
template <class TYPE, DWORD NUM_ELEMENT>
class CFixedRingBuffer {
public:
	IVOID  initialize(void);			// 初期化する
	IVOID  insert(const TYPE &data);	// データを追加する
	IDWORD getNumData(void) const;		// データ数を取得する

	// id 番目のデータを取得する //
	inline const TYPE *getData(DWORD id) const;

	CFixedRingBuffer();		// コンストラクタ
	~CFixedRingBuffer();	// デストラクタ


private:
	// データの格納先 //
	TYPE	m_Buffer[NUM_ELEMENT];

	DWORD	m_HeadPos;		// 頭の開始位置
	DWORD	m_NumData;		// 格納されているデータ数
};



//------------------------------------------------------------------------
//   名称  | CFixedRingBuffer::CFixedRingBuffer()
//   説明  | コンストラクタ
//------------------------------------------------------------------------
template <class TYPE, DWORD NUM_ELEMENT>
CFixedRingBuffer<TYPE, NUM_ELEMENT>
	::CFixedRingBuffer()
{
	initialize();
}


//------------------------------------------------------------------------
//   名称  | CFixedRingBuffer::~CFixedRingBuffer()
//   説明  | デストラクタ
//------------------------------------------------------------------------
template <class TYPE, DWORD NUM_ELEMENT>
CFixedRingBuffer<TYPE, NUM_ELEMENT>
	::~CFixedRingBuffer()
{
}


//------------------------------------------------------------------------
//   名称  | CFixedRingBuffer::initialize()
//   説明  | 初期化する
//------------------------------------------------------------------------
template <class TYPE, DWORD NUM_ELEMENT>
IVOID CFixedRingBuffer<TYPE, NUM_ELEMENT>
	::initialize(void)
{
	m_HeadPos = 0;		// 先頭のデータ
	m_NumData = 0;		// 格納されているデータ数
}


//------------------------------------------------------------------------
//   名称  | CFixedRingBuffer::insert()
//   説明  | データを追加する
//------------------------------------------------------------------------
template <class TYPE, DWORD NUM_ELEMENT>
IVOID CFixedRingBuffer<TYPE, NUM_ELEMENT>
	::insert(const TYPE &data)
{
	if(m_NumData < NUM_ELEMENT){
		m_Buffer[m_NumData] = data;
		m_NumData = m_NumData + 1;
	}
	else{
		m_Buffer[m_HeadPos] = data;
		m_HeadPos = (m_HeadPos + 1) % NUM_ELEMENT;
	}
}


//------------------------------------------------------------------------
//   名称  | CFixedRingBuffer::getNumData()
//   説明  | データ数を取得する
//  戻り値 | 格納されているデータ数
//------------------------------------------------------------------------
template <class TYPE, DWORD NUM_ELEMENT>
IDWORD CFixedRingBuffer<TYPE, NUM_ELEMENT>
	::getNumData(void) const
{
	return m_NumData;
}


//------------------------------------------------------------------------
//   名称  | CFixedRingBuffer::getData()
//   説明  | id 番目のデータを取得する
//  戻り値 | データへのポインタ
//------------------------------------------------------------------------
template <class TYPE, DWORD NUM_ELEMENT>
inline const TYPE *CFixedRingBuffer<TYPE, NUM_ELEMENT>
	::getData(DWORD id) const
{
	// そんなデータはありません //
	if(id >= m_NumData) return NULL;

	return m_Buffer + (m_HeadPos + id) % NUM_ELEMENT;
}



#endif
