/*
 *   CWaterEfc.cpp   : 水系エフェクト
 *
 */

#include "CWaterEfc.h"
#include "CTransTable.h"



namespace Pbg {



// コンストラクタ //
CWaterEfc::CWaterEfc()
{
	m_Stage     = 0;	// どのテーブルを使用するか
	m_bFinished = TRUE;	// エフェクトが完了していれば真
}


// デストラクタ //
CWaterEfc::~CWaterEfc()
{
	// 何もしません //
}


// 作成する //
FBOOL CWaterEfc::Create(int Width, int Height)
{
	// テーブルの作成を行う //
	if(FALSE == m_Table[0].Create(Width, Height)) return FALSE;
	if(FALSE == m_Table[1].Create(Width, Height)) return FALSE;

	m_Table[0].Clear(0);	// テーブル０をゼロ初期化
	m_Table[1].Clear(0);	// テーブル１をゼロ初期化

	m_bFinished = TRUE;		// 初期状態ではエフェクトは完了している

	m_Stage  = 0;

	return TRUE;
}


// １フレーム進める //
FVOID CWaterEfc::Proceed(void)
{
	CTransTable8	*pSrc, *pDest;
	int				w, h, x, y;
	int				Level;
	BYTE			*pData;
	BOOL			bFinished = TRUE;

	pSrc  = &m_Table[m_Stage];		// 前回のデータ
	pDest = &m_Table[1 - m_Stage];	// 今回のデータ

	w = pSrc->GetWidth();		// 幅
	h = pSrc->GetHeight();		// 高さ


	// 外側１ピクセルを除く全ての点に対して //
	for(y=1; y<h-1; y++){
		for(x=1; x<w-1; x++){
			pData = pSrc->GetPtrFromY(y-1) + (x-1);
			if(*pData) bFinished = FALSE;

			// 現在位置以外の値を加算します                 //
			Level  = pData[0] + pData[1] + pData[2];	pData += w;
			Level += pData[0]            + pData[2];	pData += w;
			Level += pData[0] + pData[1] + pData[2];
			Level >>= 2;		// ４で割る

			// 基本的に前回の値を使用するのだが //
			// 左上の点を指していることに注意する //
			// さらに pData を再利用している点にも注意  //
			pData = pDest->GetPtrFromY(y) + x;
			Level -= pData[0];
			//Level -= (Level >> 1);

			// Level が [0, 255] の範囲内に収まるように飽和する //
			// つまり１バイトで表現出来るようにする //
			if(Level < 0){
				*pData = 0;		// 最低値０
			}
			else{
				if(Level > 255) *pData = 255;		// 最大値２５５
				else            *pData = Level;		// 範囲内
			}
		}
	}

	m_Stage     = 1 - m_Stage;	// 切り替える
	m_bFinished = bFinished;	// 終了したら真
}


// 雨粒を降らせる //
FVOID CWaterEfc::SetRain(int x, int y, BYTE Power)
{
	*(m_Table[m_Stage].GetPtrFromY(y) + x) = Power;

	m_bFinished = FALSE;
}



}
