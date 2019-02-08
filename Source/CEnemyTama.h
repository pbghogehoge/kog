/*
 *   CEnemyTama.h   : 敵弾処理
 *
 */

#ifndef ENEMYTAMA_INCLUDED
#define ENEMYTAMA_INCLUDED "敵弾処理     : Version 0.06 : Update 2001/07/17"

/*  [更新履歴]
 *    Version 0.06 : 2001/07/17 : TOPT_REFX2 の追加＆バグフィックス
 *                              : クリッピング属性を Option メンバから
 *                              : IsClipped メンバに変更
 *
 *    Version 0.05 : 2001/04/08 : 当たり判定の戻り値を FBOOL -> FDWORD に変更
 *                              : 受けたダメージを返すことにした
 *
 *    Version 0.04 : 2001/03/10 : 消去エフェクトが完成
 *    Version 0.03 : 2001/03/08 : 消去エフェクトを除いて完成
 *    Version 0.02 : 2001/03/07 : 描画系統の追加＆弾の種類を決定
 *    Version 0.01 : 2001/03/05 : 製作開始
 */



#include "Gian2001.h"
#include "EAtkCtrl.h"
#include "CShaveEffect.h"	// カスりエフェクト管理
#include "Tama.h"				// 弾データ構造体



/***** [ 定数 ] *****/

// 最大値 //
#define ENEMYTAMA_KIND		16		// 敵弾の種類
#define NUM_TAMATASK		100		// 弾発動タスク最大数
#define ENEMYTAMA_MAX		1000	// 敵弾同時発生・最大数
#define ENEMYTAMA_MAXSIZE	32		// 敵弾の最大サイズ(非x256)


// 形状定数                                             //
// リストの挿入先は、４ビット右シフトする事で求められる //
#define ETAMA_DEL_NORMAL	0x00	// まる弾   消去
#define ETAMA_DEL_ANGLE		0x10	// 角度同期 消去
#define ETAMA_DEL_ANIME		0x20	// アニメーション：消去

#define ETAMA_NORMAL8		0x30	// ８×８ まる弾
#define ETAMA_ANGLE8		0x40	// ８×８ 角度同期
#define ETAMA_ANIME8		0x50	// ８×８ アニメーション

#define ETAMA_NORMAL16		0x60	// 16×16 まる弾
#define ETAMA_ANGLE16		0x70	// 16×16 角度同期
#define ETAMA_ANIME16		0x80	// 16×16 アニメーション

#define ETAMA_NORMAL24		0x90	// 24×24 まる弾
#define ETAMA_ANGLE24		0xA0	// 24×24 角度同期
#define ETAMA_ANIME24		0xB0	// 24×24 アニメーション

#define ETAMA_NORMAL32		0xC0	// 32×32 まる弾
#define ETAMA_ANGLE32		0xD0	// 32×32 角度同期
#define ETAMA_ANIME32		0xE0	// 32×32 アニメーション

#define ETAMA_SCORE			0xF0	// 得点状態



/***** [クラス定義] *****/

// 敵弾管理クラス //
class CEnemyTama : public CFixedLList<TamaData, ENEMYTAMA_KIND, ENEMYTAMA_MAX> {
public:
	// 敵弾をセットする(引数がポインタでないことに注意) //
	FVOID Set(AttackCommand Command);

	FVOID Initialize(void);		// 初期化する

	__int64 Clear(BOOL bChgScore);	// 敵弾全てに消去エフェクトをセットする
	FVOID   Move(void);				// 弾を移動させる
	FVOID   Draw(void);				// 弾を描画する

	// 当たり判定を行う               //
	// pShave  : カスった回数の格納先 //
	// pDamage : ダメージ総量の格納先 //
	FVOID HitCheck(DWORD *pShave, DWORD *pDamage);

	// 弾消しボックスとの当たり判定を行う //
	// ox, oy : 中心座標                  //
	// width  : 幅                        //
	// height : 高さ                      //
	FVOID ClearFromRect(int ox, int oy, int width, int height);

	// 弾消し８角形との当たり判定を行う        //
	// ox, oy : 中心座標                       //
	// r      : ８角形を含む正方形の一辺の長さ //
	FVOID ClearFromOct(int ox, int oy, int r);


	// コンストラクタ //
	CEnemyTama(RECT	*rcTargetX256			// 対象矩形
			 , int	*pX						// 当たり判定Ｘ座標へのポインタ
			 , int	*pY						// 当たり判定Ｙ座標へのポインタ
			 , CShaveEffect *pShaveEfc);	// カスりエフェクト発動用クラス

	~CEnemyTama();	// デストラクタ


private:
	// 難易度による書き換え //
	static IVOID EasyCommand(AttackCommand *pCmd);	// 難易度 Easy 用にセット
	static IVOID HardCommand(AttackCommand *pCmd);	// 難易度 Hard 用にセット
	static IVOID LunaCommand(AttackCommand *pCmd);	// 難易度 Luna 用にセット


	// 弾の描画サイズを読み込む //
	static IBYTE TamaSize(BYTE Color);


	// 発射角セット                                                   //
	// 引数  pCmd : 弾コマンド構造体へのポインタ                      //
	//         i  : 角度方向で何番目の弾か(i < (pCmd->Num))           //
	// 戻り値     : セットすべき角度                                  //
	// 参考：ちなみに、そのコマンドにより変化を起こす角度を返すので、 //
	//       基本角などは呼び出し側で求めておく必要がある             //
	static BYTE TamaAngleWay(AttackCommand *pCmd, int i);	// 扇状発射
	static BYTE TamaAngleAll(AttackCommand *pCmd, int i);	// 全方向発射
	static BYTE TamaAngleRnd(AttackCommand *pCmd, int i);	// ランダム方向発射
	static BYTE TamaAngleOne(AttackCommand *pCmd, int i);	// 一方向発射


	// 速度セット //
	// 引数  pCmd : 弾コマンド構造体へのポインタ              //
	//         n  : 連弾方向で何番目の弾か(n < (pCmd->NumS))  //
	// 戻り値     : セットすべき速度                          //
	static int TamaSpeedStd(AttackCommand *pCmd, int n);	// 速度標準
	static int TamaSpeedRnd(AttackCommand *pCmd, int n);	// 速度ランダム付き


	// 移動を行う //
	IVOID MoveDefault(TamaData *pTama);	// 通常の移動
	IVOID MoveOption (TamaData *pTama);	// オプションによる移動


	// 当たり判定の実行を行う                               //
	// 引数 ID         : 敵弾ＩＤ(>> 4 によるリスト指定用)  //
	//      DeleteID   : 敵弾の消去時の格納先               //
	//      Damage     : １個あたりのダメージ量             //
	//      ShaveWidth : 当たり判定の幅                     //
	//      Shave      : カスった回数                       //
	//      Hit        : ダメージ                           //
	// 参考 : pShave, pHit は呼び出し側で初期化しておくこと //
	IVOID CheckETHit(DWORD ID, DWORD DeleteID, DWORD Damage, DWORD ShaveWidth, DWORD *pShave, DWORD *pHit);

	// 矩形・敵弾消去を実行する         //
	// ID, DeleteID : CheckETHit に同じ //
	// ox, oy       : 矩形の中心        //
	// w            : 矩形の幅          //
	// h            : 矩形の高さ        //
	IVOID ETClearRect(DWORD ID, DWORD DeleteID, int ox, int oy, int w, int h);

	// 八角形・敵弾消去を実行する                    //
	// ID, DeleteID : CheckETHit に同じ              //
	// ox, oy       : 矩形の中心                     //
	// r            : ８角形を含む正方形の一辺の長さ //
	IVOID ETClearOct(DWORD ID, DWORD DeleteID, int ox, int oy, int r);

	// 描画を行う                                             //
	// 消去系は１回しか呼ばれないのでインライン関数           //
	// 通常状態のものは複数回呼ばれるので fastcall で呼び出し //
	IVOID DrawDelNormal(void);	// まる弾(消去中)の描画を行う
	IVOID DrawDelAngle(void);	// 角度同期弾(消去中)の描画を行う
	IVOID DrawDelAnime(void);	// アニメ弾(消去中)の描画を行う
	FVOID DrawNormal(DWORD ID, int Size);	// まる弾の描画を行う
	FVOID DrawAngle (DWORD ID, int Size);	// 角度同期弾の描画を行う
	FVOID DrawAnime (DWORD ID, int Size);	// アニメ弾の描画を行う

	// 頂点バッファとテクスチャ座標を初期化する //
	// ちなみに VERTEX BUFFER の事では無い      //
	void InitializeBuffer(void);

	// 頂点バッファの準備＆描画 //
	static FVOID InitRList(int Size);	// 頂点データに格納する準備する
	static FVOID DrawRListStd(void);	// 頂点リストの内容を描画(通常)
	static FVOID DrawRListDel(void);	// 頂点リストの内容を描画(消去)

	// 頂点バッファに弾データを追加する                  //
	// 引数  ox, oy : 弾の中心座標                       //
	//       UV_ID  : ＵＶ座標指定用配列のインデックス値 //
	//       d      : 回転角(回転アニメにも使用する)     //
	//       it     : 消去アニメ時に使用する             //
	static IVOID InsertRList(int ox, int oy, BYTE UV_ID);
	static IVOID InsertRList(int ox, int oy, BYTE UV_ID, BYTE d);
	static IVOID InsertRList(Iterator &it, BYTE d);


	// 当たり判定対象 //
	int		*m_pX;		// 当たり判定対象(Ｘ座標)へのポインタ
	int		*m_pY;		// 当たり判定対象(Ｙ座標)へのポインタ

	// 画面外判定用 //
	int		m_XMin;		// 左端の座標
	int		m_YMin;		// 上端の座標
	int		m_XMax;		// 右端の座標
	int		m_YMax;		// 下端の座標

	int		m_RefXMin;		// 左端の反射座標
	int		m_RefYMin;		// 上端の反射座標
	int		m_RefXMax;		// 右端の反射座標
	int		m_RefYMax;		// 下端の反射座標


	// エフェクト関連 //
	CShaveEffect	*m_pShaveEfc;


	// 描画用バッファ                                         //
	// バッファのサイズは、敵弾数×６となる                   //
	// これは、敵弾一個一個を TRIANGLE_LIST で描画するためで、//
	// 敵弾一個に２トライアングル、６頂点が必要となる         //
	#define ENEMYTAMA_BUFFERSIZE	(ENEMYTAMA_MAX * 6)
	static D3DTLVERTEX				m_RenderList[ENEMYTAMA_BUFFERSIZE];

	static D3DTLVERTEX		*m_pRLTarget;	// 現在の頂点データ挿入先
	static int				m_RLSize;		// 現在の追加ポリゴンのサイズ

	// テクスチャ座標の格納先 //
	static D3DRECTANGLE		m_TextureUV[256];

	// バッファが敵弾用に初期化されたなら、TRUE になる          //
	// すなわち、最初にコンストラクタが呼び出された時に真となる //
	static BOOL m_BufferInitialized;
};



#endif
