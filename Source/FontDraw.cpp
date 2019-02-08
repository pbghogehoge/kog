/*
 *   FontDraw.cpp   : フォント描画
 *
 */

#include "FontDraw.h"
#include "Gian2001.h"



/***** [スタティックメンバ] *****/

// テクスチャ座標の格納先 //
D3DRECTANGLE CTextureFont::m_TextureUV[256];



/***** [グローバル変数] *****/
CTextureFont		g_Font;



// コンストラクタ //
CTextureFont::CTextureFont()
{
	int			i, x;

	for(i='A', x=0; i<='P'; i++, x+=16){
		m_TextureUV[i].Set(x, 0, x+16, 16, 256);
	}

	for(i='Q', x=0; i<='Z'; i++, x+=16){
		m_TextureUV[i].Set(x, 16, x+16, 32, 256);
	}

	for(i='a', x=0; i<='p'; i++, x+=16){
		m_TextureUV[i].Set(x, 32, x+16, 48, 256);
	}

	for(i='q', x=0; i<='z'; i++, x+=16){
		m_TextureUV[i].Set(x, 48, x+16, 64, 256);
	}

	for(i='0', x=0; i<='9'; i++, x+=16){
		m_TextureUV[i].Set(x, 64, x+16, 80, 256);
	}

	m_TextureUV['!'] .Set(  0,  80,  16,  96, 256);
	m_TextureUV['?'] .Set( 16,  80,  32,  96, 256);
	m_TextureUV['\"'].Set( 32,  80,  48,  96, 256);
	m_TextureUV['#'] .Set( 48,  80,  64,  96, 256);
	m_TextureUV['$'] .Set( 64,  80,  80,  96, 256);
	m_TextureUV['%'] .Set( 80,  80,  96,  96, 256);
	m_TextureUV['&'] .Set( 96,  80, 112,  96, 256);
	m_TextureUV['\''].Set(112,  80, 128,  96, 256);
	m_TextureUV['('] .Set(128,  80, 144,  96, 256);
	m_TextureUV[')'] .Set(144,  80, 160,  96, 256);
	m_TextureUV['+'] .Set(160,  80, 176,  96, 256);
	m_TextureUV['-'] .Set(176,  80, 192,  96, 256);
	m_TextureUV['*'] .Set(192,  80, 208,  96, 256);
	m_TextureUV['/'] .Set(208,  80, 224,  96, 256);
	m_TextureUV['='] .Set(224,  80, 240,  96, 256);
	m_TextureUV[','] .Set(240,  80, 256,  96, 256);

	m_TextureUV['.'] .Set(  0,  96,  16, 112, 256);
	m_TextureUV[';'] .Set( 16,  96,  32, 112, 256);
	m_TextureUV[':'].Set(  32,  96,  48, 112, 256);
	m_TextureUV['@'] .Set( 48,  96,  64, 112, 256);
	m_TextureUV['~'] .Set( 64,  96,  80, 112, 256);
	m_TextureUV['\\'].Set( 80,  96,  96, 112, 256);
	m_TextureUV['['] .Set( 96,  96, 112, 112, 256);
	m_TextureUV[']'].Set( 112,  96, 128, 112, 256);
	m_TextureUV['{'] .Set(128,  96, 144, 112, 256);
	m_TextureUV['}'] .Set(144,  96, 160, 112, 256);
	m_TextureUV['^'] .Set(160,  96, 176, 112, 256);
	m_TextureUV['<'] .Set(176,  96, 192, 112, 256);
	m_TextureUV['>'] .Set(192,  96, 208, 112, 256);
	m_TextureUV['_'] .Set(208,  96, 224, 112, 256);
}


// デストラクタ //
CTextureFont::~CTextureFont()
{
}


// フォント描画(通常) //
FVOID CTextureFont::Draw(int sx, int sy, char *pString, int Size)
{
	DrawAlpha(sx, sy, pString, 245, Size);
/*	D3DTLVERTEX		tlv[10];
	D3DRECTANGLE	*pUV;
	D3DVALUE		u0, v0, u1, v1;

	pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	pGrp->SetTexture(TEXTURE_ID_FONT);//GRPTEXTURE_MAX);//

	while(*pString != '\0'){
		pUV = &m_TextureUV[*pString];
		u0  = pUV->m_Left;
		v0  = pUV->m_Top;
		u1  = pUV->m_Right;
		v1  = pUV->m_Bottom;

		Set2DPointC(tlv+0, sx     , sy     , u0, v0, RGBA_MAKE(255, 255, 255, 245));
		Set2DPointC(tlv+1, sx+Size, sy     , u1, v0, RGBA_MAKE(255, 255, 255, 245));
		Set2DPointC(tlv+2, sx+Size, sy+Size, u1, v1, RGBA_MAKE(255, 255, 255, 245));
		Set2DPointC(tlv+3, sx     , sy+Size, u0, v1, RGBA_MAKE(255, 255, 255, 245));

		pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		pString++;
		sx += Size;
	}
*/
}


// フォント描画(半透明) //
FVOID CTextureFont::DrawAlpha(int sx, int sy, char *pString, BYTE Alpha, int Size)
{
	D3DTLVERTEX		tlv[10];
	D3DRECTANGLE	*pUV;
	D3DVALUE		u0, v0, u1, v1;
	DWORD			col;

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	g_pGrp->SetTexture(TEXTURE_ID_FONT);//GRPTEXTURE_MAX);//

	col = RGBA_MAKE(255, 255, 255, min(245, Alpha));


	while(*pString != '\0'){
		pUV = &m_TextureUV[*pString];
		u0  = pUV->m_Left;
		v0  = pUV->m_Top;
		u1  = pUV->m_Right;
		v1  = pUV->m_Bottom;

		Set2DPointC(tlv+0, sx     , sy     , u0, v0, col);
		Set2DPointC(tlv+1, sx+Size, sy     , u1, v0, col);
		Set2DPointC(tlv+2, sx+Size, sy+Size, u1, v1, col);
		Set2DPointC(tlv+3, sx     , sy+Size, u0, v1, col);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		pString++;
		sx += Size;
	}
}


// フォント描画(半透明＆トランスフォーム有り)
FVOID CTextureFont::DrawAT(int ox, int oy, char *pString, BYTE Alpha)
{
	int				lx, ly;
	int				Size;
	D3DTLVERTEX		tlv[10];
	D3DRECTANGLE	*pUV;
	D3DVALUE		u0, v0, u1, v1;

	Size = 16;

	lx = (Size * (Alpha+1));
	ly = (Size * (256*41-(Alpha+1)*40));

	ox -= (lx * strlen(pString) / 2) / 256;
	oy -= (ly / 2) / 256;

	lx /= 256;
	ly /= 256;

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	g_pGrp->SetTexture(TEXTURE_ID_FONT);//GRPTEXTURE_MAX);//

	while(*pString != '\0'){
		pUV = &m_TextureUV[*pString];
		u0  = pUV->m_Left;
		v0  = pUV->m_Top;
		u1  = pUV->m_Right;
		v1  = pUV->m_Bottom;

		Set2DPointC(tlv+0, ox    , oy   , u0, v0, RGBA_MAKE(255, 255, 255, Alpha));
		Set2DPointC(tlv+1, ox+lx , oy   , u1, v0, RGBA_MAKE(255, 255, 255, Alpha));
		Set2DPointC(tlv+2, ox+lx , oy+ly, u1, v1, RGBA_MAKE(255, 255, 255, Alpha));
		Set2DPointC(tlv+3, ox    , oy+ly, u0, v1, RGBA_MAKE(255, 255, 255, Alpha));

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		pString++;
		ox += lx;
	}
}


// コンボ数の表示 //
FVOID CTextureFont::DrawCombo(int ox, int oy, char *pNum, BYTE Alpha)
{
	int				lx, ly;
	int				x, y;
	int				n;
	BYTE			a;
	D3DTLVERTEX		tlv[10];
	D3DVALUE		u0, v0, u1, v1;
	DWORD			c;

	lx = (16 * (Alpha+1));
	ly = (16 * (256*11-(Alpha+1)*10));
//	lx = (12 * (Alpha+1));
//	ly = (24 * (256*11-(Alpha+1)*10));

	// 6 = strlen(" combo")
	x  = ox - (lx * (strlen(pNum)) / 2) / 256 - (52/2);
	y  = oy - (ly / 2) / 256;

	lx /= 256;
	ly /= 256;

//	if(Alpha < 48) g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
//	else           g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	if(Alpha > 230 || Alpha < 48) a = 255;
	else                          a = Alpha;

	c = RGBA_MAKE(a, a, a, 255);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_FONT);

	while(*pNum != '\0'){
		n   = (pNum[0] - '0') * 16;

		u0  = D3DVAL(8 + n)  / D3DVAL(256);
		v0  = D3DVAL(232)    / D3DVAL(256);
		u1  = D3DVAL(24 + n) / D3DVAL(256);
		v1  = D3DVAL(248)    / D3DVAL(256);

		Set2DPointC(tlv+0, x+16*0    , y   , u0, v0, c);
		Set2DPointC(tlv+1, x+lx+16*0 , y   , u1, v0, c);
		Set2DPointC(tlv+2, x+lx      , y+ly, u1, v1, c);
		Set2DPointC(tlv+3, x         , y+ly, u0, v1, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		pNum++;
		x += (lx - 2);
	}

	u0 = D3DVAL(168) / D3DVAL(256);
	v0 = D3DVAL(232) / D3DVAL(256);
	u1 = D3DVAL(220) / D3DVAL(256);
	v1 = D3DVAL(248) / D3DVAL(256);
/*	u0 = D3DVAL(168) / D3DVAL(256);
	v0 = D3DVAL(240) / D3DVAL(256);
	u1 = D3DVAL(204) / D3DVAL(256);
	v1 = D3DVAL(248) / D3DVAL(256);
*/
	x += lx / 2 / 2;

	lx = (52 * (Alpha+1));
	ly = (16 * (256*11-(Alpha+1)*10));
//	lx = (43 * (Alpha+1));
//	ly = (10 * (256*11-(Alpha+1)*10));

//	y  = oy + 4 - (ly / 2) / 256 + 2;
	y  = oy - (ly / 2) / 256;

	lx /= 256;
	ly /= 256;

	Set2DPointC(tlv+0, x    , y   , u0, v0, c);
	Set2DPointC(tlv+1, x+lx , y   , u1, v0, c);
	Set2DPointC(tlv+2, x+lx , y+ly, u1, v1, c);
	Set2DPointC(tlv+3, x    , y+ly, u0, v1, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 数字列の描画 //
FVOID CTextureFont::DrawNumber(int ox, int oy, char *pNum, int size)
{
	int				n;
	int				dx;
	DWORD			c;
	D3DTLVERTEX		tlv[10];
	D3DVALUE		u0, v0, u1, v1;

	g_pGrp->SetTexture(TEXTURE_ID_FONT);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	c  = RGBA_MAKE(255, 255, 255, 255);
	dx = (size * 2) / 3;

	while(*pNum != '\0'){
		n   = (pNum[0] - '0');

		if((n >= 0) && (n <= 9)){
			n *= 24;

			u0  = D3DVAL(0 + n)  / D3DVAL(256);
			v0  = D3DVAL(118)    / D3DVAL(256);
			u1  = D3DVAL(24 + n) / D3DVAL(256);
			v1  = D3DVAL(118+24) / D3DVAL(256);

			Set2DPointC(tlv+0, ox     , oy     , u0, v0, c);
			Set2DPointC(tlv+1, ox+size, oy     , u1, v0, c);
			Set2DPointC(tlv+2, ox+size, oy+size, u1, v1, c);
			Set2DPointC(tlv+3, ox     , oy+size, u0, v1, c);

			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}

		pNum++;
		ox += dx;
	}
}


// 小さなフォントで数字列を描画 //
FVOID CTextureFont::DrawSmallNumber8(int ox, int oy, char *pNum, BYTE a)
{
	int				n;
	DWORD			c;
	D3DTLVERTEX		tlv[10];
	D3DVALUE		u0, v0, u1, v1;
	int				w, w2;

	g_pGrp->SetTexture(TEXTURE_ID_FONT);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	w  = min(8, a/3);
	a  = min(255, (510 - a*2));
	c  = RGBA_MAKE(a, a, a, 255);
	ox = ox - (w * strlen(pNum)) / 2;

	for(; *pNum != '\0'; pNum++, ox+=(w-2)){
		n = (pNum[0] - '0');

		if((n >= 0) && (n <= 9)){
			n <<= 3;

			u0  = D3DVAL(176   + n) / D3DVAL(256);
			v0  = D3DVAL(152      ) / D3DVAL(256);
			u1  = D3DVAL(176+9 + n) / D3DVAL(256);
			v1  = D3DVAL(152+9    ) / D3DVAL(256);

			Set2DPointC(tlv+0, ox  , oy  , u0, v0, c);
			Set2DPointC(tlv+1, ox+w, oy  , u1, v0, c);
			Set2DPointC(tlv+2, ox+w, oy+w, u1, v1, c);
			Set2DPointC(tlv+3, ox  , oy+w, u0, v1, c);

			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}
	}
}


// ２Ｄ文字列の描画(Begin3DScene-End3DScene の外で使用すること！！ //
FVOID Draw2DFont(Pbg::CGrpSurface *pSurf, int ox, int oy, char *pStr)
{
	RECT		src;
	int			c;

	for(; *pStr!='\0'; pStr++, ox+=6){
		c = *pStr;

		if('A' <= c && c <= 'Z'){
			c = (c - 'A') << 3;
			SetRect2(&src, 64+c, 352, 8, 8);
		}
		else if('a' <= c && c <= 'z'){
			c = (c - 'a') << 3;
			SetRect2(&src, 64+c, 352, 8, 8);
		}
		else if('0' <= c && c <= '9'){
			c = (c - '0') << 3;
			SetRect2(&src, 272+c, 352, 8, 8);
		}
		else switch(c){
			case ':':	SetRect2(&src, 360, 352, 8, 8);	break;
			case '/':	SetRect2(&src, 352, 352, 8, 8);	break;
			default:	continue;	// for に対する continue
		}

		g_pGrp->BltC(&src, ox, oy, pSurf);
	}
}
