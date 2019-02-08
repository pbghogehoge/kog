/*
 *   CAtkGrpDraw.cpp   : 攻撃送り描画
 *
 */

#include "CAtkGrpDraw.h"
#include "SCL.h"



// コンストラクタ //
CAtkGrpDraw::CAtkGrpDraw(RECT *rcTargetX256, SCENE_ID SceneID)
{
	m_ox = (rcTargetX256->right  + rcTargetX256->left) / 2;
	m_oy = (rcTargetX256->bottom + rcTargetX256->top)  / 2;

	m_pAnimeDef = NewEx(CAnimeDef(SceneID));
	m_SceneID   = SceneID;
}


// デストラクタ //
CAtkGrpDraw::~CAtkGrpDraw()
{
	DeleteEx(m_pAnimeDef);
}


// 初期化する                                      //
// arg : CharID     このプレイヤーのキャラクタＩＤ //
//     : pRivalAnm  相手プレイヤーのアニメ定義     //
FBOOL CAtkGrpDraw::Initialize(int CharID, CAtkGrpDraw *pRivalAnm, BOOL Is2PColor)
{
	// リスト内のデータを解放する //
	DeleteAllData();

	if(NULL == m_pAnimeDef) return FALSE;
	if(NULL == pRivalAnm)   return FALSE;

	// 相手側のアニメーション定義を取得する //
	m_pRivalDef = pRivalAnm->m_pAnimeDef;
	if(NULL == m_pRivalDef) return FALSE;

	return m_pAnimeDef->Load(CharID, Is2PColor);

/*
	if(SCENE_PLAYER_1 == m_SceneID){
		return m_pAnimeDef->Load(CharID, FALSE);
	}
	else{
		return m_pAnimeDef->Load(CharID, TRUE);
	}
*/
}


// １フレーム動作させる //
FVOID CAtkGrpDraw::Move(void)
{
	Iterator		it;

	// 通常状態 //
	ThisForEachFront(AGTASK_NORMAL, it){
		if(FALSE == it->Move(this)){
			ThisDelContinue(it);
		}
	}

	// 消去状態 //
	ThisForEachFront(AGTASK_DELETE, it){
		if(FALSE == it->MoveDelete()){
			ThisDelContinue(it);
		}
	}
}


// 描画を行う //
FVOID CAtkGrpDraw::Draw(void)
{
	Iterator			it;
	int					ox, oy;
	RECT				rcOrg;

	ox = m_ox;
	oy = m_oy;

	// まず、元の描画矩形を取得する //
	g_pGrp->GetViewport(&rcOrg);

	// 参考：通常状態のグラフィックが上に描画されるようにすること //

	// 消去状態 //
	ThisForEachFront(AGTASK_DELETE, it){
		it->Draw(ox, oy);
	}

	// 通常状態 //
	ThisForEachFront(AGTASK_NORMAL, it){
		it->Draw(ox, oy);
	}

	// 元の描画矩形に戻す //
	g_pGrp->SetViewport(&rcOrg);
}


// 親タスクを生成(以前のタスクにはフェードをかける) //
FBOOL CAtkGrpDraw::Set(BYTE Level)
{
	BYTE		*pAddr;
	CAnimeDef	*pAnimeDef;

	// まず、今現在生きているタスクが存在したら、消去状態へと移行する //
	MoveBackAll(AGTASK_DELETE, AGTASK_NORMAL);

	switch(Level){
		case AGD_LV1:	// レベル１攻撃
		case AGD_LV2:	// レベル２攻撃
		case AGD_BOSS:	// ボスアタック
			pAnimeDef = m_pRivalDef;
		break;

		case AGD_WON:	// 勝ち
			pAnimeDef = m_pAnimeDef;
		break;

		default:
		return FALSE;
	}

	// アニメ定義のエントリポイントを取得する //
	pAddr = CSCLDecoder::GetExAnmAddr(pAnimeDef->GetCharID(), Level);
	if(NULL == pAddr) return FALSE;

	// タスクをセットする //
	return SetTask(pAddr, pAnimeDef, NULL);
}


// アニメーションタスクを追加 //
FBOOL CAtkGrpDraw::SetTask(BYTE *pAddr, CAnimeDef *pAnimeDef, CAnimeTask *pParent)
{
	CAnimeTask		*pTask;

	pTask = InsertBack(AGTASK_NORMAL);		// 挿入して、ポインタを取得
	if(NULL == pTask) return FALSE;			// もう、空きがありません

	return pTask->Initialize(pAddr, pAnimeDef, pParent);
}
