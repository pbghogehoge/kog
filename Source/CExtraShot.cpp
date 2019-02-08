/*
 *   CExtraShot.cpp   : Ｌｖ１・溜め管理
 *
 */



#include "CExtraShot.h"



// コンストラクタ //
CExtraShot::CExtraShot(RECT *rcTargetX256, int *pX, int *pY)
{
	// 座標を格納するのみ //
	m_XMin = rcTargetX256->left;	// 左端の座標
	m_YMin = rcTargetX256->top;		// 上端の座標
	m_XMax = rcTargetX256->right;	// 右端の座標
	m_YMax = rcTargetX256->bottom;	// 下端の座標

	// 自機座標へのポインタ //
	m_pX = pX;		// Ｘ座標
	m_pY = pY;		// Ｙ座標

	// キャラクタ情報を無効化 //
	m_pCharacter = NULL;
}


// デストラクタ //
CExtraShot::~CExtraShot(void)
{
}


// 初期化する //
FVOID CExtraShot::Initialize(CCharacterBase *pCharInfo)
{
	// キャラクタ情報を接続する //
	m_pCharacter = pCharInfo;

	// リスト内のデータを解放する //
	DeleteAllData();
}
