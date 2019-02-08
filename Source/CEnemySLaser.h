/*
 *   CEnemySLaser.h   : 敵用ショートレーザー
 *
 */

#ifndef CENEMYSLASER_INCLUDED
#define CENEMYSLASER_INCLUDED "敵用ショートレーザー : Version 0.01 : Update 2001/04/01"

/*  [更新履歴]
 *    Version 0.01 : 2001/04/01 : 制作開始
 */



#include "Gian2001.h"
#include "CShaveEffect.h"
#include "EAtkCtrl.h"
#include "Laser.h"
#include "CEnemyLLaser.h"	// ふとれざ



/***** [ 定数 ] *****/

// 最大値 //
#define ESLASER_KIND	2			// レーザーの種類
#define ESLASER_MAX		500			// レーザーの同時発生最大数


// 状態定数 //
#define ESLASER_NORMAL		0x00	// 通常の状態
#define ESLASER_DELETE		0x01	// 消去中



/***** [クラス定義] *****/

// ショートレーザー管理クラス //
class CEnemySLaser : public CFixedLList<SLaserData, ESLASER_KIND, ESLASER_MAX> {
public:
	// レーザーをセットする(引数がポインタでないことに注意) //
	FVOID Set(AttackCommand Command);

	FVOID  Initialize(void);	// 初期化する

	FVOID  Clear(void);			// レーザー全てに消去エフェクトをセットする
	FVOID  Move(void);			// レーザーを移動させる
	FVOID  Draw(void);			// レーザーを描画する

	// 当たり判定を行う //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// コンストラクタ //
	CEnemySLaser(RECT			*rcTargetX256	// 対象となる矩形
			   , int			*pX				// 自機のＸ座標へのポインタ
			   , int			*pY				// 自機のＹ座標へのポインタ
			   , CShaveEffect	*pShaveEfc		// カスりエフェクト発動用クラス
			   , CEnemyLLaser	*pLLaser);		// ふとれざ管理

	// デストラクタ //
	~CEnemySLaser();


private:
	// レーザー１本を消去状態のために初期化する //
	IVOID SetClearLaserState(Iterator &it);

	// 難易度による書き換え //
	static IVOID EasyCommand(AttackCommand *pCmd);	// 難易度 Easy 用にセット
	static IVOID HardCommand(AttackCommand *pCmd);	// 難易度 Hard 用にセット
	static IVOID LunaCommand(AttackCommand *pCmd);	// 難易度 Luna 用にセット

	// 通常のレーザー動作関数 //
	IVOID NormLaserMove(SLaserData *pLaser);	// 移動処理

	// 反射レーザー専用関数 //
	IBOOL RefLaserMove(SLaserData *pLaser);		// 移動処理
	IBOOL RefLaserHitCheck(SLaserData *pLaser);	// リフレクターとの当たり判定


	// 発射角セット                                                   //
	// 引数  pCmd : 弾コマンド構造体へのポインタ                      //
	//         i  : 角度方向で何番目の弾か(i < (pCmd->Num))           //
	// 戻り値     : セットすべき角度                                  //
	// 参考：ちなみに、そのコマンドにより変化を起こす角度を返すので、 //
	//       基本角などは呼び出し側で求めておく必要がある             //
	static char LaserAngleWay(AttackCommand *pCmd, int i);	// 扇状発射
	static char LaserAngleAll(AttackCommand *pCmd, int i);	// 全方向発射
	static char LaserAngleRnd(AttackCommand *pCmd, int i);	// ランダム方向発射
	static char LaserAngleOne(AttackCommand *pCmd, int i);	// 一方向発射


	// 速度セット //
	// 引数  pCmd : 弾コマンド構造体へのポインタ              //
	// 戻り値     : セットすべき速度                          //
	static int LaserSpeedStd(AttackCommand *pCmd);	// 速度標準
	static int LaserSpeedRnd(AttackCommand *pCmd);	// 速度ランダム付き


	// 頂点バッファとテクスチャ座標を初期化する //
	// ちなみに VERTEX BUFFER の事では無い      //
	void InitializeBuffer(void);

	// 頂点バッファの準備＆描画 //
	static IVOID InitRList(void);		// 頂点データに格納する準備する
	static IVOID DrawRListStd(void);	// 描画する(通常状態)
	static IVOID DrawRListDel(void);	// 描画する(消去用)

	// 尻尾のまる球体を描画する //
	static IVOID DrawSLTail(int x, int y, int w, BYTE c);

	// 頂点バッファに弾データを追加する                  //
	// 引数  ox, oy : 弾の中心座標                       //
	//        l256  : レーザーの長さ(x256)               //
	//          c   : レーザーの色                       //
	//          d   : レーザーの角度                     //
	//        Count : カウンタ                           //
	static IVOID InsertRList(int ox, int oy, int l256, BYTE c, BYTE d);
	static IVOID InsertRList(int ox, int oy, int l256, BYTE c, BYTE d, DWORD Count);


private:
	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標

	// エフェクト関連 //
	CShaveEffect	*m_pShaveEfc;

	// ふとれざ管理 //
	CEnemyLLaser	*m_pLLaser;


	// 描画用バッファ                                         //
	// バッファのサイズは、レーザー数×12となる               //
	// これは、敵弾一個一個を TRIANGLE_LIST で描画するためで、//
	// 敵弾一個に４トライアングル、12頂点が必要となる         //
	#define ESLASER_BUFFERSIZE		(ESLASER_MAX * 12)
	static D3DTLVERTEX		m_RenderList[ESLASER_BUFFERSIZE];

	static D3DTLVERTEX		*m_pRLTarget;	// 現在の頂点データ挿入先

	// バッファが敵弾用に初期化されたなら、TRUE になる          //
	// すなわち、最初にコンストラクタが呼び出された時に真となる //
	static BOOL m_BufferInitialized;
};



#endif
