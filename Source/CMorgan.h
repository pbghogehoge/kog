/*
 *   CMorgan.h   : モーガンの定義
 *
 */

#ifndef CMORGAN_INCLUDED
#define CMORGAN_INCLUDED "モーガンの定義 : Version 0.01 : Update 2001/11/07"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/07 : 制作開始
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CMorgan : public CCharacterBase {
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

	CMorgan();		// コンストラクタ
	~CMorgan();		// デストラクタ


private:
	FVOID DrawExplosion(ExtraShot *pExShot);	// 爆発の描画
	FVOID DrawExpLockon(ExtraShot *pExShot);	// ロックしてある個所を描画
	FVOID DrawBombExp(ExtraShot *pExShot);		// ボム爆発の描画
	FVOID DrawBombExpDel(ExtraShot *pExShot);	// 消去用ボム爆発の描画
};



/***** [グローバル変数] *****/
extern CMorgan	g_MorganInfo;			// モーガンの情報格納用クラス



#endif
