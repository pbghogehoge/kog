/*
 *   CPadInput.h   : パッド入力管理
 *
 */

#ifndef CPADINPUT_INCLUDED
#define CPADINPUT_INCLUDED "パッドの入力管理 : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "CInputDevBase.h"



/***** [クラス定義] *****/

// キーボード入力管理クラス //
class CPadInput : public CInputDevBase {
public:
	// 入力コードを取得する //
	WORD GetCode(void);

	// 指定されたボタンに対して割り当てを行う //
	BOOL OnChangeSetting(BYTE TargetID);

	// ある機能に割り当てられたボタンorキーの名称を返す //
	FVOID GetButtonName(char *pBuf, BYTE TargetID);

	// 設定を初期状態にリセットする //
	void OnResetSetting(void);

	// SearchCode が一致したボタンに SetCode を割り当てる      //
	// 割り当てが成功するか SearchCode == SetCode なら真を返す //
	BOOL SwapCode(BYTE SearchCode, BYTE SetCode);

	// このパッドの名称を取得する //
	FVOID GetDeviceName(char *pBuf);

	// 設定の入出力 //
	void GetConfigData(BYTE Data[4]);	// 現在の設定を取得する
	void SetConfigData(BYTE Data[4]);	// 設定を変更する

	CPadInput(BYTE PlayerID);	// コンストラクタ
	~CPadInput();				// デストラクタ


private:
	BYTE	m_ShotID;			// ショット に割り当てられたボタン番号
	BYTE	m_BombID;			//   ボム   に割り当てられたボタン番号
	BYTE	m_ShiftID;			//  シフト  に割り当てられたボタン番号
	BYTE	m_MenuID;			// メニュー に割り当てられたボタン番号

	const BYTE	m_PlayerID;		// 割り当てられたプレイヤー番号
};



#endif
