/*
 *   CEnemyAnime.cpp   : 敵のアニメーション定義
 *
 */

#include "CEnemyAnime.h"
#include "ScrBase.h"



/***** [グローバル変数] *****/

CEnemyAnimeContainer		g_DummyEnmeyAnime;



/***** [スタティックメンバ] *****/

// アニメーション定義の格納先 //
EAnimePtn CEnemyAnimeContainer::m_Buffer[EANIME_MAX];



// コンストラクタ //
EAnimePtn::EAnimePtn()
{
	Initialize();
}


// デストラクタ //
EAnimePtn::~EAnimePtn()
{
	// 何もしないです //
}


// 敵描画用の頂点データをセットする (ptlv は要素数６以上) //
BOOL EAnimePtn::
	SetTLV(D3DTLVERTEX	*pVertex	// 頂点バッファ
		 , int			x			// 描画対象Ｘ座標
		 , int			y			// 描画対象Ｙ座標
		 , BYTE			Angle		// 角度
		 , BYTE			Ptn			// アニメーションパターン
		 , BYTE			Alpha)		// α
{
	int				w, h;
	int				dx, dy;
	D3DVALUE		u1, v1, u2, v2;
	EADrawInfo		*pInfo;


	// this ポインタが NULL なら描画する必要はない //
	if(NULL == this) return FALSE;

#ifdef PBG_DEBUG
	if(m_TextureID < 0){
		PbgError("初期化されていないアニメーションパターンの参照");
		return FALSE;
	}
#endif

	// 描画用の情報をローカル変数に納める //
	pInfo = m_DrawInfo + Ptn;	// 対象となるＵＶ情報を取得
	w     = pInfo->HalfWidth;	// 幅を取得
	h     = pInfo->HalfHeight;	// 高さを取得
	u1    = pInfo->m_Left;		// テクスチャの左
	v1    = pInfo->m_Top;		// テクスチャの上
	u2    = pInfo->m_Right;		// テクスチャの右
	v2    = pInfo->m_Bottom;	// テクスチャの下


	// 現在は、仮実装なので、高速化は追求しない                        //
	// ちなみに、注釈は、Angle = 64 の時にどの方向にあるかを示している //

	// 左上に向かうベクトルを求める //
	dx = (CosL(Angle+128, w) + CosL(Angle-64, h)) >> 8;	// ｘ方向の成分
	dy = (SinL(Angle+128, w) + SinL(Angle-64, h)) >> 8;	// ｙ方向の成分
	Set2DPointC(pVertex+0, x+dx, y+dy, u1, v1, RGBA_MAKE(255, 255, 255, Alpha));	// 左上①
	Set2DPointC(pVertex+2, x-dx, y-dy, u2, v2, RGBA_MAKE(255, 255, 255, Alpha));	// 右下①

	// 右上に向かうベクトルを求める //
	dx = (CosL(Angle, w) + CosL(Angle-64, h)) >> 8;	// ｘ方向の成分
	dy = (SinL(Angle, w) + SinL(Angle-64, h)) >> 8;	// ｙ方向の成分
	Set2DPointC(pVertex+1, x+dx, y+dy, u2, v1, RGBA_MAKE(255, 255, 255, Alpha));	// 右上
	Set2DPointC(pVertex+3, x-dx, y-dy, u1, v2, RGBA_MAKE(255, 255, 255, Alpha));	// 左下

	return TRUE;
}


// 敵描画用の頂点データをセットする (ptlv は要素数６以上) //
BOOL EAnimePtn::
	SetTLV_One(D3DTLVERTEX	*pVertex	// 頂点バッファ
			 , int			x			// 描画対象Ｘ座標
			 , int			y			// 描画対象Ｙ座標
			 , BYTE			Angle		// 角度
			 , BYTE			Ptn			// アニメーションパターン
			 , BYTE			Alpha)		// α
{
	int				w, h;
	int				dx, dy;
	D3DVALUE		u1, v1, u2, v2;
	EADrawInfo		*pInfo;


	// this ポインタが NULL なら描画する必要はない //
	if(NULL == this) return FALSE;

#ifdef PBG_DEBUG
	if(m_TextureID < 0){
		PbgError("初期化されていないアニメーションパターンの参照");
		return FALSE;
	}
#endif

	// 描画用の情報をローカル変数に納める //
	pInfo = m_DrawInfo + Ptn;	// 対象となるＵＶ情報を取得
	w     = pInfo->HalfWidth;	// 幅を取得
	h     = pInfo->HalfHeight;	// 高さを取得
	u1    = pInfo->m_Left;		// テクスチャの左
	v1    = pInfo->m_Top;		// テクスチャの上
	u2    = pInfo->m_Right;		// テクスチャの右
	v2    = pInfo->m_Bottom;	// テクスチャの下


	// 現在は、仮実装なので、高速化は追求しない                        //
	// ちなみに、注釈は、Angle = 64 の時にどの方向にあるかを示している //

	// 左上に向かうベクトルを求める //
	dx = (CosL(Angle+128, w) + CosL(Angle-64, h)) >> 8;	// ｘ方向の成分
	dy = (SinL(Angle+128, w) + SinL(Angle-64, h)) >> 8;	// ｙ方向の成分
	Set2DPointC(pVertex+0, x+dx, y+dy, u1, v1, RGBA_MAKE(Alpha, Alpha, Alpha, 255));	// 左上①
	Set2DPointC(pVertex+2, x-dx, y-dy, u2, v2, RGBA_MAKE(Alpha, Alpha, Alpha, 255));	// 右下①

	// 右上に向かうベクトルを求める //
	dx = (CosL(Angle, w) + CosL(Angle-64, h)) >> 8;	// ｘ方向の成分
	dy = (SinL(Angle, w) + SinL(Angle-64, h)) >> 8;	// ｙ方向の成分
	Set2DPointC(pVertex+1, x+dx, y+dy, u2, v1, RGBA_MAKE(Alpha, Alpha, Alpha, 255));	// 右上
	Set2DPointC(pVertex+3, x-dx, y-dy, u1, v2, RGBA_MAKE(Alpha, Alpha, Alpha, 255));	// 左下

	return TRUE;
}


// Ptn 番目のパターンを Angle だけ回転させて描画する //
// ちなみに、下方向に移動中なら Angle = 64           //
FVOID EAnimePtn::DrawNormal(int x, int y, BYTE Angle, BYTE Ptn, BYTE a)
{
	D3DTLVERTEX		Vertex[10];

	// 頂点をセット //
//	if(FALSE == SetTLV(Vertex, x, y, Angle, Ptn, 255)) return;
	if(FALSE == SetTLV(Vertex, x, y, Angle, Ptn, min(a, 240))) return;

	g_pGrp->SetTexture(m_TextureID);				// テクスチャセットして
//	g_pGrp->SetRenderStateEx(GRPST_COLORKEY);		// カラーキー付きの描画
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);		// カラーキー付きの描画
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, Vertex, 4);	//書き込み
}


// ダメージ時 //
FVOID EAnimePtn::DrawDamage(int x, int y, BYTE Angle, BYTE Ptn, BYTE a)
{
	D3DTLVERTEX		Vertex[10];

	// 頂点をセットする //
	if(FALSE == SetTLV_One(Vertex, x, y, Angle, Ptn, a)) return;

	// 描画 //
	g_pGrp->SetTexture(m_TextureID);				// テクスチャセットして
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);		// カラーキー付きの描画
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, Vertex, 4);	//書き込み
//	g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, Vertex, 6);	//書き込み
}


// 消去 //
FVOID EAnimePtn::DrawDestroy(int x, int y, BYTE Angle, BYTE Ptn, BYTE a)
{
	D3DTLVERTEX		Vertex[10];
	int				w, h;
	int				dx, dy;
	D3DVALUE		u1, v1, u2, v2;
	EADrawInfo		*pInfo;


	// this ポインタが NULL なら描画する必要はない //
	if(NULL == this) return;

#ifdef PBG_DEBUG
	if(m_TextureID < 0){
		PbgError("初期化されていないアニメーションパターンの参照");
		return;
	}
#endif

	// 描画用の情報をローカル変数に納める //
	pInfo = m_DrawInfo + Ptn;	// 対象となるＵＶ情報を取得
	w     = pInfo->HalfWidth;	// 幅を取得
	h     = pInfo->HalfHeight;	// 高さを取得
	u1    = pInfo->m_Left;		// テクスチャの左
	v1    = pInfo->m_Top;		// テクスチャの上
	u2    = pInfo->m_Right;		// テクスチャの右
	v2    = pInfo->m_Bottom;	// テクスチャの下

	// α値による拡縮処理 //
	h = (h * a)       / 255;
	w = (w * a)       / 255;
	//w = w * (255 + (255 - a) * 4) / 255;

	// 現在は、仮実装なので、高速化は追求しない                        //
	// ちなみに、注釈は、Angle = 64 の時にどの方向にあるかを示している //

	// 左上に向かうベクトルを求める //
	dx = (CosL(Angle+128, w) + CosL(Angle-64, h)) >> 8;	// ｘ方向の成分
	dy = (SinL(Angle+128, w) + SinL(Angle-64, h)) >> 8;	// ｙ方向の成分
	Set2DPointC(Vertex+0, x+dx, y+dy, u1, v1, RGBA_MAKE(255, 255, 255, a));	// 左上①
//	Set2DPointC(Vertex+5, x+dx, y+dy, u1, v1, RGBA_MAKE(255, 255, 255, a));	// 左上②
	Set2DPointC(Vertex+2, x-dx, y-dy, u2, v2, RGBA_MAKE(255, 255, 255, a));	// 右下①
//	Set2DPointC(Vertex+3, x-dx, y-dy, u2, v2, RGBA_MAKE(255, 255, 255, a));	// 右下②

	// 右上に向かうベクトルを求める //
	dx = (CosL(Angle, w) + CosL(Angle-64, h)) >> 8;	// ｘ方向の成分
	dy = (SinL(Angle, w) + SinL(Angle-64, h)) >> 8;	// ｙ方向の成分
	Set2DPointC(Vertex+1, x+dx, y+dy, u2, v1, RGBA_MAKE(255, 255, 255, a));	// 右上
	Set2DPointC(Vertex+3, x-dx, y-dy, u1, v2, RGBA_MAKE(255, 255, 255, a));	// 左下
//	Set2DPointC(Vertex+4, x-dx, y-dy, u1, v2, RGBA_MAKE(255, 255, 255, a));	// 左下


	// 描画する //
	g_pGrp->SetTexture(m_TextureID);				// テクスチャセットして
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);		// カラーキー付きの描画
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, Vertex, 4);	//書き込み
}


// 初期化を行う //
FVOID EAnimePtn::Initialize(void)
{
	m_TextureID = -1;
}



// コンストラクタ //
CEnemyAnimeContainer::CEnemyAnimeContainer()
{
}


// デストラクタ //
CEnemyAnimeContainer::~CEnemyAnimeContainer()
{
}


// テクスチャ情報をロードする //
BOOL CEnemyAnimeContainer::Load(
		BYTE *pBuffer				// Ｔｅｘプロシージャ開始アドレス
	  , CHARACTER_ID Player1_ID		// プレイヤー１のキャラクタＩＤ
	  , CHARACTER_ID Player2_ID)	// プレイヤー２のキャラクタＩＤ
{
	EADrawInfo		Ptn[EANIMEPTN_MAX];		// パターンの一時格納先
	EADrawInfo		*pEAInfo;				// パターンの代入先

	EAnimePtn		*pTarget;			// 代入先を示すポインタ
	short			u1, v1, u2, v2;		// テクスチャＵＶの一時保存用
	int				TextureID;			// テクスチャＩＤ(使用方法に注意！)
	int				Index;				// 配列のインデックス値
	int				NumArgs;			// 可変長引数の個数
	int				i, temp;


	if(NULL == pBuffer) return FALSE;

	// 読み込み先を初期化を行う //
	for(i=0; i<EANIME_MAX; i++){
		m_Buffer[i].Initialize();
	}

	// テクスチャＩＤを無効化する //
	TextureID = -1;


	if(NULL == pBuffer){
		PbgError("テクスチャのロードに失敗しました");
		return FALSE;
	}


	while(1){
#ifdef PBG_DEBUG
		if(GetAsyncKeyState(VK_F11) & 0x8000){
			PbgLog("Texture 解析中の停止要求を受理します\r\n");
			return FALSE;
		}
#endif

		// 可変長命令なので、十分に注意を払う
		switch(pBuffer[0]){
			// アニメーションの定義   : Cmd(1) + Index(1) + (n(1), ID[n]) //
			case SCR_ANIME:
				Index    = pBuffer[1];			// 配列参照用の値を取得
				NumArgs  = pBuffer[2];			// 引数の数を取得する
				pTarget  = m_Buffer + Index;	// 対象となるアニメ定義構造体へ
				pEAInfo  = pTarget->m_DrawInfo;	// 描画パターンの格納先へ

				// パターン数と対象となるテクスチャを格納する //
				pTarget->m_NumPtn    = NumArgs;		// アニメーションパターン数
				pTarget->m_TextureID = TextureID;	// テクスチャＩＤ
				pTarget->m_bStop     = FALSE;		// 繰り返しタイプのアニメーション

				// テクスチャ座標の格納先へと進む //
				pBuffer += 3;	// Cmd(1) + Index(1) + n(1)

				// テクスチャ座標を順次格納 //
				for(i=0; i<NumArgs; i++){
					*pEAInfo = Ptn[*pBuffer];
					pBuffer++;					// 次の読み込み先へと進む
					pEAInfo++;					// 次の書き込み先に移動
				}

				// 読み込み中にポインタを読み進めているので、ここでは //
				// ポインタの前進は必要ない                           //
			break;


			case SCR_STOP:
				if(NULL == pTarget) break;

				// 少々危険だが //
				pTarget->m_bStop = TRUE;

				pBuffer++;
			break;


			// 矩形型の代入命令(絶対) : Cmd(1) + Index(1) + rect(2Byte * 4) //
			case SCR_RECT:
				Index    = pBuffer[1];								// 書き込み先
				if(Index >= EANIMEPTN_MAX){
					PbgError("アニメーションパターン番号が大きすぎる");
					return FALSE;
				}
				u1       = *(short *)(&pBuffer[1 + 1 + (2 * 0)]);	// Texture 左
				v1       = *(short *)(&pBuffer[1 + 1 + (2 * 1)]);	// Texture 上
				u2       = *(short *)(&pBuffer[1 + 1 + (2 * 2)]);	// Texture 右
				v2       = *(short *)(&pBuffer[1 + 1 + (2 * 3)]);	// Texture 下
				pBuffer += (1 + 1 + (2 * 4));						// 次のデータへ

				// 注意：テクスチャのサイズは 256x256 限定    //
				Ptn[Index].Set(u1, v1, u2, v2, 256);

				// 描画時の幅・高さの半分の値を求める //

				Ptn[Index].HalfWidth  = abs(u2 - u1) * 128 - 256;	//  幅  / 2
				Ptn[Index].HalfHeight = abs(v2 - v1) * 128 - 256;	// 高さ / 2
			break;


			// テクスチャのロード     : RegID(1) + Cmd(1) + Num(1) + string + \0  //
			case TEX_REG_ENEMY:		// 敵テクスチャ
//			case TEX_REG_ENEMY2:	// 敵テクスチャ２番
//			case TEX_REG_ENEMY3:	// 敵テクスチャ３番
//			case TEX_REG_ENEMY4:	// 敵テクスチャ４番
				// 次の命令コードの格納アドレスを返すので、注意 //
				pBuffer = ParseLoadTexture(pBuffer, &TextureID);
				if(NULL == pBuffer) return FALSE;
			break;

			case TEX_REG_BG:
				pBuffer = ParseLoadExtraTexture(pBuffer, TEXTURE_ID_BG01 + pBuffer[2]);
				if(NULL == pBuffer) return FALSE;
			break;

			// 条件付きテクスチャロード                                 //
			// 通常のテクスチャロードとは処理が異なるので注意           //
			// 参考：色違いを実現するなら、修正が必要だ                 //
			case TEX_REG_VIVIT:		// 条件テクスチャ(VIVIT)
			case TEX_REG_STG1BOSS:	// 条件テクスチャ(ミリア)
			case TEX_REG_STG2BOSS:	// 条件テクスチャ(めい＆まい)
			case TEX_REG_STG3BOSS:	// 条件テクスチャ(ゲイツ)
			case TEX_REG_STG4BOSS:	// 条件テクスチャ(マリー)
			case TEX_REG_STG5BOSS:	// 条件テクスチャ(エーリッヒ)
			case TEX_REG_MORGAN:	// 条件テクスチャ(モーガン)
			case TEX_REG_MUSE:		// 条件テクスチャ(ミューズ)
			case TEX_REG_YUKA:		// 条件テクスチャ(幽香)
				// プレイヤー１のテクスチャロード //
				temp = GetTextureID(Player1_ID, 1, pBuffer);
				if(temp >= 0){
					ParseLoadExtraTexture(pBuffer, temp);
					TextureID = temp;
				}

				// プレイヤー２のテクスチャロード //
				temp = GetTextureID(Player2_ID, 2, pBuffer);
				if(temp >= 0){
					ParseLoadExtraTexture(pBuffer, temp);
					TextureID = temp;
				}

				// 次の命令まで読み飛ばす //
				pBuffer = SkipLoadBlock(pBuffer);
			break;


			// 終了命令(正常終了) //
			case SCR_EXIT:
			return TRUE;


			default:
				PbgError("CEnemyAnime::Load() : スクリプトエラー");
			return FALSE;
		}
	}
}


// テクスチャ情報をロードする //
BOOL CEnemyAnimeContainer::LoadBA(BYTE *pBuffer
								 , CHARACTER_ID Player1_ID)		// Ｔｅｘプロシージャ開始アドレス
{
	EADrawInfo		Ptn[EANIMEPTN_MAX];		// パターンの一時格納先
	EADrawInfo		*pEAInfo;				// パターンの代入先

	EAnimePtn		*pTarget;			// 代入先を示すポインタ
	short			u1, v1, u2, v2;		// テクスチャＵＶの一時保存用
	int				TextureID;			// テクスチャＩＤ(使用方法に注意！)
	int				Index;				// 配列のインデックス値
	int				NumArgs;			// 可変長引数の個数
	int				i, temp;
	int				numTexture = 0;

	if(NULL == pBuffer) return FALSE;

	// 読み込み先を初期化を行う //
	for(i=0; i<EANIME_MAX; i++){
		m_Buffer[i].Initialize();
	}

	// テクスチャＩＤを無効化する //
	TextureID = -1;


	if(NULL == pBuffer){
		PbgError("テクスチャのロードに失敗しました");
		return FALSE;
	}


	while(1){
#ifdef PBG_DEBUG
		if(GetAsyncKeyState(VK_F11) & 0x8000){
			PbgLog("Texture 解析中の停止要求を受理します\r\n");
			return FALSE;
		}
#endif

		// 可変長命令なので、十分に注意を払う
		switch(pBuffer[0]){
			// アニメーションの定義   : Cmd(1) + Index(1) + (n(1), ID[n]) //
			case SCR_ANIME:
				Index    = pBuffer[1];			// 配列参照用の値を取得
				NumArgs  = pBuffer[2];			// 引数の数を取得する
				pTarget  = m_Buffer + Index;	// 対象となるアニメ定義構造体へ
				pEAInfo  = pTarget->m_DrawInfo;	// 描画パターンの格納先へ

				// パターン数と対象となるテクスチャを格納 //
				pTarget->m_NumPtn    = NumArgs;		// アニメーションパターン数
				pTarget->m_TextureID = TextureID;	// テクスチャＩＤ
				pTarget->m_bStop     = FALSE;		// 繰り返しタイプのアニメーション

				// テクスチャ座標の格納先へと進む //
				pBuffer += 3;	// Cmd(1) + Index(1) + n(1)

				// それでは、テクスチャ座標を順次格納 //
				for(i=0; i<NumArgs; i++){
					*pEAInfo = Ptn[*pBuffer];
					pBuffer++;					// 次の読み込み先へと進む
					pEAInfo++;					// 次の書き込み先に移動
				}

				// 読み込み中にポインタを読み進めているので、ここでは //
				// ポインタの前進は必要ない                           //
			break;


			case SCR_STOP:
				if(NULL == pTarget) break;

				pTarget->m_bStop = TRUE;

				pBuffer++;
			break;


			// 矩形型の代入命令(絶対) : Cmd(1) + Index(1) + rect(2Byte * 4) //
			case SCR_RECT:
				Index    = pBuffer[1];								// 書き込み先
				if(Index >= EANIMEPTN_MAX){
					PbgError("アニメーションパターン番号が大きすぎる");
					return FALSE;
				}
				u1       = *(short *)(&pBuffer[1 + 1 + (2 * 0)]);	// Texture 左
				v1       = *(short *)(&pBuffer[1 + 1 + (2 * 1)]);	// Texture 上
				u2       = *(short *)(&pBuffer[1 + 1 + (2 * 2)]);	// Texture 右
				v2       = *(short *)(&pBuffer[1 + 1 + (2 * 3)]);	// Texture 下
				pBuffer += (1 + 1 + (2 * 4));						// 次のデータへ

				// 注意：テクスチャのサイズは 256x256 限定 //
				Ptn[Index].Set(u1, v1, u2, v2, 256);

				// 描画時の幅・高さの半分の値を求める //

				Ptn[Index].HalfWidth  = abs(u2 - u1) * 128 - 256;	//  幅  / 2
				Ptn[Index].HalfHeight = abs(v2 - v1) * 128 - 256;	// 高さ / 2
			break;


			// テクスチャのロード     : RegID(1) + Cmd(1) + Num(1) + string + \0  //
			case TEX_REG_ENEMY:		// 敵テクスチャ
//			case TEX_REG_ENEMY2:	// 敵テクスチャ２番
//			case TEX_REG_ENEMY3:	// 敵テクスチャ３番
//			case TEX_REG_ENEMY4:	// 敵テクスチャ４番
				// 次の命令コードの格納アドレスを返すので、注意 //
				pBuffer = ParseLoadTexture(pBuffer, &TextureID);
				if(NULL == pBuffer) return FALSE;
			break;

			case TEX_REG_BG:
				pBuffer = ParseLoadExtraTexture(pBuffer, TEXTURE_ID_BG01 + pBuffer[2]);
				if(NULL == pBuffer) return FALSE;
			break;

			// 条件付きテクスチャロード                             //
			// 通常のテクスチャロードとは処理が異なるので注意       //
			// 参考：色違いを実現するなら、修正が必要               //
			case TEX_REG_VIVIT:		// 条件テクスチャ(VIVIT)
			case TEX_REG_STG1BOSS:	// 条件テクスチャ(ミリア)
			case TEX_REG_STG2BOSS:	// 条件テクスチャ(めい＆まい)
			case TEX_REG_STG3BOSS:	// 条件テクスチャ(ゲイツ)
			case TEX_REG_STG4BOSS:	// 条件テクスチャ(マリー)
			case TEX_REG_STG5BOSS:	// 条件テクスチャ(エーリッヒ)
			case TEX_REG_MORGAN:	// 条件テクスチャ(モーガン)
			case TEX_REG_MUSE:		// 条件テクスチャ(ミューズ)
			case TEX_REG_YUKA:		// 条件テクスチャ(幽香)
				// プレイヤー１のテクスチャロード(先頭の１枚のみ) //
				temp = GetTextureID(Player1_ID, 1, pBuffer);
				if(temp >= 0 && 0 == pBuffer[2]){
					ParseLoadExtraTexture(pBuffer, TEXTURE_ID_P1_00);
					TextureID = TEXTURE_ID_P1_00;
				}

				// プレイヤー２側に強制的ロード //
				temp = TEXTURE_ID_P2_TAIL - numTexture;
				if(temp >= 0){
					ParseLoadExtraTexture(pBuffer, temp);
					TextureID = temp;
					numTexture++;
				}

				// 次の命令まで読み飛ばす //
				pBuffer = SkipLoadBlock(pBuffer);
			break;


			// 終了命令(正常終了) //
			case SCR_EXIT:
			return TRUE;


			default:
				PbgError("CEnemyAnime::Load() : スクリプトエラー");
			return FALSE;
		}
	}
}


// 実際にテクスチャを読み込む(次の命令へのアドレスを返す) //
BYTE *CEnemyAnimeContainer::ParseLoadTexture(BYTE *pBuffer, int *pTextureID)
{
	// ロード命令でなかった場合はスクリプトエラー //
	if(SCR_LOAD != pBuffer[1]){
		PbgError("CEnemyAnime::ParseLoadTexture() : 未定義の命令コード");
		return NULL;
	}

	// テクスチャレジスタ→テクスチャＩＤに変換する //
	switch(pBuffer[2]){
		case 0:		*pTextureID = TEXTURE_ID_ENEMY1;	break;
		case 1:		*pTextureID = TEXTURE_ID_ENEMY2;	break;
		case 2:		*pTextureID = TEXTURE_ID_ENEMY3;	break;
		case 3:		*pTextureID = TEXTURE_ID_ENEMY4;	break;

		default:	// そりゃないでしょ
			PbgError("CEnemyAnime::ParseLoadTexture() : 未定義のテクスチャＩＤ");
		return NULL;
	}

	// 後は、お任せする //
	return ParseLoadExtraTexture(pBuffer, *pTextureID);
}


// テクスチャＩＤを指定する(条件付きテクスチャ) //
BYTE *CEnemyAnimeContainer::ParseLoadExtraTexture(BYTE *pBuffer, int TextureID)
{
	DWORD			flag;

	switch(TextureID){
		case TEXTURE_ID_BG01:	case TEXTURE_ID_BG02:
		case TEXTURE_ID_BG03:	case TEXTURE_ID_BG04:
			flag = 0;
		break;

		default:
			flag = GRPTEX_TRANSBLACK;
		break;
	}
	// レジスタＩＤと命令コード(SCR_LOAD)とテクスチャ枚数の読み飛ばしを行う //
	pBuffer += 3;		// Regs(1) + Cmd(1) + Num(1)

	// この関数を呼び出す時点で、 pBuffer はテクスチャ名を指している //
	if(FALSE == g_pGrp->CreateTextureP(
						TextureID				// テクスチャの読み込み先
					  , "GRAPH.DAT"				// アーカイブファイル名
					  , (char *)pBuffer			// テクスチャ名
					  , flag)){					// テクスチャフラグ
		PbgError("CEnemyAnime::ParseLoadExtraTexture() : \r\n\tテクスチャがみつからない or 適切なフォーマットが見つからない");
		return NULL;
	}

#ifdef PBG_DEBUG
	char buf[1024];
	wsprintf(buf, "LoadTexture %s -> %d", pBuffer, TextureID);
	PbgLog(buf);
#endif

	// テクスチャ名 + \0 を読み飛ばすと、次の命令コードの格納先になる //
	return pBuffer + strlen((char *)pBuffer) + 1;	// \0 の読み飛ばしに注意
}


// LOAD ブロックの読み飛ばしを行う //
BYTE *CEnemyAnimeContainer::SkipLoadBlock(BYTE *pBuffer)
{
	// レジスタＩＤと命令コード(SCR_LOAD)の読み飛ばしを行う //
	// + テクスチャ番号(2001/10/11)
	pBuffer += 3;		// Regs(1) + Cmd(1) + Num(1)

	// テクスチャ名 + \0 を読み飛ばすと、次の命令コードの格納先になる //
	return pBuffer + strlen((char *)pBuffer) + 1;	// \0 の読み飛ばしに注意
}


/*
// 廃止 : 2001/10/11
// LOAD2 命令を解析する //
BYTE *CEnemyAnimeContainer::ParseLoad2(BYTE				*pBuffer
									 , CHARACTER_ID		P1
									 , CHARACTER_ID		P2)
{
	CHARACTER_ID		Target;
//	DWORD				TextureID;
	char				*pTexName;

	Target   = TexRegs2ID(pBuffer[0]);
	pTexName = (char *)(pBuffer + 3);

	if(P1 == Target){
		TextureID = TEXTURE_ID_P1A_01U + pBuffer[2];

		if(FALSE == g_pGrp->CreateTextureP(
						TextureID				// テクスチャの読み込み先
					  , "GRAPH2.DAT"			// アーカイブファイル名
					  , pTexName				// テクスチャ名
					  , GRPTEX_TRANSBLACK)){	// 黒を透過色とする
			PbgError("CEnemyAnime::ParseLoad2() : \r\n\tテクスチャがみつからない or 適切なフォーマットが見つからない");
			return NULL;
		}

#ifdef PBG_DEBUG
	char buf[1024];
	wsprintf(buf, "LoadTexture(Player1) %s -> %d", pTexName, TextureID);
	PbgLog(buf);
#endif
	}

	if(P2 == Target){
		TextureID = TEXTURE_ID_P2A_01U + pBuffer[2];

		if(FALSE == g_pGrp->CreateTextureP(
						TextureID				// テクスチャの読み込み先
					  , "GRAPH2.DAT"			// アーカイブファイル名
					  , pTexName				// テクスチャ名
					  , GRPTEX_TRANSBLACK)){	// 黒を透過色とする
			PbgError("CEnemyAnime::ParseLoad2() : \r\n\tテクスチャがみつからない or 適切なフォーマットが見つからない");
			return NULL;
		}

#ifdef PBG_DEBUG
	char buf[1024];
	wsprintf(buf, "LoadTexture(Player2) %s -> %d", pTexName, TextureID);
	PbgLog(buf);
#endif
	}

	PbgError("テクスチャロード部は現在工事中ですぞ");

	pBuffer += 3;
	return pBuffer + strlen((char *)pBuffer) + 1;
}
*/

// テクスチャＩＤを取得する                             //
//                                                      //
// arg  CharID   : キャラクタＩＤ(CHARACTER_VIVIT, ...) //
//      PlayerID : プレイヤーＩＤ(1 or 2)               //
//      pScript  : 現在のスクリプト読み込み位置         //
//                                                      //
// ret  -1 なら失敗、それ以外なら、テクスチャＩＤ       //
FINT CEnemyAnimeContainer::GetTextureID(CHARACTER_ID	CharID		// キャラクタＩＤ
									  , int				PlayerID	// プレイヤーＩＤ
									  , BYTE			*pScript)	// テクスチャの格納対象
{
	CHARACTER_ID		Target;

	// テクスチャレジスタから、キャラクタＩＤを取得する //
	Target = TexRegs2ID(pScript[0]);
	if(CharID != Target){
		return -1;
	}

	if(pScript[2] >= LOADTEXTURE_MAX){
#ifdef PBG_DEBUG
		PbgError("不正なテクスチャ番号：Ecl コンパイラに問題があります");
#endif
		return -1;
	}

	if(     1 == PlayerID) return TEXTURE_ID_P1_TAIL - pScript[2];
	else if(2 == PlayerID) return TEXTURE_ID_P2_TAIL - pScript[2];
	else                   return -1;
}


// レジスタ番号からテクスチャＩＤを取得 //
CHARACTER_ID CEnemyAnimeContainer::TexRegs2ID(BYTE TexRegs)
{
	switch(TexRegs){
		// テクスチャ指定レジスタ → プレイヤーＩＤに変換する //
		case TEX_REG_VIVIT:		return CHARACTER_VIVIT;
		case TEX_REG_STG1BOSS:	return CHARACTER_STG1;
		case TEX_REG_STG2BOSS:	return CHARACTER_STG2;
		case TEX_REG_STG3BOSS:	return CHARACTER_STG3;
		case TEX_REG_STG4BOSS:	return CHARACTER_STG4;
		case TEX_REG_STG5BOSS:	return CHARACTER_STG5;
		case TEX_REG_MORGAN:	return CHARACTER_MORGAN;
		case TEX_REG_MUSE:		return CHARACTER_MUSE;
		case TEX_REG_YUKA:		return CHARACTER_YUKA;

		default:
		return CHARACTER_UNKNOWN;
	}
}
