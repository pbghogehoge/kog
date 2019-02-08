/*
 *   CCfgIO.cpp   : コンフィグ入出力
 *
 */

#include "CCfgIO.h"
#include "CKeyInput.h"
#include "CPadInput.h"



char *g_pCfgFileName = "稀翁CFG.dat";
char *g_pGrpCfgID    = "CFG/GRAPH";
char *g_pSndCfgID    = "CFG/SOUND";
char *g_pInpCfgID    = "CFG/INPUT";
char *g_pExtraCfgID  = "CFG/EXTRA";


// コンストラクタ //
CCfgIO::CCfgIO()
{
	LoadConfig();
}


// デストラクタ //
CCfgIO::~CCfgIO()
{
	// 起動回数をインクリメント //
	m_ConfigData.m_Ext.m_ExecCount += 1;

	SaveConfig();
}



// グラフィックの設定を取得 //
FVOID CCfgIO::GetGrpCfg(GrpCfgData *pGrp)
{
	*pGrp = m_ConfigData.m_Grp;
}


// グラフィックの設定を書き込み //
FVOID CCfgIO::SetGrpCfg(GrpCfgData *pGrp)
{
	m_ConfigData.m_Grp = *pGrp;
}


// サウンドの設定を取得 //
FVOID CCfgIO::GetSndCfg(SndCfgData *pSnd)
{
	*pSnd = m_ConfigData.m_Snd;
}


// サウンドの設定を書き込み //
FVOID CCfgIO::SetSndCfg(SndCfgData *pSnd)
{
	m_ConfigData.m_Snd = *pSnd;
}


// 入力デバイスの設定を取得 //
FVOID CCfgIO::GetInpCfg(InpCfgData *pInp)
{
	*pInp = m_ConfigData.m_Inp;
}


// 入力デバイスの設定を書き込み //
FVOID CCfgIO::SetInpCfg(InpCfgData *pInp)
{
	m_ConfigData.m_Inp = *pInp;
}


// 設定を読み込む //
FVOID CCfgIO::LoadConfig(void)
{
	Pbg::LzDecode		Decode;
	GrpCfgData			*pGrp;
	SndCfgData			*pSnd;
	InpCfgData			*pInp;
	ExtraData			*pExtra;
	int					n;

	// ファイルがオープンできない場合、強制的に初期化する //
	if(FALSE == Decode.Open(g_pCfgFileName)){
		InitializeConfig();
		return;
	}


	// 参考：別個に保存してロードするのは、ファイルが多少破壊されていても //
	//       その他のデータは破壊されないような状態を作り出す為である     //

	// グラフィック系のデータをロードする //
	pGrp = (GrpCfgData *)Decode.Decode(g_pGrpCfgID);
	if(NULL == pGrp){
		InitializeGrp();
	}
	else{
		m_ConfigData.m_Grp = *pGrp;
		MemFree(pGrp);
	}

	// サウンド系のデータをロードする //
	pSnd = (SndCfgData *)Decode.Decode(g_pSndCfgID);
	if(NULL == pSnd){
		InitializeSnd();
	}
	else{
		m_ConfigData.m_Snd = *pSnd;
		MemFree(pSnd);
	}

	// 入力系のデータをロードする //
	pInp = (InpCfgData *)Decode.Decode(g_pInpCfgID);
	if(NULL == pInp){
		InitializeInp();
	}
	else{
		m_ConfigData.m_Inp = *pInp;
		MemFree(pInp);
	}

	// その他のデータをロードする //
	pExtra = (ExtraData *)Decode.Decode(g_pExtraCfgID);
	if(NULL == pExtra){
		InitializeExtra();
	}
	else{
		m_ConfigData.m_Ext = *pExtra;
		MemFree(pExtra);
	}

	n = m_ConfigData.m_Ext.m_NumCharacters;
	if(n < 6 || n > 9) m_ConfigData.m_Ext.m_NumCharacters = 6;

/*
	char buf[100];
	wsprintf(buf, "Fps %d    / Gamma %d", m_ConfigData.m_Grp.m_Fps, m_ConfigData.m_Grp.m_Gamma);
	PbgError(buf);
*/
	// 最後にファイルを閉じる //
	Decode.Close();
/*
	char	buf[1024];
//	wsprintf(buf, "Gamma:%d   Fps:%d", m_ConfigData.m_Grp.m_Gamma, m_ConfigData.m_Grp.m_Fps);
	wsprintf(buf, "Dev:%d / %d     MDev:%d    MVol:%d    SEVol:%d", m_ConfigData.m_Snd.m_CurrentMidiDevice
					, m_ConfigData.m_Snd.m_MaxMidiDevice
					, m_ConfigData.m_Snd.m_MusicDevice
					, m_ConfigData.m_Snd.m_MusicVolume
					, m_ConfigData.m_Snd.m_SEVolume);
	PbgError(buf);
*/
}


// 設定を保存する //
FVOID CCfgIO::SaveConfig(void)
{
	Pbg::LzEncode		Encode;
	Pbg::CBitMemIn		MemFile;

	// ファイルがオープンできない場合、失敗 //
	if(FALSE == Encode.Open(g_pCfgFileName)){
		return;
	}

	// グラフィック系の保存 //
	MemFile.Attach(&m_ConfigData.m_Grp, sizeof(GrpCfgData));
	Encode.Encode(&MemFile, g_pGrpCfgID);
	MemFile.Detach();

	// サウンド系の保存 //
	MemFile.Attach(&m_ConfigData.m_Snd, sizeof(SndCfgData));
	Encode.Encode(&MemFile, g_pSndCfgID);
	MemFile.Detach();

	// 入力系の保存 //
	MemFile.Attach(&m_ConfigData.m_Inp, sizeof(InpCfgData));
	Encode.Encode(&MemFile, g_pInpCfgID);
	MemFile.Detach();

	// その他の保存 //
	MemFile.Attach(&m_ConfigData.m_Ext, sizeof(ExtraData));
	Encode.Encode(&MemFile, g_pExtraCfgID);
	MemFile.Detach();

	// 閉じる //
	Encode.Close();
/*
	char	buf[1024];
//	wsprintf(buf, "Gamma:%d   Fps:%d", m_ConfigData.m_Grp.m_Gamma, m_ConfigData.m_Grp.m_Fps);
	wsprintf(buf, "Dev:%d / %d     MDev:%d    MVol:%d    SEVol:%d", m_ConfigData.m_Snd.m_CurrentMidiDevice
					, m_ConfigData.m_Snd.m_MaxMidiDevice
					, m_ConfigData.m_Snd.m_MusicDevice
					, m_ConfigData.m_Snd.m_MusicVolume
					, m_ConfigData.m_Snd.m_SEVolume);
	PbgError(buf);
*/
}


// 起動回数を取得する //
FDWORD CCfgIO::GetNumExecCount(void)
{
	return m_ConfigData.m_Ext.m_ExecCount;
}


// 使用できるキャラクタ数をセットする //
FVOID  CCfgIO::SetNumCharacters(int n)
{
	m_ConfigData.m_Ext.m_NumCharacters = n;
}


// 使用できるキャラクタ数を返す //
FDWORD CCfgIO::GetNumCharacters(void)
{
	return m_ConfigData.m_Ext.m_NumCharacters;
}


// コンフィグ情報の初期化を行う //
FVOID CCfgIO::InitializeConfig(void)
{
	InitializeGrp();
	InitializeSnd();
	InitializeInp();
	InitializeExtra();
}


// グラフィック設定の初期化 //
FVOID CCfgIO::InitializeGrp(void)
{
	m_ConfigData.m_Grp.m_Fps   = 60;	// ＦＰＳの設定
	m_ConfigData.m_Grp.m_Gamma = 100;	// γの設定
}


// サウンド設定の初期化 //
FVOID CCfgIO::InitializeSnd(void)
{
	m_ConfigData.m_Snd.m_MusicDevice = MDEV_WAVE;	// 曲の出力デバイス
	m_ConfigData.m_Snd.m_MusicVolume = 255;			// 曲のボリューム
	m_ConfigData.m_Snd.m_SEVolume    = 255;			// 効果音のボリューム

	m_ConfigData.m_Snd.m_CurrentMidiDevice = 0;	// 選択中の MIDI デバイス
	m_ConfigData.m_Snd.m_MaxMidiDevice     = 0;	// MIDI デバイス数
}


// 入力設定の初期化 //
FVOID CCfgIO::InitializeInp(void)
{
	BYTE		buf[256];

	CKeyInput	FullKey(buf, IDEVID_FULLKEY);
	CKeyInput	HalfKey1P(buf, IDEVID_HALFKEY1P);
	CKeyInput	HalfKey2P(buf, IDEVID_HALFKEY2P);
	CPadInput	Pad(0);
/*
	FullKey.OnResetSetting();
	HalfKey1P.OnResetSetting();
	HalfKey2P.OnResetSetting();
	Pad.OnResetSetting();
*/
	FullKey.GetConfigData(m_ConfigData.m_Inp.FullKeyCfg);
	HalfKey1P.GetConfigData(m_ConfigData.m_Inp.HalfKey1PCfg);
	HalfKey2P.GetConfigData(m_ConfigData.m_Inp.HalfKey2PCfg);
	Pad.GetConfigData(m_ConfigData.m_Inp.Pad1PCfg);
	Pad.GetConfigData(m_ConfigData.m_Inp.Pad2PCfg);
}


// その他の情報の初期化 //
FVOID CCfgIO::InitializeExtra(void)
{
	m_ConfigData.m_Ext.m_ExecCount     = 0;
	m_ConfigData.m_Ext.m_NumCharacters = 6;
}


/*

	const int NumMainFiles = 3;
	const int NumSubFiles  = 2;

	// 必ず存在する必要があるファイル //
	char *pMainFileName[NumMainFiles] = {
		"GRAPH.DAT", "GRAPH2.DAT", "ENEMY.DAT"
	};

	// 必ずしも存在する必要のないファイル //
	char *pSubFileName[NumSubFiles] = {
		"SOUND.DAT", "MUSIC.DAT",
	};

	int					i;
	char				buf[1024];
	char				Version[256];
	char				*pTemp;
	BOOL				result;
	Pbg::LzDecode		Decode;
	ConfigData			Config;


	m_bSameVersion = FALSE;	// バージョンが一致しているなら真

	ZEROMEM(m_ConfigData);	// コンフィグ情報
	ZEROMEM(m_Version);		// バージョン情報


	// 主要ファイルのチェックを行う //
	for(i=0; i<NumMainFiles; i++){
		// ファイルの存在＆妥当性チェック //
		if(FALSE == Decode.Open(pMainFileName[i])){
			wsprintf(buf,	"\"%s\" が見つかりません。\n\t"
							"このファイルはプログラムの実行に必要です。"
							, pMainFileName[i]);
			PbgError(buf);
			return;
		}

		// バージョン情報の取得 //
		pTemp = (char *)Decode.Decode(pFileVersionID);
		if(NULL == pTemp){
			wsprintf(buf,	"\"%s\" からバージョン情報が取得できませんでした。"
							, pMainFileName[i]);
			PbgError(buf);
			return;
		}

		// 一つ目のファイルの場合 //
		if(0 == i){
			strcpy(Version, pTemp);
			result = TRUE;
		}
		// バージョンが一致しない場合 //
		else if(strcmp(pTemp, Version)){
			PbgError("各データ間のバージョンが一致しません");
			result = FALSE;
		}
		// バージョンが一致した場合 //
		else{
			result = TRUE;
		}

		MemFree(pTemp);		// 解放して
		Decode.Close();		// 閉じる

		if(FALSE == result) return;
	}
	// 補助ファイルのチェックを行う //
	for(i=0; i<NumSubFiles; i++){
		// ファイルの存在＆妥当性チェック //
		if(FALSE == Decode.Open(pSubFileName[i])){
			wsprintf(buf,	"\"%s\" が見つかりませんな。", pSubFileName[i]);
			PbgError(buf);
			continue;
		}

		// バージョン情報の取得 //
		pTemp = (char *)Decode.Decode(pFileVersionID);
		if(NULL == pTemp){
			wsprintf(buf,	"\"%s\" からバージョン情報が取得できませんでした。"
							, pSubFileName[i]);
			PbgError(buf);
			return;		// これはファイルが壊れていることを意味しているので偽を返す
		}

		// バージョンが一致しない場合 //
		if(strcmp(pTemp, Version)){
			PbgError("各データ間のバージョンが一致しないので、正常に動作しない可能性があります");
		}

		MemFree(pTemp);		// 解放して
		Decode.Close();		// 閉じる
	}

	// 新規作成の場合 //
	if(FALSE == LoadConfig()){
		strcpy(m_Version, Version);		// バージョン文字列をコピーする
		InitializeConfig();				// コンフィグ情報の初期化
	}
	// すでに存在するがバージョンが一致しない場合 //
	else if(strcmp(Config.m_FileVerson, Version)){
		wsprintf(buf, "データファイルと \"%s\" のバージョンが一致しません ", pCfgFileName);
		PbgError(buf);
		return;
	}

	// バージョン情報をコピーする //
	strcpy(m_ConfigData.m_FileVerson, m_Version);

	m_bSameVersion = TRUE;

*/
