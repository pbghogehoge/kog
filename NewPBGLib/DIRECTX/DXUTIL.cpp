/*
 *   DxUtil.cpp   : DirectX 用補助関数
 *
 */



#include "DxUtil.h"
#include "PbgMath.h"



// ＧＵＩＤから文字列を生成する //
void GuidToString(const GUID *lpGuid, char String[32+4+1])
{
	// 8 - 4 - 4 - 4 - 12 \0                 //
	// 88888888-4444-4444-4444-cccccccccccc0 //

	int		i;

	// GUID が NULL の場合は "NULL" とする //
	if(NULL == lpGuid){
		strcpy(String, "NULL");
		return;
	}

	// 8 - 4 - 4 - まで、書き出し //
	wsprintf(String,"%08x-%04x-%04x-",
				lpGuid->Data1, lpGuid->Data2, lpGuid->Data3);

	// 4 - を書き出す //
	wsprintf(String+19,"%02x%02x-", lpGuid->Data4[0], lpGuid->Data4[1]);

	// 12 \0 を書き出す //
	for(i=2;i<8;i++){
		wsprintf(String+24+(i-2)*2,"%02x",lpGuid->Data4[i]);
	}
}


// 与えられた頂点に対してＸＹＺ回転を行う //
FVOID RotateTLV(D3DTLVERTEX *tlv, int n, BYTE rx, BYTE ry, BYTE rz)
{
	int			v1[3], v2[3];

	while(n--){
		v2[0] = (tlv->sx * 65536);
		v2[1] = (tlv->sy * 65536);
		v2[2] = (tlv->sz * 65536);

		v1[0] = v2[0];
		v1[1] =  CosL(rx, v2[1]) + SinL(rx, v2[2]);
		v1[2] = -SinL(rx, v2[1]) + CosL(rx, v2[2]);

		v2[0] = CosL(ry, v1[0]) - SinL(ry, v1[2]);
		v2[1] = v1[1];
		v2[2] = SinL(ry, v1[0]) + CosL(ry, v1[2]);

		v1[0] =  CosL(rz, v2[0]) + SinL(rz, v2[1]);
		v1[1] = -SinL(rz, v2[0]) + CosL(rz, v2[1]);

		tlv->sx = D3DVAL(v1[0]) / D3DVAL(65536);
		tlv->sy = D3DVAL(v1[1]) / D3DVAL(65536);

		tlv++;
	}
}


// ビットマスクからビット数を算出する //
FINT GetMask2BitCount(DWORD dwBitMask)
{
	DWORD		i = 1;
	int			n = 0;

	while(i != 0){
		// ビットが立っている場合 //
		if(i & dwBitMask){
			n = n + 1;
		}

		i <<= 1;
	}

	return n;
}
