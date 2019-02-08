/*
 *   CGrpEnum.h   : ドライバ列挙用クラス
 *
 */

#ifndef CGRPENUM_INCLUDED
#define CGRPENUM_INCLUDED "ドライバ列挙用クラス : Version 0.03 : Update 2001/09/14"

/*  [ 参考 ]
 *    このクラスで列挙されるのは６４０x４８０x１６をサポートするデバイス
 */

/*  [更新履歴]
 *    Version 0.03 : 2001/09/14 : 微調整
 *    Version 0.02 : 2001/02/22 : 諸事情により、他のフォーマットをサポートしない
 *    Version 0.01 : 2001/02/16 : 後で継承するので、クラス名だけ定義する
 */



#include "PbgType.h"



/***** [ 定数 ] *****/
#define GRPDEV_NAME_LENGTH		35		// Ｄ３Ｄデバイスの名前
#define ENUM_GRAPHIC_DRIVER		20		// グラフィックドライバの列挙最大数



namespace Pbg {



/***** [クラス定義] *****/
typedef struct tagGrpPixelFormat {
	BYTE	m_NumRBit;			// 赤のビット数
	BYTE	m_NumGBit;			// 緑のビット数
	BYTE	m_NumBBit;			// 青のビット数
	BYTE	m_NumRGBBits;		// ＲＧＢのビット数
} GrpPixelFormat;

// サウンドドライバ情報 //
typedef struct tagGrpDrvInfo {
	char			m_strDesc[GRPDEV_NAME_LENGTH+1];	// デバイスのお名前
	D3DDEVICEDESC7	m_D3DDeviceDesc;					// D3DDevice の 性能

	LPGUID	m_lpDDGUID;		// DirectDraw の GUID へのポインタ
	LPGUID	m_lpD3DGUID;	// Direct3D の GUID へのポインタ

	GUID	m_DDGUID;		// DirectDraw  の GUID
	GUID	m_D3DGUID;		// Direct3D    の GUID

	char	m_strD3DGUID[32+4+1];	// GUID 文字列(Direct3D)
	char	m_strDDGUID[32+4+1];	// GUID 文字列(DirectDraw)

	DDCAPS	m_DDrawDriverCaps;		// DDrawDriver(HAL) の 性能
	DDCAPS	m_DDrawHELCaps;			// DDrawDriver(HEL) の 性能

	DWORD			m_ZBufferDepth;		// Ｚバッファの深さ(一番小さい値)
	DDPIXELFORMAT	m_ZBufferFormat;	// Ｚバッファのピクセルフォーマット

	GrpPixelFormat	m_PixelFormat;	// １６ビットのビットの配置情報
	BOOL			m_bIsHardware;	// ハードウェアデバイスか
} GrpDrvInfo;


class CGrpEnum {
public:
	// デバイスの列挙を行う(引数がNULL なら、制限なしで列挙する) //
	BOOL EnumerateDevice(GrpConfirmFunc pFunction = NULL);

	// 列挙されたデバイス数を取得する //
	DWORD GetNumEnumeratedDevices(void);

	CGrpEnum();		// コンストラクタ
	~CGrpEnum();	// デストラクタ


protected:
	GrpDrvInfo		m_DrvInfo[ENUM_GRAPHIC_DRIVER];	// ドライバ情報の格納先
	DWORD			m_NumEnumDrivers;				// 列挙されたドライバ数


private:
	// 列挙済みのデータをクリアする //
	void CleanupEnumeratedDevices(void);

	// DirectDraw のドライバを列挙する //
	static BOOL WINAPI EnumDDraw(GUID FAR	*pGUID
							   , LPSTR		sDesc
							   , LPSTR		sName
							   , LPVOID		pArg);

	// Direct3D.Device を列挙する //
	static HRESULT WINAPI EnumD3D(TCHAR				*sDesc
								, TCHAR				*sName
								, D3DDEVICEDESC7	*pDesc
								, VOID				*pArg);

	// ディスプレイモードを列挙する //
	static HRESULT WINAPI EnumDisplayMode(DDSURFACEDESC2	*pddsd
										, VOID				*pArg);

	// Ｚバッファのフォーマットを列挙する //
	static HRESULT WINAPI EnumZBuffer(LPDDPIXELFORMAT	lpDDPixFmt
									, LPVOID			pArg);


	// デバイスのソート用関数 //
	static int DeviceSortFunction(const void *p1, const void *p2);


private:
	GrpConfirmFunc	m_pConfirmFunction;	// デバイスの選別用関数
	GrpDrvInfo		m_ParentInfo;		// 親ドライバの情報(DD->D3D へと派生)
	LPDIRECT3D7		m_lpParentD3D;		// 親ドライバのＤ３Ｄインターフェース
};



} // namespace Pbg



#endif
