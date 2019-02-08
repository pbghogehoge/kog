/*
 *   CInputDevBase.h   : 入力デバイス基底
 *
 */

#ifndef CINPUTDEVBASE_INCLUDED
#define CINPUTDEVBASE_INCLUDED "入力デバイス基底 : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/02 : 制作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/

// OnConfigure() における TargetID //
#define c_DirUp			0		//  上  方向への移動
#define c_DirDown		1		//  下  方向への移動
#define c_DirLeft		2		//  左  方向への移動
#define c_DirRight		3		//  右  方向への移動
#define c_DirUpLeft		4		// 左上 方向への移動
#define c_DirUpRight	5		// 右上 方向への移動
#define c_DirDownLeft	6		// 左下 方向への移動
#define c_DirDownRight	7		// 右下 方向への移動
#define c_ButtonShot	128		// ショット
#define c_ButtonBomb	129		// ボム
#define c_ButtonShift	130		// 低速移動
#define c_ButtonMenu	131		// メニュー


// デバイス固有のＩＤ //
typedef enum tagINPUT_DEVICE_ID {
	IDEVID_FULLKEY   = 0,
	IDEVID_HALFKEY1P = 1,
	IDEVID_HALFKEY2P = 2,
	IDEVID_PAD1P     = 3,
	IDEVID_PAD2P     = 4,
} INPUT_DEVICE_ID;



/***** [クラス定義] *****/

// 入力デバイス基底 //
class CInputDevBase {
public:
	// 入力コードを取得する //
	virtual WORD GetCode(void) = 0;

	// 指定された [キー/ボタン] に対して割り当てを行う //
	virtual BOOL OnChangeSetting(BYTE TargetID) = 0;

	// ある機能に割り当てられたボタンorキーの名称を返す //
	virtual FVOID GetButtonName(char *pBuf, BYTE TargetID) = 0;

	// 設定を初期状態にリセットする //
	virtual void OnResetSetting(void) = 0;

	// 設定を変更出来ない状態に推移する                             //
	// 参考：基本的に全てのボタンorキーを放せば変更出来るようになる //
	void DisableChangeSetting(void){
		m_bEnableChange = FALSE;
	};

	// デバイス識別子を返す //
	FINT GetDeviceID(void){ return m_DeviceID; };


protected:
	CInputDevBase(){};			// コンストラクタ
	virtual ~CInputDevBase(){};	// デストラクタ

	INPUT_DEVICE_ID		m_DeviceID;			// 固有のデバイスＩＤ
	BOOL				m_bEnableChange;	// 変更可能ならば真
};



#endif
