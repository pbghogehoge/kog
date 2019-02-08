/*
 *   CDirectShow.h   : DirectShow によるデコード
 *
 */

#ifndef CDIRECTSHOW_INCLUDED
#define CDIRECTSHOW_INCLUDED "DirectShow によるデコード : Version 0.02 : Update 2001/09/31"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/31 : 微調整
 *    Version 0.01 : 2001/08/25 : 制作開始
 */

#include "WaveFileBase.h"
#include <amstream.h>


#pragma comment(lib, "AMSTRMID.lib")


namespace PbgEx {



/***** [クラス定義] *****/

class CDirectShow : public WaveFileBase {
public:
	BOOL Open(char *pFileName);			// ファイルをオープンする
	BOOL Close(void);					// ファイルを閉じる
	BOOL Reset(void);					// ファイル先頭に戻る
	UINT Read(BYTE *pData, UINT Size);	// ファイルを読み込む(Ret:読み込みSize)
	BOOL Seek(UINT Offset);				// ファイル内の移動(データ部における)
	BOOL SetEnd(UINT Offset);			// 終端をセットする

	WAVEFORMATEX	*GetFormat(void);	// フォーマットを取得する
	DWORD			GetSize(void);		// バッファのサイズを取得

	CDirectShow();						// コンストラクタ
	~CDirectShow();						// デストラクタ


private:	// -- 関数
	FBOOL CreateMMStream(void);		// MMStream の作成
	FVOID Cleanup(void);


private:	// -- 変数
	WAVEFORMATEX			m_DestFormat;	// 出力フォーマット

	UINT					m_Size;			// ファイルサイズ
	UINT					m_Current;		// 現在位置
	UINT					m_End;			// ループ終端位置

	// DirectShow インターフェース周辺 //
	IAMMultiMediaStream		*m_pAMStream;		// マルチメディアストリーム
	IMultiMediaStream		*m_pMMStream;		// MMStream のコントロール
	IMediaStream			*m_pStream;			// メディアストリーム
	IAudioStreamSample		*m_pSample;			// AudioData からのデータ取得
	IAudioMediaStream		*m_pAudioStream;	// 波フォーマット取得用(出力)
	IAudioData				*m_pAudioData;		// 波フォーマットセット用(入力)
	IGraphBuilder			*m_pGraphBuilder;
};



} // namespace PbgEx



#endif
