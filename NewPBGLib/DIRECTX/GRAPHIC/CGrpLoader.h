/*
 *   CGrpLoader.h   : ファイル読み込み
 *
 */

#ifndef CGRPLOADER_INCLUDED
#define CGRPLOADER_INCLUDED "ファイル読み込み : Version 0.01 : Update 2001/10/07"

/*  [更新履歴]
 *    Version 0.01 : 2001/10/07 : 制作開始
 */



#include "CDIB32.h"
#include "PbgCache.h"



namespace Pbg {



/***** [クラス定義] *****/

// 画像ファイル読み込み用クラス //
class CGrpLoader : public CPbgCache {
public:
	// ファイルから読み込む
	//
	// 参考   : ファイルのキャッシュ対象となるのは、 SetCache() を行っている
	//        : リソース以外のファイルである
	//
	// 引数   : pDib       / 画像管理クラス
	//        : pFileName  / ファイル名
	//        : pFileID    / 圧ファイル用識別ＩＤ
	//        : FileNo     / ファイル番号
	//
	// 戻り値 : 成功なら TRUE
	//
	static BOOL Load(CDIB32 *pDib, char *pFileName);					// 通常ファイル
	static BOOL LoadR(CDIB32 *pDib, char *pFileName);					// りそ
	static BOOL LoadP(CDIB32 *pDib, char *pFileName, char *pFileID);	// 圧ファイル
	static BOOL LoadP(CDIB32 *pDib, char *pFileName, DWORD FileNo);		// 圧ファイル
	static BOOL LoadPR(CDIB32 *pDib, char *pFileName, char *pFileID);	// りそ圧
	static BOOL LoadPR(CDIB32 *pDib, char *pFileName, DWORD FileNo);	// りそ圧

	// 画像のαチャンネルを使用して、 col べた塗り画像と合成する //
	static void AlphaBlend(CDIB32 *pDib, RGBQUAD &col);


private:
	// メモリから読み込む //
	static BOOL LoadFromMemory(CDIB32 *pDib, HGLOBAL hData, DWORD size);

	// LoadFromMemory() 補助用関数 //
	static BOOL LoadBmpFile(CDIB32 *pDib, HGLOBAL hData, DWORD size);	// BMP
	static BOOL LoadTargaFile(CDIB32 *pDib, HGLOBAL hData, DWORD size);	// TGA
	static BOOL LoadOemFile(CDIB32 *pDib, HGLOBAL hData, DWORD size);	// JPG,GIF
};



} // namespace Pbg



#endif
