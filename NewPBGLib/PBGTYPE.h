/*
 *   PbgType.h   : 汎用マクロ＆型の定義
 *
 */

#ifndef PBGTYPE_INCLUDED
#define PBGTYPE_INCLUDED "汎用マクロ＆型の定義 : Version 0.03 : Update 2001/02/24"

/*  [更新履歴]
 *    Version 0.03 : 2001/01/24 : DirectX 関連の型を追加
 *    Version 0.02 : 2001/01/18 : ヘッダ、ライブラリの追加
 */



#define D3D_OVERLOADS



/***** [ ヘッダファイル ] *****/
#include <windows.h>

#include <mmsystem.h>
#include <mmreg.h>

#include <ddraw.h>		// DirectDraw
#include <d3d.h>		// Direct3D
#include <dsound.h>		// DirectSound
#include <dinput.h>		// DirectInput

#include <dplay.h>		// DirectPlay
#include <dplobby.h>	// DirectPlayLobby



/***** [ ライブラリ ] *****/
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "dxguid.lib")		// ＧＵＩＤの類
#pragma comment(lib, "ddraw.lib")		// DirectDraw
#pragma comment(lib, "d3dim.lib")		// Direct3D_IM
#pragma comment(lib, "dsound.lib")		// DirectSound
#pragma comment(lib, "dinput.lib")		// DirectInput



/***** [ 定数 ] *****/
#ifndef EOF
	#define EOF		(-1)		// ファイル終端とか、エラーとか
#endif



/***** [ 型 ] *****/

// ポインタ用の型(名前空間で、一応保護する) //
namespace PbgEx {
	typedef void  *     ptrvoid;	// void へのポインタ
	typedef BOOL  *     ptrBOOL;	// BOOL へのポインタ

	typedef BYTE  *     ptrBYTE;	// BYTE  へのポインタ
	typedef WORD  *     ptrWORD;	// WORD  へのポインタ
	typedef DWORD *     ptrDWORD;	// DWORD へのポインタ

	typedef int   *     ptrint;		// int   へのポインタ
	typedef long  *     ptrlong;	// long  へのポインタ
	typedef short *     ptrshort;	// short へのポインタ
	typedef char  *     ptrchar;	// char  へのポインタ
}

// fastcall 系の呼び出し規約 //
#define FVOID   void  __fastcall
#define FBOOL   BOOL  __fastcall
#define FINT    int   __fastcall
#define FLONG   long  __fastcall
#define FSHORT  short __fastcall
#define FCHAR   char  __fastcall
#define FBYTE   BYTE  __fastcall
#define FWORD   WORD  __fastcall
#define FDWORD  DWORD __fastcall

// fastcall(ポインタ) 系の呼び出し規約 //
#define FPVOID  PbgTypeEx::ptrvoid  __fastcall
#define FPBOOL  PbgTypeEx::ptrBOOL  __fastcall
#define FPINT   PbgTypeEx::ptrint   __fastcall
#define FPLONG  PbgTypeEx::ptrlong  __fastcall
#define FPSHORT PbgTypeEx::ptrshort __fastcall
#define FPCHAR  PbgTypeEx::ptrchar  __fastcall
#define FPBYTE  PbgTypeEx::ptrBYTE  __fastcall
#define FPWORD  PbgTypeEx::ptrWORD  __fastcall
#define FPDWORD PbgTypeEx::ptrDWORD __fastcall

// inline 系の呼び出し規約 //
#define IVOID   __inline void
#define IBOOL   __inline BOOL
#define IINT    __inline int
#define ILONG   __inline long
#define ISHORT  __inline short
#define ICHAR   __inline char
#define IBYTE   __inline BYTE
#define IWORD   __inline WORD
#define IDWORD  __inline DWORD

// inline(ポインタ) 系の呼び出し規約 //
#define IPVOID  __inline PbgTypeEx::ptrvoid
#define IPBOOL  __inline PbgTypeEx::ptrBOOL
#define IPINT   __inline PbgTypeEx::ptrint
#define IPLONG  __inline PbgTypeEx::ptrlong
#define IPSHORT __inline PbgTypeEx::ptrshort
#define IPCHAR  __inline PbgTypeEx::ptrchar
#define IPBYTE  __inline PbgTypeEx::ptrBYTE
#define IPWORD  __inline PbgTypeEx::ptrWORD
#define IPDWORD __inline PbgTypeEx::ptrDWORD

// fastcall 系の関数ポインタ呼び出し規約 //
#define FVOIDFN(fn)  void  (__fastcall fn)
#define FBOOLFN(fn)  BOOL  (__fastcall fn)
#define FINTFN(fn)   int   (__fastcall fn)
#define FLONGFN(fn)  long  (__fastcall fn)
#define FSHORTFN(fn) short (__fastcall fn)
#define FCHARFN(fn)  char  (__fastcall fn)
#define FBYTEFN(fn)  BYTE  (__fastcall fn)
#define FWORDFN(fn)  WORD  (__fastcall fn)
#define FDWORDFN(fn) DWORD (__fastcall fn)

// fastcall(ポインタ) 系の関数ポインタ呼び出し規約 //
#define FPVOIDFN(fn)  PbgTypeEx::ptrvoid  (__fastcall fn).
#define FPBOOLFN(fn)  PbgTypeEx::ptrBOOL  (__fastcall fn)
#define FPINTFN(fn)   PbgTypeEx::ptrint   (__fastcall fn)
#define FPLONGFN(fn)  PbgTypeEx::ptrlong  (__fastcall fn)
#define FPSHORTFN(fn) PbgTypeEx::ptrshort (__fastcall fn)
#define FPCHARFN(fn)  PbgTypeEx::ptrchar  (__fastcall fn)
#define FPBYTEFN(fn)  PbgTypeEx::ptrBYTE  (__fastcall fn)
#define FPWORDFN(fn)  PbgTypeEx::ptrWORD  (__fastcall fn)
#define FPDWORDFN(fn) PbgTypeEx::ptrDWORD (__fastcall fn)

// DirectX 用 //
typedef LPDIRECTDRAWSURFACE7	LPSURFACE;					// 簡潔な表現を
typedef BOOL (*GrpConfirmFunc)(DDCAPS *, D3DDEVICEDESC7 *);	// デバイス選別関数



/***** [ マクロ ] *****/

// ４バイト作成 //
#define MAKE_DWORD(a,b,c,d)	((DWORD)(MAKEWORD(c,d)<<16)|(DWORD)MAKEWORD(a,b))
#define MAKE_INV_DWORD(a,b,c,d)		MAKE_DWORD(d,c,b,a)

// ２バイト作成(反転) //
#define MAKE_INV_WORD(a, b)			MAKE_WORD(b, a)


// ポインタ無効化付き ->Release() //
#define SAFE_RELEASE(x) {if(x) {(x)->Release();  (x) = NULL; } }

// メモリをゼロ初期化する //
#define ZEROMEM(x)      memset(&x, 0, sizeof(x))



#endif
