/*
 *   CLoadingGrp.h   : 初期化画面管理
 *
 */

#ifndef CLOADINGGRP_INCLUDED
#define CLOADINGGRP_INCLUDED "初期化画面管理 : Version 0.01 : Update 2001/09/10"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/10 : 制作開始
 */

#include "PbgMain.h"



/***** [クラス定義] *****/

// 初期化画面管理 //
class CLoadingGrp : public Pbg::CCriticalSection {
public:
	FBOOL Initialize(int Width, int Height, DWORD NumExec);	// 初期化する
	FVOID OnDraw(HDC hdc);									// 全体を再描画する
	FVOID Update(HDC hdc, BOOL bWantFinish);				// １フレーム更新する
	FVOID SetProgress(int Progress);						// 進行状況をセットする(0..10)
	FBOOL IsEfcFinished(void);								// 雨エフェクトが完了すれば真

	CLoadingGrp();		// コンストラクタ
	~CLoadingGrp();		// デストラクタ


private:
	int				m_wx, m_wy;		// 壁紙を表示する座標
	BYTE			m_Count;		// アニメーション用カウンタ

	Pbg::CDIB32Effect	m_Back;			// 画像格納先(いわゆる裏画面)
	Pbg::CFont			m_Progress;		// 進行状況
	Pbg::CDIB32			m_WallPaper;	// 壁紙画像

	Pbg::CWaterEfc	m_Water;		// 雫エフェクト
	Pbg::CRnd		m_Rnd;			// 乱数発生
};



#endif
