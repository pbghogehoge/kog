/*
 *   CDrawWonLost.cpp   : 勝敗表示
 *
 */

#include "CDrawWonLost.h"
#include "Gian2001.h"



// コンストラクタ //
CDrawWonLost::CDrawWonLost(RECT *rcTargetX256)
{
	m_ox = (rcTargetX256->left + rcTargetX256->right) / 2;
	m_oy = rcTargetX256->top;

	m_IsActive = FALSE;
}


// デストラクタ //
CDrawWonLost::~CDrawWonLost()
{
	// 特に何もしません //
}


// 初期化する //
FVOID CDrawWonLost::Initialize(void)
{
	// リスト内のデータを解放する //
	DeleteAllData();

	m_IsActive = FALSE;
}


// セットする //
FVOID CDrawWonLost::Set(BYTE State)
{
	WLTask		*pTask;

	Initialize();

	switch(State){
	case DWL_WON:	// 「勝ち」の場合
		// Ｗ //
		pTask = AddTask(-60, -50, 53, 44, 100);
		pTask->m_D3DRect.Set(0, 160, 53, 204, 256);

		// Ｏ //
		pTask = AddTask(0, -80, 43, 44, 100);
		pTask->m_D3DRect.Set(53, 160, 96, 204, 256);

		// Ｎ //
		pTask = AddTask(60, -110, 40, 44, 100);
		pTask->m_D3DRect.Set(96, 160, 136, 204, 256);
	break;

	case DWL_LOST:	// 「負け」の場合
		// Ｌ //
		pTask = AddTask(-100, -50, 33, 44, 100);
		pTask->m_D3DRect.Set(0, 208, 33, 252, 256);

		// Ｏ //
		pTask = AddTask(-35, -70, 43, 44, 100);
		pTask->m_D3DRect.Set(33, 208, 76, 252, 256);

		// Ｓ //
		pTask = AddTask(35, -90, 37, 44, 100);
		pTask->m_D3DRect.Set(76, 208, 113, 252, 256);

		// Ｔ //
		pTask = AddTask(100, -110, 37, 44, 100);
		pTask->m_D3DRect.Set(113, 208, 150, 252, 256);
	break;

	case DWL_DRAW:	// 「引き分け」の場合
		// Ｄ //
		pTask = AddTask(-100, -50, 40, 44, 100);
		pTask->m_D3DRect.Set(158, 160, 198, 204, 256);

		// Ｒ //
		pTask = AddTask(-35, -70, 38, 44, 100);
		pTask->m_D3DRect.Set(202, 160, 240, 204, 256);

		// Ａ //
		pTask = AddTask(35, -90, 45, 44, 100);
		pTask->m_D3DRect.Set(156, 208, 201, 252, 256);

		// Ｗ //
		pTask = AddTask(100, -110, 54, 44, 100);
		pTask->m_D3DRect.Set(200, 208, 254, 252, 256);
	break;
	}

	// 初期化完了 //
	m_IsActive = TRUE;
}


// １フレームだけ動作させる //
FVOID CDrawWonLost::Move(void)
{
	Iterator		it;
	WLTask			*pTask;

	// 重力による加速と跳ね返りを考慮に入れた移動処理 //
	ThisForEachFront(0, it){
		pTask = it.GetPtr();

		pTask->m_vy += 128;
		pTask->m_y  += pTask->m_vy;

		// 地面との衝突 //
		if(pTask->m_y >= pTask->m_ymax){
			pTask->m_vy = -(pTask->m_vy * 2 / 3);		// 速度反転＆減衰
			pTask->m_y -= (pTask->m_y - pTask->m_ymax);	// Ｙ座標を補正

			if(abs(pTask->m_vy) < 8){
				pTask->m_y  = pTask->m_ymax;
				pTask->m_vy = 0;
			}
		}

		pTask->m_Count++;
	}
}


// 描画する //
FVOID CDrawWonLost::Draw(void)
{
	Iterator		it;
	WLTask			*pTask;

	D3DTLVERTEX		tlv[30];
	D3DVALUE		u1, v1, u2, v2;
	DWORD			c;
	int				x, y, w, h;

	c = RGBA_MAKE(255, 255, 255, 128);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_BONUS);

	ThisForEachFront(0, it){
		pTask = it.GetPtr();

		x  = pTask->m_x >> 8;	// 現在のＸ座標
		y  = pTask->m_y >> 8;	// 現在のＹ座標

		w  = pTask->m_Width;	// 画像の幅
		h  = pTask->m_Height;	// 画像の高さ

		u1 = pTask->m_D3DRect.m_Left;	// テクスチャ 左
		v1 = pTask->m_D3DRect.m_Top;	// テクスチャ 上
		u2 = pTask->m_D3DRect.m_Right;	// テクスチャ 右
		v2 = pTask->m_D3DRect.m_Bottom;	// テクスチャ 下

		Set2DPointC(tlv+0, x,   y,   u1, v1, c);
		Set2DPointC(tlv+1, x+w, y,   u2, v1, c);
		Set2DPointC(tlv+2, x+w, y+h, u2, v2, c);
		Set2DPointC(tlv+3, x,   y+h, u1, v2, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// タスクを追加する                                  //
// 引数    x, y : 原点からの相対座標(非x256)         //
//         w, h : 画像の幅と高さ(非x256)             //
//         ymax : いわゆる地面のｙ座標(相対, 非x256) //
WLTask *CDrawWonLost::AddTask(int x, int y, int w, int h, int ymax)
{
	WLTask		*pTask;

	pTask = InsertBack(0);
	if(NULL == pTask) return NULL;

	pTask->m_Count = 0;
	pTask->m_vy    = 0;
	pTask->m_x     = m_ox + (x * 256) - (w * 128);
	pTask->m_y     = m_oy + (y * 256) - (h * 128);
	pTask->m_ymax  = m_oy + (ymax * 256) - (h * 128);

	pTask->m_Width  = w;	// 画像の幅
	pTask->m_Height = h;	// 画像の高さ

	return pTask;
}


// エフェクトが完了していれば真を返す //
FBOOL CDrawWonLost::IsFinished(void)
{
	Iterator		it;
	WLTask			*pTask;

	// 重力による加速と跳ね返りを考慮に入れた移動処理 //
	ThisForEachFront(0, it){
		pTask = it.GetPtr();

		// まだ完了していない場合 //
		if(pTask->m_y != pTask->m_ymax) return FALSE;
	}

	return TRUE;
}
