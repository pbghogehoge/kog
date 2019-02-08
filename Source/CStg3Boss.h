/*
 *   CStg3Boss.h   : ゲイツの定義
 *
 */

#ifndef CSTG3BOSS_INCLUDED
#define CSTG3BOSS_INCLUDED "ゲイツの定義 : Version 0.02 : Update 2001/06/26"

/*  [更新履歴]
 *    Version 0.02 : 2001/06/26 : はじめ
 *    Version 0.01 : 2001/04/05 : 制作開始
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CStg3Boss : public CCharacterBase {
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

	CStg3Boss();		// コンストラクタ
	~CStg3Boss();		// デストラクタ


public:
	// 火炎粒子を描画する //
	FVOID DrawFire(int x256, int y256, int n);

	// ボム演出用のわっかを描画する //
	FVOID DrawExRing(ExtraOption *pOpt);
};



/***** [グローバル変数] *****/
extern CStg3Boss	g_Stg3BossInfo;			// ゲイツの情報格納用クラス



#endif
