/*
 *   CStg2Boss.h   : めい＆まいの定義
 *
 */

#ifndef CSTG2BOSS_INCLUDED
#define CSTG2BOSS_INCLUDED "めい＆まいの定義 : Version 0.01 : Update 2001/04/05"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/05 : 仮組み
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CStg2Boss : public CCharacterBase {
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

	CStg2Boss();		// コンストラクタ
	~CStg2Boss();		// デストラクタ
};



/***** [グローバル変数] *****/
extern CStg2Boss	g_Stg2BossInfo;			// めい＆まいの情報格納用クラス



#endif
