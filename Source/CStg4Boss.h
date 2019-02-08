/*
 *   CStg4Boss.h   : マリーの定義
 *
 */

#ifndef CSTG4BOSS_INCLUDED
#define CSTG4BOSS_INCLUDED "マリーの定義 : Version 0.01 : Update 2001/04/05"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/05 : 製作開始
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CStg4Boss : public CCharacterBase {
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

	CStg4Boss();		// コンストラクタ
	~CStg4Boss();		// デストラクタ

public:
	FVOID MoveSmoke(ExtraShot *pShot);		// 煙を動作させる
	FVOID MoveBombSmoke(ExtraShot *pShot);	// ボム用の煙を動作させる
	FBOOL DeleteSmoke(ExtraShot *pShot);// 煙を消滅に向かわせる(TRUE:消滅完了)
	FVOID DrawMissile(ExtraShot *pShot);// ミサイルを描画する(煙含む)
	FVOID DrawShip(ExtraShot *pShot);	// ボム用乗り物を描画する
};



/***** [グローバル変数] *****/
extern CStg4Boss	g_Stg4BossInfo;		// マリーの情報格納用クラス



#endif
