/*
 *   CPlayerCtrl.cpp   : キャラクタの移動処理
 *
 */

#include "CPlayerCtrl.h"
#include "CVivit.h"			// 主人公の定義
#include "CStg1Boss.h"		// ミリアの定義
#include "CStg2Boss.h"		// めい＆まいの定義
#include "CStg3Boss.h"		// ゲイツの定義
#include "CStg4Boss.h"		// マリーの定義
#include "CStg5Boss.h"		// エーリッヒの定義
#include "CMorgan.h"		// モーガンの定義
#include "CMuse.h"			// ミューズの定義
#include "CYuka.h"

#include "CEnemyCtrl.h"		// 敵管理
#include "SECtrl.h"
#include "FontDraw.h"

#include "StateID.h"



/***** [ 定数 ] *****/
#define GBCOUNT_INIT	600		// ガードブレイク持続時間
#define HITOFF_GBREAK	120		// ガードブレイク無敵時間



// コンストラクタ //
CPlayerCtrl::
	CPlayerCtrl(RECT			*rcTargetX256	// 移動範囲矩形
			  , DWORD			TextureID		// テクスチャＩＤ
			  , CTriEffect		*pTriEffect		// 三角形エフェクト
			  , CShaveEffect	*pShaveEfc)		// カスリエフェクト
	: CChargeGauge(rcTargetX256, &m_X, &m_Y)
{
	// ポインタ周辺の初期化 //
	m_pCharInfo   = NULL;	// キャラクタ情報クラス

	// 移動可能な範囲 //
	m_XMin   = rcTargetX256->left;				// 左端の停止座標
	m_YMin   = rcTargetX256->top + 40 * 256;	// 上端の停止座標
	m_XMax   = rcTargetX256->right;				// 右端の停止座標
	m_YMax   = rcTargetX256->bottom - 50*256;	// 下端の停止座標

	// 座標の初期値 //
	m_StartX = (m_XMin + m_XMax) / 2;	// Ｘ座標初期値
	m_StartY =  m_YMax - (50 * 256);	// Ｙ座標初期値

	// 現在の座標 //
	m_X = m_StartX;			// 現在のＸ座標 x 256
	m_Y = m_StartY;			// 現在のＹ座標 x 256

	// 溜めゲージの始点座標 //
	m_ChargeX = (m_XMin / 256);
	m_ChargeY = 480 - 40;

	// ポインタの接続 //
	m_pStdAttack   = NULL;			// 攻撃送り
	m_pRivalBGDraw = NULL;			// 背景描画
	m_pTriEffect   = pTriEffect;	// 三角形エフェクト
	m_pShaveEffect = pShaveEfc;		// カスリエフェクト

	// その他 //
	m_GrpState    = 0;				// グラフィックの状態
	m_TextureID   = TextureID;		// テクスチャＩＤ
	m_State       = PSTATE_DEAD;	// 現在の状態(ボム等)
	m_IsDamaged   = 0;				// ダメージを受けていれば非ゼロ値
	m_ShieldCount = 0;				// シールドの有効期間
	m_ShieldStart = 0;				// シールド開始カウント
}


// デストラクタ //
CPlayerCtrl::~CPlayerCtrl()
{
}


// 初期化する //
FBOOL CPlayerCtrl::PlayerInitialize(
			CHARACTER_ID	CharID				// キャラクタＩＤ
		  ,	int				AtkLv				// 初期攻撃レベル
		  , CStdAttack		*pStdAttack			// 標準攻撃送り
		  , CSCLDecoder		*pRivalSCLDecoder	// 相手のＳＣＬデコーダ
		  , CBGDraw			*pRivalBGDraw		// 相手の背景描画
		  , CAtkGrpDraw		*pRivalAtkGrpDraw	// 相手側のキャラ描画
		  , CBGDraw			*pThisAtkGrpDraw	// コイツのキャラ描画
		  , CEnemyCtrl		*pEnemy				// 敵管理(ExShot用)
		  , CEnemyCtrl		*pRivalEnemy		// 相手側の敵管理(ボス強化用)
		  , DWORD			CGaugeMax			// 溜めゲージ
		  , CStateWindow	*pThisStWnd			// 自分の状態窓
		  , CStateWindow	*pRivalStWnd)		// 相手の状態窓
{
	// 状態＆座標を初期化する //
	m_State       = PSTATE_NORMAL;	// 現在の状態
	m_X           = m_StartX;		// 現在のＸ座標
	m_Y           = m_StartY;		// 現在のＹ座標

	// ポインタを接続する //
	m_pStdAttack       = pStdAttack;		// 攻撃送り
	m_pRivalSCLDecoder = pRivalSCLDecoder;	// ＳＣＬデコーダ
	m_pRivalAtkGrpDraw = pRivalAtkGrpDraw;	// キャラ描画
	m_pRivalBGDraw     = pRivalBGDraw;		// 背景描画(相手)
	m_pThisBGDraw      = pThisAtkGrpDraw;	// 背景描画(コイツ)
	m_pEnemyCtrl       = pEnemy;			// 敵管理
	m_pRivalEnemy      = pRivalEnemy;		// 相手側の敵管理

	m_pThisState  = pThisStWnd;
	m_pRivalState = pRivalStWnd;

	// ガードゲージの初期化 //
	m_CurrentGuard = 100 * 10000;	// 現在の値
	m_RestoreGuard = 100 * 10000;	// 最大回復量
	m_DisplayGuard =   0 * 10000;	// 表示する値
	m_ChainDamage  =   0 * 10000;	// 連続して受けたダメージ
	m_GBreakCount  =   0;			// ガードブレイク状態用カウンタ
	m_ShieldCount  = 0;				// シールドの有効期間
	m_ShieldStart  = 0;				// シールド開始カウント

	// コンボの初期化 //
	m_LastBuzzChain = 0;	// 前回のカスリ繋ぎ
	m_MaxBuzzCombo  = 0;	// 最大コンボ数

	// その他の初期化 //
	m_IsDamaged      = 0;
	m_FinCount       = 0;
	m_bNeedETClear   = FALSE;
	m_bBombKeyPushed = FALSE;

	switch(CharID){
		// びびっと定義クラスに差し替える //
		case CHARACTER_VIVIT:
			m_pCharInfo = &g_VivitInfo;
		break;

		// ミリア定義クラスに差し替える //
		case CHARACTER_STG1:
			m_pCharInfo = &g_Stg1BossInfo;
		break;

		// めい＆まい定義クラスに差し替える //
		case CHARACTER_STG2:
			m_pCharInfo = &g_Stg2BossInfo;
		break;

		// ゲイツ定義クラスに差し替える //
		case CHARACTER_STG3:
			m_pCharInfo = &g_Stg3BossInfo;
		break;

		// マリー定義クラスに差し替える //
		case CHARACTER_STG4:
			m_pCharInfo = &g_Stg4BossInfo;
		break;

		// エーリッヒ定義クラスに差し替える //
		case CHARACTER_STG5:
			m_pCharInfo = &g_Stg5BossInfo;
		break;

		// モーガン定義クラスに差し替える //
		case CHARACTER_MORGAN:
			m_pCharInfo = &g_MorganInfo;
		break;

		// ミューズ定義クラスに差し替える //
		case CHARACTER_MUSE:
			m_pCharInfo = &g_MuseInfo;
		break;

		// 幽香定義クラスに差し替える //
		case CHARACTER_YUKA:
			m_pCharInfo = &g_YukaInfo;
		break;

		default:
			m_pCharInfo = NULL;
			PbgError("CPlayerCtrl::PlayerInitialize() : 未定義のプレイヤーＩＤ");
		return FALSE;
	}


	// ショット周辺の初期化を行う //
	m_NormalShot.Initialize(m_pCharInfo);	// ノーマルショットを初期化する
	m_ExtraShot.Initialize(m_pCharInfo);	// エキストラショットを初期化する

	// エフェクト系の初期化を行う //
	m_ChargeEfc.Initialize();		// 溜め完了エフェクト
	m_DeadEfc.Initialize();			// 死亡エフェクト

	// 溜めゲージの初期化を行って終了とする //
	return CGaugeInitialize(AtkLv, CGaugeMax);
}


// キーコードによって移動したときの座標を取得する //
// 引数 rX, rY  : 元の座標への参照                //
//      KeyCode : キーボード入力                  //
FVOID CPlayerCtrl::SetXYFromKeyCode(int &rX, int &rY, WORD KeyCode)
{
	int				dx, dy;		// ｘｙ方向の移動量
	int				Speed;		// 移動速度


	// 速度成分をゼロ初期化 //
	dx = dy = 0;

	// シフトを押しているか否かにより、関数を切り替える //
	if(KeyCode & KEY_SHIFT){
		// シフト移動を行う場合 //
		Speed = m_pCharInfo->GetShiftSpeed();
	}
	else{
		// 通常移動を行う場合 //
		Speed = m_pCharInfo->GetNormalSpeed();
	}

	if(KeyCode & KEY_LEFT)	dx -= Speed;	// 左方向(x256)
	if(KeyCode & KEY_RIGHT)	dx += Speed;	// 右方向(x256)
	if(KeyCode & KEY_UP)	dy -= Speed;	// 上方向(x256)
	if(KeyCode & KEY_DOWN)	dy += Speed;	// 下方向(x256)

	// ナナメ移動を行う場合 //
	if(dx && dy){
		// 256 * sqrt(2) = 362 //
		dx = (dx<<8) / 362;	// Ｘを２の平方根(x256) で割る
		dy = (dy<<8) / 362;	// Ｙを２の平方根(x256) で割る
	}

	// まずは、一時変数に格納する //
	dx += rX;		// Ｘ方向の移動成分と元の値を加算する
	dy += rY;		// Ｙ方向の移動成分と元の値を加算する

	// それから、範囲チェックをして //
	if(      dx < m_XMin)	dx = m_XMin;	// 左端チェック
	else if( dx > m_XMax)	dx = m_XMax;	// 右端チェック
	if(      dy < m_YMin)	dy = m_YMin;	// 上端チェック
	else if( dy > m_YMax)	dy = m_YMax;	// 下端チェック

	// 更新 //
	rX = dx;	// Ｘ座標を更新
	rY = dy;	// Ｙ座標を更新
}


// 移動する //
FVOID CPlayerCtrl::PlayerMove(WORD KeyCode)
{
	ExtraShotInfo	ExInfo;		// エキストラショット情報
	int				restore;	// 回復量(難易度別)
	int				size;		// ガード膜の当たり判定用

	// 敵弾消去要求フラグを解除する //
	if(m_bNeedETClear) m_bNeedETClear = FALSE;

	if(PSTATE_DEAD == m_State || PSTATE_WON == m_State){
		KeyCode = 0;
	}
	else{
		// 溜めゲージを更新する //
		CGaugeUpdate(KeyCode);
	}

	// 描画する画像パターンを更新する //
	m_pCharInfo->PlayerSetGrp(&m_GrpState, KeyCode);

	// エキストラショット用の情報をセットする //
	ExInfo.Charge     = 0;				// このメンバは無効化する
	ExInfo.KeyCode    = KeyCode;		// キーコード
	ExInfo.pExtraShot = &m_ExtraShot;	// エキストラショットの格納先
	ExInfo.State      = m_GrpState;			// キャラクタの傾き

	// ショットを動作させる //
	m_NormalShot.Move();								// Normal ショット
	m_pCharInfo->MoveExtraShot(&ExInfo, m_pEnemyCtrl);	// Extra  ショット

	// エフェクト系の動作 //
	m_ChargeEfc.Move();		// 溜め完了エフェクト
	m_DeadEfc.Move();		// 死亡エフェクト

	switch(m_State){
		// 勝敗が決定していたら、ここで終了 //
		case PSTATE_DEAD:
			m_FinCount++;
		return;

		case PSTATE_WON:
			m_FinCount++;

			if(m_Y > -50 * 256){
				m_Y -= (m_FinCount < 40)
					 ? (- 3 * 256 / 2)
					 : (4 * 256 - 64 + m_FinCount * 32);
//				m_Y -= (m_FinCount < 40)
//					? (- m_pCharInfo->GetShiftSpeed())
//					: (m_pCharInfo->GetNormalSpeed() + m_FinCount * 32);
			}
		return;

		default:
			// キーコードに従って、座標を更新する //
			SetXYFromKeyCode(m_X, m_Y, KeyCode);
		break;
	}

	// ガードゲージの制御(表示部) //
	if(m_CurrentGuard != m_DisplayGuard){
		if(m_CurrentGuard > m_DisplayGuard){
			m_DisplayGuard += 8167;

			if(m_CurrentGuard < m_DisplayGuard){
				m_DisplayGuard = m_CurrentGuard;
			}
		}
		else{
			m_DisplayGuard -= 8167;

			if(m_CurrentGuard > m_DisplayGuard){
				m_DisplayGuard = m_CurrentGuard;
			}
		}
	}

	// ガードブレイク状態の反映 //
	if(PSTATE_GBREAK == m_State){
		m_GBreakCount--;

		if(m_GBreakCount <= 0){
			m_ChargeEfc.SetGBFinished();
			m_State = PSTATE_NORMAL;
		}
	}
	// ガードゲージの制御(回復) //
	else if(m_CurrentGuard < m_RestoreGuard){	// 回復量のほうが大きい場合
		restore = 10000 / 60;					// １ｓに１％回復
		m_CurrentGuard = min(m_RestoreGuard, m_CurrentGuard + restore);
	}

	// 保護膜の処理(敵弾消去＆残り時間のデクリメント) //
	if(m_ShieldCount){
		const int TextureSize = 64;	// テクスチャのサイズ
		const int SpaceSize   = 6;	// 空白のサイズ

		// 敵弾 <-> シールド の当たり判定 //
		size = (GetShieldSize() * (TextureSize - SpaceSize)) / TextureSize;
		m_pEnemyCtrl->GetEnemyTamaPtr()->ClearFromOct(m_X, m_Y, size);

		// 残り時間減少 //
		m_ShieldCount--;
	}

	// コンボにより自動アタックを発動する //
	ChainAttack();

	// ダメージ表示用変数のデクリメント //
	if(m_IsDamaged){
		m_IsDamaged--;
	}
	else if(m_ChainDamage){
		// 連続ヒット打ち切りとして、回復量を求める //
		m_RestoreGuard = m_CurrentGuard + (m_ChainDamage*50)/100;
		if(m_RestoreGuard > 100 * 10000){
			m_RestoreGuard = 100 * 10000;
		}
		m_pRivalState->InsertString(STWNDID_DAMAGE, m_RestoreGuard/10000);

		m_ChainDamage = 0;
	}
}


// ヒットしている場合 //
FVOID CPlayerCtrl::OnDamage(DWORD Damage)
{
	// 無敵状態ならダメージの処理は行わない //
	if(m_ShieldCount) return;

	// 勝敗が決定していたら、何もしない //
	if(PSTATE_DEAD == m_State || PSTATE_WON == m_State){
		return;
	}

	// ガードブレイク中の動作 //
	if(PSTATE_GBREAK == m_State){
		// 負け or 引き分け確定 //
		m_DeadEfc.Set(m_X, m_Y);	// 死亡エフェクトセット
		m_pTriEffect->SetGuardBreak(m_X, m_Y);
		m_pTriEffect->SetGuardBreak(m_X, m_Y);

		CGaugeReset();				// 溜めゲージをリセット
		m_State = PSTATE_DEAD;		// 状態推移
		return;
	}

	// ガード不能 //
	if(CGaugeGetCurrent() >= CGAUGE_LV1HALF){
//		m_CurrentGuard = 0;
		m_ChainDamage  = 0;

		if(Damage > m_CurrentGuard) m_CurrentGuard  = 0;
		else                        m_CurrentGuard -= Damage;

		m_RestoreGuard = m_CurrentGuard;

		if(m_RestoreGuard){
			m_pRivalState->InsertString(STWNDID_DAMAGE, m_RestoreGuard/10000);
		}

		PlaySE(SNDID_DAMAGE_CHARGE, m_X);
	}
	// ガードＯＫ //
	else{
		if(Damage > m_CurrentGuard){
			m_CurrentGuard  = 0;
		}
		else{
			m_CurrentGuard -= Damage;
			m_ChainDamage  += Damage;
		}

		PlaySE(SNDID_DAMAGE_NORMAL, m_X);
	}

	// ガードブレイク発生 //
	if(0 == m_CurrentGuard){
		m_RestoreGuard = 30 * 10000;	// 最大回復量
		m_ChainDamage  = 0;				// 累積ダメージ
		m_GBreakCount  = GBCOUNT_INIT;	// ガードブレイク時間
		m_State        = PSTATE_GBREAK;	// ガードブレイク状態に移行

		ShieldOn(HITOFF_GBREAK);	// シールドを有効にする
		ChainAttack();				// これまでに繋いできたコンボを相手側に送る
		CGaugeReset();				// 溜めゲージをリセット

		m_pTriEffect->SetGuardBreak(m_X, m_Y);
		m_pRivalState->InsertString(STWNDID_KOCHANCE, 0);
		m_pThisState->InsertString(STWNDID_GBREAK, 0);
	}
	else{
		// ダメージフラグを立てる                                 //
		// 注意：２０ｆｐｓでも表示できるように３以上の値とすべし //
		m_IsDamaged = 3;
	}
}


// カスっている場合 //
FVOID CPlayerCtrl::OnShave(DWORD Shave)
{
	CGaugeBuzz(Shave);
}


// 勝った時に呼び出す //
FVOID CPlayerCtrl::OnPlayerWon(void)
{
	m_State = PSTATE_WON;
}


// 敵<->自機ショットの当たり判定 //
FDWORD CPlayerCtrl::HitCheck(EnemyData *pEnemy)
{
	DWORD		Damage;

	Damage  = m_NormalShot.HitCheck(pEnemy);
	Damage += m_pCharInfo->HitCheckExtraShot(&m_ExtraShot, pEnemy);

	return Damage;
}


// プレイヤーの描画を行う //
FVOID CPlayerCtrl::DrawPlayer(void)
{
	ExtraShotInfo		ExInfo;		// エキストラショット情報構造体
	BOOL				bDamaged;	// ダメージフラグ


	// ダメージを受けていれば、真とする //
	if(m_IsDamaged) bDamaged = TRUE;
	else            bDamaged = FALSE;

/*
	if(PSTATE_DEAD != m_State || m_FinCount < 80){
		// プレイヤーそのものの描画を行う //
		m_pCharInfo->PlayerDraw(m_X, m_Y, m_GrpState, bDamaged, m_TextureID);
	}
*/
	if(PSTATE_DEAD != m_State){
		// プレイヤーそのものの描画を行う //
		m_pCharInfo->PlayerDraw(m_X, m_Y, m_GrpState, bDamaged, 0, m_TextureID);
	}
	else if(m_FinCount < 80){
		// プレイヤーそのものの描画を行う //
		m_pCharInfo->PlayerDraw(m_X, m_Y, m_GrpState, TRUE, m_FinCount, m_TextureID);
	}

	// エキストラショット用の情報をセットする //
	ExInfo.Charge     = 0;				// このメンバは無効化する
	ExInfo.KeyCode    = 0;				// 描画にキーコードは不要
	ExInfo.pExtraShot = &m_ExtraShot;	// エキストラショットの格納先
	ExInfo.State      = m_GrpState;		// キャラクタの傾き

	// ショット系の描画を行う //
	m_NormalShot.Draw();								// Normal ショット
	m_pCharInfo->DrawExtraShot(&ExInfo, m_TextureID);	// Extra  ショット

	// 溜めエフェクトを描画する //
	m_ChargeEfc.Draw();

	// ガードエフェクトを描画する //
	DrawShield();

	// 死亡エフェクトを描画する //
	m_DeadEfc.Draw();
}


// ガードゲージの描画を行う //
FVOID CPlayerCtrl::DrawGuardGauge(void)
{
	D3DTLVERTEX		tlv[10];
	D3DVALUE		u, v;
	char			buf[30];
	int				i;//, x0, x1, y0, y1, x2, y2;
	int				sx, sy, n, nv, dx, dy;
	DWORD			c1, c2;//, c3;
	BYTE			d;

	sx = (m_XMin / 256);
	sy = 0;

	g_pGrp->SetTexture(TEXTURE_ID_GUARD);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);


	c2 = RGBA_MAKE(255, 255, 255, 16);

	Set2DPointC(tlv+0, sx+36, sy+36, 0.5, 0.5, c2);	// org
	Set2DPointC(tlv+1, sx+72, sy+72, 1.0, 1.0, c2);	// 32
	Set2DPointC(tlv+2, sx,    sy+72, 0.0, 1.0, c2);	// 64
	Set2DPointC(tlv+3, sx,    sy,    0.0, 0.0, c2);	// 128
	Set2DPointC(tlv+4, sx+72, sy,    1.0, 0.0, c2);	// 128
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 5);

	c2 = RGBA_MAKE(255, 0, 0, 16);

	n  = m_DisplayGuard / 10000;
	if(n >= 80){
		c1 = RGBA_MAKE( 10,  10, 156, 255);
	}
	else if(n >= 20){
		c1 = RGBA_MAKE(100, 100,  32, 255);
	}
	else{
		c1 = RGBA_MAKE(196,  32,  32, 255);
	}

	d  = ((n * 192) / 100) + 32;
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	Set2DPointC(tlv+0, sx+36, sy+36, 0.5, 0.5, c1);
	Set2DPointC(tlv+1, sx+72, sy+72, 1.0, 1.0, c1);

	if(d > 160){
		Set2DPointC(tlv+2, sx,    sy+72, 0.0, 1.0, c1);	// 64
		Set2DPointC(tlv+3, sx,    sy,    0.0, 0.0, c1);	// 128


		dx = -CosL(d, 36*256) / SinL(d, 256) + (sx + 36);
		u  = D3DVAL(dx-sx) / D3DVAL(72);
		Set2DPointC(tlv+4, dx, sy, u, 0.0, c1);

		nv = 5;
	}
	else if(d > 96){
		Set2DPointC(tlv+2, sx, sy+72, 0.0, 1.0, c1);	// 64

		dy = SinL(d, -36*256) / CosL(d, 256) + (sy + 36);
		v  = D3DVAL(dy-sy) / D3DVAL(72);
		Set2DPointC(tlv+3, sx, dy, 0.0, v, c1);

		nv = 4;
	}
	else{
		dx = CosL(d, 36*256) / SinL(d, 256) + (sx + 36);
		u  = D3DVAL(dx-sx) / D3DVAL(72);

		Set2DPointC(tlv+2, dx, sy+72, u, 1.0, c1);

		nv = 3;
	}

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, nv);

	for(i=0; i<nv; i++){
		tlv[i].color = RGBA_MAKE(48, 48, 48, 255);
	}
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, nv);


	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);

	sx = 43 + (m_XMin / 256 - 4);
	sy = 36;

	wsprintf(buf, "%03d", m_DisplayGuard/10000);
//	g_Font.DrawNumber(18, 20, buf, 20);
	g_Font.DrawNumber(sx-22, sy-16, buf, 20);

	wsprintf(buf, "%04d", m_DisplayGuard%10000);
//	g_Font.DrawNumber(42, 39, buf, 12);
	g_Font.DrawNumber(sx+2, sy+3, buf, 12);

	g_Font.Draw(sx+40, sy+3, "%", 12);
}


// その他・小物の描画を行う //
FVOID CPlayerCtrl::DrawExtra(Pbg::CGrpSurface *pSurface)
{
	CGaugeDraw(m_ChargeX, m_ChargeY, pSurface);
}


// カスリコンボによる攻撃 //
FVOID CPlayerCtrl::ChainAttack(void)
{
	DWORD	Current = m_pShaveEffect->GetShaveCombo();
	DWORD	i;


	// ボスアタックモードの場合 //
	if(NULL == m_pRivalEnemy){
		return;
	}

	// コンボが切れた場合 //
	if(Current < m_LastBuzzChain){
		// なんか発動 //
	}

	// ボスの強化を試みて、青玉転送が行える場合はさらに追加攻撃を //
	if(TRUE == m_pRivalEnemy->BossSendExp(Current - m_LastBuzzChain)){
		for(i=m_LastBuzzChain+1; i<=Current; i++){
			if(i % 4 == 0){
				m_pStdAttack->Set(m_X, m_Y, i/18 + CGaugeGetAtkLv()/4);
			}

			if(i % 10 == 0){
				m_pRivalSCLDecoder->SetComboAttack(m_pCharInfo->GetCharID());
			}
		}
	}

	m_LastBuzzChain = Current;

	if(Current > m_MaxBuzzCombo){
		m_MaxBuzzCombo = Current;
	}
}


// 保護膜のサイズを取得(x256) //
FINT CPlayerCtrl::GetShieldSize(void)
{
	return min(m_ShieldCount/2, 50) << 8;
}


// 無敵時間の保護膜？描画 //
FVOID CPlayerCtrl::DrawShield(void)
{
	D3DTLVERTEX		tlv[20];
	D3DVALUE		u1, v1, u2, v2;
	int				x, y, dx, dy;
	int				t, t1;//, t2;
	DWORD			c;
	BYTE			a, d;


	// 当たり判定が存在しているなら、即リターン //
	if(0 == m_ShieldCount) return;

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_SHIELD);

	x  = m_X >> 8;
	y  = m_Y >> 8;
	a  = min(255 - (SinL(m_ShieldCount, 20) + 40), m_ShieldCount*4);
	c  = RGBA_MAKE(a, a, a, 255);
	d  = m_ShieldCount * 10;

	dx = GetShieldSize();
//	dy = 0;//SinL(m_ShieldCount*18, dx / 8);
	t1 = dx;// + dy;
//	t2 = dx;// - dy;

	dx = (CosL(d+128, t1) + CosL(d-64, t1)) >> 8;
	dy = (SinL(d+128, t1) + SinL(d-64, t1)) >> 8;
//	dx = (CosL(d+128, t1) + CosL(d-64, t2)) >> 8;
//	dy = (SinL(d+128, t1) + SinL(d-64, t2)) >> 8;
	Set2DPointC(tlv+0, x+dx, y+dy, 0.0, 0.0, c);
	Set2DPointC(tlv+2, x-dx, y-dy, 1.0, 1.0, c);

	dx = (CosL(d, t1) + CosL(d-64, t1)) >> 8;
	dy = (SinL(d, t1) + SinL(d-64, t1)) >> 8;
//	dx = (CosL(d, t1) + CosL(d-64, t2)) >> 8;
//	dy = (SinL(d, t1) + SinL(d-64, t2)) >> 8;
	Set2DPointC(tlv+1, x+dx, y+dy, 1.0, 0.0, c);
	Set2DPointC(tlv+3, x-dx, y-dy, 0.0, 1.0, c);

	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);



	c = RGBA_MAKE(0, 255, 0, 32);
	Set2DPointC(tlv+0, x, y, 0, 0, c);

	c = RGBA_MAKE(0, 255, 0, 64);
	Set2DPointC(tlv+1, x+10, y+50, 0, 0, c);

	c = RGBA_MAKE(0, 255, 0, 96);
	Set2DPointC(tlv+2, x+20, y+50, 0, 0, c);

	g_pGrp->SetTexture(GRPTEXTURE_MAX);
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
	g_pGrp->DrawPrimitive(D3DPT_LINESTRIP, tlv, 3);

	t = min(m_ShieldStart, 2 * (m_ShieldStart - m_ShieldCount));
	dx = (144 * t) / max(1, m_ShieldStart);
	dx -= (dx%16);

	u1 = D3DVAL(  0) / D3DVAL(256);
	v1 = D3DVAL(184) / D3DVAL(256);
	u2 = D3DVAL( dx) / D3DVAL(256);
	v2 = D3DVAL(200) / D3DVAL(256);

	x += 20;
	y += 46;

	c = RGBA_MAKE(255, 255, 255, 255);

	Set2DPointC(tlv+0, x     , y     , u1, v1, c);
	Set2DPointC(tlv+1, x+dx/3, y     , u2, v1, c);
	Set2DPointC(tlv+2, x+dx/3, y+16/3, u2, v2, c);
	Set2DPointC(tlv+3, x     , y+16/3, u1, v2, c);

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->SetTexture(TEXTURE_ID_FONT);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);


	if((t < m_ShieldStart) || (m_ShieldCount % 8 > 3)){
		u1 = D3DVAL(144) / D3DVAL(256);
		v1 = D3DVAL(152) / D3DVAL(256);
		u2 = D3DVAL(160) / D3DVAL(256);
		v2 = D3DVAL(168) / D3DVAL(256);

		Set2DPointC(tlv+0, x+dx/3,   y     , u1, v1, c);
		Set2DPointC(tlv+1, x+dx/3+8, y     , u2, v1, c);
		Set2DPointC(tlv+2, x+dx/3+8, y+16/3, u2, v2, c);
		Set2DPointC(tlv+3, x+dx/3,   y+16/3, u1, v2, c);

		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}
}
