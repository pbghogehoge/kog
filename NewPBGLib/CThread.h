/*
 *   CThread.h   : スレッド管理クラス
 *
 */

#ifndef CTHREAD_INCLUDED
#define CTHREAD_INCLUDED "スレッド管理クラス : Version 0.02 : Update 2001/09/10"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/10 : 強制終了＆プライオリティ管理用関数の追加
 *    Version 0.01 : 2001/06/22 : 製作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// スレッド管理クラス(継承するのが前提) //
class CThread {
public:
	// 以下の関数を使用するのは特別に強制終了を行う必要がある場合のみである //
	// そのスレッド内でファイルハンドルを開いていたり、メモリを確保している //
	// 場合は、解放される保証が無いので注意しなければ...                    //
	FVOID __CallTerminateThread(void);	// スレッドを強制的に停止させる
	FVOID __CallSuspendThread(void);	// スレッドを一時停止させる
	FVOID __CallResumeThread(void);		// スレッドを再開させる


protected:
	// スレッドとなす関数 //
	virtual void ThreadFunction(void) = 0;

	BOOL CreateThread(void);			// スレッドを発動する
	void StopThread(void);				// スレッドを停止する
	void DisableThread(void);			// スレッドを無効化する
	BOOL IsEnableThread(void);			// スレッドの停止要求があれば真
	BOOL IsActiveThread(void);			// スレッドが動作中なら真
	void SetPriority(int nPriority);	// スレッドの優先順位を変更

	CThread();				// コンストラクタ
	virtual ~CThread();		// デストラクタ


private:
	// スレッド呼び出し用関数 //
	static DWORD WINAPI ThreadCallback(LPVOID lpParam);

	volatile BOOL	m_IsActive;		// スレッドはアクティブか
	volatile BOOL	m_IsEnable;		// スレッドの停止要求あれば偽

	DWORD			m_ThreadID;		// スレッド識別子
	HANDLE			m_hThread;		// スレッドのハンドル
};



#endif
