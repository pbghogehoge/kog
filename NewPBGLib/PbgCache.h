/*
 *   PbgCache.h   : 圧ファイルのキャッシュ
 *
 */

#ifndef PBGCACHE_INCLUDED
#define PBGCACHE_INCLUDED "圧ファイルのキャッシュ : Version 0.03 : Update 2001/10/08"

/*  [更新履歴]
 *    Version 0.03 : 2001/10/08 : CGrpLoader と結合するため、コンストラクタと
 *                              : デストラクタを protected とする
 *
 *    Version 0.02 : 2001/09/10 : 注釈
 *
 *    Version 0.01 : 2001/07/21 : 製作開始
 *                              : このクラスを生成するとキャッシュが有効になる
 *                              : 参照カウントが０になるとキャッシュが無効になる
 */



#include "PbgType.h"



namespace Pbg {



/***** [ 定数 ] *****/
#define PBGCACHE_MAX	80		// キャッシュの最大幅



/***** [クラス定義] *****/

// キャッシュ情報構造体 //
typedef struct tagCacheInfo {
	LPVOID		m_pData;				// データ格納先へのポインタ
	char		m_FileName[MAX_PATH];	// ファイル名
	char		m_pFileID[MAX_PATH];	// ファイル識別子
	DWORD		m_FileNo;				// ファイル番号
	DWORD		m_Size;					// ファイルサイズ
} CacheInfo;


// 圧ファイルのキャッシュ //
class CPbgCache {
public:
	// キャッシュをかける //
	FBOOL SetCache(char *pFileName, DWORD FileNo);
	FBOOL SetCache(char *pFileName, char *pFileID);

	// 指定されたファイルがキャッシュされていれば、ポインタを返す //
	static LPVOID Search(char *pFileName, DWORD FileNo, DWORD *pFileSize);
	static LPVOID Search(char *pFileName, char *pFileID, DWORD *pFileSize);


#ifdef PBG_DEBUG
	static FVOID OutputCacheInfo(void);		// キャッシュ情報を出力する
#endif


protected:
	CPbgCache();	// コンストラクタ
	~CPbgCache();	// デストラクタ


private:
	// 現在使用されていないインデックスを探す(失敗時は PBGCACHE_MAX を返す) //
	static FDWORD SearchFreeIndex(void);

	static CacheInfo	m_CacheInfo[PBGCACHE_MAX];	// キャッシュ情報
	static int			m_RefCount;					// 参照カウント
};



} // namespace Pbg



#endif
