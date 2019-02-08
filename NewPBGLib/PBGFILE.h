/*
 *   PbgFile.h   : 簡易File I/O (Win32? API 版)
 *
 */

#ifndef PBGFILE_INCLUDED
#define PBGFILE_INCLUDED "簡易File I/O : Version 0.04 : Update 2001/10/09"

/*  [注意事項]
 *    シークの際には以下の定数を用いること！！
 *
 *    FILE_BEGIN     開始点は 0、つまりファイルの先頭です。
 *    FILE_CURRENT   開始点は、ファイルポインタの現在の位置です。
 *    FILE_END       開始点は、ファイルの終わり（EOF）です。
 */

/*  [更新履歴]
 *    Version 0.04 : 2001/10/09 : メモリ上展開機能を追加
 *    Version 0.03 : 2001/02/24 : 書き込み時の動作を変更した
 *    Version 0.02 : 2001/02/09 : Seek(), GetPosition() を追加した
 *                              : (アーカイブライブラリ対策)
 *
 *    Version 0.01 : 2001/01/23 : マルチスレッド対応のため作成する
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/
class CPbgFile {
public:
	virtual BOOL Open(const char *FileName, char *mode);	// ファイルをオープンする
	virtual void Close(void);								// ファイルをクローズする

	virtual BOOL Read(void *Buffer, DWORD Size, DWORD *ReadSize);	// いわゆる fread
	virtual BOOL Write(void *Buffer, DWORD Size, DWORD *WriteSize);	// いわゆる fwrite

	virtual int GetChar(void);		// いわゆる fgetc
	virtual int PutChar(int Data);	// いわゆる fputc

	virtual BOOL  Seek(DWORD Offset, DWORD Method);		// いわゆる fseek
	virtual DWORD GetPosition(void);					// いわゆる ftell(現在位置)
	virtual DWORD GetSize(void);						// いわゆる ftell(サイズ)

	BOOL WriteStr(void *Buffer);				// 文字列を書き込む

	// メモリ上にデータを展開する Uses MemAlloc() //
	// arg : MaxSize 許される最大サイズ           //
	// ret : 失敗なら NULL                        //
	virtual LPVOID GetData(DWORD MaxSize);

	CPbgFile();		// コンストラクタ
	~CPbgFile();	// デストラクタ

protected:
	HANDLE		m_hFile;			// ファイルハンドル

private:
	DWORD		m_DesiredAccess;	// ファイルアクセスのモード
};



} // namespace Pbg



#endif
