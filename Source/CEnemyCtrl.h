/*
 *   CEnemyCtrl.h   : 敵管理クラス
 *
 */

#ifndef CENEMYCTRL_INCLUDED
#define CENEMYCTRL_INCLUDED "敵管理クラス : Version 0.04 : Update 2001/04/22"

/*  [注意事項]
 *  　ボスの管理については、このクラスで扱わないこと！
 *    これは、敵が最大数を超えた場合でもボスは発生できる、という条件を満たすためである。
 */

/*  [更新履歴]
 *    Version 0.05 : 2001/09/14 : Tama & Laser の所有権を持つようになった
 *    Version 0.04 : 2001/04/22 : 破片処理の追加
 *    Version 0.03 : 2001/04/01 : ショートレーザー管理の追加
 *    Version 0.02 : 2001/03/25 : 枠組みの開発
 *    Version 0.01 : 2001/03/14 : クラス設計
 */



#include "Gian2001.h"
#include "Enemy.h"
#include "CBossCtrl.h"
#include "CEnemyTama.h"
#include "CEnemySLaser.h"
#include "CEnemyLLaser.h"
#include "CEnemyHLaser.h"
#include "CEnemyLSphere.h"
#include "CEnemyExShot.h"
#include "CFragmentEfc.h"
#include "CBGDraw.h"



/***** [ 定数 ] *****/

// 最大値 //
#define ENEMY_KIND		4				// 敵の状態数
#define ENEMY_MAX		128				// 敵同時発生・最大数
#define EATK_KIND		(ENEMY_MAX + 2)	// 攻撃オブジェクトの種類
#define EATK_MAX		1024			// 敵の攻撃管理・同時発生数

#define EATK_NOPARENT	ENEMY_MAX		// 攻撃オブジェクトＩＤ：親無し
#define EATK_BOSS		(ENEMY_MAX+1)	// 攻撃オブジェクトＩＤ：ボス


// 敵の状態 //
#define ENEMY_NORMAL		0x00	// 通常の雑魚
#define ENEMY_EXATK			0x01	// エキストラアタック用
#define ENEMY_EFFECT		0x02	// エフェクト用
#define ENEMY_DELETE		0x03	// 敵が爆発中！

// UpdateEnemyData() 用の戻り値 //
typedef enum tagUPDATE_ENEMYRET {
	ENRET_DELETE = 0,	// 消去すべし
	ENRET_OK     = 1,	// 通常の終了
	ENRET_KILL   = 2,	// ~0 のダメージを与える
} UPDATE_ENEMYRET;



/***** [クラス定義] *****/

// 敵管理クラス //
class CEnemyCtrl : public CFixedLList<EnemyData, ENEMY_KIND, ENEMY_MAX> {
public:
	// 親の存在しない敵をセットする(2001/09/22 : 挿入対象の追加) //
	FVOID SetEnemy(	int			x256		// 初期Ｘ座標
				,	int			y256		// 初期Ｙ座標
				,	DWORD		Offset		// 開始アドレス
				,	int			InitGr0		// 初期 Gr0
				,	BYTE		Target);	// 挿入対象(_NORMAL, ...)

	// 子となる雑魚を生成する //
	FVOID SetChild(	DWORD		Offset		// 開始アドレス
				,	EnemyData	*pParent	// 親データ
				,	int			InitGr0		// 初期 Gr0
				,	BYTE		ChildID		// 子オブジェクトＩＤ
				,	BYTE		Target);	// 挿入対象(_NORMAL, ...)

	// ボスをセットする(出現中なら、ＨＰ回復などなど) //
	FVOID SetBoss(DWORD Offset, int InitGr0);

	// ボスを強化する(青玉を出現させられる場合は真を返す) //
	FBOOL BossSendExp(DWORD nShave);

	// 指定された敵に強制的に KILL を実行させる(親との切り離しも行う) //
	FVOID KillChild(EnemyData *pEnemy);

	// 自分の全ての子に対して、 KillEnemy() を実行させる //
	FVOID KillAllChild(EnemyData *pParent);

	// 親に対して、自分が死んだことを報告する //
	FVOID ChildDeadNotify(EnemyData *pChild);

	// 敵弾コントロールオブジェクトを発生させる //
	FVOID SetAtkObject(EnemyData *pThis, int dx256, int dy256, BYTE *pAddr, DWORD Param);

	FVOID Initialize(void);	// 初期化する

	FVOID   Clear(void);					// 敵全てに消去エフェクトをセットする
	__int64 ClearAtkObj(BOOL bChgScore);	// 敵弾などに消去エフェクトをセットする
	FVOID   Move(void);						// 敵を移動させる
	FVOID   Draw(void);						// 敵を描画する
	FVOID   DrawAtkObj(void);				// 敵弾などの描画を行う
	FVOID   DrawMadnessGauge(void);			// マッドネスゲージの描画を行う

	// 通常の雑魚が生きていれば真を返す //
	FBOOL IsNomalEnemyAlive(void);

	// ヒット有効な敵に対して DamageCallback を呼び出し、//
	// 死亡した敵の合計スコアを返す                      //
	FDWORD EnumActiveEnemy(DamageCallback Callback, void *pParam);

	// 当たり判定を行う               //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// このクラスに関連づけられた敵弾クラスへのポインタを返す //
	inline CEnemyTama *GetEnemyTamaPtr(void){
		return &m_EnemyTama;
//		return m_pEnemyTama;
	};

	// このクラスに関連づけられたショートレーザークラスへのポインタを返す //
	inline CEnemySLaser *GetEnemySLaserPtr(void){
		return &m_EnemySLaser;
//		return m_pEnemySLaser;
	};

	// このクラスに関連づけられた太レーザークラスへのポインタを返す //
	inline CEnemyLLaser *GetEnemyLLaserPtr(void){
		return &m_EnemyLLaser;
//		return m_pEnemyLLaser;
	};

	// このクラスに関連づけられたホーミングレーザークラスへのポインタを返す //
	inline CEnemyHLaser *GetEnemyHLaserPtr(void){
		return &m_EnemyHLaser;
//		return m_pEnemyHLaser;
	};

	// 破片エフェクト管理へのポインタを返す //
	inline CFragmentEfc *GetFragmentEfcPtr(void){
		return m_pFragment;
	};

	// びりびり球体管理へのポインタを返す //
	inline CEnemyLSphere *GetEnemyLSphere(void){
		return &m_EnemyLSphere;
	};

	// 特殊ショット管理へのポインタを返す //
	inline CEnemyExShot *GetEnemyExShot(void){
		return &m_EnemyExShot;
	};

	// マッドネスゲージに対してダメージ(基本的にボス始動用) //
	inline void MadnessGaugeDamage(int Damage){
		m_BossData.OnDamage(Damage);
	};

	// 関連づけられた攻撃オブジェクトが存在すれば真を返す //
	BOOL IsExistAtkObj(EnemyData *pThis);

	// ボスが生きていれば真を返す //
	BOOL IsBossAlive(void){
		if(m_BossData.GetEnemyData()) return TRUE;
		else                          return FALSE;
	}

	// 関連づけられた当たり判定用座標の現在の値を返す //
	inline int GetTargetX(void){ return *m_pX;  };	// Ｘ座標
	inline int GetTargetY(void){ return *m_pY;  };	// Ｙ座標
	inline int GetMidX(void)   { return m_XMid; };	// Ｘ座標中心


	// コンストラクタ＆デストラクタ //
	CEnemyCtrl(RECT			*rcTargetX256	// 移動範囲矩形
			 , int			*pX				// 当たり判定ｘ座標
			 , int			*pY				// 当たり判定ｙ座標
			 , CFragmentEfc	*pFragment		// 破片管理クラス
			 , CShaveEffect	*pShaveEfc		// カスリエフェクト管理
			 , CTriEffect	*pTriEffect		// 破片エフェクト管理
			 , CBGDraw		*pBGDraw);		// 背景描画管理
	~CEnemyCtrl();


private:
	// 敵の初期化コア //
	IVOID InitEnemyCore(	EnemyData	*pEnemy		// 初期化対象
						, 	int			x256		// 初期Ｘ座標
						,	int			y256		// 初期Ｙ座標
						,	DWORD		Offset		// 開始アドレス
						,	EnemyData	*pParent	// 親データ(NULL : 親なし)
						,	int			InitGr0);	// 初期 Gr0

	// 敵データの更新を行う(範囲外チェックは含まない) //
	UPDATE_ENEMYRET UpdateEnemyData(EnemyData *pEnemy, BYTE Target);

	// 関連づけられた AtkCtrl を殺す //
	FVOID KillAtkCtrl(EnemyData *pEnemy);	// 雑魚用
	FVOID KillBossAtkCtrl(void);			// ボス用

	// 敵にダメージを与える(ret:Score) //
	FDWORD OnDamage(Iterator &ref_it, DWORD Damage);

	// ボス管理クラス //
	CBossCtrl		m_BossData;

	// 各種攻撃オブジェクト管理クラス //
	CEnemyTama		m_EnemyTama;		// 敵弾
	CEnemyLLaser	m_EnemyLLaser;		// 太レーザー
	CEnemySLaser	m_EnemySLaser;		// ショートレーザー
	CEnemyHLaser	m_EnemyHLaser;		// ホーミングレーザー
	CEnemyLSphere	m_EnemyLSphere;		// びりびり球体
	CEnemyExShot	m_EnemyExShot;		// その他特殊ショット

	// 破片管理クラス //
	CFragmentEfc	*m_pFragment;


	// 攻撃管理クラス                      //
	// 0..ENEMY_MAX-1 : 親付きオブジェクト //
	// ENEMY_MAX      : 親無しオブジェクト //
	// ENEMY_MAX+1    : ボス               //
	typedef CFixedLList<EnemyAtkCtrl, EATK_KIND, EATK_MAX> AttackList;
	AttackList		m_AtkCtrl;


	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標

	int		m_XMid;			// Ｘ座標中心
	int		m_YMid;			// Ｙ座標中心
	int		m_HalfWidth;	// 幅   / 2
	int		m_HalfHeight;	// 高さ / 2

	// マッドネスゲージ描画用矩形 //
	RECT	m_rcTarget;
};



#endif
