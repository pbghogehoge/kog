/*
 *   CTransTable.h   : 変換テーブル
 *
 */

#ifndef CTRANSTABLE_INCLUDED
#define CTRANSTABLE_INCLUDED "変換テーブル : Version 0.01 : Update 2001/09/08"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/08 : 制作開始
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/

// 変換テーブル //
class CTransTable8 {
public:
	FBOOL Create(int Width, int Height);	// 作成する
	FVOID Clear(BYTE Value);				// 全データに Value をセットする

	IBYTE *GetHeadPtr(void){ return m_pData; };				// 先頭を取得
	IBYTE *GetPtrFromY(int y){ return m_pPitchTable[y]; };	// Ｙ座標開始を取得

	IINT GetWidth(void){ return m_Width; };		// 幅
	IINT GetHeight(void){ return m_Height; };	// 高さ

	CTransTable8();				// コンストラクタ
	virtual ~CTransTable8();	// デストラクタ


protected:
	int		m_Width;		// テーブルの幅
	int		m_Height;		// テーブルの高さ

	BYTE	*m_pData;			// データ列
	BYTE	**m_pPitchTable;	// Ｙ座標開始テーブル


public:
	FVOID Cleanup(void);	// データの解放
};



} // namespace Pbg



#endif
