/*
 *   CCharacterBase.h   : キャラクタの定義(基底)
 *
 */

#ifndef CCHARACTERBASE_INCLUDED
#define CCHARACTERBASE_INCLUDED "キャラクタの定義(基底) : Version 0.02 : Update 2001/09/25"

/*  [更新履歴]
 *    Version 0.02 : 2001/09/25 : 攻撃送りの管理をスクリプト側に委託
 *    Version 0.01 : 2001/04/05 : 制作開始
 */



#include "Gian2001.h"
#include "CExtraShot.h"
#include "ChargeDef.h"
#include "CBGDraw.h"
#include "Enemy.h"



/***** [クラス定義] *****/

// エキストラショット管理構造体 //
typedef struct tagExtraShotInfo {
	CExtraShot	*pExtraShot;	// エキストラショットの格納先
	DWORD		Charge;			// 現在のチャージ値
	int			State;			// 現在のキャラクタ傾き
	WORD		KeyCode;		// キーボードの状態
} ExtraShotInfo;

/*
// 攻撃送りグラフィック管理構造体 //
typedef struct tagAtkGrpInfo {
	int		m_EfcX;		// テクスチャ中心のＸ座標
	int		m_EfcY;		// テクスチャ上端のＹ座標
	int		m_DeltaY;	// 描画時のＹ座標シフト値
	int		m_Speed;	// 移動速度

	DWORD	m_State;	// 状態定数
	DWORD	m_Count;	// エフェクトカウンタ

	WORD	m_Size;		// 拡縮用倍率

	BYTE	m_Angle;	// 進行方向
	BYTE	m_Alpha;	// α値
} AtkGrpInfo;
*/

// 相互参照のため //
class CSCLDecoder;


// キャラクタ定義用クラス(関数ポインタ・一括差し替え用) //
class CCharacterBase {
public:
	// 速度吐き出し //
	FINT   GetNormalSpeed(void);	// 通常移動の速度を取得する
	FINT   GetShiftSpeed(void);		// シフト移動の速度を取得する
	FDWORD GetChargeSpeed(void);	// 溜める早さを求める
	FDWORD GetBuzzSpeed(void);		// カスり溜めの早さを求める
	FINT   GetNShotDx(void);		// 通常ショットの発射座標のシフト値
	FINT   GetNShotDy(void);		// 通常ショットの発射座標のシフト値
	FINT   GetBombTime(void);		// ボムの有効時間を求める

	// キャラクタ固有のＩＤを取得する //
	CHARACTER_ID GetCharID(void);

	// エキストラショットを動作させる //
	virtual FVOID MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy)  = 0;

	// エキストラショットの当たり判定を行う //
	virtual FDWORD HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy) = 0;

	// エキストラショットを描画する //
	virtual FVOID DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID) = 0;

	// キャラクター固有の描画を行う //
	virtual FVOID PlayerDraw(	int		x256				// Ｘ座標中心
							,	int		y256				// Ｙ座標中心
							,	int		State				// 状態(角度)
							,	BOOL	bDamaged			// ダメージを受けていれば真
							,	int		Transform			// 変形等をするときは非ゼロ
							,	int		TextureID) = 0;		// テクスチャ番号

	virtual FVOID PlayerSetGrp(int *pState, WORD KeyCode) = 0;

	// 通常の溜め攻撃 //
	virtual FVOID Level1Attack(ExtraShotInfo *pExShotInfo) = 0;		// レベル１
	FVOID Level2Attack(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv);	// レベル２
	FVOID Level3Attack(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv);	// レベル３
	FVOID Level4Attack(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv);	// ボスアタック

	// ボムアタック //
	virtual FBOOL Level1BombAtk(ExtraShotInfo *pExShotInfo) = 0;	// レベル１
	FBOOL Level2BombAtk(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv);	// レベル２
	FBOOL Level3BombAtk(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv);	// レベル３
	FBOOL Level4BombAtk(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv);	// ボスアタック

	// ノーマルショットが撃てれば真を返す
	virtual FBOOL IsEnableNormalShot(CExtraShot *pExtraShot) = 0;

	// ボム //
	virtual FVOID NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw) = 0;


protected:
	// AtkGrp のデフォルト初期化用関数 //
//	static FVOID InitDefaultAtkGrp(AtkGrpInfo *pInfo);

	int		m_NormalSpeed;		// 通常時の早さ
	int		m_ShiftSpeed;		// シフト移動時の早さ
	int		m_BombTime;			// ボムの有効時間(つまり無敵時間)
	DWORD	m_ChargeSpeed;		// 溜めの早さ
	DWORD	m_BuzzSpeed;		// カスり溜めの早さ

	int				m_NormalShotDx;	// 通常ショットの発射開始位置(x)
	int				m_NormalShotDy;	// 通常ショットの発射開始位置(y)
	CHARACTER_ID	m_CharID;		// キャラクタ固有のＩＤ
};



#endif
