/*
 *   PbgSound.h   : WAVE 管理クラス
 *
 */

#ifndef PBGSOUND_INCLUDED
#define PBGSOUND_INCLUDED "WAVE 管理クラス : Version 0.03 : Update 2001/09/13"

/*
 *  [関連するクラスの情報]
 *    CSound       : DirectSound 管理。
 *    CSoundBuffer : 同じ効果音の同時再生をサポート(CSoundState のベクタ)
 *    CSoundState  : 所有権付き再生(一つ一つの効果音管理)
 */

/*  [更新履歴]
 *    Version 0.03 : 2001/09/13 : 微調整
 *    Version 0.02 : 2001/01/21 : CSoundBuffer/State に分割する
 *    Version 0.01 : 2001/01/18 : クラス設計・その他
 */



#include "PbgType.h"
#include "CSoundBuffer.h"
#include "CSoundState.h"
#include "CWaveMusic.h"



namespace Pbg {



/***** [ 定数 ] *****/
#define ENUM_SOUND_DRIVER	5		// サウンドドライバの列挙数(最大)
#define SNDDEV_NAME_LENGTH	35		// 列挙した名前の最大長
#define SOUND_BUFFER_SIZE	40		// 同時にロード可能な効果音の種類最大数



/***** [クラス定義] *****/

// サウンドドライバ情報 //
typedef struct tagSndDrvInfo {
	GUID	m_Guid;				// ドライバＧＵＩＤ
	char	m_strGuid[32+4+1];	// ＧＵＩＤをＣ文字列にしたもの

	// DirectSound デバイスのテキスト記述 //
	char	m_strDesc[SNDDEV_NAME_LENGTH+1];

	// DirectSound ドライバのモジュール名 //
	char	m_DrvName[SNDDEV_NAME_LENGTH+1];
} SndDrvInfo;


// ＷＡＶＥ管理クラス //
class CSound {
public:
	BOOL Initialize(HWND hWnd);		// デフォルト(ID=0)のドライバで初期化
	void Cleanup(void);				// サウンドに関係するオブジェクトの解放

	void StopAllSoundBuffer(void);		// 全ての音を停止する(所有権も奪う)
	void SetMasterVolume(BYTE Volume);	// マスターボリュームを変更する
	BYTE GetMasterVolume(void);			// マスターボリュームを取得する

	// WAVE ファイルからサウンドバッファを生成する //
	BOOL CreateSoundBuffer(CSoundBuffer **ppBuffer);

	// 曲再生用のスタティックバッファを生成する //
	BOOL CreateWaveMusicBuffer(CWaveMusic **ppMusic);

	// 作成したサウンドバッファを削除する //
	void DeleteAllSoundBuffer(void);	// 全て削除

	BOOL  ChangeDriver(DWORD DrvID);	// ドライバを変更する
	DWORD GetNumDevices(void);			// DirectSound デバイスの数を取得する

	CSound();		// コンストラクタ
	~CSound();		// デストラクタ


private:
	// DirectSound ドライバを列挙する //
	static BOOL CALLBACK EnumDrivers(
		GUID *pGuid, LPCSTR strDesc, LPCSTR strDrvName, VOID *pContext);


private:
	LPDIRECTSOUND			m_lpDS;			// DirectSound インターフェース
	LPDIRECTSOUNDBUFFER		m_lpPrim;		// プライマリサウンドバッファ

	// スタティックサウンドバッファを所有するポインタへのポインタ //
	CSoundBuffer	**m_ppSoundBuffer[SOUND_BUFFER_SIZE];
	CWaveMusic		**m_ppWaveMusic;

	HWND		m_hWindow;						// 対象となるウィンドウハンドル

	DWORD		m_CurrentDriverID;				// 現在使用しているドライバ番号
	DWORD		m_NumDriver;					// 列挙されたドライバ
	SndDrvInfo	m_DrvInfo[ENUM_SOUND_DRIVER];	// ドライバ情報の格納先

	BYTE		m_MasterVolume;					// マスターボリューム
};



} // namespace Pbg



#endif
