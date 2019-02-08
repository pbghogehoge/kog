/*
 *   Enemy.h   : 敵管理構造体
 *
 */

#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED "敵管理構造体 : Version 0.02 : Update 2001/09/28"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/28 : 親子関連部の追加(for BossAttack)
 *    Version 0.01 : 2001/03/14 : 制作開始
 */



#include "PbgType.h"
#include "CEnemyAnime.h"



/***** [ 定数 ] *****/

// 最大数 //
#define ENEMY_GREG_MAX		8		// 汎用レジスタの本数
#define ENEMY_CSTK_MAX		8		// コールスタックの深さ
#define ENEMY_CHILD_MAX		8		// 子オブジェクトの最大数

// 敵フラグ //
#define EFLG_CLIP_ON		0x0001		// 画面外でも消去しない
#define EFLG_RLCHG_ON		0x0002		// 左右反転を有効にする
#define EFLG_DRAW_ON		0x0004		// 描画を行う
#define EFLG_HIT_ON			0x0008		// 当たり判定がある
#define EFLG_BOMB2X			0x0010		// 爆発サイズ４倍
#define EFLG_KILL			0x0020		// 強制的に殺す(fatk 有効)
#define EFLG_HPDAMAGE		0x0040		// ヒット時にＨＰ分だけダメージ
#define EFLG_SYNCANGLE_OFF	0x0080		// 角度同期をオフにする

#define EFLG_TOPMOST		0x0100		// 非 TOPMOST 属性の敵よりも
										// 上に描画されるようにする

#define EFLG_ALPHAONE		0x0200		// 必ず加算αで描画する



/***** [ クラス定義 ] *****/

// 相互参照のため... //
class CEnemyCtrl;


// 敵データ //
class EnemyData {
public:
	int		x, y;				// 現在の座標(x256 固定小数点数)
	int		vx, vy;				// 現在の速度成分(マクロコマンド専用)

	int		size;				// 当たり判定サイズ
	int		v;					// 現在の速度(x256 ...)
	int		a;					// 加速度
	DWORD	hp;					// 体力の残り
	DWORD	Count;				// 発生時から何フレーム経過したか
	DWORD	Score;				// 得点

	int		Gr[ENEMY_GREG_MAX];			// 汎用レジスタ
	int		CallStack[ENEMY_CSTK_MAX];	// 関数コールスタック

	BYTE		*pCmd;				// 現在の命令格納アドレス
	BYTE		*pFinalAttack;		// 死亡時の打ち返し実行関数
	EAnimePtn	*pAnimePtn;			// アニメーションパターン定義構造体

	EnemyData	*pParent;					// 親データへのポインタ
	EnemyData	*pChild[ENEMY_CHILD_MAX];	// 子データに対するユニークな値

	WORD	CallSP;		// 関数コールスタックのスタックポインタ
	WORD	RepCount;	// マクロ命令の繰り返し回数

	WORD	Flag;		// 状態フラグ
	BYTE	d;			// 進行角度
	char	vd;			// 角速度
	char	AnmSpd;		// アニメーションスピード
	BYTE	AnmCount;	// アニメーションカウンタ
	BYTE	DamageFlag;	// ダメージを受けていれば非０になる
	BYTE	Alpha;		// α値


	// ＥＣＬ命令を１フレーム分だけデコードする  //
	// 戻り値 : FALSE この敵を削除する必要がある //
	FBOOL ParseECLCmd(CEnemyCtrl *pParent, BYTE TargetList);


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
};



/***** [ 型 ] *****/

// EnumActiveEnemy() 用コールバック関数     //
// ret : その敵に対するダメージ             //
//                                          //
// args     pParam : 呼び出し元定義のデータ //
//          pEnemy : 敵データ               //
typedef DWORD (*DamageCallback)(void *pParam, EnemyData *pEnemy);



#endif
