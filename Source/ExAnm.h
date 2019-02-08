/*
 *   ExAnm.h   : アニメ定義ブロック
 *
 */

#ifndef EXANM_INCLUDED
#define EXANM_INCLUDED "アニメ定義ブロック : Version 0.01 : Update 2001/09/24"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/24 : 制作開始
 */



#include "PbgType.h"
#include "CAnimeDef.h"



/***** [ 定数 ] *****/

#define EXANM_REG_AX		0x00	// アニメーション：相対Ｘ・中心
#define EXANM_REG_AY		0x01	// アニメーション」相対Ｙ・中心
#define EXANM_REG_ASX		0x02	// アニメーションパターンＸ拡大率
#define EXANM_REG_ASY		0x03	// アニメーションパターンＹ拡大率

#define EXANM_REG_AD		0x04	// アニメーションパターンの傾き
#define EXANM_REG_AA		0x05	// アニメーションパターンのα値
#define EXANM_REG_APTN		0x06	// アニメーションパターン番号
#define EXANM_REG_APTN2		0x07	// アニメーションパターン番号(2)

#define EXANM_REG_CX		0x08	// クリッピング枠：相対Ｘ座標中心
#define EXANM_REG_CY		0x09	// クリッピング枠：相対Ｙ座標中心
#define EXANM_REG_CW		0x0a	// クリッピング枠：横幅
#define EXANM_REG_CH		0x0b	// クリッピング枠：縦幅

#define EXANM_REG_CR		0x0c	// クリッピング枠の色：赤成分
#define EXANM_REG_CG		0x0d	// クリッピング枠の色：緑成分
#define EXANM_REG_CB		0x0e	// クリッピング枠の色：青成分
#define EXANM_REG_CA		0x0f	// クリッピング枠の色：α成分

#define EXANM_REG_GR0		0x10	// 汎用レジスタ０番
#define EXANM_REG_GR1		0x11	// 汎用レジスタ１番
#define EXANM_REG_GR2		0x12	// 汎用レジスタ２番
#define EXANM_REG_GR3		0x13	// 汎用レジスタ３番
#define EXANM_REG_GR4		0x14	// 汎用レジスタ４番
#define EXANM_REG_GR5		0x15	// 汎用レジスタ５番
#define EXANM_REG_GR6		0x16	// 汎用レジスタ６番
#define EXANM_REG_GR7		0x17	// 汎用レジスタ７番
#define EXANM_REG_GR8		0x18	// 汎用レジスタ８番
#define EXANM_REG_GR9		0x19	// 汎用レジスタ９番

#define EXANM_NUMREGS		26		// 全レジスタの本数

#define EXANM_GR_MAX		10		// 汎用レジスタ１０本
#define EXANM_CSTK_MAX		10		// ループは１０段まで

#define TEXMODE_ALPHANORM	0x00	// 通常半透明モード
#define TEXMODE_ALPHAADD	0x01	// 加算半透明モード
#define TEXMODE_SINGLE		0x00	// シングルテクスチャモード
#define TEXMODE_DOUBLE		0x02	// ダブルテクスチャモード



/***** [クラス定義] *****/

class CAtkGrpDraw;


// アニメーション専用タスククラス //
class CAnimeTask {
public:
	// 初期化する
	// arg : pCommand    初期命令列へのポインタ             //
	//     : pAnimeDef   アニメーション定義(キャラクタ固有) //
	//     : pParent     親タスクへのポインタ(grX を継承)   //
	// ret : 成功なら TRUE                                  //
	FBOOL Initialize(BYTE *pCommand, CAnimeDef *pAnimeDef, CAnimeTask *pParent);

	// １フレーム分更新する(FALSE ならば、タスクを消すべし) //
	FBOOL Move(CAtkGrpDraw *pParent);	// 通常の更新
	FBOOL MoveDelete(void);				// 消去状態の更新

	// 描画を行う(注:呼び出し前後でクリッピング矩形が変化) //
	// arg : ox256, oy256 中心となる座標(x256)             //
	FVOID Draw(int ox256, int oy266);

	CAnimeTask();		// コンストラクタ
	~CAnimeTask();		// デストラクタ


private:
	// レジスタに定数を代入する //
	IBOOL MoveRegister(BYTE *pCmd);

	// アドレススタック関連 //
	IBOOL AStk_Push(int Value);	// スタックに積む
	IINT  AStk_Pop(void);		// スタックから取り出し
	IINT  AStk_PopN(int n);		// ｎ回Popして、最後に取り出した値を返す

	// トップが０ならば POP して FALSE を返し、           //
	//       非０ならば DEC して TRUE  を返す             //
	IINT  AStk_DecTop(void);	// スタックトップをデクリメント


private:
	// スクリプト ExAnm 使用者には隠すべきメンバ(1) //
	CAnimeDef	*m_pAnimeDef;	// アニメーションパターン定義クラス
	BYTE		*m_pCmd;		// 命令コマンド列(現在位置)

	// スクリプト ExAnm 使用者には隠すべきメンバ(2) //
	int		m_Gr[EXANM_GR_MAX];				// 汎用レジスタ
	int		m_CallStack[EXANM_CSTK_MAX];	// 関数コールスタック
	WORD	m_CallSP;						// スタックポインタ
	WORD	m_RepCount;						// 繰り返し命令用カウンタ

	// アニメーション表示位置 //
	int		m_AnimeX, m_AnimeY;		// アニメーションパターンの中心座標
	int		m_AnimeSX, m_AnimeSY;	// アニメーションパターンの拡大率(256 = 1.0)

	// クリッピング矩形 //
	int		m_ClipX, m_ClipY;		// クリッピング枠の中心座標
	int		m_ClipW, m_ClipH;		// クリッピング枠のサイズ

	// テクスチャモード //
	BYTE	m_TexMode;

	// アニメーションパターンの補助データ //
	BYTE	m_AnimeAngle;	// 傾き(０が元の角度)
	BYTE	m_AnimePtn[2];	// アニメーション番号
	short	m_AnimeAlpha;	// テクスチャのα値

	// アニメーション用クリッピング矩形・塗りつぶし色 //
	short	m_ClipRed;		// 赤成分
	short	m_ClipGreen;	// 緑成分
	short	m_ClipBlue;		// 青成分
	short	m_ClipAlpha;	// α成分
};



#endif
