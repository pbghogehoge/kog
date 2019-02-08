/*
 *   SCL.h   : 敵配置スクリプト
 *
 */

#ifndef SCL_INCLUDED
#define SCL_INCLUDED "敵配置スクリプト : Version 0.04 : Update 2001/09/24"

/*  [更新履歴]
 *    Version 0.04 : 2001/09/24 : アニメーション定義系の追加
 *    Version 0.03 : 2001/04/05 : プレイヤー定義系の追加・修正
 *    Version 0.02 : 2001/03/14 : デコーダの実装
 *    Version 0.01 : 2001/03/13 : 製作開始
 */



#include "PbgType.h"
#include "CEnemyCtrl.h"



/***** [ 定数 ] *****/
#define SCL_NUMREGS			0		// ＳＣＬ命令のレジスタ本数
#define NUM_CHARACTERS		10		// キャラクタの最大数
#define SCLBUFFER_SIZE		50		// ＳＣＬ配置用バッファのサイズ
//#define LOADTEXTURE_MAX		8		// 同時ロードできるテクスチャの最大枚数



/***** [クラス定義] *****/

// テクスチャロード命令のエントリポイント集 //
typedef struct tagLoadTextureEntry {
	int		EntryPoint[LOADTEXTURE_MAX];	// エントリポイント
	int		NumTextures;					// テクスチャ枚数
} LoadTextureEntry;


// ＳＣＬヘッダ //
typedef struct tagSCLHeader {
	// 編隊の定義数 //
	int NumLv1SCL;		// Level １ ＳＣＬ定義数
	int NumLv2SCL;		// Level ２ ＳＣＬ定義数
	int NumLv3SCL;		// Level ３ ＳＣＬ定義数
	int NumLv4SCL;		// Level ４ ＳＣＬ定義数

	// テクスチャ初期化ブロックの開始アドレス //
	int TexInitializer;

	// プレイヤー初期化用テーブル //
	int Lv1Attack[NUM_CHARACTERS];	// Ｌｅｖｅｌ１アタック
	int Lv2Attack[NUM_CHARACTERS];	// Ｌｅｖｅｌ２アタック
	int BossAddr [NUM_CHARACTERS];	// ボス定義の開始アドレス
	int ComboAddr[NUM_CHARACTERS];	// コンボアタックの開始アドレス

	// アニメーション定義テーブル //
	int ExAnmLv1[NUM_CHARACTERS];	// Ｌｅｖｅｌ１アタック用アニメ
	int ExAnmLv2[NUM_CHARACTERS];	// Ｌｅｖｅｌ２アタック用アニメ
	int ExAnmBoss[NUM_CHARACTERS];	// ボスアタック用アニメ
	int ExAnmWin[NUM_CHARACTERS];	// 勝った時のアニメ

	// 編隊定義用テーブル //
	int SCL_Lv1[SCLBUFFER_SIZE];	// Ｌｅｖｅｌ１ 編隊定義
	int SCL_Lv2[SCLBUFFER_SIZE];	// Ｌｅｖｅｌ２ 編隊定義
	int SCL_Lv3[SCLBUFFER_SIZE];	// Ｌｅｖｅｌ３ 編隊定義
	int SCL_Lv4[SCLBUFFER_SIZE];	// Ｌｅｖｅｌ４ 編隊定義

	// テクスチャロードのエントリポイント //
	LoadTextureEntry	LTEntry[NUM_CHARACTERS];
} SCLHeader;


// ＳＣＬ展開 //
class CSCLDecoder {
public:
	virtual BOOL Initialize(int Level);	// バッファ内のデータをリセットする
	virtual void Proceed(void);			// １フレーム分バッファを進める

	// 相手から送られてきた攻撃をセットする //
	void SetExAttackLv1(BYTE PlayerID, BYTE AtkLv);	// Lv 1
	void SetExAttackLv2(BYTE PlayerID, BYTE AtkLv);	// Lv 2
	void SetBossAttack(BYTE PlayerID, BYTE AtkLv);	// Boss
	void SetComboAttack(BYTE PlayerID);				// Combo

	// 現在のカウンタ値を返す //
	IINT GetCurrentCount(void){
		return m_Count;
	}

	// 条件ロードテクスチャのエントリポイントを取得する //
	static LoadTextureEntry *GetTextureEntry(int CharID);

	// アニメーション定義の開始位置を取得する //
	static BYTE *GetExAnmAddr(int CharID, BYTE Level);

	// ＳＣＬデータをロードする //
	static BOOL Load(char			*FileID		// ファイルＩＤ
				   , CHARACTER_ID	P1			// プレイヤー１の使用キャラ
				   , CHARACTER_ID	P2);		// プレイヤー２の使用キャラ

	// ＳＣＬの敵配置テーブルをシャッフルする //
	static FBOOL ShuffleSCLData(Pbg::CRnd *pRnd);

	// オフセットからポインタを取得 //
	static IBYTE *Offset2Ptr(DWORD Offset){
		return ((BYTE *)m_pDataBuffer) + Offset;
	};

	// ポインタからオフセットを取得 //
	static IINT Ptr2Offset(BYTE *pCmd){
		return (pCmd - (BYTE *)m_pDataBuffer);
	};

	BOOL IsBossAlive(void){
		return m_pEnemyCtrl->IsBossAlive();
	}

	CSCLDecoder(RECT *rcTargetX256, CEnemyCtrl *pEnemyCtrl);// コンストラクタ
	virtual ~CSCLDecoder();									// デストラクタ


protected:
	static FBOOL LoadWithoutTexture(char *pFileID);		// ファイルＩＤ


private:
	// 次の編隊ブロックに進む //
	FVOID JumpNextBuffer(void);


	BYTE		*m_pCurrentSCL;		// 現在のＳＣＬアドレス
	DWORD		m_NopCount;			// ＮＯＰ命令の待ち時間
	CEnemyCtrl	*m_pEnemyCtrl;		// 敵管理クラス

	int			m_ox;		// 中心のＸ座標
	int			m_oy;		// 中心のＹ座標

	int			m_Level;	// 現在のレベル
	int			m_Target;	// SCL テーブルのインデックス値
	int			m_Count;	// カウンタ(Proceed 呼び出しの度に１だけ増える)

	static DWORD		m_RefCount;		// 参照数(０になるとデータバッファ解放)
	static SCLHeader	*m_pDataBuffer;	// データ格納地点(プレイヤー間で共有)

	static int			m_Level1Exit;	// Level 1 脱出のタイミング
	static int			m_Level2Exit;	// Level 2 脱出のタイミング
	static int			m_Level3Exit;	// Level 3 脱出のタイミング
};



#endif
