/*
 *   CTransTable.cpp   : 変換テーブル
 *
 */

#include "CTransTable.h"
#include "PbgMain.h"



namespace Pbg {



// コンストラクタ //
CTransTable8::CTransTable8()
{
	m_Width       = 0;		// テーブルの幅
	m_Height      = 0;		// テーブルの高さ
	m_pData       = NULL;	// データ格納先
	m_pPitchTable = NULL;	// Ｙ方向参照用テーブル
}


// デストラクタ //
CTransTable8::~CTransTable8()
{
	Cleanup();
}


// 作成する //
FBOOL CTransTable8::Create(int Width, int Height)
{
	int			i;

	// すでに作成されている場合、そのデータを解放する //
	Cleanup();

	// データ格納用のメモリを確保する //
	m_pData = (BYTE *)MemAlloc(Width * Height);
	if(NULL == m_pData) return FALSE;

	// Ｙ方向参照用テーブルを作成する //
	m_pPitchTable = (BYTE **)MemAlloc(Height * sizeof(BYTE *));
	if(NULL == m_pPitchTable){
		MemFree(m_pData);
		return FALSE;
	}

	// テーブルを初期化する //
	for(i=0; i<Height; i++){
		m_pPitchTable[i] = m_pData + i * Width;
	}

	m_Width  = Width;
	m_Height = Height;

	return TRUE;
}


// 全データに Value をセットする //
FVOID CTransTable8::Clear(BYTE Value)
{
	// 作成されていない場合 //
	if(NULL == m_pData) return;

	memset(m_pData, Value, m_Width * m_Height);
}


// データの解放 //
FVOID CTransTable8::Cleanup(void)
{
	// メモリが確保されている場合は、解放する //
	if(m_pData)			MemFree(m_pData);
	if(m_pPitchTable)	MemFree(m_pPitchTable);

	m_Width  = 0;	// 幅
	m_Height = 0;	// 高さ
}



} // namespace Pbg
