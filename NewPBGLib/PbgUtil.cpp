/*
 *   PbgUtil.cpp   : 汎用テンプレート等
 *
 */

#include "PbgUtil.h"
#include "PbgError.h"



namespace Pbg {

class CMutex {
public:
	// 作成する //
	BOOL Create(LPCTSTR lpName){
		// まずは、解放する //
		Release();

		// ミューテックスオブジェクトを作成する //
		m_hMutex = CreateMutex(NULL, TRUE, lpName);

		if(ERROR_ALREADY_EXISTS == GetLastError()){
			return FALSE;	// すでに存在する
		}

		return TRUE;	// 新規作成
	}

	// 解放する //
	void Release(void){
		if(m_hMutex){	// このプロセスですでに呼び出されていた場合
			ReleaseMutex(m_hMutex);	// 解放して
			m_hMutex = NULL;		// 一応、ポインタを無効化
		}
	}

	CMutex()	{ m_hMutex = NULL; };	// コンストラクタ
	~CMutex()	{ Release(); };			// デストラクタ


private:
	// ミューテックスオブジェクトのハンドル //
	HANDLE		m_hMutex;
};


// すでにアプリケーションが起動していれば、そのウィンドウを //
// アクティブにして、ハンドルを返す                         //
HWND IsRunningApp(LPCTSTR lpClassName, LPCTSTR lpWindowName)
{
	static CMutex		Mutex;

	HWND				hWindow;
	char				AppName[MAX_PATH+4];
	int					i;


	// まず、頭に "おまけ" をくっつける //
	strcpy(AppName, "PBG@");

	// 「この」アプリのファイル名を取得しましょう //
	GetModuleFileName(NULL, AppName+4, MAX_PATH);

	// ファイル名に少々細工を加える //
	for(i=strlen(AppName)-1; i>=4; i--){
		if('\\' == AppName[i]){	// \ はミューテックスオブジェクト名に使えない
			AppName[i] = ':';	// コロンはファイル名に使えない
		}
	}

	// 作成できない、つまり所有権が取得できなかった場合 //
	if(FALSE == Mutex.Create(AppName)){
		hWindow = FindWindow(lpClassName, lpWindowName);	// ウィンドウを探して

		if(hWindow){	// ウィンドウが見つかったら
			SetForegroundWindow(hWindow);						// 前にもってきて
			SendMessage(hWindow, WM_SYSCOMMAND, SC_RESTORE, 0);	// リストアする
		}

		// すでに起動しています //
		return hWindow;
	}

	// まだ起動してない //
	return NULL;
}



} // namespace Pbg
