/*
 *   CEnemyTama.cpp   : 敵弾処理
 *
 */

#include "CEnemyTama.h"
#include "TamaSize.h"
#include "PbgError.h"
#include "FontDraw.h"

#include "RndCtrl.h"



/***** [プライベートな定数] *****/

// ダメージ定数 //
#define ETSIZE8_DAMAGE		(12345*5)	//  ８ × ８ 弾のダメージ
#define ETSIZE16_DAMAGE		(23456*5)	// １６×１６弾のダメージ
#define ETSIZE24_DAMAGE		(34567*5)	// ２４×２４弾のダメージ
#define ETSIZE32_DAMAGE		(47890*5)	// ３２×３２弾のダメージ



/***** [スタティックメンバ] *****/

// 描画用バッファ //
D3DTLVERTEX CEnemyTama::m_RenderList[ENEMYTAMA_MAX * 6];

D3DTLVERTEX *CEnemyTama::m_pRLTarget;	// 現在の頂点データ挿入先
int			 CEnemyTama::m_RLSize;		// 現在の追加ポリゴンのサイズ

// テクスチャ座標の格納先 //
D3DRECTANGLE CEnemyTama::m_TextureUV[256];

// 敵弾バッファが初期化されると TRUE になる //
BOOL CEnemyTama::m_BufferInitialized = FALSE;



// コンストラクタ //
CEnemyTama
	::CEnemyTama(RECT			*rcTargetX256	// 対象矩形
			   , int			*pX				// 当たり判定Ｘ座標へのポインタ
			   , int			*pY				// 当たり判定Ｙ座標へのポインタ
			   , CShaveEffect	*pShaveEfc)	// カスりエフェクト発動用クラス
{
	// 反射座標 //
	m_RefXMin = rcTargetX256->left;		// 左端の反射座標
	m_RefYMin = rcTargetX256->top;		// 上端の反射座標
	m_RefXMax = rcTargetX256->right;	// 右端の反射座標
	m_RefYMax = rcTargetX256->bottom;	// 下端の反射座標

	// 消去座標(１２８倍しているのは、２５６倍して２で割っているため) //
	m_XMin = m_RefXMin - (ENEMYTAMA_MAXSIZE * 128);	// 左端の消去座標
	m_YMin = m_RefYMin - (ENEMYTAMA_MAXSIZE * 128);	// 上端の消去座標
	m_XMax = m_RefXMax + (ENEMYTAMA_MAXSIZE * 128);	// 右端の消去座標
	m_YMax = m_RefYMax + (ENEMYTAMA_MAXSIZE * 128);	// 下端の消去座標

	// ポインタの接続 //
	m_pX        = pX;			// 当たり判定Ｘ座標
	m_pY        = pY;			// 当たり判定Ｙ座標
	m_pShaveEfc = pShaveEfc;	// カスりエフェクト管理

	// バッファの初期化を行う                       //
	// すでに初期化が完了している場合は何も行わない //
	InitializeBuffer();
}


// デストラクタ //
CEnemyTama::~CEnemyTama()
{
}


// 敵弾をセットする(引数がポインタでないことに注意) //
FVOID CEnemyTama::Set(AttackCommand Command)
{
	// 関数ポインタ //
	BYTE (*pGetAngle)(AttackCommand *pCmd, int i);	// 角度取得用のポインタ
	int  (*pGetSpeed)(AttackCommand *pCmd, int n);	// 速度取得用のポインタ

	TamaData	*pTama;			// データ書き込み用のポインタ

	DWORD		TargetList;		// 対象となるリストＩＤ
	DWORD		i;				// ループ用
	DWORD		NumTama;		// セットしたい弾数
	DWORD		n, ns;			// 弾数・連弾数

	BYTE		Size;			// 敵弾のサイズ
	BYTE		Angle;			// 角度の一時格納用
	int			Speed;			// 速度の一時格納用
	BYTE		AShift;			// 角度のシフト値
	char		NdAngle;		// 角度ずらしの単位


	// エラーログ用の関数名をセット //
#ifdef PBG_DEBUG
	PbgErrorInit("CEnemyTama::Set()");
#endif

	// 難易度変化の無効化ビットが立っていない場合 //
	if( !(Command.Cmd & TCMD_NODIF) ){
		//EasyCommand(&Command);
		//HardCommand(&Command);
		//LunaCommand(&Command);
	}


	// 発射方向別に、関数を振り分ける                            //
	// 0xff00 と論理積を取っているのは、上位８ビットが発射形状を //
	// 確定するためのビットとなっているためである                //
	switch(Command.Cmd & 0xff00){
		case TCMD_WAY:	pGetAngle = TamaAngleWay;	break;	// 扇状発射
		case TCMD_ALL:	pGetAngle = TamaAngleAll;	break;	// 全方向発射
		case TCMD_RND:	pGetAngle = TamaAngleRnd;	break;	// ランダム発射

		case TCMD_ONE:	// １方向発射
			pGetAngle   = TamaAngleOne;		// ０を返す関数
			Command.Num = 1;
		break;

		default:
		#ifdef PBG_DEBUG
			PbgErrorEx("Command.Cmd : 発射形状ビットが不正ね♪");
		#endif
		return;
	}

	// 弾の速度セット用の関数をセットする //
	// 参考：速度ランダムビットで判定する //
	if(Command.Cmd & TCMD_RNDSPD) pGetSpeed = TamaSpeedRnd;	// ランダムセット時
	else                          pGetSpeed = TamaSpeedStd;	// 通常時

	// 連弾時の [ずらす単位] を求める //
	if(Command.Cmd & TCMD_SHIFT) NdAngle = Command.NdAngle;	// ビットが有効
	else                         NdAngle = 0;				// ビットが無効

	// 敵弾の色から、サイズと格納先を求める //
	// 参考：格納先は上位４ビットで示される //
	Size       = TamaSize(Command.Color);	// 敵弾のサイズを求める
	TargetList = Command.Color >> 4;		// 挿入対象となるリストはどこかな

#ifdef PBG_DEBUG
	if(TargetList > ENEMYTAMA_KIND){
		PbgErrorEx("Command.Color : 形状指定のＩＤが大きすぎますよぉ");
		return;
	}
#endif

	// 結局、角度方向の弾数×速度方向の弾数だけセットする必要がある //
	// 連弾フラグが立っていない場合は、強制的に無効化する           //
	if(Command.Cmd & TCMD_NUM) NumTama = Command.Num * Command.NumS;// 連弾時
	else                       NumTama = Command.Num;				// 通常時

	n = ns = 0;

	// AShift は、角度変数と自機セット反映に用いられる          //
	// 前Version と異なり、ここで基本角もセットしている点に注意 //
	if(Command.Cmd & TCMD_SET){
		// 角度セット属性ビットが立っている場合 //
		AShift = Atan8((*m_pX) - Command.ox, (*m_pY) - Command.oy) + Command.Angle;
	}
	else{
		// 角度セットビットが立っていない場合 //
		AShift = Command.Angle;
	}

	for(i=0; i<NumTama; i++){
		// 基本的に線形リストテンプレートにお任せする //
		pTama = InsertBack(TargetList);		// 挿入して、ポインタを取得
		if(NULL == pTama) return;			// 空きがありません

		// まずは、セットする弾で共通するデータを代入する //
		pTama->x         = Command.ox;		// 初期Ｘ座標
		pTama->y         = Command.oy;		// 初期Ｙ座標
		pTama->Color     = Command.Color;	// 外見の指定(形状 | 色)
		pTama->Rep       = Command.Rep;		// 繰り返し回数の指定
		pTama->Type      = Command.Type;	// 弾の種類(通常、加速など)
		pTama->Option    = Command.Option;	// 弾の属性(反射など)
		pTama->vAngle    = Command.vAngle;	// 弾の角速度
		pTama->a         = Command.Accel;	// 弾の加速度
		pTama->AnmSpd    = Command.AnmSpd;	// アニメーションスピード
		pTama->Size      = Size;			// 弾の大きさ(描画・当たり判定用)
		pTama->Anime     = 0;				// 現在のアニメーション角度
		pTama->Count     = 0;				// 発生から何フレームが経過したか
		pTama->IsShaved  = 0;				// すでにカスった弾かどうか
		pTama->Score     = 0;				// 得点変化用
		pTama->IsClipped = FALSE;			// クリッピングされているか

		// 初速度、初期角を求める                      //
		// [ 初期角 ] にはシフト値が反映される点に注意 //
		Speed = pGetSpeed(&Command, ns);			// 初速度を求める
		Angle = pGetAngle(&Command, n) + AShift;	// 初期角を求める
		pTama->Angle         = Angle;				// 実際に角度を代入
		pTama->v = pTama->v0 = Speed;				// 速度も代入

		// 速度のＸ＆Ｙ成分を求めましょ //
		pTama->vx = CosL(Angle, Speed);		// 速度のＸ成分
		pTama->vy = SinL(Angle, Speed);		// 速度のＹ成分


		// インクリメントして(角度方向、何番目の弾か) //
		n = n + 1;

		if(n >= Command.Num){	// 角度方向数を越えていたら
			ns      = ns + 1;	// 連弾方向にインクリメント
			n       = 0;		// 角度方向はリセット
			AShift += NdAngle;	// 角度ずらし
		}
	}
}


// 初期化する //
FVOID CEnemyTama::Initialize(void)
{
	// リスト内のデータを解放する //
	DeleteAllData();
}


// 敵弾全てに消去エフェクトをセットする //
__int64 CEnemyTama::Clear(BOOL bChgScore)
{
	Iterator			it;
	__int64				score;

	score = 0;

	if(bChgScore){
		ThisForEachFront(ETAMA_NORMAL8 >>4, it){it->Score = 1;	score+=100;}
		ThisForEachFront(ETAMA_NORMAL16>>4, it){it->Score = 2;	score+=200;}
		ThisForEachFront(ETAMA_NORMAL24>>4, it){it->Score = 3;	score+=300;}
		ThisForEachFront(ETAMA_NORMAL32>>4, it){it->Score = 4;	score+=400;}

		ThisForEachFront(ETAMA_ANGLE8 >>4, it){it->Score = 1;	score+=100;}
		ThisForEachFront(ETAMA_ANGLE16>>4, it){it->Score = 2;	score+=200;}
		ThisForEachFront(ETAMA_ANGLE24>>4, it){it->Score = 3;	score+=300;}
		ThisForEachFront(ETAMA_ANGLE32>>4, it){it->Score = 4;	score+=400;}

		ThisForEachFront(ETAMA_ANIME8 >>4, it){it->Score = 1;	score+=100;}
		ThisForEachFront(ETAMA_ANIME16>>4, it){it->Score = 2;	score+=200;}
		ThisForEachFront(ETAMA_ANIME24>>4, it){it->Score = 3;	score+=300;}
		ThisForEachFront(ETAMA_ANIME32>>4, it){it->Score = 4;	score+=400;}
	}

	// カウンタをゼロ初期化する //
	ThisForEachFront(ETAMA_NORMAL8 >>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_NORMAL16>>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_NORMAL24>>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_NORMAL32>>4, it)	it->Count = 0;

	ThisForEachFront(ETAMA_ANGLE8 >>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_ANGLE16>>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_ANGLE24>>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_ANGLE32>>4, it)	it->Count = 0;

	ThisForEachFront(ETAMA_ANIME8 >>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_ANIME16>>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_ANIME24>>4, it)	it->Count = 0;
	ThisForEachFront(ETAMA_ANIME32>>4, it)	it->Count = 0;


	// データの移動先は終端 //

	// まる弾 //
	MoveBackAll(ETAMA_DEL_NORMAL>>4, ETAMA_NORMAL8 >>4);	// まる弾８を消去
	MoveBackAll(ETAMA_DEL_NORMAL>>4, ETAMA_NORMAL16>>4);	// まる弾16を消去
	MoveBackAll(ETAMA_DEL_NORMAL>>4, ETAMA_NORMAL24>>4);	// まる弾24を消去
	MoveBackAll(ETAMA_DEL_NORMAL>>4, ETAMA_NORMAL32>>4);	// まる弾32を消去

	// 角度同期 //
	MoveBackAll(ETAMA_DEL_ANGLE>>4, ETAMA_ANGLE8 >>4);	// 角度同期８を消去
	MoveBackAll(ETAMA_DEL_ANGLE>>4, ETAMA_ANGLE16>>4);	// 角度同期16を消去
	MoveBackAll(ETAMA_DEL_ANGLE>>4, ETAMA_ANGLE24>>4);	// 角度同期24を消去
	MoveBackAll(ETAMA_DEL_ANGLE>>4, ETAMA_ANGLE32>>4);	// 角度同期32を消去

	// アニメーション //
	MoveBackAll(ETAMA_DEL_ANIME>>4, ETAMA_ANIME8 >>4);	// アニメ８を消去
	MoveBackAll(ETAMA_DEL_ANIME>>4, ETAMA_ANIME16>>4);	// アニメ16を消去
	MoveBackAll(ETAMA_DEL_ANIME>>4, ETAMA_ANIME24>>4);	// アニメ24を消去
	MoveBackAll(ETAMA_DEL_ANIME>>4, ETAMA_ANIME32>>4);	// アニメ32を消去

	return score;
}


// 弾を移動させる //
FVOID CEnemyTama::Move(void)
{
	Iterator			it;
	TamaData			*pTama;
	DWORD				i;
	int					mx, my;
	int					XMin, YMin, XMax, YMax;

	// メンバ変数 -> ローカル変数に変換して、高速化を図る //
	XMin = m_XMin;		// 左端の座標
	YMin = m_YMin;		// 上端の座標
	XMax = m_XMax;		// 右端の座標
	YMax = m_YMax;		// 下端の座標

/////////////////////////////////////////////////////////////////////////////
			//                     重要                         //
			// エフェクト動作の奴は、後で処理を変更すること！！ //
/////////////////////////////////////////////////////////////////////////////

	ThisForEachFront(ETAMA_DEL_NORMAL >>4, it){
		if(it->Count >= 31){
			if(it->Score){
				it->Count = 0;
				MoveBack(it, ETAMA_SCORE>>4);
			}
			else{
				ThisDelContinue(it);
			}
		}
	}

	ThisForEachFront(ETAMA_DEL_ANGLE >>4, it){
		if(it->Count >= 31){
			if(it->Score){
				it->Count = 0;
				MoveBack(it, ETAMA_SCORE>>4);
			}
			else{
				ThisDelContinue(it);
			}
		}
	}

	ThisForEachFront(ETAMA_DEL_ANIME >>4, it){
		if(it->Count >= 31){
			if(it->Score){
				it->Count = 0;
				MoveBack(it, ETAMA_SCORE>>4);
			}
			else{
				ThisDelContinue(it);
			}
		}
	}

	// 得点状態 //
	ThisForEachFront(ETAMA_SCORE>>4, it){
		pTama = it.GetPtr();

		if(pTama->Count >= 63){
			ThisDelContinue(it);
		}
		else{
			if(pTama->Count < 256/6){
				pTama->y += SinL(-(int)pTama->Count*6, 256*4);
			}
			pTama->Count++;
		}
	}

	// 得点状態は除く //
	for(i=0; i<ENEMYTAMA_KIND-1; i++){
		ThisForEachFront(i, it){
			pTama = it.GetPtr();

			MoveDefault(pTama);		// 通常の移動
			MoveOption(pTama);		// オプションによる移動

			pTama->Count++;					// カウンタをインクリメントする
			pTama->Anime += pTama->AnmSpd;	// アニメーションの更新

			// クリッピング属性が付いている場合はココマデ //
			if(pTama->IsClipped) continue;
			//if(pTama->Option & TOPT_CLIP) continue;

			// 座標を取得する //
			mx = pTama->x;		// 弾のＸ座標
			my = pTama->y;		// 弾のＹ座標

			// 範囲外ならば、削除する //
			if(mx < XMin || mx > XMax || my < YMin  || my > YMax){
				ThisDelContinue(it);
			}
		}
	}
}


// 弾を描画する //
FVOID CEnemyTama::Draw(void)
{
	if(NULL == g_pGrp){
#ifdef PBG_DEBUG
		PbgError("CEnemyTama::Draw() : グラフィックが使用できる状況にありませんぜ");
		return;
#endif
	}

	// テクスチャをセットする //
	g_pGrp->SetTexture(TEXTURE_ID_TAMA);

	// 消去エフェクトの弾を描画する //
	// 描画優先順位が一番低い       //
g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	DrawDelNormal();	// まる弾
	DrawDelAngle();		// 角度同期弾
	DrawDelAnime();		// アニメ弾
g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);

	// 以下は、大きい順に弾を描画するもの                       //
	// つまり、小さい弾が大きい弾に隠されないようにする         //
	// ちなみにサイズ/２ を行っているのは、中心から各頂点までの //
	// 距離は、一辺の半分になるためである                       //
	// ４ビット右シフトしているのは、上位バイトを取り出す為     //

	// サイズ３２の弾を描画する //
	DrawNormal(ETAMA_NORMAL32>>4,  16);		// まる弾
	DrawAngle( ETAMA_ANGLE32 >>4,  23);		// 角度同期弾(x sqrt(2))
	DrawAnime( ETAMA_ANIME32 >>4,  23);		// アニメ弾  (x sqrt(2))

	// サイズ２４の弾を描画する //
	DrawNormal(ETAMA_NORMAL24>>4, 12);		// まる弾
	DrawAngle( ETAMA_ANGLE24 >>4, 17);		// 角度同期弾(x sqrt(2))
	DrawAnime( ETAMA_ANIME24 >>4, 17);		// アニメ弾  (x sqrt(2))

	// サイズ１６の弾を描画する //
	DrawNormal(ETAMA_NORMAL16>>4, 8);	// まる弾
	DrawAngle( ETAMA_ANGLE16 >>4, 11);	// 角度同期弾(x sqrt(2))
	DrawAnime( ETAMA_ANIME16 >>4, 11);	// アニメ弾  (x sqrt(2))

	// サイズ８の弾を描画する //
	DrawNormal(ETAMA_NORMAL8>>4, 4);	// まる弾
	DrawAngle( ETAMA_ANGLE8 >>4, 6);	// 角度同期弾(x sqrt(2))
	DrawAnime( ETAMA_ANIME8 >>4, 6);	// アニメ弾  (x sqrt(2))


	char *pScoreTable[5] = {
		"だみぃ", "100", "200", "300", "400"
	};

	Iterator		it;
	TamaData		*pTama;

	ThisForEachFront(ETAMA_SCORE>>4, it){
		pTama = it.GetPtr();
		g_Font.DrawSmallNumber8((pTama->x>>8), pTama->y>>8
						, pScoreTable[pTama->Score], pTama->Count<<2);
	}
}


// 当たり判定を行う //
// pShave  : カスった回数の格納先 //
// pDamage : ダメージ総量の格納先 //
FVOID CEnemyTama::HitCheck(DWORD *pShave, DWORD *pDamage)
{
	// 初期化は無しです(2001/09/14) //
//	(*pShave)  = 0;	// カスった回数
//	(*pDamage) = 0;	// ダメージ合計

	// ８×８ //
	CheckETHit(ETAMA_NORMAL8>>4, ETAMA_DEL_NORMAL>>4, ETSIZE8_DAMAGE, ETAMA8_HITSIZE, pShave, pDamage);	// まる弾
	CheckETHit(ETAMA_ANGLE8 >>4, ETAMA_DEL_ANGLE >>4, ETSIZE8_DAMAGE, ETAMA8_HITSIZE, pShave, pDamage);	// 角度同期
	CheckETHit(ETAMA_ANIME8 >>4, ETAMA_DEL_ANIME >>4, ETSIZE8_DAMAGE, ETAMA8_HITSIZE, pShave, pDamage);	// アニメ

	// １６×１６ //
	CheckETHit(ETAMA_NORMAL16>>4, ETAMA_DEL_NORMAL>>4, ETSIZE16_DAMAGE, ETAMA16_HITSIZE, pShave, pDamage);// まる弾
	CheckETHit(ETAMA_ANGLE16 >>4, ETAMA_DEL_ANGLE >>4, ETSIZE16_DAMAGE, ETAMA16_HITSIZE, pShave, pDamage);// 角度同期
	CheckETHit(ETAMA_ANIME16 >>4, ETAMA_DEL_ANIME >>4, ETSIZE16_DAMAGE, ETAMA16_HITSIZE, pShave, pDamage);// アニメ

	// ２４×２４ //
	CheckETHit(ETAMA_NORMAL24>>4, ETAMA_DEL_NORMAL>>4, ETSIZE24_DAMAGE, ETAMA24_HITSIZE, pShave, pDamage);// まる弾
	CheckETHit(ETAMA_ANGLE24 >>4, ETAMA_DEL_ANGLE >>4, ETSIZE24_DAMAGE, ETAMA24_HITSIZE, pShave, pDamage);// 角度同期
	CheckETHit(ETAMA_ANIME24 >>4, ETAMA_DEL_ANIME >>4, ETSIZE24_DAMAGE, ETAMA24_HITSIZE, pShave, pDamage);// アニメ

	// ３２×３２ //
	CheckETHit(ETAMA_NORMAL32>>4, ETAMA_DEL_NORMAL>>4, ETSIZE32_DAMAGE, ETAMA32_HITSIZE, pShave, pDamage);// まる弾
	CheckETHit(ETAMA_ANGLE32 >>4, ETAMA_DEL_ANGLE >>4, ETSIZE32_DAMAGE, ETAMA32_HITSIZE, pShave, pDamage);// 角度同期
	CheckETHit(ETAMA_ANIME32 >>4, ETAMA_DEL_ANIME >>4, ETSIZE32_DAMAGE, ETAMA32_HITSIZE, pShave, pDamage);// アニメ
}


// 弾消しボックスとの当たり判定を行う //
// ox, oy : 中心座標                  //
// width  : 幅                        //
// height : 高さ                      //
FVOID CEnemyTama::ClearFromRect(int ox, int oy, int width, int height)
{
	// ８×８ //
	ETClearRect(ETAMA_NORMAL8>>4, ETAMA_DEL_NORMAL>>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANGLE8 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANIME8 >>4, ETAMA_DEL_ANIME >>4, ox, oy, width, height);

	// １６×１６ //
	ETClearRect(ETAMA_NORMAL16>>4, ETAMA_DEL_NORMAL>>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANGLE16 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANIME16 >>4, ETAMA_DEL_ANIME >>4, ox, oy, width, height);

	// ２４×２４ //
	ETClearRect(ETAMA_NORMAL24>>4, ETAMA_DEL_NORMAL>>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANGLE24 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANIME24 >>4, ETAMA_DEL_ANIME >>4, ox, oy, width, height);

	// ３２×３２ //
	ETClearRect(ETAMA_NORMAL32>>4, ETAMA_DEL_NORMAL>>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANGLE32 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, width, height);
	ETClearRect(ETAMA_ANIME32 >>4, ETAMA_DEL_ANIME >>4, ox, oy, width, height);
}


// 弾消し８角形との当たり判定を行う        //
// ox, oy : 中心座標                       //
// r      : ８角形を含む正方形の一辺の長さ //
FVOID CEnemyTama::ClearFromOct(int ox, int oy, int r)
{
	// ８×８ //
	ETClearOct(ETAMA_NORMAL8>>4, ETAMA_DEL_NORMAL>>4, ox, oy, r);
	ETClearOct(ETAMA_ANGLE8 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, r);
	ETClearOct(ETAMA_ANIME8 >>4, ETAMA_DEL_ANIME >>4, ox, oy, r);

	// １６×１６ //
	ETClearOct(ETAMA_NORMAL16>>4, ETAMA_DEL_NORMAL>>4, ox, oy, r);
	ETClearOct(ETAMA_ANGLE16 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, r);
	ETClearOct(ETAMA_ANIME16 >>4, ETAMA_DEL_ANIME >>4, ox, oy, r);

	// ２４×２４ //
	ETClearOct(ETAMA_NORMAL24>>4, ETAMA_DEL_NORMAL>>4, ox, oy, r);
	ETClearOct(ETAMA_ANGLE24 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, r);
	ETClearOct(ETAMA_ANIME24 >>4, ETAMA_DEL_ANIME >>4, ox, oy, r);

	// ３２×３２ //
	ETClearOct(ETAMA_NORMAL32>>4, ETAMA_DEL_NORMAL>>4, ox, oy, r);
	ETClearOct(ETAMA_ANGLE32 >>4, ETAMA_DEL_ANGLE >>4, ox, oy, r);
	ETClearOct(ETAMA_ANIME32 >>4, ETAMA_DEL_ANIME >>4, ox, oy, r);
}


// 難易度 Easy 用にデータを書き換え //
IVOID CEnemyTama::EasyCommand(AttackCommand *pCmd)
{
}


// 難易度 Hard 用にデータを書き換え //
IVOID CEnemyTama::HardCommand(AttackCommand *pCmd)
{
}


// 難易度 Luna 用にデータを書き換え //
IVOID CEnemyTama::LunaCommand(AttackCommand *pCmd)
{
}


// 弾の描画サイズを読み込む //
IBYTE CEnemyTama::TamaSize(BYTE Color)
{
	// エラーログ用の関数名をセット //
#ifdef PBG_DEBUG
	PbgErrorInit("CEnemyTama::TamaSize()");
#endif

	// 上位４ビットが敵弾のタイプを示している                         //
	// 中心から各頂点までの距離を示しているので1/2*sqrt(2) の値をとる //
	switch(Color & 0xf0){
		// サイズ ８ の敵弾 //
		case ETAMA_NORMAL8:		case ETAMA_ANGLE8:		case ETAMA_ANIME8:
		return 6;

		// サイズ 16 の敵弾 //
		case ETAMA_NORMAL16:	case ETAMA_ANGLE16:		case ETAMA_ANIME16:
		return 11;

		// サイズ 24 の敵弾 //
		case ETAMA_NORMAL24:	case ETAMA_ANGLE24:		case ETAMA_ANIME24:
		return 17;

		// サイズ 32 の敵弾 //
		case ETAMA_NORMAL32:	case ETAMA_ANGLE32:		case ETAMA_ANIME32:
		return 23;

		// 消去タイプの敵弾の場合 //
		case ETAMA_DEL_NORMAL:	case ETAMA_DEL_ANGLE:	case ETAMA_DEL_ANIME:
		#ifdef PBG_DEBUG
			PbgErrorEx("最初から消去タイプの敵弾は発生させられません");
		#endif
		return 0;

		// それ以外じゃ駄目 //
		default:
		#ifdef PBG_DEBUG
			PbgErrorEx("弾ＩＤが範囲外ですぞ");
		#endif
		return 0;
	}
}


// 発射角セット : 扇状発射 //
BYTE CEnemyTama::TamaAngleWay(AttackCommand *pCmd, int i)
{
	int			temp;

	i    = i + 1;
	temp = (i >> 1) * (pCmd->wAngle) * (1 - ((i & 1) << 1));

	if((pCmd->Num) & 1) return temp;						// 奇数方向弾の場合
	else                return temp - (pCmd->wAngle >> 1);	// 偶数方向弾の場合
}


// 発射角セット : 全方向発射 //
BYTE CEnemyTama::TamaAngleAll(AttackCommand *pCmd, int i)
{
	// つまり、(256 * i) / n //
	return (i << 8) / (pCmd->Num);
}


// 発射角セット : ランダム方向発射 //
BYTE CEnemyTama::TamaAngleRnd(AttackCommand *pCmd, int i)
{
#ifdef PBG_DEBUG
	if(0 == pCmd->wAngle){
		PbgError("CEnemyTama::TamaAngleRnd() : wAngle がゼロじゃ困るのね");
		return 0;
	}
#endif

	return RndEx() % (pCmd->wAngle) - (pCmd->wAngle >> 1);
}


// 発射角セット : 一方向発射 //
BYTE CEnemyTama::TamaAngleOne(AttackCommand *pCmd, int i)
{
	return 0;
}


// 速度セット : 速度標準 //
int CEnemyTama::TamaSpeedStd(AttackCommand *pCmd, int n)
{
	int		temp = pCmd->Speed;

	if(0 == n) return temp;						// 通常
	else       return temp + (temp >> 3) * n;	// 連弾
}


// 速度セット : 速度ランダム付き //
int CEnemyTama::TamaSpeedRnd(AttackCommand *pCmd, int n)
{
	int		temp;

	// この部分は、十分な微調整が必要である //
	temp = (pCmd->Speed >> 4) + 1;							// 速度 / 16 + 1
	temp = pCmd->Speed + (RndEx() % temp) - (temp >> 1);	// 速度 ± (temp/2)

	if(0 == n) return temp;						// 通常
	else       return temp + (temp >> 3) * n;	// 連弾
}


// 通常の移動 //
IVOID CEnemyTama::MoveDefault(TamaData *pTama)
{
	int			vx, vy;
	int			ideg;

	// 通常弾の処理(一番、出現確率が高い) //
	if(TAMA_NORM == pTama->Type){
		pTama->x += pTama->vx;		// Ｘ座標の更新
		pTama->y += pTama->vy;		// Ｙ座標の更新
		return;
	}

	// 種類別に処理を行う                                      //
	// この switch 内で break を行うと、通常弾へと状態推移する //
	switch(pTama->Type){
		case TAMA_NORM_A:	// 加速弾
			vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
			vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得

			// 速度・座標の更新 //
			pTama->x += vx;			// Ｘ座標の更新
			pTama->y += vy;			// Ｙ座標の更新
			pTama->v += pTama->a;	// 速度の更新

			// 加速時間が終了した場合 //
			if(pTama->Count >= pTama->Rep) break;
		return;

		case TAMA_HOMING:	// ｎ回ホーミング
			vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
			vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得

			// 速度・座標の更新 //
			pTama->x += vx;			// Ｘ座標の更新
			pTama->y += vy;			// Ｙ座標の更新
			pTama->v += pTama->a;	// 速度の更新

			// 加速モード：速度が初速度を上回った場合 //
			if( (pTama->a > 0) && (pTama->v >= pTama->v0) ){
				pTama->a = -(pTama->a);		// 加速度を反転する(加速→減速へ)

				// 繰り返し回数が０になった場合 //
				if( (--(pTama->Rep)) == 0 ) break;
			}

			// 減速モード：速度が負の値を取った場合 //
			if( (pTama->a < 0) && (pTama->v <= 0) ){
				pTama->a = -(pTama->a);	// 加速度を反転する(減速→加速へ)

				// 進行角度を目標にセットする //
				pTama->Angle = Atan8((*m_pX)-(pTama->x), (*m_pY)-(pTama->y));
			}
		return;

		case TAMA_HOMING_M:	// ｎ％ホーミング
			vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
			vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得

			// 速度・座標の更新 //
			pTama->x += vx;			// Ｘ座標の更新
			pTama->y += vy;			// Ｙ座標の更新

//			if( (pTama->Count & 1) == 0 ){
				ideg = Atan8((*m_pX)-(pTama->x), (*m_pY)-(pTama->y)) - pTama->Angle;
				if(     ideg < -128) ideg += 256;
				else if(ideg >  128) ideg -= 256;

//				if(!(pTama->Option & (TOPT_CLIP)) || abs(ideg) < 8){
				if(!(pTama->IsClipped) || abs(ideg) < 8){
					if(pTama->v < pTama->v0){
						pTama->v += pTama->a;		// 速度の更新
					}

//					pTama->Option &= (~TOPT_CLIP);	// クリッピング属性の除去
					pTama->IsClipped = 0;

					pTama->Angle += ((ideg * 5) / 255);
				}
				else{
					if(pTama->v > 256){
						pTama->v -= pTama->a;
						pTama->Angle += ((ideg * (BYTE)pTama->vAngle) / (255 * 2));
					}
					else{
						pTama->Angle += ((ideg * (BYTE)pTama->vAngle) / 255);
					}
				}

//			}

			// ホーミングの時間切れ //
			if(pTama->Count >= pTama->Rep){
				// pTama->Option &= (~TOPT_CLIP);	// クリッピング属性の除去
				pTama->IsClipped = 0;
//				vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
//				vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得
//				break;
			}
		return;

		case TAMA_ROLL:		// 回転弾
			vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
			vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得

			// 速度・座標の更新 //
			pTama->x     += vx;				// Ｘ座標の更新
			pTama->y     += vy;				// Ｙ座標の更新
			pTama->Angle += pTama->vAngle;	// 角速度に従い、角度を変更する

			// 回転時間が終了した場合 //
			if(pTama->Count >= pTama->Rep) break;
		return;

		case TAMA_ROLL_A:	// 回転弾(加速)
			vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
			vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得

			// 速度・座標の更新 //
			pTama->x += vx;			// Ｘ座標の更新
			pTama->y += vy;			// Ｙ座標の更新
			pTama->v += pTama->a;	// 速度の更新

			// 減速モード //
			if( (pTama->a < 0) && (pTama->v <= 0) ){
				pTama->a = -(pTama->a);		// 加速度反転
			}
			// 加速モード //
			else if(pTama->a > 0){
				pTama->Angle += pTama->vAngle;	// 回転を行う

				// 現在の速度が初速度よりも大きくなった場合 //
				if(pTama->v >= pTama->v0){
					// 回転の繰り返し回数まで到達した場合 //
					if( (--(pTama->Rep)) == 0) break;

					pTama->a = -(pTama->a);		// 加速度反転
				}
			}
		return;

		case TAMA_ROLL_R:	// 回転弾(反転)
			vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分の取得
			vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分の取得

			// 速度・座標の更新 //
			pTama->x     += vx;				// Ｘ座標の更新
			pTama->y     += vy;				// Ｙ座標の更新
			pTama->v     += pTama->a;		// 速度の更新
			pTama->Angle += pTama->vAngle;	// 角速度に従い、角度を変更する

			// 減速モード //
			if( (pTama->a < 0) && (pTama->v <= 0) ){
				pTama->Angle += 128;			// 進行方向を反転
				pTama->a      = -(pTama->a);	// 加速度も反転
			}
			// 加速モード //
			else if( (pTama->a > 0) && (pTama->v >= pTama->v0) ){
				pTama->a = -(pTama->a);		// 加速度を反転

				// 繰り返しの終了　//
				if( (--(pTama->Rep)) == 0 ) break;
			}
		return;

		case TAMA_GRAVITY:	// 落下弾
			// 注意：ここでは (vx, vy) ではなく pTama->XX のほうを用いる //
			pTama->x  += pTama->vx;		// Ｘ座標の更新
			pTama->y  += pTama->vy;		// Ｙ座標の更新
			pTama->vy += pTama->a;		// 落下
		return;

		case TAMA_CHANGE:	// 角度変更弾
			// 注意：ここでは (vx, vy) ではなく pTama->XX のほうを用いる //
			pTama->x  += pTama->vx;		// Ｘ座標の更新
			pTama->y  += pTama->vy;		// Ｙ座標の更新

			// 繰り返しの終了 //
			if(pTama->Count >= pTama->Rep){
				// 角度を変更する辺りが他の動きとは違う               //
				// 他の弾とは少々性質が異なるので、break で処理しない //
				pTama->Type    = TAMA_NORM;				// 通常弾に変更
				//pTama->Option &= (~TOPT_CLIP);			// クリッピング属性除去
				pTama->IsClipped = 0;
				pTama->Angle   = (BYTE)pTama->vAngle;	// 角度変更
				pTama->vx = CosL(pTama->Angle, pTama->v);	// Ｘ速度成分
				pTama->vy = SinL(pTama->Angle, pTama->v);	// Ｙ速度成分
			}
		return;

		default:
		#ifdef PBG_DEBUG
			PbgError(	"CEnemyTama::MoveDefault() : "
						"あのぉ、敵弾の種類ＩＤがおかしいんですけど");
		#endif
		return;
	}

	// ここに到達するのは、特殊な状態にあった敵弾が、//
	// 通常弾へと状態推移する場合である              //
	pTama->Type    = TAMA_NORM;		// 通常弾に変更

//	pTama->Option &= (~TOPT_CLIP);	// クリッピング属性の除去
	pTama->IsClipped = 0;

	pTama->vx      = vx;			// 速度のＸ成分
	pTama->vy      = vy;			// 速度のＹ成分
}


// オプションによる移動 //
IVOID CEnemyTama::MoveOption(TamaData *pTama)
{
	BYTE		op;
	int			dx, dy;

	// 下位４ビットは繰り返し回数なので、除去する //
	op = pTama->Option & 0xf0;
/*
	if(pTama->Option){
		char	buf[100];
		wsprintf(buf, "MaskBit %02x  op=%02x", pTama->Option, op);
		PbgError(buf);
	}
*/
	switch(op){
		case TOPT_NONE:		// オプションなし
			// 無論、何もしません //
		return;

		case TOPT_REFX:		// 反射Ｘ
			op = (pTama->Option) & 0x0f;	// 繰り返し回数を求める
			if( (pTama->x < m_RefXMin) || (pTama->x > m_RefXMax) ){
				pTama->Angle = 128 - pTama->Angle;		// 横方向に折り返す
				pTama->vx    = -(pTama->vx);			// 速度Ｘ成分の反転
				pTama->x    += CosL(pTama->Angle, pTama->v);	// Ｘ座標修正
				pTama->y    += SinL(pTama->Angle, pTama->v);	// Ｙ座標修正

				// 繰り返し回数が残り０ならば、反射Ｘフラグを除去 //
				// そうでなければ、繰り返し回数をデクリメント     //
				if(0 == op) pTama->Option = TOPT_NONE;
				else        pTama->Option = TOPT_REFX | (op - 1);
			}
		return;

		case TOPT_REFX2:	// 反射Ｘ + 自機セット
			op = (pTama->Option) & 0x0f;	// 繰り返し回数を求める
			if( (pTama->x < m_RefXMin) || (pTama->x > m_RefXMax) ){
				dx = (*m_pX) - pTama->x;	// Ｘ方向の座標差分を求める
				dy = (*m_pY) - pTama->y;	// Ｙ方向の座標差分を求める

				pTama->Angle = Atan8(dx, dy);		// 角度を自機にセットする
				pTama->vx    = CosL(pTama->Angle, pTama->v);	// 速度のＸ成分を更新
				pTama->vy    = SinL(pTama->Angle, pTama->v);	// 速度のＹ成分を更新
				pTama->x    += pTama->vx;		// １フレーム分の移動
				pTama->y    += pTama->vy;		// 上に同じ

				// 繰り返し回数が残り０ならば、反射Ｘフラグを除去 //
				// そうでなければ、繰り返し回数をデクリメント     //
				if(0 == op) pTama->Option = TOPT_NONE;
				else        pTama->Option = TOPT_REFX2 | (op - 1);
			}
		return;

		case TOPT_REFY:		// 反射Ｙ
			op = (pTama->Option) & 0x0f;	// 繰り返し回数を求める
			if(pTama->y < m_RefYMin){
				pTama->Angle = -(pTama->Angle);		// 縦方向に折り返す
				pTama->vy    = -(pTama->vy);		// 速度Ｙ成分の反転
				pTama->x    += CosL(pTama->Angle, pTama->v);	// Ｘ座標修正
				pTama->y    += SinL(pTama->Angle, pTama->v);	// Ｙ座標修正

				// 繰り返し回数が残り０ならば、反射Ｙフラグを除去 //
				// そうでなければ、繰り返し回数をデクリメント     //
				if(0 == op) pTama->Option = TOPT_NONE;
				else        pTama->Option = TOPT_REFY | (op - 1);
			}
		return;

		case TOPT_REFXY:	// 反射ＸＹ
			op = (pTama->Option) & 0x0f;	// 繰り返し回数を求める
			if( (pTama->x < m_RefXMin) || (pTama->x > m_RefXMax) ){
				pTama->Angle = 128 - pTama->Angle;		// 横方向に折り返す
				pTama->vx    = -(pTama->vx);			// 速度Ｘ成分の反転
				pTama->x    += CosL(pTama->Angle, pTama->v);	// Ｘ座標修正
				pTama->y    += SinL(pTama->Angle, pTama->v);	// Ｙ座標修正

				// 繰り返し回数が残り０ならば、反射Ｘフラグを除去 //
				// そうでなければ、繰り返し回数をデクリメント     //
				if(0 == op) pTama->Option = TOPT_NONE;
				else        pTama->Option = TOPT_REFXY | (op - 1);
			}

			if(pTama->y < m_RefYMin){
				pTama->Angle = -(pTama->Angle);		// 縦方向に折り返す
				pTama->vy    = -(pTama->vy);		// 速度Ｙ成分の反転
				pTama->x    += CosL(pTama->Angle, pTama->v);	// Ｘ座標修正
				pTama->y    += SinL(pTama->Angle, pTama->v);	// Ｙ座標修正

				// 繰り返し回数が残り０ならば、反射Ｙフラグを除去 //
				// そうでなければ、繰り返し回数をデクリメント     //
				if(0 == op) pTama->Option = TOPT_NONE;
				else        pTama->Option = TOPT_REFXY | (op - 1);
			}
		return;

		default:
		#ifdef PBG_DEBUG
			PbgError(	"CEnemyTama::MoveOption() : "
						"あのぉ、オプション番号が変なんですけど...");
		#endif
		return;
	}
}


// 当たり判定の実行を行う //
//                                                      //
// 参考 : pShave, pHit は呼び出し側で初期化しておくこと //
//                                                      //
IVOID CEnemyTama::CheckETHit(DWORD ID			// 敵弾ＩＤ(>>4 によるリスト指定用)
						   , DWORD DeleteID		// 消去時の格納先
						   , DWORD Damage		// １個あたりのダメージ量
						   , DWORD HitWidth		// 当たり判定の幅
						   , DWORD *pShave		// カスった回数
						   , DWORD *pHit)		// ダメージ
{
	Iterator		it;
	int				mx, my;		// 自機の座標
	int				tx, ty;		// 敵弾の座標
	BYTE			d;
	DWORD			ShaveWidth;	// カスリ幅
	CShaveEffect	*pEfc;		// エフェクト管理

	ShaveWidth = HitWidth * 6;

	// 当たり判定の対象を確定する //
	mx = *m_pX;		// メンバ → ローカル
	my = *m_pY;		// の変換を行う

	pEfc = m_pShaveEfc;

	// 今回は && で結合するので、確率が低い順(y, x)で実行した方が効率が良い //
	ThisForEachFront(ID, it){
		tx = it->x;		// 敵弾のＸ座標を取得
		ty = it->y;		// 敵弾のＹ座標を取得

		// まずは、カスり判定を行う //
		if(HitCheckFast(my, ty, ShaveWidth)
			&& HitCheckFast(mx, tx, ShaveWidth)){
			if(0 == it->IsShaved){
				// it->Angle+128
				d = Atan8(tx-mx, ty-my);
				pEfc->Set((tx+mx)>>1, (ty+my)>>1, d);
				it->IsShaved = 0xff;	// カスり済み
				(*pShave)++;			// ポインタ前進ではない
			}

			// カスり判定内にある場合だけ、敵弾にＨＩＴする可能性がある //
			if(HitCheckFast(my, ty, HitWidth)
				&& HitCheckFast(mx, tx, HitWidth)){
				(*pHit) += Damage;

				it->Count = 0;
				MoveBack(it, DeleteID);
			}
		}
	}
}


// 矩形・敵弾消去を実行する         //
// ID, DeleteID : CheckETHit に同じ //
// ox, oy       : 矩形の中心        //
// w            : 矩形の幅          //
// h            : 矩形の高さ        //
IVOID CEnemyTama
	::ETClearRect(DWORD ID, DWORD DeleteID, int ox, int oy, int w, int h)
{
	Iterator		it;
	int				tx, ty;

	// 今回は && で結合するので、確率が低い順(y, x)で実行した方が効率が良い //
	ThisForEachFront(ID, it){
		tx = it->x;		// 敵弾のＸ座標を取得
		ty = it->y;		// 敵弾のＹ座標を取得

		// 範囲内チェック //
		if(HitCheckFast(oy, ty, h)
			&& HitCheckFast(ox, tx, w)){
			it->Count = 0;
			MoveBack(it, DeleteID);
		}
	}
}


// 八角形・敵弾消去を実行する                    //
// ID, DeleteID : CheckETHit に同じ              //
// ox, oy       : 矩形の中心                     //
// r            : ８角形を含む正方形の一辺の長さ //
IVOID CEnemyTama
	::ETClearOct(DWORD ID, DWORD DeleteID, int ox, int oy, int r)
{
	Iterator		it;
	int				tx, ty;
	int				dx, dy;
	int				r2;

	r2 = (r * 3) >> 1;		// r2 = 1.5 * r

	// 今回は && で結合するので、確率が低い順(y, x)で実行した方が効率が良い //
	ThisForEachFront(ID, it){
		tx = it->x;		// 敵弾のＸ座標を取得
		ty = it->y;		// 敵弾のＹ座標を取得

		dx = abs(tx - ox);
		dy = abs(ty - oy);

		// 八角形の内側にあるかね？ //
		if((dx < r) && (dy < r) && (dx + dy < r2)){
			it->Count = 0;
			MoveBack(it, DeleteID);
		}
	}
}


// まる弾(消去中)の描画を行う //
IVOID CEnemyTama::DrawDelNormal(void)
{
	Iterator			it;

	InitRList(0);	// 頂点バッファの初期化

	ThisForEachFront(ETAMA_DEL_NORMAL>>4, it){
		InsertRList(it, it->Angle);
	}

//	DrawRListDel();		// 後は、描画するのみ！！
}


// 角度同期弾(消去中)の描画を行う //
IVOID CEnemyTama::DrawDelAngle(void)
{
	Iterator			it;

	InitRList(0);	// 頂点バッファの初期化

	ThisForEachFront(ETAMA_DEL_ANGLE>>4, it){
		InsertRList(it, it->Angle);
	}

//	DrawRListDel();		// 後は、描画するのみ！！
}


// アニメ弾(消去中)の描画を行う //
IVOID CEnemyTama::DrawDelAnime(void)
{
	Iterator			it;

	InitRList(0);	// 頂点バッファの初期化

	ThisForEachFront(ETAMA_DEL_ANIME>>4, it){
		InsertRList(it, it->Angle);
	}

//	DrawRListDel();		// 後は、描画するのみ！！
}


// まる弾の描画を行う //
FVOID CEnemyTama::DrawNormal(DWORD ID, int Size)
{
	Iterator			it;
	int					x, y;

	InitRList(Size);	// 頂点バッファの初期化

	ThisForEachFront(ID, it){
		x = (it->x) >> 8;	// 座標はx256 固定小数点数で表現されている
		y = (it->y) >> 8;	// したがって、描画時は、８ビット右シフト
		InsertRList(x, y, it->Color);
	}
}


// 角度同期弾の描画を行う //
FVOID CEnemyTama::DrawAngle(DWORD ID, int Size)
{
	Iterator			it;
	int					x, y;

	InitRList(Size);	// 頂点バッファの初期化

	ThisForEachFront(ID, it){
		x = (it->x) >> 8;	// 座標はx256 固定小数点数で表現されている
		y = (it->y) >> 8;	// したがって、描画時は、８ビット右シフト
		InsertRList(x, y, it->Color, it->Angle);
	}
}


// アニメ弾の描画を行う //
FVOID CEnemyTama::DrawAnime(DWORD ID, int Size)
{
	Iterator			it;
	int					x, y;

	InitRList(Size);	// 頂点バッファの初期化

	ThisForEachFront(ID, it){
		x = (it->x) >> 8;	// 座標はx256 固定小数点数で表現されている
		y = (it->y) >> 8;	// したがって、描画時は、８ビット右シフト
		InsertRList(x, y, it->Color, it->Anime);
	}
}


// 頂点バッファを初期化する            //
// ちなみに VERTEX BUFFER の事では無い //
void CEnemyTama::InitializeBuffer(void)
{
	DWORD		i;

	// すでにバッファの初期化は行われている //
	if(TRUE == m_BufferInitialized) return;

	// ここで重要なのでは、実際に描画を行うときに代入する回数を減らすことが //
	// 出来ることである。すなわち、描画時には、(x, y, u, v) の代入だけを    //
	// 行えば良いことになる                                                 //
	for(i=0; i<ENEMYTAMA_BUFFERSIZE; i++){
		Set2DPointC(m_RenderList+i, 0, 0, 0, 0, RGBA_MAKE(255,255,255,220));
	}


	// それでは、テクスチャ座標の格納と行きましょうか //
	#define TEXTURE_SIZE		256		// 弾テクスチャのサイズ
	#define NORMAL_TEX_COLOR	7		// まる弾の種類
	#define ANIME_TEX_COLOR		5		// アニメーション弾の種類
	#define ANGLE_TEX_COLOR		5		// 角度同期弾の種類

	// まる弾のテクスチャ //
	m_TextureUV[ETAMA_NORMAL8 + 0].Set( 8,  8, 24, 24, TEXTURE_SIZE);	// 赤弾
	m_TextureUV[ETAMA_NORMAL8 + 1].Set( 8, 32, 24, 48, TEXTURE_SIZE);	// 青弾
	m_TextureUV[ETAMA_NORMAL8 + 2].Set( 8, 56, 24, 72, TEXTURE_SIZE);	// 緑弾
	m_TextureUV[ETAMA_NORMAL8 + 3].Set( 8, 80, 24, 96, TEXTURE_SIZE);	// 紫弾
	m_TextureUV[ETAMA_NORMAL8 + 4].Set( 8,104, 24,120, TEXTURE_SIZE);	// 橙弾
	m_TextureUV[ETAMA_NORMAL8 + 5].Set(56, 48, 88, 80, TEXTURE_SIZE);	// ◎弾
	m_TextureUV[ETAMA_NORMAL8 + 6].Set( 8,152, 24,168, TEXTURE_SIZE);	// 暗黒弾

	// 回転アニメーション弾のテクスチャ //
	m_TextureUV[ETAMA_ANIME8 + 0].Set(32,  8, 48, 24, TEXTURE_SIZE);	// 青弾
	m_TextureUV[ETAMA_ANIME8 + 1].Set(32, 32, 48, 48, TEXTURE_SIZE);	// 赤弾
	m_TextureUV[ETAMA_ANIME8 + 2].Set(32, 56, 48, 72, TEXTURE_SIZE);	// 紫弾
	m_TextureUV[ETAMA_ANIME8 + 3].Set(32, 80, 48, 96, TEXTURE_SIZE);	// 緑弾
	m_TextureUV[ETAMA_ANIME8 + 4].Set(56,  8, 88, 40, TEXTURE_SIZE);	// ☆弾

	// 回転アニメーション弾のテクスチャ //
	m_TextureUV[ETAMA_ANGLE8 + 0].Set(32,104, 48,120, TEXTURE_SIZE);	// 青弾
	m_TextureUV[ETAMA_ANGLE8 + 1].Set(32,128, 48,144, TEXTURE_SIZE);	// 赤弾
	m_TextureUV[ETAMA_ANGLE8 + 2].Set(32,152, 48,168, TEXTURE_SIZE);	// 橙弾
	m_TextureUV[ETAMA_ANGLE8 + 3].Set(32,176, 48,192, TEXTURE_SIZE);	// －弾
	m_TextureUV[ETAMA_ANGLE8 + 4].Set(56, 88, 88,120, TEXTURE_SIZE);	// みさいる


	// 別サイズのものにも同じ値をセット //
	for(i=0; i<NORMAL_TEX_COLOR; i++){
		m_TextureUV[ETAMA_NORMAL16 + i] = m_TextureUV[ETAMA_NORMAL24 + i]
			= m_TextureUV[ETAMA_NORMAL32 + i] = m_TextureUV[ETAMA_NORMAL8 + i];
	}

	for(i=0; i<ANIME_TEX_COLOR; i++){
		m_TextureUV[ETAMA_ANIME16 + i] = m_TextureUV[ETAMA_ANIME24 + i]
			= m_TextureUV[ETAMA_ANIME32 + i] = m_TextureUV[ETAMA_ANIME8 + i];
	}

	for(i=0; i<ANGLE_TEX_COLOR; i++){
		m_TextureUV[ETAMA_ANGLE16 + i] = m_TextureUV[ETAMA_ANGLE24 + i]
			= m_TextureUV[ETAMA_ANGLE32 + i] = m_TextureUV[ETAMA_ANGLE8 + i];
	}

	// 初期化完了フラグを立てる //
	m_BufferInitialized = TRUE;
}


// 頂点データに格納する準備する //
FVOID CEnemyTama::InitRList(int Size)
{
	m_RLSize    = Size;				// 描画する弾のサイズ
	m_pRLTarget = m_RenderList;		// リストの先頭に持ってくる
}


// 頂点リストの内容を描画(通常) //
FVOID CEnemyTama::DrawRListStd(void)
{
return;
	int			n;

	// ポリゴン数 //
	n = m_pRLTarget - m_RenderList;
	if(0 == n) return;


	// まずは５０％半透明でカラーキー付き描画を行う //
	g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);

/*
	// 次に１：１半透明で描画する //
	pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);
*/
}


// 頂点リストの内容を描画(消去) //
FVOID CEnemyTama::DrawRListDel(void)
{
return;
	int			n;

	// ポリゴン数 //
	n = m_pRLTarget - m_RenderList;
	if(0 == n) return;

/*
	// まずは５０％半透明でカラーキー付き描画を行う //
	pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);
*/
	// 次に１：１半透明で描画する //
	g_pGrp->SetRenderStateEx(GRPST_ALPHAONE);
	g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_RenderList, n);
}


// 頂点バッファに弾データを追加する(丸弾専用) //
IVOID CEnemyTama::InsertRList(int ox, int oy, BYTE UV_ID)
{
//	int			x, y;
	D3DVALUE	u1, v1, u2, v2;
	D3DRECTANGLE	*pr;

	// テクスチャ座標 //
	pr = &m_TextureUV[UV_ID];
	u1 = pr->m_Left;	// テクスチャの左の座標
	v1 = pr->m_Top;		// テクスチャの上の座標
	u2 = pr->m_Right;	// テクスチャの右の座標
	v2 = pr->m_Bottom;	// テクスチャの下の座標
/*
	// 左上の頂点 //
	x = ox - m_RLSize;		// 左上の頂点のＸ座標
	y = oy - m_RLSize;		// 左上の頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+0, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+5, x, y, u1, v1);

	// 隣り合う頂点は、Ｘ座標もしくはＹ座標が一致するので、 //
	// 一致する方の座標とＵＶ座標をセットする               //
	SET2DP_YUV(m_pRLTarget+1, y, u2, v1);
	SET2DP_XUV(m_pRLTarget+4, x, u1, v2);

	// 右下の頂点 //
	x = ox + m_RLSize;		// 右下の頂点のＸ座標
	y = oy + m_RLSize;		// 右下の頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+2, x, y, u2, v2);
	SET2DP_XYUV(m_pRLTarget+3, x, y, u2, v2);

	// 先ほど格納できていない方の座標だけセットする //
	SET2DP_X(m_pRLTarget+1, x);
	SET2DP_Y(m_pRLTarget+4, y);
*/
	int d = m_RLSize;
	SET2DP_XYUV(m_pRLTarget+0, ox-d, oy-d, u1, v1);
	SET2DP_XYUV(m_pRLTarget+1, ox+d, oy-d, u1, v2);
	SET2DP_XYUV(m_pRLTarget+2, ox+d, oy+d, u2, v2);
	SET2DP_XYUV(m_pRLTarget+3, ox-d, oy+d, u2, v1);
g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, m_pRLTarget, 4);
return;
//g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_pRLTarget, 6);
//return;

	// 次の頂点に //
//	m_pRLTarget += 6;
}


// 頂点バッファに弾データを追加する(角度含む) //
IVOID CEnemyTama::InsertRList(int ox, int oy, BYTE UV_ID, BYTE d)
{
	int			x, y, dx, dy;
	D3DVALUE	u1, v1, u2, v2;
	D3DRECTANGLE	*pr;

	// テクスチャ座標を求めてしまいましょう //
	pr = &m_TextureUV[UV_ID];
	u1 = pr->m_Left;	// テクスチャの左の座標
	v1 = pr->m_Top;		// テクスチャの上の座標
	u2 = pr->m_Right;	// テクスチャの右の座標
	v2 = pr->m_Bottom;	// テクスチャの下の座標

	// それぞれの頂点は、中心から９０度回転した場所に位置しているので、 //
	// Ｃｏｓ＆Ｓｉｎは一つだけ求めれば良い                             //
	dx = CosL(d-96, m_RLSize);	// ０度のとき、左上に来る頂点のＸ座標
	dy = SinL(d-96, m_RLSize);	// ... Ｙ座標
/*
	x = ox + dx;	// (u1, v1) に対する頂点のＸ座標
	y = oy + dy;	// (u1, v1) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+0, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+5, x, y, u1, v1);

	x = ox - dy;	// (u2, v1) に対する頂点のＸ座標
	y = oy + dx;	// (u2, v1) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+1, x, y, u2, v1);

	x = ox - dx;	// (u2, v2) に対する頂点のＸ座標
	y = oy - dy;	// (u2, v2) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+2, x, y, u2, v2);
	SET2DP_XYUV(m_pRLTarget+3, x, y, u2, v2);

	x = ox + dy;	// (u1, v2) に対する頂点のＸ座標
	y = oy - dx;	// (u1, v2) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+4, x, y, u1, v2);
*/
	SET2DP_XYUV(m_pRLTarget+0, ox+dx, oy+dy, u1, v1);
	SET2DP_XYUV(m_pRLTarget+1, ox+dy, oy-dx, u1, v2);
	SET2DP_XYUV(m_pRLTarget+2, ox-dx, oy-dy, u2, v2);
	SET2DP_XYUV(m_pRLTarget+3, ox-dy, oy+dx, u2, v1);
g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, m_pRLTarget, 4);
return;
//g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_pRLTarget, 6);
//return;

	// 次の頂点に //
//	m_pRLTarget += 6;
}


// 頂点バッファに弾データを追加する(消去エフェクト含む) //
IVOID CEnemyTama::InsertRList(Iterator &it, BYTE d)
{
	int			x, y, dx1, dy1, dx2, dy2;

	int				ox, oy;
	int				size;
	D3DVALUE		u1, v1, u2, v2;
	D3DRECTANGLE	*pr;
	BYTE			UV_ID;
	BYTE			c;

	TamaData		*pt = it.GetPtr();

	ox    = pt->x >> 8;
	oy    = pt->y >> 8;
	UV_ID = pt->Color;
	c     = 96 + ((BYTE)pt->Count);// * 2;//((BYTE)(it->Count) * 3) / 2;
	size  = pt->Size + pt->Count / 3;

	// テクスチャ座標を求めてしまいましょう //
	pr = &m_TextureUV[UV_ID];
	u1 = pr->m_Left;	// テクスチャの左の座標
	v1 = pr->m_Top;		// テクスチャの上の座標
	u2 = pr->m_Right;	// テクスチャの右の座標
	v2 = pr->m_Bottom;	// テクスチャの下の座標

	dx1 = CosL(d - c, size);	// ０度のとき、左上に来る頂点のＸ座標
	dy1 = SinL(d - c, size);	// ... Ｙ座標
	dx2 = CosL(d + c, size);	// ０度のとき、左下に来る頂点のＸ座標
	dy2 = SinL(d + c, size);	// ... Ｙ座標
/*
	x = ox + dx1;	// (u1, v1) に対する頂点のＸ座標
	y = oy + dy1;	// (u1, v1) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+0, x, y, u1, v1);
	SET2DP_XYUV(m_pRLTarget+5, x, y, u1, v1);

	x = ox - dx2;	// (u2, v1) に対する頂点のＸ座標
	y = oy - dy2;	// (u2, v1) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+1, x, y, u2, v1);

	x = ox - dx1;	// (u2, v2) に対する頂点のＸ座標
	y = oy - dy1;	// (u2, v2) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+2, x, y, u2, v2);
	SET2DP_XYUV(m_pRLTarget+3, x, y, u2, v2);

	x = ox + dx2;	// (u1, v2) に対する頂点のＸ座標
	y = oy + dy2;	// (u1, v2) に対する頂点のＹ座標
	SET2DP_XYUV(m_pRLTarget+4, x, y, u1, v2);
*/
	SET2DP_XYUV(m_pRLTarget+0, ox+dx1, oy+dy1, u1, v1);
	SET2DP_XYUV(m_pRLTarget+1, ox+dx2, oy+dy2, u1, v2);
	SET2DP_XYUV(m_pRLTarget+2, ox-dx1, oy-dy1, u2, v2);
	SET2DP_XYUV(m_pRLTarget+3, ox-dx2, oy-dy2, u2, v1);
g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, m_pRLTarget, 4);
return;
//g_pGrp->DrawPrimitive(D3DPT_TRIANGLELIST, m_pRLTarget, 6);
//return;

	// 次の頂点に //
//	m_pRLTarget += 6;
}
