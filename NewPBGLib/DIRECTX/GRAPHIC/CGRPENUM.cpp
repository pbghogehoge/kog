/*
 *   CGrpEnum.cpp   : ドライバ列挙用クラス
 *
 */

#include "CGrpEnum.h"
#include "..\\DxUtil.h"
#include "PbgError.h"



namespace Pbg {



// コンストラクタ //
CGrpEnum::CGrpEnum()
{
	m_pConfirmFunction = NULL;	// デバイス選別用の関数

	m_NumEnumDrivers = 0;		// 列挙されたドライバ数
	ZEROMEM(m_DrvInfo);			// 列挙情報の格納先
	ZEROMEM(m_ParentInfo);		// 元となる列挙情報
}


// デストラクタ //
CGrpEnum::~CGrpEnum()
{
	char		buf[1000];
	DWORD		i;
	GrpDrvInfo	*pInfo;

//	PbgDebugLog("\tCGrpEnum デストラクタ\r\n"
//				"\t以下にデバイスの列挙内容を示します");

	if(m_NumEnumDrivers){	// ドライバが存在していれば

		// 列挙したデバイスを表示します   //
		wsprintf(buf, "\tGraphic : ドライバ %d つみっけ", m_NumEnumDrivers);
		PbgLog(buf);

		// 出力されたデバイスを吐き出す //
		for(i=0; i<m_NumEnumDrivers; i++){
			pInfo = &m_DrvInfo[i];

			wsprintf(buf,	"\t %02d : [%s]   --- %s ---\r\n\tDirectDraw\tGUID { %s }\r\n"
							"\tDirect 3D  \tGUID { %s }\r\n"
								, i+1
								, pInfo->m_bIsHardware ? "HAL" : "HEL or REF"
								, pInfo->m_strDesc
								, pInfo->m_strDDGUID
								, pInfo->m_strD3DGUID);

			// ピクセルフォーマットの出力準備 //
			wsprintf(buf+strlen(buf), "\tPixelFormat(R, G, B, Z) = (%d, %d, %d, %d)",
										pInfo->m_PixelFormat.m_NumRBit,
										pInfo->m_PixelFormat.m_NumGBit,
										pInfo->m_PixelFormat.m_NumBBit,
										pInfo->m_ZBufferDepth);

			PbgLog(buf);
		}
	}

	// 実のところ、静的配列なので、クリアする必要はない           //
	// ただし、データ構造が変化した場合を考え、一応呼び出している //
	CleanupEnumeratedDevices();
}


// デバイスの列挙を行う //
BOOL CGrpEnum::EnumerateDevice(GrpConfirmFunc pFunction)
{
	HRESULT			hr;
	int				i;

	// 前回列挙を行っている可能性があるので、初期化する //
	CleanupEnumeratedDevices();

	// デバイス選別用の関数を接続する //
	m_pConfirmFunction = pFunction;

	// 列挙 //
	hr = DirectDrawEnumerate(EnumDDraw, (LPVOID)this);

	// それでは、デバイスをソート //
	qsort(m_DrvInfo, m_NumEnumDrivers, sizeof(GrpDrvInfo), DeviceSortFunction);

	// このままだと、GUID へのポインタが間違っているので、修正 //
	for(i=m_NumEnumDrivers-1; i>=0; i--){
		// DirectDraw の GUID //
		if(NULL != m_DrvInfo[i].m_lpDDGUID){
			m_DrvInfo[i].m_lpDDGUID = &(m_DrvInfo[i].m_DDGUID);
		}

		// Direct3D の GUID //
		if(NULL != m_DrvInfo[i].m_lpD3DGUID){
			m_DrvInfo[i].m_lpD3DGUID = &(m_DrvInfo[i].m_D3DGUID);
		}
	}


	if(FAILED(hr)) return FALSE;	// 失敗(内部エラー)
	else           return TRUE;		// 成功(失敗しないはず)
}


// 列挙されたデバイス数を取得する //
DWORD CGrpEnum::GetNumEnumeratedDevices(void)
{
	// デバイス数を返すだけ //
	return m_NumEnumDrivers;
}


// 列挙済みのデータをクリアする //
void CGrpEnum::CleanupEnumeratedDevices(void)
{
	m_NumEnumDrivers = 0;	// 列挙されたドライバ数
	ZEROMEM(m_DrvInfo);		// 列挙情報の格納先
	ZEROMEM(m_ParentInfo);	// 元となる列挙情報
}


// DirectDraw のドライバを列挙する //
BOOL WINAPI
	CGrpEnum::EnumDDraw(GUID FAR	*pGUID	// ＤＤｒａｗオブジェクトのＧＵＩＤ
					  , LPSTR		sDesc	// ドライバ記述・文字列
					  , LPSTR		sName	// ドライバ名を含む文字列
					  , LPVOID		pArg)	// this ポインタが格納されている
{
	LPDIRECTDRAW7		lpDD;
	LPDIRECT3D7			lpD3D;
	HRESULT				hr;
	CGrpEnum			*pThis;
	GrpDrvInfo			*pParent;

	// 内部エラーにつき、列挙を停止する //
	if(NULL == pArg) return TRUE;

	// 親情報へのアクセスを簡潔にする //
	pThis   = (CGrpEnum *)pArg;
	pParent = &(pThis->m_ParentInfo);

	// 親情報をゼロ初期化する //
	memset(pParent, 0, sizeof(GrpDrvInfo));

	// まずは、インターフェースを作成してみましょう //
	hr = DirectDrawCreateEx(pGUID, (void **)&lpDD, IID_IDirectDraw7, NULL);
	if(FAILED(hr)){
		return TRUE;
	}

	// まず、640x480x16 が使用できないと、話にならない //
	lpDD->EnumDisplayModes(0, NULL
						, &(pParent->m_PixelFormat)
						, pThis->EnumDisplayMode);

	// 上の関数は失敗しても構わない。ここではフラグの値が重要となる //
	if(0 == pParent->m_PixelFormat.m_NumRGBBits){
		SAFE_RELEASE(lpDD);
		return TRUE;
	}

	// Ｄ３Ｄインターフェースを取得する //
	hr = lpDD->QueryInterface(IID_IDirect3D7, (void **)&lpD3D);
	if(FAILED(hr)){
		SAFE_RELEASE(lpDD);
		return TRUE;
	}

	// デバイス名を親情報として格納する //
	lstrcpyn(pParent->m_strDesc, sDesc, GRPDEV_NAME_LENGTH);

	// GetCaps() 呼び出しのために、構造体のサイズをセットする //
	pParent->m_DDrawDriverCaps.dwSize = sizeof(DDCAPS);	// ドライバの性能(SIZE)
	pParent->m_DDrawHELCaps.dwSize    = sizeof(DDCAPS);	// ＨＥＬの性能(SIZE)

	// 能力を調べて //
	hr = lpDD->GetCaps(
				&(pParent->m_DDrawDriverCaps)	// ドライバの性能
			  , &(pParent->m_DDrawHELCaps)		// ＨＥＬの性能
		 );
	if(FAILED(hr)){
		SAFE_RELEASE(lpD3D);	// Ｄｉｒｅｃｔ３Ｄインターフェースを解放
		SAFE_RELEASE(lpDD);		// ＤｉｒｅｃｔＤｒａｗインターフェースを解放
		return TRUE;
	}

	// デフォルトではない、ドライバの場合 //
	if(NULL != pGUID){
		pParent->m_DDGUID   = (*pGUID);
		pParent->m_lpDDGUID = &(pParent->m_DDGUID);
	}
	// デフォルトのドライバの場合 //
	else{
		pParent->m_lpDDGUID = NULL;
	}

	// GUID を文字列に変換する //
	GuidToString(pParent->m_lpDDGUID, pParent->m_strDDGUID);

	// Ｚバッファの列挙のためにインターフェースを格納する //
	pThis->m_lpParentD3D = lpD3D;

	// Direct3D デバイスの列挙を行う  //
	// この関数は失敗しても、構わない //
	lpD3D->EnumDevices(pThis->EnumD3D, pThis);

	// 一応、ポインタを無効化する //
	pThis->m_lpParentD3D = NULL;

	SAFE_RELEASE(lpD3D);	// Direct3D   インターフェース
	SAFE_RELEASE(lpDD);		// DirectDraw インターフェース

	return TRUE;
}


// Direct3D.Device を列挙する //
HRESULT WINAPI
	CGrpEnum::EnumD3D(TCHAR				*sDesc	// デバイスのテクスチャ記述
					, TCHAR				*sName	// デバイス名のアドレス
					, D3DDEVICEDESC7	*pDesc	// デバイスの能力
					, VOID				*pArg)	// this ポインタの格納先
{
	GrpDrvInfo		*pDrvInfo;
	GrpDrvInfo		*pParent;
	CGrpEnum		*pThis;
	DWORD			RenderDepth;
	HRESULT			hr;

	// this ポインタを生成する //
	pThis = (CGrpEnum *)pArg;

	// 内部エラーにつき、列挙を停止する //
	if(NULL == pThis) return D3DENUMRET_CANCEL;

	// 列挙できる最大数を超えたので、列挙を停止する //
	if(pThis->m_NumEnumDrivers >= ENUM_GRAPHIC_DRIVER){
		return D3DENUMRET_CANCEL;
	}

	// データの書き込み先を決定する //
	pDrvInfo = pThis->m_DrvInfo + pThis->m_NumEnumDrivers;

	// 親情報へのポインタをセットする //
	pParent = &(pThis->m_ParentInfo);

	// ゼロ初期化する //
	memset(pDrvInfo, 0, sizeof(GrpDrvInfo));

	// このデバイスがハードウェアかどうかを格納する //
	if(pDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION){
		pDrvInfo->m_bIsHardware = TRUE;		// ハードウェアデバイス
	}
	else{
		pDrvInfo->m_bIsHardware = FALSE;	// こいつは遅いけど、いいの？
	}

	// ＤＤｒａｗドライバの情報を格納する(HAL) //
	pDrvInfo->m_DDrawDriverCaps = pParent->m_DDrawDriverCaps;

	// ＤＤｒａｗドライバの情報を格納する(HEL) //
	pDrvInfo->m_DDrawHELCaps = pParent->m_DDrawHELCaps;

	// ピクセルフォーマットを格納する //
	pDrvInfo->m_PixelFormat = pParent->m_PixelFormat;

	// ＤｉｒｅｃｔＤｒａｗドライバのＧＵＩＤを格納する //
	if(NULL != pParent->m_lpDDGUID){
		pDrvInfo->m_DDGUID   = pParent->m_DDGUID;			// GUID 実体
		pDrvInfo->m_lpDDGUID = &(pDrvInfo->m_DDGUID);		// GUIDへのポインタ
	}
	strcpy(pDrvInfo->m_strDDGUID, pParent->m_strDDGUID);	// GUID 文字列

	// Ｄｉｒｅｃｔ３ＤデバイスのＧＵＩＤを格納する //
	pDrvInfo->m_D3DGUID   = pDesc->deviceGUID;		// GUID 実体の格納
	pDrvInfo->m_lpD3DGUID = &(pDrvInfo->m_D3DGUID);	// GUID へのポインタ
	GuidToString(&pDrvInfo->m_D3DGUID, pDrvInfo->m_strD3DGUID);	// GUID 文字列

	// Ｄ３Ｄデバイスの情報を格納する //
	pDrvInfo->m_D3DDeviceDesc = *pDesc;

	// デフォルトのドライバではない場合 //
	if(NULL != pDrvInfo->m_lpDDGUID){
		// デフォルトのドライバ以外は、非ハードウェアの列挙を行わない //
		if(FALSE == pDrvInfo->m_bIsHardware) return D3DENUMRET_OK;

		// つまり、ＨＡＬだけになるので、ＤＤｒａｗのドライバ名を格納する //
		lstrcpyn(pDrvInfo->m_strDesc, pParent->m_strDesc, GRPDEV_NAME_LENGTH);
	}
	// DDrawドライバがデフォルトの場合 //
	else{
		// デバイス記述文字列を３Ｄのものにする //
		lstrcpyn(pDrvInfo->m_strDesc, sName, GRPDEV_NAME_LENGTH);
	}

	// デバイス選別用関数が指定されている場合、呼び出す //
	if(pThis->m_pConfirmFunction){
		// 引数は、(DDrawCaps, D3DCaps) の順になっている //
		if(FALSE == pThis->m_pConfirmFunction(&(pDrvInfo->m_DDrawDriverCaps)
											, &(pDrvInfo->m_D3DDeviceDesc))){
			return D3DENUMRET_OK;
		}
	}

	// ３Ｄカードが１６ビットをサポートしているかどうかを調べる //
	RenderDepth = pDrvInfo->m_D3DDeviceDesc.dwDeviceRenderBitDepth;
	if(!(RenderDepth & DDBD_16)) return D3DENUMRET_OK;

	// Ｚバッファのフォーマットを列挙する //
	hr = pThis->m_lpParentD3D->EnumZBufferFormats(
									pDesc->deviceGUID
								  , pThis->EnumZBuffer
								  , pDrvInfo);
	if(FAILED(hr)){
		if(DDERR_NOZBUFFERHW != hr) return D3DENUMRET_OK;
	}

	// ドライバ１個列挙できた //
	pThis->m_NumEnumDrivers += 1;

	return D3DENUMRET_OK;
}


// ディスプレイモードを列挙する(640x480x16 をサポートしているか) //
HRESULT WINAPI
	CGrpEnum::EnumDisplayMode(DDSURFACEDESC2 *pddsd	// 周波数＆フォーマット
							, VOID *pArg)			// PixelFormat
{
	DDPIXELFORMAT	*pddpf;
	DWORD			Width, Height, Bpp;
	BYTE			r, g, b;
	GrpPixelFormat	*pFormat;

	// ポインタが無効(内部エラー) //
	if(NULL == pArg){
		return DDENUMRET_CANCEL;
	}

	// ポインタをセットする //
	pFormat = (GrpPixelFormat *)pArg;		// ビット並びの格納先
	pddpf   = &(pddsd->ddpfPixelFormat);	// ピクセルフォーマット参照用

	// 解像度・ビット深度を代入する //
	Width  = pddsd->dwWidth;		// 画面の幅
	Height = pddsd->dwHeight;		// 画面の高さ
	Bpp    = pddpf->dwRGBBitCount;	// ピクセルあたりのビット数

	// 必要な解像度が見つかった //
	if(Width==640 && Height==480 && Bpp==16){
		// それぞれの色に対するビット数を求める //
		r = GetMask2BitCount(pddpf->dwRBitMask);	// 赤のビット数
		g = GetMask2BitCount(pddpf->dwGBitMask);	// 緑のビット数
		b = GetMask2BitCount(pddpf->dwBBitMask);	// 青のビット数

		pFormat->m_NumRBit    = r;			// 赤のビット数
		pFormat->m_NumGBit    = g;			// 緑のビット数
		pFormat->m_NumBBit    = b;			// 青のビット数
		pFormat->m_NumRGBBits = r + g + b;	// 全ての色の総ビット数

		// もう、列挙する必要はない //
		return DDENUMRET_CANCEL;
	}

	return DDENUMRET_OK;
}


// Ｚバッファのフォーマットを列挙する //
HRESULT WINAPI
	CGrpEnum::EnumZBuffer(LPDDPIXELFORMAT lpDDPixFmt	// ピクセルフォーマット
						, LPVOID pArg)					// ドライバ情報構造体
{
	GrpDrvInfo		*pDrvInfo;

	// 内部エラー(引数がおかしい) のため、列挙を中断する //
	if(NULL == pArg) return D3DENUMRET_CANCEL;

	// DDPIXELFORMAT へのポインタとなるようにキャストする //
	pDrvInfo = (GrpDrvInfo *)pArg;

	// 構造体間の代入を行う //
	pDrvInfo->m_ZBufferFormat = *lpDDPixFmt;			// ピクセルフォーマット
	pDrvInfo->m_ZBufferDepth  = lpDDPixFmt->dwZBufferBitDepth;	// ビット深度

	// 最適な値が検出されたので、列挙を中止する //
	if(16 == pDrvInfo->m_ZBufferDepth){
		return D3DENUMRET_CANCEL;
	}

	// なるべく１６ビットになるようにする //
	return DDENUMRET_OK;
}


// デバイスのソート用関数 //
int CGrpEnum::DeviceSortFunction(const void *p1, const void *p2)
{
	GrpDrvInfo		*Drv1 = (GrpDrvInfo *)p1;
	GrpDrvInfo		*Drv2 = (GrpDrvInfo *)p2;

	if(Drv1->m_bIsHardware){
		if(Drv2->m_bIsHardware) return  0;
		else                    return -1;
	}

	return 1;
}



} // namespace Pbg
