/*
 *   SECtrl.h   : ＳＥ管理関数
 *
 */

#ifndef SECTRL_INCLUDED
#define SECTRL_INCLUDED "ＳＥ管理関数 : Version 0.01 : Update 2001/07/04"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/04 : 久々の
 */



#include "PbgType.h"
#include "SoundID.h"



/***** [関数プロトタイプ] *****/
FBOOL InitSE(BYTE InitVolume);		// システム効果音の初期化(Load)
FVOID PlayVoice(DWORD nID);			// 音声を再生する(Volume 最大！)
FVOID PlaySE(DWORD nID);			// システム効果音を鳴らす
FVOID PlaySE(DWORD nID, int Pan);	// パン付き効果音を鳴らす
FVOID SetSEOrg(int org);			// パン指定用の中心座標を決める



#endif
