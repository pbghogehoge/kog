/*
 *   CStateWindow.cpp   : 状態表示窓
 *
 */

#include "CStateWindow.h"
#include "StateID.h"
#include "Gian2001.h"



//------------------------------------------------------------------------
//   名称  | CStateTxtInfo::CStateTxtInfo()
//   説明  | 初期化用コンストラクタ
//------------------------------------------------------------------------
CStateTxtInfo::CStateTxtInfo(char *pTxt, DWORD col, DWORD flag)
{
	// 文字列をコピー //
	strcpy(m_Txt, pTxt);

	m_Color = col;		// 色をセットする
	m_Flag  = flag;		// フラグをセット
}


//------------------------------------------------------------------------
//   名称  | CStateTxtInfo::getTxt()
//   説明  | 文字列を取得
//  戻り値 | 文字列へのポインタ
//------------------------------------------------------------------------
inline const char *CStateTxtInfo::getTxt(void) const
{
	return m_Txt;
}


//------------------------------------------------------------------------
//   名称  | CStateTxtInfo::getColor()
//   説明  | 色を取得
//  戻り値 | 文字列に関連付けられた色
//------------------------------------------------------------------------
IDWORD CStateTxtInfo::getColor(void) const
{
	return m_Color;
}


//------------------------------------------------------------------------
//   名称  | CStateTxtInfo::getFlag()
//   説明  | フラグを取得
//  戻り値 | 文字列に関連付けられた状態フラグ
//------------------------------------------------------------------------
IDWORD CStateTxtInfo::getFlag(void) const
{
	return m_Flag;
}


#define FONT_SIZE			(8)
#define STWINDOW_WIDTH		((FONT_SIZE-2)*14)
#define STWINDOW_DY			2
//------------------------------------------------------------------------
//   名称  | CStateWindow::CStateWindow()
//   説明  | CStateWindow のコンストラクタ
//------------------------------------------------------------------------
CStateWindow::CStateWindow(RECT *rcTarget, int *pX, int *pY)
	: m_pMx(pX), m_pMy(pY)	// 自機の座標へのポインタ
{
	// 左端の座標 //
	m_XMin = rcTarget->left >> 8;

	// 右端の座標 //
	m_XMax = (rcTarget->right >> 8) - STWINDOW_WIDTH;

	// 中央の座標(x256) //
	m_XMid256 = (rcTarget->right + rcTarget->left) / 2;

	// 表示開始Ｙ座標 //
	m_Y = 440 - (LINE * FONT_SIZE) - STWINDOW_DY * 2;
//	m_Y = (rcTarget->bottom >> 8) - (LINE << 4);

	// 左端、及び右端の窓のα値 //
	m_LeftAlpha  = 0;
	m_RightAlpha = 255;
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::~CStateWindow()
//   説明  | CStateWindow のデストラクタ
//------------------------------------------------------------------------
CStateWindow::~CStateWindow()
{
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::Initialize()
//   説明  | 初期化を行う
//------------------------------------------------------------------------
FVOID CStateWindow::Initialize(void)
{
	m_DispInfo.initialize();

	m_LeftAlpha  = 0;		// 左窓のα値
	m_RightAlpha = 255;		// 右窓のα値
/*
	InsertString(STWNDID_LV2ATK, 0);
	InsertString(STWNDID_LV3ATK, 0);
	InsertString("On Initialize", RGB_MAKE(255, 255, 255), 0);
	InsertString(STWNDID_LV4ATK, 0);

	InsertString(STWNDID_BOMB, 0);
	InsertString(STWNDID_KOCHANCE, 0);
	InsertString(STWNDID_GBREAK, 0);

	InsertString(STWNDID_DAMAGE, 100);
*/
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::Move()
//   説明  | １フレーム動作させる
//------------------------------------------------------------------------
FVOID CStateWindow::Move(void)
{
	int			mx;

	mx = *m_pMx;

	// 自機が画面の右側にいる場合 //
	if(mx > m_XMid256){
		m_RightAlpha = max(0, m_RightAlpha - 8);

		if(!m_RightAlpha){
			m_LeftAlpha  = min(255, m_LeftAlpha + 8);
		}
	}
	// 自機が画面の左側にいる場合 //
	else{
		m_LeftAlpha  = max(0, m_LeftAlpha - 8);

		if(!m_LeftAlpha){
			m_RightAlpha = min(255, m_RightAlpha + 8);
		}
	}
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::Draw()
//   説明  | 描画を行う
//------------------------------------------------------------------------
FVOID CStateWindow::Draw(void)
{
	DrawStateWindow(m_XMin, m_LeftAlpha);		// 右側の窓を描画
	DrawStateWindow(m_XMax, m_RightAlpha);		// 左側の窓を描画
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::InsertString()
//   説明  | 文字列を挿入する(ＩＤ指定)
//------------------------------------------------------------------------
FVOID CStateWindow::InsertString(BYTE StrID, int value)
{
	char		buf[128];


	// 諸事情により、抜ける //
	if(NULL == this) return;

	switch(StrID){
	case STWNDID_LV2ATK:	// レベル２アタック
		InsertString("Lv.2 Attack!!", RGB_MAKE(205, 255, 223), 0);
	break;

	case STWNDID_LV3ATK:	// レベル３アタック
		InsertString("Lv.3 Attack!!", RGB_MAKE(239, 219, 182), 0);
	break;

	case STWNDID_LV4ATK:	// ボスアタック
		InsertString("Boss Attack!!", RGB_MAKE(246, 167, 186), 0);
	break;

	case STWNDID_BOMB:		// ボムを使用した
		InsertString("Bomb Used.", RGB_MAKE(200, 200, 200), 0);
	break;

	case STWNDID_MADNESS:	// マッドネスモードに突入
		InsertString("Enter Madness Mode!!", RGB_MAKE(255, 0, 0), 0);
	break;

	case STWNDID_KOCHANCE:	// 相手がガードブレイクした
		InsertString("K.O. Chance!!", RGB_MAKE(255, 255, 255), 0);
	break;

	case STWNDID_GBREAK:	// 自分がガードブレイクした
		InsertString("Guard Break!!", RGB_MAKE(255, 50, 50), 0);
	break;

	case STWNDID_DAMAGE:	// 被弾して、ダメージを受けた(相手側)
		wsprintf(buf, "> Hit %d%%", value);
		InsertString(buf, RGB_MAKE(255, 255, 255), 0);
	break;
	}
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::InsertString()
//   説明  | 文字列を挿入する(文字列指定)
//------------------------------------------------------------------------
FVOID CStateWindow::InsertString(char *pStr, DWORD col, DWORD flag)
{
	m_DispInfo.insert(CStateTxtInfo(pStr, col, flag));
}



//------------------------------------------------------------------------
//   名称  | CStateWindow::DrawStateWindow()
//   説明  | CStateWindow::ウィンドウの描画を行う
//------------------------------------------------------------------------
FVOID CStateWindow::DrawStateWindow(int sx, BYTE a)
{
	D3DTLVERTEX			tlv[8];
	DWORD				c;
	int					sy, n, i, h;
	BYTE				a2;

	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->SetTexture(GRPTEXTURE_MAX);

	sy = m_Y;
	h  = LINE*FONT_SIZE + STWINDOW_DY * 2;

	a2 = 64 * a / 255;
	c = RGBA_MAKE(0, 0, 0, a2);
	Set2DPointC(tlv+0, sx,                sy,   0, 0, c);
	Set2DPointC(tlv+1, sx+STWINDOW_WIDTH, sy,   0, 0, c);
	Set2DPointC(tlv+2, sx+STWINDOW_WIDTH, sy+h, 0, 0, c);
	Set2DPointC(tlv+3, sx,                sy+h, 0, 0, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	a2 = 128 * a / 255;
	c = RGBA_MAKE(0, 0, 0, a2);
	Set2DPointC(tlv+0, sx-1,              sy-1, 0, 0, c);
	Set2DPointC(tlv+1, sx+STWINDOW_WIDTH, sy-1, 0, 0, c);
	Set2DPointC(tlv+2, sx+STWINDOW_WIDTH, sy+h, 0, 0, c);
	Set2DPointC(tlv+3, sx-1,              sy+h, 0, 0, c);
	Set2DPointC(tlv+4, sx-1,              sy-1, 0, 0, c);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);

//	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_STFONT);

	n  = m_DispInfo.getNumData();
	sy = sy + 2;
	sx = sx + 2;
	for(i=0; i<n; i++, sy+=(FONT_SIZE+1)){
		DrawString(m_DispInfo.getData(i), sx, sy, a);
	}
}


//------------------------------------------------------------------------
//   名称  | CStateWindow::DrawString()
//   説明  | 文字列の描画
//------------------------------------------------------------------------
FVOID CStateWindow::DrawString(const CStateTxtInfo *pInfo, int sx, int sy, BYTE a)
{
	DWORD				col;
	BYTE				r, g, b;
	const char			*p;
	char				c;
	int					x, y;
	D3DVALUE			u0, v0, u1, v1, l;
	D3DTLVERTEX			tlv[8];

	col = pInfo->getColor();

	a = (BYTE)(a * 255/ 255);
	r = (BYTE)(RGBA_GETRED(col)   * a / 255);
	g = (BYTE)(RGBA_GETGREEN(col) * a / 255);
	b = (BYTE)(RGBA_GETBLUE(col)  * a / 255);

	col = RGBA_MAKE(r, g, b, 255);

	p = pInfo->getTxt();
	l = 0;

	while(*p && l < (STWINDOW_WIDTH/(FONT_SIZE-2))){
		c = *p;

		if(c >= 'A' && c <= 'M'){
			u0 = D3DVAL((c-'A')  *16) / D3DVAL(256);
			u1 = D3DVAL((c-'A'+1)*16) / D3DVAL(256);
			v0 = D3DVAL(0)  / D3DVAL(256);
			v1 = D3DVAL(16) / D3DVAL(256);
		}
		else if(c >= 'N' && c <= 'Z'){
			u0 = D3DVAL((c-'N')  *16) / D3DVAL(256);
			u1 = D3DVAL((c-'N'+1)*16) / D3DVAL(256);
			v0 = D3DVAL(16) / D3DVAL(256);
			v1 = D3DVAL(32) / D3DVAL(256);
		}
		else if(c >= 'a' && c <= 'm'){
			u0 = D3DVAL((c-'a')  *16) / D3DVAL(256);
			u1 = D3DVAL((c-'a'+1)*16) / D3DVAL(256);
			v0 = D3DVAL(32)  / D3DVAL(256);
			v1 = D3DVAL(48) / D3DVAL(256);
		}
		else if(c >= 'n' && c <= 'z'){
			u0 = D3DVAL((c-'n')  *16) / D3DVAL(256);
			u1 = D3DVAL((c-'n'+1)*16) / D3DVAL(256);
			v0 = D3DVAL(48) / D3DVAL(256);
			v1 = D3DVAL(64) / D3DVAL(256);
		}
		else if(c >= '0' && c <= '9'){
			u0 = D3DVAL((c-'0'+4)*16) / D3DVAL(256);
			u1 = D3DVAL((c-'0'+5)*16) / D3DVAL(256);
			v0 = D3DVAL(96)  / D3DVAL(256);
			v1 = D3DVAL(112) / D3DVAL(256);
		}
		else{
//    ! ? " # $ % & ' ( ) + - *
//    / = , . ; : @ ~ \ [ ] { }
//    ^ < > _

			switch(c){
			case '!':	x = 0;		y = 4;		break;
			case '?':	x = 1;		y = 4;		break;
			case '"':	x = 2;		y = 4;		break;
			case '#':	x = 3;		y = 4;		break;
			case '$':	x = 4;		y = 4;		break;
			case '%':	x = 5;		y = 4;		break;
			case '&':	x = 6;		y = 4;		break;
			case '\'':	x = 7;		y = 4;		break;
			case '(':	x = 8;		y = 4;		break;
			case ')':	x = 9;		y = 4;		break;
			case '+':	x = 10;		y = 4;		break;
			case '-':	x = 11;		y = 4;		break;
			case '*':	x = 12;		y = 4;		break;

			case '/':	x = 0;		y = 5;		break;
			case '=':	x = 1;		y = 5;		break;
			case ',':	x = 2;		y = 5;		break;
			case '.':	x = 3;		y = 5;		break;
			case ';':	x = 4;		y = 5;		break;
			case ':':	x = 5;		y = 5;		break;
			case '@':	x = 6;		y = 5;		break;
			case '~':	x = 7;		y = 5;		break;
			case '\\':	x = 8;		y = 5;		break;
			case '[':	x = 9;		y = 5;		break;
			case ']':	x = 10;		y = 5;		break;
			case '{':	x = 11;		y = 5;		break;
			case '}':	x = 12;		y = 5;		break;

			case '^':	x = 0;		y = 6;		break;
			case '<':	x = 1;		y = 6;		break;
			case '>':	x = 2;		y = 6;		break;
			case '_':	x = 3;		y = 6;		break;

			default:
				p  = p + 1;
				l  = l + 1;
				sx = sx + FONT_SIZE - 2;
			continue;
			}

			u0 = D3DVAL((x  ) * 16) / D3DVAL(256);
			u1 = D3DVAL((x+1) * 16) / D3DVAL(256);
			v0 = D3DVAL((y  ) * 16) / D3DVAL(256);
			v1 = D3DVAL((y+1) * 16) / D3DVAL(256);
		}

		Set2DPointC(tlv+0, sx,           sy,           u0, v0, col);
		Set2DPointC(tlv+1, sx+FONT_SIZE, sy,           u1, v0, col);
		Set2DPointC(tlv+2, sx+FONT_SIZE, sy+FONT_SIZE, u1, v1, col);
		Set2DPointC(tlv+3, sx,           sy+FONT_SIZE, u0, v1, col);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		p  = p + 1;
		l  = l + 1;
		sx = sx + FONT_SIZE - 2;
	}
}
