/*
 *   PbgMem.h   : メモリ管理
 *
 */

#ifndef PBGMEM_INCLUDED
#define PBGMEM_INCLUDED "メモリ管理   : Version 0.04 : Update 2001/03/11"

/*  [使用上の注意]
 *    new 演算子をオーバーロードするようなライブラリを使用する場合は
 *    このファイルのインクルートおよび PbgMem.obj とのリンクを行わない事
 */

/*  [更新履歴]
 *    Version 0.04 : 2001/03/11 : 一部のエラーメッセージをコメントアウト
 *    Version 0.03 : 2001/01/16 : 再構成・一応落ち着く。
 *    Version 0.02 : 2000/10/10 : クラスのメンバ名を変更(m_XXX)
 *    Version 0.01 : 2000/10/03 : 基本部分を設計
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
#define MIHEADER_MAX		2000



/***** [マクロ・関数 ] *****/

// リリース版 //
#ifndef PBG_DEBUG
	// メモリを確保する //
	#define MemAlloc(size)	LocalAlloc(LPTR, size)

	// メモリを解放する //
	#define MemFree(ptr)		\
		if(NULL != ptr){		\
			LocalFree(ptr);		\
			ptr = NULL;			\
		}						\

	// ログ付き new //
	#define NewEx(x)	new x

	// ポインタ無効化付き delete //
	#define DeleteEx(x)  {if(x) {delete    (x);  (x) = NULL; } }

	// ポインタ無効化付き delete [配列版] //
	#define DeleteArray(x) {if(x) {delete [] (x);  (x) = NULL; } }
/*
	void * operator new   (size_t Size);
	void * operator new[] (size_t Size);
	void operator delete   (void *Memory);
	void operator delete[] (void *Memory);
*/

// デバッグ版 //
#else
	#include "PbgError.h"

	// メモリを確保する //
	#define MemAlloc(size)	\
		PbgEx::Mem.Allocate(size, __LINE__, __FILE__, "*MemAlloc*");

	// メモリを解放する(これもSAFE_FREE) //
	#define MemFree(ptr)									\
		if(NULL != ptr){									\
			PbgEx::Mem.Free(ptr);							\
			ptr = NULL;										\
		}

	// ログ付き new //
	#define NewEx(x)	new(__LINE__, __FILE__, #x) x;

	// ポインタ無効化付き delete //
	#define DeleteEx(x) {									\
		if(NULL != x){										\
			delete (x);										\
			(x) = NULL;										\
		}													\
	}

	// ポインタ無効化付き delete [配列版] //
	#define DeleteArray(x) {								\
		if(NULL != x){										\
			delete[](x);									\
			(x) = NULL;										\
		}													\
	}

	void * operator new    (size_t Size);
	void * operator new[]  (size_t Size);
	void * operator new    (size_t Size, DWORD Line, char *FileName, char *TypeName);
	void * operator new[]  (size_t Size, DWORD Line, char *FileName, char *TypeName);
	void operator delete   (void *Memory);
	void operator delete[] (void *Memory);
	void operator delete   (void *Memory, DWORD Line, char *FileName, char *TypeName);
	void operator delete[] (void *Memory, DWORD Line, char *FileName, char *TypeName);



/***** [構造体・クラス] *****/

// メモリ管理用ヘッダ //
typedef struct tagMemoryInfoHeader {
	void	*m_Address;		// アドレス
	DWORD	m_Size;			// 確保サイズ

	char	*m_FileName;	// ファイル名
	char	*m_TypeName;	// (存在すれば)変数名
	DWORD	m_Line;			// 行番号
} MemoryInfoHeader;


// メモリ確保用クラス //
class CMemAllocInfo {
public:
	void *Allocate(DWORD Size, DWORD Line, char *FileName, char *TypeName);
	void  Free(void *Addr);

	CMemAllocInfo();		// コンストラクタ
	~CMemAllocInfo();		// デストラクタ

private:
	MemoryInfoHeader		m_Head[MIHEADER_MAX];
	DWORD					m_NumAllocate;
};



/***** [グローバル変数] *****/
namespace PbgEx {
	extern CMemAllocInfo	Mem;
}



#endif // PBG_DEBUG
#endif // PBGMEM_INCLUDED
