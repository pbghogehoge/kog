/*
 *   CEnemyAnime.h   : 敵のアニメーション定義
 *
 */

#ifndef CENEMYANIME_INCLUDED
#define CENEMYANIME_INCLUDED "敵のアニメーション定義 : Version 0.01 : Update 2001/03/27"

/*  [更新履歴]
 *    Version 0.01 : 2001/03/27 : 制作開始
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/

// 無条件テクスチャロード //
#define TEX_REG_ENEMY		0x00	// 敵テクスチャ
//#define TEX_REG_ENEMY2		0x01	// 敵テクスチャ２番
//#define TEX_REG_ENEMY3		0x02	// 敵テクスチャ３番
//#define TEX_REG_ENEMY4		0x03	// 敵テクスチャ４番

// 条件付きテクスチャロード //
#define TEX_REG_VIVIT		0x01	// 条件テクスチャ(VIVIT)
#define TEX_REG_STG1BOSS	0x02	// 条件テクスチャ(ミリア)
#define TEX_REG_STG2BOSS	0x03	// 条件テクスチャ(めい＆まい)
#define TEX_REG_STG3BOSS	0x04	// 条件テクスチャ(ゲイツ)
#define TEX_REG_STG4BOSS	0x05	// 条件テクスチャ(マリー)
#define TEX_REG_STG5BOSS	0x06	// 条件テクスチャ(エーリッヒ)
#define TEX_REG_MORGAN		0x07	// 条件テクスチャ(エーリッヒ)
#define TEX_REG_MUSE		0x08	// 条件テクスチャ(エーリッヒ)
#define TEX_REG_BG			0x09	// 背景用テクスチャ
#define TEX_REG_YUKA		0x0a	// 条件テクスチャ(幽香)


// 最大数 //
#define EANIME_MAX			256		// 敵のアニメーションの種類
#define EANIMEPTN_MAX		32		// 敵ののアニメーションパターン数
#define TEX_NUMREGS			11		// ＴＥＸレジスタ数



/***** [クラス定義] *****/

// 相互参照のため... //
class CEnemyAnimeContainer;



// 描画情報格納用構造体(継承してるけど..) //
typedef struct tagCEADrawInfo : public D3DRECTANGLE {
	int		HalfWidth;		//  幅  / 2
	int		HalfHeight;		// 高さ / 2
} EADrawInfo;


// アニメーションパターン定義用クラス //
class EAnimePtn {
	friend class CEnemyAnimeContainer;

public:
	// Ptn 番目のパターンを Angle だけ回転させて描画する //
	// ちなみに、下方向に移動中なら Angle = 64           //
	FVOID DrawNormal(int x, int y, BYTE Angle, BYTE Ptn, BYTE a);	// 通常
	FVOID DrawDamage(int x, int y, BYTE Angle, BYTE Ptn, BYTE a);	// ダメージ時
	FVOID DrawDestroy(int x, int y, BYTE Angle, BYTE Ptn, BYTE a);	// 消去

	// 次のアニメーションＩＤを取得する //
	IBYTE GetNextPtn(BYTE CurrentPtn, int Direction){
		if(Direction > 0){
			if(CurrentPtn >= m_NumPtn-1){
				if(m_bStop) return m_NumPtn - 1;
				else        return 0;
			}

			return CurrentPtn + 1;
		}
		if(Direction < 0){
			if(0 == CurrentPtn){
				if(m_bStop) return 0;
				else        return m_NumPtn - 1;
			}

			return CurrentPtn - 1;
		}

		return CurrentPtn;
	};

	// 当たり判定サイズ(x256) を返す //
	IINT GetHitSize(BYTE Ptn){
		if(NULL == this) return 0;

		EADrawInfo *Info = m_DrawInfo + Ptn;
		return min(Info->HalfWidth, Info->HalfHeight);
	}

	// 消去判定幅(x256)を返す //
	IINT GetClipSize(BYTE Ptn){
		if(NULL == this) return 0;

		EADrawInfo *Info = m_DrawInfo + Ptn;
		return (Info->HalfWidth + Info->HalfHeight);
	}

	EAnimePtn();		// コンストラクタ
	~EAnimePtn();		// デストラクタ


private:
	FVOID Initialize(void);		// 初期化を行う

	// 敵描画用の頂点データをセットする (ptlv は要素数６以上) //
	BOOL SetTLV(D3DTLVERTEX	*pVertex	// 頂点バッファ
			  , int			x			// 描画対象Ｘ座標
			  , int			y			// 描画対象Ｙ座標
			  , BYTE		Angle		// 角度
			  , BYTE		Ptn			// アニメーションパターン
			  , BYTE		Alpha);		// α

	// 敵描画用の頂点データをセットする (ptlv は要素数６以上) //
	BOOL SetTLV_One(D3DTLVERTEX	*pVertex	// 頂点バッファ
				  , int			x			// 描画対象Ｘ座標
				  , int			y			// 描画対象Ｙ座標
				  , BYTE		Angle		// 角度
				  , BYTE		Ptn			// アニメーションパターン
				  , BYTE		Alpha);		// α


	int				m_TextureID;				// テクスチャＩＤ
	int				m_NumPtn;					// アニメーションパターン数
	EADrawInfo		m_DrawInfo[EANIMEPTN_MAX];	// テクスチャのＵＶ座標
	BOOL			m_bStop;					// 最終コマで停止するなら真
};


// アニメーション管理クラス //
class CEnemyAnimeContainer {
public:
	// テクスチャ情報をロードする //
	static BOOL Load(BYTE *pBuffer			// Ｔｅｘプロシージャ開始アドレス
				   , CHARACTER_ID Player1_ID	// プレイヤー１のＩＤ
				   , CHARACTER_ID Player2_ID);	// プレイヤー２のＩＤ

	// テクスチャ情報をロードする //
	static BOOL LoadBA(BYTE *pBuffer		// Ｔｅｘプロシージャ開始アドレス
					, CHARACTER_ID Player1_ID);

	// アニメＩＤから EnemyAnime へのポインタに変換する //
	static EAnimePtn *GetPointer(int ID){
		return m_Buffer + ID;
	};


	CEnemyAnimeContainer();		// コンストラクタ
	~CEnemyAnimeContainer();		// デストラクタ


private:
	// 実際にテクスチャを読み込む(次の命令へのアドレスを返す) //
	static BYTE *ParseLoadTexture(BYTE *pBuffer, int *pTextureID);

	// テクスチャＩＤを指定する(条件付きテクスチャ) //
	static BYTE *ParseLoadExtraTexture(BYTE *pBuffer, int TextureID);

	// LOAD ブロックの読み飛ばしを行う //
	static BYTE *SkipLoadBlock(BYTE *pBuffer);

//	// LOAD2 命令を解析する //
//	static BYTE *ParseLoad2(BYTE *pBuffer, CHARACTER_ID P1, CHARACTER_ID P2);

	// テクスチャＩＤを取得する                             //
	//                                                      //
	// arg  CharID   : キャラクタＩＤ(CHARACTER_VIVIT, ...) //
	//      PlayerID : プレイヤーＩＤ(1 or 2)               //
	//      pScript  : 現在のスクリプト読み込み位置         //
	//                                                      //
	// ret  -1 なら失敗、それ以外なら、テクスチャＩＤ       //
	static FINT CEnemyAnimeContainer::GetTextureID(
							CHARACTER_ID	CharID		// キャラクタＩＤ
						,	int				PlayerID	// プレイヤーＩＤ
						,	BYTE			*pScript);	// テクスチャの格納対象


	// レジスタ番号からテクスチャＩＤを取得 //
	static CHARACTER_ID TexRegs2ID(BYTE TexRegs);


	// アニメーションパターンの格納先 //
	static EAnimePtn	m_Buffer[EANIME_MAX];
};



#endif
