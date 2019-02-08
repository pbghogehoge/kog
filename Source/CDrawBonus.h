/*
 *   CDrawBonus.h   : ボーナスの表示
 *
 */

#ifndef CDRAWBONUS_INCLUDED
#define CDRAWBONUS_INCLUDED "ボーナスの表示 : Version 0.01 : Update 2001/07/10"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/10 : 製作開始
 */



#include "PbgType.h"
#include "DirectX\\DxUtil.h"



/***** [クラス定義] *****/

// ボーナス表示用クラス //
class CDrawBonus {
public:
	// 表示の準備を行う //
	FDWORD Set(DWORD MaxCombo		// 最大コンボ数
			 , DWORD ClearTime);	// クリア時の経過フレーム数

	FVOID Move(void);		// 動作させる
	FVOID Draw(void);		// 描画する
	FBOOL IsActive(void);	// アクティブなら真を返す
	FBOOL IsFinished(void);	// 終了していれば真を返す
	FVOID Initialize(void);	// 状態を初期化する(無効状態にする)

	CDrawBonus(RECT *rcTargetX256);	// コンストラクタ
	~CDrawBonus();					// デストラクタ


private:
	FVOID DrawInit(DWORD Count);		// 最初の半透明四角形を描画する
	FVOID DrawMaxCombo(DWORD Count);	// 最大コンボ数を表示する
	FVOID DrawClearTime(DWORD Count);	// クリア時間を表示する
	FVOID DrawTotal(DWORD Count);		// 合計得点を表示する
	FVOID DrawFinish(DWORD Count);		// エフェクトの後始末表示

	// 色を考慮に入れた数字を描画する //
	FVOID DrawNumber(int sx, int sy, DWORD n, BYTE ColID, BYTE Transform);

	// 色を考慮に入れた数字を描画する(+ Pts 表示) //
	FVOID DrawPoints(int sx, int sy, DWORD n, BYTE ColID, BYTE Transform);

	int			m_ox;			// 原点のＸ座標
	int			m_oy;			// 原点のＹ座標

	DWORD		m_MaxCombo;		// 最大コンボ数
	DWORD		m_ClearTime;	// クリア時間(msec)

	DWORD		m_ComboScore;	// コンボ数による得点
	DWORD		m_TimeScore;	// クリア時間による得点

	DWORD		m_Count;		// カウンタ
	DWORD		m_State;		// 現在の状態
};



#endif
