/*
 *   CAtkGrpDraw.h   : 攻撃送り描画
 *
 */

#ifndef CATKGRPDRAW_INCLUDED
#define CATKGRPDRAW_INCLUDED "攻撃送り描画 : Version 0.02 : Update 2001/09/25"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/25 : 大幅に変更
 *    Version 0.01 : 2001/07/28 : 製作開始(〆切間近)
 */



#include "Gian2001.h"
#include "ExAnm.h"



/***** [ 定数 ] *****/
#define AGTASK_MAX		30		// 同時発生可能なタスク数
#define AGTASK_KIND		2		// タスクの種類

#define AGTASK_NORMAL	0x00	// 通常状態(完全にスクリプトに従う)
#define AGTASK_DELETE	0x01	// 消去状態(勝手にフェードがかかる)

#define AGD_LV1			0x01	// レベル１攻撃
#define AGD_LV2			0x02	// レベル２攻撃
#define AGD_BOSS		0x03	// ボスアタック
#define AGD_WON			0x04	// 勝ち



/***** [クラス定義] *****/

class CAtkGrpDraw : public CFixedLList<CAnimeTask, AGTASK_KIND, AGTASK_MAX> {
public:
	// 初期化する                                      //
	// arg : CharID     このプレイヤーのキャラクタＩＤ //
	//     : pRivalAnm  相手プレイヤーのアニメ定義     //
	FBOOL Initialize(int CharID, CAtkGrpDraw *pRivalAnm, BOOL Is2PColor);

	FVOID Move(void);		// １フレーム動作させる
	FVOID Draw(void);		// 描画を行う
	FBOOL Set(BYTE Level);	// 親タスクを生成(以前のタスクにはフェードをかける)

	// アニメーションタスクを追加 //
	FBOOL SetTask(BYTE *pAddr, CAnimeDef *pAnimeDef, CAnimeTask *pParent);

	CAtkGrpDraw(RECT *rcTargetX256, SCENE_ID SceneID);	// コンストラクタ
	~CAtkGrpDraw();										// デストラクタ


private:
	int			m_ox;			// 画面中心のＸ座標
	int			m_oy;			// 画面上端のＹ座標

	CAnimeDef	*m_pAnimeDef;	// アニメーション定義
	CAnimeDef	*m_pRivalDef;	// 相手側のアニメーション定義

	SCENE_ID	m_SceneID;		// どちらの画面か
};



#endif
