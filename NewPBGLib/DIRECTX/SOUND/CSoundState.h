/*
 *   CSoundState.h   : 占有モード用クラス
 *
 */

#ifndef CSOUNDSTATE_INCLUDED
#define CSOUNDSTATE_INCLUDED "占有モード用クラス : Version 0.02 : Update 2001/09/14"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/14 : SetPan() の修正
 *    Version 0.01 : 2001/01/21 : PbgSound から分割 / ほぼ完成
 */



#include "PbgType.h"



namespace Pbg {



/***** [クラス定義] *****/

class CSoundBuffer;

// 占有時に利用するクラス(注意:変数のセットはCSoundBuffer が行う) //
class CSoundState {
	friend class CSoundBuffer;


	// 占有時に提供するメンバ関数 //
public:
	void SetPan(LONG Pan);					// パンを変更する
	void SetVolume(BYTE Volume);			// ボリュームを変更する
	void SetFrequency(DWORD Frequency);		// 周波数を変更する
	void Stop(void);						// 停止＆所有権消失

	// マスターボリュームの変更時に呼び出す //
	void OnMasterVolumeChanged(BYTE MasterVolume);

	CSoundState();		// コンストラクタ
	~CSoundState();		// デストラクタ


private:
	CSoundState				**m_ppOwner;	// 所有者へのポインタ
	LPDIRECTSOUNDBUFFER		m_lpBuffer;		// サウンドバッファ

	BYTE		m_Volume;
	BYTE		m_MasterVolume;
};



} // namespace Pbg



#endif
