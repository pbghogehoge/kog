/*
 *   CTexContainer.cpp   : テクスチャ管理クラス
 *
 */

#include "CTexture.h"
#include "PbgError.h"
#include "CGrpLoader.h"
#include "..\\DxUtil.h"



namespace Pbg {



// コンストラクタ //
CTextureContainer::CTextureContainer()
{
	m_lpDevice = NULL;		// Direct3D.Device を無効化する

	// その他については、CGrpTexture() のコンストラクタがやってくれる筈 //
}


// デストラクタ //
CTextureContainer::~CTextureContainer()
{
//PbgError("CTC_DST_IN");
//	OutputTextureFormat();
//PbgError("CTC_DST_OUT");
}


// ＢＭＰイメージからテクスチャを作成する(ＢＭＰファイル) //
BOOL CTextureContainer::CreateTexture(
					DWORD	TextureID		// テクスチャＩＤ
				  , char	*pBMPFileName	// 元ＢＭＰファイル名
				  , DWORD	Flag)			// テクスチャ属性フラグ
{
	CDIB32		*pDib;

	// デバイスが無効なら、復旧は行えない //
	if(NULL == m_lpDevice) return FALSE;

	// 指定されたテクスチャの管理は行えないね //
	if(TextureID >= GRPTEXTURE_MAX) return FALSE;

	// テクスチャに接続されるＤＩＢを取得 //
	pDib = m_Texture[TextureID].Lock();
	if(NULL == pDib) return FALSE;

	// ファイルから読み込む //
	if(FALSE == CGrpLoader::Load(pDib, pBMPFileName)){
		m_Texture[TextureID].Release();
	}

	// 後は、テクスチャを作成して、完了 //
	return m_Texture[TextureID].Unlock(m_lpDevice, Flag);
}


// ＢＭＰイメージからテクスチャを作成する(圧縮ファイルＩＤ) //
BOOL CTextureContainer::CreateTextureP(
					DWORD	TextureID		// テクスチャＩＤ
				  , char	*pPackFileName	// 圧縮ファイル名
				  , char	*pFileID		// 圧縮ファイルＩＤ
				  , DWORD	Flag)			// テクスチャの属性
{
	CDIB32		*pDib;

	// デバイスが無効なら、復旧は行えない //
	if(NULL == m_lpDevice) return FALSE;

	// 指定されたテクスチャの管理は行えない //
	if(TextureID >= GRPTEXTURE_MAX) return FALSE;

	// テクスチャに接続されるＤＩＢを取得 //
	pDib = m_Texture[TextureID].Lock();
	if(NULL == pDib) return FALSE;

	// ファイルから読み込む //
	if(FALSE == CGrpLoader::LoadP(pDib, pPackFileName, pFileID)){
		m_Texture[TextureID].Release();
	}

	// 後は、テクスチャを作成して、完了 //
	return m_Texture[TextureID].Unlock(m_lpDevice, Flag);
}


// ＢＭＰイメージからテクスチャを作成する(圧縮ファイルＮｏ) //
BOOL CTextureContainer::CreateTextureP(
					DWORD	TextureID		// テクスチャＩＤ
				  , char	*pPackFileName	// 圧縮ファイル名
				  , DWORD	FileNo			// 圧縮ファイルＮｏ
				  , DWORD	Flag)			// テクスチャの属性
{
	CDIB32		*pDib;

	// デバイスが無効なら、復旧は行えない //
	if(NULL == m_lpDevice) return FALSE;

	// 指定されたテクスチャの管理は行えないね //
	if(TextureID >= GRPTEXTURE_MAX) return FALSE;

	// テクスチャに接続されるＤＩＢを取得 //
	pDib = m_Texture[TextureID].Lock();
	if(NULL == pDib) return FALSE;

	// ファイルから読み込む //
	if(FALSE == CGrpLoader::LoadP(pDib, pPackFileName, FileNo)){
		m_Texture[TextureID].Release();
	}

	// 後は、テクスチャを作成して、完了 //
	return m_Texture[TextureID].Unlock(m_lpDevice, Flag);
}


// ＢＭＰイメージからテクスチャを作成する(りそ圧縮ファイルＩＤ) //
BOOL CTextureContainer::CreateTexturePR(DWORD	TextureID
									  , char	*pPackFileName
									  , char	*pFileID
									  , DWORD	Flag)
{
	CDIB32		*pDib;

	// デバイスが無効なら、復旧は行えない //
	if(NULL == m_lpDevice) return FALSE;

	// 指定されたテクスチャの管理は行えない //
	if(TextureID >= GRPTEXTURE_MAX) return FALSE;

	// テクスチャに接続されるＤＩＢを取得 //
	pDib = m_Texture[TextureID].Lock();
	if(NULL == pDib) return FALSE;

	// ファイルから読み込む //
	if(FALSE == CGrpLoader::LoadPR(pDib, pPackFileName, pFileID)){
		m_Texture[TextureID].Release();
	}

	// 後は、テクスチャを作成して、完了 //
	return m_Texture[TextureID].Unlock(m_lpDevice, Flag);
}


// ＢＭＰイメージからテクスチャを作成する(りそ圧縮ファイルＮｏ) //
BOOL CTextureContainer::CreateTexturePR(DWORD	TextureID
									  , char	*pPackFileName
									  , DWORD	FileNo
									  , DWORD	Flag)
{
	CDIB32		*pDib;

	// デバイスが無効なら、復旧は行えない //
	if(NULL == m_lpDevice) return FALSE;

	// 指定されたテクスチャの管理は行えない //
	if(TextureID >= GRPTEXTURE_MAX) return FALSE;

	// テクスチャに接続されるＤＩＢを取得 //
	pDib = m_Texture[TextureID].Lock();
	if(NULL == pDib) return FALSE;

	// ファイルから読み込む //
	if(FALSE == CGrpLoader::LoadPR(pDib, pPackFileName, FileNo)){
		m_Texture[TextureID].Release();
	}

	// 後は、テクスチャを作成して、完了 //
	return m_Texture[TextureID].Unlock(m_lpDevice, Flag);
}


// 全てのテクスチャを修復する //
BOOL CTextureContainer::RestoreAllTexture(void)
{
	DWORD		i;
	BOOL		bResult = TRUE;		// 成功していると仮定

	// デバイスが無効なら、復旧は行えない //
	if(NULL == m_lpDevice) return FALSE;

	// 全てのテクスチャを復旧対象と成す //
	for(i=0; i<GRPTEXTURE_MAX; i++){
		// もし、復旧に失敗した場合は、フラグを偽とする //
		// 成功時は、前回のフラグの状態を維持する       //
		if(FALSE == m_Texture[i].Restore(m_lpDevice)){
			bResult = FALSE;
		}
	}

	// どれか一つでもも復旧に失敗したならば偽を返す //
	return bResult;
}


// 全てのテクスチャを開放する
void CTextureContainer::ReleaseAllTexture(void)
{
	DWORD		i;

	// 全てのテクスチャに対して、開放要求 //
	for(i=0; i<GRPTEXTURE_MAX; i++){
		m_Texture[i].Release();
	}
}


// テクスチャ Surface を取得する //
LPSURFACE CTextureContainer::GetTextureSurface(DWORD TextureID)
{
	// このテクスチャクラスは指定された番号のテクスチャを管理できない //
	// 従って、NULL を返し、エラーとなす //
	if(TextureID >= GRPTEXTURE_MAX) return NULL;

	// 後はお任せ //
	return m_Texture[TextureID].GetSurface();
}


// テクスチャフォーマットを出力する //
BOOL CTextureContainer::OutputTextureFormat(void)
{
//	char		buf[1024];

	if(NULL == m_lpDevice) return FALSE;

	// テクスチャフォーマットを列挙＆バッファに格納 //
//	wsprintf(buf, "サポートされるテクスチャフォーマット\r\n");
	PbgLog2("サポートされるテクスチャフォーマット");
	m_lpDevice->EnumTextureFormats(EnumTextureFormatForOutput, NULL);

	// 実際に出力 //
	PbgLog("");

	return TRUE;
}


BOOL CTextureContainer::Set3DDeviceForTexture(LPDIRECT3DDEVICE7 lpD3Dev)
{
	m_lpDevice = lpD3Dev;

	if(NULL == lpD3Dev) return FALSE;	// 失敗
	else                return TRUE;	// 成功
}


// テクスチャフォーマットを列挙し、バッファにデータを追加する //
HRESULT CALLBACK
	CTextureContainer::EnumTextureFormatForOutput(
						DDPIXELFORMAT	*pddpf
					  , VOID			*pStringBuf)
{
	char	buf[1024];
//	char	*pTarget;
	int		r, g, b, a, n, use;


	// バンプマッピング用のテクスチャフォーマットは使用しない //
	if(pddpf->dwFlags & (DDPF_LUMINANCE | DDPF_BUMPLUMINANCE | DDPF_BUMPDUDV)){
		return D3DENUMRET_OK;
	}

	// FOURCC 系のフォーマットは選択しない //
	if(pddpf->dwFourCC) return D3DENUMRET_OK;

	// パレット系のフォーマットの場合 //
	if(pddpf->dwFlags &
					( DDPF_PALETTEINDEXED1 | DDPF_PALETTEINDEXED2
					| DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED8)){

		if(     pddpf->dwFlags & DDPF_PALETTEINDEXED8) n = 8;
		else if(pddpf->dwFlags & DDPF_PALETTEINDEXED4) n = 4;
		else if(pddpf->dwFlags & DDPF_PALETTEINDEXED2) n = 2;
		else if(pddpf->dwFlags & DDPF_PALETTEINDEXED1) n = 1;
		else return D3DENUMRET_OK;

		if(pddpf->dwFlags & DDPF_PALETTEINDEXEDTO8) use = 8;
		else                                        use = n;

		wsprintf(buf,"\t%2dBit TextureFormat (PaletteIndexed %d)", use, n);
	}
	else{
		r = GetMask2BitCount(pddpf->dwRBitMask);		// 赤のビット数
		g = GetMask2BitCount(pddpf->dwGBitMask);		// 緑のビット数
		b = GetMask2BitCount(pddpf->dwBBitMask);		// 青のビット数
		a = GetMask2BitCount(pddpf->dwRGBAlphaBitMask);	// αのビット数

		wsprintf(buf,"\t%2dBit TextureFormat (R:%2d  G:%2d  B:%2d  A:%2d)"
					, pddpf->dwRGBBitCount, r, g, b, a);
	}

	PbgLog2(buf);

	return D3DENUMRET_OK;
}



} // namespace Pbg
