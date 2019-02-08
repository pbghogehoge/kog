/*
 *   CGrpLoader.cpp   : ファイル読み込み
 *
 */

#include "CGrpLoader.h"
#include "PbgFile.h"
#include "PbgMem.h"
#include "LZUTY\\LzDecode.h"
#include "PbgError.h"
#include <olectl.h>



namespace Pbg {



// ファイルから読み込む(通常ファイル) //
BOOL CGrpLoader::Load(CDIB32 *pDib, char *pFileName)
{
	CPbgFile		file;
	HGLOBAL			hData;
	DWORD			size, rsize;
	BOOL			ret;

	// 通常のファイルを開く //
	if(FALSE == file.Open(pFileName, "r")){
		return FALSE;
	}

	// ファイルサイズを取得し、必要なメモリを確保する //
	size  = file.GetSize();				// サイズ取得
	hData = (HGLOBAL)MemAlloc(size);	// メモリ確保
	if(NULL == hData) return FALSE;		// メモリが足りん

	// ファイルからメモリへと //
	file.Read(hData, size, &rsize);

	// メモリから読み込む(ファイルの種類は自動判別で) //
	ret = LoadFromMemory(pDib, hData, size);

	MemFree(hData);

	return ret;
}


// ファイルから読み込む(りそ) //
BOOL CGrpLoader::LoadR(CDIB32 *pDib, char *pFileName)
{
//	CBitMemIn		*pMem;
	DWORD			DataSize;
	VOID			*pData;
	HGLOBAL			hMem;
	HRSRC			hRes;
	BOOL			ret;
	HMODULE			hModule;


	hModule = NULL;//GetModuleHandle(NULL);

	// まずは、与えられた名前をリソース内から探し出す //
	hRes = FindResource(hModule, pFileName, RT_RCDATA);
	if(NULL == hRes){
		PbgError("FindResource Failed.");
		PbgError(pFileName);
		return FALSE;
	}

	// リソースをロードする //
	hMem = LoadResource(hModule, hRes);
	if(NULL == hMem){
		PbgError("LoadResource Failed.");
		return FALSE;
	}

	// リソースをロックする //
	pData = LockResource(hMem);
	if(NULL == pData){
		PbgError("LockResource Failed.");
		FreeResource(pData);
		return FALSE;
	}

	DataSize = SizeofResource(hModule, hRes);	// リソースのサイズを調べて
/*	m_pResData = MemAlloc(DataSize);			// そのサイズだけメモリを確保
	if(NULL == m_pResData){
		PbgError("MemAlloc Failed.");
		FreeResource(hMem);
		return FALSE;
	}
*/
	ret = LoadFromMemory(pDib, pData, DataSize);
	if(FALSE == ret){
		char temp[1024];
		wsprintf(temp, "size = %d", DataSize);
		PbgError(temp);
		PbgError("LoadFromMemory Failed.");
	}
	FreeResource(hMem);
	return ret;
}


// ファイルから読み込む(圧ファイル) //
BOOL CGrpLoader::LoadP(CDIB32 *pDib, char *pFileName, char *pFileID)
{
	LzDecode		dec;
	HGLOBAL			hData;
	DWORD			size;
	BOOL			ret;

	// キャッシュに存在するかを調べる //
	hData = (HGLOBAL)Search(pFileName, pFileID, &size);
	if(hData){	// ヒットした場合
		return LoadFromMemory(pDib, hData, size);
	}

	// ファイルのデコーダに問い合わせる //
	if(FALSE == dec.Open(pFileName)){
		return FALSE;
	}

	// 解凍を開始する //
	hData = (HGLOBAL)dec.Decode(pFileID);
	if(NULL == hData) return FALSE;

	// メモリからの解凍 //
	ret = LoadFromMemory(pDib, hData, dec.GetFileSize(pFileID));

	MemFree(hData);

	return ret;
}


// ファイルから読み込む(圧ファイル) //
BOOL CGrpLoader::LoadP(CDIB32 *pDib, char *pFileName, DWORD FileNo)
{
	LzDecode		dec;
	HGLOBAL			hData;
	DWORD			size;
	BOOL			ret;

	// キャッシュに存在するかを調べる //
	hData = (HGLOBAL)Search(pFileName, FileNo, &size);
	if(hData){	// ヒットした場合
		return LoadFromMemory(pDib, hData, size);
	}

	// ファイルのデコーダに問い合わせる //
	if(FALSE == dec.Open(pFileName)){
		return FALSE;	// 開けない
	}

	// 解凍を開始する //
	hData = (HGLOBAL)dec.Decode(FileNo);
	if(NULL == hData) return FALSE;

	// メモリからの解凍 //
	ret = LoadFromMemory(pDib, hData, dec.GetFileSize(FileNo));

	MemFree(hData);

	return ret;
}


// ファイルから読み込む(りそ圧) //
BOOL CGrpLoader::LoadPR(CDIB32 *pDib, char *pFileName, char *pFileID)
{
	LzDecode		dec;
	HGLOBAL			hData;
	DWORD			size;
	BOOL			ret;

	// 圧縮済みファイルをオープンする //
	if(FALSE == dec.OpenR(NULL, pFileName)){
		return FALSE;	// ファイルが開けない
	}

	// 実際に解凍する //
	hData = dec.Decode(pFileID);		// 解凍
	if(NULL == hData) return FALSE;		// ファイルにゃい

	size = dec.GetFileSize(pFileID);			// ファイルサイズを調べる
	ret  = LoadFromMemory(pDib, hData, size);	// メモリから読み込み
	MemFree(hData);								// 一時メモリを解放する

	return ret;
}


// ファイルから読み込む(りそ圧) //
BOOL CGrpLoader::LoadPR(CDIB32 *pDib, char *pFileName, DWORD FileNo)
{
	LzDecode		dec;
	HGLOBAL			hData;
	DWORD			size;
	BOOL			ret;

	// 圧縮済みファイルをオープンする //
	if(FALSE == dec.OpenR(NULL, pFileName)){
		return FALSE;	// ファイルが開けない
	}

	// 実際に解凍する //
	hData = dec.Decode(FileNo);			// 解凍
	if(NULL == hData) return FALSE;

	size = dec.GetFileSize(FileNo);			// ファイルサイズを調べる
	ret = LoadFromMemory(pDib, hData, size);	// メモリから読み込み
	MemFree(hData);								// 一時メモリを解放する

	return ret;
}


// 画像のαチャンネルを使用して、 col べた塗り画像と合成する //
void CGrpLoader::AlphaBlend(CDIB32 *pDib, RGBQUAD &col)
{
	int			i, size;
	DWORD		r, g, b, a, a2;
	RGBQUAD		*pSrc;

	if(FALSE == pDib->HasAlpha()) return;

	size = pDib->GetWidth() * pDib->GetHeight();
	pSrc = (RGBQUAD *)pDib->GetTarget();

	for(i=0; i<size; i++, pSrc++){
		a  = pSrc->rgbReserved;
		a2 = 255 - a;

		r  = min(255, ((pSrc->rgbRed   * a) + (col.rgbRed   * a2)) / 255);
		g  = min(255, ((pSrc->rgbGreen * a) + (col.rgbGreen * a2)) / 255);
		b  = min(255, ((pSrc->rgbBlue  * a) + (col.rgbBlue  * a2)) / 255);

		pSrc->rgbRed   = (BYTE)r;
		pSrc->rgbGreen = (BYTE)g;
		pSrc->rgbBlue  = (BYTE)b;
//		pSrc->rgbReserved = a;
	}

	pDib->SetAlpha(FALSE);
}


// メモリから読み込む //
BOOL CGrpLoader::LoadFromMemory(CDIB32 *pDib, HGLOBAL hData, DWORD size)
{
	// この部分に、通常のＢＭＰローダを追加 //

	if(TRUE == LoadBmpFile(pDib, hData, size))		return TRUE;
	if(TRUE == LoadTargaFile(pDib, hData, size))	return TRUE;
	if(TRUE == LoadOemFile(pDib, hData, size))		return TRUE;

	// ここまで来てダメなら、無理です //
	return FALSE;
}


// メモリから読み込む(BMP) //
BOOL CGrpLoader::LoadBmpFile(CDIB32 *pDib, HGLOBAL hData, DWORD size)
{
	BITMAPFILEHEADER		*pFileHeader;
	BITMAPINFOHEADER		*pInfoHeader;
	RGBQUAD					*pPalette;
	BYTE					*pBitData;

	int						width,height;
//	HDC						hMemoryDC;
//	HBITMAP					hMemoryBMP;
//	HGDIOBJ					hOldObject;
	BOOL					ret;

	PbgErrorInit("\tCGrpLoader::LoadBmpFile()");

	// ポインタが不正 //
	if(NULL == hData){
		PbgErrorEx("内部エラー：ポインタが不正です");
		return FALSE;
	}

	// ＤＩＢ作成の為に、ポインタを初期化する //
	pFileHeader = (BITMAPFILEHEADER *)hData;				// FILEHEADER
	pInfoHeader = (BITMAPINFOHEADER *)((pFileHeader) + 1);	// INFOHEADER
	pPalette    = (RGBQUAD          *)((pInfoHeader) + 1);	// RGBQUAD[256]

	// ファイルの先頭は [ＢＭ] ね //
	if(MAKEWORD('B','M') != pFileHeader->bfType){
//		PbgErrorEx("ファイルのヘッダが不正です");
		return FALSE;
	}

	// 読み込み元のＢＭＰの幅と高さを求める //
	width  = (pInfoHeader->biWidth+3)&(~3);		// ＤＷＯＲＤ境界にする
	height =  pInfoHeader->biHeight;			// 高さはそのまま

	// データ列の開始位置を求める //
	pBitData = ((BYTE *)hData) + pFileHeader->bfOffBits;

	// 作成されていない場合は、作成する //
	if(NULL == pDib->GetTarget()){
		if(FALSE == pDib->Create(width, height)){
			return FALSE;
		}
	}

	ret = SetDIBitsToDevice(
					pDib->GetDC()		// 転送先のデバイスコンテキスト
				,	0, 0, width, height	// 転送先の右上の座標 - 幅 & 高さ
				,	0, 0				// 転送元の左下座標
				,	0, height			// 転送開始行、転送行数
				,	pBitData					// ビット列へのポインタ
				,	(BITMAPINFO *)pInfoHeader	// BITMAPINFO へのポインタ
				,	DIB_RGB_COLORS);			// ＲＧＢ値そのもので
	if(0 == ret) return FALSE;

	return TRUE;
}


// メモリから読み込む(BMP/GIF/JPG) //
BOOL CGrpLoader::LoadOemFile(CDIB32 *pDib, HGLOBAL hData, DWORD size)
{
	IPicture		*pPic = NULL;
	IStream			*pIs  = NULL;
	HRESULT			hr;

	if(NULL == pDib) return FALSE;

	hr = CreateStreamOnHGlobal(hData, FALSE, &pIs);
	if(FAILED(hr)){
		return FALSE;
	}

	hr = OleLoadPicture(pIs, size, TRUE, IID_IPicture, (LPVOID *)&pPic);
	pIs->Release();
	if(FAILED(hr)){
		return FALSE;
	}

	HDC		hdc;
	LONG	w, h, sx, sy;

	hdc = GetDC(NULL);

	pPic->get_Width(&w);
	pPic->get_Height(&h);
	sx = MulDiv(w, GetDeviceCaps(hdc, LOGPIXELSX), 2540);
	sy = MulDiv(h, GetDeviceCaps(hdc, LOGPIXELSY), 2540);

	ReleaseDC(NULL, hdc);

	// 作成されていない場合は、作成する //
	if(NULL == pDib->GetTarget()){
		if(FALSE == pDib->Create(sx, sy)){
			pPic->Release();
			return FALSE;
		}
	}

	hr = pPic->Render(pDib->GetDC(), 0, 0, sx, sy, 0, h, w, -h, NULL);

	pPic->Release();

	if(FAILED(hr)){
		return FALSE;
	}

//	PbgError("OEM File (GIF/JPEG) Loaded.");
	return TRUE;
}


// メモリから読み込む(TGA) //
BOOL CGrpLoader::LoadTargaFile(CDIB32 *pDib, HGLOBAL hData, DWORD size)
{
	typedef struct tagTargaHeader {
		BYTE	IDLength;
		BYTE	ColormapType;
		BYTE	ImageType;
		BYTE	ColormapSpecification[5];
		WORD	XOrigin;
		WORD	YOrigin;
		WORD	ImageWidth;
		WORD	ImageHeight;
		BYTE	PixelDepth;
		BYTE	ImageDescriptor;
	} TargaHeader;

	TargaHeader		tga;
	DWORD			x, y, offset, w, h;
	BYTE			r, g, b, a;
	BYTE			*pData;
	RGBQUAD			*pImage;

	BYTE			info;
	WORD			type, count;


	pData = (BYTE *)hData;

	if(NULL == pData) return FALSE;

	// ヘッダを読み込む //
	memcpy(&tga, pData, sizeof(TargaHeader));
	pData += sizeof(TargaHeader);

	// そのファイル形式をサポートしているかをチェックする //
	if((0 != tga.ColormapType)
		|| (tga.ImageType != 10 &&  tga.ImageType != 2)){
//		if(tga.ImageType == 10){
//			PbgError("*.tga パケット形式には対応していません");
//		}
//		else{
//			PbgError("サポートされていない *.tga ファイルです");
//		}

		return FALSE;
	}

	switch(tga.PixelDepth){
		// 24,32 Bit のみ対応する
		case 24:	break;
		case 32:	break;

		default:				return FALSE;	// それ以外は不可
	}

	// ID フィールドの読み飛ばしを行う //
	pData += tga.IDLength;

	// 無条件に作成を行う //
	if(FALSE == pDib->Create(tga.ImageWidth, tga.ImageHeight)){
		return FALSE;
	}

	w      = tga.ImageWidth;
	h      = tga.ImageHeight;
	pImage = (RGBQUAD *)pDib->GetTarget();

	if(tga.ImageType == 10){
//#ifdef PBG_DEBUG
//		PbgLog("Targa PacketMode(10)");
//#endif
		for(y=0; y<h; y++){
			// 上下反転がある場合は、逆方向に読み込む //
			if(tga.ImageDescriptor & 0x0010) offset = y * w;
			else                             offset = (h - y - 1) * w;

			for(x=0; x<w; ){
				info  = *pData;		pData++;
				type  = (0x80 & info);
				count = (0x7f & info) + 1;

				if(type){
					b = *pData;		pData++;
					g = *pData;		pData++;
					r = *pData;		pData++;

					if(tga.PixelDepth == 32){
						a = *pData;		pData++;
					}
					else{
						a = 0xff;
					}

					while(count--){
						pImage[offset + x].rgbRed      = r;
						pImage[offset + x].rgbGreen    = g;
						pImage[offset + x].rgbBlue     = b;
						pImage[offset + x].rgbReserved = a;
						x++;
					}
				}
				else{
					while(count--){
						pImage[offset + x].rgbBlue     = *pData;	pData++;
						pImage[offset + x].rgbGreen    = *pData;	pData++;
						pImage[offset + x].rgbRed      = *pData;	pData++;

						if(tga.PixelDepth == 32){
							pImage[offset + x].rgbReserved = *pData;	pData++;
						}
						else{
							pImage[offset + x].rgbReserved = 0xff;
						}

						x++;
					}
				}
			}
		}
	}
	else{
//#ifdef PBG_DEBUG
//		PbgLog("Targa StdMode(2)");
//#endif
		for(y=0; y<h; y++){
			// 上下反転がある場合は、逆方向に読み込む //
			if(tga.ImageDescriptor & 0x0010) offset = y * w;
			else                             offset = (h - y - 1) * w;

			for(x=0; x<w; x++){
				b = *pData;		pData++;
				g = *pData;		pData++;
				r = *pData;		pData++;

				if(tga.PixelDepth == 32){
					a = *pData;		pData++;
				}
				else{
					a = 0xff;
				}

				pImage[offset + x].rgbRed      = r;
				pImage[offset + x].rgbGreen    = g;
				pImage[offset + x].rgbBlue     = b;
				pImage[offset + x].rgbReserved = a;
			}
		}
	}

	if(tga.PixelDepth == 32){
		pDib->SetAlpha(TRUE);
	}

//	PbgError("Targa File Loaded.");
	return TRUE;
}



} // namespace Pbg
