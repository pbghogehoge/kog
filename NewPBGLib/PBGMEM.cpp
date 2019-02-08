/*
 *   PbgMem.cpp   : メモリ管理
 *
 */

#include "PbgMem.h"		// メモリ管理
#include "PbgError.h"	// エラー処理



#ifndef PBG_DEBUG	// リリース版

/*
void * operator new (size_t Size)
{
	return (void *)LocalAlloc(LPTR, Size);
}

void * operator new[] (size_t Size)
{
	return (void *)LocalAlloc(LPTR, Size);
}

void operator delete(void *Memory)
{
	LocalFree(Memory);
}

void operator delete[] (void *Memory)
{
	LocalFree(Memory);
}
*/


#else	// デバッグ版



namespace PbgEx {
	CMemAllocInfo	Mem;
}


void * operator new   (size_t Size)
{
	return PbgEx::Mem.Allocate(Size, 0, "Unknown", "?????");
}


void * operator new[] (size_t Size)
{
	return PbgEx::Mem.Allocate(Size, 0, "Unknown", "?????");
}


void * operator new (size_t Size, DWORD Line, char *FileName, char *TypeName)
{
	return PbgEx::Mem.Allocate(Size, Line, FileName, TypeName);
}

void * operator new[] (size_t Size, DWORD Line, char *FileName, char *TypeName)
{
	return PbgEx::Mem.Allocate(Size, Line, FileName, TypeName);
}


void operator delete (void *Memory)
{
	PbgEx::Mem.Free(Memory);
}


void operator delete[] (void *Memory)
{
	PbgEx::Mem.Free(Memory);
}


void operator delete (void *Memory, DWORD Line, char *FileName, char *TypeName)
{
	PbgEx::Mem.Free(Memory);
}


void operator delete[] (void *Memory, DWORD Line, char *FileName, char *TypeName)
{
	PbgEx::Mem.Free(Memory);
}


// コンストラクタ //
CMemAllocInfo::CMemAllocInfo()
{
	m_NumAllocate     = 0;

	ZEROMEM(m_Head);
}


// デストラクタ //
CMemAllocInfo::~CMemAllocInfo()
{
	int		i;
	char	buf[300];

	// メモリが全て解放された、もしくは確保を行っていない //
	if(0 == m_NumAllocate){
		PbgDebugLog("\tCMemAllocInfo デストラクタ\r\n"
					"\t全メモリは正常に解放されています");
		return;
	}

//	if(TRUE == m_ErrorFlag) return;

	wsprintf(buf, "\tCMemAllocInfo デストラクタ\r\n"
				"\t%d 個のメモリリークを検出しました", m_NumAllocate);
	PbgError(buf);

	for(i=0; i<MIHEADER_MAX; i++){
		if(NULL != m_Head[i].m_Address){
			wsprintf(buf, "\t\t場所  : %-10s (%d行)"
							"\r\n\t\t対象  : %-10s (%u Bytes [%u])"
					,m_Head[i].m_FileName	// ファイル名記述
					,m_Head[i].m_Line		// 行数保持
					,m_Head[i].m_TypeName	// 型格納
					,m_Head[i].m_Size		// サイズ
					,m_Head[i].m_Address	// アドレス
				);

			PbgDebugLog(buf);
		}
	}
}


// メモリを確保する //
void *CMemAllocInfo::Allocate(DWORD Size, DWORD Line, char *FileName, char *TypeName)
{
	void	*temp;
	char	*p;
	int		i;

	// メモリを実際に確保 //
	temp = (void *)LocalAlloc(LPTR, Size);
	if(NULL == temp) return NULL;		// ダメでした

	// ファイル名(__FILE__ だから、一般にフルパス)のパスを削除する //
	if(FileName){
		for(p=FileName+strlen(FileName); p>FileName; p--){
			if('\\' == *p){
				p++;
				break;
			}
		}
	}
	else return temp;

	// ヘッダに格納できるかな //
	if(m_NumAllocate < MIHEADER_MAX){
		// 線形探索ぅ //
		for(i=0; i<MIHEADER_MAX; i++){
			// 未使用バッファを検索する //
			if(NULL == m_Head[i].m_Address){
				m_Head[i].m_Address  = temp;		// アドレス格納
				m_Head[i].m_Size     = Size;		// サイズ保持
				m_Head[i].m_FileName = p;			// ファイル名記述
				m_Head[i].m_TypeName = TypeName;	// 型記憶
				m_Head[i].m_Line     = Line;		// 行数

				m_NumAllocate++;

				break;
			}
		}
		if(MIHEADER_MAX == i)
			PbgError("CMemAllocInfo 内部エラー");
	}
	else{
		if(m_NumAllocate == MIHEADER_MAX)
			PbgError("\tエラーの個数が指定された数を超えましたので、\r\n"
						"\tこれ以降のメモリリークの検出は行われません。");
	}

	return temp;
}


// メモリを解放するに //
void CMemAllocInfo::Free(void *Addr)
{
	int			i;
//	char		buf[1024];
//	char		msg[1024];

/*
	wsprintf(buf, "Free     %p", Addr);
	PbgLog(buf);
*/
	if(NULL == Addr){
		PbgLog(	"\tCMemAllocInfo :: Free\r\n"
				"\t内部エラー : NULL による解放要求がありました");
/*
		if(NULL == m_pDeleteFileName){
			strcat(buf, "MemFree / DeleteEx 以外での解放です。場所の特定はできません");
			PbgLog(buf);
		}
		else{
			strcat(buf, "\r\n\t場所  : %-10s (%d行)");
			wsprintf(msg, buf, m_pDeleteFileName, m_DeleteLine);
			PbgLog(msg);
		}
*/
		return;
	}


/*	// その他の delete //
	if(NULL == m_pDeleteFileName){
//		strcpy(buf, "\tCMemAloocInfo :: Free\r\n"
//					"MemFree / DeleteEx 以外での解放が発生しました。");
//		PbgLog(buf);				// ログは吐くけど
		LocalFree((HLOCAL)Addr);	// 解放は行う
		return;
	}
*/

	for(i=0; i<MIHEADER_MAX; i++){
		// メモリが一致する項目を検出 //
		if(Addr == m_Head[i].m_Address){
/*
			wsprintf(buf, "\t\t場所  : %-10s (%d行)"
							"\r\n\t\t対象  : %-10s (%u Bytes [%u]) を解放しました"
					,m_Head[i].m_FileName	// ファイル名記述
					,m_Head[i].m_Line		// 行数保持
					,m_Head[i].m_TypeName	// 型格納
					,m_Head[i].m_Size		// サイズ
					,m_Head[i].m_Address	// アドレス
				);

			PbgDebugLog(buf);
*/

/*
			char buf2[1024];
			if(m_pDeleteFileName)
				wsprintf(buf2, "Free     %d : %s(%d) <- %s(%d)", m_NumAllocate, m_Head[i].m_FileName, m_Head[i].m_Line, m_pDeleteFileName, m_DeleteLine);
			else
				wsprintf(buf2, "Free     %d : %s(%d)", m_NumAllocate, m_Head[i].m_FileName, m_Head[i].m_Line);
				PbgLog(buf2);
*/
			m_Head[i].m_Address  = NULL;	// アドレス無効化
			m_Head[i].m_Size     = 0;		// サイズゼロ初期化
			m_Head[i].m_FileName = "";		// ファイル名未定
			m_Head[i].m_TypeName = "";		// 型不明
			m_Head[i].m_Line     = 0;		// 仮に０行とする

			m_NumAllocate--;			// 管理数を１つ減らす
			LocalFree((HLOCAL)Addr);	// では、解放を
			//m_pDeleteFileName = NULL;

			return;
		}
	}

	if(m_NumAllocate == 0){
		PbgLog(	"\tCMemAloocInfo :: Free\r\n"
				"\t解放できるデータはもうありませんが、解放を行おうとしました");
/*
		if(NULL == m_pDeleteFileName){
		//	strcat(buf, "MemFree / DeleteEx 以外での解放です。場所の特定はできません");
		//	PbgLog(buf);
		}
		else{
			strcat(buf, "\r\n\t場所  : %-10s (%d行)");
			wsprintf(msg, buf, m_pDeleteFileName, m_DeleteLine);
			PbgLog(msg);
		}
*/
		return;
	}
	else{
		LocalFree((HLOCAL)Addr);
		PbgLog("MemFree / DeleteEx 以外での解放です。場所の特定はできません");
	}
/*
	strcpy(buf,	"\tCMemAloocInfo :: Free\r\n"
				"\t不正なアドレスに対してメモリの解放を行おうとしました\r\n\t");

	strcat(buf, "場所  : %-10s (%d行)");
	wsprintf(msg, buf, m_pDeleteFileName, m_DeleteLine);
	PbgError(msg);

	m_pDeleteFileName = NULL;
	m_ErrorFlag       = TRUE;
*/
}



#endif // PBG_DEBUG
