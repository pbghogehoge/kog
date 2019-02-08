/*
 *   CYuka.h   : 幽香の定義
 *
 */

#ifndef CYUKA_INCLUDED
#define CYUKA_INCLUDED "幽香の定義   : Version 0.01 : Update 2001/11/30"

/*  [更新履歴]
 *    Version 0.01 : 2001/11/30 : 制作開始
 */



#include "CCharacterBase.h"



/***** [クラス定義] *****/

class CYuka : public CCharacterBase {
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
/*
	FVOID InitAtkGrp(AtkGrpInfo *pInfo, DWORD AtkLv);	// 初期化
	FBOOL Lv2AtkGrpMove(AtkGrpInfo *pInfo);				// レベル２
	FBOOL Lv3AtkGrpMove(AtkGrpInfo *pInfo);				// レベル３
	FBOOL Lv4AtkGrpMove(AtkGrpInfo *pInfo);				// レベル４
*/

	CYuka();		// コンストラクタ
	~CYuka();		// デストラクタ


private:
	FVOID DrawFlowerShot(ExtraShot *pExShot, BYTE a);
	FVOID DrawFlowerBomb(ExtraShot *pExShot, BYTE a);
};



/***** [グローバル変数] *****/
extern CYuka	g_YukaInfo;			// 幽香の情報格納用クラス



#endif
