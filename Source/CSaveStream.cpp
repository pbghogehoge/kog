/*
 *   CSaveStream.cpp   : 保存ストリーム
 *
 */

#include "CSaveStream.h"
#include "VKeyCode.h"



//------------------------------------------------------------------------
//   名称  | CSaveStream::CSaveStream()
//   説明  | コンストラクタ
//------------------------------------------------------------------------
CSaveStream::CSaveStream()
{
	m_Current = 0;
	m_bOpened = FALSE;
}


//------------------------------------------------------------------------
//   名称  | CSaveStream::~CSaveStream()
//   説明  | デストラクタ
//------------------------------------------------------------------------
CSaveStream::~CSaveStream()
{
	Close();
}


//------------------------------------------------------------------------
//   名称  | CSaveStream::Open()
//   説明  | 書き込み開始
//  戻り値 | オープンできたら TRUE を返す
//------------------------------------------------------------------------
FBOOL CSaveStream::Open(char *pFileName, DWORD RndSeed)
{
	StreamInfoHeader		info;
	Pbg::CBitMemIn			mem;

	Close();

	if(FALSE == m_Enc.Open(pFileName)) return FALSE;

	info.m_RndSeed = RndSeed;
	strcpy(info.m_VersionStr, KIOH_VERSION);

	mem.Attach(&info, sizeof(info));
	if(FALSE == m_Enc.Encode(&mem, "FILE_HEADER")) return FALSE;

	m_bOpened = TRUE;
	m_Current = 0;

	return TRUE;
}


//------------------------------------------------------------------------
//   名称  | CSaveStream::WriteData()
//   説明  | データを書き込む
//  戻り値 | 書き込みに失敗した場合 FALSE
//------------------------------------------------------------------------
FBOOL CSaveStream::WriteData(WORD KeyCode)
{
	if(FALSE == m_bOpened) return FALSE;

	m_WriteData[m_Current] = KeyCode;

	m_Current++;
	if((m_Current >= BUFFER_SIZE) || (KeyCode & KEY_MENU)){
		Pbg::CBitMemIn		mem;

		mem.Attach(m_WriteData, m_Current, NULL);

		m_Current = 0;

		if(KeyCode & KEY_MENU){
			m_bOpened = FALSE;
		}

		return m_Enc.Encode(&mem, "DATA");
	}

	return TRUE;
}


//------------------------------------------------------------------------
//   名称  | CSaveStream::Close()
//   説明  | 閉じる
//  戻り値 | 書き込みに失敗した場合 FALSE
//------------------------------------------------------------------------
FBOOL CSaveStream::Close(void)
{
	WriteData(KEY_MENU);

	return m_Enc.Close();
}
