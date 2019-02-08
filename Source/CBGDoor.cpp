/*
 *   CBGDoor.cpp   : 背景窓の描画
 *
 */

#include "CBGDoor.h"
#include "SECtrl.h"



int					CBGDoor::m_RefCount = 0;	// 参照カウンタ
Pbg::CGrpSurface	*CBGDoor::m_pDoorL1 = NULL;	// 上の扉
Pbg::CGrpSurface	*CBGDoor::m_pDoorL2 = NULL;	// 下の扉



// コンストラクタ //
CBGDoor::CBGDoor(RECT *rcTargetX256)
{
	// 最初の呼び出しならば、データをロードする //
	if(0 == m_RefCount){
		InitializeStaticMember();
	}

	// 参照カウンタをインクリメントする //
	m_RefCount++;

	// Ｘ座標の左端(x256) //
	m_XMin = rcTargetX256->left;

	// Ｘ座標の中心(x256) //
	m_XMid = (rcTargetX256->right + rcTargetX256->left) / 2;

	// Ｙ座標の中心(x256) //
	m_YMid = (440 / 2) * 256;

	// カウンタ(０ならば停止状態) //
	m_Count = 0;
}


// デストラクタ //
CBGDoor::~CBGDoor()
{
	// 参照カウンタをデクリメントする //
	m_RefCount--;

	// 参照しているオブジェクトが無くなったら、データを解放する //
	if(0 == m_RefCount){
		CleanupStaticMember();
	}
}


// セットする //
FVOID CBGDoor::Set(BOOL IsOpenMode)
{
	// 真ならば扉を開き、偽ならば扉を閉じる //
	m_IsOpenMode = IsOpenMode;

	m_Count = 60;

	if(IsOpenMode){
		m_DeltaX = -23*256;	// 上の扉がＸ軸方向にどれだけ離れているか(x256)
		m_DeltaY = -80*256;	// 下の扉がＹ軸方向にどれだけ離れているか(x256)
	}
	else{
		m_DeltaX = 160 * 256;
		m_DeltaY = 260 * 256;
		PlaySE(SNDID_STG2_MOVE);
	}
}


// １フレーム動作させる //
FVOID CBGDoor::Move(void)
{
	int		Speed = (580 * 256) / 60;

	if(0 == m_Count) return;

	// 扉を開いている場合 //
	if(m_IsOpenMode){
		if(m_Count < 50) m_DeltaX += Speed/2;
		if(m_Count < 40) m_DeltaY += Speed;
	}
	// 扉を閉じている場合 //
	else{
		if(m_Count < 50) m_DeltaX = max(-23*256, m_DeltaX - Speed/2);
		m_DeltaY = max(-80*256, m_DeltaY - Speed);
		if(1 == m_Count) m_Count = 2;

//		m_DeltaX = -23*256;	// 上の扉がＸ軸方向にどれだけ離れているか(x256)
	}

	m_Count--;
}


// 描画する(２Ｄモードでコールすべし) //
FVOID CBGDoor::Draw(int SyncDx)
{
	RECT	src;
	int		sx, sy;

	if(0 == m_Count) return;
	if(NULL == m_pDoorL1) return;
	if(NULL == m_pDoorL2) return;

	// 左端の座標値 //
	sx = (m_XMin >> 8);

	// 下の扉・上 //
	SetRect(&src, 268, 0, 536, 308);
	sy = ((m_YMid - m_DeltaY) >> 8) - 308;
	g_pGrp->BltC(&src, sx+SyncDx, sy, m_pDoorL2);

	// 下の扉・下 //
	SetRect(&src, 0, 0, 268, 283);
	sy = ((m_YMid + m_DeltaY) >> 8)+40;
	g_pGrp->BltC(&src, sx+SyncDx, sy, m_pDoorL2);

	// 上の扉・左側 //
	SetRect(&src, 158, 0, 313, 440);
	sx = ((m_XMid - m_DeltaX) >> 8) - 155 - 2;
	g_pGrp->BltC(&src, sx+SyncDx, 0, m_pDoorL1);

	// 上の扉・右側 //
	SetRect(&src, 0, 0, 158, 440);
	sx = ((m_XMid + m_DeltaX) >> 8) - 2 + 1;
	g_pGrp->BltC(&src, sx+SyncDx, 0, m_pDoorL1);
}


// 扉が閉じられていれば真を返す //
FBOOL CBGDoor::IsClosed(void)
{
	// 終了カウントを示していて、かつクローズモードの場合 //
	if((16 > m_Count) && (!m_IsOpenMode)) return TRUE;
	else                                  return FALSE;
}


// 停止中なら真を返す //
FBOOL CBGDoor::IsStopped(void)
{
	if(0 == m_Count) return TRUE;
	else             return FALSE;
}


// 静的メンバを初期化する //
FVOID CBGDoor::InitializeStaticMember(void)
{
	const int L1Width  = 313;	// 上の扉の幅
	const int L1Height = 440;	// 上の扉の高さ
	const int L2Width  = 536;	// 下の扉の幅
	const int L2Height = 308;	// 下の扉の高さ

	// 上の扉をロードする //
	if(NULL == m_pDoorL1){
		if(g_pGrp->CreateSurface(&m_pDoorL1, L1Width, L1Height)){
			m_pDoorL1->LoadP("GRAPH.DAT", "GRP/扉01");
			m_pDoorL1->SetColorKey(0);
		}
	}

	// 下の扉をロードする //
	if(NULL == m_pDoorL2){
		if(g_pGrp->CreateSurface(&m_pDoorL2, L2Width, L2Height)){
			m_pDoorL2->LoadP("GRAPH.DAT", "GRP/扉02");
			m_pDoorL2->SetColorKey(0);
		}
	}
}


// 静的メンバを解放する //
FVOID CBGDoor::CleanupStaticMember(void)
{
	// 上の扉を開放する //
	if(m_pDoorL1){
		g_pGrp->ReleaseOffsSurface(m_pDoorL1);
	}

	// 下の扉を開放する //
	if(m_pDoorL2){
		g_pGrp->ReleaseOffsSurface(m_pDoorL2);
	}
}
