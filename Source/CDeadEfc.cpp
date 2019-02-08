/*
 *   CDeadEfc.cpp   : 死亡エフェクト描画
 *
 */

#include "CDeadEfc.h"
#include "RndCtrl.h"

#define DEFC_CIRCLE		0x00	// 円エフェクト
#define DEFC_LINE		0x01	// ラインエフェクト
#define DEFC_BOMB		0x02	// 爆発エフェクト

#define DEFC_NUMCIRCLE	8							// 円エフェクトの数
#define DEFC_NUMBOMB	30							// 爆発系エフェクトの数

#define DEFC_NUMLINE	\
	(DEFC_MAX - DEFC_NUMCIRCLE - DEFC_NUMBOMB)		// ラインエフェクトの数



// コンストラクタ //
CDeadEfc::CDeadEfc()
{
	// 特に何もしません //
}


// デストラクタ //
CDeadEfc::~CDeadEfc()
{
	// 特に何もしません //
}


// エフェクトの除去 //
FVOID CDeadEfc::Initialize(void)
{
	// リストを初期化する //
	DeleteAllData();
}


// エフェクトをセットする //
FVOID CDeadEfc::Set(int x, int y)
{
	int				i;
	int				l;
	BYTE			d;
	DEfcTask		*pTask;

	// リストを初期化する //
	DeleteAllData();

	// 円形エフェクトをセット //
	for(i=0; i<DEFC_NUMCIRCLE; i++){
		pTask = InsertBack(DEFCTASK_WAIT);
		if(NULL == pTask) return;

		pTask->Type   = DEFC_CIRCLE;
		pTask->x      = x;
		pTask->y      = y;
		pTask->Length = 0;

		if(i < DEFC_NUMCIRCLE / 2){
			pTask->Count = 5 + i * 15;
			pTask->Delta = (char)196;
		}
		else{
			pTask->Count = 10 + ((DEFC_NUMCIRCLE/2-1) * 15) + i * 5;
			pTask->Delta = 64+16;
		}
	}

	// 爆発エフェクトをセット //
	for(i=0; i<DEFC_NUMBOMB; i++){
		pTask = InsertBack(DEFCTASK_WAIT);
		if(NULL == pTask) return;

		d = (BYTE)(RndEx()>>1);
		l = (RndEx() % 140) * 256;

		pTask->Type   = DEFC_BOMB;
		pTask->x      = x + CosL(d, l);
		pTask->y      = y + SinL(d, l);
		pTask->Length = (40 + RndEx()%40) * 256;
		pTask->Alpha  = 0;
		pTask->Count  = (i * 110) / DEFC_NUMBOMB;
	}

	// ラインエフェクトをセット //
	for(i=0; i<DEFC_NUMLINE; i++){
		pTask = InsertBack(DEFCTASK_WAIT);
		if(NULL == pTask) return;

		pTask->Type   = DEFC_LINE;
		pTask->x      = x;
		pTask->y      = y;
		pTask->Length = (RndEx()%100) * 256;
		pTask->Count  = (i * 40) / DEFC_NUMLINE;
		pTask->Angle  = (BYTE)RndEx();

		pTask->Delta  = 1 + RndEx() % 2;

		if((RndEx() >> 2) & 1){
			pTask->Delta = -(pTask->Delta);
		}
	}
}


// エフェクトを１フレーム動作させる //
FVOID CDeadEfc::Move(void)
{
	DEfcTask		*pTask;
	Iterator		it;
	BOOL			ret;

	// 待ち状態にあるエフェクトを更新する //
	ThisForEachFront(DEFCTASK_WAIT, it){
		pTask = it.GetPtr();	// ポインタ取得

		// 残り時間が存在すればデクリメント //
		// 相でなければ、通常状態に移行する //
		if(pTask->Count) pTask->Count--;
		else             MoveBack(it, DEFCTASK_NORM);
	}

	// 通常状態にあるエフェクトを更新する //
	ThisForEachFront(DEFCTASK_NORM, it){
		pTask = it.GetPtr();	// ポインタ取得

		// 種類別に更新を行う //
		switch(pTask->Type){
			case DEFC_CIRCLE:	// 円エフェクト
				ret = MoveCircle(pTask);
			break;

			case DEFC_LINE:		// ラインエフェクト
				ret = MoveLine(pTask);
			break;

			case DEFC_BOMB:		// 爆発エフェクト
				ret = MoveBomb(pTask);
			break;

			default:			// 強制的に消去する
				ret = FALSE;
			break;
		}

		// 消去要求が出ているなら消去する //
		if(FALSE == ret){
			ThisDelContinue(it);
		}

		// カウンタをインクリメント //
		pTask->Count++;
	}
}


// エフェクトを描画する //
FVOID CDeadEfc::Draw(void)
{
	DEfcTask		*pTask;
	Iterator		it;

	// 通常状態にあるエフェクトを更新する //
	ThisForEachFront(DEFCTASK_NORM, it){
		pTask = it.GetPtr();	// ポインタ取得

		// 種類別に描画を行う //
		switch(pTask->Type){
			case DEFC_CIRCLE:	// 円エフェクト
				DrawCircle(pTask);
			break;

			case DEFC_LINE:		// ラインエフェクト
				DrawLine(pTask);
			break;

			case DEFC_BOMB:		// 爆発エフェクト
				DrawBomb(pTask);
			break;

			default:	// ここには来ないハズなのですが...
			continue;
		}
	}
}


// 円状態の動作 //
FBOOL CDeadEfc::MoveCircle(DEfcTask *pTask)
{
	const DWORD FinishCount = 60;
	int			t;

	// 終了時刻が訪れたので、偽を返す //
	if(pTask->Count >= FinishCount) return FALSE;

	t = abs(pTask->Count - (FinishCount / 2));
	t = (FinishCount / 2) - t;
	pTask->Alpha = (BYTE)( (255 * t) / (FinishCount / 2) );

	// 円の半径を拡大する //
	pTask->Length += (pTask->Count * ((BYTE)pTask->Delta));

	return TRUE;
}


// ライン状態の動作 //
FBOOL CDeadEfc::MoveLine(DEfcTask *pTask)
{
	const DWORD FinishCount = 50;
	int			t;

	if(pTask->Count >= FinishCount) return FALSE;

	t = abs(pTask->Count - (FinishCount / 2));
	t = (FinishCount / 2) - t;

	pTask->Angle  += pTask->Delta;
	pTask->Length += 256;
	pTask->Alpha   = (BYTE)( (255 * t) / (FinishCount / 2) );

	return TRUE;
}


// 爆発状態の動作 //
FBOOL CDeadEfc::MoveBomb(DEfcTask *pTask)
{
	const DWORD FinishCount = 30;
	int			t;

	if(pTask->Count >= FinishCount) return FALSE;

	t = abs(pTask->Count - (FinishCount / 2));
	t = (FinishCount / 2) - t;

	pTask->Alpha = (BYTE)( (255 * t) / (FinishCount / 2) );

	return TRUE;
}


// 円状態の描画 //
FVOID CDeadEfc::DrawCircle(DEfcTask *pTask)
{
	D3DTLVERTEX		tlv[20];
	int				x, y, w;
	DWORD			c;
	BYTE			a;

	w = pTask->Length >> 8;	// 円の半径
	x = (pTask->x) >> 8;	// エフェクト中心のＸ座標
	y = (pTask->y) >> 8;	// エフェクト中心のＹ座標
	a = pTask->Alpha;		// 半透明の具合

	c = RGBA_MAKE(a/8, a/5, a, 255);

	Set2DPointC(tlv+0, x-w, y-w, 0.0, 0.0, c);
	Set2DPointC(tlv+1, x+w, y-w, 1.0, 0.0, c);
	Set2DPointC(tlv+2, x+w, y+w, 1.0, 1.0, c);
	Set2DPointC(tlv+3, x-w, y+w, 0.0, 1.0, c);

	g_pGrp->SetTexture(TEXTURE_ID_GUARD);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// ライン状態の描画 //
FVOID CDeadEfc::DrawLine(DEfcTask *pTask)
{
	D3DTLVERTEX		tlv[20];
	int				x, y, l;
	int				dx1, dx2, dy1, dy2;
	BYTE			a;
	DWORD			c;

	l = pTask->Length;		// ラインの長さ
	x = (pTask->x) >> 8;	// エフェクト中心のＸ座標
	y = (pTask->y) >> 8;	// エフェクト中心のＹ座標

	dx1 = CosL(pTask->Angle-2, l) >> 8;
	dy1 = SinL(pTask->Angle-2, l) >> 8;
	dx2 = CosL(pTask->Angle+2, l) >> 8;
	dy2 = SinL(pTask->Angle+2, l) >> 8;

	a = pTask->Alpha;
	c = RGBA_MAKE(a/8, a/5, a, 255);

	Set2DPointC(tlv+0, x,     y,     0, 0, c);
	Set2DPointC(tlv+1, x+dx1, y+dy1, 0, 0, c);
	Set2DPointC(tlv+2, x+dx2, y+dy2, 0, 0, c);

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 3);
}


// 爆発状態の描画 //
FVOID CDeadEfc::DrawBomb(DEfcTask *pTask)
{
	D3DTLVERTEX		tlv[20];
	int				x, y, w;
	BYTE			a;
	DWORD			c;

	w = (pTask->Length >> 8);	// 爆発半径
	x = (pTask->x) >> 8;		// エフェクト中心のＸ座標
	y = (pTask->y) >> 8;		// エフェクト中心のＹ座標

	a = pTask->Alpha;
	c = RGBA_MAKE(a, a, a, 255);

	Set2DPointC(tlv+0, x-w, y-w, 0.0, 0.0, c);
	Set2DPointC(tlv+1, x+w, y-w, 1.0, 0.0, c);
	Set2DPointC(tlv+2, x+w, y+w, 1.0, 1.0, c);
	Set2DPointC(tlv+3, x-w, y+w, 0.0, 1.0, c);

	g_pGrp->SetTexture(TEXTURE_ID_ATKEFC);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}
