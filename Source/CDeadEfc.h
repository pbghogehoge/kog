/*
 *   CDeadEfc.h   : 死亡エフェクト描画
 *
 */

#ifndef CDEADEFC_INCLUDED
#define CDEADEFC_INCLUDED "死亡エフェクト描画 : Version 0.01 : Update 2001/07/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/27 : 製作開始
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define DEFC_MAX		70		// 死亡エフェクトタスク最大数
#define DEFC_KIND		2		// エフェクト状態の数
#define DEFCTASK_WAIT	0x00	// 待ち状態
#define DEFCTASK_NORM	0x01	// 通常状態



/***** [クラス定義] *****/

// 死亡エフェクト用タスク //
typedef struct tagDEfcTask {
	int		x, y;		// エフェクトの中心
	int		Length;		// 長さパラメータ(ラインエフェクト用)
	DWORD	Count;		// エフェクトカウンタ

	BYTE	Type;		// エフェクトの種類
	BYTE	Angle;		// 角度
	char	Delta;		// 角度差分
	BYTE	Alpha;		// α値
} DEfcTask;


// 死亡時エフェクト描画 //
class CDeadEfc  : public CFixedLList<DEfcTask, DEFC_KIND, DEFC_MAX> {
public:
	FVOID Initialize(void);		// エフェクトの除去
	FVOID Set(int x, int y);	// エフェクトをセットする
	FVOID Move(void);			// エフェクトを１フレーム動作させる
	FVOID Draw(void);			// エフェクトを描画する

	CDeadEfc();		// コンストラクタ
	~CDeadEfc();	// デストラクタ

private:
	static FBOOL MoveCircle(DEfcTask *pTask);	// 円状態の動作
	static FBOOL MoveLine(DEfcTask *pTask);		// ライン状態の動作
	static FBOOL MoveBomb(DEfcTask *pTask);		// 爆発状態の動作

	static FVOID DrawCircle(DEfcTask *pTask);	// 円状態の描画
	static FVOID DrawLine(DEfcTask *pTask);		// ライン状態の描画
	static FVOID DrawBomb(DEfcTask *pTask);		// 爆発状態の描画
};



#endif
