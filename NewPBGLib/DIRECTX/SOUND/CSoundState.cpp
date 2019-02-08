/*
 *   CSoundState.cpp   : 占有モード用クラス
 *
 */

#include "CSoundState.h"
#include "DSTable.h"
#include "PbgError.h"



namespace Pbg {


// コンストラクタ //
CSoundState::CSoundState()
{
	m_lpBuffer      = NULL;		// バッファ
	m_ppOwner       = NULL;		// 所有者へのポインタ

	m_Volume       = 255;	// ローカルボリューム
	m_MasterVolume = 255;	// マスターボリューム
}


// デストラクタ //
CSoundState::~CSoundState()
{
	Stop();		// 停止＆所有権を奪う

	SAFE_RELEASE(m_lpBuffer);
}


// パンを変更する //
void CSoundState::SetPan(LONG Pan)
{
	HRESULT		hr;

	if(NULL == this) return;

#ifdef PBG_DEBUG
	if(NULL == m_lpBuffer)
		PbgError("バッファの確保が出来ていない");
#endif


	// DSBPAN_LEFT から DSBPAN_RIGHT の範囲内におさめる
	if(     Pan < DSBPAN_LEFT)  Pan = DSBPAN_LEFT;		// 左
	else if(Pan > DSBPAN_RIGHT) Pan = DSBPAN_RIGHT;		// 右


	// パンをセット //
	hr = m_lpBuffer->SetPan(Pan);
	if(FAILED(hr)){
		PbgDebugLog("CSoundState::SetPan()  SetPan() に失敗");
	}
}


// ボリュームを変更する //
void CSoundState::SetVolume(BYTE Volume)
{
	HRESULT		hr;
	LONG		DSoundVolume;
	int			temp;

	if(NULL == this) return;

	// ボリュームは覚えておく //
	m_Volume = Volume;

#ifdef PBG_DEBUG
	if(NULL == m_lpBuffer){
		PbgError("CSoundState::SetVolume()  バッファの確保が出来ていない");
		return;
	}
#endif

	// ＤｉｒｅｃｔＳｏｕｎｄでのボリューム設定値に変更 //
	temp = ((int)Volume * (int)m_MasterVolume) / 255;
	DSoundVolume = DSVolumeTable[temp];

	// 実際にボリュームをセットする //
	hr = m_lpBuffer->SetVolume(DSoundVolume);
	if(FAILED(hr)){
		PbgDebugLog("CSoundState::SetVolume()  SetVolume() に失敗");
	}
}


// 周波数を変更する //
void CSoundState::SetFrequency(DWORD Frequency)
{
	HRESULT		hr;

	if(NULL == this) return;

#ifdef PBG_DEBUG
	if(NULL == m_lpBuffer)
		PbgError("バッファの確保が出来ていない");
#endif

	// 周波数をセット //
	hr = m_lpBuffer->SetFrequency(Frequency);
	if(FAILED(hr)){
		PbgDebugLog("CSoundState::SetFrequency()  SetFrequency() に失敗");
	}
}


// 停止＆所有権消失 //
void CSoundState::Stop(void)
{
	HRESULT		hr;

	// だめ //
	if(NULL == this) return;

#ifdef PBG_DEBUG
	if(NULL == m_lpBuffer)
		PbgError("バッファの確保が出来ていない");
#endif

	// ここに停止用のコードを記述 //
	hr = m_lpBuffer->Stop();
	if(FAILED(hr)){
		PbgDebugLog("CSoundState::Stop()  Stop() に失敗");
	}

	// 一応、バッファの先頭にカーソルを移動する              //
	// 自動的に行ってくれるはずなのだが、信用できないので... //
	hr = m_lpBuffer->SetCurrentPosition(0);
	if(FAILED(hr)){
		PbgDebugLog("CSoundState::Stop  SetCurrentPosition() に失敗");
	}

	// 所有権付きの場合、所有元のポインタを無効化する //
	if(NULL != m_ppOwner){
		*m_ppOwner = NULL;		// 実際のオブジェクト
		m_ppOwner  = NULL;		// 持ち主へのポインタ
	}
}


// マスターボリュームの変更時に呼び出す //
void CSoundState::OnMasterVolumeChanged(BYTE MasterVolume)
{
	if(NULL == this) return;

	m_MasterVolume = MasterVolume;		// ボリュームを覚えて
	SetVolume(m_Volume);
}



} // namespace Pbg
