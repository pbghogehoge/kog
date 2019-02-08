/*
 *   Gian2001.h   : ゲームの定数定義
 *
 */

#ifndef GIAN2001_INCLUDED
#define GIAN2001_INCLUDED "ゲームの定数定義 : Version 0.02 : Update 2001/03/07"

/*  [注意事項]
 *    ゲーム本体に直接関係するファイルについては、PbgType.h ではなく、
 *   このヘッダファイルをインクルード
 *
 *    一方、このヘッダファイル内ではゲームに直接関係する
 *    ヘッダファイルをインクルードしない事
 */

/*  [更新履歴]
 *    Version 0.02 : 2001/03/07 : グラフィック管理クラスをグローバル変数とした
 *    Version 0.01 : 2001/02/27 : 制作開始
 */



#include "PbgMain.h"
#include "CExLList.h"
#include "CFixedLList.h"
//#include "FontDraw.h"
#include "CAnmCursor.h"
#include "CTextWindow.h"
#include "CMusicCtrl.h"

#include "VKeyCode.h"


/***** [ 定数 ] *****/

// ガードブレイク関連 //
#define GBREAK_TIME		(60*2)	// ガードブレイクする時間(行動不能)


// テクスチャＩＤ //
#define TEXTURE_ID_TAMA		0x00	// 弾テクスチャ
#define TEXTURE_ID_HLASER	0x01	// ホーミングレーザー用テクスチャ
#define TEXTURE_ID_FONT		0x02	// フォントテクスチャ

#define TEXTURE_ID_BG01		0x03	// 背景テクスチャ１番
#define TEXTURE_ID_BG02		0x04	// 背景テクスチャ２番
#define TEXTURE_ID_BG03		0x05	// 背景テクスチャ３番
#define TEXTURE_ID_BG04		0x06	// 背景テクスチャ４番

#define TEXTURE_ID_ENEMY1	0x07	// 敵テクスチャ１番
#define TEXTURE_ID_ENEMY2	0x08	// 敵テクスチャ２番
#define TEXTURE_ID_ENEMY3	0x09	// 敵テクスチャ３番
#define TEXTURE_ID_ENEMY4	0x0a	// 敵テクスチャ４番

#define TEXTURE_ID_EFFECT	0x0b	// 汎用エフェクト用テクスチャ

#define TEXTURE_ID_STAGE	0x0c	// ステージ表示エフェクト用テクスチャ
#define TEXTURE_ID_ATKEFC	0x0d	// 攻撃予告エフェクト用テクスチャ
#define TEXTURE_ID_BONUS	0x0e	// ボーナス表示エフェクト用テクスチャ
#define TEXTURE_ID_GUARD	0x0f	// ガードゲージ用テクスチャ
#define TEXTURE_ID_SHIELD	0x10	// シールド用テクスチャ

#define TEXTURE_ID_P1_00	0x11	// プレイヤー１用テクスチャ１枚目
#define TEXTURE_ID_P1_01	0x12	// プレイヤー１用テクスチャ２枚目
#define TEXTURE_ID_P1_02	0x13	// プレイヤー１用テクスチャ３枚目
#define TEXTURE_ID_P1_03	0x14	// プレイヤー１用テクスチャ４枚目
#define TEXTURE_ID_P1_04	0x15	// プレイヤー１用テクスチャ５枚目
#define TEXTURE_ID_P1_05	0x16	// プレイヤー１用テクスチャ６枚目
#define TEXTURE_ID_P1_06	0x17	// プレイヤー１用テクスチャ７枚目
#define TEXTURE_ID_P1_07	0x18	// プレイヤー１用テクスチャ８枚目
#define TEXTURE_ID_P1_08	0x19	// プレイヤー１用テクスチャ９枚目
#define TEXTURE_ID_P1_09	0x1a	// プレイヤー１用テクスチャ10枚目
#define TEXTURE_ID_P1_10	0x1b	// プレイヤー１用テクスチャ11枚目
#define TEXTURE_ID_P1_11	0x1c	// プレイヤー１用テクスチャ12枚目

#define TEXTURE_ID_P2_00	0x1d	// プレイヤー２用テクスチャ１枚目
#define TEXTURE_ID_P2_01	0x1e	// プレイヤー２用テクスチャ２枚目
#define TEXTURE_ID_P2_02	0x1f	// プレイヤー２用テクスチャ３枚目
#define TEXTURE_ID_P2_03	0x20	// プレイヤー２用テクスチャ４枚目
#define TEXTURE_ID_P2_04	0x21	// プレイヤー２用テクスチャ５枚目
#define TEXTURE_ID_P2_05	0x22	// プレイヤー２用テクスチャ６枚目
#define TEXTURE_ID_P2_06	0x23	// プレイヤー２用テクスチャ７枚目
#define TEXTURE_ID_P2_07	0x24	// プレイヤー２用テクスチャ８枚目
#define TEXTURE_ID_P2_08	0x25	// プレイヤー２用テクスチャ９枚目
#define TEXTURE_ID_P2_09	0x26	// プレイヤー２用テクスチャ10枚目
#define TEXTURE_ID_P2_10	0x27	// プレイヤー２用テクスチャ11枚目
#define TEXTURE_ID_P2_11	0x28	// プレイヤー２用テクスチャ12枚目

#define TEXTURE_ID_EXTAMA	0x29	// 特殊弾幕用テクスチャ
#define TEXTURE_ID_STFONT	0x2a	// 状態表示用テクスチャ



// キャラクタ固有テクスチャ(後)の番号 //
#define TEXTURE_ID_P1_TAIL		TEXTURE_ID_P1_11
#define TEXTURE_ID_P2_TAIL		TEXTURE_ID_P2_11

// キャラクタごとに使用できるテクスチャの枚数
// この周辺を変更した場合は、CEnemyAnime 及び、CAtkGrpXX 等に修正を加える事！！ //
#define LOADTEXTURE_MAX		12		// 同時ロードできるテクスチャの最大枚数



/***** [ 列挙型 ] *****/

// プレイヤー指定用のＩＤ //
typedef enum tagCHARACTER_ID {
	CHARACTER_VIVIT  = 0,		// びびっとさん
	CHARACTER_STG1   = 1,		// ミリア
	CHARACTER_STG2   = 2,		// めい＆まい
	CHARACTER_STG3   = 3,		// ゲイツ
	CHARACTER_STG4   = 4,		// マリー
	CHARACTER_STG5   = 5,		// エーリッヒ
	CHARACTER_MORGAN = 6,		// モーガン
	CHARACTER_MUSE   = 7,		// ミューズ
	CHARACTER_YUKA   = 8,		// 幽香

	CHARACTER_UNKNOWN = 99999	// １Ｐストーリーモード用
} CHARACTER_ID;

// シーン指定用のＩＤ //
typedef enum tagSCENE_ID {
	SCENE_PLAYER_1 = 1,
	SCENE_PLAYER_2 = 2
} SCENE_ID;



/***** [ マクロ ] *****/

// Fast の方は、動作環境に依存するかもね //
#define HitCheckFast(a,b,h)	( ((DWORD)(a)-(b)+(h)) < ((DWORD) (h)*2) )	// 条件分岐無しVersion
#define HitCheckSTD(a,b,h)	(abs((a)-(b))<(h))							// いわゆる普通の判定



/***** [グローバル変数] *****/
extern Pbg::CGraphic		*g_pGrp;		// グラフィック管理クラス
extern Pbg::CGrpSurface		*g_pSystemSurf;	// システム画像の格納先

extern Pbg::CInput			*g_pInp;		// 入力デバイス管理クラス
extern CAnmCursor			*g_pCursor;		// カーソル管理クラス
extern CTextWindow			*g_pTextWindow;	// テキスト窓管理クラス

extern CMusicCtrl			*g_pMusic;		// 曲管理クラス



#endif
