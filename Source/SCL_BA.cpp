/*
 *   SCL_BA.cpp   : ボスモード用SCL
 *
 */

#include "SCL_BA.h"



// コンストラクタ //
CSCLBADecoder::CSCLBADecoder(RECT *rcTargetX256, CEnemyCtrl *pEnemyCtrl)
	: CSCLDecoder(rcTargetX256, pEnemyCtrl)
{
}


// デストラクタ //
CSCLBADecoder::~CSCLBADecoder()
{
}


// バッファ内のデータをリセットする //
BOOL CSCLBADecoder::Initialize(Pbg::CRnd *pRnd)
{
	int			i;

	m_BACount = 0;

	for(i=0; i<9; i++){
		m_BossID[i] = i;
	}
	Pbg::Shuffle(m_BossID, 9, pRnd);
	Pbg::Shuffle(m_BossID, 9, pRnd);

	return CSCLDecoder::Initialize(1);
//	return FALSE;
}


// １フレーム分バッファを進める //
void CSCLBADecoder::Proceed(void)
{
	if(m_BACount < 9 && (!IsBossAlive())){
		CSCLBADecoder::SetBossAttack(
			m_BossID[m_BACount], min(25, 5+m_BACount*2));
			//m_BossID[m_BACount], min(25, 5+m_BACount*2));

		m_BACount++;
	}
}


// ＳＣＬデータをロードする //
BOOL CSCLBADecoder::LoadBA(
				char			*pFileID,	// ファイルＩＤ
				CHARACTER_ID	P1)			// プレイヤー１の使用キャラ
{
	BYTE				*pTextureBlock;
	int					TexInitAddr;
	SCLHeader			*pSCLHead;

	// テクスチャ以外のロードを行う //
	if(FALSE == LoadWithoutTexture(pFileID)) return FALSE;


///////////////////////////////////////////////////////////////////////////////
//              では、テクスチャのロードを行いましょう                       //
///////////////////////////////////////////////////////////////////////////////

	// 先頭へのポインタを取得 //
	pSCLHead = (SCLHeader *)Offset2Ptr(0);

	// テクスチャ初期化関数の開始オフセットを格納 //
	TexInitAddr = pSCLHead->TexInitializer;

	// アドレス確定(キャストに注意 : オフセットはバイト指定) //
	pTextureBlock = ((BYTE *)pSCLHead) + TexInitAddr;

	// テクスチャのロード //
	if(FALSE == CEnemyAnimeContainer::LoadBA(pTextureBlock, P1)){
		return FALSE;
	}
//PbgError("LoadBA - OUT");
	return TRUE;
}
