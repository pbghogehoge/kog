/*
 *   CEnemyCtrl.cpp   : 敵管理クラス
 *
 */

#include "CEnemyCtrl.h"
#include "SCL.h"
#include "ECL.h"
#include "SECtrl.h"
#include "ScrBase.h"

#define ENEMY_DAMAGE		(18901)	// 約１％(削リ系)



// コンストラクタ //
CEnemyCtrl::
	CEnemyCtrl(RECT			*rcTargetX256	// 移動範囲矩形
			 , int			*pX				// 当たり判定ｘ座標
			 , int			*pY				// 当たり判定ｙ座標
			 , CFragmentEfc	*pFragment		// 破片管理クラス
			 , CShaveEffect	*pShaveEfc		// カスリエフェクト管理
			 , CTriEffect	*pTriEffect		// 破片エフェクト管理
			 , CBGDraw		*pBGDraw)		// 背景描画管理
	: m_EnemyTama(rcTargetX256, pX, pY, pShaveEfc)		// 弾管理クラス
	, m_EnemyLLaser(pX, pY, pShaveEfc)					// 太レーザー管理クラス
	, m_EnemySLaser(rcTargetX256, pX, pY, pShaveEfc, &m_EnemyLLaser)	// レーザー管理クラス
	, m_EnemyHLaser(rcTargetX256, pX, pY, pShaveEfc)	// Ｈレーザー管理クラス
	, m_EnemyLSphere(rcTargetX256, pX, pY, pShaveEfc)	// びりびり球体
	, m_EnemyExShot(rcTargetX256, pX, pY, pShaveEfc)	// その他特殊ショット
	, m_BossData(rcTargetX256, pTriEffect, pBGDraw)		// ボス管理クラス
{
	// 消去座標を初期化する                                           //
	// なお、敵弾とは異なり、敵のサイズには様々なものがあるので、     //
	// この時点では正確な消去座標が確定できない                       //
	m_XMin = rcTargetX256->left;		// 左端の消去座標
	m_YMin = rcTargetX256->top;			// 上端の消去座標
	m_XMax = rcTargetX256->right;		// 右端の消去座標
	m_YMax = rcTargetX256->bottom;		// 下端の消去座標

	m_XMid = (m_XMin + m_XMax) / 2;		// Ｘ座標の中心
	m_YMid = (m_YMin + m_YMax) / 2;		// Ｙ座標の中心

	m_HalfWidth  = (m_XMax - m_XMin) / 2;	// 幅 / 2
	m_HalfHeight = (m_YMax - m_YMin) / 2;	// 高さ / 2

	// マッドネスゲージ描画用矩形(グラフィック座標に変換のこと) //
	m_rcTarget = *rcTargetX256;
	m_rcTarget.left   >>= 8;
	m_rcTarget.top    >>= 8;
	m_rcTarget.right  >>= 8;
	m_rcTarget.bottom >>= 8;

	m_pX = pX;		// 当たり判定対象のＸ座標
	m_pY = pY;		// 当たり判定対象のＹ座標

	m_BossData.Initialize();

	// 各種クラスの関連づけを行う //
	m_pFragment    = pFragment;			// 破片管理クラス
}


// デストラクタ //
CEnemyCtrl::~CEnemyCtrl()
{
	// 現時点では、なにも行いません //
}


// 敵の初期化コア //
IVOID CEnemyCtrl::InitEnemyCore(	EnemyData	*pEnemy		// 初期化対象
								, 	int			x256		// 初期Ｘ座標
								,	int			y256		// 初期Ｙ座標
								,	DWORD		Offset		// 開始アドレス
								,	EnemyData	*pParent	// 親データ(NULL : 親なし)
								,	int			InitGr0)	// 初期 Gr0
{
	// 後は、各種パラメータを代入するのみ！                           //
	// 今回は、前作とは異なり、必要のないパラメータの初期化は行わない //
	pEnemy->x          = x256;			// 初期Ｘ座標
	pEnemy->y          = y256;			// 初期Ｙ座標
	pEnemy->hp         = 0xffffffff;	// 初期体力
	pEnemy->v          = 0;				// 速度
	pEnemy->d          = 64;			// 進行方向
	pEnemy->AnmCount   = 0;				// アニメーションカウンタ
	pEnemy->AnmSpd     = 0;				// アニメーションスピード
	pEnemy->RepCount   = 0;				// 繰り返し回数
	pEnemy->Score      = 0;				// スコア
	pEnemy->CallSP     = 0;				// コールスタックのスタックポインタ
	pEnemy->Count      = 0;				// 経過フレーム数
	pEnemy->Flag       = 0;				// フラグ無効化(最初に必ずセットすべし)
	pEnemy->DamageFlag = 0;				// ダメージフラグ
	pEnemy->size       = 0;				// 当たり判定サイズ
	pEnemy->Gr[0]      = InitGr0;		// ＧＲ０を特別に初期化
	pEnemy->pParent    = pParent;		// 親データに接続
	pEnemy->Alpha      = 255;			// α値

	// 子オブジェクトへのポインタを初期化する //
	memset(pEnemy->pChild, NULL, ENEMY_CHILD_MAX * sizeof(EnemyData *));

	// 命令開始アドレス //
	pEnemy->pCmd = CSCLDecoder::Offset2Ptr(Offset);

	pEnemy->pFinalAttack = NULL;	// 打ち返し関数
	pEnemy->pAnimePtn    = NULL;	// アニメーションパターン
}


// 親の存在しない敵をセットする(2001/09/22 : 挿入対象の追加) //
FVOID CEnemyCtrl::SetEnemy(
					int			x256		// 初期Ｘ座標
				,	int			y256		// 初期Ｙ座標
				,	DWORD		Offset		// 開始アドレス
				,	int			InitGr0		// 初期 Gr0
				,	BYTE		Target)		// 挿入対象(_NORMAL, ...)
{
	EnemyData		*pEnemy;

	pEnemy = InsertBack(Target);		// 挿入して、ポインタを取得
	if(NULL == pEnemy) return;			// 空きがありません

	InitEnemyCore(pEnemy, x256, y256, Offset, pEnemy, InitGr0);
}


// 子となる雑魚を生成する //
FVOID CEnemyCtrl::SetChild(
				DWORD		Offset		// 開始アドレス
			,	EnemyData	*pParent	// 親データ
			,	int			InitGr0		// 初期 Gr0
			,	BYTE		ChildID		// 子オブジェクトＩＤ
			,	BYTE		Target)		// 挿入対象(_NORMAL, ...)
{
	EnemyData		*pEnemy;

#ifdef PBG_DEBUG
	if(NULL == pParent){
		PbgError("NULL に対して SetChild() は実行できませんぜ");
		return;
	}

	if(ChildID >= ENEMY_CHILD_MAX){
		PbgError("子オブジェクトに対する参照が範囲外");
		return;
}
#endif

	pEnemy = InsertBack(Target);		// 挿入して、ポインタを取得
	if(NULL == pEnemy) return;			// 空きがありません

	// すでに子が割り当てられている場合は、その子を殺す //
	if(pParent->pChild[ChildID]){
		KillChild(pParent->pChild[ChildID]);
	}

	// 親から子に結びつける //
	pParent->pChild[ChildID] = pEnemy;

	InitEnemyCore(pEnemy, pParent->x, pParent->y, Offset, pParent, InitGr0);
}


// ボスをセットする(出現中なら、ＨＰ回復などなど) //
FVOID CEnemyCtrl::SetBoss(DWORD Offset, int InitGr0)
{
	EnemyData		*pEnemy;
	BOOL			bNeedInit;

	// すでにボスが出現しているかどうかを調べる //
	if(NULL != m_BossData.GetEnemyData()) bNeedInit = FALSE;	// ボス出現中
	else                                  bNeedInit = TRUE;		// 要初期化

	// ボスアタック要求を送信 //
	m_BossData.OnBossAttack();

	// ボスデータ内の標準敵データを取得する //
	pEnemy = m_BossData.GetEnemyData();
	if(NULL == pEnemy){
		PbgError("内部エラー：ボスの発生が出来ない");
		return;
	}

	// 初期化が必要な場合
	if(bNeedInit){
		// 参考：ボスは勿論、親無しオブジェクトである //
		InitEnemyCore(pEnemy, m_XMid, m_YMid, Offset, pEnemy, InitGr0);
	}
}


// ボスを強化する(青玉を出現させられる場合は真を返す) //
FBOOL CEnemyCtrl::BossSendExp(DWORD nShave)
{
	if(NULL == m_BossData.GetEnemyData()) return TRUE;

	return m_BossData.OnSendExp(nShave);

//	return TRUE;
}


// 指定された敵に強制的に KILL を実行させる(親との切り離しも行う) //
FVOID CEnemyCtrl::KillChild(EnemyData *pEnemy)
{
	int		i;

	// 死亡要求命令列 //
	static BYTE ForceKillTask[2] = {SCR_KILL, SCR_EXIT};


	for(i=0; i<ENEMY_CHILD_MAX; i++){
		if(pEnemy->pChild[i]){
			KillChild(pEnemy->pChild[i]);
			pEnemy->pChild[i] = NULL;
		}
	}

	// 親との接続を切り離す //
	pEnemy->pParent = pEnemy;

	// 死にたまえ //
	if(pEnemy->Flag & EFLG_DRAW_ON) pEnemy->pCmd = ForceKillTask;
	else                            pEnemy->pCmd = ForceKillTask + 1;
}


// 自分の全ての子に対して、 KillEnemy() を実行させる //
FVOID CEnemyCtrl::KillAllChild(EnemyData *pParent)
{
	int		i;

	for(i=0; i<ENEMY_CHILD_MAX; i++){
		if(pParent->pChild[i]){
			KillChild(pParent->pChild[i]);
			pParent->pChild[i] = NULL;
		}
	}
}


// 親に対して、自分が死んだことを報告する //
FVOID CEnemyCtrl::ChildDeadNotify(EnemyData *pChild)
{
	int			i;
	EnemyData	*pParent;

	pParent = pChild->pParent;

	// 親がいない場合 //
	if(pChild == pParent) return;

	// 私は死にましたよ //
	for(i=0; i<ENEMY_CHILD_MAX; i++){
		if(pChild == pParent->pChild[i]){
			pParent->pChild[i] = NULL;
			return;
		}
	}
}


// 敵弾コントロールオブジェクトを発生させる //
FVOID CEnemyCtrl
	::SetAtkObject(EnemyData *pThis, int dx256, int dy256, BYTE *pAddr, DWORD Param)
{
	EnemyAtkCtrl		*pAtk;
	int					ox, oy;

	// 親の存在している場合 //
	if(pThis){
		// ボスの場合 //
		if(m_BossData.GetEnemyData() == pThis){
			pAtk = m_AtkCtrl.InsertBack(EATK_BOSS);
		}
		// 通常の親の場合 //
		else{
			pAtk = m_AtkCtrl.InsertBack(GetUniqueID(pThis));
		}
		ox   = pThis->x + dx256;
		oy   = pThis->y + dy256;
	}
	// 親無しの場合 //
	else{
		pAtk = m_AtkCtrl.InsertBack(EATK_NOPARENT);
		ox   = dx256;
		oy   = dy256;
	}

	if(NULL == pAtk) return;


	// 構造体のゼロ初期化 //
	ZEROMEM(pAtk->m_Cmd);

	// 座標データの初期化 //
	pAtk->m_dx      = dx256;	// ｘ座標の差
	pAtk->m_dy      = dy256;	// ｙ座標の差
	pAtk->m_Cmd.ox  = ox;		// 現在のｘ座標
	pAtk->m_Cmd.ox  = oy;		// 現在のｙ座標

	// ポインタの初期化 //
	pAtk->m_pParent = pThis;	// 親データへのポインタ
	pAtk->m_pTCLCmd = pAddr;	// 命令へのポインタ

	// 繰り返し回数 //
	pAtk->RepCount  = 0;	// マクロ命令
	pAtk->CallSP    = 0;	// スタックポインタ

	// ATK2 用パラメータ //
	pAtk->m_Cmd.Param = Param;

	// 太レーザーとの接続は存在しない //
	pAtk->DisconnectLLaser(&m_EnemyLLaser);
}


// 初期化する //
FVOID CEnemyCtrl::Initialize(void)
{
	// リストを初期化する //
	DeleteAllData();

	// 全ての攻撃オブジェクトを破棄する //
	m_AtkCtrl.DeleteAllData();

	// 攻撃系オブジェクトの初期化      //
	// 2001/09/14 : 所有権の移動による //
	m_EnemyTama.Initialize();		// 敵弾
	m_EnemySLaser.Initialize();		// ショートレーザー
	m_EnemyLLaser.Initialize();		// 太レーザー
	m_EnemyHLaser.Initialize();		// ホーミングレーザー
	m_EnemyLSphere.Initialize();	// びりびり球体
	m_EnemyExShot.Initialize();		// その他特殊ショット

	// ボスの初期化を行う //
	m_BossData.Initialize();
}


// 敵全てに消去エフェクトをセットする //
FVOID CEnemyCtrl::Clear(void)
{
	Iterator				it;
	EnemyData				*p;
	AttackList::Iterator	atk_it;
	int						i;

	const int				ClearTarget[2] = {ENEMY_NORMAL, ENEMY_EXATK};
	int						Target, n;


	// 死亡中の敵の打ち返しをキャンセルする //
	ThisForEachFront(ENEMY_DELETE, it){
		it->pFinalAttack = NULL;
	}

	// エフェクト状態以外の敵に対して    //
	// -> これによって、鳥は死ななくなる //
	for(n=0; n<2; n++){
		Target = ClearTarget[n];

		// カウンタをゼロ初期化する //
		ThisForEachFront(Target, it){
			p = it.GetPtr();

			p->Count        = 0;
			p->hp           = 0;
			p->pFinalAttack = NULL;
			p->size         = (it->size * 5) / 2;

			if(p->Flag & EFLG_DRAW_ON) MoveBack(it, ENEMY_DELETE);
			else                       ThisDelContinue(it);
		}

		// 消去エフェクト側に移動する(通常→消去) //
//		MoveBackAll(ENEMY_DELETE, Target);
	}


	// 太レーザーの切り離しを行う             //
	// １つだけ多いのは、親無しオブジェクト用 //
	for(i=0; i<EATK_KIND; i++){
		ForEachFront(m_AtkCtrl, i, atk_it){
			atk_it->DisconnectLLaser(&m_EnemyLLaser);
//			atk_it->DisconnectLLaser(m_pEnemyLLaser);
		}
	}

	// 全ての攻撃オブジェクトを破棄する //
	m_AtkCtrl.DeleteAllData();


	// ボスを殺す //
	m_BossData.OnClear();
}


// 敵弾などに消去エフェクトをセットする //
__int64 CEnemyCtrl::ClearAtkObj(BOOL bChgScore)
{
	__int64		temp;

	temp = m_EnemyTama.Clear(bChgScore);	// 敵弾の消去
	m_EnemySLaser.Clear();					// ショートレーザーの消去
	m_EnemyLLaser.Clear();					// 太レーザーの消去
	m_EnemyHLaser.Clear();					// ホーミングレーザーの消去
	m_EnemyLSphere.Clear();					// びりびり球体の消去
	m_EnemyExShot.Clear();					// その他特殊ショットの消去

	return temp;
}


// 敵を移動させる //
FVOID CEnemyCtrl::Move(void)
{
	Iterator				it;
	AttackList::Iterator	atk_it;
	EnemyData				*pEnemy;
	int						i, n;
	int						width, height;
	int						ox, oy;
	int						w, h;

	const int		MoveTarget[3] = {ENEMY_NORMAL, ENEMY_EXATK, ENEMY_EFFECT};
	int				Target;

	ox     = this->m_XMid;
	oy     = this->m_YMid;
	width  = this->m_HalfWidth;
	height = this->m_HalfHeight;

	// ボスの処理(2001/09/23) //
	m_BossData.MoveBossGauge();
	pEnemy = m_BossData.GetEnemyData();
	if(pEnemy){
		// ボスの生成する雑魚は、エキストラアタック扱いとなる //
		UpdateEnemyData(pEnemy, ENEMY_EXATK);
	}

	// 通常状態の敵 //
	for(n=0; n<3; n++){
		Target = MoveTarget[n];

		ThisForEachFront(Target, it){
			pEnemy = it.GetPtr();

			// １フレーム分の更新を行う //
			switch(UpdateEnemyData(pEnemy, Target)){
				case ENRET_KILL:
					OnDamage(it, ~0);		// 死にたまえ
				continue;

				case ENRET_DELETE:
					KillAllChild(pEnemy);		// 全ての子を殺す
					ChildDeadNotify(pEnemy);	// 死んだことを報告
					KillAtkCtrl(pEnemy);		// オブジェクト切り離し
					ThisDelContinue(it);		// 消去
				continue;
			}

			// この部分で、敵の範囲内外チェックを行う //
			if(!(pEnemy->Flag & EFLG_CLIP_ON)){
				i = pEnemy->pAnimePtn->GetClipSize(pEnemy->AnmCount);
				w = width  + i;
				h = height + i;
				if(!(HitCheckSTD(oy, pEnemy->y, h)				// 高さ方向
						&& HitCheckSTD(ox, pEnemy->x, w))){		//  幅 方向
					KillAllChild(pEnemy);		// 全ての子を殺す
					ChildDeadNotify(pEnemy);	// 死んだことを報告
					KillAtkCtrl(pEnemy);		// オブジェクト切り離し
					ThisDelContinue(it);		// 消去
				}
			}
		}
	}


	// 消去状態の敵 //
	ThisForEachFront(ENEMY_DELETE, it){
		pEnemy = it.GetPtr();

		// 爆発エフェクトのサイズを小さくする //
		pEnemy->size -= 256;

		// 敵を削除する //
		if(pEnemy->size <= 0){
			// 打ち返しオブジェクトをセットする //
			if(pEnemy->pFinalAttack){
				SetAtkObject(NULL, pEnemy->x, pEnemy->y, pEnemy->pFinalAttack, 0);
			}

			ThisDelContinue(it);
		}

		m_pFragment->SetPointEfc(pEnemy->x, pEnemy->y, pEnemy->size >> 12);

		pEnemy->Count++;
		pEnemy->d -= 7;
	}


	// １つだけ多いのは、親無しオブジェクト用 //
	// 修正：2001/09/23 : ボス用のものを追加  //
	for(i=0; i<EATK_KIND; i++){
		ForEachFront(m_AtkCtrl, i, atk_it){
			if(FALSE == atk_it->ParseECLCmd(this)){
				DelContinue(m_AtkCtrl, atk_it);
			}
		}
	}

	m_EnemyTama.Move();		// 敵弾を更新
	m_EnemySLaser.Move();	// ショートレーザーを更新
	m_EnemyLLaser.Move();	// 細レーザーを更新
	m_EnemyHLaser.Move();	// ホーミングレーザーを更新
	m_EnemyLSphere.Move();	// びりびり球体を更新
	m_EnemyExShot.Move();	// その他特殊ショットを更新
}


// 敵を描画する //
FVOID CEnemyCtrl::Draw(void)
{
	Iterator			it;
	EnemyData			*pEnemy;
	D3DTLVERTEX			tlv[10];
	int					x, y, size;
	int					a;
	BYTE				d;

	// DrawTarget は描画を行う順番で配置すること //
	const int		DrawTarget[3] = {ENEMY_EFFECT, ENEMY_NORMAL, ENEMY_EXATK};
	int				Target, n;

	// 通常状態の敵 //
	for(n=0; n<3; n++){
		Target = DrawTarget[n];

		ThisForEachFront(Target, it){
			pEnemy = it.GetPtr();

			// 描画ビットが立っていない、かつ最前面フラグが立っていない //
			if(!(pEnemy->Flag & EFLG_DRAW_ON)) continue;
			if(  pEnemy->Flag & EFLG_TOPMOST)  continue;

			x = (pEnemy->x) >> 8;
			y = (pEnemy->y) >> 8;

			if(pEnemy->Flag & EFLG_SYNCANGLE_OFF) d = 0;
			else                                  d = (pEnemy->d);

			// pAnimePtn == NULL でもＯＫ //
			if(pEnemy->DamageFlag || (pEnemy->Flag & EFLG_ALPHAONE)){
				pEnemy->pAnimePtn->DrawDamage(x, y, d, pEnemy->AnmCount, pEnemy->Alpha);
			}
			else{
				pEnemy->pAnimePtn->DrawNormal(x, y, d, pEnemy->AnmCount, pEnemy->Alpha);
			}
		}
	}

	// ボスの描画を行う(2001/09/23) //
	pEnemy = m_BossData.GetEnemyData();
	if(pEnemy){
		x = (pEnemy->x) >> 8;
		y = (pEnemy->y) >> 8;

		if(pEnemy->Flag & EFLG_SYNCANGLE_OFF) d = 0;
		else                                  d = (pEnemy->d);

		// pAnimePtn == NULL でもＯＫ //
		if(pEnemy->DamageFlag || (pEnemy->Flag & EFLG_ALPHAONE)){
			pEnemy->pAnimePtn->DrawDamage(x, y, d, pEnemy->AnmCount, pEnemy->Alpha);
		}
		else{
			pEnemy->pAnimePtn->DrawNormal(x, y, d, pEnemy->AnmCount, pEnemy->Alpha);
		}
	}

	// TOPMOST 属性の敵 //
	for(n=0; n<3; n++){
		Target = DrawTarget[n];

		ThisForEachFront(Target, it){
			pEnemy = it.GetPtr();

			// 描画ビットが立っていない、または最前面フラグが立っていない //
			// 場合は、描画を行わない                                     //
			if(!(pEnemy->Flag & EFLG_TOPMOST)) continue;
			if(!(pEnemy->Flag & EFLG_DRAW_ON)) continue;

			x = (pEnemy->x) >> 8;
			y = (pEnemy->y) >> 8;

			if(pEnemy->Flag & EFLG_SYNCANGLE_OFF) d = 0;
			else                                  d = (pEnemy->d);

			// pAnimePtn == NULL でもＯＫ //
			if(pEnemy->DamageFlag || (pEnemy->Flag & EFLG_ALPHAONE)){
				pEnemy->pAnimePtn->DrawDamage(x, y, d, pEnemy->AnmCount, pEnemy->Alpha);
			}
			else{
				pEnemy->pAnimePtn->DrawNormal(x, y, d, pEnemy->AnmCount, pEnemy->Alpha);
			}
		}
	}

	// 消去状態の敵 //
	ThisForEachFront(ENEMY_DELETE, it){
		pEnemy = it.GetPtr();

		x    = pEnemy->x    >> 8;
		y    = pEnemy->y    >> 8;
		size = pEnemy->size >> 8;
		d    = pEnemy->d;
		a    = (255 - (pEnemy->Count * 5));
//		a    = (pEnemy->Alpha *  (255 - (pEnemy->Count * 5)) ) >> 8;

		if(a > 0){
			if(pEnemy->Flag & EFLG_ALPHAONE){
				a = (pEnemy->Alpha * (a+1)) >> 8;
				pEnemy->pAnimePtn->DrawDamage(x, y, d, pEnemy->AnmCount, a);
			}
			else{
				pEnemy->pAnimePtn->DrawDestroy(x, y, d, pEnemy->AnmCount, a);
			}
		}

		Set2DPointC(tlv+0, x-size, y-size, 0.0, 0.0, RGBA_MAKE(255, 255, 255, 255));
		Set2DPointC(tlv+1, x+size, y-size, 1.0, 0.0, RGBA_MAKE(255, 255, 255, 255));
		Set2DPointC(tlv+2, x+size, y+size, 1.0, 1.0, RGBA_MAKE(255, 255, 255, 255));
		Set2DPointC(tlv+3, x-size, y+size, 0.0, 1.0, RGBA_MAKE(255, 255, 255, 255));

		g_pGrp->SetTexture(TEXTURE_ID_EFFECT);
		g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

/*
	// 中心に線をひっぱる //
	Set2DPointC(tlv+0, m_XMid>>8, m_YMin>>8, 0, 0, RGBA_MAKE(255, 255, 255, 255));
	Set2DPointC(tlv+1, m_XMid>>8, m_YMax>>8, 0, 0, RGBA_MAKE(255, 255, 255, 255));
	pGrp->SetTexture(GRPTEXTURE_MAX);
	pGrp->DrawPrimitive(D3DPT_LINELIST, tlv, 2);
*/
}


// 敵弾などの描画を行う //
FVOID CEnemyCtrl::DrawAtkObj(void)
{
	// 描画順に注意すべし //
	m_EnemyLLaser.Draw();	// 太レーザー
	m_EnemyHLaser.Draw();	// ホーミングレーザー
	m_EnemyLSphere.Draw();	// びりびり球体
	m_EnemySLaser.Draw();	// ショートレーザー
	m_EnemyTama.Draw();		// 弾
	m_EnemyExShot.Draw();	// その他特殊ショット
}


// マッドネスゲージの描画を行う //
FVOID CEnemyCtrl::DrawMadnessGauge(void)
{
	m_BossData.DrawBossGauge();
}


// 通常の雑魚が生きていれば真を返す //
FBOOL CEnemyCtrl::IsNomalEnemyAlive(void)
{
	Iterator		it;

	// 一匹でも生きていれば、このループに突入する //
	ThisForEachFront(ENEMY_NORMAL, it){
		return TRUE;
	}

	return FALSE;
}


// ヒット有効な敵に対して DamageCallback を呼び出し、//
// 死亡した敵の合計スコアを返す                      //
FDWORD CEnemyCtrl::EnumActiveEnemy(DamageCallback Callback, void *pParam)
{
	Iterator		it;
	DWORD			Score;
	DWORD			Damage;
	EnemyData		*pEnemy;
	const int		EnumTarget[2] = {ENEMY_NORMAL, ENEMY_EXATK};
	int				Target, n;

	Score = 0;

	// 破壊可能な敵弾の処理(得点無し) //
	m_EnemyExShot.EnumBreakableExShot(Callback, pParam);

	// 全ての雑魚に対して(EFFECT は含まず) //
	for(n=0; n<2; n++){
		Target = EnumTarget[n];

		ThisForEachFront(Target, it){
			if(it->Flag & EFLG_HIT_ON){
				// ダメージ量の計算を行う //
				Damage = Callback(pParam, it.GetPtr());

				if(Damage){		// 実際にヒットしている場合
					Score += OnDamage(it, Damage);
				}
			}
		}
	}

	// ボスに対してダメージを与える(2001/09/23) //
	pEnemy = m_BossData.GetEnemyData();
	if(pEnemy && (pEnemy->Flag & EFLG_HIT_ON)){
		// 現時点では、ボスにスコアは存在しない //
		Damage = Callback(pParam, pEnemy);
		if(Damage){
			if(m_BossData.OnDamage(Damage)){
				KillBossAtkCtrl();
				KillAllChild(pEnemy);
			}
		}
	}

	// 得点を返す //
	return Score;
}


// 当たり判定を行う               //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemyCtrl::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	Iterator		it;
	EnemyData		*pEnemy;
	int				mx, my;			// 自機の座標
	int				tx, ty;			// 敵弾の座標
	int				dx, dy;			// 自機と敵弾の距離
	int				size, size2;	// 当たり判定幅(矩形,八角形)
//	CShaveEffect	*pEfc;			// エフェクト管理
	const int		HitCheckTarget[2] = {ENEMY_NORMAL, ENEMY_EXATK};
	int				Target, n;

	*pShave  = 0;
	*pDamage = 0;

	// 当たり判定の対象を確定する //
	mx = *m_pX;		// メンバ → ローカル
	my = *m_pY;		// の変換を行う

	// クラスへのポインタ参照の高速化 //
//	pEfc = m_pShaveEfc;

	// エフェクト状態の敵を除く敵との当たり判定を調べる //
	for(n=0; n<2; n++){
		Target = HitCheckTarget[n];
		ThisForEachFront(Target, it){
			pEnemy = it.GetPtr();

			// 当たり判定の存在しない場合は即座に戻る //
			if(!(pEnemy->Flag & EFLG_HIT_ON)) continue;

			tx    = pEnemy->x;		// 敵弾のＸ座標を取得
			ty    = pEnemy->y;		// 敵弾のＹ座標を取得
			size  = pEnemy->size;	// 当たり判定サイズ
			size2 = (size * 3) / 2;

			dx = abs(tx - mx);
			dy = abs(ty - my);

			// 八角形の内側にあるかね？ //
			if((dx < size) && (dy < size) && (dx + dy < size2)){
				if(pEnemy->Flag & EFLG_HPDAMAGE) (*pDamage) += (pEnemy->hp << 5);
				else                             (*pDamage) += ENEMY_DAMAGE;
//				pEnemy->Count = 0;
//				MoveBack(it, ENEMY_DELETE);
			}
		}
	}

	// ボスとの当たり判定を行う(2001/09/23) //
	pEnemy = m_BossData.GetEnemyData();
	if(pEnemy && (pEnemy->Flag & EFLG_HIT_ON)){
		tx    = pEnemy->x;		// 敵弾のＸ座標を取得
		ty    = pEnemy->y;		// 敵弾のＹ座標を取得
		size  = pEnemy->size;	// 当たり判定サイズ
		size2 = (size * 3) / 2;

		dx = abs(tx - mx);
		dy = abs(ty - my);

		// 八角形の内側にあるかね？ //
		if((dx < size) && (dy < size) && (dx + dy < size2)){
			(*pDamage) += ENEMY_DAMAGE;
		}
	}

	// 追加 (2001/09/14) 所有者の変更による                               //
	// なお、各関数の仕様が変更になっているので(ゼロ初期化無し)注意すべし //
	m_EnemyTama.HitCheck(pShave, pDamage);		// 敵弾の当たり判定
	m_EnemySLaser.HitCheck(pShave, pDamage);	// ショートレーザーの当たり判定
	m_EnemyLLaser.HitCheck(pShave, pDamage);	// 太レーザーの当たり判定
	m_EnemyHLaser.HitCheck(pShave, pDamage);	// ホーミングレーザーの当たり判定
	m_EnemyLSphere.HitCheck(pShave, pDamage);	// びりびり球体の当たり判定
	m_EnemyExShot.HitCheck(pShave, pDamage);	// その他特殊ショットの当たり判定
}


// 関連づけられた攻撃オブジェクトが存在すれば真を返す //
BOOL CEnemyCtrl::IsExistAtkObj(EnemyData *pThis)
{
	AttackList::Iterator	it;

// #pragma message(__FILE__" : ボス用の処理が抜け取るよ")
	// ボスの所有する攻撃オブジェクトの場合(2001/09/23) //
	if(pThis == m_BossData.GetEnemyData()){
		ForEachFront(m_AtkCtrl, EATK_BOSS, it){
			return TRUE;
		}

		return FALSE;
	}

	ForEachFront(m_AtkCtrl, GetUniqueID(pThis), it){
		return TRUE;
	}

	return FALSE;
}


// 関連づけられた AtkCtrl を殺す(雑魚用) //
FVOID CEnemyCtrl::KillAtkCtrl(EnemyData *pEnemy)
{
	AttackList::Iterator	atk_it;
	int						ID;

	// 現在、無条件で消去を行っているが、           //
	// レーザー系オブジェクトの場合は処理が別になる //
	ID = GetUniqueID(pEnemy);

	ForEachFront(m_AtkCtrl, ID, atk_it){
		atk_it->DisconnectLLaser(&m_EnemyLLaser);
		m_AtkCtrl.Delete(atk_it);
	}
}


// 関連づけられた AtkCtrl を殺す(ボス用) //
FVOID CEnemyCtrl::KillBossAtkCtrl(void)
{
	AttackList::Iterator	atk_it;

	// ボスの所有する攻撃オブジェクトの場合(2001/09/23) //
	// KillAtkCtrl() から分離する(2001/10/21)           //
	ForEachFront(m_AtkCtrl, EATK_BOSS, atk_it){
		atk_it->DisconnectLLaser(&m_EnemyLLaser);
		m_AtkCtrl.Delete(atk_it);
	}
}


// 敵データの更新を行う(範囲外チェックは含まない) //
UPDATE_ENEMYRET CEnemyCtrl::UpdateEnemyData(EnemyData *pEnemy, BYTE Target)
{
	int						spd;

	// １フレーム進めて、消去要求がある場合は消去する //
	if(FALSE == pEnemy->ParseECLCmd(this, Target)){
		if(pEnemy->Flag & EFLG_KILL){
			return ENRET_KILL;
			// OnDamage(it, ~0);		// 死にたまえ
			// continue;
		}
		else{
			return ENRET_DELETE;
			// KillAtkCtrl(pEnemy);	// オブジェクト切り離し
			// ThisDelContinue(it);	// 消去
		}
	}

	pEnemy->Count++;								// カウンタを更新する
	if(pEnemy->DamageFlag) pEnemy->DamageFlag--;	// ダメージフラグのデクリメント

	// アニメーションパターンを更新する //
	spd = pEnemy->AnmSpd;
	if(spd && (0 == (pEnemy->Count % spd))){
		pEnemy->AnmCount = pEnemy->pAnimePtn->GetNextPtn(pEnemy->AnmCount, spd);
	}

	// 当たり判定サイズの取得 //
	pEnemy->size = pEnemy->pAnimePtn->GetHitSize(pEnemy->AnmCount);

	return ENRET_OK;
}


// 敵にダメージを与える (ret : スコア) //
FDWORD CEnemyCtrl::OnDamage(Iterator &ref_it, DWORD Damage)
{
	EnemyData	*pEnemy;

	// イテレータ２ポインタ //
	pEnemy = ref_it.GetPtr();

	// ダメージを受けない状態の場合 //
	if(!(pEnemy->Flag & EFLG_HIT_ON)) return 0;

	// ダメージフラグを立てる //
	pEnemy->DamageFlag = 2;

	// 点エフェクトをセットする //
	m_pFragment->SetPointEfc(pEnemy->x, pEnemy->y, 20);


	if(Damage >= pEnemy->hp){
		KillAtkCtrl(pEnemy);		// 攻撃オブジェクトの破棄
		KillAllChild(pEnemy);		// 全ての子を殺す
		ChildDeadNotify(pEnemy);	// 死んだことを報告

		pEnemy->hp    = 0;						// ＨＰ無効化
		pEnemy->Count = 0;						// カウンタ初期化

		// 爆発半径をセットする //
		if(pEnemy->Flag & EFLG_BOMB2X){		// ２倍モード
			pEnemy->size  = (pEnemy->size * 5);
		}
		else{		// 通常モード
			pEnemy->size  = (pEnemy->size * 5)/2;
		}

		// 消去状態に移行する                                      //
		// (参考:移動しても、pEnemyは依然として正しいデータを指す) //
		MoveBack(ref_it, ENEMY_DELETE);

		// 効果音を再生する //
		PlaySE(SNDID_ENEMY_DESTROY, pEnemy->x);

		// スコアを返す //
		return pEnemy->Score;
	}
	else{
		// 体力を減少させる //
		pEnemy->hp -= Damage;
		return 0;
	}
}
