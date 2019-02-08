/*
 *   PbgMidi.h   : ＭＩＤＩコントロール
 *
 */

#ifndef PBGMIDI_INCLUDED
#define PBGMIDI_INCLUDED "ＭＩＤＩコントロール : Version 0.02 : Update 2001/09/10"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/10 : 微調整
 *
 *    Version 0.01 : 2001/05/29 : 制作開始
 *                              : 再生中しかデバイスを占有しない
 */



#include "PbgType.h"
#include "PbgMem.h"
#include "PbgTimer.h"
#include "CCriticalSection.h"
#include "MidiNotify.h"



/***** [ 定数 ] *****/

// MIDI 再生の状態 //
#define MIDIST_STOP		0x00	// 停止中
#define MIDIST_PLAY		0x01	// 再生中
#define MIDIST_LOOP		0x02	// ループ再生中
//#define MIDIST_FADE		0x03	// フェードアウト中

#define MIDI_STDTEMPO	(1<<7)	// 標準のテンポ補正値
#define MIDI_STDVOLUME	255		// 標準のボリューム
#define MIDI_NUMPORT	2		// MIDI の同時使用可能な最大ポート数



/***** [ 型宣言 ] *****/

// コールバック関数の戻り値 //
typedef enum tagMIDIN_RET {
	MRET_OK,		// デフォルト動作要求
	MRET_STOP,		// 停止要求
} MIDIN_RET;

// コールバック関数の型 //
typedef MIDIN_RET (*PBGMIDI_CALLBACK)(BYTE Message, DWORD Param);



namespace Pbg {



/***** [クラス定義] *****/

// MIDI デバイスを列挙するクラス //
class MidiEnum {
public:
	int  GetNumDevices(void);						// デバイス数を取得する
	BOOL GetDeviceName(int ID, char *pDeviceName);	// デバイス名を取得する


protected:
	HMIDIOUT OpenDevice(int ID);			// デバイスを開く
	void     CloseDevice(HMIDIOUT &rhMidi);	// デバイスを閉じる

	MidiEnum();		// コンストラクタ
	~MidiEnum();	// デストラクタ


private:
	MIDIOUTCAPS		*m_pDeviceCaps;		// デバイス性能
	int				m_NumDevices;		// デバイス数
};


// MIDI トラック管理構造体 //
class MidiTrack {
public:
	int			m_DataSize;		// このトラックのデータサイズ
	int			m_WaitCount;	// 現在のウェイトカウント
	BOOL		m_bFinished;	// そのトラックの再生が完了しているか

	BYTE		*m_pDataHead;	// データ先頭
	BYTE		*m_pCurrentPos;	// 現在位置

	BYTE		m_Status;		// 状態
	BYTE		m_TargetPort;	// 対象となるポート(A or B)

	MidiTrack(){
		m_pDataHead   = NULL;	// 先頭
		m_pCurrentPos = NULL;	// 現在位置
		m_TargetPort  = 0;		// 対象ポート
		m_bFinished   = TRUE;	// 終了しているか
	};

	~MidiTrack(){
		// 確保されているならば、解放する //
		if(m_pDataHead) DeleteEx(m_pDataHead);
	}
};


// MIDI ポート管理構造体 //
typedef struct tagMidiPort {
	HMIDIOUT	m_hMidi;				// MIDI デバイスのハンドル
	BYTE		m_PortID;				// ポートＩＤ

	BYTE		m_SpectEfc[16][128];	// 疑似スペアナ表示用
	BYTE		m_NoteOn[16][128];		// ノートＯＮなら非ゼロ
	BYTE		m_NoteOffMask[16][128];	// On:0xff   Off:0x00
	BYTE		m_PanPod[16];			// パン
	BYTE		m_Volume[16];			// ボリューム
	BYTE		m_Expression[16];		// エクスプレッション
} MidiPort;


// MIDI 再生クラス //
class PbgMidi : public MidiEnum {
public:
	BOOL Load(char *pFileName);						// ファイルから曲をロード
	BOOL LoadP(char *pPackFileName, char *pFileID);	// 圧ファイルからロード(ID)
	BOOL LoadP(char *pPackFileName, DWORD FileNo);	// 圧ファイルからロード(No)

	BOOL Play(BOOL bLoop);				// 再生を開始する
	void Stop(void);					// [CS] 停止する
	void Fade(BYTE Speed, BYTE Volume);	// [CS] フェード(Volume : 目標値)
	void Tempo(char Tempo);				// [CS] テンポ
	void SetMasterVolume(BYTE Volume);	// [CS] マスターボリューム変更
	void Cleanup(void);					// データを解放する

	// [CS] イベント通知関数をセットする(NULL で解除できる) //
	void SetNotify(PBGMIDI_CALLBACK pFunction);

	void GetNoteOn(int Track, BYTE *pTable);	// [CS] ノートオンの状態を取得
	void GetPanPod(BYTE *pTable);				// [CS] パンの状態を取得
	void GetVolume(BYTE *pTable);				// [CS] ボリュームの状態を取得
	void GetExpression(BYTE *pTable);			// [CS] エクスプレッションの状態を取得


	DWORD GetPlayTimes(void);		// [CS] 現在の再生時間を取得する
	BOOL  GetTitle(char *pData);	// [CS] タイトル文字列を取得する
	BYTE  GetStatus(void);			// [CS] 現在の状態を取得する

	// [CS] 使用デバイスを変更する(32ch 対応!!) //
	BOOL ChangeDevice(int PortA_ID, int PortB_ID);

	PbgMidi();		// コンストラクタ
	~PbgMidi();		// デストラクタ


private:
	// メモリから読み込みを行う //
	BOOL LoadFromMemory(BYTE *pData);

	// 各トラックの初期化を行う //
	void InitializeTrack(void);

	// ノート、パンなどのポート情報を初期化する //
	void InitializePortData(void);

	// ＧＭリセットを放つ //
	void GM_Reset(void);

	// コールバック関数が使用する停止用関数 //
	void InternalStop(void);

	// タイマー呼び出しのコールバック関数 //
	static void CALLBACK
		MidiCallbackFn(UINT uID, UINT uMsg, DWORD dwUser, DWORD, DWORD);

	// 解析部 //
	IVOID MidiParse(MidiTrack *pTrack, MidiPort *pPort, int BaseVolume);

	// タイトル解析 //
	FVOID CheckTitle(BYTE *pData, DWORD dwSize);


	int			m_Format;		// MIDI フォーマット
	int			m_NumTracks;	// トラック数
	int			m_TimeBase;		// タイムベース
	int			m_Tempo;		// テンポ

	DWORDLONG	m_PlayTime;		// 再生時間(ミリ秒)
	DWORDLONG	m_PlayCount1;	// カウンタ１(適当だな)
	DWORDLONG	m_PlayCount2;	// カウンタ２(...)

	MidiPort	m_Port[MIDI_NUMPORT];	// MIDI ポート管理構造体
	MidiTrack	*m_aTrack;				// MIDI トラック管理構造体(配列)

	PBGMIDI_CALLBACK	m_pCallBack;	// イベント通知コールバック関数

	int		m_ExtraTempo;	// テンポ補正用の値(Up/Down)
	int		m_ExtraVolume;	// 基本的に フェード用
	int		m_MasterVolume;	// こちらがマスターボリュームエミュレーション用
	int		m_FadeTarget;	// フェードアウト目標値

	BOOL	m_bAlwaysCloseDevice;	// 停止時に必ずデバイスを解放するか

	BOOL	m_bIsFadeOut;	// フェードアウト中なら真
	int		m_FadeCount;	// 現在のフェードアウトカウンタ値
	int		m_FadeWait;		// フェードアウトの待ちカウント
	BYTE	m_PlayState;	// 現在の状態(MIDIST_xxxx)
	BYTE	m_FadeOutCh;	// 次のフェードアウト対象チャンネル

	char	m_MusicTitle[MAX_PATH];		// 曲名バッファ


	// まるちめでぃあタイマー //
	Pbg::CPbgTimer		m_Timer;

	// 解放順序を考えると、ここにいるのが妥当かと //
	Pbg::CCriticalSection		m_Critical;
};



} // namespace Pbg



#endif
