/*
 *   CScoreCtrl.cpp   : スコア管理
 *
 */

#include "CScoreCtrl.h"
#include "FontDraw.h"



// コンストラクタ //
CScoreCtrl::CScoreCtrl(RECT *rcTargetX256, CShaveEffect *pShaveEfc)
{
	m_ox = rcTargetX256->left;	// 表示開始座標(X)
	m_oy = rcTargetX256->top;	// 表示開始座標(Y)

	m_CurrentScore = 0;		// 現在のスコア
	m_DispScore    = 0;		// 表示スコア

	m_pShave = pShaveEfc;	// カスリエフェクト管理
}


// デストラクタ //
CScoreCtrl::~CScoreCtrl()
{
	// 特に何もしません //
}


// 初期化を行う //
FVOID CScoreCtrl::Initialize(__int64 Score)
{
	m_CurrentScore = Score;		// 現在のスコア
	m_DispScore    = Score;		// 表示スコア
}


// 更新する(カウンタまわし) //
FVOID CScoreCtrl::Update(void)
{
	__int64		delta;

	delta = (m_CurrentScore - m_DispScore);
	if(0 == delta) return;

	// 逆方向に移動 /
	if(delta < 0){
		if(delta <= -1000000) m_DispScore -= 1000000;
		if(delta <=  -100000) m_DispScore -=  100000;
		if(delta <=   -10000) m_DispScore -=   10000;
		if(delta <=    -1000) m_DispScore -=    1000;
		if(delta <=     -100) m_DispScore -=     100;
		if(delta <=      -10) m_DispScore -=      10;
		else                  m_DispScore = m_CurrentScore;
	}
	// 正方向に移動 //
	else{
		if(delta >= 1000000) m_DispScore += 1000000;
		if(delta >=  100000) m_DispScore +=  100000;
		if(delta >=   10000) m_DispScore +=   10000;
		if(delta >=    1000) m_DispScore +=    1000;
		if(delta >=     100) m_DispScore +=     100;
		if(delta >=      10) m_DispScore +=      10;
		else                 m_DispScore = m_CurrentScore;
	}
}


// 描画する //
#include <stdio.h>
FVOID CScoreCtrl::Draw(void)
{
	DWORD		Chain;
	int			sx, sy;
	char		buf[100];

	sx = (m_ox >> 8);
	sy = (m_oy >> 8);

	sprintf(buf, "%10I64d", m_DispScore);
	g_Font.Draw(sx+48, sy, buf);

	Chain = m_pShave->GetShaveCombo();
	wsprintf(buf, "x%2d.%01d", 1+Chain/10, Chain%10);

	sx += 216;
	g_Font.Draw(sx, sy, "Rate", 10);
	g_Font.Draw(sx, sy+10, buf, 10);
}


// 現在のスコアに加算する //
FVOID CScoreCtrl::AddScore(__int64 dScore)
{
	m_CurrentScore += dScore;
}


// 現在のスコアを取得する //
__int64 CScoreCtrl::GetScore(void)
{
	return m_CurrentScore;
}
