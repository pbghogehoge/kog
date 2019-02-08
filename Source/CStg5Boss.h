/*
 *   CStg5Boss.h   : エーリッヒの定義
 *
 */

#ifndef CSTG5BOSS_INCLUDED
#define CSTG5BOSS_INCLUDED "エーリッヒの定義 : Version 0.01 : Update 2001/04/05"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/05 :
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CStg5Boss : public CCharacterBase {
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

	CStg5Boss();		// コンストラクタ
	~CStg5Boss();		// デストラクタ


private:
	FVOID DrawHLaser(ExtraShot *pExShot);		// ホーミングレーザーを描画する
	FVOID DrawBombHLaser(ExtraShot *pExShot);	// ホーミングレーザーを描画する
	FVOID DrawLockOnMarker(ExtraShot *pExShot);	// ロックオンマーカーを描画する
};



/***** [グローバル変数] *****/
extern CStg5Boss	g_Stg5BossInfo;		// エーリッヒの情報格納用クラス



#endif
