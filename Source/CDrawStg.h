/*
 *   CDrawStg.h   : ステージ数表示
 *
 */

#ifndef CDRAWSTG_INCLUDED
#define CDRAWSTG_INCLUDED "ステージ数表示 : Version 0.01 : Update 2001/07/04"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/04 : 制作開始
 */



#include "PbgType.h"
#include "CFixedLList.h"
#include "DirectX\\DxUtil.h"



/***** [ 定数 ] *****/
#define PSTG_TASK_MAX		16



/***** [クラス定義] *****/

// ステージ数表示タスク //
typedef struct tagPStgTask {
	D3DRECTANGLE	m_D3DRect;		// テクスチャの対象となる矩形
	int				m_ox;			// 中心のＸ座標(相対)
	int				m_oy;			// 中心のＹ座標(相対)
	int				m_Width;		// テクスチャの元の幅
	int				m_Height;		// テクスチャの元の高さ
	int				m_XScale;		//  幅 方向の倍率(x256)
	int				m_YScale;		// 高さ方向の倍率(x256)
	int				m_Shadow;		// 影?との距離

	DWORD			m_Count;		// カウンタ
	BYTE			m_Cmd;			// 動作タイプ
	BYTE			m_Angle;		// 回転角

	// テクスチャセット用の関数 //
	FVOID DefTexture(int x1, int y1, int x2, int y2){
		// テクスチャ座標 //
		m_D3DRect.Set(x1, y1, x2, y2, 256);

		// x256 に変換して、２で割った値 //
		m_Width  = (x2 - x1) * 128;	// 幅
		m_Height = (y2 - y1) * 128;	// 高さ
	};
} PStgTask;


// ステージ数表示用クラス //
class CDrawStg : public CFixedLList<PStgTask, 3, PSTG_TASK_MAX> {
public:
	FBOOL Initialize(int nStage, BOOL Is2P);	// 表示の準備を行う
	FVOID Move(void);							// 動作させる
	FVOID Draw(void);							// 描画する

	FBOOL IsActive(void){ return m_IsActive; };	// アクティブなら真を返す

	CDrawStg(RECT *rcTargetX256);	// コンストラクタ
	~CDrawStg();					// デストラクタ


private:
	FBOOL MovePStgTask(PStgTask *pTask);	// タスクごとの動作(偽なら削除)
	FVOID DrawPStgTask(PStgTask *pTask);	// 各タスクごとの描画を行う

	// タスクを追加＆一部初期化 //
	PStgTask *AddTask(int x, int y, BYTE Cmd, DWORD Count);

	// 音声タスクを追加 //
	BOOL AddVoiceTask(BYTE ID, DWORD Count);

	int		m_ox;		// 左上のＸ座標
	int		m_oy;		// 左上のＹ座標

	DWORD	m_Count;	// カウンタ
	BOOL	m_IsActive;	// アクティブなら真
};



#endif
