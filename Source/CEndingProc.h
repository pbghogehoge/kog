/*
 *   CEndingProc.h   : エンディング処理
 *
 */

#ifndef CENDINGPROC_INCLUDED
#define CENDINGPROC_INCLUDED "エンディング処理 : Version 0.01 : Update 2001/08/06"

/*  [更新履歴]
 *    Version 0.01 : 2001/08/06 : 制作開始
 */



#include "Gian2001.h"
#include "CScene.h"



/***** [クラス定義] *****/

class CEndingProc : public CScene {
public:
	IBOOL      Initialize(void);	// 初期化をする
	PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	IVOID      Draw(void);			// 描画する

	CEndingProc(CGameInfo *pSys);	// コンストラクタ
	~CEndingProc();					// デストラクタ


private:
	Pbg::CDIB32Effect	m_LogoDIB;	// 描画用ＤＩＢ
	DWORD				m_Count;	// カウンタ
	BYTE				m_State;	// 状態
};



#endif
