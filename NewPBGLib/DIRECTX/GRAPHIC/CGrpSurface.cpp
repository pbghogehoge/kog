/*
 *   CGrpSurface.cpp   : Surface管理クラス
 *
 */

#include "CGrpSurface.h"
#include "CDIB32.h"
#include "PbgError.h"
#include "PbgMem.h"



namespace Pbg {



// コンストラクタ //
CGrpSurface::CGrpSurface(LPSURFACE Surface, CGraphic *pParent)
{
	HRESULT			hr;
	DDSURFACEDESC2	ddsd;

	m_lpSurf        = Surface;	// DirectDrawSurface インターフェース
	m_pParent       = pParent;	// Restore() 要求先
	m_pDIB          = NULL;		// 関連づけられたＤＩＢ

	if(NULL != Surface){
		// Surface に関連づけられたＢＭＰデータ     //
		// 失敗した場合、その後の関数は全て失敗する //
		m_pDIB = NewEx(Pbg::CDIB32);
		if(NULL == m_pDIB) return;

		// Surface の特性を調べるために構造体を初期化する //
		ZEROMEM(ddsd);					// ゼロ初期化
		ddsd.dwSize = sizeof(ddsd);		// 構造体のサイズ

		// それでは、調べてみましょう //
		hr = Surface->GetSurfaceDesc(&ddsd);
		if(FAILED(hr)){
			DeleteEx(m_pDIB);	// これは分からないね
			return;
		}

		// サイズがわかったので、ＤＩＢを作成する //
		if(FALSE == m_pDIB->Create(ddsd.dwWidth, ddsd.dwHeight)){
			DeleteEx(m_pDIB);	// メモリ足りないかね
			return;
		}
	}
}


// デストラクタ //
CGrpSurface::~CGrpSurface()
{
	SAFE_RELEASE(m_lpSurf);		// DDSurface を解放
	DeleteEx(m_pDIB);			// リストア用 DIB を削除
}


// 他の Surface の画像を張り付ける(カラーキー付き) //
FBOOL CGrpSurface::BltC(RECT *src, int x, int y, const CGrpSurface *pSurf)
{
	HRESULT			hr;
	LPSURFACE		pSurface;

	pSurface = const_cast<CGrpSurface *>(pSurf)->GetSurface();


	for(;;){
		// まずは、描画してみようとする //
		hr = m_lpSurf->BltFast(x, y						// 描画する座標
								, pSurface				// DDrawSURFACE
								, src					// 描画元の矩形
								, DDBLTFAST_SRCCOLORKEY	// カラーキー有りの転送
			 );
		if(DD_OK == hr) return TRUE;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == m_pParent->RestoreAllSurfaces()){
				return FALSE;
			}
		}

		// "描画中" 以外は、解決できないエラー //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// 他の Surface の画像を張り付ける(カラーキー無し) //
FBOOL CGrpSurface::BltN(RECT *src, int x, int y, const CGrpSurface *pSurf)
{
	HRESULT			hr;
	LPSURFACE		pSurface;


	pSurface = const_cast<CGrpSurface *>(pSurf)->GetSurface();

	for(;;){
		// まずは、描画してみようとする //
		hr = m_lpSurf->BltFast(x, y						// 描画する座標
								, pSurface				// DDrawSURFACE
								, src					// 描画元の矩形
								, DDBLTFAST_SRCCOLORKEY	// カラーキー無しの転送
			 );
		if(DD_OK == hr) return TRUE;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == m_pParent->RestoreAllSurfaces()){
				return FALSE;
			}
		}

		// "描画中" 以外は、解決できないエラー //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// 書き込み用 CDIB32 を取得する //
CDIB32 *CGrpSurface::Lock(void)
{
	return m_pDIB;
}


// 書き込んだデータを反映する //
void CGrpSurface::Unlock(void)
{
	Restore();
}


// ファイルを読み込んで、Surface に関連づける //
BOOL CGrpSurface::Load(char *pBMPFileName)
{
	if(NULL == m_pDIB) return FALSE;

	if(FALSE == CGrpLoader::Load(m_pDIB, pBMPFileName)){
		return FALSE;
	}

	return Restore();
}


// ファイルを読み込んで、Surface に関連づける [圧ファイル(ＩＤ)] //
BOOL CGrpSurface::LoadP(char *pPackFileName, char *pFileID)
{
	if(NULL == m_pDIB) return FALSE;

	if(FALSE == CGrpLoader::LoadP(m_pDIB, pPackFileName, pFileID)){
		return FALSE;
	}

	return Restore();
}


// ファイルを読み込んで、Surface に関連づける [圧ファイル(Ｎｏ)] //
BOOL CGrpSurface::LoadP(char *pPackFileName, DWORD FileNo)
{
	if(NULL == m_pDIB) return FALSE;

	if(FALSE == CGrpLoader::LoadP(m_pDIB, pPackFileName, FileNo)){
		return FALSE;
	}

	return Restore();
}


// CDIB32 画像と接続する(自動的に Restore() を呼び出す) //
BOOL CGrpSurface::LoadDIB32(CDIB32 *pDIB32, int sx, int sy)
{
	int		w, h;

	// まずは更新して //
	if(FALSE == pDIB32->Update()) return FALSE;

	w = pDIB32->GetWidth();		// 幅を取得
	h = pDIB32->GetHeight();	// 高さを取得

	BitBlt(m_pDIB->GetDC(), sx, sy, w, h, pDIB32->GetDC(), 0, 0, SRCCOPY);

	return Restore();
}


// 画面を指定色で塗りつぶす(ＢＭＰ切り離し) //
BOOL CGrpSurface::Cls(DWORD Color, RECT *Target)
{
	DDBLTFX		ddbltfx;	// ＢＬＴオペ指定用構造体
	HRESULT		hr;			// エラーコードの格納先

	// Surface の指定がおかしい(これは内部エラーね) //
	if(NULL == m_lpSurf){
		PbgError("CGrpSurface::Cls() : 内部エラー(NULL == Surface)");
		return FALSE;
	}

	if(m_pDIB){
		// ロードした画像の方も塗りつぶす //
		m_pDIB->Clear(Color);
	}

	ZEROMEM(ddbltfx);						// 構造体のゼロ初期化
	ddbltfx.dwSize      = sizeof(ddbltfx);	// DDBLTFX 構造体のサイズ
	ddbltfx.dwFillColor = Color;			// 塗りつぶす色

	for(;;){
		// 単一色での塗りつぶしを実行する //
		hr = m_lpSurf->Blt(Target, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
		if(DD_OK == hr) return TRUE;	// 塗りつぶし完了

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == m_pParent->RestoreAllSurfaces())
				return FALSE;	// 原因不明のエラー
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;	// 解決できません
		}
	}
}


// カラーキーを設定する(座標指定) //
BOOL CGrpSurface::SetColorKey(int x, int y)
{
	HRESULT				hr;			// 戻り値の格納先
	DDSURFACEDESC2		ddsd;		// Surface の記述
	BYTE				*pTarget;	// 読み込み元
	DWORD				Value;		// 値の格納先
	DWORD				Offset;		// 画面端からのオフセット

	ZEROMEM(ddsd);					// 構造体のゼロ初期化
	ddsd.dwSize = sizeof(ddsd);		// 構造体のサイズをセットする

	// Surface を Lock する //
	hr = m_lpSurf->Lock(NULL, &ddsd, DDLOCK_WAIT, 0);
	if(FAILED(hr)) return FALSE;

	pTarget = (BYTE *)ddsd.lpSurface;			// 読み込み先を取得
	Offset  = (x * 2) + (y * ddsd.lPitch);		// オフセットを求める
	Value   = *(WORD *)(pTarget + Offset);		// 実際の値を取得

	// Surface を Unlock する //
	m_lpSurf->Unlock(NULL);

	return SetColorKey(Value);
}


// カラーキー指定(デフォルトは黒) //
BOOL CGrpSurface::SetColorKey(DWORD Color)
{
	DDCOLORKEY		Key;	// カラーキー指定用構造体
	HRESULT			hr;		// エラーコードの格納先

	// 色空間での指定ではないので、両方に同じ値を代入する //
	Key.dwColorSpaceHighValue = Color;	// 大きい方
	Key.dwColorSpaceLowValue  = Color;	// 小さい方

	for(;;){
		// カラーキーをセットする //
		hr = m_lpSurf->SetColorKey(DDCKEY_SRCBLT, &Key);
		if(DD_OK == hr) return TRUE;	// 成功！

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == m_pParent->RestoreAllSurfaces())
			return FALSE;	// 原因不明、かつ解決不可
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// この Surface を Restore する(関連画像のリロード) //
BOOL CGrpSurface::Restore(void)
{
	HDC			hDest;
	HRESULT		hr;
	int			w, h;

	if(FALSE == m_pDIB->Update()) return FALSE;

	for(;;){
		// デバイスコンテキストを取得する //
		hr = m_lpSurf->GetDC(&hDest);
		if(DD_OK == hr) break;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == m_pParent->RestoreAllSurfaces())
				return FALSE;	// 原因不明のエラー
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}

	w = (int)m_pDIB->GetWidth();	// 幅を取得
	h = (int)m_pDIB->GetHeight();	// 高さを取得

	// 簡易 減色or増色 処理付き転送 //
	BitBlt(hDest, 0, 0, w, h, m_pDIB->GetDC(), 0, 0, SRCCOPY);

	// デバイスコンテキストを解放 //
	m_lpSurf->ReleaseDC(hDest);

	return TRUE;
}



} // namespace Pbg
