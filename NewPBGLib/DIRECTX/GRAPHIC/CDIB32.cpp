/*
 *   CDIB32.cpp   : RGBA画像管理
 *
 */

#include "CDIB32.h"
#include "PbgFile.h"
#include "PbgMem.h"
#include "PbgError.h"
#include "LzUty\\LzDecode.h"
#include "PbgCache.h"



namespace Pbg {



// コンストラクタ //
CDIB32::CDIB32()
{
	m_hBMP    = NULL;	// ビットマップハンドル
	m_hDC     = NULL;	// デバイスコンテキスト
	m_hOldObj = NULL;	// 以前に関連づけされていたオブジェクト

	m_dwWidth  = 0;		// 画像の幅
	m_dwHeight = 0;		// 画像の高さ
	m_lPitch   = 0;		// 画像のピッチ

	m_pData     = NULL;		// 画像のビットデータ列
	m_bHasAlpha = FALSE;	// α値を持っていれば真
}


// デストラクタ //
CDIB32::~CDIB32()
{
	Cleanup();
}


// ＤＩＢ作成 //
BOOL CDIB32::Create(int Width, int Height)
{
	BITMAPINFO		BmpInfo;

	// すでに画像用のメモリが確保されている可能性があるので、解放する //
	Cleanup();

	if(Width <= 0 || Height <= 0) return FALSE;

	ZEROMEM(BmpInfo);										// ゼロ初期化
	BmpInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);	// 構造体のサイズ

	BmpInfo.bmiHeader.biWidth  = Width;		// 画像の幅
	BmpInfo.bmiHeader.biHeight = -Height;	// 画像の高さ(top-downビットマップ)

	BmpInfo.bmiHeader.biPlanes      = 1;		// プレーン数
	BmpInfo.bmiHeader.biBitCount    = 32;		// ビット深度
	BmpInfo.bmiHeader.biCompression = BI_RGB;	// 通常のＤＩＢ

	// アプリの現在の画面と互換性のあるデバイスコンテキストを作成 //
	m_hDC = CreateCompatibleDC(NULL);
	if(NULL == m_hDC) return FALSE;

	// DIB を作成しましょ //
	m_hBMP = CreateDIBSection(NULL				// デバイスコンテキスト
							, &BmpInfo			// BMPINFO 構造体へのポインタ
							, DIB_RGB_COLORS	// データの種類
							, (void **)&m_pData	// ビット値
							, NULL				// 無視
							, 0);				// 無視
	if(NULL == m_hBMP){
		DeleteDC(m_hDC);
		return FALSE;
	}

	m_dwWidth  = Width;		// 画像の幅
	m_dwHeight = Height;	// 画像の高さ
	m_lPitch   = Width * 4;	// 一つの点に４バイト使用する

	m_hOldObj = SelectObject(m_hDC, m_hBMP);
	if(NULL == m_hOldObj){
		Cleanup();
		return FALSE;
	}

	// ＤＩＢをゼロ初期化する //
	Clear(RGB(0, 0, 0));

	return TRUE;
}


// オブジェクト解放 //
void CDIB32::Cleanup(void)
{
	if(m_hDC){
		if(m_hOldObj){
			SelectObject(m_hDC, m_hOldObj);	// 戻してから
			m_hOldObj = NULL;				// 無効化
		}

		DeleteDC(m_hDC);
		m_hDC = NULL;		// ハンドル無効化
	}

	if(m_hBMP){
		DeleteObject(m_hBMP);
		m_hBMP = NULL;			// ハンドル無効化
	}

	// この時点で、ポインタは無効になっている筈 //
	m_pData = NULL;

	m_dwWidth   = 0;		// 画像の幅
	m_dwHeight  = 0;		// 画像の高さ
	m_lPitch    = 0;		// 画像のピッチ
	m_bHasAlpha = FALSE;	// α値を持っていれば真
}


// α付き画像転送 //
FBOOL CDIB32::DrawA(int x, int y, CDIB32 &Src)
{
	if(FALSE == Src.Update()) return FALSE;

	// 結局のトコロ、まだ実装されていないのです //

	return TRUE;
}


// 指定色で塗りつぶし //
FVOID CDIB32::Clear(COLORREF Color)
{
	DWORD		nPixels;
	DWORD		*pDest;

	// ＤＩＢを作成していない場合 //
	if(NULL == m_pData) return;

	nPixels = m_dwWidth * m_dwHeight;	// 塗りつぶすピクセル数を求める
	pDest   = m_pData;					// 左上の座標

	// 塗りつぶす //
	while(nPixels--){
		*pDest = Color;		// 塗りつぶし
		pDest++;			// 次のピクセルへ
	}
}


// α値をもっていれば真 //
FBOOL CDIB32::HasAlpha(void)
{
	return m_bHasAlpha;
}


// α値を持たせる場合は、TRUE を渡す //
FVOID CDIB32::SetAlpha(BOOL bHasAlpha)
{
	m_bHasAlpha = bHasAlpha;
}


// ファイルに書き出す //
FBOOL CDIB32::Save(char *pBMPFileName)
{
	Pbg::CPbgFile		File;

	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;

	DWORD				FileSize, DataSize;
	DWORD				WriteSize;
	int					Width, i;
	DWORD				nLines;
	DWORD				*pSrc;
	BYTE				*WriteBuffer;

	// ＤＩＢを作成していない場合 //
	if(NULL == m_pData) return FALSE;

	// ファイルがオープンできん //
	if(FALSE == File.Open(pBMPFileName, "w")) return FALSE;

	// 書き込み用バッファを作成する //
	WriteBuffer = NewEx(BYTE[m_dwWidth * 3]);
	if(NULL == WriteBuffer){
		DeleteArray(WriteBuffer);
		return FALSE;				// 失敗
	}

	ZEROMEM(FileHeader);
	ZEROMEM(InfoHeader);

	Width    = m_dwWidth;				// 横幅
	DataSize = Width * m_dwHeight * 3;	// サイズ

	// ファイルサイズは 縦幅×横幅＋ヘッダ となる //
	FileSize = DataSize + sizeof(FileHeader) + sizeof(InfoHeader);

	FileHeader.bfType    = MAKEWORD('B', 'M');
	FileHeader.bfSize    = FileSize;			// 上に同じ

	// ヘッダ分のサイズを読み飛ばしてくれぃ //
	FileHeader.bfOffBits = sizeof(FileHeader) + sizeof(InfoHeader);

	InfoHeader.biSize        = sizeof(InfoHeader);	// 構造体のサイズ
	InfoHeader.biWidth       = m_dwWidth;			// 画像の幅
	InfoHeader.biHeight      = m_dwHeight;			// 画像の高さ
	InfoHeader.biPlanes      = 1;					// プレーン数
	InfoHeader.biBitCount    = 24;					// ビット深度
	InfoHeader.biCompression = BI_RGB;				// 通常のＤＩＢ

	// ヘッダの書き込みぃ //
	File.Write(&FileHeader, sizeof(FileHeader), &WriteSize);
	File.Write(&InfoHeader, sizeof(InfoHeader), &WriteSize);

	nLines = m_dwHeight;				// ピクセル数を求める
	pSrc   = m_pData + DataSize/3 - 1;	// 右下へのポインタ

	// 結局３の倍数になるので... //
	Width *= 3;

	// １ラインずつ塗りつぶす //
	while(nLines--){
		// ２４ビットのＲＧＢ //
		for(i=Width-3; i>=0; i-=3){
			WriteBuffer[i+0] = GetRValue(*pSrc);
			WriteBuffer[i+1] = GetGValue(*pSrc);
			WriteBuffer[i+2] = GetBValue(*pSrc);

			pSrc--;		// 前のピクセルへ
		}

		// ファイルに１ライン分のデータを書き込む //
		File.Write(WriteBuffer, Width, &WriteSize);
	}

	File.Close();				// ファイルを閉じる
	DeleteArray(WriteBuffer);

	return TRUE;
}


// 画像幅を取得する //
DWORD CDIB32::GetWidth(void)
{
	return m_dwWidth;
}


// 画像の高さを取得する //
DWORD CDIB32::GetHeight(void)
{
	return m_dwHeight;
}


// 更新する(コピーの前に自動的に呼び出される) //
BOOL CDIB32::Update(void)
{
	return TRUE;
}



} // namespace Pbg
