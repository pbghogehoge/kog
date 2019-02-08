/*
 *   CBGDoor.h   : 背景窓の描画
 *
 */

#ifndef CBGDOOR_INCLUDED
#define CBGDOOR_INCLUDED "背景窓の描画 : Version 0.01 : Update 2001/09/06"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/06 : 制作開始
 */



#include "Gian2001.h"



/***** [クラス定義] *****/

class CBGDoor {
public:
	FVOID Set(BOOL IsOpenMode);		// セットする
	FVOID Move(void);				// １フレーム動作させる
	FVOID Draw(int SyncDx);			// 描画する(２Ｄモードでコールすべし)
	FBOOL IsClosed(void);			// 扉が閉じられていれば真を返す
	FBOOL IsStopped(void);			// 停止中なら真を返す

	CBGDoor(RECT *rcTargetX256);	// コンストラクタ
	~CBGDoor();						// デストラクタ


private:
	FVOID InitializeStaticMember(void);	// 静的メンバを初期化する
	FVOID CleanupStaticMember(void);	// 静的メンバを解放する

	int		m_XMin;			// Ｘ座標の左端(x256)
	int		m_XMid;			// Ｘ座標の中心(x256)
	int		m_YMid;			// Ｙ座標の中心(x256)
	int		m_DeltaX;		// 上の扉がＸ軸方向にどれだけ離れているか(x256)
	int		m_DeltaY;		// 下の扉がＹ軸方向にどれだけ離れているか(x256)
	DWORD	m_Count;		// カウンタ(０ならば停止状態)
	BOOL	m_IsOpenMode;	// 真ならば扉を開き、偽ならば扉を閉じる

	static int					m_RefCount;		// 参照カウンタ
	static Pbg::CGrpSurface		*m_pDoorL1;		// 上の扉
	static Pbg::CGrpSurface		*m_pDoorL2;		// 下の扉
};



#endif
