/*
 *   CGrpSurface.h   : Surface管理クラス
 *
 */

#ifndef CGRPSURFACE_INCLUDED
#define CGRPSURFACE_INCLUDED "Surface管理クラス : Version 0.03 : Update 2001/02/22"

/*  [更新履歴]
 *    Version 0.03 : 2001/02/22 : 微調整(名前空間まわり)
 *    Version 0.02 : 2001/02/16 : クラス設計：一部仕様変更
 *    Version 0.01 : 2001/02/15 : 製作開始
 */



#include "PbgType.h"
#include "PbgGraphic.h"
#include "CDIB32.h"



namespace Pbg {



/***** [ 型の定義 ] *****/
class CGraphic;		// 相互参照用



/***** [クラス定義] *****/
class CGrpSurface {
	friend class CGraphic;

public:
	// 他の Surface の画像を張り付ける //
	FBOOL BltC(RECT *src, int x, int y, const CGrpSurface *pSurf);// カラーキー付き
	FBOOL BltN(RECT *src, int x, int y, const CGrpSurface *pSurf);// カラーキー無し

	CDIB32 *Lock(void);		// 書き込み用 CDIB32 を取得する
	void    Unlock(void);	// 書き込んだデータを反映する

	// ファイルを読み込んで、Surface に関連づける //
	BOOL Load(char *pBMPFileName);					// 通常のファイル
	BOOL LoadP(char *pPackFileName, char *pFileID);	// 圧縮(ID)指定
	BOOL LoadP(char *pPackFileName, DWORD FileNo);	// 圧縮(No)指定

	// CDIB32 画像と接続する(自動的に Restore() を呼び出す) //
	BOOL LoadDIB32(CDIB32 *pDIB32, int sx=0, int sy=0);

	// 画面を指定色で塗りつぶす(ＢＭＰ切り離し) //
	BOOL Cls(DWORD Color = 0, RECT *Target = NULL);

	// カラーキーを設定する //
	BOOL SetColorKey(int x, int y);		// 座標指定
	BOOL SetColorKey(DWORD Color = 0);	// カラーキー指定(デフォルトは黒)

	// Surface を取得する //
	inline const LPSURFACE GetSurface(void){
		return m_lpSurf;
	};

	CGrpSurface(LPSURFACE Surface, CGraphic *pParent);	// コンストラクタ
	~CGrpSurface();										// デストラクタ


private:
	BOOL Restore(void);		// この Surface を Restore する(関連画像のリロード)


private:
	LPSURFACE	m_lpSurf;		// DDrawSurface インターフェース
	CDIB32		*m_pDIB;		// Restore() 用の画像格納先
	CGraphic	*m_pParent;		// RestoreAllSurfaces() 呼び出し用
};



} // namespace Pbg



#endif
