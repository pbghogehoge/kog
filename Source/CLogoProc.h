/*
 *   CLogoProc.h   : 西方ロゴの表示
 *
 */

#ifndef CLOGOPROC_INCLUDED
#define CLOGOPROC_INCLUDED "西方ロゴの表示 : Version 0.01 : Update 2001/07/09"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/09 : 製作開始
 */



#include "Gian2001.h"
#include "CScene.h"



/***** [クラス定義] *****/
class CLogoProc : public CScene {
public:
	IBOOL      Initialize(void);	// 初期化をする
	PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	IVOID      Draw(void);			// 描画する

	CLogoProc(CGameInfo *pSys);	// コンストラクタ
	~CLogoProc();				// デストラクタ


private:
	Pbg::CDIB32Effect	m_LogoDIB;		// 描画用ＤＩＢ
	DWORD				m_Count;		// エフェクトカウンタ
	BOOL				m_FinishFlag;	// 強制抜けようフラグ
};



#endif
