/*
 *   CWaveRead.h   : WAVE 読み込み
 *
 */

#ifndef CWAVEREAD_INCLUDED
#define CWAVEREAD_INCLUDED "WAVE 読み込み : Version 0.02 : Update 2001/01/24"

/*  [更新履歴]
 *    Version 0.03 : 2001/01/24 : Seek & SetEnd の追加(ループ再生用)
 *                              : 仕様変更につき、乱れが生じているので修正のこと
 *    Version 0.02 : 2001/01/21 : 一通り完成
 *    Version 0.01 : 2001/01/20 : 参考資料(Dx7-SDK WavRead.h)
 */



#include "WaveFileBase.h"



namespace PbgEx {



/***** [クラス定義] *****/
class CWaveFileRead : public WaveFileBase {
public:
	BOOL Open(char *FileName);						// ファイルをオープンする
	BOOL OpenP(char *pPackFileName, char *pFileID);	// 圧ファイルをオープンする
	BOOL OpenP(char *pPackFileName, DWORD FileNo);	// 圧ファイルをオープンする

	BOOL Reset(void);					// ファイル先頭に戻る
	UINT Read(BYTE *pData, UINT Size);	// ファイルを読み込む(Ret:読み込みSize)
	BOOL Seek(UINT Offset);				// ファイル内の移動(データ部における)
	BOOL SetEnd(UINT Offset);			// 終端をセットする
	BOOL Close(void);					// ファイルを閉じる

	WAVEFORMATEX	*GetFormat(void);	// フォーマットを取得する
	DWORD			GetSize(void);		// バッファのサイズを取得

	CWaveFileRead();		// コンストラクタ
	~CWaveFileRead();		// デストラクタ


private:
	BOOL WaveOpenFile(char *FileName);					// ファイルを開く(低レベル...)
	BOOL WaveOpenMemory(LPVOID lpData, DWORD dwSize);	// メモリ上のイメージを開く
	BOOL ReadMMIO(void);								// ヘッダを調べる


private:
	WAVEFORMATEX	*m_pwfx;	// WAVEFORMATEX 構造体へのポインタ
	HMMIO			m_hmmioIn;	// マルチメディア I/O ハンドル(for WAVE)
	MMCKINFO		m_ckIn;		// マルチメディア RIFF チャンク
	MMCKINFO		m_ckInRiff;	// WAVE ファイルを開くのに使用

	DWORD			m_Size;		// ファイルのデータ部のサイズ
	UINT			m_End;		// 新たに設定された終端(ファイル終端からのバイト数)

	LPVOID			m_pMemoryData;	// メモリ上で展開した場合
};



} // namespace PbgEx



#endif
