/*
 *   CLoadStream.cpp   : 読み込みストリーム
 *
 */

#include "CLoadStream.h"
#include "VKeyCode.h"
#include "PbgMain.h"

#define LSID_DATA		0x00	// データが壊れている？
#define LSID_VERSION	0x01	// バージョンが異なる
#define LSID_MEMORY		0x02	// メモリ確保できない


//------------------------------------------------------------------------
//   名称  | CLoadStream::CLoadStream()
//   説明  | CLoadStream のコンストラクタ
//------------------------------------------------------------------------
CLoadStream::CLoadStream()
{
	m_pInfo    = NULL;	// ヘッダ
	m_pData    = NULL;	// データ格納域
	m_pCurrent = NULL;	// 現在位置
	m_DataSize = 0;		// データ長
}


//------------------------------------------------------------------------
//   名称  | CLoadStream::~CLoadStream()
//   説明  | CLoadStream のデストラクタ
//------------------------------------------------------------------------
CLoadStream::~CLoadStream()
{
	Cleanup();
}


//------------------------------------------------------------------------
//   名称  | CLoadStream::Open()
//   説明  | 読み込み開始
//  戻り値 | 成功ならば TRUE
//------------------------------------------------------------------------
FBOOL CLoadStream::Open(char *pFileName)
{
	Pbg::LzDecode		dec;
	DWORD				i, n, size;
	void				*pTemp;

	Cleanup();

	if(FALSE == dec.Open(pFileName)) return FALSE;

	size = 0;
	n    = dec.GetNumFiles();
	if(n < 2) return FALSE;		// ファイル数は２個以上

	// データヘッダ展開 //
	m_pInfo = (StreamInfoHeader *)dec.Decode((DWORD)0);
	if(NULL == m_pInfo){
		LoadFileError(pFileName, LSID_DATA);
		return FALSE;
	}

	// バージョン文字列を比較する //
	if(0 == strcmp(m_pInfo->m_VersionStr, KIOH_VERSION)){
		LoadFileError(pFileName, LSID_VERSION);
		return FALSE;
	}

	// データ格納に必要なサイズを求める //
	for(i=1; i<n; i++){
		size += dec.GetFileSize(i);
	}

	// データ格納用のメモリを確保する //
	m_pData = m_pCurrent = (WORD *)MemAlloc(sizeof(WORD) * size);
	if(NULL == m_pData){
		LoadFileError(pFileName, LSID_MEMORY);
		return FALSE;
	}

	// サイズをコピーする //
	m_DataSize = size;

	// データをコピーする //
	for(i=1; i<n; i++){
		pTemp = dec.Decode(i);
		if(NULL == pTemp){
			LoadFileError(pFileName, LSID_DATA);
			return FALSE;
		}

		size = dec.GetFileSize(i);
		memcpy(m_pData+(size/sizeof(WORD)), pTemp, size);

		MemFree(pTemp);
	}

	return TRUE;
}


//------------------------------------------------------------------------
//   名称  | CLoadStream::GetNextData()
//   説明  | 次のデータを取得
//  戻り値 | 得られたキーコード
//------------------------------------------------------------------------
FWORD CLoadStream::GetNextData(void)
{
	WORD		data;

	if(NULL == m_pData)                    return KEY_MENU;
	if(m_pCurrent - m_pData >= m_DataSize) return KEY_MENU;

	data = *m_pCurrent;
	m_pCurrent++;

	return data;
}


//------------------------------------------------------------------------
//   名称  | CLoadStream::GetRndSeed()
//   説明  | 乱数の種を取得
//  戻り値 | 読み込んだデータに対する乱数の種を取得
//------------------------------------------------------------------------
FDWORD CLoadStream::GetRndSeed(void)
{
	return m_pInfo->m_RndSeed;
}


//------------------------------------------------------------------------
//   名称  | CLoadStream::Cleanup()
//   説明  | メモリぽいぽい
//------------------------------------------------------------------------
FVOID CLoadStream::Cleanup(void)
{
	MemFree(m_pInfo);	// ヘッダ
	MemFree(m_pData);	// データ格納域

	m_pCurrent = NULL;	// 現在位置
	m_DataSize = 0;		// データ長
}


//------------------------------------------------------------------------
//   名称  | LoadFileError()
//   説明  | ファイルオープン失敗
//------------------------------------------------------------------------
FVOID CLoadStream::LoadFileError(char *pFileName, BYTE ErrorID)
{
	char			buf[MAX_PATH];

	switch(ErrorID){
	case LSID_DATA:
		wsprintf(buf, "ファイル \"%s\" が開けませんでした", pFileName);
	break;

	case LSID_VERSION:
		wsprintf(buf, "ファイル \"%s\" とバージョンの互換性がありません", pFileName);
	break;

	case LSID_MEMORY:
		wsprintf(buf, "リプレイデータの展開に必要なメモリが確保できませんでした", pFileName);
	break;

	default:
		strcpy(buf, "リプレイファイルに関するエラー");
	break;
	}

	PbgError(buf);
	Cleanup();
}
