/*
 *   CTexture.h   : テクスチャ管理クラス
 *
 */

#ifndef CTEXTURE_INCLUDED
#define CTEXTURE_INCLUDED "テクスチャ管理クラス : Version 0.04 : Update 2001/10/08"

/*  [更新履歴]
 *    Version 0.04 : 2001/10/08 : α関連の機能を強化
 *
 *    Version 0.03 : 2001/07/21 : １６ビットテクスチャで透過指定をした場合、
 *                              : 元画像の切り捨てられたビットによって同じ色に
 *                              : なってしまう場合、その色も透過色になってしまう
 *
 *    Version 0.02 : 2001/03/06 : テクスチャ定数(GRPTEX_DEFAULT)の追加
 *
 *    Version 0.01 : 2001/02/25 : クラス設計
 */



#include "PbgType.h"
#include "CDIB32.h"



/***** [ 定数 ] *****/
#define GRPTEXTURE_MAX			60				// テクスチャ Surface の最大数

#define GRPTEX_DEFAULT			0x00000000		// デフォルト
#define GRPTEX_TRANSWHITE		0x00000001		// 白を透過色とする
#define GRPTEX_TRANSBLACK		0x00000002		// 黒を透過色とする
#define GRPTEX_32BPP			0x00000004		// ３２ビットで作成
#define GRPTEX_USEPALETTE		0x00000008		// ２５６色テクスチャ
#define GRPTEX_16BPP			0x00000010		// １６ビットで作成



namespace Pbg {



/***** [クラス定義] *****/

// テクスチャフォーマットの検索情報 //
typedef struct tagTextureSearchInfo {
	DWORD		Bpp;				// テクスチャのビット深度
//	BOOL		bUseAlpha;			// αを使用するか(カラーキー等)
	DWORD		AlphaDepth;			// αに必要なビット数
	BOOL		bUsePalette;		// パレットを使用するか
	BOOL		bFoundGoodFormat;	// 適切なフォーマットが見つかったか

	DDPIXELFORMAT	*pddpf;		// ピクセルフォーマットの書き込み先
} TextureSearchInfo;


class CTextureContainer;	// 相互参照のため...


// テクスチャ１枚管理用クラス //
class CGrpTexture {
	friend class CTextureContainer;

public:
	// 書き込み用 DIB32 を取得する //
	CDIB32 *Lock(void);

	// テクスチャを実際に作成する //
	BOOL Unlock(LPDIRECT3DDEVICE7 lpDevice, DWORD Flags);

	// このテクスチャの Surface を取得する //
	LPSURFACE GetSurface(void);

	// 参考：Restore() はＢＭＰが関連づけられていないときは、 //
	// 何も行わずに真を返すものとする                         //
	BOOL Restore(LPDIRECT3DDEVICE7 lpDevice);	// Surface を再び作成する
	void Release(void);							// テクスチャ＆画像を破棄

	CGrpTexture();	// コンストラクタ
	~CGrpTexture();	// デストラクタ


private:
	// テクスチャ作成に必要な要素をセットする //
	void SetTextureFlags(DWORD Flag);

	BOOL CopyDIBImage(void);	// ＢＭＰイメージをテクスチャに転送する
	BOOL CopyRGBAImage(void);	// αを含むイメージをテクスチャに転送する

	// ビットマスクから、シフト値を取得する //
	static FVOID BitMask2Shift(DWORD Mask, DWORD &ShiftR, DWORD &ShiftL);

	// 使用できるテクスチャのフォーマットを検索条件から絞り込む //
	static HRESULT CALLBACK EnumTextureFormat(
								DDPIXELFORMAT	*pddpf
							  , VOID			*pParam);


private:
	DWORD		m_Width;		// テクスチャの幅
	DWORD		m_Height;		// テクスチャの高さ
	DWORD		m_Bpp;			// テクスチャのビット深度
	DWORD		m_Flags;		// テクスチャの性質フラグ
	DWORD		m_AlphaDepth;	// αに必要なビット数

	LPSURFACE	m_lpSurface;	// テクスチャSurface
	CDIB32		*m_pDIB;		// ＤＩＢイメージ
};


class CTextureContainer {
public:
	// ＢＭＰイメージからテクスチャを作成する(ＢＭＰファイル) //
	BOOL CreateTexture(DWORD	TextureID
					 , char		*pBMPFileName
					 , DWORD	Flag);

	// ＢＭＰイメージからテクスチャを作成する(圧縮ファイルＩＤ) //
	BOOL CreateTextureP(DWORD	TextureID
					  , char	*pPackFileName
					  , char	*pFileID
					  , DWORD	Flag);

	// ＢＭＰイメージからテクスチャを作成する(圧縮ファイルＮｏ) //
	BOOL CreateTextureP(DWORD	TextureID
					  , char	*pPackFileName
					  , DWORD	FileNo
					  , DWORD	Flag);

	// ＢＭＰイメージからテクスチャを作成する(りそ圧縮ファイルＩＤ) //
	BOOL CreateTexturePR(DWORD	TextureID
					  , char	*pPackFileName
					  , char	*pFileID
					  , DWORD	Flag);

	// ＢＭＰイメージからテクスチャを作成する(りそ圧縮ファイルＮｏ) //
	BOOL CreateTexturePR(DWORD	TextureID
					  , char	*pPackFileName
					  , DWORD	FileNo
					  , DWORD	Flag);

	BOOL RestoreAllTexture(void);	// 全てのテクスチャを修復する
	void ReleaseAllTexture(void);	// 全てのテクスチャを開放する

	// テクスチャ Surface を取得する //
	LPSURFACE GetTextureSurface(DWORD TextureID);

	// テクスチャフォーマットを出力する //
	BOOL OutputTextureFormat(void);

	CTextureContainer();	// コンストラクタ
	~CTextureContainer();	// デストラクタ


protected:
	// このクラスにテクスチャを関連づける //
	BOOL Set3DDeviceForTexture(LPDIRECT3DDEVICE7 lpD3Dev);


private:
	// テクスチャフォーマットを列挙し、バッファにデータを追加する //
	static HRESULT CALLBACK EnumTextureFormatForOutput(
								DDPIXELFORMAT	*pddpf
							  , VOID			*pStringBuf);


private:
	CGrpTexture			m_Texture[GRPTEXTURE_MAX];	// テクスチャ管理クラス
	LPDIRECT3DDEVICE7	m_lpDevice;					// Direct3D.Device
};



}



#endif
