/*
 *   VKeyCode.h   : 仮想キーコード定義
 *
 */

#ifndef VKEYCODE_INCLUDED
#define VKEYCODE_INCLUDED "仮想キーコード定義 : Version 0.01 : Update 2001/09/02"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/02 : "Gian2001.h" から移動



/***** [ 定数 ] *****/

// キーボード定数 //
#define KEY_UP			0x0001		// [  上方向  ] に対応するキーが押されている
#define KEY_DOWN		0x0002		// [  下方向  ] に対応するキーが押されている
#define KEY_LEFT		0x0004		// [  左方向  ] に対応するキーが押されている
#define KEY_RIGHT		0x0008		// [  右方向  ] に対応するキーが押されている
#define KEY_SHOT		0x0010		// [ ショット ] に対応するキーが押されている
#define KEY_BOMB		0x0020		// [   ボム   ] に対応するキーが押されている
#define KEY_SHIFT		0x0040		// [シフト移動] に対応するキーが押されている

//#define KEY_OK			0x0080		// [    決定    ] に対応するキー
//#define KEY_CANCEL		0x0100		// [ キャンセル ] に対応するキー
#define KEY_MENU		0x0080		// [ メニュー ] に対応するキー(ESC 等)

#define KEY_CPUMOVE		0xffff		// ＣＰＵ動作を行うときに使用するキーコード



#endif
