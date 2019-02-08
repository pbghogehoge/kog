/*
 *   CStg1Boss.h   : ミリアの定義
 *
 */

#ifndef CSTG1BOSS_INCLUDED
#define CSTG1BOSS_INCLUDED "ミリアの定義 : Version 0.01 : Update 2001/04/05"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/05 : 仮埋め込み
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CStg1Boss : public CCharacterBase {
public:
	// キャラクター固有の描画を行う //
	FVOID PlayerDraw(	int		x256				// Ｘ座標中心
					,	int		y256				// Ｙ座標中心
					,	int		State				// 状態(角度)
					,	BOOL	bDamaged			// ダメージを受けていれば真
					,	int		Transform			// 変形等をするときは非ゼロ
					,	int		TextureID);			// テクスチャ番号

	// 描画用のＩＤを変更する //
	FVOID PlayerSetGrp(int *pState, WORD KeyCode);

	// エキストラショットを動作させる //
	FVOID MoveExtraShot(ExtraShotInfo *pExShotInfo, CEnemyCtrl *pEnemy);

	// エキストラショットの当たり判定を行う //
	FDWORD HitCheckExtraShot(CExtraShot *pExtraShot, EnemyData *pEnemy);

	// エキストラショットを描画する //
	FVOID DrawExtraShot(ExtraShotInfo *pExShotInfo, int TextureID);

	FVOID Level1Attack(ExtraShotInfo *pExShotInfo);		// 通常の溜め攻撃
	FBOOL Level1BombAtk(ExtraShotInfo *pExShotInfo);	// ボムアタック

	// ノーマルショットが撃てれば真を返す //
	FBOOL IsEnableNormalShot(CExtraShot *pExtraShot);

	// ボム //
	FVOID NormalBomb(ExtraShotInfo *pExShotInfo, CBGDraw *pBGDraw);		// 通常ボム

	CStg1Boss();		// コンストラクタ
	~CStg1Boss();		// デストラクタ


public:
	// 雷の当たり判定を行う             //
	// arg  pEnemy : 対象となる敵データ //
	//      pShot  : 対象となる雷データ //
	// ret         : ダメージ           //
	FDWORD HitCheckThunder(EnemyData *pEnemy, ExtraShot *pShot);

	FVOID DrawThunder(ExtraShot *pShot);		// 雷を描画する
	FVOID DrawBombThunder(ExtraShot *pShot);	// ボム用雷を描画する
	FVOID DrawSphere(ExtraShot *pShot);			// びりびり球体を描画する
	FVOID DrawBombSphere(ExtraShot *pShot);		// ボム用球体を描画する
};



/***** [グローバル変数] *****/
extern CStg1Boss	g_Stg1BossInfo;			// ミリアの情報格納用クラス



#endif
