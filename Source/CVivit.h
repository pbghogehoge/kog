/*
 *   CVivit.h   : びびっとさんの定義
 *
 */

#ifndef CVIVIT_INCLUDED
#define CVIVIT_INCLUDED "びびっとさんの定義 : Version 0.01 : Update 2001/02/28"

/*  [更新履歴]
 *    Version 0.01 : 2001/02/28 : 制作開始
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/
class CVivit : public CCharacterBase {
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

	CVivit();		// コンストラクタ
	~CVivit();		// デストラクタ


private:
	static FVOID DrawSmallStar(ExtraOption *pOpt);	// 小さい「お星様」の描画
	static FVOID DrawBombStar(ExtraOption *pOpt);	// ボム用お星様描画
	static FVOID DrawExBomb(ExtraOption *pOpt);		// ボム用爆発描画
};



/***** [グローバル変数] *****/
extern CVivit	g_VivitInfo;			// 主人公の情報格納用クラス



#endif
