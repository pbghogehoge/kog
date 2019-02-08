/*
 *   PbgCache.cpp   : 圧ファイルのキャッシュ
 *
 */

#include "PbgCache.h"
#include "PbgMem.h"
#include "LzUty\\LzDecode.h"



namespace Pbg {



/***** [スタティックメンバ] *****/
CacheInfo CPbgCache::m_CacheInfo[PBGCACHE_MAX];	// キャッシュ情報
int       CPbgCache::m_RefCount = 0;			// 参照カウント



// コンストラクタ //
CPbgCache::CPbgCache()
{
	// 参照カウントをインクリメントする //
	m_RefCount++;
}


// デストラクタ //
CPbgCache::~CPbgCache()
{
	int			i;

	m_RefCount--;

	// まだ参照されている場合は、戻る //
	if(m_RefCount > 0) return;

#ifdef PBG_DEBUG
	// デバッグ版なら、情報を出力する //
	OutputCacheInfo();
#endif

	// 全てのデータを解放する //
	for(i=0; i<PBGCACHE_MAX; i++){
		if(m_CacheInfo[i].m_pData){				// データが生きていれば
			MemFree(m_CacheInfo[i].m_pData);	// 解放を行う
		}
	}

	// 構造体をゼロ初期化 //
	ZEROMEM(m_CacheInfo);
}


// キャッシュされていれば、ポインタを返す(ファイル番号) //
LPVOID CPbgCache::Search(char *pFileName, DWORD FileNo, DWORD *pFileSize)
{
	int			i;

	// サイズをゼロ初期化しておく //
	*pFileSize = 0;

	for(i=0; i<PBGCACHE_MAX; i++){
		// ここにはデータが格納されていない //
		if(NULL == m_CacheInfo[i].m_pData) continue;

		// ファイルＩＤとファイル名が一致したら、成功 //
		if(FileNo == m_CacheInfo[i].m_FileNo
		&& 0 == strcmp(pFileName, m_CacheInfo[i].m_FileName)){

			*pFileSize = m_CacheInfo[i].m_Size;	// サイズ格納
			return m_CacheInfo[i].m_pData;		// ポインタ返し
		}
	}

	return NULL;
}


// キャッシュされていれば、ポインタを返す(ファイルＩＤ) //
LPVOID CPbgCache::Search(char *pFileName, char *pFileID, DWORD *pFileSize)
{
	int			i;

	// サイズをゼロ初期化しておく //
	*pFileSize = 0;

	for(i=0; i<PBGCACHE_MAX; i++){
		// ここにはデータが格納されていない //
		if(NULL == m_CacheInfo[i].m_pData) continue;

		// ファイルＩＤとファイル名が一致したら、成功 //
		if(0 == strcmp(pFileID, m_CacheInfo[i].m_pFileID)
		&& 0 == strcmp(pFileName, m_CacheInfo[i].m_FileName)){
			*pFileSize = m_CacheInfo[i].m_Size;	// サイズ格納
			return m_CacheInfo[i].m_pData;		// ポインタ返し
		}
	}

	return NULL;
}


// キャッシュをかける(FileNo) //
FBOOL CPbgCache::SetCache(char *pFileName, DWORD FileNo)
{
	LzDecode	Decode;		// 圧ファイルデコーダ
	LPVOID		p;			// 一時解凍先
	DWORD		Size;		// ファイルサイズ
	int			n;			// 格納先のインデックス値

	n = SearchFreeIndex();					// 空いている格納先を探す
	if(n >= PBGCACHE_MAX) return FALSE;

	if(Search(pFileName, FileNo, &Size)) return FALSE;	// すでにキャッシュ済
	if(FALSE == Decode.Open(pFileName)) return FALSE;	// ファイルが開けない

	p = Decode.Decode(FileNo);		// 展開先データのデコード
	if(NULL == p) return FALSE;		// そんなファイルはありません

	strcpy(m_CacheInfo[n].m_FileName, pFileName);				// ファイル名
	strcpy(m_CacheInfo[n].m_pFileID, Decode.GetFileID(FileNo));	// ファイル識別名

	m_CacheInfo[n].m_pData  = p;							// 実際のデータ
	m_CacheInfo[n].m_FileNo = FileNo;						// ファイル番号
	m_CacheInfo[n].m_Size   = Decode.GetFileSize(FileNo);	// ファイルサイズ

	// 正常終了 //
	return TRUE;
}


// キャッシュをかける(FileID) //
FBOOL CPbgCache::SetCache(char *pFileName, char *pFileID)
{
	LzDecode	Decode;		// 圧ファイルデコーダ
	LPVOID		p;			// 一時解凍先
	DWORD		Size;		// ファイルサイズ
	int			n;			// 格納先のインデックス値

	n = SearchFreeIndex();					// 空きを探す
	if(n >= PBGCACHE_MAX) return FALSE;		// 空きが見つからない

	if(Search(pFileName, pFileID, &Size)) return FALSE;	// すでにキャッシュ済
	if(FALSE == Decode.Open(pFileName)) return FALSE;	// ファイルが開けない

	p = Decode.Decode(pFileID);		// ファイル識別名による解凍
	if(NULL == p) return FALSE;		// 解凍できません

	strcpy(m_CacheInfo[n].m_FileName, pFileName);	// ファイル名をコピー
	strcpy(m_CacheInfo[n].m_pFileID, pFileID);		// ファイル識別名をコピー

	m_CacheInfo[n].m_pData  = p;							// 実際のデータ
	m_CacheInfo[n].m_FileNo = Decode.GetFileNo(pFileID);	// ファイル番号
	m_CacheInfo[n].m_Size   = Decode.GetFileSize(pFileID);	// ファイルサイズ

	// 成功 //
	return TRUE;
}



#ifdef PBG_DEBUG
// キャッシュ情報を出力する //
FVOID CPbgCache::OutputCacheInfo(void)
{
	char			buf[1024] = "[ キャッシュ情報 ]\r\n";
	char			temp[1024];
	int				i, n;
	DWORD			sum = 0;
	int				r1, r2;

	for(i=n=0; i<PBGCACHE_MAX; i++){
		if(m_CacheInfo[i].m_pData){
			wsprintf(temp, "\t%02d : %-12s[%20s]  %6u Bytes\r\n", n
						, m_CacheInfo[i].m_FileName, m_CacheInfo[i].m_pFileID
						, m_CacheInfo[i].m_Size);
			strcat(buf, temp);
			n++;

			sum += m_CacheInfo[i].m_Size;
		}

		if(n && 0==(n%10)){
			PbgLog(buf);
			strcpy(buf, "");
		}
	}

	if(n != i){
		PbgLog(buf);
		strcpy(buf, "");
	}

	r1 = sum >> 20;
	r2 = 100 * ((sum - (r1 << 20)) >> 10) / 1024;
	wsprintf(buf, "\r\n合計 %u.%u MB (%u Bytes) キャッシュしています\r\n", r1, r2, sum);
	PbgLog(buf);
}
#endif


// 現在使用されていないインデックスを探す(失敗時は PBGCACHE_MAX を返す) //
FDWORD CPbgCache::SearchFreeIndex(void)
{
	DWORD		i;

	for(i=0; i<PBGCACHE_MAX; i++){
		if(NULL == m_CacheInfo[i].m_pData){		// 空きが見つかったら
			return i;							// そのインデックス値を返す
		}
	}

	return PBGCACHE_MAX;
}



} // namespace Pbg
