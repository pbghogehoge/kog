/*
 *   CCfgIO.h   : コンフィグ入出力
 *
 */

#ifndef CCFGIO_INCLUDED
#define CCFGIO_INCLUDED "コンフィグ入出力 : Version 0.01 : Update 2001/08/05"

/*  [更新履歴]
 *    Version 0.01 : 2001/08/05 : 製作開始(残り２日)
 */



#include "PbgMain.h"



/***** [ 定数 ] *****/
#define FILEVERSION_LENGTH		80		// バージョン文字列長


// 曲の出力デバイス選択 //
typedef enum tagMUSIC_DEVICE {
	MDEV_NULL = 0x00,	// ヌルデバイス
	MDEV_MIDI = 0x01,	// ＭＩＤＩ
	MDEV_WAVE = 0x02,	// ＷＡＶＥ
} MUSIC_DEVICE;



/***** [クラス定義] *****/

// グラフィック情報構造体 //
typedef struct tagGrpCfgData {
	DWORD	m_Fps;		// ＦＰＳ設定
	BYTE	m_Gamma;	// γ補正値
} GrpCfgData;


// サウンド情報構造体 //
typedef struct tagSndCfgData {
	MUSIC_DEVICE	m_MusicDevice;	// 曲の出力デバイス

	BYTE			m_SEVolume;		// 効果音のボリューム
	BYTE			m_MusicVolume;	// 曲のボリューム

	BYTE			m_MaxMidiDevice;		// MIDIデバイス数
	BYTE			m_CurrentMidiDevice;	// 現在選択されているMIDIデバイス
} SndCfgData;


// 入力デバイス情報構造体 //
typedef struct tagInpCfgData {
	BYTE	FullKeyCfg[8+3];	// キーボード１Ｐ
	BYTE	HalfKey1PCfg[8+3];	// キーボード２Ｐ・１
	BYTE	HalfKey2PCfg[8+3];	// キーボード２Ｐ・２

	BYTE	Pad1PCfg[4];	// パッド１Ｐ
	BYTE	Pad2PCfg[4];	// パッド２Ｐ
} InpCfgData;


// その他の情報 //
typedef struct tagExtraData {
	DWORD		m_ExecCount;		// 起動回数カウンタ
	DWORD		m_NumCharacters;	// 使用できるキャラクタ数
} ExtraData;


// コンフィグ情報構造体 //
typedef struct tagConfigData {
	GrpCfgData	m_Grp;		// グラフィックの設定
	SndCfgData	m_Snd;		// サウンドの設定
	InpCfgData	m_Inp;		// 入力デバイスの設定
	ExtraData	m_Ext;		// その他の設定
} ConfigData;


// コンフィグ保存＆設定用クラス //
class CCfgIO {
public:
	FVOID GetGrpCfg(GrpCfgData *pGrp);	// グラフィックの設定を取得
	FVOID SetGrpCfg(GrpCfgData *pGrp);	// グラフィックの設定を書き込み

	FVOID GetSndCfg(SndCfgData *pSnd);	// サウンドの設定を取得
	FVOID SetSndCfg(SndCfgData *pSnd);	// サウンドの設定を書き込み

	FVOID GetInpCfg(InpCfgData *pInp);	// 入力デバイスの設定を取得
	FVOID SetInpCfg(InpCfgData *pInp);	// 入力デバイスの設定を書き込み

	FVOID SaveConfig(void);				// 設定をファイルに保存する

	FDWORD GetNumExecCount(void);		// 起動回数を取得する
	FVOID  SetNumCharacters(int n);		// 使用できるキャラクタ数をセットする
	FDWORD GetNumCharacters(void);		// 使用できるキャラクタ数を返す

	CCfgIO();		// コンストラクタ
	~CCfgIO();		// デストラクタ


private:
	FVOID LoadConfig(void);			// 設定を読み込む
	FVOID InitializeConfig(void);	// コンフィグ情報の初期化を行う

	FVOID InitializeGrp(void);		// グラフィック設定の初期化
	FVOID InitializeSnd(void);		// サウンド設定の初期化
	FVOID InitializeInp(void);		// 入力設定の初期化
	FVOID InitializeExtra(void);	// その他の情報の初期化

	ConfigData		m_ConfigData;					// コンフィグ情報
};



#endif
