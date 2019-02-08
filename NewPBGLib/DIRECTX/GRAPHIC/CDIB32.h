/*
 *   CDIB32.h   : RGBA画像管理
 *
 */

#ifndef CDIB32_INCLUDED
#define CDIB32_INCLUDED "RGBA画像管理 : Version 0.02 : Update 2001/09/14"

/*  [更新履歴]
 *    Version 0.03 : 2001/09/14 : 改良する
 *
 *    Version 0.02 : 2001/03/04 : Clear() を追加
 *                              : DrawA() を FBOOL にする
 *
 *    Version 0.01 : 2001/02/17 : クラス設計
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/

// RGBA 情報を持つ画像管理用クラス //
class CDIB32 {
public:
	BOOL Create(int Width, int Height);			// ＤＩＢ作成
	void Cleanup(void);							// オブジェクト解放

	FBOOL DrawA(int x, int y, CDIB32 &Src);		// α付き画像転送
	FVOID Clear(COLORREF Color);				// 指定色で塗りつぶし

	FBOOL HasAlpha(void);				// α値をもっていれば真
	FVOID SetAlpha(BOOL bHasAlpha);		// α値を有効にする

	FBOOL Save(char *pBMPFileName);		// ファイルに書き出す


	// ＤＩＢへのアクセス //
	inline HDC    GetDC(void);			// デバイスコンテキスト取得
	inline DWORD *GetTarget(void);		// ビット列へのポインタを取得
	inline DWORD  GetPitch(void);		// 画像のピッチを取得する

	// 画像情報の取得 //
	virtual DWORD GetWidth(void);	// 画像幅を取得する
	virtual DWORD GetHeight(void);	// 画像の高さを取得する

	virtual BOOL  Update(void);		// 更新する(コピー前に呼び出される)

	CDIB32();				// コンストラクタ
	virtual ~CDIB32();		// デストラクタ


protected:
	DWORD		m_dwWidth;		// 画像の幅
	DWORD		m_dwHeight;		// 画像の高さ

	DWORD		*m_pData;		// 画像のビット列
	LONG		m_lPitch;		// 画像のピッチ

	BOOL		m_bHasAlpha;	// α値を持っていれば真

	HBITMAP		m_hBMP;		// ビットマップハンドル
	HDC			m_hDC;		// デバイスコンテキスト
	HGDIOBJ		m_hOldObj;	// 以前に関連づけされていたオブジェクト
};



// デバイスコンテキスト取得 //
inline HDC CDIB32::GetDC(void)
{
	return m_hDC;
}


// ビット列へのポインタを取得 //
inline DWORD *CDIB32::GetTarget(void)
{
	return m_pData;
}


// 画像のピッチを取得する //
inline DWORD CDIB32::GetPitch(void)
{
	return m_lPitch;
}



}



#endif
