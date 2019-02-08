/*
 *   PbgMain.h   : 一括インクルード
 *
 */

#ifndef PBGMAIN_INCLUDED
#define PBGMAIN_INCLUDED "一括インクルード : Version 0.01 : Update 2001/02/13"


/***** [ ヘッダファイル ] *****/
#include "PbgType.h"			// 型の宣言など
#include "PbgError.h"			// エラー処理
#include "PbgFile.h"			// ファイル管理(ＡＰＩ版)
#include "PbgMem.h"				// メモリ管理

#include "PbgMath.h"			// 数学関数
//#include "CRnd.h"				// 乱数発生
#include "PbgUtil.h"			// 汎用テンプレート

#include "LzUty\\LzEncode.h"	// 圧縮処理
#include "LzUty\\LzDecode.h"	// 解凍処理

#include "CCriticalSection.h"	// クリティカルセクション

#include "Midi\\PbgMidi.h"		// MIDI 管理

#include "DirectX\\DxUtil.h"				// ＤＸ用のちょっとしたマクロ集
#include "DirectX\\Sound\\PbgSound.h"		// ＤＳｏｕｎｄ管理クラス
#include "DirectX\\Input\\PbgInput.h"		// ＤＩｎｐｕｔ管理クラス
#include "DirectX\\Graphic\\PbgGraphic.h"	// ＤＤｒａｗ管理クラス
#include "DirectX\\Network\\PbgNetwork.h"	// ＤＰｌａｙ管理クラス

#include "PbgCache.h"			// キャッシュ管理



#endif
