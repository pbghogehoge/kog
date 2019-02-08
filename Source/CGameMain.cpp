/*
 *   CGameMain.cpp   : ゲーム本体
 *
 */

#include "CGameMain.h"
#include "SECtrl.h"
#include "FontDraw.h"

#include "RndCtrl.h"



#ifdef PBG_DEBUG // デバッグ情報
	BOOL CGameMain::m_Debug_DisplayInfo = FALSE;	// 表示するか否か
	BYTE CGameMain::m_Debug_EffectCount = 0;		// 表示フェードエフェクト用カウンタ

typedef enum {
	DMG_NORMAL,		// ダメージ通常
	DMG_NONE,		// ダメージ無し
	DMG_LESS		// ダメージ少量
} DEBUG_DAMAGE_MODE;

static DEBUG_DAMAGE_MODE	g_DbgDmgMode = DMG_NORMAL;
static BOOL					g_bSlowMode  = FALSE;
#endif



// コンストラクタ(矩形は非x２５６) //
CGameMain::CGameMain(RECT *rcTarget, SCENE_ID SceneID)
{
	int		*pX, *pY;
	DWORD	TexID;

	// シーンＩＤを確定する //
	m_SceneID = SceneID;

	// 描画矩形を決定する //
	m_rcViewport = *rcTarget;

	// 与えられた矩形を x256(固定小数点数) に変換する //
	m_rcTargetX256.left   = (rcTarget->left)   * 256;
	m_rcTargetX256.top    = (rcTarget->top)    * 256;
	m_rcTargetX256.right  = (rcTarget->right)  * 256;
	m_rcTargetX256.bottom = (rcTarget->bottom) * 256;

	// カウンタを初期化する //
	m_Count      = 0;
	m_CloseCount = 0;

	// ポインタを無効化する(下の奴とは区別する事！) //
	m_pRivalInfo   = NULL;	// 相手の情報

	// 失敗したときに備えて、ポインタを無効化しておく //
	m_pBGDraw      = NULL;	// 背景描画
	m_pShaveEfc    = NULL;	// カスりエフェクト
	m_pFragmentEfc = NULL;	// 破片エフェクト
	m_pTriangleEfc = NULL;	// 三角形エフェクト
	m_pEnemyCtrl   = NULL;	// 敵コントロール
	m_pSCLDecode   = NULL;	// ＳＣＬデコード
	m_pPlayerCtrl  = NULL;	// プレイヤー管理
	m_pDrawStgEfc  = NULL;	// ステージ表示エフェクト
	m_pStdAttack   = NULL;	// 攻撃送り
	m_pDrawBonus   = NULL;	// ボーナス表示エフェクト
	m_pDrawWonLost = NULL;	// 勝敗表示エフェクト
	m_pScore       = NULL;	// 得点管理
	m_pAtkGrpDraw  = NULL;	// キャラクタ描画クラス
	m_pBGDoor      = NULL;	// 扉の描画
	m_pStateWindow = NULL;	// 状態表示窓


	// シーンＩＤからプレイヤーのテクスチャＩＤを求める //
	switch(SceneID){
		case SCENE_PLAYER_1:	TexID = TEXTURE_ID_P1_TAIL;		break;
		case SCENE_PLAYER_2:	TexID = TEXTURE_ID_P2_TAIL;		break;

		default:
			// いや、駄目でしょ //
		return;
	}

	// 失敗した場合、braek で逃げる     //
	// クラスを作成する順番に注意すべし //
	while(1){
		// 背景管理クラスを生成する //
		m_pBGDraw = NewEx(CBGDraw(&m_rcTargetX256));
		if(NULL == m_pBGDraw) break;

		m_pBGDoor = NewEx(CBGDoor(&m_rcTargetX256));
		if(NULL == m_pBGDoor) break;

		// 三角形エフェクトを生成する //
		m_pTriangleEfc = NewEx(CTriEffect);
		if(NULL == m_pTriangleEfc) break;

		// カスりエフェクト管理クラスを生成する //
		m_pShaveEfc = NewEx(CShaveEffect(&m_rcTargetX256));
		if(NULL == m_pShaveEfc) break;

		// 破片エフェクト管理クラスを生成する //
		m_pFragmentEfc = NewEx(CFragmentEfc(&m_rcTargetX256));
		if(NULL == m_pFragmentEfc) break;

		// ステージ表示エフェクト管理クラスを生成する //
		m_pDrawStgEfc = NewEx(CDrawStg(&m_rcTargetX256));
		if(NULL == m_pDrawStgEfc) break;

		// ボーナス表示エフェクト管理クラスを生成 //
		m_pDrawBonus = NewEx(CDrawBonus(&m_rcTargetX256));
		if(NULL == m_pDrawBonus) break;

		// 勝敗表示エフェクト管理クラスを生成 //
		m_pDrawWonLost = NewEx(CDrawWonLost(&m_rcTargetX256));
		if(NULL == m_pDrawWonLost) break;

		// 攻撃送り管理クラスを作成 //
		m_pStdAttack = NewEx(CStdAttack());
		if(NULL == m_pStdAttack) break;

		// 得点管理クラスを生成 //
		m_pScore = NewEx(CScoreCtrl(&m_rcTargetX256, m_pShaveEfc));
		if(NULL == m_pScore) break;

		// キャラクタ描画クラスを作成 //
		m_pAtkGrpDraw = NewEx(CAtkGrpDraw(&m_rcTargetX256, SceneID));
		if(NULL == m_pAtkGrpDraw) break;

		// プレイヤー情報を生成する //
		m_pPlayerCtrl = NewEx(CPlayerCtrl(&m_rcTargetX256
										, TexID
										, m_pTriangleEfc
										, m_pShaveEfc));
		if(NULL == m_pPlayerCtrl) break;

		pX = m_pPlayerCtrl->GetX_Ptr();		// Ｘ座標取得用
		pY = m_pPlayerCtrl->GetY_Ptr();		// Ｙ座標取得用

		// 敵管理クラスを生成する //
		m_pEnemyCtrl = NewEx(CEnemyCtrl(&m_rcTargetX256		// 移動領域
									  , pX					// 当たり判定ｘ
									  , pY					// 当たり判定ｙ
									  , m_pFragmentEfc		// 破片管理
									  , m_pShaveEfc			// カスリエフェクト管理
									  , m_pTriangleEfc		// ガードブレイク用エフェクト
									  , m_pBGDraw));		// 背景管理
		if(NULL == m_pEnemyCtrl) break;

		m_pStateWindow = NewEx(CStateWindow(&m_rcTargetX256
										,	pX
										,	pY));	// 状態表示窓
		if(NULL == m_pStateWindow) break;

		// ＳＣＬデコードクラスを生成する //
		m_pSCLDecode = NewEx(CSCLDecoder(&m_rcTargetX256, m_pEnemyCtrl));
		if(NULL == m_pSCLDecode) break;

		return;		// 正常終了
	}

	Cleanup();
}


// デストラクタ //
CGameMain::~CGameMain()
{
	Cleanup();
}


// 初期化を行う //
FBOOL CGameMain
	::Initialize(CHARACTER_ID	CharID		// 自分のキャラクタＩＤ
			   , CHARACTER_ID	RivalID		// 相手のキャラクタＩＤ
			   , CGameMain		*pRival		// 相手側のクラス
			   , BOOL			Is2PMode	// ２Ｐモードなら真
			   , int			nStage		// 現在のステージorラウンド数
			   , int			GameLv		// ＳＣＬ初期化レベル
			   , int			AtkLv		// 初期攻撃レベル
			   , __int64		Score		// 初期化スコア
			   , DWORD			CGaugeMax	// 溜めゲージ
			   , BYTE			DamageWeight	// ダメージの減少
			   , BOOL			Is2PColor
			   , DWORD			RndSeed)		// 乱数の種
{
	// 乱数系列の初期化 //
	m_Rnd.SetSeed(RndSeed);

	// クラスが生成できていない(メモリが足りない)場合は失敗 //
	if(NULL == m_pSCLDecode) return FALSE;

	// ＳＣＬデコーダを初期化 //
	if(FALSE == m_pSCLDecode->Initialize(GameLv)) return FALSE;

	// プレイヤー情報を初期化する //
	if(FALSE == m_pPlayerCtrl->PlayerInitialize(
								CharID						// キャラクタＩＤ
							  , AtkLv						// 初期攻撃レベル
							  , pRival->m_pStdAttack		// 標準攻撃送り
							  , pRival->m_pSCLDecode		// ＳＣＬデコーダ
							  , pRival->m_pBGDraw			// 背景描画
							  , pRival->m_pAtkGrpDraw		// キャラ描画
							  , m_pBGDraw					// 背景描画
							  , m_pEnemyCtrl				// 敵管理
							  , pRival->m_pEnemyCtrl		// 相手側の敵管理
							  , CGaugeMax
							  , m_pStateWindow
							  , pRival->m_pStateWindow)){
		return FALSE;
	}

	// スコアを初期化する //
	m_pScore->Initialize(Score);

	// キャラクタ描画クラスの初期化 //
	m_pAtkGrpDraw->Initialize(CharID, pRival->m_pAtkGrpDraw, Is2PColor);


	m_pEnemyCtrl->Initialize();		// 敵コントロールの初期化
	m_pBGDraw->Initialize();		// 背景描画の初期化
	m_pShaveEfc->Initialize();		// カスりエフェクトの初期化
	m_pFragmentEfc->Initialize();	// 破片エフェクトの初期化
	m_pTriangleEfc->Initialize();	// 三角形エフェクトの初期化
	m_pDrawBonus->Initialize();		// ボーナス得点表示の初期化
	m_pDrawWonLost->Initialize();	// 勝敗エフェクトの初期化

	// 状態表示窓の初期化 //
	m_pStateWindow->Initialize();

	// 扉を開く準備をする //
	m_pBGDoor->Set(TRUE);

	// 攻撃送りを初期化する //
	m_pStdAttack->Initialize(m_pEnemyCtrl->GetEnemyTamaPtr(), &m_rcTargetX256);

#ifdef PBG_DEBUG
	if(FALSE == m_Debug_DisplayInfo)
#endif
	// ステージ表示エフェクトの初期化を行う //
	m_pDrawStgEfc->Initialize(nStage, Is2PMode);

	// 引数を処理する //
	m_pRivalInfo = pRival;		// 相手の情報を格納
	m_CharID     = CharID;		// プレイヤーＩＤを取得

	// カウンタを初期化する //
	m_Count      = 0;
	m_CloseCount = 0;

	// ダメージ重みをセット //
	m_DamageWeight = DamageWeight;

	// ＣＰＵの初期化をする //
	CPUInitialize(CPU_NORMAL);

	// 現在の状態 //
	m_State = GMAIN_OK;

	return TRUE;
}


// １フレーム進める //
GMAIN_RET CGameMain::Move(WORD KeyCode)
{
	DWORD		Shave;
	DWORD		Damage;
	DWORD		Score;

	// 初期化されていません //
	if(NULL == m_pSCLDecode) return GMAIN_ERROR;

	// 乱数発生クラスを切り替える //
	SetRndClass(&m_Rnd);

	// 効果音に備える //
	SetSEOrg((m_rcTargetX256.right + m_rcTargetX256.left) / 2);

	// １フレームだけ進行させる //
	m_pDrawStgEfc->Move();		// ステージ表示エフェクト
	m_pDrawBonus->Move();		// ボーナス表示エフェクト
	m_pDrawWonLost->Move();		// 勝敗表示エフェクト

	// 最初のエフェクトが終了するまで、デコードしない //
	if(FALSE == m_pDrawStgEfc->IsActive()
	&& GMAIN_OK == m_State){
		m_pSCLDecode->Proceed();	// ＳＣＬデコーダ
	}

	m_pEnemyCtrl->Move();		// 敵コントロール(攻撃Objを含む)
	m_pFragmentEfc->Move();		// 破片エフェクト
	m_pTriangleEfc->Move();		// 三角形エフェクト

	m_pAtkGrpDraw->Move();		// キャラクタ描画
	m_pBGDraw->Move();			// 背景コントロール
	m_pBGDoor->Move();			// 扉描画

	// ＣＰＵモードで１フレーム進める場合 //
	if(KEY_CPUMOVE == KeyCode){
		KeyCode = CPUGetKeyCode();
	}

	// プレイヤー関連を１フレーム進行させる //
	m_pPlayerCtrl->PlayerMove(KeyCode);	// プレイヤーの移動を行う
	m_pShaveEfc->Move();				// カスりエフェクト
	m_pStdAttack->Move();				// 弾幕送り

	// 敵 <-> ノーマルショット の当たり判定   //
	Score = m_pEnemyCtrl->EnumActiveEnemy(HitCheckCallback, this);
	Score = (Score * (10 + m_pShaveEfc->GetShaveCombo())) / 10;
	m_pScore->AddScore(Score);


	// 自機に当たり判定が存在する場合、判定を行う //
	if(m_pPlayerCtrl->IsHitOn()){
		// 自機 <-> 敵の当たり判定 //
		m_pEnemyCtrl->HitCheck(&Shave, &Damage);

		Damage = Damage * m_DamageWeight / 255;

		// ダメージの反映 //
#ifdef PBG_DEBUG
		if(g_bSlowMode && Damage) Sleep(50);	// わざと処理落ちさせる
		switch(g_DbgDmgMode){
			case DMG_NORMAL:	// ダメージ通常
			break;

			case DMG_NONE:		// ダメージ無し
				Damage = 0;
			break;

			case DMG_LESS:		// ダメージ少量
				Damage /= 3;
			break;
		}
#endif

		// 取得した値を実際に反映する //
		if(Damage)	m_pPlayerCtrl->OnDamage(Damage);	// ダメージ
		if(Shave)	m_pPlayerCtrl->OnShave(Shave);		// カスり
	}
	else{
		m_pStdAttack->Clear();
	}


#ifdef PBG_DEBUG
	// デバッグ用殺しコマンド //
	if(GetAsyncKeyState(VK_F3) & 0x8000){
		m_pPlayerCtrl->OnDamage(99999999);
	}

	static BOOL bDbgDmg = FALSE;
	if(GetAsyncKeyState(VK_F2) & 0x8000){
		if(FALSE == bDbgDmg){
			bDbgDmg = TRUE;
			switch(g_DbgDmgMode){
			case DMG_NORMAL:	// ダメージ通常
				g_DbgDmgMode = DMG_NONE;
			break;

			case DMG_NONE:		// ダメージ無し
				g_DbgDmgMode = DMG_LESS;
			break;

			case DMG_LESS:		// ダメージ少量
				g_DbgDmgMode = DMG_NORMAL;
			break;
			}
		}
	}
	else{
		bDbgDmg = FALSE;
	}

	static BOOL bF4 = FALSE;
	if(GetAsyncKeyState(VK_F4) & 0x8000){
		if(FALSE == bF4){
			g_bSlowMode = g_bSlowMode ? FALSE : TRUE;
			bF4 = TRUE;
		}
	}
	else{
		bF4 = FALSE;
	}

	static BOOL bF6 = FALSE;
	static BOOL bF7 = FALSE;
	static BOOL bF8 = FALSE;
	static BOOL bF9 = FALSE;

	int thisAtkLv  = 10 + m_pPlayerCtrl->GetCurrentAtkLv();
	int rivalAtkLv = 10 + m_pRivalInfo->m_pPlayerCtrl->GetCurrentAtkLv();

	if(GetAsyncKeyState(VK_F6) & 0x8000){
		if(FALSE == bF6){
			m_pRivalInfo->m_pSCLDecode->SetExAttackLv1(m_CharID, thisAtkLv);
			m_pRivalInfo->m_pAtkGrpDraw->Set(AGD_LV1);
			m_pRivalInfo->m_pBGDraw->SetBlendColor(180, 180, 180, 80);

			m_pSCLDecode->SetExAttackLv1(m_pRivalInfo->m_CharID, rivalAtkLv);
			m_pAtkGrpDraw->Set(AGD_LV1);
			m_pBGDraw->SetBlendColor(180, 180, 180, 80);

			bF6 = TRUE;
		}
	}
	else{
		bF6 = FALSE;
	}
	if(GetAsyncKeyState(VK_F7) & 0x8000){
		if(FALSE == bF7){
			m_pRivalInfo->m_pSCLDecode->SetExAttackLv2(m_CharID, thisAtkLv);
			m_pRivalInfo->m_pAtkGrpDraw->Set(AGD_LV2);
			m_pRivalInfo->m_pBGDraw->SetBlendColor(180, 180, 180, 80);

			m_pSCLDecode->SetExAttackLv2(m_pRivalInfo->m_CharID, rivalAtkLv);
			m_pAtkGrpDraw->Set(AGD_LV2);
			m_pBGDraw->SetBlendColor(180, 180, 180, 80);

			bF7 = TRUE;
		}
	}
	else{
		bF7 = FALSE;
	}
	if(GetAsyncKeyState(VK_F8) & 0x8000){
		if(FALSE == bF8){
			m_pRivalInfo->m_pSCLDecode->SetBossAttack(m_CharID, thisAtkLv);
			m_pRivalInfo->m_pAtkGrpDraw->Set(AGD_BOSS);

			m_pSCLDecode->SetBossAttack(m_pRivalInfo->m_CharID, rivalAtkLv);
			m_pAtkGrpDraw->Set(AGD_BOSS);

			bF8 = TRUE;
		}
	}
	else{
		bF8 = FALSE;
	}
	if(GetAsyncKeyState(VK_F9) & 0x8000){
		if(FALSE == bF9){
			m_pAtkGrpDraw->Set(AGD_WON);
			m_pRivalInfo->m_pAtkGrpDraw->Set(AGD_WON);
			bF9 = TRUE;
		}
	}
	else{
		bF9 = FALSE;
	}

//VK_PRIOR 21 PAGE UP key
//VK_NEXT
	static BOOL bUpDown = FALSE;
	if(GetAsyncKeyState(VK_PRIOR) & 0x8000){
		if(FALSE == bUpDown){
			m_pPlayerCtrl->DebugAddAtkLv(1);
			m_pRivalInfo->m_pPlayerCtrl->DebugAddAtkLv(1);
			bUpDown = TRUE;
		}
	}
	else if(GetAsyncKeyState(VK_NEXT) & 0x8000){
		if(FALSE == bUpDown){
			m_pPlayerCtrl->DebugAddAtkLv(-1);
			m_pRivalInfo->m_pPlayerCtrl->DebugAddAtkLv(-1);
			bUpDown = TRUE;
		}
	}
	else{
		bUpDown = FALSE;
	}
#endif


	// 敵弾消去要求が存在する場合 //
	if(m_pPlayerCtrl->IsETClear()){
		// ガードブレイク中でなければ得点にする //
		OnETClear(!m_pPlayerCtrl->IsGBreak());
	}

	m_pBGDraw->SetSpeed(256 * 2);

	DWORD t = m_Count % 1600;
	switch(t){
		case 1200:
			m_pBGDraw->SetZoom(256 * 256, 256);
		break;

		case 400:
			m_pBGDraw->SetZoom(64 * 256, 256);
		break;

		default:
			if(t > 800  && t < 880)  m_pBGDraw->SetAngleR(1);
			if(t > 1520 && t < 1600) m_pBGDraw->SetAngleR(-1);
		break;
	}


	m_pScore->Update();


	// カウンタを進行させる //
	m_Count++;
/*
	switch(m_State){
	// 勝ちの場合、ボーナス表示の完了を待つ //
	case GMAIN_WON:
//		if(m_pDrawBonus->IsFinished()) return TRUE;
	break;

	// それ以外の場合、勝ち負け表示の完了を待つ //
	case GMAIN_DEAD:
//		if(m_pDrawWonLost->IsFinished()
//		&& m_pPlayerCtrl->IsEffectFinished()) return TRUE;
	break;
	}
*/

	m_pStateWindow->Move();

	// 背景ドアが閉じられていない場合 //
	if((m_pBGDoor->IsStopped()) && (GMAIN_OK != m_State)){
		// こちら側のプレイヤーが勝者の場合 //
		if(GMAIN_WON == m_State){
			if(m_pDrawBonus->IsFinished()){
				m_CloseCount++;
			}
		}
		// 相手側のプレイヤーが勝者の場合 //
		else if(GMAIN_WON == m_pRivalInfo->m_State){
			if(m_pRivalInfo->m_pDrawBonus->IsFinished()){
				m_CloseCount++;
			}
		}
		// 引き分けの場合 //
		else{
			if(m_pDrawWonLost->IsFinished()
				&& m_pPlayerCtrl->IsEffectFinished()){
				m_CloseCount++;
			}
		}
	}

	if(30 == m_CloseCount){
		m_pBGDoor->Set(FALSE);
		m_CloseCount++;
	}

	// 以下で乱数の使用を禁止する //
	SetRndClass(NULL);

	if(GMAIN_OK == m_State
	&& m_pPlayerCtrl->IsPlayerDead()){
		return GMAIN_DEAD;
	}

	return GMAIN_OK;
}


// 描画を行う //
FBOOL CGameMain::Draw(int SyncDx)
{
	// 描画対象となる矩形をセットする //
	if(FALSE == g_pGrp->SetViewport(&m_rcViewport)) return FALSE;

	// それでは、一気に描画しましょう //
	m_pBGDraw->Draw();			// 背景を描画する
	m_pAtkGrpDraw->Draw();		// キャラクタ描画

	m_pShaveEfc->DrawFragment();	// カスりエフェクトの描画

	m_pEnemyCtrl->Draw();			// 敵を描画する

	m_pFragmentEfc->Draw();			// 破片を描画する

	// プレイヤーの描画を行う(ショット＆Ｅｘショットの描画も行う) //
	m_pPlayerCtrl->DrawPlayer();

	// 三角形エフェクト //
	m_pTriangleEfc->Draw();

	m_pEnemyCtrl->DrawAtkObj();	// 敵弾などの描画を行う

	m_pShaveEfc->DrawCombo();	// コンボ数の描画
	m_pDrawStgEfc->Draw();		// ステージ表示エフェクト
	m_pDrawWonLost->Draw();		// 勝敗表示エフェクト
	m_pDrawBonus->Draw();		// ボーナス表示エフェクト

	// マッドネスゲージの描画 //
	m_pEnemyCtrl->DrawMadnessGauge();

	m_pScore->Draw();					// 得点表示
	m_pPlayerCtrl->DrawGuardGauge();	// ガードゲージの描画
//	m_pPlayerCtrl->DrawExtra(pSurface);	// ゲージ系の描画

	m_pStateWindow->Draw();		// 状態表示窓

	m_pBGDoor->Draw(SyncDx);			// 扉描画

	// 正常終了 //
	return TRUE;
}


// 全ての敵弾＆レーザーに Clear() 要求を出力する //
FVOID CGameMain::OnETClear(BOOL bChgScore)
{
	// 乱数発生クラスを切り替える //
	SetRndClass(&m_Rnd);

	// 消去は敵管理クラスに委譲する //
	m_pScore->AddScore(
		m_pEnemyCtrl->ClearAtkObj(bChgScore)
	);

//	m_pEnemyTama->Clear();		// 敵弾コントロール
//	m_pEnemySLaser->Clear();	// ショートレーザー
//	m_pEnemyLLaser->Clear();	// 太いレーザー

	// 乱数発生クラスを切り替える //
	SetRndClass(NULL);
}


// 勝敗が決定したときに呼び出される //
FVOID CGameMain::OnWonLost(BYTE DWL_State)
{
	DWORD		Score;

	// 乱数発生クラスを切り替える //
	SetRndClass(&m_Rnd);

	m_pDrawWonLost->Set(DWL_State);

	m_pEnemyCtrl->Clear();
	m_pStdAttack->Clear();
	OnETClear(FALSE);

	switch(DWL_State){
	case DWL_WON:
		Score = m_pDrawBonus->Set(m_pPlayerCtrl->GetMaxCombo(), m_Count);
		m_pScore->AddScore(Score);

		m_pPlayerCtrl->OnPlayerWon();
		m_pAtkGrpDraw->Set(AGD_WON);

		m_State = GMAIN_WON;
	break;

	default: case DWL_LOST: case DWL_DRAW:
		m_State = GMAIN_DEAD;
	break;
	}

	// 乱数発生クラスを切り替える //
	SetRndClass(NULL);
}


// 現在のスコアを取得する //
__int64 CGameMain::GetCurrentScore(void)
{
	if(NULL == m_pScore) return 0;

	return m_pScore->GetScore();
}


// 現在の攻撃レベルを取得する //
int CGameMain::GetCurrentAtkLv(void)
{
	if(NULL == m_pPlayerCtrl) return 1;

	return m_pPlayerCtrl->GetCurrentAtkLv();
}


// その他情報の描画を行う //
FBOOL CGameMain::DrawInfo(Pbg::CGrpSurface *pSurface)
{
	m_pStdAttack->Draw();				// 攻撃送り予告エフェクト
	m_pPlayerCtrl->DrawExtra(pSurface);	// ゲージ系の描画


#ifdef PBG_DEBUG
	// デバッグ用情報の出力 //
	char			buf[128];
	BYTE			alpha;
	D3DTLVERTEX		tlv[20];
	DWORD			c;
	int				x, y, w, h;

	alpha = m_Debug_EffectCount;

	if( (m_Debug_DisplayInfo) && alpha != 255){
		m_Debug_EffectCount = min(255, m_Debug_EffectCount+8);
	}
	else if((!m_Debug_DisplayInfo) && alpha != 0){
		m_Debug_EffectCount = max(0, m_Debug_EffectCount-8);
	}

	if(alpha){
		x = *(m_pPlayerCtrl->GetX_Ptr()) / 256;
		y = *(m_pPlayerCtrl->GetY_Ptr()) / 256;
		w = CPUGetCheckRange() / 256;
		c = RGBA_MAKE(0, 0, 255, 128);

		Set2DPointC(tlv+0, x-w, y-w, 0, 0, c);
		Set2DPointC(tlv+4, x-w, y-w, 0, 0, c);
		Set2DPointC(tlv+1, x+w, y-w, 0, 0, c);
		Set2DPointC(tlv+2, x+w, y+w, 0, 0, c);
		Set2DPointC(tlv+3, x-w, y+w, 0, 0, c);
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
		g_pGrp->SetTexture(GRPTEXTURE_MAX);
		g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 5);


		c = RGBA_MAKE(0, 0, 0, alpha/2);
		x = m_rcViewport.left;
		y = m_rcViewport.bottom - 56+10;
		w = m_rcViewport.right;
		h = m_rcViewport.bottom;

		Set2DPointC(tlv+0, x, y, 0, 0, c);
		Set2DPointC(tlv+1, w, y, 0, 0, c);
		Set2DPointC(tlv+2, w, h, 0, 0, c);
		Set2DPointC(tlv+3, x, h, 0, 0, c);
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
		g_pGrp->SetTexture(GRPTEXTURE_MAX);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


		strcpy(buf, "DebugInfo [Push TAB to Delete]");
		g_Font.DrawAlpha(m_rcViewport.left+4, m_rcViewport.bottom-56+10
											, buf, alpha, 9);

		wsprintf(buf, " Enemy:%5d   Count:%5d"
					, m_pEnemyCtrl->GetActiveData()
					, m_pSCLDecode->GetCurrentCount());
		g_Font.DrawAlpha(m_rcViewport.left, m_rcViewport.bottom-56+12+10
											, buf, alpha, 10);

		wsprintf(buf, " Tama :%5d   EExSh:%5d"
					, m_pEnemyCtrl->GetEnemyTamaPtr()->GetActiveData()
					, m_pEnemyCtrl->GetEnemyExShot()->GetActiveData());
		g_Font.DrawAlpha(m_rcViewport.left, m_rcViewport.bottom-56+24+10
											, buf, alpha, 10);
/*		wsprintf(buf, " Tama :%5d   Laser:%5d"
					, m_pEnemyCtrl->GetEnemyTamaPtr()->GetActiveData()
					, m_pEnemyCtrl->GetEnemySLaserPtr()->GetActiveData());
		g_Font.DrawAlpha(m_rcViewport.left, m_rcViewport.bottom-56+24+10
											, buf, alpha, 10);
*/
/*
		wsprintf(buf, " TrEfc:%5d   PtEfc:%5d"
					, m_pTriangleEfc->GetActiveData()
					, m_pFragmentEfc->GetActiveData());
*/
		wsprintf(buf, " HLaser:%4d   ExShot:%4d"
					, m_pEnemyCtrl->GetEnemyHLaserPtr()->GetActiveData()
					, m_pPlayerCtrl->GetNumExShot());
		g_Font.DrawAlpha(m_rcViewport.left, m_rcViewport.bottom-56+36+10
											, buf, alpha, 10);
	}

	switch(g_DbgDmgMode){
		default:
		case DMG_NORMAL:	// ダメージ通常
			strcpy(buf, "DamageMode = NORMAL [F2]");
		break;

		case DMG_NONE:		// ダメージ無し
			strcpy(buf, "DamageMode = NONE   [F2]");
		break;

		case DMG_LESS:		// ダメージ少量
			strcpy(buf, "DamageMode = LESS   [F2]");
		break;
	}

	g_Font.DrawAlpha(m_rcViewport.left+20, m_rcViewport.top+70, buf, alpha, 10);
	g_Font.DrawAlpha(m_rcViewport.left+20, m_rcViewport.top+80, "Reset[F1]  Kill[F3]", alpha, 10);
	g_Font.DrawAlpha(m_rcViewport.left+20, m_rcViewport.top+90, "PauseECL[F11]", alpha, 10);
	g_Font.DrawAlpha(m_rcViewport.left+20, m_rcViewport.top+100, "ExAnime [F6-F9]", alpha, 10);

	wsprintf(buf, "SlowMode[F4] %s", g_bSlowMode ? "On" : "Off");
	g_Font.DrawAlpha(m_rcViewport.left+20, m_rcViewport.top+110, buf, alpha, 10);
#endif
	return TRUE;
}


// ゲーム終了時に各種エフェクトが完了していれば真を返す //
FBOOL CGameMain::IsGameFinished(void)
{
	switch(m_State){
		case GMAIN_WON:		case GMAIN_DEAD:
		break;

		default:
		return FALSE;
	}


	if(m_pBGDoor->IsClosed()) return TRUE;

/*
	switch(m_State){
	// 勝ちの場合、ボーナス表示の完了を待つ //
	case GMAIN_WON:
		if(m_pDrawBonus->IsFinished()) return TRUE;
	break;

	// それ以外の場合、勝ち負け表示の完了を待つ //
	case GMAIN_DEAD:
		if(m_pDrawWonLost->IsFinished()
		&& m_pPlayerCtrl->IsEffectFinished()) return TRUE;
	break;
	}
*/
	return FALSE;
}


// 動的確保したオブジェクトの解放を行う //
FVOID CGameMain::Cleanup(void)
{
	// 注意 : m_pRivelInfo は相手側のポインタなので、解放しない事！！ //

	DeleteEx(m_pBGDoor);		// 扉の描画
	DeleteEx(m_pAtkGrpDraw);	// キャラクタ描画
	DeleteEx(m_pScore);			// スコア描画
	DeleteEx(m_pDrawWonLost);	// 勝敗表示エフェクト
	DeleteEx(m_pDrawBonus);		// ボーナス表示エフェクト
	DeleteEx(m_pDrawStgEfc);	// ステージ表示エフェクト

	DeleteEx(m_pStdAttack);		// 攻撃送り

	DeleteEx(m_pBGDraw);		// 背景管理
	DeleteEx(m_pShaveEfc);		// カスりエフェクト
	DeleteEx(m_pFragmentEfc);	// 破片管理
	DeleteEx(m_pTriangleEfc);	// 三角エフェクト

	DeleteEx(m_pEnemyCtrl);		// 敵コントロール

	DeleteEx(m_pSCLDecode);		// ＳＣＬデコード

	DeleteEx(m_pStateWindow);	// 状態表示ウィンドウ

	DeleteEx(m_pPlayerCtrl);	// プレイヤー管理
}


// ヒットチェック用コールバック関数 //
DWORD CGameMain::HitCheckCallback(void *pParam, EnemyData *pEnemy)
{
	CGameMain		*pThis;

	pThis = (CGameMain *)pParam;

	return pThis->m_pPlayerCtrl->HitCheck(pEnemy);
}
