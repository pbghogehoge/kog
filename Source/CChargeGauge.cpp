/*
 *   CChargeGauge.cpp   : 溜めゲージ管理
 *
 */

#include "CChargeGauge.h"
#include "FontDraw.h"
#include "SECtrl.h"
#include "StateID.h"



// コンストラクタ //
CChargeGauge::CChargeGauge(RECT *rcTargetX256, int *pX, int *pY)
	: m_NormalShot(rcTargetX256, pX, pY)	// ノーマルショットのコンストラクタ
	, m_ExtraShot(rcTargetX256, pX, pY)		// エキストラショットのコンストラクタ
	, m_ChargeEfc(pX, pY)					// 溜めエフェクトのコンストラクタ
{
	m_pCharInfo        = NULL;		// キャラクタ固有情報の取得用
	m_pRivalSCLDecoder = NULL;		// 相手側のＳＣＬデコーダ
	m_pRivalAtkGrpDraw = NULL;		// 相手側のキャラ描画
	m_pEnemyCtrl       = NULL;		// 敵管理

	m_pThisState  = NULL;
	m_pRivalState = NULL;
}


// デストラクタ //
CChargeGauge::~CChargeGauge()
{
	// 現時点では、特に何も行いません //
}


// 死亡エフェクト完了なら真を返す //
FBOOL CChargeGauge::IsEffectFinished(void)
{
	if(m_DeadEfc.GetActiveData()) return FALSE;
	else                          return TRUE;
}


// 溜めゲージを初期化する //
FBOOL CChargeGauge::CGaugeInitialize(int InitAtkLv, DWORD CGaugeMax)
{
	m_bCGaugeEnable  = TRUE;				// 溜め発動準備ＯＫ
	m_CGaugeCurrent  = 0;					// 溜めゲージの現在の値
//	m_CGaugeMax      = CGAUGE_LEVEL1;		// 常にレベル１アタックは出せる
	m_CGaugeMax      = CGaugeMax;			// 常にレベル１アタックは出せる
	m_CGaugeAuto     = CGAUGE_ATATK_TIME;	// 溜めゲージの自動発動までの時間
	m_CGaugeBombLeft = 2;					// ボム数
	m_CGaugeAtkLv    = InitAtkLv;			// 攻撃レベル

	// エフェクト用カウンタをゼロ初期化する //
	m_Count = 0;

	return TRUE;
}


// ガードブレイク時の動作 //
FVOID CChargeGauge::CGaugeReset(void)
{
	m_bCGaugeEnable = TRUE;					// チャージ可にする
	m_CGaugeAuto    = CGAUGE_ATATK_TIME;	// 自動発動時間をリセット
	m_CGaugeCurrent = 0;					// 溜めを無効化する
}


// 溜めゲージを描画する //
FVOID CChargeGauge::CGaugeDraw(int ox, int oy, Pbg::CGrpSurface *pSurface)
{
	RECT			src;
	int				Current;
	int				MaxValue;
	int				x, y, i;
	D3DTLVERTEX		tlv[10];
	char			buf[20];

	DWORD			c = RGBA_MAKE(0, 0, 0, 0);

	Set2DPointC(tlv+0, ox    , 440, 0, 0, c);
	Set2DPointC(tlv+1, ox+268, 440, 0, 0, c);
	Set2DPointC(tlv+2, ox+268, 480, 0, 0, c);
	Set2DPointC(tlv+3, ox    , 480, 0, 0, c);

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_NORMAL);
//	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);

	wsprintf(buf, "%2d", m_CGaugeAtkLv);
	g_Font.Draw(ox+165, oy+14, buf, 16);

	g_pGrp->End3DScene();

	Current  = min(256, m_CGaugeCurrent / CGAUGE_WEIGHT);
	MaxValue = min(256, m_CGaugeMax / CGAUGE_WEIGHT);

	if(MaxValue >= 64){
		y = (MaxValue / 64 - 1) * 40;
		SetRect(&src, 64, y, 64+164, y+40);
		g_pGrp->BltC(&src, ox, oy, pSurface);

		x = MaxValue / 2;
		SetRect(&src, 232, 64, 232+x, 64+16);
		g_pGrp->BltC(&src, ox+32, oy+9, pSurface);
	}

	if(Current){
		x =  Current / 2;
		y = (max(64, Current) / 64 - 1) * 16;

		SetRect(&src, 232, y, 232+x, y+16);
		g_pGrp->BltC(&src, ox+32, oy+9, pSurface);
	}

	SetRect(&src, 232, 80,  232+32, 80+32);
	for(i=0; i<m_CGaugeBombLeft; i++){
		g_pGrp->BltC(&src, ox+200+i*32, oy+9, pSurface);
	}

	g_pGrp->Begin3DScene();
}


// カスりにより溜めの最大値を上昇させる //
FVOID CChargeGauge::CGaugeBuzz(DWORD nBuzz)
{
	DWORD		Delta;
	DWORD		Prev;

	Delta = nBuzz * m_pCharInfo->GetBuzzSpeed();	// 今回の増分を求める
	Prev  = m_CGaugeMax;							// 前回の値を求める

	// レベル４アタックよりは大きくならないようにする //
//	m_CGaugeMax = min(m_CGaugeMax+Delta, CGAUGE_LEVEL3);
	if(m_pRivalSCLDecoder){
		m_CGaugeMax = min(m_CGaugeMax+Delta, CGAUGE_LEVEL4);
	}
	else{
		m_CGaugeMax = min(m_CGaugeMax+Delta, CGAUGE_LEVEL1);
	}

	// ここでは、レベル２～４に初めて到達したときに、       //
	// チャージ完了エフェクトを発動させるかどうかを判定する //

/*	if(Prev < CGAUGE_LV1HALF){
		if(m_CGaugeMax >= CGAUGE_LEVEL2) PlaySE(SNDID_CHARGED);
	}
	else if(Prev < CGAUGE_LEVEL1){		// 前回がレベル１以下
		if(m_CGaugeMax >= CGAUGE_LEVEL1){
			PlaySE(SNDID_CHARGED);
		}
	}
	else */if(Prev < CGAUGE_LEVEL2){		// 前回がレベル２以下
		if(m_CGaugeMax >= CGAUGE_LEVEL2){
			m_ChargeEfc.Set(2);
//			PlaySE(SNDID_CHARGED);
		}
	}
	else if(Prev < CGAUGE_LEVEL3){	// 前回がレベル３以下
		if(m_CGaugeMax >= CGAUGE_LEVEL3){
			m_ChargeEfc.Set(3);
//			PlaySE(SNDID_CHARGED);
		}
	}
	else if(Prev < CGAUGE_LEVEL4){	// 前回がレベル４以下
		if(m_CGaugeMax >= CGAUGE_LEVEL4){
			m_ChargeEfc.Set(4);
//			PlaySE(SNDID_CHARGED);
		}
	}


	// 溜めゲージの自動発動までの時間をリセットする //
	m_CGaugeAuto = CGAUGE_ATATK_TIME;
}


// 溜めゲージの状態を更新する //
FVOID CChargeGauge::CGaugeUpdate(WORD KeyCode)
{
	// 現在溜めているか否かで状態推移を行う //
	if(m_CGaugeCurrent > 0) StateCharge(KeyCode);	// 溜め状態
	else                    StateNormal(KeyCode);	// 通常状態

	if(!(KeyCode & KEY_BOMB)){
		m_bBombKeyPushed = FALSE;
	}

	// カウンタを更新する //
	m_Count += 8;
}


// 攻撃レベルを取得する //
FDWORD CChargeGauge::CGaugeGetAtkLv(void)
{
	return m_CGaugeAtkLv;
}


// 通常の状態 //
FVOID CChargeGauge::StateNormal(WORD KeyCode)
{
	ExtraShotInfo		ExInfo;

	// ボムが残っていて、発動要求があった場合 //
	if((KeyCode & KEY_BOMB) && (m_CGaugeBombLeft > 0)
	&& (m_ShieldCount/2 <= 50) && (FALSE == m_bBombKeyPushed)){
		// エキストラアタック用の構造体をセットする //
		ExInfo.Charge     = m_CGaugeCurrent;	// 現在のチャージ
		ExInfo.pExtraShot = &m_ExtraShot;		// エキストラショットの格納先
		ExInfo.KeyCode    = KeyCode;			// キーボードの状態
		ExInfo.State      = m_GrpState;			// キャラクタの傾き
		m_pCharInfo->NormalBomb(&ExInfo, m_pThisBGDraw);
		ShieldOn(m_pCharInfo->GetBombTime() + 60);

		m_bBombKeyPushed = TRUE;	// ボムキーフラグを立てる
		m_CGaugeBombLeft--;			// 残りのボム数を１減らす
	}

	// 通常ショットを撃ちたいな //
	if(KeyCode & KEY_SHOT){
		// ノーマルショット発動 //
		if(m_pCharInfo->IsEnableNormalShot(&m_ExtraShot)){
			m_NormalShot.Set();

			// １段階の溜めに移行する //
			// 注意：溜め撃ち発動中でもチャージ出来るキャラが存在するなら、 //
			// 以下の文はこの if の外側に記述すべきである                   //
			m_CGaugeCurrent = m_pCharInfo->GetChargeSpeed();
		}
	}
}


// 溜め状態 //
FVOID CChargeGauge::StateCharge(WORD KeyCode)
{
	ExtraShotInfo		ExInfo;
	DWORD				Prev;

	// 参考：ボムキーとショットキーが両方押されていた場合、 //
	//       ボムアタックの発動を優先する                   //
/*
	// ボムアタック発動なるか？ //
	if( (KeyCode & KEY_BOMB) && (m_CGaugeBombLeft)){
		// ゲージがＬｖ１/２まで溜まっていない場合、通常ボムになる //
		// はっきり言って、溜めが無駄になるんだよね....            //
		if(m_CGaugeCurrent < CGAUGE_LV1HALF){
			// ボムが発動できた場合(ボム動作中には発動できないため) //
			if(TRUE == m_pCharInfo->NormalBomb(KeyCode)){
				m_CGaugeBombLeft--;		// ボム数を減らして
				m_CGaugeCurrent = 0;	// ゼロ初期化する

				ShieldOn(m_pCharInfo->GetBombTime());

				return;					// もう用は無い
			}
		}
		// ボムアタック発動！！ //
		else{
			// ボムが発動できた場合(ボム動作中には発動できないため) //
			if(TRUE == BombAttack(KeyCode)){
				m_CGaugeBombLeft--;					// ボム数減らして
				m_bCGaugeEnable = FALSE;			// チャージ不可にする
				m_CGaugeMax     = CGAUGE_LEVEL1;	// 最大値セット
				m_CGaugeAuto    = CGAUGE_ATATK_TIME;// 自動発動時間をリセット
				return;								// さようなら
			}
		}
	}
*/
	// ここに到達するのは、ボムアタックを発動しない場合か //
	// ボムアタックが発動出来なかった場合である           //

	// さらに溜める場合 //
	if(KeyCode & KEY_SHOT){
		// まだまだ溜められる //
		if(m_CGaugeCurrent < m_CGaugeMax){
			// キャラ別の溜め速度を取得し、加算する //
			Prev             = m_CGaugeCurrent;

			m_CGaugeCurrent += m_pCharInfo->GetChargeSpeed();
			if(m_CGaugeCurrent > m_CGaugeMax){		// 最大値まで到達
				m_CGaugeCurrent = m_CGaugeMax;		// 範囲内に修正
			}


/*			if(Prev < CGAUGE_LV1HALF){
				if(m_CGaugeCurrent >= CGAUGE_LV1HALF){
					PlaySE(SNDID_CHARGED);
				}
			}
			else if(Prev < CGAUGE_LEVEL1){		// 前回がレベル１以下
				if(m_CGaugeCurrent >= CGAUGE_LEVEL1){
					PlaySE(SNDID_CHARGED);
				}
			}
			else*/ if(Prev < CGAUGE_LEVEL2){		// 前回がレベル２以下
				if(m_CGaugeCurrent >= CGAUGE_LEVEL2){
					PlaySE(SNDID_CHARGED);
				}
			}
			else if(Prev < CGAUGE_LEVEL3){	// 前回がレベル３以下
				if(m_CGaugeCurrent >= CGAUGE_LEVEL3){
					PlaySE(SNDID_CHARGED);
				}
			}
			else if(Prev < CGAUGE_LEVEL4){	// 前回がレベル４以下
				if(m_CGaugeCurrent >= CGAUGE_LEVEL4){
					PlaySE(SNDID_CHARGED);
				}
			}

//			PlaySE(SNDID_CHARGED);

			return;
		}
		// 最大値まで溜まっている場合 //
		else{
			m_CGaugeAuto--;					// 自動発動までの残り時間を減らす
			if(m_CGaugeAuto > 0) return;	// まだ、発動せず
		}

		// 自動発動の時が訪れた場合、ここに到達する //
	}

	// ここまできたら、溜め撃ちが必ず発動する //
	m_bCGaugeEnable = FALSE;				// チャージ不可
	m_CGaugeAuto    = CGAUGE_ATATK_TIME;	// 自動発動までの時間をリセット


	// エキストラアタック用の構造体をセットする //
	ExInfo.Charge     = m_CGaugeCurrent;	// 現在のチャージ
	ExInfo.pExtraShot = &m_ExtraShot;		// エキストラショットの格納先
	ExInfo.KeyCode    = KeyCode;			// キーボードの状態
	ExInfo.State      = m_GrpState;			// キャラクタの傾き

	// 通常発動 [ Ｌｅｖｅｌ１Ａｔｔａｃｋ ] //
	if(m_CGaugeCurrent < CGAUGE_LEVEL2){
		m_pCharInfo->Level1Attack(&ExInfo);
	}
	// 通常発動 [ Ｌｅｖｅｌ２Ａｔｔａｃｋ ] //
	else if(m_CGaugeCurrent < CGAUGE_LEVEL3){
		m_pCharInfo->Level1Attack(&ExInfo);
		m_pCharInfo->Level2Attack(m_pRivalSCLDecoder, (BYTE)m_CGaugeAtkLv+10);
		m_pRivalAtkGrpDraw->Set(AGD_LV1);

		m_CGaugeMax -= CGAUGE_LEVEL1;
		m_CGaugeAtkLv = min(25, m_CGaugeAtkLv+1);
		m_pRivalBGDraw->SetBlendColor(180, 180, 180, 80+40);
		m_pRivalState->InsertString(STWNDID_LV2ATK, 0);
	}
	// 通常発動 [ Ｌｅｖｅｌ３Ａｔｔａｃｋ ] //
	else if(m_CGaugeCurrent < CGAUGE_LEVEL4){
		m_pCharInfo->Level1Attack(&ExInfo);
		m_pCharInfo->Level3Attack(m_pRivalSCLDecoder, (BYTE)m_CGaugeAtkLv+10);
		m_pRivalAtkGrpDraw->Set(AGD_LV2);

		m_CGaugeMax -= CGAUGE_LEVEL2;
		m_CGaugeAtkLv = min(25, m_CGaugeAtkLv+2);
		m_pRivalBGDraw->SetBlendColor(180, 180, 180, 80+40);
		m_pRivalState->InsertString(STWNDID_LV3ATK, 0);
	}
	// 通常発動 [ ＢｏｓｓＡｔｔａｃｋ ] //
	else{
		m_pCharInfo->Level1Attack(&ExInfo);
		m_pCharInfo->Level4Attack(m_pRivalSCLDecoder, (BYTE)m_CGaugeAtkLv+10);
		m_pRivalAtkGrpDraw->Set(AGD_BOSS);

		m_CGaugeMax -= CGAUGE_LEVEL3;
		m_CGaugeAtkLv = min(25, m_CGaugeAtkLv+3);

		m_pRivalState->InsertString(STWNDID_LV4ATK, 0);
		//m_pRivalBGDraw->SetBossAlive(TRUE);
		//>SetBlendColor(180, 180, 180, 80+40);
	}

	// いいのか？ //
	m_CGaugeCurrent = 0;
}


// ボムアタック(発動したらTRUE) //
FBOOL CChargeGauge::BombAttack(WORD KeyCode)
{
	// レベルによる分岐を行って、後はお任せする　//

	ExtraShotInfo		ExInfo;

	ExInfo.Charge     = m_CGaugeCurrent;
	ExInfo.pExtraShot = &m_ExtraShot;
	ExInfo.KeyCode    = KeyCode;
	ExInfo.State      = m_GrpState;			// キャラクタの傾き


	// ボムアタック [ Ｌｅｖｅｌ１Ａｔｔａｃｋ ] //
	if(m_CGaugeCurrent < CGAUGE_LEVEL2){
		return m_pCharInfo->Level1BombAtk(&ExInfo);
	}
	// ボムアタック [ Ｌｅｖｅｌ２Ａｔｔａｃｋ ] //
	else if(m_CGaugeCurrent < CGAUGE_LEVEL3){
		m_pCharInfo->Level1Attack(&ExInfo);
		return m_pCharInfo->Level2BombAtk(m_pRivalSCLDecoder, (BYTE)m_CGaugeAtkLv);
	}
	// ボムアタック [ Ｌｅｖｅｌ３Ａｔｔａｃｋ ] //
	else if(m_CGaugeCurrent < CGAUGE_LEVEL4){
		m_pCharInfo->Level1Attack(&ExInfo);
		return m_pCharInfo->Level3BombAtk(m_pRivalSCLDecoder, (BYTE)m_CGaugeAtkLv);
	}
	// ボムアタック [ ＢｏｓｓＡｔｔａｃｋ ] //
	else{
		m_pCharInfo->Level1Attack(&ExInfo);
		return m_pCharInfo->Level4BombAtk(m_pRivalSCLDecoder, (BYTE)m_CGaugeAtkLv);
	}
}


// シールドをＯＮにする //
FVOID CChargeGauge::ShieldOn(int Count)
{
	m_ShieldCount  = Count;	// ガードブレイク無敵時間
	m_ShieldStart  = Count;	// シールド開始カウント
	m_bNeedETClear = TRUE;	// 敵弾消去要求

	// コンボを強制リセット //
	m_pShaveEffect->ForceReset();
}
