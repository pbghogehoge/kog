/*
 *   FontDraw.h   : フォント描画
 *
 */

#ifndef FONTDRAW_INCLUDED
#define FONTDRAW_INCLUDED "フォント描画 : Version 0.01 : Update 2001/04/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/02 : 製作開始
 */



#include "Gian2001.h"



/***** [関数プロトタイプ] *****/

class CTextureFont {
public:
	// フォント描画(通常) //
	static FVOID Draw(int sx, int sy, char *pString, int Size=16);

	// フォント描画(半透明) //
	static FVOID DrawAlpha(int sx, int sy, char *pString, BYTE Alpha, int Size=16);

	// フォント描画(半透明＆トランスフォーム有り)
	static FVOID DrawAT(int ox, int oy, char *pString, BYTE Alpha);

	// コンボ数の表示 //
	static FVOID DrawCombo(int ox, int oy, char *pNum, BYTE Alpha);

	// 数字列の描画 //
	static FVOID DrawNumber(int ox, int oy, char *pNum, int size=24);

	// 小さなフォントで数字列を描画 //
	static FVOID DrawSmallNumber8(int ox, int oy, char *pNum, BYTE a);

	CTextureFont();		// コンストラクタ
	~CTextureFont();	// デストラクタ


private:
	static D3DRECTANGLE		m_TextureUV[256];
};



// ２Ｄ文字列の描画(Begin3DScene-End3DScene の外で使用すること！！ //
FVOID Draw2DFont(Pbg::CGrpSurface *pSurf, int ox, int oy, char *pStr);



/***** [グローバル変数] *****/
extern CTextureFont		g_Font;



#endif
