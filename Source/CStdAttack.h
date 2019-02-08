/*
 *   CStdAttack.h   : 標準弾送り
 *
 */

#ifndef CSTDATTACK_INCLUDED
#define CSTDATTACK_INCLUDED "標準弾送り   : Version 0.01 : Update 2001/07/06"

/*  [注意事項]
 *    ・EAtkCtrl と区別すること。CStdAttack では、通常弾送りと
 *    そのエフェクトを管理するものとする。
 *    ・２段固定長リストを使用する(待ち状態、消去中)
 *    ・毎回、規則的に攻撃コマンド(ONE/ALL/RND)が変化する
 */

/*  [更新履歴]
 *    Version 0.01 : 2001/07/06 : 時間がない
 */

#include "PbgType.h"
#include "EAtkCtrl.h"
#include "CEnemyTama.h"
#include "CFixedLList.h"



/***** [ 定数 ] *****/
#define SENDTAMA_MAX		40		// 同時に動作可能な発動警告エフェクト



/***** [クラス定義] *****/

// 攻撃予告用のＷＡＲＮＩＮＧ表示 //
typedef struct tagAtkWarning {
	AttackCommand	m_Cmd;		// 発射する弾幕コマンド
	int				m_x;		// 現在のＸ座標
	int				m_y;		// 現在のＹ座標

	int				m_Count;	// 経過時間カウンタ
	int				m_Speed;	// このエフェクトの移動速度
	int				m_Size;		// 待ちエフェクトのサイズ
	BYTE			m_Angle;	// このエフェクトの進行方向
} AtkWarning;


// 攻撃予告の表示 //
class CStdAttack : public CFixedLList<AtkWarning, 3, SENDTAMA_MAX> {
public:
	// 初期化：セット対象を決める //
	FBOOL Initialize(CEnemyTama *pTarget, RECT *rcTargetX256);

	// 弾を送りつける //
	FVOID Set(int sx, int sy, DWORD Level);

	FVOID Move(void);			// １フレーム分動作させる
	FVOID Draw(void);			// 描画する
	FVOID Clear(void);			// 攻撃エフェクトを消去する

	CStdAttack();		// コンストラクタ
	~CStdAttack();		// デストラクタ


private:
	// 弾をセットする対象 //
	CEnemyTama	*m_pTarget;

	// 画面外判定用(全てx256) //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標

	WORD	m_CurrentCmd;	// 最後に送りつけた攻撃コマンド(ONE/ALL/RND)
};


#endif
