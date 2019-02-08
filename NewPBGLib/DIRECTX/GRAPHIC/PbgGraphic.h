/*
 *   PbgGraphic.h   : グラフィック管理クラス
 *
 */

#ifndef PBGGRAPHIC_INCLUDED
#define PBGGRAPHIC_INCLUDED "グラフィック管理クラス : Version 0.05 : Update 2001/07/27"


/*  [更新履歴]
 *    Version 0.05 : 2001/07/27 : ->Restore() 呼び出しで、表と裏のSurface を
 *                              : ゼロ初期化するようにした
 *
 *    Version 0.04 : 2001/02/27 : このクラスを２つ同時に使用することは
 *                              : ないので、メンバ変数を全て static にする
 *
 *    Version 0.03 : 2001/02/25 : ２Ｄ部の完成＆３Ｄ初期化を除いて完成
 *                              : テクスチャクラスと接続する
 *
 *    Version 0.02 : 2001/02/16 : 大幅に仕様を修正した
 *    Version 0.01 : 2001/02/15 : クラス設計
 */



#include "PbgType.h"
#include "CGrpEnum.h"
#include "CGrpSurface.h"
#include "CTexture.h"
#include "CDIB32.h"
#include "CGrpLoader.h"
#include "CFont.h"
#include "CGrpEffect.h"
#include "PbgError.h"
#include "Matrix.h"



/***** [ 定数 ] *****/

// Ｓｕｒｆａｃｅ //
#define SURFACE_MAX		30		// 通常の Surface の最大数

// 列挙 //
#define DDRAW_ENUM_MAX		4		// DirectDraw インターフェース列挙最大数
#define D3D_ENUM_MAX		8		// Direct3D インターフェース列挙最大数

// レンダリングステート一括指定 //
#define GRPST_NORMAL		0x00	// 通常の描画
#define GRPST_COLORKEY		0x01	// 通常の描画＋カラーキー
#define GRPST_ALPHASTD		0x02	// 通常の半透明
#define GRPST_ALPHASTDCK	0x03	// 通常の半透明＋カラーキー
#define GRPST_ALPHAONE		0x04	// １：１加算半透明
#define GRPST_ALPHAADD		0x05	// α値付きの加算半透明



namespace Pbg {



/***** [クラス定義] *****/

class CGrpSurface;


// DDraw & D3D 管理クラス //
class CGraphic : public CGrpEnum, public CTextureContainer {
public:
	// フルスクリーン排他モードで初期化する //
	BOOL Initialize(HWND hWindow, DWORD DeviceID);

	// 全インターフェースを解放する //
	void Cleanup(void);

	// 幅と高さを指定して、Surface を作成する //
	BOOL CreateSurface(CGrpSurface **ppBuffer, int Width, int Height);

	// 全ての Surface (Prim, Back, Z, OffScreen, Texture) を Resore する //
	// なお、テクスチャ＆オフスクリーンについては ＢＭＰ復旧も行う       //
	BOOL RestoreAllSurfaces(void);

	// オフスクリーン Surface 管理クラスを解放する //
	void ReleaseOffsSurface(void);					// 全て解放
	void ReleaseOffsSurface(CGrpSurface *pBuffer);	// １つだけ解放

	// ３Ｄ関連 //
	BOOL Begin3DScene(void);			// ３Ｄを使用するモードに移行する
	BOOL End3DScene(void);				// ３Ｄの使用を終了する
	BOOL SetViewport(RECT *rcTarget);	// 描画対象となる矩形をセットする
	void GetViewport(RECT *rcTarget);	// 現在の描画対象矩形を取得する

	FBOOL SetTexture(DWORD TextureID);	// テクスチャをセットする
	FVOID SetRenderStateEx(DWORD Flag);	// レンダリングステートを一括変更
	FVOID SetZCompare(BOOL bEnableZ);	// Ｚ比較を有効にするかの設定

	// ポリゴンの描画(D3DTLVERTEX) : トランスフォーム＆ライティング済み //
	IBOOL DrawPrimitive(D3DPRIMITIVETYPE	dptPrimitiveType	// 描画方法
					  , LPD3DTLVERTEX		lpvVertices			// 頂点配列
					  , DWORD				dwVertexCount);		// 頂点数

	// ポリゴンの描画(D3DLVERTEX) : 未トランスフォーム＆ライティング済み//
	IBOOL DrawPrimitive(D3DPRIMITIVETYPE	dptPrimitiveType	// 描画方法
					  , LPD3DLVERTEX		lpvVertices			// 頂点配列
					  , DWORD				dwVertexCount);		// 頂点数

	// ポリゴンの描画(D3DLVERTEX) : 未トランスフォーム＆未ライティング//
	IBOOL DrawPrimitive(D3DPRIMITIVETYPE	dptPrimitiveType	// 描画方法
					  , LPD3DVERTEX			lpvVertices			// 頂点配列
					  , DWORD				dwVertexCount);		// 頂点数

	// Direct3D Device を取得する //
	LPDIRECT3DDEVICE7 Get3DDevice(void){
		return m_lpD3Dev;
	};

	// 全ての変換行列を単位行列にする //
	FBOOL ResetAllMatrix(void);

	// ワールド変換行列をセットする //
	FBOOL SetWorldMatrix(CMatrix &rMatrix);

	// ビュー変換行列をセットする               //
	// arg : from  カメラの座標                 //
	//     : at    見つめる座標                 //
	//     : c_up  カメラの上方向を示すベクトル //
	FBOOL SetViewMatrix(D3DVECTOR &from, D3DVECTOR &at, D3DVECTOR &c_up);

	// 射影変換行列をセットする                            //
	// arg  : nearZ   前クリップ平面のＺ座標               //
	//      : farZ    後クリップ平面のＺ座標               //
	//      : fov     視野                                 //
	// 参考 : アスペクト比は現在のクリッピング枠を使用する //
	FBOOL SetProjectionMatrix(D3DVALUE nearZ, D3DVALUE farZ, D3DVALUE fov);

	// ２Ｄ関連 //
	BOOL Cls(DWORD Color = 0);	// 画面を指定色でクリアする
	BOOL Flip(void);			// 表画面と裏画面を入れ替える

	// 裏画面に画像を転送する //
	FBOOL BltC(RECT *src, int x, int y, const CGrpSurface *pSurf);// カラーキー付き
	FBOOL BltN(RECT *src, int x, int y, const CGrpSurface *pSurf);// カラーキー無し

	// ガンマランプ管理(フェードＩ/Ｏとか、明るさの調整とか) //
	BOOL SetGammaRamp(LPDDGAMMARAMP pGamma);	// ガンマランプを設定する
	BOOL GetGammaRamp(LPDDGAMMARAMP pGamma);	// ガンマランプを取得する

	// 表画面を CDIB32 に読み込む //
	BOOL Capture(CDIB32 *pTarget);

	// デバッグ用テキストぽいぽい //
	FVOID TextOut(int x, int y, char *pStr);

	// 仮です。デバッグ終了後に消去してください //
	BOOL BitBltEx(CDIB32 *Src, int x, int y);

	CGraphic();		// コンストラクタ
	~CGraphic();	// デストラクタ


private:
	// IDirectDraw7 インターフェース作成      //
	// 注意：インターフェースの解放は行わない //
	BOOL CreateDDraw(GUID *pDDGUID);

	// プライマリ＆バック Surface 作成 ＆ フルスクリーンに移行する //
	// 注意：インターフェースの解放は行わない　                    //
	BOOL CreateFSBuffer(DWORD Width, DWORD Height, DWORD Bpp);

	// IDirect3D7 & IDirect3DDevice7          //
	// 注意：インターフェースの解放は行わない //
	BOOL CreateD3D(GrpDrvInfo *pDrvInfo);

	// Ｚバッファを作成し、裏画面と関連づける //
	BOOL CreateZBuffer(LPDDPIXELFORMAT lpddpf, BOOL bIsHW);

	// ビューポートに収まるようにクリッピングを掛ける         //
	// [戻り値] TRUE:クリッピングされた  FALSE:完全に範囲外   //
	// [ 引数 ] pSrc:元となる矩形  pDest:クリッピング後の矩形 //
	static FBOOL ClipRECT(int *pX, int *pY, RECT *pSrc, RECT *pDest);


private:
	static LPDIRECTDRAW7			m_lpDD;		// DirectDraw インターフェース
	static LPDIRECTDRAWGAMMACONTROL	m_lpGamma;	// DDGammaCtrl インターフェース
	static LPDIRECT3D7				m_lpD3D;	// Direct3D インターフェース
	static LPDIRECT3DDEVICE7		m_lpD3Dev;	// Direct3D Device インターフェース

	static LPSURFACE	m_lpPrim;	// プライマリ Surface
	static LPSURFACE	m_lpBack;	// バック Surface
	static LPSURFACE	m_lpZBuf;	// Ｚバッファ

	// オフスクリーン Surface クラスへのポインタのポインタの配列 //
	static CGrpSurface				**m_ppSurface[SURFACE_MAX];

	static int		m_ClipXMin;			// 描画矩形(左)
	static int		m_ClipXMax;			// 描画矩形(右)
	static int		m_ClipYMin;			// 描画矩形(上)
	static int		m_ClipYMax;			// 描画矩形(下)
	static int		m_ClipWidth;		// 描画矩形の幅
	static int		m_ClipHeight;		// 描画矩形の高さ

	static DWORD	m_RenderWidth;		// 画面の横幅
	static DWORD	m_RenderHeight;		// 画面の高さ
	static DWORD	m_RenderBpp;		// 画面のビット深度(16, 24, 32)
	static HWND		m_hWindow;			// ウィンドウハンドル

	static BOOL		m_bCreated;			// 多重作成の禁止用フラグ
};



// ポリゴンの描画(D3DTLVERTEX) : トランスフォーム＆ライティング済み頂点 //
IBOOL CGraphic::DrawPrimitive(
						D3DPRIMITIVETYPE	dptPrimitiveType	// 描画方法
					  , LPD3DTLVERTEX		lpvVertices			// 頂点配列
					  , DWORD				dwVertexCount)		// 頂点数
{
	HRESULT		hr;

#ifdef PBG_DEBUG
	if(NULL == m_lpD3Dev){
		PbgError("DrawPrimitive(TLVERTEX) : デバイスが作成されていない");
		return FALSE;
	}
#endif

	// 描いておしまいっ //
	hr = m_lpD3Dev->DrawPrimitive(dptPrimitiveType	// 描画方法
								, D3DFVF_TLVERTEX	// 頂点フォーマット
								, lpvVertices		// 頂点配列
								, dwVertexCount		// 頂点数
								, 0);				// 描画フラグ

#ifdef PBG_DEBUG
	if(FAILED(hr)) return FALSE;
#endif

	return TRUE;
}


// ポリゴンの描画(D3DLVERTEX) : 非トランスフォーム＆ライティング済み頂点 //
IBOOL CGraphic::DrawPrimitive(
						D3DPRIMITIVETYPE	dptPrimitiveType	// 描画方法
					  , LPD3DLVERTEX		lpvVertices			// 頂点配列
					  , DWORD				dwVertexCount)		// 頂点数
{
	HRESULT		hr;

#ifdef PBG_DEBUG
	if(NULL == m_lpD3Dev){
		PbgError("DrawPrimitive(LVERTEX) : デバイスが作成されていない");
		return FALSE;
	}

#endif

	// 描いておしまいっ //
	hr = m_lpD3Dev->DrawPrimitive(dptPrimitiveType	// 描画方法
								, D3DFVF_LVERTEX	// 頂点フォーマット
								, lpvVertices		// 頂点配列
								, dwVertexCount		// 頂点数
								, 0);				// 描画フラグ

#ifdef PBG_DEBUG
	if(FAILED(hr)) return FALSE;
#endif

	return TRUE;
}


// ポリゴンの描画(D3DLVERTEX) : 未トランスフォーム＆未ライティング//
IBOOL CGraphic::DrawPrimitive(D3DPRIMITIVETYPE	dptPrimitiveType	// 描画方法
						   , LPD3DVERTEX		lpvVertices			// 頂点配列
						   , DWORD				dwVertexCount)		// 頂点数
{
	HRESULT		hr;

#ifdef PBG_DEBUG
	if(NULL == m_lpD3Dev){
		PbgError("DrawPrimitive(VERTEX) : デバイスが作成されていない");
		return FALSE;
	}

#endif

	// 描いておしまいっ //
	hr = m_lpD3Dev->DrawPrimitive(dptPrimitiveType	// 描画方法
								, D3DFVF_VERTEX		// 頂点フォーマット
								, lpvVertices		// 頂点配列
								, dwVertexCount		// 頂点数
								, 0);				// 描画フラグ

#ifdef PBG_DEBUG
	if(FAILED(hr)) return FALSE;
#endif

	return TRUE;
}



} // namespace Pbg



#endif
