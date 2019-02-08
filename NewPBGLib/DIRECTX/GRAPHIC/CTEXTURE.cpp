/*
 *   CTexture.cpp   : テクスチャ管理クラス
 *
 */

#include "CTexture.h"
#include "PbgError.h"
#include "PbgMem.h"
#include "..\\DxUtil.h"



namespace Pbg {



// コンストラクタ //
CGrpTexture::CGrpTexture()
{
	m_Width     = 0;		// テクスチャの幅(一辺が２のｎ乗、正方形とすべし)
	m_Height    = 0;		// テクスチャの高さ
	m_Bpp       = 0;		// ビット深度

	m_AlphaDepth = 0;		// αを使用するか
	m_Flags      = 0;		// テクスチャ生成フラグ

	m_lpSurface = NULL;		// テクスチャ Surface
	m_pDIB      = NULL;		// ＤＩＢイメージの格納先
}


// デストラクタ //
CGrpTexture::~CGrpTexture()
{
	Release();
}


// 書き込み用 DIB32 を取得する //
CDIB32 *CGrpTexture::Lock(void)
{
	// まず、テクスチャが作成されていたら、破棄する //
	Release();

	// ＤＩＢ管理クラスを発生させる //
	m_pDIB = NewEx(Pbg::CDIB32);

	return m_pDIB;
}


// テクスチャを実際に作成する //
BOOL CGrpTexture::Unlock(LPDIRECT3DDEVICE7 lpDevice, DWORD Flags)
{
	if(NULL == m_pDIB) return FALSE;

	// テクスチャ用のフラグをセットする //
	SetTextureFlags(Flags);

	return Restore(lpDevice);
}


// このテクスチャの Surface を取得する //
LPSURFACE CGrpTexture::GetSurface(void)
{
	return m_lpSurface;
}


// Surface を再び作成する                                 //
// 参考：Restore() はＢＭＰが関連づけられていないときは、 //
// 何も行わずに真を返すものとする                         //
BOOL CGrpTexture::Restore(LPDIRECT3DDEVICE7 lpDevice)
{
	HRESULT					hr;				// 戻り値格納用
	LPDIRECTDRAW7			lpDD;			// ＤＤインターフェース
	LPDIRECTDRAWSURFACE7	lpRenderSurf;	// レンダリングの対象
	D3DDEVICEDESC7			ddDesc;			// Direct3D.Deivce の記述
	DDSURFACEDESC2			ddsd;			// DirectDraw Surface の記述
	TextureSearchInfo		TexInfo;		// テクスチャの列挙情報

	// エラーログ用の関数名をセット //
	PbgErrorInit("CGrpTexture::Restore()");

	// このテクスチャは、使用中でない　//
	if(NULL == m_pDIB) return TRUE;

	// それは、無理っす //
	if(NULL == lpDevice){
		PbgErrorEx("内部エラー：3D Device が見つかりません");
		return FALSE;
	}

	// すでにテクスチャを生成した形跡があり、ロストしている(するのか？) //
	// 場合は、一度解放してからリロードする                             //
	if(m_lpSurface){
		if(DDERR_SURFACELOST != m_lpSurface->IsLost()){
			return TRUE;
		}

		SAFE_RELEASE(m_lpSurface);
	}

	// このテクスチャは、使用中でない　//
	if(NULL == m_pDIB) return TRUE;

	// ３Ｄデバイスの性能を取得する //
	hr = lpDevice->GetCaps(&ddDesc);
	if(FAILED(hr)){
		PbgErrorEx("GetCaps() Failed.");
		return FALSE;
	}

	ZEROMEM(ddsd);		// ゼロ初期化
	ddsd.dwSize                 = sizeof(ddsd);			// 構造体のサイズ
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);// 上に同じ
	ddsd.ddsCaps.dwCaps         = DDSCAPS_TEXTURE;		// テクスチャである
	ddsd.dwTextureStage         = 0;		// テクスチャステージ
	ddsd.dwWidth                = m_Width;	// テクスチャの幅(調整済み)
	ddsd.dwHeight               = m_Height;	// テクスチャの高さ(調整済み)

	// 構造体の有効なメンバを指定する                            //
	// (CAPS、幅、高さ、ピクセルフォーマット、テクスチャステージ //
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH
				 | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;

	// ハードウェアでのラスタライズを行う場合 //
	if(ddDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION){
		// テクスチャマネージメントの使用 //
		ddsd.ddsCaps.dwCaps2
			= DDSCAPS2_TEXTUREMANAGE | DDSCAPS2_HINTANTIALIASING;
	}
	// ソフトウェアの場合 //
	else{
		// システムメモリ上に //
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	}

	// パレットを使うのかな？ //
	if(8 == m_Bpp) TexInfo.bUsePalette = TRUE;	// ８ビットなら使用する
	else           TexInfo.bUsePalette = FALSE;	// 他は使用しない

	TexInfo.bFoundGoodFormat = FALSE;			// 適切なフォーマットが見つかった
	TexInfo.Bpp              = m_Bpp;			// ビット深度
	TexInfo.AlphaDepth       = m_AlphaDepth;	// αのビット数
//	TexInfo.bUseAlpha        = m_bHasAlpha;	//α(カラーキー)を使用するのか

	// データの書き込み先をセットする //
	TexInfo.pddpf = &(ddsd.ddpfPixelFormat);

	// それでは、テクスチャを列挙してみましょう //
	lpDevice->EnumTextureFormats(EnumTextureFormat, &TexInfo);

	// 適切なフォーマットが見つからなかった場合 //
	if(FALSE == TexInfo.bFoundGoodFormat){
		TexInfo.bUsePalette = FALSE;	// パレットを使用しない
		TexInfo.Bpp         = 16;		// １６ビット

		// それじゃ、4-4-4-4 フォーマットで //
		if(TexInfo.AlphaDepth > 4) TexInfo.AlphaDepth = 4;

		// デフォルトのフォーマットで探して下さいな //
		lpDevice->EnumTextureFormats(EnumTextureFormat, &TexInfo);

		if(FALSE == TexInfo.bFoundGoodFormat){
			PbgErrorEx("適切なテクスチャフォーマットが見つからない");
			return FALSE;
		}
	}

	// レンダリングを行う Surface を取得する //
	hr = lpDevice->GetRenderTarget(&lpRenderSurf);
	if(FAILED(hr)){
		PbgErrorEx("GetRenderTarget() に失敗");
		return FALSE;
	}

	// DirectDraw インターフェースを取得する //
	hr = lpRenderSurf->GetDDInterface((VOID **)&lpDD);
	SAFE_RELEASE(lpRenderSurf);		// 失敗・成功に関わらず、開放する
	if(FAILED(hr)){
		PbgErrorEx("GetDDInterface() に失敗");
		return FALSE;
	}

	// それじゃ、Surface を作成しようか //
	hr = lpDD->CreateSurface(&ddsd, &m_lpSurface, NULL);
	SAFE_RELEASE(lpDD);				// 上と同様に、開放する
	if(FAILED(hr)){
		PbgErrorEx("CreateSurface() に失敗");
		return FALSE;
	}

	// *.tga ファイルにより生成された画像の場合 //
	if(m_pDIB->HasAlpha()){
		if(FALSE == CopyRGBAImage()){
			PbgErrorEx("画像データの転送に失敗(RGBA)");
			Release();
			return FALSE;
		}
	}
	// 通常のＢＭＰまたは、カラーキー付き //
	else{
		if(FALSE == CopyDIBImage()){
			PbgErrorEx("画像データの転送に失敗(RGB)");
			Release();		// 開放しておいて頂戴
			return FALSE;
		}
	}

	// 復旧を完了いたしました //
	return TRUE;
}


// テクスチャ＆画像を破棄
void CGrpTexture::Release(void)
{
	// テクスチャ Surface を破棄する //
	SAFE_RELEASE(m_lpSurface);

	// ＤＩＢイメージを持っている場合 //
	if(NULL != m_pDIB) DeleteEx(m_pDIB);
}


// テクスチャ作成に必要な要素をセットする //
void CGrpTexture::SetTextureFlags(DWORD Flag)
{
	DWORD		Width, Height;
	DWORD		i;

	// 単純にフラグをセットする //
	m_Flags = Flag;


	// αが必要な場合 //
	if(m_pDIB->HasAlpha()){
		// メモリを節約する場合 //
		if(Flag & GRPTEX_16BPP){	// 強制的に１６ビットにする
			m_AlphaDepth = 4;	// この設定は、暗に 4-4-4-4 フォーマットを
			m_Bpp        = 16;	// 要求するものである
		}
		// 非常に贅沢なフォーマット //
		else{						// デフォルトを３２ビットとする
			m_AlphaDepth = 8;	// αに８ビットを求める
			m_Bpp        = 32;	// できれば 8-8-8-8 フォーマットを
		}
	}
	// αが必要ない場合 //
	else{
		if(Flag & (GRPTEX_TRANSBLACK | GRPTEX_TRANSWHITE)){
			m_AlphaDepth = 1;	// カラーキーとして使用する
		}
		else{
			m_AlphaDepth = 0;	// 一切使用しない
		}

		// ビット深度の設定を行う //
		if(     Flag & GRPTEX_USEPALETTE) m_Bpp = 8;	// パレットを使用する
		else if(Flag & GRPTEX_32BPP)      m_Bpp = 32;	// ３２ビット使用する
		else                              m_Bpp = 16;	// デフォルトは１６ビット
	}

	Width  = m_pDIB->GetWidth();	// 元画像から幅を取得
	Height = m_pDIB->GetHeight();	// 元画像から高さを取得
	Width  = max(Width, Height);	// 幅と高さから大きい方を選択

	// ２のｎ乗で、２５６以下に納める //
	for(i=1; i<256; i*=2){
		if(i >= Width) break;
	}

	m_Width  = i;		// テクスチャの幅
	m_Height = i;		// テクスチャの高さ
}


// ＢＭＰイメージをテクスチャに転送する //
typedef union tagRGBA_INFO {
	DWORD		rgba;

	struct {
		BYTE	rgbBlue;
		BYTE	rgbGreen;
		BYTE	rgbRed;
		BYTE	rgbReserved;
	};
} RGBA_INFO;

BOOL CGrpTexture::CopyDIBImage(void)
{
	RGBA_INFO	mask, test, *p;
	int			w, h, i, n;

//	PbgLog("CopyDIBImage()");
	p = (RGBA_INFO *)m_pDIB->GetTarget();
	if(NULL == p) return FALSE;

	w = m_pDIB->GetWidth();
	h = m_pDIB->GetHeight();
	n = w * h;

	// 透過色指定が存在する場合 //
	if(m_Flags & (GRPTEX_TRANSWHITE | GRPTEX_TRANSBLACK)){
		mask.rgbRed = mask.rgbGreen = mask.rgbBlue = 0xff;
		mask.rgbReserved = 0;

		if(m_Flags & GRPTEX_TRANSWHITE){
			test = mask;
		}
		else{
			ZEROMEM(test);
		}

		for(i=0; i<n; i++, p++){
			// 色を抜く部分 //
			if(test.rgba == (mask.rgba & (*p).rgba)){
				p->rgbReserved = 0;
			}
			// 色を抜かない部分 //
			else{
				p->rgbReserved = 0xff;
			}
		}
	}
	// 透過色指定が無い場合 //
	else{
		for(i=0; i<n; i++, p++){
			p->rgbReserved = 0xff;	// 透過色無し
		}
	}

	// 後は任せた //
	return CopyRGBAImage();
}


// αを含むイメージをテクスチャに転送する //
BOOL CGrpTexture::CopyRGBAImage(void)
{
	LPDIRECTDRAW7			lpDD;			// DirectDraw インターフェース
	LPDIRECTDRAWSURFACE7	lpSysSurface;	// システムメモリ上の Surface
	DDSURFACEDESC2			ddsd;			// Surface の記述
	HRESULT					hr;				// 戻り値の格納先

	// エラーログ用の関数名をセット //
	PbgErrorInit("CGrpTexture::CopyRGBAImage()");

	// 初期化されていないみたい... //
	if(NULL == m_lpSurface || NULL == m_pDIB){
		PbgErrorEx("内部エラー：初期化されていない");
		return FALSE;
	}

	ZEROMEM(ddsd);					// ゼロ初期化
	ddsd.dwSize = sizeof(ddsd);		// 構造体のサイズをセット

	// Surface 記述子の取得 //
	hr = m_lpSurface->GetSurfaceDesc(&ddsd);
	if(FAILED(hr)){
		PbgErrorEx("GetSurfaceDesc() Failed.");
		return FALSE;
	}

	// DirectDraw インターフェースを取得する //
	hr = m_lpSurface->GetDDInterface((VOID **)&lpDD);
	if(FAILED(hr)){
		PbgErrorEx("GetDDInterface() Failed.");
		return FALSE;
	}

	// 構造体の有効なメンバを指定する                            //
	// (CAPS、幅、高さ、ピクセルフォーマット、テクスチャステージ //
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH
				 | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;

	// テクスチャとして、システムメモリ上に作成 //
	ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;

	ddsd.ddsCaps.dwCaps2 = 0;			// フラグを無効化する
	ddsd.dwWidth         = m_Width;		// テクスチャの幅
	ddsd.dwHeight        = m_Height;	// テクスチャの高さ

	// 画像転送用の Surface を作成する //
	hr = lpDD->CreateSurface(&ddsd, &lpSysSurface, NULL);
	if(FAILED(hr)){
		PbgErrorEx("CreateSurface(Texture) Failed.");
		SAFE_RELEASE(lpDD);	// DirectDraw インターフェースを開放
		return FALSE;
	}


	BYTE		*pLockTarget;			// ロック時における左上のアドレス
	DWORD		LockWidth, LockHeight;	// ロック時の幅＆高さ
	DWORD		LockPitch;				// ロック時のピッチ
	DWORD		x, y;					// 順次アクセス用
	WORD		*p16Bit;				// １６ビット書き込み用のポインタ
	DWORD		*p32Bit;				// ３２ビット書き込み用のポインタ

	// テクスチャ読み込み用の Surface をロックする //
	hr = lpSysSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
	if(FAILED(hr)){
		PbgErrorEx("Lock(SystemSurface) Failed.");
		SAFE_RELEASE(lpSysSurface);
		SAFE_RELEASE(lpDD);
		return FALSE;
	}

	// 書き込みに関する情報を準備 //
	LockWidth   = ddsd.dwWidth;			// テクスチャの横幅
	LockHeight  = ddsd.dwHeight;		// テクスチャの縦幅
	LockPitch   = ddsd.lPitch;			// テクスチャのピッチ
	pLockTarget = (BYTE *)ddsd.lpSurface;	// 書き込み先

	// 各ビットマスクを取得する //
	DWORD RMask = ddsd.ddpfPixelFormat.dwRBitMask;			// 赤のビットマスク
	DWORD GMask = ddsd.ddpfPixelFormat.dwGBitMask;			// 緑のビットマスク
	DWORD BMask = ddsd.ddpfPixelFormat.dwBBitMask;			// 青のビットマスク
	DWORD AMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;	// αのビットマスク

	// 何ともまぁ、気持ちの悪い記述だことで... //
	DWORD RShiftR, RShiftL;		BitMask2Shift(RMask, RShiftR, RShiftL);
	DWORD GShiftR, GShiftL;		BitMask2Shift(GMask, GShiftR, GShiftL);
	DWORD BShiftR, BShiftL;		BitMask2Shift(BMask, BShiftR, BShiftL);
	DWORD AShiftR, AShiftL;		BitMask2Shift(AMask, AShiftR, AShiftL);

	DWORD		dr, dg, db, da;
	RGBQUAD		*pSrc, data;

/*
#ifdef PBG_DEBUG
		char	buf[1024];
		BYTE	r, g, b, a;

		r = GetMask2BitCount(ddsd.ddpfPixelFormat.dwRBitMask);		// 赤のビット数
		g = GetMask2BitCount(ddsd.ddpfPixelFormat.dwGBitMask);		// 緑のビット数
		b = GetMask2BitCount(ddsd.ddpfPixelFormat.dwBBitMask);		// 青のビット数
		a = GetMask2BitCount(ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);	// αのビット数

		wsprintf(buf,	"\tα付きフォーマットを使用します\r\n"
						"\t\t%2dBit TextureFormat (R:%2d  G:%2d  B:%2d  A:%2d)\r\n"
					, ddsd.ddpfPixelFormat.dwRGBBitCount, r, g, b, a);

		PbgLog(buf);
#endif
*/

	pSrc = (RGBQUAD *)m_pDIB->GetTarget();


	// 書き込み開始 //
	for(y=0; y<LockHeight; y++){
		p16Bit = (WORD  *)pLockTarget;
		p32Bit = (DWORD *)pLockTarget;

		for(x=0; x<LockWidth; x++){
			data = *pSrc;

			dr = ( (data.rgbRed      >> RShiftL) << RShiftR) & RMask;
			dg = ( (data.rgbGreen    >> GShiftL) << GShiftR) & GMask;
			db = ( (data.rgbBlue     >> BShiftL) << BShiftR) & BMask;
			da = ( (data.rgbReserved >> AShiftL) << AShiftR) & AMask;

			if(32 == ddsd.ddpfPixelFormat.dwRGBBitCount){
				p32Bit[x] = (DWORD)(dr + dg + db + da);
			}
			else{
				p16Bit[x] = (WORD)(dr + dg + db + da);
			}

			pSrc++;
		}

		pLockTarget += LockPitch;
	}

	// 書き込み完了 //
	lpSysSurface->Unlock(0);

	// SYSTEM -> VIDEO にテクスチャを転送する //
	m_lpSurface->Blt(NULL, lpSysSurface, NULL, DDBLT_WAIT, NULL);

	// さよなら //
	SAFE_RELEASE(lpSysSurface);
	SAFE_RELEASE(lpDD);


	return TRUE;
}


// ビットマスクから、シフト値を取得する //
FVOID CGrpTexture::BitMask2Shift(DWORD Mask, DWORD &ShiftR, DWORD &ShiftL)
{
	DWORD	temp;

	ShiftR = 0;
	ShiftL = 8;

	for(temp=Mask; temp && !(temp & 1); temp >>= 1) ShiftR++;
	for(; temp; temp >>= 1)                         ShiftL--;
}


// 使用できるテクスチャのフォーマットを検索条件から絞り込む //
HRESULT CALLBACK
	CGrpTexture::EnumTextureFormat(
				DDPIXELFORMAT *pddpf	// テクスチャのフォーマット
			  , VOID *pParam)			// TextureSearchInfo 構造体へのポインタ
{
	TextureSearchInfo		*pInfo;

	// 内部エラー：書き込み先のポインタが不正 //
	if(NULL == pParam) return D3DENUMRET_CANCEL;

	// 引数を TextureSearchInfo 構造体へのポインタにキャスト //
	pInfo = (TextureSearchInfo *)pParam;

	// バンプマッピング用のテクスチャフォーマットは使用しない //
	if(pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV)){
		return D3DENUMRET_OK;
	}

	// FOURCC 系のフォーマットは選択しない //
	if(pddpf->dwFourCC) return D3DENUMRET_OK;

	// パレットが必要な場合 //
	if(pInfo->bUsePalette){
		// ８ビットパレットではなかったので、却下 //
		if(!(pddpf->dwFlags & DDPF_PALETTEINDEXED8)) return D3DENUMRET_OK;

		*(pInfo->pddpf)         = *pddpf;
		pInfo->bFoundGoodFormat = TRUE;
		return D3DENUMRET_CANCEL;			// もう、列挙はしないで
	}

	// 非パレットで１６ビット以下のフォーマットは使用しない //
	if(pddpf->dwRGBBitCount < 16) return D3DENUMRET_OK;

	// カラーキー・αを使用しない場合、α用のビットは必要ない //
	if(0 == pInfo->AlphaDepth){
		if(pddpf->dwFlags & DDPF_ALPHAPIXELS) return D3DENUMRET_OK;
	}
	// αに必要なビット数を満たしているかを調べる //
	else if(pInfo->AlphaDepth > (DWORD)GetMask2BitCount(pddpf->dwRGBAlphaBitMask)){
		return D3DENUMRET_OK;
	}

	// ビット深度もばっちり一致した場合 //
	if(pddpf->dwRGBBitCount == pInfo->Bpp){
		*(pInfo->pddpf) = *pddpf;			// データ格納
		pInfo->bFoundGoodFormat = TRUE;		// フラグを真にする
		return D3DENUMRET_CANCEL;
	}

	return D3DENUMRET_OK;
}



}
