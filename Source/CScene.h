/*
 *   CScene.h   : シーン定義・基底
 *
 */

#ifndef CSCENE_INCLUDED
#define CSCENE_INCLUDED "シーン定義・基底 : Version 0.01 : Update 2001/05/22"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/22 : 製作開始
 */



#include "PbgMain.h"
#include "PbgProcID.h"



/***** [クラス定義] *****/

// システム情報・管理クラス //
class CGameInfo;


// シーン定義・基底 //
class CScene {
public:
	virtual IBOOL      Initialize(void) = 0;	// 初期化をする
	virtual PBG_PROCID Move(void)       = 0;	// １フレーム分だけ動作させる
	virtual IVOID      Draw(void)       = 0;	// 描画する

	// コンストラクタ //
	CScene(CGameInfo *pSys) : m_pSys(pSys) {};

	// デストラクタ //
	virtual ~CScene(){};


protected:
	CGameInfo	* const m_pSys;		// システム情報...
};



#endif
