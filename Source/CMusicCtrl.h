/*
 *   CMusicCtrl.h   : 音楽管理クラス
 *
 */

#ifndef CMUSICCTRL_INCLUDED
#define CMUSICCTRL_INCLUDED "音楽管理クラス : Version 0.01 : Update 2001/06/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/06/27 : 制作開始
 */

#include "PbgMain.h"
#include "CCfgIO.h"



/***** [クラス定義] *****/

// うむ //
//class CCfgIO;


// 音楽管理クラス //
class CMusicCtrl {
public:
	FVOID Play(void);							// 再生する
	FVOID Stop(void);							// 停止する
	FVOID Fade(BYTE Speed, BYTE Volume);		// フェードイン・アウト
	FBOOL ChangeMusicDev(MUSIC_DEVICE MDevice);	// デバイス(MIDI/WAVE)を変更する
	FBOOL Load(int FileNo);						// 曲のロードを行う(番号)

	FVOID SetMusicVolume(BYTE Volume);			// 曲のボリュームを変更する
	FBYTE GetMusicVolume(void);					// 曲のボリュームを取得する
	FVOID SetSoundVolume(BYTE Volume);			// 効果音のボリュームをセット
	FBYTE GetSoundVolume(void);					// 効果音のボリュームを取得


	FVOID GetTitle(char *pTitle);			// ロードされている曲名を取得

	MUSIC_DEVICE GetMusicDev(void);			// 現在選択されているデバイスを取得

	// ＭＩＤＩ専用命令 //
	FBOOL ChangeMidDev(BYTE DevID);					// ＭＩＤＩ出力ポートを変更
	FBYTE GetNumMidDev(void);						// ＭＩＤＩデバイス数を取得
	FBOOL GetMidDevName(char *pName, BYTE DevID);	// ＭＩＤＩデバイス名を取得
	FBYTE GetCurrentMidDev(void);					// 現在のデバイスを取得
	FBOOL IsEnableMid(void);						// ＭＩＤＩデバイスは有効か

	FVOID GetExpression(BYTE *pTable);			// エクスプレッションを取得する
	FVOID GetPanPod(BYTE *pTable);				// パンを取得する
	FVOID GetPartVolume(BYTE *pTable);			// パートのボリュームを取得する
	FVOID GetNoteOn(int Track, BYTE *pTable);	// ノートＯＮ／ＯＦＦの取得


	// ＷＡＶＥ専用命令 //
	FVOID GetWaveScope(BYTE Buffer[128+128]);		// 現在の波形を取得する
	FBOOL IsEnableWave(void);						// ＷＡＶＥデバイスは有効か

	CMusicCtrl(HWND hWnd, CCfgIO *pCfgIO);	// コンストラクタ
	~CMusicCtrl();							// デストラクタ


private:
	FBOOL LoadFromID(char *pFileID);			// 曲のロードを行う(ＩＤ)


	// コンフィグ入出力用 //
	SndCfgData		m_SndCfg;	// 設定の状態格納用
	CCfgIO			*m_pCfgIO;	// 入出力先

//	MUSIC_DEVICE	m_MusicDevice;	// 曲の出力デバイス

	Pbg::PbgMidi		m_Midi;			// ＭＩＤＩ管理クラス
	Pbg::CWaveMusic		*m_pWave;		// ＷＡＶＥストリーム管理クラス

	BOOL		m_bEnableMid;		// みぢデバイスが有効
	BOOL		m_bEnableWave;		// 波デバイスが有効

//	char			m_FileName[MAX_PATH];	// ファイル名
	DWORD			m_FileNo;		// ファイル番号

//	BYTE			m_Volume;	// 現在のボリューム
//	BYTE			m_MidiDev;	// 現在のＭＩＤＩデバイス
	BYTE			m_State;	// 現在の状態(再生・停止・フェード)
};



/****** [グローバル変数] *****/
extern Pbg::CSound			*g_pSnd;	// サウンド管理



#endif
