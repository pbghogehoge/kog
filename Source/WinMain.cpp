/*
 *   WinMain.cpp   : ウィンドウ周りの処理
 *
 */

#include "WinMain.h"
#include "CLoadingGrp.h"



static const char *ApplicationClass = "G I A N   2001";	// ウィンドウクラス
static const char *ApplicationName  = "稀翁玉";			// タイトル
static const char *ApplicationIcon  = "TAMA_ICON";		// アイコンのお名前

static BOOL		g_bIsActive     = FALSE;	// 現在アクティブになっているか
static BOOL		g_bMouseVisible = FALSE;	// マウスが可視状態にあるか
static POINT	g_WindowSize    = {0, 0};	// 起動時のウィンドウの幅と高さ

HWND g_hMainWindow;		// ウィンドウハンドル



// ウィンドウプロシージャ //
long FAR PASCAL WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

// ウィンドウ周りの初期化 //
static BOOL WindowInitialize(HINSTANCE hInstance, int nShowCmd);

BOOL InitStartupFont(DWORD NumExec);	// フォントを初期化
void CleanupStartupFont(void);			// フォントを解放
BOOL SetCache(Pbg::CPbgCache &Cache);	// キャッシュを有効にする

// 読み込み表示用の○をバッファに書き込む //
void SetProgress(int Progress);



int WINAPI WinMain(HINSTANCE hInstance		// 現在のインスタンスハンドル
				 , HINSTANCE hPrevInstance	// WIN32 だから NULL
				 , LPSTR lpCmdLine			// コマンドライン
				 , int nShowCmd)			// 表示状態
{
	Pbg::CGrpLoader		Cache;
	MSG					msg;
	BOOL				bRet;

	// エラーログの吐き出し先を決める //
	PbgLogName("稀翁LOG.txt");

	// ここに多重起動防止を記述する //
	if(Pbg::IsRunningApp(ApplicationClass, ApplicationName)){
		return 0;
	}

	if(!WindowInitialize(hInstance,nShowCmd)){
		PbgError("ウィンドウの初期化に失敗しました");
		return FALSE;
	}


	// ここで、コンフィグの初期化を行う //
	CCfgIO		ConfigData;


	// フォントの初期化をする //
	if(FALSE == InitStartupFont(ConfigData.GetNumExecCount())){
		PbgError("アプリケーションの初期化に失敗しました");
		PostMessage(g_hMainWindow, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// キャッシュを有効にする //
	if(FALSE == SetCache(Cache)){
		PbgError("初期化が中断されました");
		CleanupStartupFont();
		PostMessage(g_hMainWindow, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// アプリケーション固有の初期化 //
	bRet = XInitialize(&ConfigData);
	CleanupStartupFont();

	if(FALSE == bRet){
		XCleanup();
//		CleanupStartupFont();
		PostMessage(g_hMainWindow, WM_CLOSE, 0, 0);
		return FALSE;
	}

	while(1){
		// お約束の、メッセージ受け取りね //
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)){
			if(!GetMessage(&msg,NULL,0,0)){
				return msg.wParam;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(g_bIsActive)	XMainLoop();	// メインループ
		else					WaitMessage();
	}
}


static BOOL WindowInitialize(HINSTANCE hInstance,int nCmdShow)
{
	WNDCLASS	wc;
	HMENU		hMenu;

	PbgErrorInit("::WindowInitialize");

	ZEROMEM(wc);
	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= hInstance;
	wc.lpszClassName	= ApplicationClass;
	wc.hIcon			= (HICON)LoadIcon(hInstance, ApplicationIcon);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);

	// 登録 //
	if(!RegisterClass(&wc)){
		PbgErrorEx("ウィンドウクラスの登録に失敗");
		return FALSE;
	}

	g_WindowSize.x = GetSystemMetrics(SM_CXSCREEN);
	g_WindowSize.y = GetSystemMetrics(SM_CYSCREEN);

	g_hMainWindow = CreateWindowEx(
		0,
		ApplicationClass,	// クラス名
		ApplicationName,	// アプリケーション名
		(WS_VISIBLE|WS_SYSMENU|WS_EX_TOPMOST|WS_POPUP),
		0,					// ウィンドウの左上のＸ座標
		0,					// ウィンドウ左上のＹ座標
		g_WindowSize.x,		// ウィンドウ幅
		g_WindowSize.y,		// ウィンドウの高さ
		NULL,
		NULL,
		hInstance,
		NULL);

	// 失敗 //
	if(NULL == g_hMainWindow){
		PbgErrorEx("ウィンドウの作成に失敗");
		return FALSE;
	}

	ShowCursor(FALSE);				// かーそるぽいぽい

	hMenu = GetSystemMenu(g_hMainWindow,FALSE);	// メニューのハンドルを取ったら
	DeleteMenu(hMenu,SC_MAXIMIZE,MF_BYCOMMAND);	// 最大化を消して
	DeleteMenu(hMenu,SC_MINIMIZE,MF_BYCOMMAND);	// 最小化を消して
	DeleteMenu(hMenu,SC_MOVE    ,MF_BYCOMMAND);	// 移動を消して
	DeleteMenu(hMenu,SC_SIZE    ,MF_BYCOMMAND);	// サイズ変更を消す

	ShowWindow(g_hMainWindow,nCmdShow);	// WS_VISIBLE 指定を
	UpdateWindow(g_hMainWindow);		// しているから、本当は
	SetFocus(g_hMainWindow);			// いらないけどね...

	return TRUE;
}


static CLoadingGrp	*g_pLoading = NULL;
/*
static Pbg::CFont	*g_pLoading   = NULL;
static Pbg::CFont	*g_pTitle     = NULL;
static Pbg::CFont	*g_pPbg       = NULL;
static Pbg::CFont	*g_pProgress  = NULL;
static Pbg::CDIB32	*g_pWallPaper = NULL;

static int			g_lx, g_ly;
static int			g_tx, g_ty;
static int			g_px, g_py;
static int			g_wx, g_wy;
*/


// フォントを初期化 //
BOOL InitStartupFont(DWORD NumExec)
{
	g_pLoading = NewEx(CLoadingGrp);
	if(NULL == g_pLoading) return FALSE;

	if(FALSE == g_pLoading->Initialize(
								g_WindowSize.x
							,	g_WindowSize.y
							,	NumExec)){
		DeleteEx(g_pLoading);
		return FALSE;
	}

	return TRUE;
}


// フォントを解放 //
void CleanupStartupFont(void)
{
	DeleteEx(g_pLoading);
}


long FAR PASCAL WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT			ps;
	HDC					hdc;
//	RECT				rc;

	static BOOL			ExitFlag = FALSE;


	switch(message){
		case WM_CREATE:
			// 再び、最前面に移動する //
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, g_WindowSize.x, g_WindowSize.y, 0);
		break;

		case WM_CLOSE:
			ExitFlag = TRUE;

			CleanupStartupFont();
			XCleanup();

			ShowWindow(hWnd,SW_HIDE);
			MoveWindow(hWnd,0,0,0,0,TRUE);
			DestroyWindow(hWnd);
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		case WM_ACTIVATEAPP:
			g_bIsActive = (BOOL)wParam;
			if(g_bIsActive) g_bMouseVisible = FALSE;
			else            g_bMouseVisible = TRUE;
		break;

		case WM_SETCURSOR:
			if(g_bMouseVisible) SetCursor(LoadCursor(NULL,IDC_ARROW));
			else                SetCursor(NULL),ShowCursor(TRUE);
		return 1;

		case WM_ERASEBKGND:
		return 1;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

//			SetRect(&rc, 0, 0, g_WindowSize.x, g_WindowSize.y);
//			FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

//			if(g_pLoading) g_pLoading->OnDraw(hdc);

//			DrawStartupFont(hdc);
			EndPaint(hWnd,&ps);
		return 1;

		// IME 関連のメッセージは無視だ！ //
		case WM_IME_CHAR:		case WM_IME_COMPOSITION:		case WM_IME_COMPOSITIONFULL:
		case WM_IME_CONTROL:	case WM_IME_ENDCOMPOSITION:		case WM_IME_KEYDOWN:
		case WM_IME_KEYUP:		case WM_IME_NOTIFY:				case WM_IME_SELECT:
		case WM_IME_SETCONTEXT:	case WM_IME_STARTCOMPOSITION:

		#if(WINVER >= 0x0500)
			case WM_IME_REQUEST:
		#endif

		return 1;

		case WM_SYSCOMMAND:
			if(wParam == SC_CLOSE)		break;
			if(wParam == SC_RESTORE)	break;
		return 1;

		case WM_SYSKEYDOWN:
			if(ExitFlag) return 0;
		break;

		default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


class CacheThread : public CThread {
public:
	BOOL Start(Pbg::CPbgCache &Cache){
		m_pCache = &Cache;
		return CreateThread();
	};

	void ThreadFunction(void){
		Pbg::LzDecode	Decode;

		char			FileName[MAX_PATH];
		char			FileID[MAX_PATH];

		char			*pFileID = "CACHEFILES";
		char			*pData;
		char			*p;
		int				i, j, n;

		//SetPriority(THREAD_PRIORITY_NORMAL);
		SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
		if(FALSE == Decode.Open("ENEMY.DAT")) return;

		pData = (char *)Decode.Decode(pFileID);
		if(NULL == pData) return;

		n = Decode.GetFileSize(pFileID);

		for(i=0; i<n; i+=80){
			if(FALSE == IsEnableThread()){
				MemFree(pData);
				break;
			}

#ifdef PBG_DEBUG
			if(GetAsyncKeyState(VK_SPACE) & 0x8000){
				PbgError("DEBUG COMMAND - Cancel Cache -");
				MemFree(pData);
				return;
			}
#endif

			p = pData + i;

			for(j=0; p[j]!='#'; j++){
				FileName[j] = p[j];
			}

			FileName[j] = '\0';

			strcpy(FileID, p+j+1);

			m_pCache->SetCache(FileName, FileID);
//			Sleep(600);

			//SetProgress(1 + 10 * i / n);
			if(g_pLoading){
				g_pLoading->SetProgress(1 + 10 * i / n);
			}
		}

		MemFree(pData);
	}

	BOOL IsActive(void){
		return IsActiveThread();
	};

	void OnDisable(void){
		SetPriority(THREAD_PRIORITY_HIGHEST);
		DisableThread();
	};

private:
	Pbg::CPbgCache		*m_pCache;
};


// キャッシュを有効にする //
BOOL SetCache(Pbg::CPbgCache &Cache)
{
	HDC				hdc;
	CacheThread		Thread;

	if(g_pLoading){
		hdc = GetDC(g_hMainWindow);
		g_pLoading->OnDraw(hdc);
		ReleaseDC(g_hMainWindow, hdc);
	}

	Thread.Start(Cache);

	while(1){
		// 強制的に終了する場合 //
		if(GetAsyncKeyState(VK_ESCAPE) & 0x8000){
			Thread.OnDisable();

			while(Thread.IsActive());

			return FALSE;
		}

		// キャッシュスレッドが停止している場合 //
		if(FALSE == Thread.IsActive()){
			while(FALSE == g_pLoading->IsEfcFinished()){
				Sleep(1000/60);
				hdc = GetDC(g_hMainWindow);
				g_pLoading->Update(hdc, TRUE);
				ReleaseDC(g_hMainWindow, hdc);
			}

			return TRUE;
		}

		// 通常の更新を行う場合 //
		Sleep(1000/60);
		if(g_pLoading){
			hdc = GetDC(g_hMainWindow);
			g_pLoading->Update(hdc, FALSE);
			ReleaseDC(g_hMainWindow, hdc);
		}
	}
}

/*
// 読み込み表示用の★をバッファに書き込む //
void SetProgress(int Progress)
{
	int			i;
	char		buf[100] = "待ち時間 : ";
	char		*p;
//	RECT		rc;
	HDC			hdc;

	if(NULL == g_pProgress) return;

	Progress *= 2;
	p = buf + strlen(buf);

	for(i=0; i<Progress; i+=2){
		strcpy(p+i, "★");
	}

	for(i=Progress; i<20; i+=2){
		strcpy(p+i, "☆");
	}

	g_pProgress->SetText(buf);
	g_pProgress->Update();

	hdc = GetDC(g_hMainWindow);
	DrawStartupFont(hdc);
	ReleaseDC(g_hMainWindow, hdc);

}

*/
