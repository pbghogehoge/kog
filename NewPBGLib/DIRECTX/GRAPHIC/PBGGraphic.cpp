/*
 *   PbgGraphic.cpp   : グラフィック管理クラス
 *
 */



#include "PbgGraphic.h"
#include "PbgError.h"
#include "PbgMem.h"
#include "PbgCPU.h"



namespace Pbg {

/***** [スタティックメンバ] *****/

LPDIRECTDRAW7            CGraphic::m_lpDD;		// DirectDraw インターフェース
LPDIRECTDRAWGAMMACONTROL CGraphic::m_lpGamma;	// DDGammaCtrl インターフェース
LPDIRECT3D7              CGraphic::m_lpD3D;		// Direct3D インターフェース
LPDIRECT3DDEVICE7        CGraphic::m_lpD3Dev;	// Direct3D Device インターフェース

LPSURFACE CGraphic::m_lpPrim;	// プライマリ Surface
LPSURFACE CGraphic::m_lpBack;	// バック Surface
LPSURFACE CGraphic::m_lpZBuf;	// Ｚバッファ

// オフスクリーン Surface クラスへのポインタのポインタの配列 //
CGrpSurface **CGraphic::m_ppSurface[SURFACE_MAX];

int CGraphic::m_ClipXMin;		// 描画矩形(左)
int CGraphic::m_ClipXMax;		// 描画矩形(右)
int CGraphic::m_ClipYMin;		// 描画矩形(上)
int CGraphic::m_ClipYMax;		// 描画矩形(下)
int CGraphic::m_ClipWidth;		// 描画矩形の幅
int CGraphic::m_ClipHeight;		// 描画矩形の高さ

DWORD CGraphic::m_RenderWidth;	// 画面の横幅
DWORD CGraphic::m_RenderHeight;	// 画面の高さ
DWORD CGraphic::m_RenderBpp;	// 画面のビット深度(16, 24, 32)
HWND  CGraphic::m_hWindow;		// ウィンドウハンドル

BOOL CGraphic::m_bCreated   = FALSE;	// 多重作成の禁止用フラグ



// コンストラクタ //
CGraphic::CGraphic()
{
	if(m_bCreated){
		PbgError("CGraphic::CGraphic() : 内部エラー(クラスの多重作成)");
		return;
	}

	m_bCreated = TRUE;

	m_lpDD    = NULL;	// DirectDraw インターフェース
	m_lpGamma = NULL;	// DDGammaCtrl インターフェース
	m_lpD3D   = NULL;	// Direct3D インターフェース
	m_lpD3Dev = NULL;	// Direct3D Device インターフェース

	m_lpPrim = NULL;	// プライマリ Surface
	m_lpBack = NULL;	// バック Surface
	m_lpZBuf = NULL;	// Ｚバッファ

	// オフスクリーン Surface 管理クラス //
	ZEROMEM(m_ppSurface);

	m_RenderWidth  = 640;		// 画面の横幅
	m_RenderHeight = 480;		// 画面の高さ
	m_RenderBpp    = 16;		// 画面のビット深度(16, 24, 32)
	m_hWindow      = NULL;		// ウィンドウハンドル

	EnumerateDevice();			// デバイスの(仮)列挙
}


// デストラクタ //
CGraphic::~CGraphic()
{
	Cleanup();

	m_bCreated = FALSE;
}


// フルスクリーン排他モードで初期化する //
BOOL CGraphic::Initialize(HWND hWindow, DWORD DeviceID)
{
	RECT			rcTarget;

	// 全オブジェクトを解放する //
	Cleanup();

	// ウィンドウハンドルを記憶する //
	m_hWindow = hWindow;

	// 指定されたデバイスは存在しない //
	if(DeviceID >= m_NumEnumDrivers){
		PbgError("CGraphic::Initialize() : 内部エラー(デバイスＩＤが不正)");
		return FALSE;
	}

#ifdef PBG_DEBUG
	Pbg::CFont		PleaseWait;
	char			buf[4096];

	wsprintf(buf,	"--- 初 期 化 情 報 ---\n"
					"    ドライバ名  : %s\n"
					"    DDraw GUID  : %s\n"
					"    D3D   GUID  : %s\n"
					"    PixelFormat : (R, G, B, Z) = (%d, %d, %d, %d)\n"
					"    MMX 命令    : %sable"
				, m_DrvInfo[DeviceID].m_strDesc
				, m_DrvInfo[DeviceID].m_strDDGUID
				, m_DrvInfo[DeviceID].m_strD3DGUID
				, m_DrvInfo[DeviceID].m_PixelFormat.m_NumRBit
				, m_DrvInfo[DeviceID].m_PixelFormat.m_NumGBit
				, m_DrvInfo[DeviceID].m_PixelFormat.m_NumBBit
				, m_DrvInfo[DeviceID].m_ZBufferDepth
				, CCPUCheck::IsEnableMMX() ? "En" : "Dis");
	PleaseWait.SetText(buf);
	PleaseWait.SetFontID(FONTID_MS_GOTHIC);
	PleaseWait.SetSize(18);
	PleaseWait.SetColor(RGB(255, 255, 255));
	PleaseWait.Update();

	wsprintf(buf, "デバイス %s の初期化", m_DrvInfo[DeviceID].m_strDesc);
	PbgLog(buf);
#endif

	if(FALSE == CreateDDraw(m_DrvInfo[DeviceID].m_lpDDGUID)){
		Cleanup();
		return FALSE;
	}

	// 次に、プライマリ＆バック Surface を作成 //
	if(FALSE == CreateFSBuffer(m_RenderWidth, m_RenderHeight, m_RenderBpp)){
		Cleanup();
		return FALSE;
	}

	Cls();							// 裏画面消去
#ifdef PBG_DEBUG
	BitBltEx(&PleaseWait, 20, 240);	// 待ち画面を再び描画
#endif
	Flip();							// 表裏切り替え

	// ３Ｄ環境を構築する //
	if(FALSE == CreateD3D(&m_DrvInfo[DeviceID])){
		Cleanup();
		return FALSE;
	}
//#ifdef PBG_DEBUG
//	// Millenium 対策のため、追加 (2002/01/10) //
	OutputTextureFormat();
//#endif

	// クリッピング枠を設定する //
	SetRect(&rcTarget, 0, 0, m_RenderWidth, m_RenderHeight);
	SetViewport(&rcTarget);

	return TRUE;
}


// 全インターフェースを解放する //
void CGraphic::Cleanup(void)
{
	// 参考：インターフェースの解放は、作成した順番と「ほぼ逆順に」なる //
//PbgError("Cleanup");
	// テクスチャを切り離す //
	if(NULL != m_lpD3Dev){
		m_lpD3Dev->SetTexture(0, NULL);
	}
//PbgError("1");
	// オフスクリーン Surface 管理クラスの解放 //
	ReleaseOffsSurface();
//PbgError("2");

	// テクスチャの解放 //
	ReleaseAllTexture();
//PbgError("3");

	SAFE_RELEASE(m_lpZBuf);		// Ｚバッファ
//PbgError("4");
	SAFE_RELEASE(m_lpBack);		// バック Surface
//PbgError("5");
	SAFE_RELEASE(m_lpPrim);		// プライマリ Surface
//PbgError("6");

	SAFE_RELEASE(m_lpD3Dev);	// Direct3D Device インターフェース
//PbgError("7");
	SAFE_RELEASE(m_lpD3D);		// Direct3D インターフェース
//PbgError("8");
	SAFE_RELEASE(m_lpGamma);	// DDGammaCtrl インターフェース
//PbgError("9");
	SAFE_RELEASE(m_lpDD);		// DirectDraw インターフェース
//PbgError("10");
}


// 幅と高さを指定して、Surface を作成する //
BOOL CGraphic::CreateSurface(CGrpSurface **ppBuffer, int Width, int Height)
{
	DDSURFACEDESC2		ddsd;
	HRESULT				hr;
	DWORD				i;
	LPSURFACE			pTempSurf;

	// 空きバッファを検索する //
	for(i=0; i<SURFACE_MAX; i++){
		if(NULL == m_ppSurface[i]) break;
	}

	// 作成できる最大数を超えている場合 //
	if(i >= SURFACE_MAX) return FALSE;

	ZEROMEM(ddsd);
	ddsd.dwSize         = sizeof(ddsd);	// 構造体のサイズ
	ddsd.dwWidth        = Width;		// Surface の幅
	ddsd.dwHeight       = Height;		// Surface の高さ
	ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;	// オフスクリーンSurface

	hr = m_lpDD->CreateSurface(&ddsd, &pTempSurf, NULL);
	if(FAILED(hr)){
		return FALSE;
	}

	// Surface 管理クラスを作成 //
	*ppBuffer = NewEx(CGrpSurface(pTempSurf, this));
	if(NULL == (*ppBuffer)){
		SAFE_RELEASE(pTempSurf);
		return FALSE;
	}

	// 呼び出し元のポインタを記憶する //
	m_ppSurface[i] = ppBuffer;

	return TRUE;
}


// 全ての Surface (Prim, Back, Z, OffScreen, Texture) を Resore する //
// なお、テクスチャ＆オフスクリーンについては ＢＭＰ復旧も行う       //
BOOL CGraphic::RestoreAllSurfaces(void)
{
	DWORD			i;
	CGrpSurface		*Target;
	BOOL			bResult;
	HRESULT			hr;

	// DirectX が信用ならんから、一応明示的に Restore() する   //
	// 半透明「ちかちか」の原因がここら辺にあると良いのだが... //
	if(m_lpPrim) m_lpPrim->Restore();
	if(m_lpBack) m_lpBack->Restore();	// これは、恐らく失敗する
	if(m_lpZBuf) m_lpZBuf->Restore();

	// これまでに作成した全ての Surface に対してRestore() を実行する //
	hr = m_lpDD->RestoreAllSurfaces();
	if(FAILED(hr)) return FALSE;

	// オフスクリーン Surface の復旧を行う(ＢＭＰリロード) //
	for(i=0; i<SURFACE_MAX; i++){
		// ポインタが有効な場合 //
		if(NULL != m_ppSurface[i]){
			Target = *(m_ppSurface[i]);

			// 有効なオブジェクトである場合 //
			if(NULL != Target){
				bResult = Target->Restore();		// 再度読み込みを行う
				if(FALSE == bResult) return FALSE;	// 復旧できず
			}
		}
	}

	// Ｚバッファをクリアする                         //
	// 参考：ZBuffer は 0.0(近い) ～ 1.0(遠い) となる //
	if(m_lpD3Dev){
		m_lpD3Dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0, 0);
	}

	Cls();		// 消して
	Flip();		// 切り替えて
	Cls();		// 消す

	// 後は、テクスチャの復旧ができればＯＫ                             //
	return RestoreAllTexture();
}


// オフスクリーン Surface 管理クラスを全て解放する //
void CGraphic::ReleaseOffsSurface(void)
{
	DWORD			i;
	CGrpSurface		*Target;

	for(i=0; i<SURFACE_MAX; i++){
		// オブジェクト作成の形跡がある場合 //
		if(NULL != m_ppSurface[i]){
			Target = *(m_ppSurface[i]);
			DeleteEx(Target);

			*m_ppSurface[i] = NULL;
			m_ppSurface[i] = NULL;
		}
	}
}


// オフスクリーン Surface 管理クラスを１つだけ解放する //
void CGraphic::ReleaseOffsSurface(CGrpSurface *pBuffer)
{
	DWORD			i;
	CGrpSurface		*Target;

	for(i=0; i<SURFACE_MAX; i++){
		if(NULL == m_ppSurface[i]) continue;

		// 目標セット //
		Target = *(m_ppSurface[i]);

		// 削除する対象が見つかった場合 //
		if(Target == pBuffer){
			DeleteEx(Target);

			*m_ppSurface[i] = NULL;
			m_ppSurface[i] = NULL;

			return;
		}
	}
}


// ３Ｄを使用するモードに移行する //
BOOL CGraphic::Begin3DScene(void)
{
	HRESULT			hr;

	// デバイスが作成されていない //
	if(NULL == m_lpD3Dev) return FALSE;

	// シーンを終了する //
	hr = m_lpD3Dev->BeginScene();
	if(SUCCEEDED(hr)) return TRUE;

	// Surface がロストしている場合 //
	if(DDERR_SURFACELOST == hr){
		RestoreAllSurfaces();		// 復旧して

		hr = m_lpD3Dev->BeginScene();	// 再度呼び出す
		if(SUCCEEDED(hr)) return TRUE;
	}

	return FALSE;
}


// ３Ｄの使用を終了する //
BOOL CGraphic::End3DScene(void)
{
	HRESULT			hr;

	// デバイスが作成されていない //
	if(NULL == m_lpD3Dev) return FALSE;

	// シーンを終了する //
	hr = m_lpD3Dev->EndScene();
	if(SUCCEEDED(hr)) return TRUE;

	// Surface がロストしている場合 //
	if(DDERR_SURFACELOST == hr){
		RestoreAllSurfaces();		// 復旧して

		hr = m_lpD3Dev->EndScene();	// 再度呼び出す
		if(SUCCEEDED(hr)) return TRUE;
	}

	return FALSE;
}


// テクスチャをセットする //
FBOOL CGraphic::SetTexture(DWORD TextureID)
{
	HRESULT		hr;

	// ３Ｄデバイスが作成されていない //
	if(NULL == m_lpD3Dev) return FALSE;

	// テクスチャＩＤが不正(テクスチャを無効化する) //
	if(TextureID >= GRPTEXTURE_MAX){
		hr = m_lpD3Dev->SetTexture(0, NULL);
	}
	// 指定されたテクスチャをセットする //
	else{
		hr = m_lpD3Dev->SetTexture(0, GetTextureSurface(TextureID));
	}

	if(FAILED(hr)) return FALSE;
	else           return TRUE;
}


// レンダリングステートを一括変更 //
FVOID CGraphic::SetRenderStateEx(DWORD Flag)
{
//	m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_lpD3Dev->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

//	m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_lpD3Dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_lpD3Dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_lpD3Dev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_lpD3Dev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	m_lpD3Dev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

    m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
    m_lpD3Dev->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
	m_lpD3Dev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);	// default(D3DCULL_CCW)

	switch(Flag){
		case GRPST_NORMAL:		// 通常の描画
//			m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_SPECULAR);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,  FALSE);
		return;

		case GRPST_COLORKEY:	// 通常の描画＋カラーキー
//			m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_SPECULAR);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,  TRUE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHAREF,         0x08);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		return;

		case GRPST_ALPHASTD:	// 通常の半透明
//			m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		return;

		case GRPST_ALPHASTDCK:	// 通常の半透明＋カラーキー
//			m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,  TRUE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHAREF,         0x08);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		return;

		case GRPST_ALPHAONE:	// １：１加算半透明
//			m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		return;

		case GRPST_ALPHAADD:	// α値付きの加算半透明
//			m_lpD3Dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA);
			m_lpD3Dev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		return;

		default:
			PbgError("CGraphic::SetRenderStateEx() : "
						"未定義のレンダリングステート");
		return;
	}
}


// Ｚ比較を有効にするかの設定 //
FVOID CGraphic::SetZCompare(BOOL bEnableZ)
{
	m_lpD3Dev->SetRenderState(D3DRENDERSTATE_ZENABLE, bEnableZ);
}


// 全ての変換行列を単位行列にする //
FBOOL CGraphic::ResetAllMatrix(void)
{
	CMatrix		temp;
	HRESULT		hr;

	temp.Identity();

	hr = m_lpD3Dev->SetTransform(D3DTRANSFORMSTATE_WORLD, temp.GetMatrix());
	if(FAILED(hr)) return FALSE;

	hr = m_lpD3Dev->SetTransform(D3DTRANSFORMSTATE_VIEW, temp.GetMatrix());
	if(FAILED(hr)) return FALSE;

	hr = m_lpD3Dev->SetTransform(D3DTRANSFORMSTATE_PROJECTION, temp.GetMatrix());
	if(FAILED(hr)) return FALSE;

	return TRUE;
}


// ワールド変換行列をセットする //
FBOOL CGraphic::SetWorldMatrix(CMatrix &rMatrix)
{
	HRESULT			hr;

	// ワールド変換行列をセットする //
	hr = m_lpD3Dev->SetTransform(D3DTRANSFORMSTATE_WORLD, rMatrix.GetMatrix());
	if(FAILED(hr)) return FALSE;

	return TRUE;
}


// ビュー変換行列をセットする               //
// arg : from  カメラの座標                 //
//     : at    見つめる座標                 //
//     : c_up  カメラの上方向を示すベクトル //
FBOOL CGraphic::SetViewMatrix(D3DVECTOR &from, D3DVECTOR &at, D3DVECTOR &c_up)
{
	D3DMATRIX		temp;
	D3DVECTOR		vd, up, right;
	D3DVALUE		dot, len;
	HRESULT			hr;

	vd  = at - from;
	len = Magnitude(vd);
	if(len < 1e-6f) return FALSE;

	vd = vd / len;

	// 内積を求める //
	dot = DotProduct(c_up, vd);

	up = c_up - (dot * vd);

	len = Magnitude(up);
	if(len < 1e-6f){
		up = D3DVECTOR(0.0f, 1.0f, 0.0f) - vd.y * vd;

		len = Magnitude(up);
		if(len < 1e-6f){
			up = D3DVECTOR(0.0f, 0.0f, 1.0f) - vd.z * vd;

			len = Magnitude(up);
			if(len < 1e-6f) return FALSE;
		}
	}

	up = up / len;

	// 外積を求める //
	right = CrossProduct(up, vd);

	temp(0, 0) = right.x;
	temp(1, 0) = right.y;
	temp(2, 0) = right.z;

	temp(0, 1) = up.x;
	temp(1, 1) = up.y;
	temp(2, 1) = up.z;

	temp(0, 2) = vd.x;
	temp(1, 2) = vd.y;
	temp(2, 2) = vd.z;

	temp(0, 3) = temp(1, 3) = temp(2, 3) = 0.0f;

	temp(3, 0) = -DotProduct(from, right);
	temp(3, 1) = -DotProduct(from, up);
	temp(3, 2) = -DotProduct(from, vd);
	temp(3, 3) = 1.0f;


	// ビュー変換行列をセットする //
	hr = m_lpD3Dev->SetTransform(D3DTRANSFORMSTATE_VIEW, &temp);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}


// 射影変換行列をセットする                            //
// arg  : nearZ    前クリップ平面のＺ座標              //
//      : farZ     後クリップ平面のＺ座標              //
//      : fov      視野                                //
// 参考 : アスペクト比は現在のクリッピング枠を使用する //
FBOOL CGraphic::SetProjectionMatrix(D3DVALUE nearZ, D3DVALUE farZ, D3DVALUE fov)
{
	D3DVALUE		q, s, c;
	D3DVALUE		aspect;
	CMatrix			temp;
	HRESULT			hr;

	q = farZ - nearZ;
	if(fabs(q) < 0.01f) return FALSE;
	q = farZ / q;

	fov = D3DVAL(fov * 0.5);
	s   = D3DVAL(sinf(fov));
	if(fabs(s) < 0.01f) return FALSE;

	c      = D3DVAL(cosf(fov));
	aspect = D3DVAL(m_ClipHeight) / D3DVAL(m_ClipWidth);

	temp.Zero();

	temp(0, 0) = (aspect) * c / s;		// W
	temp(1, 1) = (1     ) * c / s;		// H
	temp(2, 2) = q;						// Q
	temp(2, 3) = 1.0f;
	temp(3, 2) = -q * nearZ;

	// 射影変換行列をセットする //
	hr = m_lpD3Dev->SetTransform(D3DTRANSFORMSTATE_PROJECTION, temp.GetMatrix());
	if(FAILED(hr)) return FALSE;

	return TRUE;
}


// 描画対象となる矩形をセットする //
BOOL CGraphic::SetViewport(RECT *rcTarget)
{
	D3DVIEWPORT7		View;
	HRESULT				hr;

	// デバイスが作成されていない //
	if(NULL == m_lpD3Dev) return FALSE;

	// ２Ｄ用のクリッピング枠を設定する //
	m_ClipXMin = rcTarget->left;		// 左端
	m_ClipYMin = rcTarget->top;			// 上端
	m_ClipXMax = rcTarget->right  - 1;	// 右端
	m_ClipYMax = rcTarget->bottom - 1;	// 下端

	// クリッピング枠の太さを設定する //
	m_ClipWidth  = rcTarget->right - rcTarget->left;	// 矩形の幅
	m_ClipHeight = rcTarget->bottom - rcTarget->top;	// 矩形の高さ

	View.dwX      = rcTarget->left;	// 左端
	View.dwY      = rcTarget->top;	// 上端
	View.dwWidth  = (rcTarget->right - rcTarget->left);	// 矩形の幅
	View.dwHeight = (rcTarget->bottom - rcTarget->top);	// 矩形の高さ

	View.dvMinZ   = (D3DVALUE)0.0;	// Ｚ最小値
	View.dvMaxZ   = (D3DVALUE)1.0;	// Ｚ最大値

	// 描画対象をセットする //
	hr = m_lpD3Dev->SetViewport(&View);
	if(FAILED(hr)) return FALSE;
	else           return TRUE;
}


// 現在の描画対象矩形を取得する //
void CGraphic::GetViewport(RECT *rcTarget)
{
	SetRect(rcTarget, m_ClipXMin, m_ClipYMin, m_ClipXMax+1, m_ClipYMax+1);
}


// 裏画面に画像を転送する(カラーキー付き) //
FBOOL CGraphic::BltC(RECT *src, int x, int y, const CGrpSurface *pSurf)
{
	HRESULT			hr;
	LPSURFACE		pSurface;
	static RECT		temp;

	pSurface = const_cast<CGrpSurface *>(pSurf)->GetSurface();

	// 描画する必要があるかどうかを調べる //
	if(FALSE == ClipRECT(&x, &y, src, &temp)) return TRUE;

	for(;;){
		// まずは、描画してみようとする //
		hr = m_lpBack->BltFast(x, y						// 描画する座標
								, pSurface				// DDrawSURFACE
								, &temp					// 描画元の矩形
								, DDBLTFAST_SRCCOLORKEY	// カラーキー有りの転送
			 );
		if(DD_OK == hr) return TRUE;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == RestoreAllSurfaces())
			return FALSE;
		}

		// "描画中" 以外は、解決できないエラー //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// 裏画面に画像を転送する(カラーキー無し) //
FBOOL CGraphic::BltN(RECT *src, int x, int y, const CGrpSurface *pSurf)
{
	HRESULT			hr;
	LPSURFACE		pSurface;
	static RECT		temp;

	pSurface = const_cast<CGrpSurface *>(pSurf)->GetSurface();

	// 描画する必要があるかどうかを調べる //
	if(FALSE == ClipRECT(&x, &y, src, &temp)) return TRUE;

	for(;;){
		// まずは、描画してみようとする //
		hr = m_lpBack->BltFast(x, y						// 描画する座標
								, pSurface				// DDrawSURFACE
								, &temp					// 描画元の矩形
								, DDBLTFAST_NOCOLORKEY	// カラーキー無しの転送
			 );
		if(DD_OK == hr) return TRUE;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == RestoreAllSurfaces())
			return FALSE;
		}

		// "描画中" 以外は、解決できないエラー //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// 画面を指定色でクリアする //
BOOL CGraphic::Cls(DWORD Color)
{
	DDBLTFX		ddbltfx;	// ＢＬＴオペ指定用構造体
	HRESULT		hr;			// エラーコードの格納先

	// Surface の指定がおかしい //
	if(NULL == m_lpBack){
		PbgError("CGrpSurface::Cls() : 内部エラー(NULL == Surface)");
		return FALSE;
	}

	ZEROMEM(ddbltfx);						// 構造体のゼロ初期化
	ddbltfx.dwSize      = sizeof(ddbltfx);	// DDBLTFX 構造体のサイズ
	ddbltfx.dwFillColor = Color;			// 塗りつぶす色

	for(;;){
		// 単一色での塗りつぶしを実行する //
		hr = m_lpBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
		if(DD_OK == hr) return TRUE;	// 塗りつぶし完了

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == RestoreAllSurfaces())
				return FALSE;	// 原因不明のエラー
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// 表画面と裏画面を入れ替える //
BOOL CGraphic::Flip(void)
{
	HRESULT		hr;

	// プライマリ Surface が作成されていない //
	if(m_lpPrim == NULL) return FALSE;

	for(;;){
		// 画面の差し替えを行う //
		hr = m_lpPrim->Flip(NULL, DDFLIP_DONOTWAIT);
		if(DD_OK == hr) return TRUE;	// フリップ完了

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == RestoreAllSurfaces())
				return FALSE;	// 原因不明のエラー
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}
}


// ガンマランプを設定する //
BOOL CGraphic::SetGammaRamp(LPDDGAMMARAMP pGamma)
{
	HRESULT		hr;

	// インターフェースの取得が出来ていない場合 //
	if(NULL == m_lpGamma) return FALSE;

	// ガンマランプの設定を行う //
	hr = m_lpGamma->SetGammaRamp(0, pGamma);

	if(FAILED(hr)) return FALSE;	// 失敗
	else           return TRUE;		// 成功
}


// ガンマランプを取得する //
BOOL CGraphic::GetGammaRamp(LPDDGAMMARAMP pGamma)
{
	HRESULT		hr;

	// インターフェースの取得が出来ていない場合 //
	if(NULL == m_lpGamma) return FALSE;

	// ガンマランプの取得を行う //
	hr = m_lpGamma->GetGammaRamp(0, pGamma);

	if(FAILED(hr)) return FALSE;	// 失敗
	else           return TRUE;		// 成功
}


// 表画面を CDIB32 に読み込む //
BOOL CGraphic::Capture(CDIB32 *pTarget)
{
	HDC			hSrc;
	HRESULT		hr;

	for(;;){
		// デバイスコンテキストを取得する //
		hr = m_lpPrim->GetDC(&hSrc);
		if(DD_OK == hr) break;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == RestoreAllSurfaces())
				return FALSE;	// 原因不明のエラー
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}

	BitBlt(pTarget->GetDC(), 0, 0, 640, 480, hSrc, 0, 0, SRCCOPY);
	m_lpPrim->ReleaseDC(hSrc);

	return TRUE;
}


// デバッグ用テキストぽいぽい //
FVOID CGraphic::TextOut(int x, int y, char *pStr)
{
	HDC			hdc;
	HRESULT		hr;

	if(NULL == m_lpBack) return;

	hr = m_lpBack->GetDC(&hdc);
	if(FAILED(hr)) return;

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	::TextOut(hdc, x, y, pStr, strlen(pStr));

	m_lpBack->ReleaseDC(hdc);
}


// 仮関数です。デバッグ終了後に消去してください //
BOOL CGraphic::BitBltEx(CDIB32 *Src, int x, int y)
{
	HDC			hDest;
	HRESULT		hr;
	int			w, h;

	if(FALSE == Src->Update()) return FALSE;

	for(;;){
		// デバイスコンテキストを取得する //
		hr = m_lpBack->GetDC(&hDest);
		if(DD_OK == hr) break;

		// ロストしているので、親に Restore() 要求 //
		if(DDERR_SURFACELOST == hr){
			if(FALSE == RestoreAllSurfaces())
				return FALSE;
		}

		// "描画中" 以外は、解決できないエラーなので、return する //
		if(DDERR_WASSTILLDRAWING != hr){
			return FALSE;
		}
	}

	w = (int)Src->GetWidth();
	h = (int)Src->GetHeight();
	BitBlt(hDest, x, y, w, h, Src->GetDC(), 0, 0, SRCCOPY);

	m_lpBack->ReleaseDC(hDest);

	return TRUE;
}



// IDirectDraw7 インターフェース作成      //
// 注意：インターフェースの解放は行わない //
BOOL CGraphic::CreateDDraw(GUID *pDDGUID)
{
	HRESULT		hr;
	DWORD		flag;

	// エラーログ用の関数名をセット //
	PbgErrorInit("CGraphic::CreateDDraw()");

	// DirectDraw オブジェクトを作成する //
	hr = DirectDrawCreateEx(pDDGUID, (VOID **)&m_lpDD, IID_IDirectDraw7, NULL);
	if(FAILED(hr)){
		PbgErrorEx("DirectDrawCreateEx() に失敗");
		return FALSE;
	}

	// フルスクリーン用にフラグをセットする //
	flag = DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

	// 協調レベルをセットする //
	hr = m_lpDD->SetCooperativeLevel(m_hWindow, flag);
	if(FAILED(hr)){
		PbgErrorEx("SetCooperativeLevel() に失敗");
		return FALSE;
	}

	return TRUE;
}


// プライマリ＆バック Surface 作成 ＆ フルスクリーンに移行する //
// 注意：インターフェースの解放は行わない　                    //
BOOL CGraphic::CreateFSBuffer(DWORD Width, DWORD Height, DWORD Bpp)
{
	DDSURFACEDESC2		ddsd;
	DDSCAPS2			ddscaps = {DDSCAPS_BACKBUFFER, 0, 0, 0};
	HRESULT				hr;

	// エラーログ用の関数名をセット //
	PbgErrorInit("CGraphic::CreateFSBuffer()");

	// ディスプレイモードをセットする //
	hr = m_lpDD->SetDisplayMode(Width, Height, Bpp, 0, 0);
	if(FAILED(hr)){
		PbgErrorEx("SetDisplayMode() に失敗");
		return FALSE;
	}

	// Surface に要求される性能を示す //
	ZEROMEM(ddsd);
	ddsd.dwSize            = sizeof(ddsd);
	ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE
                           | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	// プライマリ Surface を作成する //
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpPrim, NULL);
	if(FAILED(hr)){
		if(hr == DDERR_OUTOFVIDEOMEMORY){
			PbgErrorEx("CreateSurface() に失敗\r\n"
					   "  [原因]  ビデオメモリが足りませんでした");
		}
		else{
			PbgErrorEx("CreateSurface() に失敗");
		}

		return FALSE;
	}

	// バックバッファの取得 //
	hr = m_lpPrim->GetAttachedSurface(&ddscaps, &m_lpBack);
	if(FAILED(hr)){
		PbgErrorEx("GetAttachedSurface に失敗");
		return FALSE;
	}

	// ガンマコントロールの取得(失敗しても構わない) //
	hr = m_lpPrim->QueryInterface(IID_IDirectDrawGammaControl	// 識別子
									, (void **)&m_lpGamma		// 格納先
								 );
	if(FAILED(hr))
		PbgErrorEx("QueryInterface(IID_IDirectDrawGammaControl) に失敗");


	// 画面の解像度に関する変数をセットする //
	m_RenderWidth  = Width;		// 幅
	m_RenderHeight = Height;	// 高さ
	m_RenderBpp    = Bpp;		// ビット深度

	return TRUE;
}


// IDirect3D7 & IDirect3DDevice7 & Z-Buffer を作成する //
// 注意：インターフェースの解放は行わない              //
BOOL CGraphic::CreateD3D(GrpDrvInfo *pDrvInfo)
{
	DWORD				ZBufferDepth;	// Ｚバッファのビット深度
	LPDDPIXELFORMAT		lpddpf;			// Ｚバッファのフォーマット
	HRESULT				hr;				// 戻り値の格納先

	// エラーログ用の関数名をセット //
	PbgErrorInit("CGraphic::CreateD3D()");

	// Direct3D インターフェースを取得する //
	hr = m_lpDD->QueryInterface(IID_IDirect3D7, (VOID **)&m_lpD3D);
	if(FAILED(hr)){
		PbgErrorEx("QueryInterface() に失敗");
		return FALSE;
	}

	// Ｚバッファが必要かどうかを判別する //
	ZBufferDepth = pDrvInfo->m_ZBufferDepth;
	if(0 == ZBufferDepth){
		PbgLog("CGraphic::Initialize() : Ｚバッファは作成しません");
	}
	else{
		// Ｚバッファのフォーマット記述を行う構造体にポインタを合わせる //
		lpddpf = &(pDrvInfo->m_ZBufferFormat);

		// Ｚバッファの作成と接続 //
		if(FALSE == CreateZBuffer(lpddpf, pDrvInfo->m_bIsHardware)){
			PbgErrorEx("Ｚバッファの作成に失敗");
			return FALSE;
		}
	}

	// Direct3DDevice を取得する //
	hr = m_lpD3D->CreateDevice(pDrvInfo->m_D3DGUID, m_lpBack, &m_lpD3Dev);
	if(FAILED(hr)){
		PbgErrorEx("CreateDevice() に失敗");
		return FALSE;
	}

	// テクスチャクラスに送りつける //
	Set3DDeviceForTexture(m_lpD3Dev);

	return TRUE;
}


// Ｚバッファを作成し、裏画面と関連づける //
BOOL CGraphic::CreateZBuffer(LPDDPIXELFORMAT lpddpf, BOOL bIsHW)
{
	HRESULT				hr;
	DDSURFACEDESC2		ddsd;
	DWORD				MemoryFlag;

	// エラーログ用の関数名をセット //
	PbgErrorInit("CGraphic::CreateZBuffer()");

	// Ｚバッファを作成するメモリを指定する //
	if(bIsHW) MemoryFlag = DDSCAPS_VIDEOMEMORY;		// ＶＲＡＭ上におく
	else      MemoryFlag = DDSCAPS_SYSTEMMEMORY;	// システムメモリにおく

	ZEROMEM(ddsd);					// ゼロ初期化する
	ddsd.dwSize  = sizeof(ddsd);	// 構造体のサイズ
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT			// 画面の幅・高さ
				 | DDSD_CAPS  | DDSD_PIXELFORMAT;	// Caps とビット深度

	ddsd.dwWidth  = m_RenderWidth;		// Ｚバッファの横幅
	ddsd.dwHeight = m_RenderHeight;		// Ｚバッファの縦幅

	// Ｚバッファのビット深度 //
	ddsd.ddpfPixelFormat = *lpddpf;

	// 指定されたメモリ上にＺバッファとして作成する //
	ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | MemoryFlag;

	// Ｚバッファを作成する //
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpZBuf, NULL);
	if(FAILED(hr)){
		PbgErrorEx("CreateSurface() に失敗");
		return FALSE;
	}

	// バックバッファにＺバッファを接続する //
	hr = m_lpBack->AddAttachedSurface(m_lpZBuf);
	if(FAILED(hr)){
		PbgErrorEx("AddAttachedSurface に失敗");
		return FALSE;
	}

	return TRUE;
}


// ビューポートに収まるようにクリッピングを掛ける         //
// [戻り値] TRUE:クリッピングされた  FALSE:完全に範囲外   //
// [ 引数 ] pSrc:元となる矩形  pDest:クリッピング後の矩形 //
//          pX: 描画するＸ座標  pY: 描画するＹ座標        //
FBOOL CGraphic::ClipRECT(int *pX, int *pY, RECT *pSrc, RECT *pDest)
{
	static int w;
	static int h;

	h = pSrc->bottom - pSrc->top  - 1;
	w = pSrc->right  - pSrc->left - 1;

	// 完全に外かな？ //
	if( ((*pX) > m_ClipXMax)     || ((*pY) > m_ClipYMax)
	 || ((*pX) + w < m_ClipXMin) || ((*pY) + h < m_ClipYMin) ){
		return FALSE;
	}

	// Ｘクリッピング //
	if((*pX) < m_ClipXMin){		// 左端クリッピング
		if((*pX) + w > m_ClipXMax){	// 右端クリッピング
			pDest->right = (pSrc->right) - ((*pX) + w - m_ClipXMax);
		}
		else{
			pDest->right = pSrc->right;
		}

		pDest->left = pSrc->left + (m_ClipXMin - (*pX));
		(*pX)       = m_ClipXMin;
	}
	else if((*pX) + w > m_ClipXMax){	// 右端クリッピング
		pDest->right = pSrc->right - ((*pX) + w - m_ClipXMax);
		pDest->left  = pSrc->left;
	}
	else{	// Ｘクリップ無し
		pDest->right = pSrc->right;
		pDest->left  = pSrc->left;
	}

	// Ｙクリッピング //
	if((*pY) < m_ClipYMin){	// 上端クリッピング
		if((*pY) + h > m_ClipYMax){	// 下端クリッピング
			pDest->bottom = pSrc->bottom - ((*pY) + h - m_ClipYMax);
		}
		else{
			pDest->bottom = pSrc->bottom;
		}

		pDest->top = pSrc->top + (m_ClipYMin - (*pY));
		(*pY)      = m_ClipYMin;
	}
	else if((*pY) + h > m_ClipYMax){	// 下端クリッピング
		pDest->bottom = pSrc->bottom - ((*pY) + h - m_ClipYMax);
		pDest->top    = pSrc->top;
	}
	else{	// Ｙクリップ無し
		pDest->bottom = pSrc->bottom;
		pDest->top    = pSrc->top;
	}

	return TRUE;
}



} // namespace Pbg
