/*
 *   CWaveMusic.h   : WAVEによる曲の再生
 *
 */

#ifndef CWAVEMUSIC_INCLUDED
#define CWAVEMUSIC_INCLUDED "WAVEによる曲の再生 : Version 0.07 : Update 2001/06/23"

/*  [更新履歴]
 *    Version 0.07 : 2001/06/23 : 
 *    Version 0.06 : 2001/06/22 : スレッドを外に出して、継承(CThread)
 *    Version 0.05 : 2001/01/29 : スレッド系バグの修正(過酷な条件下で停止ＯＫ)
 *    Version 0.04 : 2001/01/25 : Seek 系のバグの除去
 *    Version 0.03 : 2001/01/25 : GetCurrentPosition, GetLoopPosition の追加
 *                              : ロスト対策、エラーメッセージ追加
 *    Version 0.02 : 2001/01/24 : ループ再生対応。注釈が少々粗いので、修正のこと
 *    Version 0.01 : 2001/01/21 : クラス設計、などなど
 */



#include "PbgType.h"
#include "CThread.h"
#include "CWaveRead.h"
#include "CDirectShow.h"



namespace Pbg {



/***** [ 定数 ] *****/
#define NUM_WAVMUSIC_NOTIFY		4		// 再生場所通知の個数



/***** [クラス定義] *****/

// WAVE による曲の再生を行うクラス //
class CWaveMusic : public CThread {
public:
	BOOL Play(void);		// 再生する
	BOOL Seek(DWORD Pos);	// 再生場所の移動
	void Stop(void);		// 曲を停止する(先頭に戻る)
	void Pause(void);		// 曲を一時停止する

	BOOL  SetLoopPosition(DWORD Start, DWORD End);	// ループタイミングのセット
	BOOL  GetLoopPosition(DWORD *Start, DWORD *End);// ループタイミングの取得
	DWORD GetCurrentPosition(void);					// 現在位置の取得
	DWORD GetEofPosition(void);						// ファイル終端位置の取得
	DWORD GetFrequency(void);						// 周波数の取得

	// 現在の波形を表示する(Stereo / 8Bit に変換される) //
	void GetCurrentScope(BYTE Table[128+128]);

	void SetVolume(BYTE Volume);					// ボリュームをセットする
	BOOL Load(char *FileName);						// 曲をロードする

	CWaveMusic(LPDIRECTSOUND lpDS);		// コンストラクタ
	~CWaveMusic();						// デストラクタ


private:
	void Initialize(void);		// DSound オブジェクトを破棄する
	BOOL Restore(void);			// バッファの修復

	BOOL FillNextSoundData(void);	// 次のデータを補充する
	void UpdateProgress(void);		// どこまで再生したか示す変数を更新する

	BOOL WriteData(BYTE *pBuffer, DWORD BufferLength);	// 指定だけデータを書き込み

	// 曲の再生用スレッド //
	void ThreadFunction(void);


private:
	LPDIRECTSOUNDNOTIFY		m_lpDSNotify;	// 再生バッファの通知イベント
	LPDIRECTSOUNDBUFFER		m_lpDSBuffer;	// DirectSound バッファ

	LPDIRECTSOUND			m_lpDS;			// DirectSound インターフェース

	// 再生場所通知箇所の指定 //
	DSBPOSITIONNOTIFY	m_PosNotify[NUM_WAVMUSIC_NOTIFY+1];

	HANDLE		m_hEvent;			// イベントハンドル

	DWORD		m_BufferSize;		// バッファのサイズ
	DWORD		m_NotifySize;		// 通知イベントの間隔
	DWORD		m_NextWriteOffset;	// 次の書き込み先
	DWORD		m_Progress;			// ファイル上での現在位置
	DWORD		m_LastPos;			// 前回の通知先

	DWORD		m_LoopStart;		// ループの開始バイト数
	DWORD		m_LoopEnd;			// ループの終了バイト数
	BOOL		m_IsPaused;			// 一時停止中であるか

	BYTE		m_Volume;			// ボリューム(補正前の値)

	PbgEx::WaveFileBase		*m_pFile;		// 波ファイル読み込み用抽象クラス

	PbgEx::CDirectShow		m_DShowFile;	// DirectShow による読み込みクラス
	PbgEx::CWaveFileRead	m_RawFile;		// ファイル読み出し用クラス
};



} // namespace Pbg



#endif
