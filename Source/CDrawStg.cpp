/*
 *   CDrawStg.cpp   : ステージ数表示
 *
 */

#include "CDrawStg.h"
#include "Gian2001.h"
#include "SECtrl.h"



// タスク定数 //
#define TASK_WAIT		0		// 待ち状態のタスク
#define TASK_ACTIVE		1		// アクティブなタスク
#define TASK_VOICE		2		// 音声出力用タスク

// タスクの種類定数 //
#define T_STAGE		0x00	// タスク：ステージ数
#define T_ROUND		0x01	// タスク：ラウンド数
#define T_READY		0x02	// タスク：Ｒｅａｄｙ
#define T_GO		0x03	// タスク：Ｇｏ
#define T_NUM		0x04	// タスク：数字



// コンストラクタ //
CDrawStg::CDrawStg(RECT *rcTargetX256)
{
	m_IsActive = FALSE;		// アクティブなら真

	// 始点のＸ座標 //
	m_ox = (rcTargetX256->right + rcTargetX256->left) / 2;

	// 始点のＹ座標 //
	m_oy = rcTargetX256->top;
}


// デストラクタ //
CDrawStg::~CDrawStg()
{
}


// 表示の準備を行う //
FBOOL CDrawStg::Initialize(int nStage, BOOL Is2P)
{
	PStgTask		*pTask;

	m_IsActive = TRUE;		// アクティブなら真
	m_Count    = 0;			// カウンタ

	// リスト内のデータを解放する //
	DeleteAllData();

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
	// タスク：Ｓｔａｇｅ //
	if(FALSE == Is2P){
		pTask = AddTask(0, 180, T_STAGE, 0);
		if(NULL == pTask) return FALSE;
		pTask->DefTexture(4, 8, 4+108, 8+52);
//		AddVoiceTask(SNDID_V_STAGE, 10);
	}
	// タスク：Ｒｏｕｎｄ //
	else{
		pTask = AddTask(0, 180, T_ROUND, 0);
		if(NULL == pTask) return FALSE;
		pTask->DefTexture(124, 4, 124+132, 4+48);
//		AddVoiceTask(SNDID_V_ROUND, 10);
	}


	// タスク：数字 //
	pTask = AddTask(0, 250, T_NUM, 0);
	if(NULL == pTask) return FALSE;

	switch(nStage){		// ステージごとに表示の切り替えを
	case 1:	pTask->DefTexture( 56, 128,  75, 172);	break;
	case 2:	pTask->DefTexture( 92, 128, 128, 172);	break;
	case 3:	pTask->DefTexture(140, 128, 172, 172);	break;
	case 4: pTask->DefTexture(180, 128, 216, 172);	break;

	case 5:	pTask->DefTexture(  8, 188,  36, 232);	break;
	case 6:	pTask->DefTexture( 52, 188,  80, 232);	break;
	case 7:	pTask->DefTexture( 96, 188, 128, 232);	break;
	case 8:	pTask->DefTexture(140, 188, 168, 232);	break;
	case 9:	pTask->DefTexture(188, 188, 216, 232);	break;

	default:
	case 0:	pTask->DefTexture(  4, 128, 401, 172);	break;
	}

//	AddVoiceTask(SNDID_V_1+nStage-1, 45+5);


	// タスク：Ｒｅａｄｙ //
	pTask = AddTask(0, 180, T_READY, 120);
	if(NULL == pTask) return FALSE;
	pTask->DefTexture(4, 64, 4+120, 64+56);
//	AddVoiceTask(SNDID_V_READY, 135);

	// タスク：Ｇｏ！！ //
	pTask = AddTask(0, 250, T_GO, 120);
	if(NULL == pTask) return FALSE;
	pTask->DefTexture(132, 64, 132+96, 64+48);
//	AddVoiceTask(SNDID_V_GO, 190);

	return TRUE;
}


// 動作させる //
FVOID CDrawStg::Move(void)
{
	Iterator		it;

	// 現在、アクティブではない
	if(FALSE == m_IsActive) return;

	// 発動待ちのタスク //
	ThisForEachFront(TASK_WAIT, it){
		// 待ち時間終了。アクティブタスクに移行する //
		if(0 == it->m_Count){
			MoveBack(it, TASK_ACTIVE);	// 移動する
		}
		else{
			// カウンタをデクリメント //
			it->m_Count -= 1;
		}
	}

	// アクティブなタスク //
	ThisForEachFront(TASK_ACTIVE, it){
		// 動作後に、タスク終了の要求があった場合は //
		if(FALSE == MovePStgTask(it.GetPtr())){
			// 消えて
			ThisDelContinue(it);
		}

		// カウンタをインクリメント //
		it->m_Count += 1;
	}

	// 音声出力用タスク //
	ThisForEachFront(TASK_VOICE, it){
		// 待ち時間終了。アクティブタスクに移行する //
		if(0 == it->m_Count){
			PlayVoice(it->m_Cmd);
			ThisDelContinue(it);
		}
		else{
			// カウンタをデクリメント //
			it->m_Count -= 1;
		}
	}

	if(0 == GetActiveData()){
		m_IsActive = FALSE;
	}
}


// 描画する //
FVOID CDrawStg::Draw(void)
{
	Iterator		it;

	// 現在、アクティブではない
	if(FALSE == m_IsActive) return;

	// 注意：待ち状態のタスクは描画しない //
	ThisForEachFront(TASK_ACTIVE, it){
		DrawPStgTask(it.GetPtr());
	}
}


// タスクごとの動作(偽なら削除) //
FBOOL CDrawStg::MovePStgTask(PStgTask *pTask)
{
	// コマンドによって分岐する //
	switch(pTask->m_Cmd){
	case T_STAGE:	// タスク：ステージ数
	case T_ROUND:	// タスク：ラウンド数
		if(pTask->m_Count < 20){
			pTask->m_XScale += 16;
			pTask->m_YScale += 16;

			if(pTask->m_XScale > 256){
				pTask->m_XScale = 256;
				pTask->m_YScale = 256;
			}
		}
		if(pTask->m_Count > 80){
			pTask->m_XScale += 64;
			pTask->m_YScale -= 16;

			if(pTask->m_YScale < 0){
				pTask->m_YScale = 0;
			}
		}
	break;

	case T_READY:	// タスク：Ｒｅａｄｙ
		if(pTask->m_Count < 20){
			pTask->m_XScale += 16;
			pTask->m_YScale += 16;

			if(pTask->m_XScale > 256){
				pTask->m_XScale = 256;
				pTask->m_YScale = 256;
			}
		}
		if(pTask->m_Count > 80){
			//pTask->m_oy     += 2 * 256;
			pTask->m_XScale += 64;
			pTask->m_YScale -= 16;

			if(pTask->m_YScale < 0){
				pTask->m_YScale = 0;
			}
		}
	break;

	case T_GO:		// タスク：Ｇｏ
		if(pTask->m_Count < 64){
			pTask->m_XScale += 4;
			pTask->m_YScale += 4;

			if(pTask->m_XScale > 256){
				pTask->m_XScale = 256;
				pTask->m_YScale = 256;
			}

			pTask->m_Angle += 8;
		}

		if(pTask->m_Count > 80){
			//pTask->m_oy     -= 2 * 256;
			pTask->m_XScale += 512;
			pTask->m_YScale += 512;
		}
	break;

	case T_NUM:		// タスク：数字
		if(pTask->m_Count < 20){
			pTask->m_XScale += 16;
			pTask->m_YScale += 16;

			if(pTask->m_XScale > 256){
				pTask->m_XScale = 256;
				pTask->m_YScale = 256;
			}
		}
		if(pTask->m_Count > 80){
			pTask->m_XScale -= 16;
			pTask->m_YScale += 128;

			if(pTask->m_XScale < 0){
				pTask->m_XScale = 0;
			}
		}
	break;
	}

	if(pTask->m_Shadow) pTask->m_Shadow -= min(pTask->m_Shadow, 2);

	if(pTask->m_Count > 110) return FALSE;
	return TRUE;
}


// 各タスクごとの描画を行う //
FVOID CDrawStg::DrawPStgTask(PStgTask *pTask)
{
	int				ox, oy;
	int				dx, dy;
	int				w, h;
	DWORD			c;
	BYTE			d;

	D3DTLVERTEX		tlv[20];
	D3DVALUE		u0, v0, u1, v1;

	ox = m_ox + pTask->m_ox;	// 中心となるＸ座標
	oy = m_oy + pTask->m_oy;	// 中心となるＹ座標

	c  = RGBA_MAKE(255, 255, 255, 32);

	// 拡縮と回転用の値を格納 //
	w  = (pTask->m_Width  * pTask->m_XScale) / 255;	// 幅 / 2
	h  = (pTask->m_Height * pTask->m_YScale) / 256;	// 高さ / 2
	d  =  pTask->m_Angle;							// 回転角

	// テクスチャのＵＶ座標を格納 //
	u0 = pTask->m_D3DRect.m_Left;	// 左
	u1 = pTask->m_D3DRect.m_Right;	// 右
	v0 = pTask->m_D3DRect.m_Top;	// 上
	v1 = pTask->m_D3DRect.m_Bottom;	// 下

	dx = CosL(d+128, w) + CosL(d-64, h);
	dy = SinL(d+128, w) + SinL(d-64, h);
	Set2DPointC(tlv+0, (ox+dx)/256, (oy+dy)/256, u0, v0, c);
	Set2DPointC(tlv+2, (ox-dx)/256, (oy-dy)/256, u1, v1, c);

	dx = CosL(d, w) + CosL(d-64, h);
	dy = SinL(d, w) + SinL(d-64, h);
	Set2DPointC(tlv+1, (ox+dx)/256, (oy+dy)/256, u1, v0, c);
	Set2DPointC(tlv+3, (ox-dx)/256, (oy-dy)/256, u0, v1, c);

	g_pGrp->SetTexture(TEXTURE_ID_STAGE);

	int		i, j;
	int		delta = pTask->m_Shadow;

	d = (BYTE)(64 + pTask->m_Count * 4);

	if(delta){
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);

		for(j=0; j<10; j++){
			dx = CosL(d+(j*164)/10, delta);
			dy = SinL(d+(j*164)/10, delta);
			c  = RGBA_MAKE(255, 255, 255, 8 + j*4);
			for(i=0; i<4; i++){
				tlv[i+4] = tlv[i];
				tlv[i+4].sx += dx;
				tlv[i+4].sy += dy;
				tlv[i+4].color = c;
			}
			g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv+4, 4);
		}
	}

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// タスクを追加＆一部初期化 //
PStgTask *CDrawStg::AddTask(int x, int y, BYTE Cmd, DWORD Count)
{
	PStgTask		*pTask;

	// 待ち状態タスクに追加 //
	pTask = InsertBack(TASK_WAIT);

	if(pTask){
		pTask->m_Cmd    = Cmd;		// 動作コマンド
		pTask->m_ox     = x * 256;	// 中心となるＸ座標
		pTask->m_oy     = y * 256;	// 中心となるＹ座標
		pTask->m_Count  = Count;		// 待ちカウント数
		pTask->m_Angle  = 0;			// 回転角
		pTask->m_XScale = 0;			// 倍率(X)
		pTask->m_YScale = 0;			// 倍率(Y)
		pTask->m_Shadow = 128;			// 影？との距離
	}
#ifdef PBG_DEBUG
	else{
		PbgError("CDrawStg::AddTask() 内部エラー：PSTG_TASK_MAX の値をもっと大きくしてくださいな");
	}
#endif

	return pTask;
}


// 音声タスクを追加 //
BOOL CDrawStg::AddVoiceTask(BYTE ID, DWORD Count)
{
	PStgTask		*pTask;

	// 音声出力タスクに追加 //
	pTask = InsertBack(TASK_VOICE);

	if(NULL == pTask){
#ifdef PBG_DEBUG
	PbgError("CDrawStg::AddTask() 内部エラー：PSTG_TASK_MAX の値をもっと大きくしてくださいな");
#endif
		return FALSE;
	}

	pTask->m_Cmd    = ID;			// 音声ＩＤ
	pTask->m_Count  = Count;		// 待ちカウント数

	return TRUE;
}
