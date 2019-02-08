/*
 *   CDrawWonLost.h   : 勝敗表示
 *
 */

#ifndef CDRAWWONLOST_INCLUDED
#define CDRAWWONLOST_INCLUDED "勝敗表示     : Version 0.01 : Update 2001/07/23"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/23 : 制作開始
 */



#include "PbgType.h"
#include "CFixedLList.h"
#include "DirectX\\DxUtil.h"



/***** [ 定数 ] *****/
#define DWL_WON		0x00		// 勝ち
#define DWL_LOST	0x01		// 負け
#define DWL_DRAW	0x02		// 引き分け



/***** [クラス定義] *****/

// 勝敗表示用タスク //
typedef struct tagWLTask {
	D3DRECTANGLE	m_D3DRect;		// テクスチャの対象となる矩形

	int		m_x;			// 現在のＸ座標
	int		m_y;			// 現在のＹ座標
	int		m_vy;			// 現在の速度(Ｙ軸方向)
	int		m_ymax;			// 跳ね返る地面のＹ座標

	int		m_Width;		// 画像の幅
	int		m_Height;		// 画像の高さ

	DWORD	m_Count;		// カウンタ
} WLTask;


// 勝敗表示用クラス //
class CDrawWonLost : public CFixedLList<WLTask, 1, 10> {
public:
	FVOID Initialize(void);		// 初期化する
	FVOID Set(BYTE State);		// セットする
	FVOID Move(void);			// １フレームだけ動作させる
	FVOID Draw(void);			// 描画する
	FBOOL IsFinished(void);		// エフェクトが完了していれば真を返す

	CDrawWonLost(RECT *rcTargetX256);	// コンストラクタ
	~CDrawWonLost();					// デストラクタ


private:
	// タスクを追加する                                  //
	// 引数    x, y : 原点からの相対座標(非x256)         //
	//         w, h : 画像の幅と高さ(非x256)             //
	//         ymax : いわゆる地面のｙ座標(相対, 非x256) //
	WLTask *AddTask(int x, int y, int w, int h, int ymax);

	int		m_ox;			// 中央のＸ座標
	int		m_oy;			// 上端のＹ座標

	BOOL	m_IsActive;		// アクティブなら真
};



#endif
