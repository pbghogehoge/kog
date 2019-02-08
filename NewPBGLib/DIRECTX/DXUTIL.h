/*
 *   DxUtil.h   : DirectX 用補助関数
 *
 */

#ifndef DXUTIL_INCLUDED
#define DXUTIL_INCLUDED "DirectX 用補助関数 : Version 0.03 : Update 2001/03/07"

/*  [更新履歴]
 *    Version 0.03 : 2001/03/07 : SET2DP_xxx マクロの追加
 *    Version 0.02 : 2001/02/25 : GuidToString() ポインタが NULL の場合に対応
 *    Version 0.01 : 2001/01/18 : GUID->文字列 変換用関数
 */



#include "PbgType.h"



/***** [ マクロ ] *****/

// (x, y, u, v) をセットする為のマクロ //
#define SET2DP_XYUV(p, x, y, u, v)		\
	(p)->sx = D3DVAL(x);				\
	(p)->sy = D3DVAL(y);				\
	(p)->tu = u;						\
	(p)->tv = v;

// (x, u, v) をセットする為のマクロ //
#define SET2DP_XUV(p, x, u, v)		\
	(p)->sx = D3DVAL(x);			\
	(p)->tu = u;					\
	(p)->tv = v;

// (y, u, v) をセットする為のマクロ //
#define SET2DP_YUV(p, y, u, v)		\
	(p)->sy = D3DVAL(y);			\
	(p)->tu = u;					\
	(p)->tv = v;

// (y, u) をセットする為のマクロ //
#define SET2DP_UV(p, u, v)		\
	(p)->tu = u;				\
	(p)->tv = v;

// x をセットする為のマクロ //
#define SET2DP_X(p, x)		\
	(p)->sx = D3DVAL(x);

// y をセットする為のマクロ //
#define SET2DP_Y(p, y)		\
	(p)->sy = D3DVAL(y);



/***** [ 構造体 ] *****/

// Direct3D のテクスチャ座標指定用矩クラス //
class D3DRECTANGLE {
public:
	// 座標代入用関数(D3DVALUE で指定) //
	IVOID Set(D3DVALUE x1, D3DVALUE y1, D3DVALUE x2, D3DVALUE y2){
		m_Left   = x1;	// 左側の座標
		m_Top    = y1;	// 上側の座標
		m_Right  = x2;	// 右側の座標
		m_Bottom = y2;	// 左側の座標
	};

	// 座標代入用関数(テクスチャ上の座標で指定) //
	// ゼロ除算チェックは行わないので注意！！   //
	IVOID Set(int x1, int y1, int x2, int y2, int w){
		m_Left   = D3DVAL(x1) / D3DVAL(w);	// 左側の座標
		m_Top    = D3DVAL(y1) / D3DVAL(w);	// 上側の座標
		m_Right  = D3DVAL(x2) / D3DVAL(w);	// 右側の座標
		m_Bottom = D3DVAL(y2) / D3DVAL(w);	// 左側の座標
	}

	D3DVALUE	m_Left;			// 矩形の左
	D3DVALUE	m_Top;			// 矩形の上
	D3DVALUE	m_Right;		// 矩形の右
	D3DVALUE	m_Bottom;		// 矩形の左
};



/***** [関数プロトタイプ] *****/

// ＧＵＩＤから文字列を生成する //
void GuidToString(const GUID *lpGuid, char String[32+4+1]);

// 与えられた頂点に対してＸＹＺ回転を行う //
FVOID RotateTLV(D3DTLVERTEX *tlv, int n, BYTE rx, BYTE ry, BYTE rz);

// ビットマスクからビット数を算出する //
FINT GetMask2BitCount(DWORD dwBitMask);



// 点(D3DTLVERTEX) をセットする //
IVOID Set2DPointC(D3DTLVERTEX *tlv, int x, int y, float u, float v, DWORD c)
{
	tlv->sx       = D3DVAL(x);
	tlv->sy       = D3DVAL(y);
	tlv->sz       = D3DVAL(0);
	tlv->tu       = u;
	tlv->tv       = v;
	tlv->rhw      = D3DVAL(1);
	tlv->color    = c;
	tlv->specular = RGB_MAKE(0,0,0);
}


// 外積を求める //
IVOID VectorCrossProduct(D3DVECTOR *pResult, D3DTLVERTEX *pSrc)
{
	D3DVECTOR		V1, V2;

	V1.x = pSrc[1].sx - pSrc[0].sx;
	V1.y = pSrc[1].sy - pSrc[0].sy;
	V1.z = pSrc[1].sz - pSrc[0].sz;

	V2.x = pSrc[2].sx - pSrc[0].sx;
	V2.y = pSrc[2].sy - pSrc[0].sy;
	V2.z = pSrc[2].sz - pSrc[0].sz;

	pResult->x = (V1.y * V2.z) - (V1.z * V2.y);
	pResult->y = (V1.z * V2.x) - (V1.x * V2.z);
	pResult->z = (V1.x * V2.y) - (V1.y * V2.x);
}


#endif
