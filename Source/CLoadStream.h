/*
 *   CLoadStream.h   : 読み込みストリーム
 *
 */

#ifndef CLOADSTREAM_INCLUDED
#define CLOADSTREAM_INCLUDED "読み込みストリーム : Version 0.01 : Update 2002/02/07"

/*  [更新履歴]
 *    Version 0.01 : 2002/02/07 : 制作開始
 */



#include "StreamInfo.h"



/***** [クラス定義] *****/

// 読み込みストリーム //
class CLoadStream {
public:
	FBOOL  Open(char *pFileName);		// 読み込み開始
	FWORD  GetNextData(void);			// 次のデータを取得
	FDWORD GetRndSeed(void);			// 乱数の種を取得

	CLoadStream();		// コンストラクタ
	~CLoadStream();		// デストラクタ


private:
	FVOID Cleanup(void);					// メモリ開放を

	// ファイルオープン失敗 //
	FVOID LoadFileError(char *pFileName, BYTE ErrorID);

	StreamInfoHeader		*m_pInfo;		// ヘッダ
	WORD					*m_pData;		// データ格納域
	WORD					*m_pCurrent;	// 現在位置
	DWORD					m_DataSize;		// データ長
};



#endif
