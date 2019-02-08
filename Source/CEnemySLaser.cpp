/*
 *   CEnemySLaser.cpp   : 敵用ショートレーザー
 *
 */

#include "CEnemySLaser.h"

#include "RndCtrl.h"



/***** [スタティックメンバ] *****/

// 描画リスト //
D3DTLVERTEX CEnemySLaser::m_RenderList[ESLASER_BUFFERSIZE];

// 現在の頂点データ挿入先 //
D3DTLVERTEX *CEnemySLaser::m_pRLTarget;

// バッファが敵弾用に初期化されたなら、TRUE になる          //
// すなわち、最初にコンストラクタが呼び出された時に真となる //
BOOL CEnemySLaser::m_BufferInitialized = FALSE;



/***** [ 定数 ] *****/
#define ESL_WIDTH			(2*256)
#define ESL_SHAVE			(16*256)
#define ESL_DAMAGE			(123456*2)	// 約２０％
#define ESL_SHAVEWEIGHT		3			// １カスりの重み



// コンストラクタ //
CEnemySLaser
	::CEnemySLaser(RECT			*rcTargetX256	// 対象となる矩形
				 , int			*pX				// 自機のＸ座標へのポインタ
				 , int			*pY				// 自機のＹ座標へのポインタ
				 , CShaveEffect	*pShaveEfc		// カスりエフェクト発動用クラス
				 , CEnemyLLaser	*pLLaser)		// ふとれざ管理
{
	// 座標を格納するのみ //
	m_XMin = rcTargetX256->left;	// 左端の座標
	m_YMin = rcTargetX256->top;		// 上端の座標
	m_XMax = rcTargetX256->right;	// 右端の座標
	m_YMax = rcTargetX256->bottom;	// 下端の座標

	// ポインタの接続を行う //
	m_pX        = pX;			// 当たり判定Ｘ座標
	m_pY        = pY;			// 当たり判定Ｙ座標
	m_pShaveEfc = pShaveEfc;	// カスりエフェクト管理クラス
	m_pLLaser   = pLLaser;		// ふとれざ管理

	// 描画用バッファを初期化する //
	InitializeBuffer();
}


// デストラクタ //
CEnemySLaser::~CEnemySLaser()
{
}


// 敵弾をセットする(引数がポインタでないことに注意) //
FVOID CEnemySLaser::Set(AttackCommand Command)
{
	// 関数ポインタ //
	char (*pGetAngle)(AttackCommand *pCmd, int i);	// 角度取得用のポインタ
	int  (*pGetSpeed)(AttackCommand *pCmd);			// 速度取得用のポインタ

	SLaserData	*pLaser;			// データ書き込み用のポインタ

//	DWORD		TargetList;		// 対象となるリストＩＤ
	DWORD		i;				// ループ用
	DWORD		NumLaser;		// セットしたい本数
//	DWORD		n;				// 本数

	BYTE		Angle;			// 角度の一時格納用
	int			Speed;			// 速度の一時格納用
	BYTE		AShift;			// 角度のシフト値
//	char		NdAngle;		// 角度ずらしの単位


	// エラーログ用の関数名をセット //
#ifdef PBG_DEBUG
	PbgErrorInit("CEnemySLaser::Set()");
#endif

	// 難易度変化の無効化ビットが立っていない場合 //
	if( !(Command.Cmd & LCMD_NODIF) ){
		//EasyCommand(&Command);
		//HardCommand(&Command);
		//LunaCommand(&Command);
	}


	// 発射方向別に、関数を振り分ける                            //
	// 0xff00 と論理積を取っているのは、上位８ビットが発射形状を //
	// 確定するためのビットとなっているためである                //
	switch(Command.Cmd & 0xff00){
		case LCMD_WAY:	pGetAngle = LaserAngleWay;	break;	// 扇状発射
		case LCMD_ALL:	pGetAngle = LaserAngleAll;	break;	// 全方向発射
		case LCMD_RND:	pGetAngle = LaserAngleRnd;	break;	// ランダム発射

		case LCMD_ONE:	// １方向発射
			pGetAngle   = LaserAngleOne;	// ０を返す関数
			Command.Num = 1;				// これは忘れちゃイカンね
		break;

		default:
		#ifdef PBG_DEBUG
			PbgErrorEx("Command.Cmd : 発射形状ビットが不正ね♪");
		#endif
		return;
	}

	// 弾の速度セット用の関数をセットする //
	// 参考：速度ランダムビットで判定する //
	if(Command.Cmd & LCMD_RNDSPD) pGetSpeed = LaserSpeedRnd;// ランダムセット時
	else                          pGetSpeed = LaserSpeedStd;// 通常時

	NumLaser = Command.Num;

	// AShift は、角度変数と自機セット反映に用いられる          //
	// 前Version と異なり、ここで基本角もセットしている点に注意 //
	if(Command.Cmd & LCMD_SET){
		// 角度セット属性ビットが立っている場合 //
		AShift = Atan8((*m_pX) - Command.ox, (*m_pY) - Command.oy) + Command.Angle;
	}
	else{
		// 角度セットビットが立っていない場合 //
		AShift = Command.Angle;
	}

	// それでは、挿入してしまいましょ //
	for(i=0; i<NumLaser; i++){
		pLaser = InsertBack(ESLASER_NORMAL);	// 挿入＆ポインタ取得
		if(NULL == pLaser) return;				// 空き領域が存在せず

		// そのまま代入して問題の無いパラメータ //
		pLaser->x        = Command.ox;		// 初期ｘ座標
		pLaser->y        = Command.oy;		// 初期ｙ座標
		pLaser->lmax     = Command.Length;	// 最終的な長さ
		pLaser->Color    = Command.Color;	// 色
		pLaser->Rep      = Command.Rep;		// 繰り返し回数
		pLaser->Type     = Command.Type;	// 種類
		pLaser->l        = 0;				// 初期のレーザー長
		pLaser->Count    = 0;				// 発生から経過したフレーム数
		pLaser->IsShaved = 0;				// カスっているかどうかのフラグ
		pLaser->RefState = LRST_SHOT;		// 反射レーザーの状態定数

		// まずは、速度＆角度を一時的に取得する //
		Angle = pGetAngle(&Command, i) + AShift;
		Speed = pGetSpeed(&Command);

		// 速度と角度を決定する //
		pLaser->v     = Speed;
		pLaser->Angle = Angle;

		// 速度成分を決定する //
		pLaser->vx = CosL(Angle, Speed);
		pLaser->vy = SinL(Angle, Speed);
	}
}


// 初期化する //
FVOID CEnemySLaser::Initialize(void)
{
	// リスト内のデータを解放する //
	DeleteAllData();
}


// 敵弾全てに消去エフェクトをセットする //
FVOID CEnemySLaser::Clear(void)
{
	Iterator			it;

	// カウンタをゼロ初期化する //
	ThisForEachFront(ESLASER_NORMAL, it){
		SetClearLaserState(it);
	}

	// 消去側に移動する //
	MoveBackAll(ESLASER_DELETE, ESLASER_NORMAL);
}


// レーザーを移動させる //
FVOID CEnemySLaser::Move(void)
{
	Iterator		it;
	SLaserData		*pLaser;
	int				mx, my;
	int				XMin, YMin, XMax, YMax;


	// メンバ変数 -> ローカル変数に変換して、高速化を図る //
	XMin = m_XMin;		// 左端の座標
	YMin = m_YMin;		// 上端の座標
	XMax = m_XMax;		// 右端の座標
	YMax = m_YMax;		// 下端の座標


	// 通常状態のレーザー //
	ThisForEachFront(ESLASER_NORMAL, it){
		// イテレータからポインタに変換 //
		pLaser = it.GetPtr();

		switch(pLaser->Type){
		case SLASER_NORM:		// 通常のレーザーの場合
			NormLaserMove(pLaser);
		break;

		case SLASER_REF:		// 反射レーザーの場合
		case SLASER_LLREF:		// ふとれざ反射の場合
		case SLASER_SEARCH:		// 探索する場合
			if(FALSE == RefLaserMove(pLaser)){
				ThisDelContinue(it);
			}
		break;
		}

		// 座標を取得する //
		mx = pLaser->x;		// 弾のＸ座標
		my = pLaser->y;		// 弾のＹ座標

		// 範囲外ならば、削除する                                     //
		if(mx < XMin || mx > XMax || my < YMin  || my > YMax){
			ThisDelContinue(it);
		}

		// カウンタをインクリメントする //
		pLaser->Count++;
	}

	// 消去状態のレーザー //
	ThisForEachFront(ESLASER_DELETE, it){
		// イテレータからポインタに変換 //
		pLaser = it.GetPtr();

//		if(pLaser->Count > 30) ThisDelContinue(it);
/*
		pLaser->l += pLaser->v  / 4;
		pLaser->x += pLaser->vx / 2;
		pLaser->y += pLaser->vy / 2;
*/
		pLaser->l -= (pLaser->v / 2);
		if(pLaser->l < 0) ThisDelContinue(it);

		pLaser->Angle += 8;
		pLaser->x = pLaser->vx - CosL(pLaser->Angle, pLaser->l/2);
		pLaser->y = pLaser->vy - SinL(pLaser->Angle, pLaser->l/2);
		// カウンタをインクリメントする //
		pLaser->Count++;
	}
}


// 弾を描画する //
FVOID CEnemySLaser::Draw(void)
{
	Iterator		it;
	SLaserData		*pLaser;
	int				lmax, l;


	// テクスチャをセットする //
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);


	// 通常状態のレーザーを描画 //
	InitRList();	// リストを初期化する
	ThisForEachFront(ESLASER_NORMAL, it){
		// イテレータ２ポインタ //
		pLaser = it.GetPtr();

		l    = pLaser->l;
		lmax = pLaser->lmax;

		// 描画リストに挿入する //
		InsertRList(pLaser->x			// 始点のｘ座標
				  , pLaser->y			// 始点のｙ座標
				  , l					// レーザーの長さ
				  , pLaser->Color		// レーザーの色
				  , pLaser->Angle);		// レーザーの角度

		if(SLASER_NORM == pLaser->Type
		|| (pLaser->RefState != LRST_HIT)){
			if(lmax > l){
//				l = 18 * 256 - abs(12 * 256 * (lmax/2 - l)) / (lmax + 1);
				l = 18 * 256 - abs(8 * 256 * (lmax/2 - l)) / (lmax + 1);
				DrawSLTail(pLaser->x, pLaser->y, l, pLaser->Color);
			}
		}
	}
	DrawRListStd();	// 通常状態での描画


	// 消去中のレーザーを描画 //
	InitRList();	// リストを初期化する
	ThisForEachFront(ESLASER_DELETE, it){
		// イテレータ２ポインタ //
		pLaser = it.GetPtr();

		// 描画リストに挿入する //
		InsertRList(pLaser->x			// 始点のｘ座標
				  , pLaser->y			// 始点のｙ座標
				  , pLaser->l			// レーザーの長さ
				  , pLaser->Color		// レーザーの色
				  , pLaser->Angle		// レーザーの角度
				  , pLaser->Count);		// カウンタ
	}
	DrawRListDel();	// 消去用に描画する
}


// 当たり判定を行う //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemySLaser::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	Iterator		it;
	SLaserData		*pLaser;
	CShaveEffect	*pEfc;
	int				mx, my, i;
	int				tx, ty;
	int				width, length;
	int				awidth;
	BYTE			d;

	// 初期化は無しです(2001/09/14) //
	// ゼロ初期化する //
//	(*pShave)  = 0;		// かすった回数
//	(*pDamage) = 0;		// ダメージ量

	mx = (*m_pX);	// 自機のＸ座標
	my = (*m_pY);	// 自機のＹ座標

	// カスりエフェクト管理クラスに接続する //
	pEfc = m_pShaveEfc;


	// 通常状態のレーザーに対してだけ当たり判定を発行する //
	ThisForEachFront(ESLASER_NORMAL, it){
		pLaser = it.GetPtr();	// レーザー構造体へのポインタを取得する
		tx = mx - pLaser->x;	// ベクトルのＸ成分
		ty = my - pLaser->y;	// ベクトルのＹ成分
		d  = pLaser->Angle;		// レーザーの進行方向

		// レーザーと自機との距離を求める //
		length =  CosL(d, tx) + SinL(d, ty);	// 長さ方向
		width  = -SinL(d, tx) + CosL(d, ty);	// 幅方向
		awidth = abs(width);					// 幅の絶対値

		// カスり判定を行う //
		if((awidth < ESL_SHAVE) && (length > 0) && (length < pLaser->l) ){
			// まだ一度もカスっていないレーザーの場合 //
			if(0 == pLaser->IsShaved){
				if(width > 0) d -= 64;
				else          d += 64;

				tx = mx + CosL(d, awidth);
				ty = my + SinL(d, awidth);

				pLaser->IsShaved = 0xff;

				for(i=0; i<ESL_SHAVEWEIGHT; i++){
					pEfc->Set(tx, ty, d);
					(*pShave)++;
				}
			}

			// 当たり判定を行う //
			if(awidth < ESL_WIDTH){
				(*pDamage) += ESL_DAMAGE;

				SetClearLaserState(it);
				MoveBack(it, ESLASER_DELETE);
			}
		}
	}
}


// レーザー１本を消去する //
IVOID CEnemySLaser::SetClearLaserState(Iterator &it)
{
	it->Count = 0;
	it->vx = it->x + CosL(it->Angle, it->l / 2);
	it->vy = it->y + SinL(it->Angle, it->l / 2);
}


// 難易度 Easy 用にセット //
IVOID CEnemySLaser::EasyCommand(AttackCommand *pCmd)
{
}


// 難易度 Hard 用にセット //
IVOID CEnemySLaser::HardCommand(AttackCommand *pCmd)
{
}


// 難易度 Luna 用にセット //
IVOID CEnemySLaser::LunaCommand(AttackCommand *pCmd)
{
}


// 通常のレーザー動作関数(移動処理) //
IVOID CEnemySLaser::NormLaserMove(SLaserData *pLaser)
{
	int				Length, LMax;

	// まずは、長さをローカル変数に格納 //
	Length = pLaser->l;			// 現在の長さ
	LMax   = pLaser->lmax;		// 目標とする長さ

	// 目標とする長さに達していない場合 //
	if(Length < LMax){
		// 最終的な長さを越えないようにレーザーを伸ばす //
		pLaser->l = min(LMax, Length + pLaser->v);
	}
	// 必要な長さになったので、始点の移動を始める //
	else{
		pLaser->x += pLaser->vx;	// ｘ方向の移動
		pLaser->y += pLaser->vy;	// ｙ方向の移動
	}
}


// 移動処理 //
IBOOL CEnemySLaser::RefLaserMove(SLaserData *pLaser)
{
	int				Length, LMax;

	switch(pLaser->RefState){
	case LRST_NONE:	// [E-->  R] : 通常の移動中
		pLaser->x += pLaser->vx;
		pLaser->y += pLaser->vy;

		if(RefLaserHitCheck(pLaser)){
			pLaser->RefState = LRST_HIT;
		}
	break;

	case LRST_SHOT:	// [E --> R] : 発射中
		Length = pLaser->l;			// 現在の長さ
		LMax   = pLaser->lmax;		// 目標とする長さ

		pLaser->l += pLaser->v;
		if(pLaser->l >= pLaser->lmax){
			pLaser->l        = LMax;
			pLaser->RefState = LRST_NONE;
		}

		if(RefLaserHitCheck(pLaser)){
			pLaser->lwait     = pLaser->l;
			pLaser->RefState |= LRST_HIT;
		}
	break;

	case LRST_HIT:	// [E  -->R] : ヒット中
		if(pLaser->l <= pLaser->v){
			return FALSE;	// 消去要求
		}

		pLaser->l -= pLaser->v;
		pLaser->x += pLaser->vx;
		pLaser->y += pLaser->vy;
	break;

	case LRST_NMOVE:// [E---->R] : 反射＆ヒット中
		pLaser->lwait += pLaser->v;
		if((pLaser->lwait) >= (pLaser->lmax)){
			pLaser->RefState = LRST_HIT;
		}
	break;
	}

	return TRUE;
}


// リフレクターとの当たり判定 //
IBOOL CEnemySLaser::RefLaserHitCheck(SLaserData *pLaser)
{
	int					lx, ly;
	BYTE				d, dret, c;
	AttackCommand		AtkCmd;


	// レーザー先端の座標を求める //
	lx = pLaser->x + CosL(pLaser->Angle, pLaser->l);	// 先端のＸ座標
	ly = pLaser->y + SinL(pLaser->Angle, pLaser->l);	// 先端のＹ座標


	// ふとれざ反射タイプ //
	if(pLaser->Type == SLASER_LLREF){
		// この間では反射を許さない //
		if(pLaser->Count < 2) return FALSE;

		d = pLaser->Angle;

		dret = m_pLLaser->HitCheckEx(lx, ly, d, pLaser->Color);
		if(d == dret) return FALSE;		// ヒットしていない

		AtkCmd.Type   = SLASER_LLREF;
		AtkCmd.Cmd    = LCMD_WAY | LCMD_NODIF;
		AtkCmd.ox     = lx;
		AtkCmd.oy     = ly;
		AtkCmd.Speed  = pLaser->v;
		AtkCmd.Angle  = dret;
		AtkCmd.Length = pLaser->lmax;
		AtkCmd.Color  = pLaser->Color;
		AtkCmd.Rep    = pLaser->Rep;
		AtkCmd.Num    = 1;
		Set(AtkCmd);
	}
	// 自機追従タイプ //
	else if(pLaser->Type == SLASER_SEARCH){
		// Ｘ壁反射を行う //
		if(lx <= m_XMin){		// 左の壁にヒット
			lx = m_XMin + 6 * 256;
			if(ly > (*m_pY)) d = -64;
			else             d =  64;
		}
		else if(lx >= m_XMax){	// 右の壁にヒット
			lx = m_XMax - 6 * 256;
			if(ly > (*m_pY)) d = -64;
			else             d =  64;
		}
		else if(abs(ly - (*m_pY)) < 6 * 256){
			d = pLaser->Angle;
			if(!(d == 64 || d == 256-64)) return FALSE;

			if(lx < (m_XMax + m_XMin)/2) d = 0;
			else                         d = 128;
		}
		else{	// 壁にＨＩＴしていない場合
			return FALSE;
		}

		if(pLaser->Rep){	// 反射回数が残っている場合
			AtkCmd.Type   = SLASER_SEARCH;	//SLASER_REF;
			AtkCmd.Cmd    = LCMD_WAY | LCMD_NODIF;
			AtkCmd.ox     = lx;
			AtkCmd.oy     = ly;
			AtkCmd.Speed  = pLaser->v;
			AtkCmd.Angle  = d;
			AtkCmd.Length = pLaser->lmax;
			AtkCmd.Color  = pLaser->Color;
			AtkCmd.Rep    = pLaser->Rep - 1;
			AtkCmd.Num    = 1;
			Set(AtkCmd);
		}
	}
	// 壁反射タイプ //
	else{
		// Ｘ壁反射を行う //
		if(lx <= m_XMin){		// 左の壁にヒット
			lx = m_XMin;
		}
		else if(lx >= m_XMax){	// 右の壁にヒット
			lx = m_XMax;
		}
		else{	// 壁にＨＩＴしていない場合
			return FALSE;
		}

		if(pLaser->Rep){	// 反射回数が残っている場合
			AtkCmd.Type   = SLASER_REF;
			AtkCmd.Cmd    = LCMD_WAY | LCMD_NODIF;
			AtkCmd.ox     = lx;
			AtkCmd.oy     = ly;
			AtkCmd.Speed  = pLaser->v;
			AtkCmd.Angle  = 128 - pLaser->Angle;
			AtkCmd.Length = pLaser->lmax;
			AtkCmd.Color  = pLaser->Color;
			AtkCmd.Rep    = pLaser->Rep - 1;
			AtkCmd.Num    = 1;
			Set(AtkCmd);
		}
	}

	// ヒットしていた場合 //
	return TRUE;
}


// 扇状発射 //
char CEnemySLaser::LaserAngleWay(AttackCommand *pCmd, int i)
{
	int			temp;

	i    = i + 1;
	temp = (i >> 1) * (pCmd->wAngle) * (1 - ((i & 1) << 1));

	if((pCmd->Num) & 1) return temp;						// 奇数方向弾の場合
	else                return temp - (pCmd->wAngle >> 1);	// 偶数方向弾の場合
}


// 全方向発射 //
char CEnemySLaser::LaserAngleAll(AttackCommand *pCmd, int i)
{
	// (256 * i) / n  //
	return (i << 8) / (pCmd->Num);
}


// ランダム方向発射 //
char CEnemySLaser::LaserAngleRnd(AttackCommand *pCmd, int i)
{
#ifdef PBG_DEBUG
	if(0 == pCmd->wAngle){
		PbgError("CEnemySLaser::LaserAngleRnd() : wAngle がゼロじゃ困るのね");
		return 0;
	}
#endif

	return RndEx() % (pCmd->wAngle) - (pCmd->wAngle >> 1);
}


// 一方向発射 //
char CEnemySLaser::LaserAngleOne(AttackCommand *pCmd, int i)
{
	return 0;
}


// 速度セット(標準) //
int CEnemySLaser::LaserSpeedStd(AttackCommand *pCmd)
{
	return pCmd->Speed;
}


// 速度セット(ランダム) //
int CEnemySLaser::LaserSpeedRnd(AttackCommand *pCmd)
{
	int		temp;

	// この部分は、十分な微調整が必要である //
	temp = (pCmd->Speed >> 4) + 1;						// 速度 / 16 + 1
	temp = pCmd->Speed + (RndEx() % temp) - (temp >> 1);	// 速度 ± (temp/2)

	return temp;
}


// 頂点バッファとテクスチャ座標を初期化する //
// ちなみに VERTEX BUFFER の事では無い      //
void CEnemySLaser::InitializeBuffer(void)
{
	DWORD		i;


	// すでに初期化されている場合 //
	if(TRUE == m_BufferInitialized) return;

	// ここで重要なのでは、実際に描画を行うときに代入する回数を減らすことが //
	// 出来ることである。すなわち、描画時には、(x, y, u, v) の代入だけを    //
	// 行えば良いことになる                                                 //
	for(i=0; i<ESLASER_BUFFERSIZE; i++){
		Set2DPointC(m_RenderList+i, 0, 0, 0, 0, RGBA_MAKE(255,255,255,150));
	}


	m_BufferInitialized = TRUE;
}


// 頂点データに格納する準備する //
IVOID CEnemySLaser::InitRList(void)
{
	// 挿入先を初期化する //
	m_pRLTarget = m_RenderList;
}


// 現在の描画リストで描画する(通常) //
IVOID CEnemySLaser::DrawRListStd(void)
{
	int			n;

	// ポリゴン数を求める //
	n = m_pRLTarget - m_RenderList;
	if(0 == n) return;				// レーザー無い

	// １：１加算半透明 //
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);
}


// 現在の描画リストで描画する(消去) //
IVOID CEnemySLaser::DrawRListDel(void)
{
	int			n;

	// ポリゴン数を求める //
	n = m_pRLTarget - m_RenderList;
	if(0 == n) return;				// レーザー無い

	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);
}


// 尻尾のまる球体を描画する //
IVOID CEnemySLaser::DrawSLTail(int x, int y, int w, BYTE c)
{
	D3DTLVERTEX		tlv[20];
	const DWORD		col = RGBA_MAKE(255, 255, 255, 255);
	D3DVALUE		u0, v0, u1, v1;
	int				lc;

	lc = (c & 3) * (64+8);
	u0 = D3DVAL(   136) / D3DVAL(256);
	v0 = D3DVAL(lc+  8) / D3DVAL(256);
	u1 = D3DVAL(   200) / D3DVAL(256);
	v1 = D3DVAL(lc+ 72) / D3DVAL(256);

	x = x >> 8;
	y = y >> 8;
	w = w >> 8;

	Set2DPointC(tlv+0, x-w, y-w, u0, v0, col);
	Set2DPointC(tlv+1, x+w, y-w, u1, v0, col);
	Set2DPointC(tlv+2, x+w, y+w, u1, v1, col);
	Set2DPointC(tlv+3, x-w, y+w, u0, v1, col);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
}


// 頂点バッファに弾データを追加する                  //
// 引数  ox, oy : 弾の中心座標                       //
//        l256  : レーザーの長さ(x256)               //
//          c   : レーザーの色                       //
//          d   : レーザーの角度                     //
IVOID CEnemySLaser::InsertRList(int ox, int oy, int l256, BYTE c, BYTE d)
{
	int			lx, ly;
	int			wx, wy;
	int			x, y;
	D3DVALUE	u0, v0, u1, v1;

	// まずは、ｕｖ座標を確定する //
	u0 = D3DVAL(96   )  / D3DVAL(256);
	u1 = D3DVAL(128-2) / D3DVAL(256);

	c  = 8 + (c & 3) * 24;
	v0 = D3DVAL(c)    / D3DVAL(256);
	v1 = D3DVAL(c+16) / D3DVAL(256);

#define ESL_WIDTH			(2*256)

	lx = CosL(d, l256);		wx = CosL(d-64, ESL_WIDTH);
	ly = SinL(d, l256);		wy = SinL(d-64, ESL_WIDTH);

	x = (ox + wx) >> 8;
	y = (oy + wy) >> 8;
	SET2DP_XYUV(m_pRLTarget+0, x, y, u0, v0);
	SET2DP_XYUV(m_pRLTarget+5, x, y, u0, v0);

	x = (ox - wx) >> 8;
	y = (oy - wy) >> 8;
	SET2DP_XYUV(m_pRLTarget+4, x, y, u0, v1);

	x = (ox + wx + lx) >> 8;
	y = (oy + wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+7, x, y, u0, v0);

	x = (ox - wx + lx) >> 8;
	y = (oy - wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+8, x, y, u0, v1);
	SET2DP_XYUV(m_pRLTarget+9, x, y, u0, v1);

	lx >>= 1;
	ly >>= 1;

	x = (ox + wx + lx) >> 8;
	y = (oy + wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+ 1, x, y, u1, v0);
	SET2DP_XYUV(m_pRLTarget+ 6, x, y, u1, v0);
	SET2DP_XYUV(m_pRLTarget+11, x, y, u1, v0);

	x = (ox - wx + lx) >> 8;
	y = (oy - wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+ 2, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+ 3, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+10, x, y, u1, v1);

	m_pRLTarget += 12;
}


IVOID CEnemySLaser::InsertRList(int ox, int oy, int l256, BYTE c, BYTE d, DWORD Count)
{
	int			lx, ly;
	int			wx, wy;
	int			x, y;
	D3DVALUE	u0, v0, u1, v1;

	// まずは、ｕｖ座標を確定する //
	u0 = D3DVAL(96   )  / D3DVAL(256);
	u1 = D3DVAL(128-2) / D3DVAL(256);

	c  = 8 + (c & 3) * 24;
	v0 = D3DVAL(c)    / D3DVAL(256);
	v1 = D3DVAL(c+16) / D3DVAL(256);

	lx = CosL(d, l256);		wx = CosL(d-64, max(0, ESL_WIDTH-(Count<<4)));
	ly = SinL(d, l256);		wy = SinL(d-64, max(0, ESL_WIDTH-(Count<<4)));

	x = (ox + wx) >> 8;
	y = (oy + wy) >> 8;
	SET2DP_XYUV(m_pRLTarget+0, x, y, u0, v0);
	SET2DP_XYUV(m_pRLTarget+5, x, y, u0, v0);

	x = (ox - wx) >> 8;
	y = (oy - wy) >> 8;
	SET2DP_XYUV(m_pRLTarget+4, x, y, u0, v1);

	x = (ox + wx + lx) >> 8;
	y = (oy + wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+7, x, y, u0, v0);

	x = (ox - wx + lx) >> 8;
	y = (oy - wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+8, x, y, u0, v1);
	SET2DP_XYUV(m_pRLTarget+9, x, y, u0, v1);

	lx >>= 1;
	ly >>= 1;

	x = (ox + wx + lx) >> 8;
	y = (oy + wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+ 1, x, y, u1, v0);
	SET2DP_XYUV(m_pRLTarget+ 6, x, y, u1, v0);
	SET2DP_XYUV(m_pRLTarget+11, x, y, u1, v0);

	x = (ox - wx + lx) >> 8;
	y = (oy - wy + ly) >> 8;
	SET2DP_XYUV(m_pRLTarget+ 2, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+ 3, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+10, x, y, u1, v1);

	m_pRLTarget += 12;
}
