/*
 *   CStdAttack.cpp   : 標準弾送り
 *
 */

#include "CStdAttack.h"

#include "RndCtrl.h"



/*
// 弾発射コマンド定数(WORD)                     //
// 参考：上位が角度指定、下位がオプションとなる //
#define TCMD_WAY		0x0000		// 扇状発射				0000 ...
#define TCMD_ALL		0x0100		// 全方向発射			0001 ...
#define TCMD_RND		0x0200		// ランダム方向発射		0010 ...
#define TCMD_ONE		0x0300		// １方向発射			0011 ...

#define TCMD_SET		0x0001		// 自機セット属性			0000 0001
#define TCMD_NUM		0x0002		// 連弾属性					0000 0010
#define TCMD_NUMSHIFT	0x0006		// 連弾 + 角度ずらし有効	0000 0110
#define TCMD_RNDSPD		0x0008		// 速度ランダム属性			0000 1000
#define TCMD_NODIF		0x0010		// 難易度反映無効化フラグ	0001 0000

#define TCMD_SHIFT		0x0004		// 角度ズラシ有効
*/

#define STAMA_SEND		0x00		// 送り状態
#define STAMA_WAIT		0x01		// 待ち状態
#define STAMA_CLEAR		0x02		// 消去状態

#define STAMA_SENDDELAY		40		// エフェクト送りに必要な時間
#define STAMA_GRAVITY		50		// エフェクトにかかる重力加速度



// コンストラクタ //
CStdAttack::CStdAttack()
{
	m_pTarget = NULL;
}


// デストラクタ //
CStdAttack::~CStdAttack()
{
}


// 初期化：セット対象を決める //
FBOOL CStdAttack::Initialize(CEnemyTama *pTarget, RECT *rcTargetX256)
{
	if(NULL == pTarget) return FALSE;

	// 対象となる座標 //
	m_XMin = rcTargetX256->left   + (32*256);	// 左端の座標
	m_YMin = rcTargetX256->top    + (32*256);	// 上端の座標
	m_XMax = rcTargetX256->right  - (32*256);	// 右端の座標
	m_YMax = rcTargetX256->bottom - (32*256);	// 下端の座標

	m_pTarget    = pTarget;		// 発射対象を確定
	m_CurrentCmd = TCMD_RND;	// 初期状態をセット

	// リスト内のデータを解放 //
	DeleteAllData();

	return TRUE;
}


// 弾を送りつける //
FVOID CStdAttack::Set(int sx, int sy, DWORD Level)
{
	AtkWarning		*pWarn;
	AttackCommand	*pAtk;
	int				ox, oy;
	int				dx, dy;
	int				t;
	BYTE			Angle;

	pWarn = InsertBack(STAMA_SEND);
	if(NULL == pWarn) return;			// 生成数が最大数を超えている

	// 相手に届くまでの時間 //
	t = STAMA_SENDDELAY;

	pAtk           = &(pWarn->m_Cmd);	// コマンドセットの準備
	pWarn->m_Count = t;					// カウンタをリセット(相手に届くまでの時間)
	pWarn->m_x     = sx;				// 現在のＸ座標
	pWarn->m_y     = sy;				// 現在のＹ座標
	pWarn->m_Size  = 10 + Level * 2;	// 最終サイズ×２

	// セットする座標(ここは重要！！) //
	ox = m_XMin + (RndEx() % ((m_XMax   - m_XMin)/256)) * 256;
	oy = m_YMin + (RndEx() % ((m_YMax/5 - m_YMin)/256)) * 256;

	// 重力絡みの動きで、相手フィールドに移動するので、//
	// そのためのパラメータを準備するのです            //
	dx = sx - ox;	// Ｘ方向のベクトル(つまり、どれだけ離れているのか)
	dy = sy - oy;	// Ｙ方向のベクトル

	// 速度成分に変換する //
	dx = dx / t;							// Ｙ方向は加速度成分に注意
	dy = dy / t - (STAMA_GRAVITY * t) / 2;	// y = vt + (g/2) * (t^2) より

	// 射出方向を決める(斜め投げ) //
	Angle = pWarn->m_Angle = Atan8(dx, dy);

	// ０じゃない方で、割ってやりましょう //
	if(CosM(Angle)) pWarn->m_Speed = (dx * 256) / CosM(Angle);
	else            pWarn->m_Speed = (dy * 256) / SinM(Angle);

	// 発射コマンドと攻撃レベルを考慮に入れて、弾をセットする //
	switch(m_CurrentCmd){
	case TCMD_WAY:
		pAtk->Cmd      = TCMD_WAY | TCMD_NUM | TCMD_SET;
		pAtk->Type     = TAMA_NORM;
		pAtk->Speed    = 256*2 - 128 + 32 + min(256, Level * 6);
		pAtk->ox       = ox;
		pAtk->oy       = oy;
		pAtk->Color    = ETAMA_NORMAL16 + 5;
		pAtk->Num      = (BYTE)((Level / 2) * 2 + 1);
		pAtk->NumS     = (BYTE)min(255, Level/10 + 1);
		pAtk->wAngle   = 6;//64 - (Level / 10);
		pAtk->Angle    = 0;
		pAtk->AnmSpd   = -5;
		pAtk->Option   = 0;
		pAtk->EffectID = 0;

		m_CurrentCmd = TCMD_ALL;	// 次の発動の為の準備
	return;

	case TCMD_ALL:
		pAtk->Cmd      = TCMD_ALL;
		pAtk->Type     = TAMA_NORM;
		pAtk->Speed    = 256*2 - 128 + 32 + min(256, Level * 6);
		pAtk->ox       = ox;
		pAtk->oy       = oy;
		pAtk->Color    = ETAMA_NORMAL16 + 5;
		pAtk->Num      = 5 + (BYTE)min(255-8, 1 + Level);
		pAtk->NumS     = 1;
		pAtk->Angle    = (BYTE)RndEx();
		pAtk->AnmSpd   = 5;
		pAtk->Option   = 0;
		pAtk->EffectID = 0;

		m_CurrentCmd = TCMD_RND;	// 次の発動の為の準備
	return;

	case TCMD_RND:
		pAtk->Cmd      = TCMD_RND | TCMD_SET | TCMD_NUM | TCMD_RNDSPD;
		pAtk->Type     = TAMA_NORM;
		pAtk->Speed    = 256*2 - 128 + 32 + min(256, Level * 8);
		pAtk->ox       = ox;
		pAtk->oy       = oy;
		pAtk->Color    = ETAMA_NORMAL8 + 5;
		pAtk->Num      = (BYTE)min(255, Level/4+1);
		pAtk->NumS     = (BYTE)(1 + Level / 8);
		pAtk->wAngle   = (BYTE)(8 + (Level / 10) * 2);
		pAtk->Angle    = 0;		//32 + RndEx()%64;
		pAtk->AnmSpd   = 5;
		pAtk->Option   = 0;
		pAtk->EffectID = 0;

		m_CurrentCmd = TCMD_WAY;	// 次の発動の為の準備
	return;
	}
}


// １フレーム分動作させる //
FVOID CStdAttack::Move(void)
{
	Iterator		it;
	AtkWarning		*pWarn;

	int				Speed;
	int				ox, oy;
	BYTE			Angle;
	DWORD			t;

	// 送っている途中の弾 //
	ThisForEachFront(STAMA_SEND, it){
		pWarn = it.GetPtr();

		// 移動中 //
		if(pWarn->m_Count){
			Angle = pWarn->m_Angle;
			Speed = pWarn->m_Speed;
			t     = pWarn->m_Count;

			ox = pWarn->m_Cmd.ox;
			oy = pWarn->m_Cmd.oy;

			pWarn->m_x = ox + CosL(Angle, Speed) * t;
			pWarn->m_y = oy + SinL(Angle, Speed) * t + (STAMA_GRAVITY * t * t) / 2;

			pWarn->m_Count--;
		}
		// 移動完了 //
		else{
			pWarn->m_x     = pWarn->m_Cmd.ox;
			pWarn->m_y     = pWarn->m_Cmd.oy;
			pWarn->m_Count = pWarn->m_Size;		// サイズが待ち時間となる

			MoveBack(it, STAMA_WAIT);		// 移動
		}
	}

	// 発射待ちの弾 //
	ThisForEachFront(STAMA_WAIT, it){
		pWarn = it.GetPtr();

		// 準備中 //
		if(pWarn->m_Count){
			pWarn->m_Count--;
		}
		// 打ち出して、後は消えるのみ //
		else{
			m_pTarget->Set(pWarn->m_Cmd);	// セットしたら
			ThisDelContinue(it);			// 消去
		}
	}

	// エフェクトの消去中 //
	ThisForEachFront(STAMA_CLEAR, it){
		pWarn = it.GetPtr();

		// 小さくなっているところ //
		if(pWarn->m_Count) pWarn->m_Count--;
		else               ThisDelContinue(it);
	}
}


// 描画する //
FVOID CStdAttack::Draw(void)
{
	const static BYTE dAngle[4] = {0, 85, 170, 0};

	Iterator		it;
	AtkWarning		*pWarn;

	D3DTLVERTEX		tlv[20];

	int		dx, dy, ox, oy;
//	int		i, j;
	int		count;
	DWORD	c;

	g_pGrp->SetTexture(TEXTURE_ID_ATKEFC);
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);

	c = RGBA_MAKE(255, 255, 255, 255);

	// 送っている途中の弾 //
	ThisForEachFront(STAMA_SEND, it){
		pWarn = it.GetPtr();
		count = pWarn->m_Count;

		ox = pWarn->m_x >> 8;
		oy = pWarn->m_y >> 8;

		dx = (pWarn->m_Size * (STAMA_SENDDELAY-count)) / STAMA_SENDDELAY;
		dy = (pWarn->m_Size * (STAMA_SENDDELAY-count)) / STAMA_SENDDELAY;
		Set2DPointC(tlv+0, ox-dx, oy-dy, 0.0, 0.0, c);
		Set2DPointC(tlv+1, ox+dx, oy-dy, 1.0, 0.0, c);
		Set2DPointC(tlv+2, ox+dx, oy+dy, 1.0, 1.0, c);
		Set2DPointC(tlv+3, ox-dx, oy+dy, 0.0, 1.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// 発射待ちの弾 //
	ThisForEachFront(STAMA_WAIT, it){
		pWarn = it.GetPtr();
		count = pWarn->m_Count;

		ox = pWarn->m_x >> 8;
		oy = pWarn->m_y >> 8;

		dx = count;
		dy = count;
		Set2DPointC(tlv+0, ox-dx, oy-dy, 0.0, 0.0, c);
		Set2DPointC(tlv+1, ox+dx, oy-dy, 1.0, 0.0, c);
		Set2DPointC(tlv+2, ox+dx, oy+dy, 1.0, 1.0, c);
		Set2DPointC(tlv+3, ox-dx, oy+dy, 0.0, 1.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// エフェクトの消去中 //
	ThisForEachFront(STAMA_CLEAR, it){
		pWarn = it.GetPtr();
		count = pWarn->m_Count;

		ox = pWarn->m_x >> 8;
		oy = pWarn->m_y >> 8;

		dx = count;
		dy = count;
		Set2DPointC(tlv+0, ox-dx, oy-dy, 0.0, 0.0, c);
		Set2DPointC(tlv+1, ox+dx, oy-dy, 1.0, 0.0, c);
		Set2DPointC(tlv+2, ox+dx, oy+dy, 1.0, 1.0, c);
		Set2DPointC(tlv+3, ox-dx, oy+dy, 0.0, 1.0, c);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}


// 攻撃エフェクトを消去する //
FVOID CStdAttack::Clear(void)
{
	Iterator		it;

	// エフェクトの消去中 //
	ThisForEachFront(STAMA_SEND, it){
		// 現在のサイズとする //
		it->m_Count  = (it->m_Size * (STAMA_SENDDELAY - it->m_Count));
		it->m_Count /= STAMA_SENDDELAY;

		MoveBack(it, STAMA_CLEAR);		// 移動
	}

	// こちらは送るだけでＯＫ //
	MoveBackAll(STAMA_CLEAR, STAMA_WAIT);
}
