/*
 *   CSaveStraem.h   : 保存ストリーム
 *
 */

#ifndef CSAVESTREM_INCLUDED
#define CSAVESTREM_INCLUDED "保存ストリーム : Version 0.01 : Update 2002/02/07"

/*  [更新履歴]
 *    Version 0.01 : 2002/02/07 : 制作開始
 */



#include "StreamInfo.h"
#include "PbgMain.h"



/***** [クラス定義] *****/

// 保存ストリーム //
class CSaveStream {
public:
	FBOOL Open(char *pFileName, DWORD RndSeed);	// 書き込み開始
	FBOOL WriteData(WORD KeyCode);				// データを書き込む
	FBOOL Close(void);							// 閉じる

	CSaveStream();		// コンストラクタ
	~CSaveStream();		// デストラクタ


private:
	#define BUFFER_SIZE		65536

	WORD				m_WriteData[BUFFER_SIZE];	// 書き込むデータ
	DWORD				m_Current;					// 現在位置
	Pbg::LzEncode		m_Enc;						// 圧縮部
	BOOL				m_bOpened;					// 開いていれば真
};



#endif
