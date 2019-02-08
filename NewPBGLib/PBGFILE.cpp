/*
 *   PbgFile.cpp   : 簡易File I/O(API 版)
 *
 */

#include "PbgFile.h"
#include "PbgMem.h"



namespace Pbg {



// コンストラクタ //
CPbgFile::CPbgFile()
{
	// 注意：ファイルハンドルの無効な値は NULL ではない //
	m_hFile         = INVALID_HANDLE_VALUE;	// ファイルハンドル
	m_DesiredAccess = 0;					// ファイルのオープンモード
}


// デストラクタ //
CPbgFile::~CPbgFile()
{
	// ファイルの閉じ忘れを防止する //
	Close();
}


// ファイルをオープンする //
BOOL CPbgFile::Open(const char *FileName, char *mode)
{
	char		*p;
	BOOL		IsAddMode = FALSE;
	DWORD		CreationDisposition;

	// まず、クローズしましょう //
	Close();

	// 一応 fopen() 互換の動作を行う //
	for(p=mode; (*p)!='\0'; p++){
		if(*p == 'r'){
			m_DesiredAccess     = GENERIC_READ;		// 読みこみモード
			CreationDisposition = OPEN_EXISTING;	// 新規作成しない
			break;
		}
		if(*p == 'w'){
			DeleteFile(FileName);	// ファイルが存在するなら消し去る
			m_DesiredAccess     = GENERIC_WRITE;	// 書き込みモード
			CreationDisposition = OPEN_ALWAYS;		// 無くても作成
			break;
		}
		if(*p == 'a'){
			IsAddMode = TRUE;	// 追加モード
			m_DesiredAccess     = GENERIC_WRITE;	// 書き込みモード
			CreationDisposition = OPEN_ALWAYS;		// 無くても作成
			break;
		}
	}
	if(*p == '\0') return FALSE;

	// 他のハンドル系関数と違って、失敗した場合はファイルハンドルとして //
	// NULL ではなく INVALID_HANDLE_VALUE を返すものとする              //
	m_hFile = CreateFile(FileName, m_DesiredAccess, FILE_SHARE_READ, NULL,
							CreationDisposition,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(INVALID_HANDLE_VALUE == m_hFile) return FALSE;

	if(IsAddMode){	// 追加モードの場合
		// 重要：追加モードなので、ファイルポインタを最後にもってくる //
		SetFilePointer(m_hFile, 0, NULL, FILE_END);
	}

	// 正常にオープンできました //
	return TRUE;
}


// ファイルをクローズする //
void CPbgFile::Close(void)
{
	// ファイルがオープンされている場合は //
	if(INVALID_HANDLE_VALUE != m_hFile){
		CloseHandle(m_hFile);						// ファイルを閉じる
		m_hFile         = INVALID_HANDLE_VALUE;		// ファイルハンドルの無効化
		m_DesiredAccess = 0;						// これも無効化
	}
}


// いわゆる fread //
BOOL CPbgFile::Read(void *Buffer, DWORD Size, DWORD *ReadSize)
{
	// 読み込みモードで無ければ失敗 //
	if(GENERIC_READ != m_DesiredAccess) return FALSE;

	// 読み込み //
	return ReadFile(m_hFile, Buffer, Size, ReadSize, NULL);
}


// いわゆる fwrite //
BOOL CPbgFile::Write(void *Buffer, DWORD Size, DWORD *WriteSize)
{
	// 書き込みモードで無ければ失敗 //
	if(GENERIC_WRITE != m_DesiredAccess) return FALSE;

	// 書き込み //
	return WriteFile(m_hFile, Buffer, Size, WriteSize, NULL);
}


// いわゆる fgetc //
int CPbgFile::GetChar(void)
{
	BYTE	temp;
	DWORD	size;

	// １バイトだけ読み込む //
	if(FALSE == Read(&temp, 1, &size)) return EOF;

	if(0 == size) return EOF;	// ファイルの終端
	else          return temp;
}


// いわゆる fputc //
int CPbgFile::PutChar(int Data)
{
	BYTE	temp;
	DWORD	size;

	temp = Data;	// １バイトのデータに変換

	// １バイトだけ書き込む //
	if(FALSE == Write(&temp, 1, &size)) return EOF;

	if(0 == size) return EOF;
	else          return Data;
}


// いわゆる fseek //
BOOL CPbgFile::Seek(DWORD Offset, DWORD Method)
{
	// ポインタが無効 //
	if(INVALID_HANDLE_VALUE == m_hFile) return FALSE;

	// ファイルポインタを移動する(２ＧＢ未満のファイルのみ対応) //
	SetFilePointer(m_hFile, Offset, NULL, Method);

	return TRUE;
}


// いわゆる ftell(現在位置) //
DWORD CPbgFile::GetPosition(void)
{
	// ポインタが無効 //
	if(INVALID_HANDLE_VALUE == m_hFile) return 0;

	// 移動しないと、現在の位置を調べられる //
	return SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
}


// いわゆる ftell(サイズ) //
DWORD CPbgFile::GetSize(void)
{
	// ポインタが無効 //
	if(INVALID_HANDLE_VALUE == m_hFile) return 0;

	return GetFileSize(m_hFile, NULL);
}


// 文字列を書き込む //
BOOL CPbgFile::WriteStr(void *Buffer)
{
	DWORD		Length;
	DWORD		temp;

	// 書き込みモードのみ有効 //
	if(GENERIC_WRITE != m_DesiredAccess) return FALSE;

	Length = strlen((char *)Buffer);		// 文字列長を求めて
	return Write(Buffer, Length, &temp);	// 書き込む
}


// メモリ上にデータを展開する Uses MemAlloc() //
// arg : MaxSize 許される最大サイズ           //
// ret : 失敗なら NULL                        //
LPVOID CPbgFile::GetData(DWORD MaxSize)
{
	DWORD		pos, size, rsize;
	void		*pdata;

	// 読みA込みモードのみ有効 //
	if(GENERIC_READ != m_DesiredAccess) return NULL;

	// ファイルサイズの取得＆チェック //
	size = GetSize();				// ファイルのサイズを取得
	if(size > MaxSize) return NULL;	// ファイルが大きすぎる

	// 展開先のメモリを確保する //
	pdata = MemAlloc(size);			// メモリを確保
	if(NULL == pdata) return NULL;	// メモリが足りない

	// 現在位置を記憶する //
	pos = GetPosition();

	// ファイルの先頭に移動 //
	if(FALSE == Seek(pos, FILE_BEGIN)) return NULL;

	// 読み込み //
	if(FALSE == Read(pdata, size, &rsize)){
		MemFree(pdata);
		return FALSE;
	}

	// ファイルポインタを元に戻す //
	Seek(pos, FILE_BEGIN);

	return pdata;
}



} // namespace Pbg
