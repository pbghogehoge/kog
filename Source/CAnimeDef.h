/*
 *   CAnimeDef.h   : アニメーション定義
 *
 */

#ifndef CANIMEDEF_INCLUDED
#define CANIMEDEF_INCLUDED "アニメーション定義 : Version 0.01 : Update 2001/09/25"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/25 : 製作開始
 */



#include "PbgType.h"
#include "SCL.h"



/***** [ 定数 ] *****/



/***** [クラス定義] *****/

// パターン定義用構造体 //
typedef struct tagExAnmPtn : public D3DRECTANGLE {
	int		HalfWidth;		//  幅  / 2
	int		HalfHeight;		// 高さ / 2
	int		TextureID;		// テクスチャ番号
} ExAnmPtn;


class CAnimeDef {
public:
	// テクスチャのロード＆パターン定義を行う //
	// arg : CharID  キャラクタＩＤ           //
	//     : Is2PColor ２Ｐカラーなら真       //
	FBOOL Load(int CharID, BOOL Is2PColor);

	// 描画を行う                                                   //
	// arg : ox, oy  中心座標(x256)                                 //
	//     : sx, sy  拡大率(255 = 1.0)                              //
	//     : c       描画色(r, g, b, a)                             //
	//     : d       回転角                                         //
	//     : ptn     パターン番号                                   //
	//     : pptn    パターン番号配列                               //
	//--------------------------------------------------------------//
	// なお、レンダリングステートの変更は行わないので、呼び出し側で //
	// 指定する必要があるため注意すべし                             //
	FVOID DrawSingle(int ox, int oy, int sx, int sy, DWORD c, BYTE d, BYTE ptn);
	FVOID DrawDouble(int ox, int oy, int sx, int sy, DWORD c, BYTE d, BYTE *pptn);

	// キャラクタＩＤを返す //
	int GetCharID(void){ return m_CharID; };

	CAnimeDef(SCENE_ID SceneID);	// コンストラクタ
	~CAnimeDef();					// デストラクタ


private:
	ExAnmPtn	m_AnimePtn[256];					// アニメーションパターン
	int			m_TextureID[LOADTEXTURE_MAX];		// テクスチャ番号配列
	int			m_CharID;							// キャラクタＩＤ
};



#endif
