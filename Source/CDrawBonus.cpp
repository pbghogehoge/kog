/*
 *   CDrawBonus.cpp   : ボーナスの表示
 *
 */

#include "CDrawBonus.h"
#include "Gian2001.h"



// ボーナス表示クラスの状態定数 //
#define DBST_DISABLE		0x00	// 無効状態
#define DBST_WAIT			0x01	// 最初の待ち
#define DBST_INIT			0x02	// 最初のエフェクト
#define DBST_MAXCOMBO		0x03	// 最大コンボ数
#define DBST_CLEARTIME		0x04	// クリアにかかった時間
#define DBST_TOTAL			0x05	// 合計
#define DBST_FINISH			0x06	// 最後のエフェクト

// 表示枠の幅＆高さ //
#define DBW_WIDTH		136
#define DBW_HEIGHT		110



// コンストラクタ //
CDrawBonus::CDrawBonus(RECT *rcTargetX256)
{
	m_ox = (rcTargetX256->right  + rcTargetX256->left) / 2;
	m_oy = (rcTargetX256->bottom + rcTargetX256->top ) / 2;

	m_State = DBST_DISABLE;
}


// デストラクタ //
CDrawBonus::~CDrawBonus()
{
	// 特に何もしませんな //
}


// 表示の準備を行う //
FDWORD CDrawBonus::Set(DWORD	MaxCombo	// 最大コンボ数
					 , DWORD	ClearTime)	// クリア時の経過フレーム数
{
	m_MaxCombo  = MaxCombo;			// 最大コンボ数を引き継ぐ
	m_ClearTime = ClearTime / 60;	// 引数はフレーム数なので要変換ね

	m_State     = DBST_WAIT;	// 初期化状態
	m_Count     = 0;			// 初期カウンタ

	m_ComboScore = m_MaxCombo * 2000;
	m_TimeScore  = max(0, (240 - (int)m_ClearTime)) * 1500;

	return m_ComboScore + m_TimeScore;
}


// 動作させる //
FVOID CDrawBonus::Move(void)
{
	if(DBST_DISABLE == m_State) return;

	m_Count++;

	// 状態推移は、break 抜け、それ以外は return //
	switch(m_State){
	case DBST_WAIT:			// 最初の待ち
		if(m_Count > 180){
			m_State = DBST_INIT;
			break;
		}
	return;

	case DBST_INIT:			// 最初のエフェクト
		if(m_Count > 25){
			m_State = DBST_MAXCOMBO;
			break;
		}
	return;

	case DBST_MAXCOMBO:		// 最大コンボ数
		if(m_Count > 25){
			m_State = DBST_CLEARTIME;
			break;
		}
	return;

	case DBST_CLEARTIME:	// クリアにかかった時間
		if(m_Count > 25){
			m_State = DBST_TOTAL;
			break;
		}
	return;

	case DBST_TOTAL:		// 合計
		if(m_Count > 25){
			m_State = DBST_FINISH;
			break;
		}
	return;

	case DBST_FINISH:		// 最後のエフェクト
	return;
	}

	m_Count = 0;
}


// 描画する //
FVOID CDrawBonus::Draw(void)
{
	switch(m_State){
	case DBST_INIT:			// 最初のエフェクト
		DrawInit(m_Count);
	break;

	case DBST_MAXCOMBO:		// 最大コンボ数
		DrawInit(25);
		DrawMaxCombo(m_Count);
	break;

	case DBST_CLEARTIME:	// クリアにかかった時間
		DrawInit(25);
		DrawMaxCombo(25);
		DrawClearTime(m_Count);
	break;

	case DBST_TOTAL:		// 合計
		DrawInit(25);
		DrawMaxCombo(25);
		DrawClearTime(25);
		DrawTotal(m_Count);
	break;

	case DBST_FINISH:		// 最後のエフェクト
		DrawInit(25);
		DrawMaxCombo(25);
		DrawClearTime(25);
		DrawTotal(25);
		DrawFinish(m_Count);
	break;
	}
}


// アクティブなら真を返す //
FBOOL CDrawBonus::IsActive(void)
{
	if(DBST_DISABLE == m_State) return FALSE;
	else                        return TRUE;
}


// 終了していれば真を返す //
FBOOL CDrawBonus::IsFinished(void)
{
	if(DBST_FINISH == m_State) return TRUE;
	else                       return FALSE;
}


// 状態を初期化する(無効状態にする) //
FVOID CDrawBonus::Initialize(void)
{
	m_State = DBST_DISABLE;
}


// 最初の半透明四角形を描画する //
FVOID CDrawBonus::DrawInit(DWORD Count)
{
	D3DTLVERTEX		tlv[10];
	int				x, y;
	int				w, h;
	DWORD			c1, c2, c3;

	w = (Count * Count * DBW_WIDTH) / (25 * 25);	// 長方形の幅
	h = DBW_HEIGHT;									// 長方形の高さ

	x = m_ox >> 8;				// 原点のＸ座標
	y = m_oy >> 8;				// 原点のＹ座標

	c1 = RGBA_MAKE(  0,   0,   0, 130+20);	// 黒でαを使用して、
	c2 = RGBA_MAKE(  0,   0,   0, 100+20);	// グラデーションをかける
//	c1 = RGBA_MAKE(  0,  24, 120,  96);	// 黒でαを使用して、
//	c2 = RGBA_MAKE(  0,  24, 120,  16);	// グラデーションをかける
	c3 = RGBA_MAKE(255, 255, 255,  32);	// 外枠の色

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

	Set2DPointC(tlv+0, x-w, y-h, 0, 0, c1);
	Set2DPointC(tlv+1, x+w, y-h, 0, 0, c1);
	Set2DPointC(tlv+2, x+w, y+h, 0, 0, c2);
	Set2DPointC(tlv+3, x-w, y+h, 0, 0, c2);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	Set2DPointC(tlv+0, x-w+3, y-h+4, 0, 0, c3);
	Set2DPointC(tlv+1, x+w-3, y-h+4, 0, 0, c3);
	Set2DPointC(tlv+2, x+w-3, y+h-5, 0, 0, c3);
	Set2DPointC(tlv+3, x-w+3, y+h-5, 0, 0, c3);
	g_pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, 4);
}


// 最大コンボ数を表示する //
FVOID CDrawBonus::DrawMaxCombo(DWORD Count)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				ox, oy, dx, dy;
	DWORD			c;
	DWORD			Combo;
	DWORD			Score;

	g_pGrp->SetTexture(TEXTURE_ID_BONUS);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	c  = RGBA_MAKE(255, 255, 255, 255);
	oy = (m_oy >> 8) - DBW_HEIGHT + 40;

	// ＭａｘＥｖａｄｅ //
	ox = (m_ox >> 8) - DBW_WIDTH  + 70;
	u1 = D3DVAL(  0) / D3DVAL(256);
	v1 = D3DVAL(  0) / D3DVAL(256);
	u2 = D3DVAL(120) / D3DVAL(256);
	v2 = D3DVAL( 24) / D3DVAL(256);
	dx = 60 + (25 - Count) * 40;
	dy = (12 * Count) / 25;

	Set2DPointC(tlv+0, ox-dx, oy-dy, u1, v1, c);
	Set2DPointC(tlv+1, ox+dx, oy-dy, u2, v1, c);
	Set2DPointC(tlv+2, ox+dx, oy+dy, u2, v2, c);
	Set2DPointC(tlv+3, ox-dx, oy+dy, u1, v2, c);

	// Ｃｏｍｂｏ //
	ox = (m_ox >> 8) + DBW_WIDTH  - 35 - 8;
	u1 = D3DVAL(136) / D3DVAL(256);
	v1 = D3DVAL(  8) / D3DVAL(256);
	u2 = D3DVAL(192) / D3DVAL(256);
	v2 = D3DVAL( 24) / D3DVAL(256);
	dx = 28 + (25 - Count) * 40;
	dy = (8 * Count) / 25;

	Set2DPointC(tlv+4, ox-dx, oy+4-dy, u1, v1, c);
	Set2DPointC(tlv+5, ox+dx, oy+4-dy, u2, v1, c);
	Set2DPointC(tlv+6, ox+dx, oy+4+dy, u2, v2, c);
	Set2DPointC(tlv+7, ox-dx, oy+4+dy, u1, v2, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+0, 4);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+4, 4);

	if(25 == Count){
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+0, 4);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+4, 4);
	}

	ox    = (m_ox >> 8) - DBW_WIDTH  + 90;// + 70;
	Combo = m_MaxCombo;
	Score = m_ComboScore;
	DrawNumber(ox, oy+4,    Combo*Count/25, 0, (BYTE)min(Count*11, 255));
	DrawPoints(ox, oy+4+20, Score*Count/25, 0, (BYTE)min(Count*11, 255));
}


// クリア時間を表示する //
FVOID CDrawBonus::DrawClearTime(DWORD Count)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				ox, oy, dx, dy;
	DWORD			c;
	DWORD			ClearTime;
	DWORD			Score;

	g_pGrp->SetTexture(TEXTURE_ID_BONUS);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	c  = RGBA_MAKE(255, 255, 255, 255);
	oy = (m_oy >> 8) - DBW_HEIGHT + 40 + 60;

	// ＣｌｅａｒＴｉｍｅ//
	ox = (m_ox >> 8) - DBW_WIDTH  + 70;
	u1 = D3DVAL(  0) / D3DVAL(256);
	v1 = D3DVAL( 32) / D3DVAL(256);
	u2 = D3DVAL(120) / D3DVAL(256);
	v2 = D3DVAL( 56) / D3DVAL(256);
	dx = 60 + (25 - Count) * 40;
	dy = (12 * Count) / 25;

	Set2DPointC(tlv+0, ox-dx, oy-dy, u1, v1, c);
	Set2DPointC(tlv+1, ox+dx, oy-dy, u2, v1, c);
	Set2DPointC(tlv+2, ox+dx, oy+dy, u2, v2, c);
	Set2DPointC(tlv+3, ox-dx, oy+dy, u1, v2, c);

	// Ｓｅｃ //
	ox = (m_ox >> 8) + DBW_WIDTH  - 35 - 8;
	u1 = D3DVAL(136) / D3DVAL(256);
	v1 = D3DVAL( 40) / D3DVAL(256);
	u2 = D3DVAL(192) / D3DVAL(256);
	v2 = D3DVAL( 56) / D3DVAL(256);
	dx = 28 + (25 - Count) * 40;
	dy = (8 * Count) / 25;

	Set2DPointC(tlv+4, ox-dx, oy+4-dy, u1, v1, c);
	Set2DPointC(tlv+5, ox+dx, oy+4-dy, u2, v1, c);
	Set2DPointC(tlv+6, ox+dx, oy+4+dy, u2, v2, c);
	Set2DPointC(tlv+7, ox-dx, oy+4+dy, u1, v2, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+0, 4);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+4, 4);

	if(25 == Count){
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+0, 4);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+4, 4);
	}

	ox        = (m_ox >> 8) - DBW_WIDTH  + 90;// + 70;
	ClearTime = m_ClearTime;
	Score     = m_TimeScore;
	DrawNumber(ox, oy+4, ClearTime*Count/25, 1, (BYTE)min(Count*11, 255));
	DrawPoints(ox, oy+4+20, Score*Count/25, 1, (BYTE)min(Count*11, 255));
}


// 合計得点を表示する //
FVOID CDrawBonus::DrawTotal(DWORD Count)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				ox, oy, dx, dy;
	DWORD			c;
	DWORD			Score;

	g_pGrp->SetTexture(TEXTURE_ID_BONUS);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	c  = RGBA_MAKE(255, 255, 255, 255);
	ox = (m_ox >> 8) - DBW_WIDTH  + 70;
	oy = (m_oy >> 8) - DBW_HEIGHT + 40 + 60 * 2;

	u1 = D3DVAL(  0) / D3DVAL(256);
	v1 = D3DVAL( 64) / D3DVAL(256);
	u2 = D3DVAL(120) / D3DVAL(256);
	v2 = D3DVAL( 88) / D3DVAL(256);

	dx = 60 + (25 - Count) * 40;
	dy = (12 * Count) / 25;

	Set2DPointC(tlv+0, ox-dx, oy-dy, u1, v1, c);
	Set2DPointC(tlv+1, ox+dx, oy-dy, u2, v1, c);
	Set2DPointC(tlv+2, ox+dx, oy+dy, u2, v2, c);
	Set2DPointC(tlv+3, ox-dx, oy+dy, u1, v2, c);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	if(25 == Count){
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	ox    = (m_ox >> 8) - DBW_WIDTH  + 90;// + 70;
	Score = m_ComboScore + m_TimeScore;
	DrawPoints(ox, oy+4+20, Score*Count/25, 2, (BYTE)min(Count*11, 255));
}


// エフェクトの後始末表示 //
FVOID CDrawBonus::DrawFinish(DWORD Count)
{
	g_pGrp->SetTexture(TEXTURE_ID_BONUS);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
}


// 色を考慮に入れた数字を描画する //
FVOID CDrawBonus::DrawNumber(int sx, int sy, DWORD n, BYTE ColID, BYTE Transform)
{
	char			buf[20];
	char			*p;
	int				dx, dy;
	D3DVALUE		tx1, ty1, tx2, ty2;
	D3DTLVERTEX		tlv[20];
	DWORD			c;

	wsprintf(buf, "%8d", n);

	ty1 = D3DVAL(96 + ColID * 24 +  0) / D3DVAL(256);
	ty2 = D3DVAL(96 + ColID * 24 + 16) / D3DVAL(256);
	c   = RGBA_MAKE(255, 255, 255, 255);
	dx  = (8 * Transform) / 255;
	dy  =  8 + ((255-Transform) / 2);

	// 描画ループ
	for(p=buf; (*p)!='\0'; p++, sx+=14){
		switch(*p){
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			tx1 = D3DVAL( ((*p) - '0' + 0) * 16 ) / D3DVAL(256);
			tx2 = D3DVAL( ((*p) - '0' + 1) * 16 ) / D3DVAL(256);
		break;

		default:
		continue;	// for に戻り
		}

		Set2DPointC(tlv+0, sx-dx, sy-dy, tx1, ty1, c);
		Set2DPointC(tlv+1, sx+dx, sy-dy, tx2, ty1, c);
		Set2DPointC(tlv+2, sx+dx, sy+dy, tx2, ty2, c);
		Set2DPointC(tlv+3, sx-dx, sy+dy, tx1, ty2, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

		if(255 == Transform){
			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
		}
	}
}


// 色を考慮に入れた数字を描画する(+ Pts 表示) //
FVOID CDrawBonus::DrawPoints(int sx, int sy, DWORD n, BYTE ColID, BYTE Transform)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				ox, dx, dy;
	DWORD			c;

	c  = RGBA_MAKE(255, 255, 255, 255);
	ox = (m_ox >> 8) + DBW_WIDTH  - 35 - 16 - 8;		// 28

	// Ｐｔｓ //
	u1 = D3DVAL(176)          / D3DVAL(256);
	v1 = D3DVAL(96+ColID*24)  / D3DVAL(256);
	u2 = D3DVAL(200)          / D3DVAL(256);
	v2 = D3DVAL(112+ColID*24) / D3DVAL(256);
	dx = 12 + (255 - Transform) * 4;
	dy = (8 * Transform) / 255;

	Set2DPointC(tlv+0, ox-dx, sy-dy, u1, v1, c);
	Set2DPointC(tlv+1, ox+dx, sy-dy, u2, v1, c);
	Set2DPointC(tlv+2, ox+dx, sy+dy, u2, v2, c);
	Set2DPointC(tlv+3, ox-dx, sy+dy, u1, v2, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+0, 4);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+0, 4);

	DrawNumber(sx, sy, n, ColID, Transform);
}
