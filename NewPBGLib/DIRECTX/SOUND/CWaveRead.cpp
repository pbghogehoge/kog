/*
 *   CWaveRead.cpp   : WAVE 読み込み
 *
 */

#include "CWaveRead.h"
#include "PbgMem.h"
#include "PbgError.h"
#include "..\\..\\LZUTY\\LzDecode.h"



namespace PbgEx {


// コンストラクタ //
CWaveFileRead::CWaveFileRead()
{
	m_pwfx    = NULL;	// WAVEFORMATEX 構造体へのポインタ
	m_hmmioIn = NULL;	// マルチメディア I/O ハンドル(for WAVE)
	m_End     = 0;

	m_pMemoryData = NULL;	// メモリ上で展開した場合

	ZEROMEM(m_ckIn);		// マルチメディア RIFF チャンク
	ZEROMEM(m_ckInRiff);	// WAVE ファイルを開くのに使用
}


// デストラクタ //
CWaveFileRead::~CWaveFileRead()
{
	Close();			// ファイルが開いていたら、閉じる
	DeleteEx(m_pwfx);
}


// ファイルをオープンする //
BOOL CWaveFileRead::Open(char *FileName)
{
	// ファイル終端を初期化 //
	m_End = 0;

	// サイズを初期化 //
	m_Size = 0;

	// まず、ファイルがすでに開かれていたら、閉じる //
	if(FALSE == Close()) return FALSE;

	// ファイルをオープンする //
	if(FALSE == WaveOpenFile(FileName)) return FALSE;

	// 後は、ファイルの先頭に移動できれば成功 //
	if(FALSE == Reset()){
		Close();		// 失敗したので、ファイルを閉じる
		return FALSE;	// SDK のサンプルはこの Close() が抜けている...
	}

	return TRUE;
}


// 圧ファイルをオープンする //
BOOL CWaveFileRead::OpenP(char *pPackFileName, char *pFileID)
{
	Pbg::LzDecode		Decode;

	// ファイル終端を初期化 //
	m_End = 0;

	// サイズを初期化 //
	m_Size = 0;

	// まず、ファイルがすでに開かれていたら、閉じる //
	if(FALSE == Close()) return FALSE;

	// 圧縮済みファイルをオープンする //
	if(FALSE == Decode.Open(pPackFileName)) return FALSE;

	// 実際に解凍する //
	m_pMemoryData = Decode.Decode(pFileID);
	if(NULL == m_pMemoryData) return FALSE;

	// メモリからロードを行う //
	if(FALSE == WaveOpenMemory(m_pMemoryData, Decode.GetFileSize(pFileID))){
		return FALSE;
	}

	// 後は、ファイルの先頭に移動できれば成功 //
	if(FALSE == Reset()){
		Close();		// 失敗したので、ファイルを閉じる
		return FALSE;	// SDK のサンプルはこの Close() が抜けている...
	}

	// ファイルのオープンに成功した //
	return TRUE;
}


// 圧ファイルをオープンする //
BOOL CWaveFileRead::OpenP(char *pPackFileName, DWORD FileNo)
{
	Pbg::LzDecode		Decode;

	// ファイル終端を初期化 //
	m_End = 0;

	// サイズを初期化 //
	m_Size = 0;

	// まず、ファイルがすでに開かれていたら、閉じる //
	if(FALSE == Close()) return FALSE;

	// 圧縮済みファイルをオープンする //
	if(FALSE == Decode.Open(pPackFileName)) return FALSE;

	// 実際に解凍する //
	m_pMemoryData = Decode.Decode(FileNo);
	if(NULL == m_pMemoryData) return FALSE;

	// メモリからロードを行う //
	if(FALSE == WaveOpenMemory(m_pMemoryData, Decode.GetFileSize(FileNo))){
		return FALSE;
	}

	// 後は、ファイルの先頭に移動できれば成功 //
	if(FALSE == Reset()){
		Close();		// 失敗したので、ファイルを閉じる
		return FALSE;	// SDK のサンプルはこの Close() が抜けている...
	}

	// ファイルのオープンに成功した //
	return TRUE;
}


// ファイル先頭に戻る //
BOOL CWaveFileRead::Reset(void)
{
	LONG	Offset;

	// ファイル先頭からのオフセットを計算する //
	Offset = m_ckInRiff.dwDataOffset + sizeof(FOURCC);

	// ＷＡＶＥデータでの先頭(ヘッダ抜き?)に移動する //
	if(-1 == mmioSeek(m_hmmioIn, Offset, SEEK_SET)) return FALSE;

	// ファイル中の data チャンクを探す //
	m_ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(0 != mmioDescend(m_hmmioIn, &m_ckIn, &m_ckInRiff, MMIO_FINDCHUNK)){
		return FALSE;
	}

	m_Size = m_ckIn.cksize;

	return TRUE;
}


// ファイルを読み込む(Ret:読み込みSize) //
UINT CWaveFileRead::Read(BYTE *pData, UINT Size)
{
	MMIOINFO	mmioinfoIn;
	UINT		DataIn, temp;
	DWORD		cT;

	// 現在の状態を取得する //
	if(0 != mmioGetInfo(m_hmmioIn, &mmioinfoIn, 0)) return -1;

	temp = m_ckIn.cksize - m_End;
	if(temp <= 0) return -1;

	// 読み込むサイズを確定する //
	DataIn = min(Size, temp);//m_ckIn.cksize);
	m_ckIn.cksize -= DataIn;

	// それでは、読み込んでみましょう //
	for(cT=0; cT<DataIn; cT++){
		// pchNext : 読み取りまたは書き込みが可能なバッファの次のバイトを指します。
		//         : 読み取りまたは書き込みを行う場合、pchNext メンバを読み取られた
		//         : または書き込まれたバイト数だけ増やします。

		// pchEndRead  : 読み取り可能なバッファの最後の有効バイトの後の 1 バイトを指します。
		// pchEndWrite : 書き込み可能なバッファの最後の位置の後の 1 バイトを指します。

		if(mmioinfoIn.pchNext == mmioinfoIn.pchEndRead){
			if(0 != mmioAdvance(m_hmmioIn, &mmioinfoIn, MMIO_READ)){
				return -1;
			}
			if(mmioinfoIn.pchNext == mmioinfoIn.pchEndRead){
				return -1;
			}
		}

		// 実際に書き込みを行う //
		*((BYTE *)pData + cT) = *((BYTE *)mmioinfoIn.pchNext);

		// ポインタを進める //
		mmioinfoIn.pchNext++;
	}

	// バッファ情報を更新する //
	if(0 != mmioSetInfo(m_hmmioIn, &mmioinfoIn, 0)){
		return -1;
	}

	// 最後に、読み込んだバイト数を返す //
	return DataIn;
}


// ファイル内の移動(データ部における) //
BOOL CWaveFileRead::Seek(UINT Offset)
{
	// まずは、ファイルの先頭に戻る //
	if(FALSE == Reset()) return FALSE;

	// ＷＡＶＥデータのデータ部先頭から Offset だけ移動する //
	if(-1 == mmioSeek(m_hmmioIn, Offset, SEEK_CUR)) return FALSE;

//	m_ckIn.cksize -= Offset;
	m_ckIn.cksize = m_Size - Offset;

	return TRUE;
}


// 終端をセットする //
BOOL CWaveFileRead::SetEnd(UINT Offset)
{
	UINT	temp;

	temp = m_Size - Offset;

	if(temp < 0) return FALSE;

	m_End = temp;

	return TRUE;
}


// ファイルを閉じる //
BOOL CWaveFileRead::Close(void)
{
	// ファイルがオープン中ならば、閉じる //
	if(NULL != m_hmmioIn) mmioClose(m_hmmioIn, 0);

	// マルチメディアハンドルを無効化する //
	m_hmmioIn = NULL;

	// ヘッダも無効化する //
	DeleteEx(m_pwfx);

	m_End  = 0;
	m_Size = 0;

	// メモリ上に確保を行っている場合は、解放する //
	MemFree(m_pMemoryData);

	return TRUE;
}


// フォーマットを取得する //
WAVEFORMATEX *CWaveFileRead::GetFormat(void)
{
	return m_pwfx;
}


// バッファのサイズを取得 //
DWORD CWaveFileRead::GetSize(void)
{
	return m_Size;
}


// ファイルを開く(低レベル) //
BOOL CWaveFileRead::WaveOpenFile(char *FileName)
{
	// 専用のＡＰＩを用いてファイルをオープンする //
	m_hmmioIn = mmioOpen(FileName, NULL, MMIO_ALLOCBUF|MMIO_READ);
	if(NULL == m_hmmioIn) return FALSE;

	// ヘッダの読み込みを行う //
	if(FALSE == ReadMMIO()){
		mmioClose(m_hmmioIn, 0);	// 失敗したのでファイルを閉じる
		m_hmmioIn = NULL;			// ハンドルを無効化する

		return FALSE;
	}

	// ファイルオープンに成功ね //
	return TRUE;
}


// メモリ上のイメージを開く //
BOOL CWaveFileRead::WaveOpenMemory(LPVOID lpData, DWORD dwSize)
{
	MMIOINFO		mmioInfo;

	ZEROMEM(mmioInfo);
	mmioInfo.pchBuffer = (char *)lpData;
	mmioInfo.fccIOProc = FOURCC_MEM;
	mmioInfo.cchBuffer = dwSize;

	m_hmmioIn = mmioOpen(NULL, &mmioInfo, MMIO_READ);
	if(NULL == m_hmmioIn) return FALSE;

	// ヘッダの読み込みを行う //
	if(FALSE == ReadMMIO()){
		mmioClose(m_hmmioIn, 0);	// 失敗したのでファイルを閉じる
		m_hmmioIn = NULL;			// ハンドルを無効化する

		return FALSE;
	}

	// ファイルオープンに成功ね //
	return TRUE;
}


// ヘッダを調べる //
BOOL CWaveFileRead::ReadMMIO(void)
{
	MMCKINFO		ckIn;
	PCMWAVEFORMAT	pcmWaveFormat;
	LONG			size;
	WORD			extra;
	CHAR			*addr;

	// RIFF ファイルのチャンクに //
	if(0 != mmioDescend(m_hmmioIn, &m_ckInRiff, NULL, 0)){
		return FALSE;
	}

	// RIFF //
	if(FOURCC_RIFF != m_ckInRiff.ckid){
		return FALSE;
	}

	// WAVE //
	if(mmioFOURCC('W', 'A', 'V', 'E') != m_ckInRiff.fccType){
		return FALSE;
	}

	// 検索内容をセットする //
	ZEROMEM(ckIn);
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');

	// チャンクの検索開始！！ //
	if(0 != mmioDescend(m_hmmioIn, &ckIn, &m_ckInRiff, MMIO_FINDCHUNK)){
		return FALSE;	// 見つかりませんのぉ
	}

	// fmt チャンクは少なくとも PCMWAVEFORMAT 構造体よりも大きい //
	if(ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT)){
		return FALSE;
	}

	// ヘッダを読み込んで //
	size = mmioRead(m_hmmioIn, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat));
	if(size != sizeof(pcmWaveFormat)) return FALSE;

	// PCM-WAVE-FORMAT であった場合、構造体に必要なメモリを確保 //
	if(pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM){
		m_pwfx = NewEx(WAVEFORMATEX);
		if(NULL == m_pwfx) return FALSE;

		// 読み込んだデータを渡して //
		memcpy(m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
	}
	// 違ったので、格納するのに必要なバイト長を取得 //
	else{
		extra = 0;	// ゼロ初期化...
		size  = mmioRead(m_hmmioIn, (HPSTR)&extra, sizeof(WORD));
		if(size != sizeof(WORD)) return FALSE;

		// ちょっと大きめな構造体を作成する //
		m_pwfx = (WAVEFORMATEX *)NewEx(CHAR[sizeof(WAVEFORMATEX)+extra]);
		if(NULL == m_pwfx) return FALSE;

		// コピーして //
		memcpy(m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx->cbSize = extra;	// サイズがちょっと違う

		// それでは、大きい部分のデータを読み込んで //
		addr = (CHAR *) ( ((BYTE *) &(m_pwfx->cbSize)) + sizeof(WORD) );
		size = mmioRead(m_hmmioIn, addr, extra);
		if(size != extra){
			DeleteEx(m_pwfx);	// 読み込めませんでした
			return FALSE;
		}
	}

	// RIFF ファイルのチャンクから退出する //
	if(0 != mmioAscend(m_hmmioIn, &ckIn, 0)){
		DeleteEx(m_pwfx);	// 失敗したので、削除する
		return FALSE;
	}

	return TRUE;
}



} // namespace PbgEx
