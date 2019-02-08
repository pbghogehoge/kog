/*
 *   CPUCtrl.h   : ＣＰＵの動作
 *
 */

#ifndef CPUCTRL_INCLUDED
#define CPUCTRL_INCLUDED "ＣＰＵの動作 : Version 0.01 : Update 2001/05/08"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/08 : 製作開始(間に合うのか！？)
 */



#include "Gian2001.h"
#include "CPlayerCtrl.h"
#include "CEnemyCtrl.h"
#include "CEnemyTama.h"
#include "CEnemyLLaser.h"
#include "CEnemyHLaser.h"



/***** [ 定数 ] *****/

// ＣＰＵレベル定数 //
typedef enum {
	CPU_EASY    = 0,	// 基本レベル [ Easy ]
	CPU_NORMAL  = 1,	// 基本レベル [ Normal ]
	CPU_HARD    = 2,	// 基本レベル [ Hard ]
	CPU_LUNATIC = 3,	// 基本レベル [ Lunatic ]
	CPU_EXTRA   = 4,	// 基本レベル [ Extra ]
} CPULEVEL;

// 溜めレベル定数 //
typedef enum {
	CHG_LVHALF  = 0,	// レベル 1/2
	CHG_LV1     = 1,	// レベル １
	CHG_LV2     = 2,	// レベル ２
	CHG_LV3     = 3,	// レベル ３
	CHG_LV4     = 4,	// レベル ４
	CHG_WAIT    = 5,	// 溜め撃ち発射まち
} CHGLEVEL;

// 移動ルーチン定数 //
typedef enum {
	CPUMOVE_TARGET    = 0,		// 目標に向かって移動
	CPUMOVE_STOP      = 1,		// 現在位置で停止
	CPUMOVE_WAITENEMY = 2,		// 敵の発生を待つ
} MOVE_ROUTINE;



/***** [クラス定義] *****/

// ＣＰＵ管理クラス定数 //
class CCPUCtrl {
protected:
	FVOID CPUInitialize(CPULEVEL GameLevel);	// レベルの初期化を行う
	FVOID CPULevelUP(int DeltaLevel);			// レベルを変更する
	FWORD CPUGetKeyCode(void);					// キーコードを取得する

#ifdef PBG_DEBUG
	// 探索範囲の取得 //
	FINT CPUGetCheckRange(void){
		return m_CheckRange;
	};
#endif

	CCPUCtrl();		// コンストラクタ
	~CCPUCtrl();	// デストラクタ


	// 以下のオブジェクトの確保＆解放の責任はこのクラスには無い //
	CPlayerCtrl		*m_pPlayerCtrl;		// プレイヤー情報
	CEnemyCtrl		*m_pEnemyCtrl;		// 敵コントロール
//	CEnemyTama		*m_pEnemyTama;		// 敵弾コントロール
//	CEnemySLaser	*m_pEnemySLaser;	// ショートレーザー
//	CEnemyLLaser	*m_pEnemyLLaser;	// 太いレーザー
//	CEnemyHLaser	*m_pEnemyHLaser;	// ホーミングレーザー


private:
	FVOID SelectChargeLevel(void);	// 溜めレベルをセットする

	// 目標座標への移動 //
	IWORD MoveToTarget(int mx, int my, int tx, int ty);

	// 新しい目標を決める //
	IVOID SelectNewTarget(int mx, int my);

	// 敵弾を避ける方向にキーコードを変換する(ret : 変換後のキーコード) //
	IWORD ConvertEvadeKeyCode(int mx, int my, WORD KeyCode);


	CPULEVEL	m_GameLevel;		// 難易度による基本レベル
	CHGLEVEL	m_ChargeLevel;		// 次に発動しようとしている溜めレベル
	DWORD		m_CurrentLevel;		// 現在のＣＰＵ思考ルーチンレベル

	MOVE_ROUTINE	m_Move;			// 移動ルーチン
	DWORD			m_WaitCount;	// 停止要求の待ち時間
	int				m_TargetX;		// 目標とするＸ座標
	int				m_TargetY;		// 目標とするＹ座標
	int				m_CheckRange;	// 探索範囲
};



#endif
