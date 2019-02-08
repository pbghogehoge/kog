/*
 *   CSoundBuffer.h   : サウンドバッファ管理
 *
 */

#ifndef CSOUNDBUFFER_INCLUDED
#define CSOUNDBUFFER_INCLUDED "サウンドバッファ管理 : Version 0.02 : Update 2001/09/13"

/*  [参考]
 *    このクラスでは、同じファイルから作成された効果音を同時に複数再生する機能をサポートしている。
 */

/*  [更新履歴]
 *    Version 0.02 : 2001/09/13 : 一部コメントを修正
 *    Version 0.01 : 2001/01/21 : PbgSound.h を分割
 */



#include "PbgType.h"
#include "CWaveRead.h"



namespace Pbg {



/***** [クラス定義] *****/

// 相互参照のため、ここに記述 //
class CSoundState;


// サウンドバッファ管理クラス //
class CSoundBuffer {
public:
	void Play(void);									// デフォルト再生
	void Play(LONG Pan, DWORD Frequency, BYTE Volume);	// 特殊再生
	void PlayEx(CSoundState *&lprState);				// 占有ループ再生

	void StopAll(void);		// 全バッファの停止＆バッファ占有権を奪う

	// ファイルをオープンする //
	BOOL Load(char *FileName, DWORD NumBuffer);

	// 圧ファイルをオープンする //
	BOOL LoadP(char *pPackFileName, char *pFileID, DWORD NumBuffer);
	BOOL LoadP(char *pPackFileName, DWORD FileNo, DWORD NumBuffer);

	// マスターボリュームが変更された //
	void OnMasterVolumeChanged(void);

	CSoundBuffer(LPDIRECTSOUND lpDS, BYTE &rMasterVolume);	// コンストラクタ
	~CSoundBuffer();										// デストラクタ


private:
	void Initialize(void);					// オブジェクトを解放する
	BOOL Restore(void);						// バッファを復旧する
	BOOL CreateBuffer(DWORD NumBuffer);		// バッファを作成する
	BOOL FillBuffer(CSoundState *pTarget);	// バッファにデータをロードする


private:
	LPDIRECTSOUND			m_lpDS;			// DirectSound インターフェース
	DWORD					m_NumBuffer;	// バッファ数
	CSoundState				*m_aBuffer;		// スタティックバッファ
	PbgEx::CWaveFileRead	m_WaveFile;		// Restore 用のWAVEデータ

	BYTE					&m_rMasterVolume;	// マスターボリュームへの参照
};



} // namespace Pbg



#endif
