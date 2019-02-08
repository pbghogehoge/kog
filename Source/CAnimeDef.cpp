/*
 *   CAnimeDef.cpp   : アニメーション定義
 *
 */

#include "CAnimeDef.h"
#include "SCL.h"
#include "ScrBase.h"
#include "Gian2001.h"



// コンストラクタ //
CAnimeDef::CAnimeDef(SCENE_ID SceneID)
{
	int		*p;
	int		i;

	int P1Texture[LOADTEXTURE_MAX] = {
		TEXTURE_ID_P1_00, TEXTURE_ID_P1_01, TEXTURE_ID_P1_02, TEXTURE_ID_P1_03,
		TEXTURE_ID_P1_04, TEXTURE_ID_P1_05, TEXTURE_ID_P1_06, TEXTURE_ID_P1_07,
		TEXTURE_ID_P1_08, TEXTURE_ID_P1_09, TEXTURE_ID_P1_10, TEXTURE_ID_P1_11,
	};

	int P2Texture[LOADTEXTURE_MAX] = {
		TEXTURE_ID_P2_00, TEXTURE_ID_P2_01, TEXTURE_ID_P2_02, TEXTURE_ID_P2_03,
		TEXTURE_ID_P2_04, TEXTURE_ID_P2_05, TEXTURE_ID_P2_06, TEXTURE_ID_P2_07,
		TEXTURE_ID_P2_08, TEXTURE_ID_P2_09, TEXTURE_ID_P2_10, TEXTURE_ID_P2_11,
	};

	switch(SceneID){
		case SCENE_PLAYER_1:	p = P1Texture;		break;
		case SCENE_PLAYER_2:	p = P2Texture;		break;

		default:
#ifdef PBG_DEBUG
			PbgError("シーンＩＤがおかしい");
#endif
		return;
	}

	// テクスチャＩＤをコピーする //
	for(i=0; i<LOADTEXTURE_MAX; i++){
		m_TextureID[i] = p[i];
	}
}


// デストラクタ //
CAnimeDef::~CAnimeDef()
{
}


// テクスチャのロード＆パターン定義を行う //
// arg : CharID  キャラクタＩＤ           //
FBOOL CAnimeDef::Load(int CharID, BOOL Is2PColor)
{
	LoadTextureEntry		*pTexEntry;

	int						i, n;
	BYTE					*pCmd;
	char					*pGrpFileName;
	short					u1, v1, u2, v2;
	BYTE					Index;

	if(Is2PColor) pGrpFileName = "GRAPH3.DAT";
	else          pGrpFileName = "GRAPH2.DAT";

	// キャラクタＩＤを保存する //
	m_CharID = CharID;

	// テクスチャ開始アドレスを取得する //
	pTexEntry = CSCLDecoder::GetTextureEntry(CharID);
	if(NULL == pTexEntry) return FALSE;

	// テクスチャの枚数を調べる //
	n = pTexEntry->NumTextures;
	if(n > LOADTEXTURE_MAX) return FALSE;	// テクスチャの枚数が不正

	// テクスチャＩＤを無効化する //
	for(i=0; i<256; i++){
		m_AnimePtn[i].TextureID = -1;
	}

	// 必要な枚数だけロードを行う //
	for(i=0; i<n; i++){
		pCmd = CSCLDecoder::Offset2Ptr(pTexEntry->EntryPoint[i]);
		if(NULL == pCmd) return FALSE;

		if(SCR_LOADEX != pCmd[0]){
			return FALSE;
		}

		pCmd++;

		// 圧ファイルからテクスチャを生成する //
		if(FALSE == g_pGrp->CreateTextureP(	m_TextureID[i]
										,	pGrpFileName
										,	(char *)pCmd
										,	GRPTEX_TRANSBLACK)){
			PbgError("テクスチャのロードに失敗(テクスチャが見つからない)");
#ifdef PBG_DEBUG
			PbgError((char *)pCmd);
#endif
			return FALSE;
		}

		pCmd += (strlen((char *)pCmd) + 1);

		while(SCR_RECT == pCmd[0]){
			Index    = pCmd[1];								// 書き込み先
			u1       = *(short *)(&pCmd[1 + 1 + (2 * 0)]);	// Texture 左
			v1       = *(short *)(&pCmd[1 + 1 + (2 * 1)]);	// Texture 上
			u2       = *(short *)(&pCmd[1 + 1 + (2 * 2)]);	// Texture 右
			v2       = *(short *)(&pCmd[1 + 1 + (2 * 3)]);	// Texture 下
			pCmd += (1 + 1 + (2 * 4));						// 次のデータへ

			// テクスチャの再定義が行われている場合 //
			if(-1 != m_AnimePtn[Index].TextureID){
				PbgError("テクスチャが再ロードされています");
				return FALSE;
			}

			// テクスチャ番号を関連づける //
			m_AnimePtn[Index].TextureID = m_TextureID[i];

			// テクスチャ座標への変換はメンバ関数におまかせ //
			// 注意：テクスチャのサイズは 256x256 限定      //
			m_AnimePtn[Index].Set(u1, v1, u2, v2, 256);

			// 描画時の幅・高さの半分の値を求める //
			m_AnimePtn[Index].HalfWidth  = (u2 - u1) * 128 - 256;	//  幅  / 2
			m_AnimePtn[Index].HalfHeight = (v2 - v1) * 128 - 256;	// 高さ / 2
		}

		// 終端が EXIT になっていないとおかしい //
		if(SCR_EXIT != pCmd[0]){
			PbgError("テクスチャ定義ブロックが不正です");
			return FALSE;
		}
	}

	return TRUE;
}


// 描画を行う                                                   //
// arg : ox, oy  中心座標(x256)                                 //
//     : sx, sy  拡大率(255 = 1.0)                              //
//     : c       描画色(r, g, b, a)                             //
//     : d       回転角                                         //
//     : ptn     パターン番号                                   //
//--------------------------------------------------------------//
// なお、レンダリングステートの変更は行わないので、呼び出し側で //
// 指定する必要があるため注意すべし                             //
FVOID CAnimeDef
	::DrawSingle(int ox, int oy, int sx, int sy, DWORD c, BYTE d, BYTE ptn)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	ExAnmPtn		*pAnm;
	int				w, h, dx, dy;

	pAnm = m_AnimePtn + ptn;

	w = (pAnm->HalfWidth  * sx) >> 8;
	h = (pAnm->HalfHeight * sy) >> 8;

	u0 = pAnm->m_Left;
	v0 = pAnm->m_Top;
	u1 = pAnm->m_Right;
	v1 = pAnm->m_Bottom;

	dx = CosL(d+128, w) + CosL(d-64, h);
	dy = SinL(d+128, w) + SinL(d-64, h);
	Set2DPointC(tlv+0, (ox+dx)>>8, (oy+dy)>>8, u0, v0, c);
	Set2DPointC(tlv+2, (ox-dx)>>8, (oy-dy)>>8, u1, v1, c);

	dx = CosL(d, w) + CosL(d-64, h);
	dy = SinL(d, w) + SinL(d-64, h);
	Set2DPointC(tlv+1, (ox+dx)>>8, (oy+dy)>>8, u1, v0, c);
	Set2DPointC(tlv+3, (ox-dx)>>8, (oy-dy)>>8, u0, v1, c);

	g_pGrp->SetTexture(pAnm->TextureID);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 描画を行う                                                   //
// arg : ox, oy  中心座標(x256)                                 //
//     : sx, sy  拡大率(255 = 1.0)                              //
//     : c       描画色(r, g, b, a)                             //
//     : d       回転角                                         //
//     : pptn    パターン番号配列                               //
//--------------------------------------------------------------//
// なお、レンダリングステートの変更は行わないので、呼び出し側で //
// 指定する必要があるため注意すべし                             //
FVOID CAnimeDef
	::DrawDouble(int ox, int oy, int sx, int sy, DWORD c, BYTE d, BYTE *pptn)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;
	ExAnmPtn		*pAnm;
	int				w, h, dx, dy, lx, ly;


	// 上のほうのテクスチャを描画 //
	pAnm = m_AnimePtn + pptn[0];

	w = (pAnm->HalfWidth  * sx) >> 8;
	h = (pAnm->HalfHeight * sy) >> 7;	// 高さは２倍で取得

	u0 = pAnm->m_Left;
	v0 = pAnm->m_Top;
	u1 = pAnm->m_Right;
	v1 = pAnm->m_Bottom;

	dx = CosL(d+128, w);
	dy = SinL(d+128, w);
	lx = CosL(d-64, h);
	ly = SinL(d-64, h);

	Set2DPointC(tlv+0, (ox+dx+lx)>>8, (oy+dy+ly)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (ox-dx+lx)>>8, (oy-dy+ly)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (ox-dx)>>8, (oy-dy)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (ox+dx)>>8, (oy+dy)>>8, u0, v1, c);

	g_pGrp->SetTexture(pAnm->TextureID);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


	// 下のほうのテクスチャを描画 //
	pAnm = m_AnimePtn + pptn[1];

	w = (pAnm->HalfWidth  * sx) >> 8;
	h = (pAnm->HalfHeight * sy) >> 7;	// 高さは２倍で取得

	u0 = pAnm->m_Left;
	v0 = pAnm->m_Top;
	u1 = pAnm->m_Right;
	v1 = pAnm->m_Bottom;

	dx = CosL(d+128, w);
	dy = SinL(d+128, w);
	lx = CosL(d+64, h);
	ly = SinL(d+64, h);

	Set2DPointC(tlv+0, (ox+dx)>>8, (oy+dy)>>8, u0, v0, c);
	Set2DPointC(tlv+1, (ox-dx)>>8, (oy-dy)>>8, u1, v0, c);
	Set2DPointC(tlv+2, (ox-dx+lx)>>8, (oy-dy+ly)>>8, u1, v1, c);
	Set2DPointC(tlv+3, (ox+dx+lx)>>8, (oy+dy+ly)>>8, u0, v1, c);

	g_pGrp->SetTexture(pAnm->TextureID);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
