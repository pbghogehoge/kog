/*
 *   SECtrl.cpp   : ＳＥ管理関数
 *
 */

#include "SECtrl.h"
#include "CMusicCtrl.h"

#define SYSSE_MAX		20
//#define STDSE_MAX		20



static Pbg::CSoundBuffer	*g_pSysSE[SYSSE_MAX];
static int					g_SEOrg = 0;



// システム効果音の初期化(Load) //
FBOOL InitSE(BYTE InitVolume)
{
	DWORD		i;
	DWORD		NumSound[SYSSE_MAX];

	NumSound[SNDID_CANCEL]    = 1;
	NumSound[SNDID_CHARGED]   = 2;
	NumSound[SNDID_STG2_MOVE] = 2;

	NumSound[SNDID_GBREAK]               = 2;
	NumSound[SNDID_SHAVE]                = 4;
	NumSound[SNDID_DAMAGE_NORMAL]        = 1;
	NumSound[SNDID_DAMAGE_CHARGE]        = 1;
	NumSound[SNDID_ENEMY_DESTROY]        = 1;
	NumSound[SNDID_TRI_STAR_LARGE]       = 2;
	NumSound[SNDID_TRI_STAR_SMALL]       = 4;
	NumSound[SNDID_METEORIC_BLAST_FALL]  = 2;
	NumSound[SNDID_METEORIC_BLAST_CRASH] = 2;
	NumSound[SNDID_TWIN_LASER]           = 2;
	NumSound[SNDID_G_FLAME]              = 2;
	NumSound[SNDID_G_LASER]              = 8;	//
	NumSound[SNDID_MISSILE_SET]          = 1;
	NumSound[SNDID_MISSILE_ACCEL]        = 1;
	NumSound[SNDID_LOCKON]               = 2;
	NumSound[SNDID_H_L_B]                = 2;
	NumSound[SNDID_LIGHTNING_P]          = 4;
/*
	NumSound[SNDID_V_STAGE] = 1;
	NumSound[SNDID_V_ROUND] = 1;
	NumSound[SNDID_V_READY] = 1;
	NumSound[SNDID_V_GO]    = 1;
	NumSound[SNDID_V_1]     = 1;
	NumSound[SNDID_V_2]     = 1;
	NumSound[SNDID_V_3]     = 1;
	NumSound[SNDID_V_4]     = 1;
	NumSound[SNDID_V_5]     = 1;
	NumSound[SNDID_V_6]     = 1;
	NumSound[SNDID_V_7]     = 1;
	NumSound[SNDID_V_8]     = 1;
	NumSound[SNDID_V_9]     = 1;
*/

	if(NULL == g_pSnd) return FALSE;

	// 初の呼び出し、もしくはデバイス切り替えにより、//
	// バッファが破棄されている場合、再度確保する    //
	for(i=0; i<SYSSE_MAX; i++){
		if(NULL == g_pSysSE[i]){
			// サウンドバッファが作成できないときはさよなら //
			if(FALSE == g_pSnd->CreateSoundBuffer(&(g_pSysSE[i]))){
				PbgError("サウンドバッファが作成出来ませんでした");
				return FALSE;
			}
		}

		if(FALSE == g_pSysSE[i]->LoadP("SOUND.DAT", i, NumSound[i])){
			PbgError("SOUND.DAT からデータがロードできませんでした");
		}
	}

	// マスターボリュームをセットする //
	g_pSnd->SetMasterVolume(InitVolume);

	return TRUE;
}


// システム効果音を鳴らす //
FVOID PlaySE(DWORD nID)
{
	if(nID >= SYSSE_MAX){
		PbgError("内部エラー ＠ PlaySE() : 範囲外");
		return;	// 範囲外
	}

	if(NULL == g_pSysSE[nID]) return;	// 準備が出来ていない

	// ここまで来れば、再生できる //
	g_pSysSE[nID]->Play();
}


// 音声を再生する(Volume 最大) //
FVOID PlayVoice(DWORD nID)
{
	if(nID >= SYSSE_MAX){
		PbgError("内部エラー ＠ PlaySE() : 範囲外");
		return;	// 範囲外
	}

	if(NULL == g_pSysSE[nID]) return;	// 準備が出来ていない

	// ここまで来れば、再生できる //
	g_pSysSE[nID]->Play(0, DSBFREQUENCY_ORIGINAL, 255);
}


// パン付き効果音を鳴らす //
FVOID PlaySE(DWORD nID, int Pan)
{
	if(nID >= SYSSE_MAX){
		PbgError("内部エラー ＠ PlaySE() : 範囲外");
		return;	// 範囲外
	}

	if(NULL == g_pSysSE[nID]) return;	// 準備が出来ていない

	// ここまで来れば、再生できる //
	g_pSysSE[nID]->Play((Pan-g_SEOrg)/4, DSBFREQUENCY_ORIGINAL, 200);
}


// パン指定用の中心座標を決める //
FVOID SetSEOrg(int org)
{
	g_SEOrg = org;
}
