/*
 *   CKeyInput.h   : キーボード入力管理
 *
 */

#ifndef CKEYINPUT_INCLUDED
#define CKEYINPUT_INCLUDED "キーボード入力管理 : Version 0.02 : Update 2001/09/04"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/03 : メニューの入力を CInputCtrl に委託
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "CInputDevBase.h"
#include "VKeyCode.h"



/***** [クラス定義] *****/

// キーデータの名前テーブル //
typedef struct tagDIK_NameTable {
	char	Data[16];
} DIK_NameTable;


// キーボード入力管理クラス //
class CKeyInput : public CInputDevBase {
public:
	// 入力コードを取得する //
	WORD GetCode(void);

	// 指定されたキーに対して割り当てを行う //
	BOOL OnChangeSetting(BYTE TargetID);

	// ある機能に割り当てられたボタンorキーの名称を返す //
	FVOID GetButtonName(char *pBuf, BYTE TargetID);

	// 設定を初期状態にリセットする //
	void OnResetSetting(void);

	// SearchCode が一致したボタンに SetCode を割り当てる      //
	// 割り当てが成功するか SearchCode == SetCode なら真を返す //
	BOOL SwapCode(BYTE SearchCode, BYTE SetCode);

	// 設定の入出力 //
	void GetConfigData(BYTE Data[8+3]);	// 現在の設定を取得する
	void SetConfigData(BYTE Data[8+3]);	// 設定を変更する

	// キーコードを共有する対象の設定(２Ｐキーボード用) //
	void ShareKeyConfig(CKeyInput *pShare);

	// コンストラクタはキーコードバッファへのポインタを受ける //
	CKeyInput(const BYTE *pBuffer, INPUT_DEVICE_ID DevID);	// コンストラクタ
	~CKeyInput();											// デストラクタ


private:
	FVOID InitializeNameTable(void);	// 名前テーブルを初期化する
	FVOID CleanupNameTable(void);		// 名前テーブルを解放する

	BYTE	m_ShotID;		// ショット に割り当てられたキー番号
	BYTE	m_BombID;		//   ボム   に割り当てられたキー番号
	BYTE	m_ShiftID;		// 低速移動 に割り当てられたキー番号
	BYTE	m_DirID[8];		// 方向キー に割り当てられたキー番号

	class CKeyInput			*m_pShare;		// データを共有するクラス
	const BYTE * const		m_pKeyBuf;		// キーコードバッファ

	static int				m_RefCount;		// 参照カウント
	static DIK_NameTable	*m_pKeyNameTbl;	// キーのお名前テーブル
};



#endif
