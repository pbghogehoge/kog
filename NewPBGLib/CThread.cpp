/*
 *   CThread.cpp   : スレッド管理クラス
 *
 */

#include "CThread.h"




// コンストラクタ //
CThread::CThread()
{
	m_IsActive = FALSE;		// スレッドはアクティブか
	m_IsEnable = FALSE;		// スレッドの停止要求があれば偽
	m_ThreadID = 0;			// スレッド識別子
	m_hThread  = NULL;		// スレッドのハンドル
}


// デストラクタ //
CThread::~CThread()
{
	// 貴様を殺して俺も死ぬ //
	StopThread();
}


// スレッドを強制的に停止させる //
FVOID CThread::__CallTerminateThread(void)
{
	if(m_hThread){	// スレッドが死んでいるときは実行する必要が無い
		TerminateThread(m_hThread, 0);	// スレッドを強制終了する
		CloseHandle(m_hThread);			// スレッドハンドルも閉じる
		m_hThread = NULL;				// ハンドルを無効化する
	}
}


// スレッドを一時停止させる //
FVOID CThread::__CallSuspendThread(void)
{
	if(m_hThread){	// スレッドが死んでいるときは実行できない
		SuspendThread(m_hThread);	// スレッドを停止させる
	}
}


// スレッドを再開させる //
FVOID CThread::__CallResumeThread(void)
{
	if(m_hThread){	// スレッドが死んでいるときは実行せず
		ResumeThread(m_hThread);	// スレッドを再開させる
	}
}


// スレッドを発動する //
BOOL CThread::CreateThread(void)
{
	StopThread();

	m_IsActive = TRUE;	// スレッドを有効にする
	m_IsEnable = TRUE;	// 停止要求は無し

	// スレッドを立ち上げる //
	m_hThread = ::CreateThread(NULL				// セキュリティ記述子
							 , 0				// 初期スタックサイズ(default)
							 , ThreadCallback	// 呼び出す関数
							 , this				// ThreadCallback の引数
							 , NULL				// 作成オプション(動作開始)
							 , &m_ThreadID);	// スレッド識別子

	if(NULL == m_hThread){
		m_IsActive = FALSE;		// アクティブじゃない

		return FALSE;
	}

	// うむ //
	return TRUE;
}


// スレッドを停止する //
void CThread::StopThread(void)
{
	// 停止要求を送出 //
	m_IsEnable = FALSE;

	// スレッドが止まるまで待ちましょう //
	while(m_IsActive) Sleep(10);

	// スレッドが立ち上がっていたなら //
	if(m_hThread){
		CloseHandle(m_hThread);		// ハンドルを閉じて
		m_hThread = NULL;			// 無効化
	}
}


// スレッドを無効化する //
void CThread::DisableThread(void)
{
	// これを呼び出すことで、ThreadFunction() は自分が停止すべきで //
	// あると言う事を伝えることが出来る。なお、ThreadFunction() は //
	// IsEnableThread() を呼び出すことで自分が有効かをチェックする //
	m_IsEnable = FALSE;
}


// スレッドの停止要求があれば真 //
BOOL CThread::IsEnableThread(void)
{
	return m_IsEnable;
}


// スレッドが動作中なら真 //
BOOL CThread::IsActiveThread(void)
{
	// アクティブなら真を返す //
	return m_IsActive;
}


// スレッドの優先順位を変更 //
void CThread::SetPriority(int nPriority)
{
	if(m_hThread){	// スレッドが生きている場合のみ実行可能
		SetThreadPriority(m_hThread, nPriority);
	}
}


// スレッド呼び出し用関数 //
DWORD WINAPI CThread::ThreadCallback(LPVOID lpParam)
{
	CThread		*pThis;

	pThis = reinterpret_cast<CThread *>(lpParam);

	pThis->ThreadFunction();
	pThis->m_IsActive = FALSE;

	return 0;
}
