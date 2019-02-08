/*
 *   CCharacterBase.cpp   : キャラクタの定義(基底)
 *
 */

#include "CCharacterBase.h"
#include "SCL.h"



// 通常移動の速度を取得する //
FINT CCharacterBase::GetNormalSpeed(void)
{
	return m_NormalSpeed;
}


// シフト移動の速度を取得する //
FINT CCharacterBase::GetShiftSpeed(void)
{
	return m_ShiftSpeed;
}


// 溜める早さを求める //
FDWORD CCharacterBase::GetChargeSpeed(void)
{
	return m_ChargeSpeed;
}


// カスり溜めの早さを求める //
FDWORD CCharacterBase::GetBuzzSpeed(void)
{
	return m_BuzzSpeed;
}


// 通常ショットの発射座標のシフト値(x) //
FINT CCharacterBase::GetNShotDx(void)
{
	return m_NormalShotDx;
}


// 通常ショットの発射座標のシフト値(y) //
FINT CCharacterBase::GetNShotDy(void)
{
	return m_NormalShotDy;
}


// ボムの有効時間を求める //
FINT CCharacterBase::GetBombTime(void)
{
	return m_BombTime;
}


// キャラクタ固有のＩＤを取得する //
CHARACTER_ID CCharacterBase::GetCharID(void)
{
	return m_CharID;
}


// ノーマルアタック(レベル２) //
FVOID CCharacterBase::Level2Attack(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv)
{
	pRivalSCLDecoder->SetExAttackLv1(m_CharID, AtkLv);
}


// ノーマルアタック(レベル３) //
FVOID CCharacterBase::Level3Attack(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv)
{
	pRivalSCLDecoder->SetExAttackLv2(m_CharID, AtkLv);
}


// ノーマルアタック(ボスアタック) //
FVOID CCharacterBase::Level4Attack(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv)
{
	pRivalSCLDecoder->SetBossAttack(m_CharID, AtkLv);
}


// ボムアタック(レベル２) //
FBOOL CCharacterBase::Level2BombAtk(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv)
{
	return FALSE;
}


// ボムアタック(レベル３) //
FBOOL CCharacterBase::Level3BombAtk(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv)
{
	return FALSE;
}


// ボムアタック(ボスアタック) //
FBOOL CCharacterBase::Level4BombAtk(CSCLDecoder *pRivalSCLDecoder, BYTE AtkLv)
{
	return FALSE;
}

/*
// AtkGrp のデフォルト初期化用関数 //
FVOID CCharacterBase::InitDefaultAtkGrp(AtkGrpInfo *pInfo)
{
	pInfo->m_Alpha  = 0;			// α補正値
	pInfo->m_Count  = 0;			// カウンタ
	pInfo->m_DeltaY = 256 * 256;	// Ｙ座標補正値
	pInfo->m_Size   = 255;			// 倍率
}
*/
