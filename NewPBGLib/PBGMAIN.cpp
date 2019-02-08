/*
 *   PbgMain.cpp   : 一括インクルード
 *
 */

#include "PbgMain.h"



// エラー処理は名前空間 PbgEx に含まれる //
namespace PbgEx {
	CErrorCtrl		PbgEx::CError;
}



#ifdef PBG_DEBUG
	// デバッグモードの場合 //
	#pragma comment(lib, "PbgLib___Win32_Release_WithPBG_DEBUG\\PbgLib.lib")
	#include "PbgMem.cpp"
#else
	// リリースモードの場合 //
	#pragma comment(lib, "Release\\PbgLib.lib")
#endif
