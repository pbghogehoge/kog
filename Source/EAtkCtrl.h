/*
 *   EAtkCtrl.h   : 敵の攻撃管理
 *
 */

#ifndef EATKCTRL_INCLUDED
#define EATKCTRL_INCLUDED "敵の攻撃管理 : Version 0.01 : Update 2001/03/31"

/*  [更新履歴]
 *    Version 0.02 : 2001/04/01 : 大まかな部分(弾幕系)の完成
 *    Version 0.01 : 2001/03/31 : 敵弾管理から構造体名を変更して、移動
 */



#include "PbgType.h"
#include "Enemy.h"
#include "Laser.h"



// 最大数 //
#define ATTACK_CSTK_MAX			8		// 関数コールスタックの深さ



/***** [ 構造体 ] *****/

// 攻撃コマンド構造体 //
typedef struct tagAttackCommand {
	int		ox, oy;		// 基本座標(x256)

	int		Param;		// ATK2 用パラメータ

	WORD	Cmd;		// 発射コマンド
	WORD	EffectID;	// エフェクトＩＤ

	int		Speed;		// 速度(x256)
	int		Accel;		// 加速度
	int		Length;		// 長さ(ショートレーザー用)

	BYTE	wAngle;		// 発射幅
	char	NdAngle;	// 連弾発射時にずらしてゆく角度(New!)
	BYTE	Num;		// 発射する弾数
	BYTE	NumS;		// 連弾数

	BYTE	Angle;		// 基本角
	char	vAngle;		// 角速度
	BYTE	Color;		// 外見の指定(上位が形状、下位が色)

	BYTE	Rep;		// 繰り返し回数
	BYTE	Type;		// 移動タイプ
	BYTE	Option;		// 属性

	char	AnmSpd;		// アニメーションスピード
	BYTE	Reserved00;	// 予約済み
} AttackCommand;


class CEnemyCtrl;
class CEnemyLLaser;



// 敵の攻撃管理クラス(FixedLList で並列化すべし)              //
// なお、現在の(x, y) は m_Cmd メンバの (ox, oy) に格納される //
class EnemyAtkCtrl {
public:
	int		m_dx;		// 親のｘ座標からどれだけずらすか
	int		m_dy;		// 親のｙ座標からどれだけずらすか

	int		CallStack[ATTACK_CSTK_MAX];	// 関数コールスタック
	WORD	CallSP;						// 関数コールスタックのスタックポインタ
	WORD	RepCount;					// マクロ命令の繰り返し回数

	BYTE	*m_pTCLCmd;	// 次に実行する命令へのポインタ

	EnemyData		*m_pParent;		// 親構造体
	AttackCommand	m_Cmd;			// 攻撃コマンド構造体

	// １フレームだけ動作させる //
	FBOOL ParseECLCmd(CEnemyCtrl *pParent);

	// LL 呼び出し用関数(このクラスからの呼び出しは行わない) //
	FVOID ConnectLLaser(LLaserData *pLaser);		// 太レーザーを接続
	LLaserData *GetLLaserPtr(void);					// ポインタ取得
	FVOID DisableLLaserPtr(void);					// ポインタ無効化

	// EnemyCtrl 呼び出し用 //
	FVOID DisconnectLLaser(CEnemyLLaser *pLLaser);	// 太レーザーの切り離し

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


	// 太レーザーへのポインタ //
	LLaserData		*m_pLLaser;
};



#endif
