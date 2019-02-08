/*
 *   CLoadingGrp.cpp   : 初期化画面管理
 *
 */

#include "CLoadingGrp.h"
#include "StreamInfo.h"



// コンストラクタ //
CLoadingGrp::CLoadingGrp()
{
	m_Count = 0;
}


// デストラクタ //
CLoadingGrp::~CLoadingGrp()
{
	// 特に何もしません //
}


// 初期化する //
FBOOL CLoadingGrp::Initialize(int Width, int Height, DWORD NumExec)
{
	Pbg::CFont		Font;
	int				lx, ly, gx, gy, px, py;
	int				size;

	// 背景の格納先を作成する //
	if(FALSE == m_Back.Create(Width, Height)) return FALSE;


	// 背景を黒で塗りつぶす //
	m_Back.Clear(0);

	Font.SetFontID(FONTID_MS_GOTHIC);
	size = Width / 22;

	// 画面中央よりちょいと上・ NowLoading 表示 //
	Font.SetText("◎ ＮｏｗＬｏａｄｉｎｇ ◎");
	Font.SetColor(RGB(255, 255, 255));
	Font.SetSize(size);
	Font.Update();

	lx = max(0, Width/2 - Font.GetWidth() /2);
	ly = max(0, Height/2 - Font.GetHeight()/1);
	BitBlt(	m_Back.GetDC(), lx, ly
		,	Font.GetWidth(), Font.GetHeight()
		,	Font.GetDC(), 0, 0, SRCCOPY);

	// しばらくおまち・表示 //
	Font.SetText("し ば ら く お 待 ち 下 さ い");
	Font.SetColor(RGB(225, 225, 255));
	Font.SetSize(size/2);
	Font.Update();

	gx = max(0, (Width - Font.GetWidth()) /2);
	gy = max(0, ly - Font.GetHeight());
	BitBlt(	m_Back.GetDC(), gx, gy
		,	Font.GetWidth(), Font.GetHeight()
		,	Font.GetDC(), 0, 0, SRCCOPY);

	// 画面右下・バージョン＆コピーライト表示 //
//	Font.SetText("[稀翁玉・製品版となりうる体験版  Version 1.003]     Copyright (c) 西方 Project 2001.");
//	Font.SetText("[稀翁玉・状態窓追加版  Version 1.007]     Copyright (c) 西方 Project 2001.");
#ifdef PBG_DEBUG
	char	*pStr = "[稀翁玉・難易度変更追加版  "KIOH_VERSION" DEBUG_VER]     Copyright (c) 西方 Project 2004.";
#else
	char	*pStr = "[稀翁玉・難易度変更追加版  "KIOH_VERSION"]     Copyright (c) 西方 Project 2004.";
#endif

	Font.SetText(pStr);
	Font.SetColor(RGB(255, 225, 225));
	Font.SetSize(size/2);
	Font.Update();

	px = Width  - Font.GetWidth();
	py = Height - Font.GetHeight();
	BitBlt(	m_Back.GetDC(), px, py
		,	Font.GetWidth(), Font.GetHeight()
		,	Font.GetDC(), 0, 0, SRCCOPY);


	// 画面右上：ＥＳＣで強制終了表示 //
	Font.SetText("   →ＥＳＣで強制終了");
	Font.SetColor(RGB(150, 150, 150));
	Font.SetSize(size/2);
	Font.Update();
	BitBlt(	m_Back.GetDC(), 0, Font.GetHeight()
		,	Font.GetWidth(), Font.GetHeight()
		,	Font.GetDC(), 0, 0, SRCCOPY);



	// プログレスお星様の表示 //
	m_Progress.SetFontID(FONTID_MS_GOTHIC);
	m_Progress.SetColor(RGB(220, 255, 220));
	m_Progress.SetSize(size/2);
	SetProgress(0);

	// 壁紙のロード //
	char *pPackedFileID[8] = {
		"LOAD/STG1", "LOAD/STG2", "LOAD/STG4",
		"LOAD/STG5", "LOAD/STGX", "LOAD/MORGAN",
		"LOAD/VIVIT", "LOAD/ENEMY"
	};

	if(FALSE == Pbg::CGrpLoader::LoadP(&m_WallPaper, "GRAPH2.DAT", pPackedFileID[NumExec % 8])){
//	if(FALSE == m_WallPaper.LoadP(	"GRAPH2.DAT"
//								,	pPackedFileID[NumExec % 6])){
		return FALSE;
	}
	m_wx = max(0, Width/2 - m_WallPaper.GetWidth() /2);
	m_wy = Height / 2;

	// 水エフェクト管理クラスの作成 //
	if(FALSE == m_Water.Create(	m_WallPaper.GetWidth()
							,	m_WallPaper.GetHeight())){
		return FALSE;
	}

	return TRUE;
}


// 全体を再描画したい時に呼び出す //
FVOID CLoadingGrp::OnDraw(HDC hdc)
{
	EnterCS();

	BitBlt(m_Back.GetDC(), m_wx, m_wy
			, m_WallPaper.GetWidth(), m_WallPaper.GetHeight()
			, m_WallPaper.GetDC(), 0, 0, SRCCOPY);

	m_Back.EfcTransform(m_wx, m_wy, &m_WallPaper, m_Water.GetTransTable());

//	BitBlt(hdc, m_wx, m_wy, m_WallPaper.GetWidth(), m_WallPaper.GetHeight()
//			, m_Back.GetDC(), m_wx, m_wy, SRCCOPY);

	BitBlt(hdc, 0, 0, m_Back.GetWidth(), m_Back.GetHeight()
			, m_Back.GetDC(), 0, 0, SRCCOPY);

	LeaveCS();
}


// １フレーム更新する //
FVOID CLoadingGrp::Update(HDC hdc, BOOL bWantFinish)
{
	int			x, y, w, h, i;

	w = m_WallPaper.GetWidth();
	h = m_WallPaper.GetHeight();

	EnterCS();

	if(FALSE == bWantFinish){
		m_Count += 2;

		x = w / 2 + CosL(m_Count, w / 3);
		y = h / 2 + SinL(m_Count*2, h / 3);
		m_Water.SetRain(x, y, 255);

		for(i=0; i<5; i++){
			x = m_Rnd.Get()%(w-20) + 10;
			y = m_Rnd.Get()%(h-20) + 10;
			m_Water.SetRain(x, y, m_Rnd.Get()%200 + 55);
		}
	}

	m_Water.Proceed();


//	BitBlt(m_Back.GetDC(), m_wx, m_wy
//			, m_WallPaper.GetWidth(), m_WallPaper.GetHeight()
//			, m_WallPaper.GetDC(), 0, 0, SRCCOPY);

	m_Back.EfcTransform(m_wx, m_wy, &m_WallPaper, m_Water.GetTransTable());

	BitBlt(hdc, m_wx, m_wy, m_WallPaper.GetWidth(), m_WallPaper.GetHeight()
			, m_Back.GetDC(), m_wx, m_wy, SRCCOPY);

	BitBlt(hdc, 0, 0, m_Progress.GetWidth(), m_Progress.GetHeight()
			, m_Back.GetDC(), 0, 0, SRCCOPY);

	LeaveCS();
}


// 進行状況をセットする(0..10) //
FVOID CLoadingGrp::SetProgress(int Progress)
{
	int			i;
	char		buf[100] = "待ち時間 : ";
	char		*p;

	Progress *= 2;
	p = buf + strlen(buf);

	for(i=0; i<Progress; i+=2){
		strcpy(p+i, "★");
	}

	for(i=Progress; i<20; i+=2){
		strcpy(p+i, "☆");
	}

	EnterCS();

	m_Progress.SetText(buf);
	m_Progress.Update();

	BitBlt(m_Back.GetDC(), 0, 0, m_Progress.GetWidth(), m_Progress.GetHeight()
			, m_Progress.GetDC(), 0, 0, SRCCOPY);

	LeaveCS();
}


// 雨エフェクトが完了すれば真 //
FBOOL CLoadingGrp::IsEfcFinished(void)
{
	return m_Water.IsFinished();
}
