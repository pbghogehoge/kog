/*
 *   CMRoomProc.cpp   : 音楽室の処理
 *
 */

#include "CMRoomProc.h"
#include "CGameInfo.h"



// 音楽室の内部状態 //
#define MPST_INIT		0x00
#define MPST_EXIT		0x01
#define MPST_PLAY		0x02
#define MPST_ERROR		0xff

char *g_pMusicFileName = "MUSIC.DAT";



// コンストラクタ //
CMRoomProc::CMRoomProc(CGameInfo *pSys) : CScene(pSys)
{
	m_Count = 0;
	m_State = MPST_INIT;

	m_TitleWindow.Resize(640-6, 20*4);
	m_TitleWindow.SetFontSize(16);
	m_TitleWindow.SetWindowPosA(3, 24*16);
}


// デストラクタ //
CMRoomProc::~CMRoomProc()
{
}


// 初期化をする //
IBOOL CMRoomProc::Initialize(void)
{
	Pbg::LzDecode		File;

	if(NULL == g_pMusic) return FALSE;

	if(FALSE == File.Open(g_pMusicFileName)){
		PbgError("MUSIC.DAT が見つからないぜ");
		return FALSE;
	}

	m_NumMusic = 10;//File.GetNumFiles() / 2;
	File.Close();

	if(0 == m_NumMusic){
		PbgError("MUSIC.DAT 内の曲数が０です");
		return FALSE;
	}

	m_Count        = 0;
	m_CurrentMusic = 0;
	m_CurrentTempo = 0;
	m_State        = MPST_INIT;

	memset(m_LvTable, 0, sizeof(m_LvTable));

	// 通知関数を接続する //
//	g_pMidi->SetNotify(MidiNotify);

	// マウスカーソルを無効化 //
	m_pSys->EnableMouseCursor(FALSE);

	m_pSys->SetInputMode(IMODE_1P);

	return Play(0);
}


// １フレーム分だけ動作させる //
PBG_PROCID CMRoomProc::Move(void)
{
	static BOOL bKeyFlag = FALSE;

	WORD		KeyCode;
	int			i;

	switch(m_State){
	case MPST_INIT:		// 初期化動作
		m_State = MPST_PLAY;
	break;

	case MPST_EXIT:		// 終了動作
		Tempo(0);
	return PBGPROC_TITLE;

	case MPST_PLAY:		// 再生中
		KeyCode = m_pSys->GetPlayer1_KeyCode();
		if(!KeyCode) bKeyFlag = FALSE;
		if(bKeyFlag) break;
		if(KeyCode) bKeyFlag = TRUE;

		switch(KeyCode){
		case KEY_BOMB:	case KEY_MENU:
			m_State = MPST_EXIT;
		break;

		case KEY_SHOT:
//			if(MIDIST_STOP == g_pMidi->GetStatus()) Play(0);
//			else                                    Fade();
		break;

		case KEY_RIGHT:
			Play(1);
		break;

		case KEY_LEFT:
			Play(-1);
		break;

		case KEY_UP:
			Tempo(+1);
		break;

		case KEY_DOWN:
			Tempo(-1);
		break;
		}

		for(i=0; i<16; i++){
			if(m_LvTable[i]) m_LvTable[i] -= min(m_LvTable[i], 8);
		}
	break;

	case MPST_ERROR:	// エラー
		PbgError("CMRoomProc::Move() : 内部エラー");
	return PBGPROC_TITLE;
	}

	return PBGPROC_OK;
}


// 描画する //
IVOID CMRoomProc::Draw(void)
{
	RECT rcNote[12] = {
		{304, 1, 307, 11},		// しろ
		{336, 1, 339, 6},		// 黒

		{308, 1, 311, 11},		// しろ
		{336, 1, 339, 6},		// 黒

		{312, 1, 315, 11},		// しろ
		{316, 1, 319, 11},		// しろ

		{336, 1, 339, 6},		// 黒
		{320, 1, 323, 11},		// しろ

		{336, 1, 339, 6},		// 黒
		{324, 1, 327, 11},		// しろ

		{336, 1, 339, 6},		// 黒
		{328, 1, 331, 11},		// しろ
	};

	int NoteDest[12] = {
		0,		// しろ
		2,		// 黒

		4,		// しろ
		6,		// 黒

		8,		// しろ
		12,		// しろ

		14,		// 黒
		16,		// しろ

		18,		// 黒
		20,		// しろ

		22,		// 黒
		24,		// しろ
	};

	RECT		src;
	int			i, j, k, n;
	BYTE		Note[128];
	BYTE		Expr[16], Pan[16], Volume[16];
	DWORD		sum;

	g_pGrp->Cls();

	// 描画矩形を決定 //
	SetRect(&src, 0, 0, 640, 480);
	g_pGrp->SetViewport(&src);

	// タイトルの描画 //
	SetRect(&src, 360, 0, 640, 24);
	g_pGrp->BltC(&src, 640-(640-360), 0, g_pSystemSurf);

//	n = (g_pMidi->GetPlayTimes() / 120) % 8;
//	SetRect2(&src, 120+n*32, 48, 32, 32);
//	g_pGrp->BltC(&src, 360+128, 128, g_pSystemSurf);

	// 鍵盤の描画 //
	SetRect2(&src, 0, 0, 300, 24);
	for(i=0; i<16; i++){
		g_pGrp->BltC(&src, 32, i*24, g_pSystemSurf);
	}

	// 各値を取得する //
	g_pMusic->GetExpression(Expr);
	g_pMusic->GetPanPod(Pan);
	g_pMusic->GetPartVolume(Volume);

	// パート番号＆ノートの表示 //
	for(i=0; i<16; i++){
		g_pMusic->GetNoteOn(i, Note);

		for(j=n=0, sum=0; j<128; j++){
			if(Note[j]){
				k = j % 12;
				n = n + 1;
				sum += (Note[j] + 1);
				src = rcNote[k];
				g_pGrp->BltC(&src, 32+NoteDest[k]+(j/12)*28, i*24+1, g_pSystemSurf);
			}
		}

		SetRect2(&src, i*32, 24, 32, 24);
		g_pGrp->BltC(&src, 0, i*24, g_pSystemSurf);

		DrawNumber(32+ 42, i*24+13, Volume[i]);				// ボリューム
		DrawNumber(32+117, i*24+13, Expr[i]);				// エクスプレッション
		DrawNumber(32+173, i*24+13, ((int)Pan[i])-64);		// パン
		DrawNumber(32+225, i*24+13, n);						// ノート数

		if(n) sum = sum / n;

		if(sum > m_LvTable[i]){
			m_LvTable[i] = sum;
		}
		else if(0 == n){
			sum = m_LvTable[i];
			n = 1;
		}

		if(n){
			n = min(32, 1+sum / 4);

			SetRect(&src, 88, 48, 88+n, 53);
			g_pGrp->BltC(&src, 32+269, i*24+13, g_pSystemSurf);
		}
	}

	SetRect(&src, 0, 56, 272+16, 128);
	g_pGrp->BltC(&src, 350, 80, g_pSystemSurf);

	m_TitleWindow.Draw();
}


// 再生する //
FBOOL CMRoomProc::Play(int dir)
{
	char		Title[1024];
	char		temp[1024];


	if(0 == m_NumMusic) return FALSE;

	if(dir > 0){		// 次の曲
		m_CurrentMusic = (m_CurrentMusic + 1) % m_NumMusic;
	}
	else if(dir < 0){	// 前の曲
		m_CurrentMusic = (m_CurrentMusic + m_NumMusic - 1) % m_NumMusic;
	}

	// ロードする //
	if(FALSE == g_pMusic->Load(m_CurrentMusic)){
		m_State = MPST_ERROR;
		return FALSE;
	}

	// 曲の再生を開始する //
	g_pMusic->Play();

	// タイトルをウィンドウに送る //
	g_pMusic->GetTitle(Title);
	strcpy(temp, Title);
	m_TitleWindow.SetText(temp, FALSE);

	// 再生状態に移行する //
	m_State = MPST_PLAY;

	return TRUE;
}


// フェードアウト＆停止 //
FBOOL CMRoomProc::Fade(void)
{
	g_pMusic->Fade(196, 0);

	return TRUE;
}


// 停止する //
FBOOL CMRoomProc::Stop(void)
{
	g_pMusic->Stop();

	return TRUE;
}


// テンポを変更する //
FBOOL CMRoomProc::Tempo(int delta)
{
	if(delta){
		m_CurrentTempo += delta;
		if(m_CurrentTempo >  100) m_CurrentTempo =  100;
		if(m_CurrentTempo < -100) m_CurrentTempo = -100;
	}
	else{
		m_CurrentTempo = 0;
	}

//	g_pMidi->Tempo(m_CurrentTempo);
	return TRUE;
}


// 数字の描画を行う //
FVOID CMRoomProc::DrawNumber(int x, int y, int n)
{
	char	buf[32];
	RECT	src;
	char	*p = buf;

	wsprintf(buf, "%3d", n);

	while(1){
		switch(*p){
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				SetRect2(&src, ((*p)-'0')*8, 48, 4, 5);
			break;

			case '-':
				SetRect2(&src, 80, 48, 4, 5);
			break;

			case ' ':
				x += 5;
				p += 1;
			continue;

			default:
			return;
		}

		g_pGrp->BltC(&src, x, y, g_pSystemSurf);

		x += 5;
		p += 1;
	}
}


MIDIN_RET CMRoomProc::MidiNotify(BYTE Message, DWORD Param)
{
	// この関数から、PbgMidi を直接操作する関数を呼び出さないこと //

	switch(Message){
		case MIDIN_FADEZERO:
		return MRET_STOP;
	}

	return MRET_OK;
}
