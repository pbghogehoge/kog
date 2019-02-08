/*
 *   WaveFileBase.h   : 波ファイル基底
 *
 */

#ifndef WAVEFILEBASE_INCLUDED
#define WAVEFILEBASE_INCLUDED "波ファイル基底 : Version 0.01 : Update 2001/08/25"

/*  [更新履歴]
 *    Version 0.01 : 2001/08/25 : 制作開始
 */



#include "PbgType.h"



namespace PbgEx {



/***** [クラス定義] *****/

// DirectShow を使う場合と、使わない場合について、共通のインターフェースを //
// 提供するための基底クラス                                                //
class WaveFileBase {
public:
	virtual BOOL Open(char *FileName) = 0;			// ファイルをオープンする
	virtual BOOL Reset(void) = 0;					// ファイル先頭に戻る
	virtual UINT Read(BYTE *pData, UINT Size) = 0;	// ファイルを読み込む(Ret:読み込みSize)
	virtual BOOL Seek(UINT Offset) = 0;				// ファイル内の移動(データ部における)
	virtual BOOL SetEnd(UINT Offset) = 0;			// 終端をセットする
	virtual BOOL Close(void) = 0;					// ファイルを閉じる

	virtual WAVEFORMATEX *GetFormat(void) = 0;	// フォーマットを取得する
	virtual DWORD		 GetSize(void)    = 0;	// バッファのサイズを取得
};



} // namespace PbgEx



#endif
