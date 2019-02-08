/*
 *   CFont.h   : フォント
 *
 */

#ifndef CFONT_INCLUDED
#define CFONT_INCLUDED "フォント : Version 0.03 : Update 2001/09/14"

/*  [更新履歴]
 *    Version 0.03 : 2001/09/14 : 微調整
 *
 *    Version 0.02 : 2001/05/26 : 演算部の高速化＆背景色合成機能
 *                              : 計算量は[９／４倍]にアップ
 *
 *    Version 0.01 : 2001/02/17 : クラス設計
 */



#include "PbgType.h"
#include "CDIB32.h"



namespace Pbg {


/***** [ 定数 ] *****/
#define FONTID_DEFAULT			0x00		// デフォルトのフォント
#define FONTID_MS_GOTHIC		0x01		// ＭＳゴシック(サイズ一定)
#define FONTID_MS_MINCHO		0x02		// ＭＳ明朝(サイズ一定)



/***** [クラス定義] *****/

// α情報付きフォント //
class CFont : public CDIB32 {
public:
	BOOL SetText(char *pString);	// 文字列をセットする

	BOOL SetSize(int Size);					// フォントのサイズをセットする
	BOOL SetColor(COLORREF Color);			// 文字色をセットする
	BOOL SetBkBlendColor(COLORREF Color);	// 背景の合成色をセットする
	BOOL SetFontID(BYTE FontID);			// フォント名をセットする
	BOOL Update(void);						// フォント情報を更新する

	// 画像幅を取得する //
	DWORD GetWidth(void){
		return m_dwWidth / 3;
	};

	// 画像の高さを取得する //
	DWORD GetHeight(void){
		return m_dwHeight / 3;
	};

	CFont();		// コンストラクタ
	~CFont();		// デストラクタ


private:
	// 指定されたフォントで描画するのに必要なサイズを求める //
	// pWidth, pHeight : ＤＩＢのサイズ格納先へのポインタ   //
	// pLine           : 縦幅の最大値格納先へのポインタ     //
	BOOL GetTextSize(HFONT hFont, LONG *pWidth, LONG *pHeight, LONG *pLine);


private:
	int			m_FontSize;			// 文字のサイズ
	COLORREF	m_FontColor;		// 文字の色
	COLORREF	m_BkBlendColor;		// 背景の合成色
	char		*m_pString;			// 文字列
	char		*m_pFontName;		// フォント名

	BOOL		m_bNeedUpdate;		// 更新が必要か
};



} // namespace Pbg



#endif
