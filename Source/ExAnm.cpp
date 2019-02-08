/*
 *   ExAnm.cpp   : アニメ定義ブロック
 *
 */

#include "ExAnm.h"
#include "CAtkGrpDraw.h"
#include "ScrBase.h"
#include "CalcStack.h"
#include "SCL.h"



// コンストラクタ //
CAnimeTask::CAnimeTask()
{
	Initialize(NULL, NULL, NULL);
}


// デストラクタ //
CAnimeTask::~CAnimeTask()
{
}


// 初期化する
// arg : pCommand    初期命令列へのポインタ             //
//     : pAnimeDef   アニメーション定義(キャラクタ固有) //
//     : pParent     親タスクへのポインタ(grX を継承)   //
// ret : 成功なら TRUE                                  //
FBOOL CAnimeTask::Initialize(BYTE *pCommand, CAnimeDef *pAnimeDef, CAnimeTask *pParent)
{
	int				i;
	int				*p;

	// スクリプト ExAnm 使用者には隠すべきメンバ(1) //
	m_pCmd      = pCommand;		// 命令コマンド列(現在位置)
	m_pAnimeDef = pAnimeDef;	// アニメーション管理クラス

	// 汎用レジスタ //
	if(pParent){
		p = pParent->m_Gr;
		for(i=0; i<EXANM_GR_MAX; i++) m_Gr[i] = p[i];
	}
	else{
		for(i=0; i<EXANM_GR_MAX; i++) m_Gr[i] = 0;
	}

	// スタックポインタ                       //
	// 参考：スタック本体の初期化は不要である //
	m_CallSP = 0;

	// 繰り返し命令用カウンタ
	m_RepCount = 0;

	// テクスチャモード //
	m_TexMode = TEXMODE_ALPHANORM + TEXMODE_SINGLE;

	// アニメーションパターンの中心座標 //
	m_AnimeX = 0;
	m_AnimeY = 0;

	// アニメーションパターンの拡大率(256 = 1.0) //
	m_AnimeSX = 256;
	m_AnimeSY = 256;

	// クリッピング枠の中心座標 //
	m_ClipX = 0;
	m_ClipY = 0;

	// クリッピング枠のサイズ //
	m_ClipW = 268 * 256;	// CGameProc.cpp を参照のこと
	m_ClipH = 480 * 256;	// 上に同じ

	// アニメーションパターンの補助データ //
	m_AnimeAngle  = 0;	// 傾き(０が元の角度)
	m_AnimeAlpha  = 0;	// テクスチャのα値
	m_AnimePtn[0] = 0;	// アニメーション番号
	m_AnimePtn[1] = 0;	// アニメーション番号

	// アニメーション用クリッピング矩形・塗りつぶし色 //
	m_ClipRed   = 0;	// 赤成分
	m_ClipGreen = 0;	// 緑成分
	m_ClipBlue  = 0;	// 青成分
	m_ClipAlpha = 0;	// α成分

	return TRUE;
}


// １フレーム分更新する(FALSE ならば、タスクを消すべし) //
FBOOL CAnimeTask::Move(CAtkGrpDraw *pParent)
{
	BYTE		*pCurrent;
	int			Addr;


	// 一応、スタックの初期化を行う //
	CalcStack::Reset();

	// スクリプト解析系の関数に限り、goto の使用を許可する //
Start:
#ifdef PBG_DEBUG
	if(GetAsyncKeyState(VK_F11) & 0x8000){
		char	buf[1024];
		wsprintf(buf,	"ExAnm 解析中の停止要求を受理します\r\n"
			"thisPtr(%u)  Offset(%u)", this, CSCLDecoder::Ptr2Offset(m_pCmd));
		PbgLog(buf);
		return FALSE;
	}
#endif

	pCurrent = m_pCmd;
/*
	char temp[1024];
	wsprintf(temp, "op[%02x]", pCurrent[0]);
	PbgLog(temp);
*/
	switch(pCurrent[0]){
///////////////////////////////////////////////////////////////////////////////
//                     ＥｘＡｎｍ固有の命令セット                            //
///////////////////////////////////////////////////////////////////////////////
	case SCR_NOP:		// 停止命令 Cmd(1) + Time(2)
		// 命令の最初の読み込みならば、停止カウント数の取得 //
		// そうでなければ、値をデクリメントする             //
		if(0 == m_RepCount) m_RepCount = *(WORD *)(&pCurrent[1]);
		else                m_RepCount--;

		// 終了条件を満たしている場合 //
		if(0 == m_RepCount){
			m_pCmd += (1 + 2);	// Cmd(1) + Time(2)
			goto Start;			// それでは、次の命令も実行しましょう
		}
	return TRUE;		// 正常終了

	case SCR_TASK:		// タスク発生 Cmd(1) + Addr(4)
		Addr = *(int *)(&pCurrent[1]);
		pParent->SetTask(CSCLDecoder::Offset2Ptr(Addr), m_pAnimeDef, this);
		m_pCmd += (1 + 4);
	goto Start;

	case SCR_TEXMODE:	// Cmd(1) + Mode(1)
		m_TexMode = pCurrent[1];
		m_pCmd += 2;
	goto Start;

//	case SCR_PSE:
#pragma message("効果音再生が完成していないぜ.......................")
//	goto Start;


///////////////////////////////////////////////////////////////////////////////
//                       メンバ変数操作系命令                                //
//                                                                           //
//      SCR_PUSHR  :  [後] スタックにレジスタの内容を積む                    //
//      SCR_POPR   :  [後] レジスタにスタックの内容をコピー                  //
///////////////////////////////////////////////////////////////////////////////
	case EXANM_REG_AX:		// アニメーション：相対Ｘ・中心
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimeX);
		else if(SCR_POPR  == pCurrent[1]) m_AnimeX = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_AY:		// アニメーション」相対Ｙ・中心
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimeY);
		else if(SCR_POPR  == pCurrent[1]) m_AnimeY = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_ASX:		// アニメーションパターンＸ拡大率
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimeSX);
		else if(SCR_POPR  == pCurrent[1]) m_AnimeSX = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_ASY:		// アニメーションパターンＹ拡大率
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimeSY);
		else if(SCR_POPR  == pCurrent[1]) m_AnimeSY = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_AD:		// アニメーションパターンの傾き
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimeAngle);
		else if(SCR_POPR  == pCurrent[1]) m_AnimeAngle = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_AA:		// アニメーションパターンのα値
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimeAlpha);
		else if(SCR_POPR  == pCurrent[1]) m_AnimeAlpha = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_APTN:	// アニメーションパターン番号
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimePtn[0]);
		else if(SCR_POPR  == pCurrent[1]) m_AnimePtn[0] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_APTN2:	// アニメーションパターン番号(2)
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_AnimePtn[1]);
		else if(SCR_POPR  == pCurrent[1]) m_AnimePtn[1] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CX:		// クリッピング枠：相対Ｘ座標中心
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipX);
		else if(SCR_POPR  == pCurrent[1]) m_ClipX = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CY:		// クリッピング枠：相対Ｙ座標中心
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipY);
		else if(SCR_POPR  == pCurrent[1]) m_ClipY = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CW:		// クリッピング枠：横幅
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipW);
		else if(SCR_POPR  == pCurrent[1]) m_ClipW = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CH:		// クリッピング枠：縦幅
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipH);
		else if(SCR_POPR  == pCurrent[1]) m_ClipH = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CR:		// クリッピング枠の色：赤成分
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipRed);
		else if(SCR_POPR  == pCurrent[1]) m_ClipRed = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CG:		// クリッピング枠の色：緑成分
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipGreen);
		else if(SCR_POPR  == pCurrent[1]) m_ClipGreen = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CB:		// クリッピング枠の色：青成分
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipBlue);
		else if(SCR_POPR  == pCurrent[1]) m_ClipBlue = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_CA:		// クリッピング枠の色：α成分
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_ClipAlpha);
		else if(SCR_POPR  == pCurrent[1]) m_ClipAlpha = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR0:		// 汎用レジスタ０番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[0]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[0] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR1:		// 汎用レジスタ１番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[1]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[1] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR2:		// 汎用レジスタ２番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[2]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[2] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR3:		// 汎用レジスタ３番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[3]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[3] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR4:		// 汎用レジスタ４番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[4]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[4] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR5:		// 汎用レジスタ５番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[5]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[5] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR6:		// 汎用レジスタ６番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[6]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[6] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR7:		// 汎用レジスタ７番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[7]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[7] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR8:		// 汎用レジスタ８番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[8]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[8] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;

	case EXANM_REG_GR9:		// 汎用レジスタ９番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Gr[9]);
		else if(SCR_POPR  == pCurrent[1]) m_Gr[9] = CalcStack::Pop();
		else goto Error;
		m_pCmd += 2;
	goto Start;



	case SCR_MOVC:		// [中] レジスタに定数の代入を行う
		if(FALSE == MoveRegister(pCurrent)) goto Error;
		m_pCmd += (1 + 1 + 4);	// cmd(1) + RegID(1) + Const(4)
	goto Start;


	case SCR_PUSHC:		// [前] 定数をスタックにＰＵＳＨする
		CalcStack::Push(*(int *)(&pCurrent[1]));
		m_pCmd += (1 + 4);		// cmd(1) + Const(4)
	goto Start;



///////////////////////////////////////////////////////////////////////////////
//                    スクリプトがサポートすべき命令セット                   //
///////////////////////////////////////////////////////////////////////////////
	case SCR_TJMP:		// [前] POP して真ならばジャンプする
		if(CalcStack::Pop()){
			m_pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_FJMP:		// [前] POP して偽ならばジャンプする
		if(!(CalcStack::Pop())){
			m_pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_JMP:		// [前] 無条件ジャンプ
		m_pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
	goto Start;


	case SCR_OJMP:		// [前] POP して真ならPUSH(TRUE),  JMP
		if(CalcStack::Top()){
			m_pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_AJMP:		// [前] POP して偽ならPUSH(FALSE), JMP
		if(!(CalcStack::Top())){
			m_pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_EXIT:		// [前] 終了する
		// ここでポインタを進めると危険なので、何もしません //
	return FALSE;


	case SCR_LPOP:		// [前] 演算スタックトップ->JMP スタックトップ
		AStk_Push(CalcStack::Pop());
		m_pCmd += 1;			// cmd(1)
	goto Start;


	case SCR_LJMP:		// [前] ０ならジャンプ、真ならデクリメント
		if(FALSE == AStk_DecTop()){
			m_pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_ADD:		// [前] Push(Pop(1) + Pop(0))
		CalcStack::Add();
		m_pCmd++;
	goto Start;

	case SCR_SUB:		// [前] Push(Pop(1) - Pop(0))
		CalcStack::Sub();
		m_pCmd++;
	goto Start;

	case SCR_MUL:		// [前] Push(Pop(1) * Pop(0))
		CalcStack::Mul();
		m_pCmd++;
	goto Start;

	case SCR_DIV:		// [前] Push(Pop(1) / Pop(0))
		CalcStack::Div();
		m_pCmd++;
	goto Start;

	case SCR_MOD:		// [前] Push(Pop(1) % Pop(0))
		CalcStack::Mod();
		m_pCmd++;
	goto Start;

	case SCR_NEG:		// [前] Push(-Pop(0))
		CalcStack::Neg();
		m_pCmd++;
	goto Start;

	case SCR_SINL:		// [前] Push(sinl(Pop(1), Pop(0))
		CalcStack::SinL();
		m_pCmd++;
	goto Start;

	case SCR_COSL:		// [前] Push(cosl(Pop(1), Pop(0))
		CalcStack::CosL();
		m_pCmd++;
	goto Start;

	case SCR_RND:		// [前] Push(rnd() % Pop(0))
		CalcStack::Rnd();
		m_pCmd++;
	goto Start;

	case SCR_ATAN:		// [前] Push(atan(Pop(1), Pop(0))
		CalcStack::Atan();
		m_pCmd++;
	goto Start;

	case SCR_MAX:		// [前] Push( max(Pop(0), Pop(1)) )
		CalcStack::Max();
		m_pCmd++;
	goto Start;

	case SCR_MIN:		// [前] Push( min(Pop(0), Pop(1)) )
		CalcStack::Min();
		m_pCmd++;
	goto Start;

	case SCR_EQUAL:		// [前] Push(Pop(1) == Pop(0))
		CalcStack::Equal();
		m_pCmd++;
	goto Start;

	case SCR_NOTEQ:		// [前] Push(Pop(1) != Pop(0))
		CalcStack::NotEq();
		m_pCmd++;
	goto Start;

	case SCR_ABOVE:		// [前] Push(Pop(1) >  Pop(0))
		CalcStack::Above();
		m_pCmd++;
	goto Start;

	case SCR_LESS:		// [前] Push(Pop(1) <  Pop(0))
		CalcStack::Less();
		m_pCmd++;
	goto Start;

	case SCR_ABOVEEQ:	// [前] Push(Pop(1) >= Pop(0))
		CalcStack::AboveEq();
		m_pCmd++;
	goto Start;

	case SCR_LESSEQ:	// [前] Push(Pop(1) <= Pop(0))
		CalcStack::LessEq();
		m_pCmd++;
	goto Start;

	default:
	goto Error;
	}


Error:
	PbgError("ＥｘＡｎｍ エラー : 未定義の命令コード or レジスタ");
	return FALSE;	// 失敗したので、終了要求を出力する
}


// 消去状態の更新 //
FBOOL CAnimeTask::MoveDelete(void)
{
	short		a1, a2;

	a1 = m_AnimeAlpha = max(0, m_AnimeAlpha - 8);
	a2 = m_ClipAlpha  = max(0, m_ClipAlpha  - 8);

	// 消去すべき時が訪れたとき //
	if((0 == a1) && (0 == a2)) return FALSE;
	else                       return TRUE;
}


// 描画を行う(注:呼び出し前後でクリッピング矩形が変化) //
// arg : ox256, oy256 中心となる座標(x256)             //
FVOID CAnimeTask::Draw(int ox256, int oy256)
{
	D3DTLVERTEX		tlv[20];
	int				x, y, dx, dy;
	RECT			Target;
	DWORD			c;
	BYTE			a;

	if(NULL == m_pAnimeDef) return;

	// 枠を描画する //
	x  = ox256 + m_ClipX;
	y  = oy256 + m_ClipY;
	dx = m_ClipW >> 1;
	dy = m_ClipH >> 1;

	SetRect(&Target, (x-dx)>>8, (y-dy)>>8, (x+dx)>>8, (y+dy)>>8);
	g_pGrp->SetViewport(&Target);

	if(m_ClipAlpha){
		c = RGBA_MAKE(m_ClipRed, m_ClipGreen, m_ClipBlue, m_ClipAlpha);

		Set2DPointC(tlv+0, (x-dx)>>8, (y-dy)>>8, 0, 0, c);
		Set2DPointC(tlv+1, (x+dx)>>8, (y-dy)>>8, 0, 0, c);
		Set2DPointC(tlv+2, (x+dx)>>8, (y+dy)>>8, 0, 0, c);
		Set2DPointC(tlv+3, (x-dx)>>8, (y+dy)>>8, 0, 0, c);

		g_pGrp->SetRenderStateEx(GRPST_ALPHASTD);
		g_pGrp->SetTexture(GRPTEXTURE_MAX);
		g_pGrp->DrawPrimitive(D3DPT_TRIANGLEFAN, tlv, 4);
	}

	// アニメーション部のα値が０ならば描画の必要は無い //
	if(0 == m_AnimeAlpha) return;

	// 加算用にテクスチャモードをセットする //
	if(m_TexMode & TEXMODE_ALPHAADD){
		a = m_AnimeAlpha;
		c = RGBA_MAKE(a, a, a, a);
		g_pGrp->SetRenderStateEx(GRPST_ALPHAADD);
	}
	// 通常用にテクスチャモードをセットする //
	else{
		c = RGBA_MAKE(255, 255, 255, m_AnimeAlpha);
		g_pGrp->SetRenderStateEx(GRPST_ALPHASTDCK);
	}

	// 描画の中心座標を求める //
	x += m_AnimeX;
	y += m_AnimeY;

	// シングルモードで描画する //
	if(m_TexMode & TEXMODE_DOUBLE){
		m_pAnimeDef->DrawDouble(x, y, m_AnimeSX, m_AnimeSY, c, m_AnimeAngle, m_AnimePtn);
	}
	// ダブルモードで描画する //
	else{
		m_pAnimeDef->DrawSingle(x, y, m_AnimeSX, m_AnimeSY, c, m_AnimeAngle, m_AnimePtn[0]);
	}
}


// レジスタに定数を代入する //
IBOOL CAnimeTask::MoveRegister(BYTE *pCmd)
{
	// BYTE pCmd[0] : SCR_MOVC
	// BYTE pCmd[1] : RegsID
	// int  pCmd[2] : Constant

	switch(pCmd[1]){
		case EXANM_REG_AX:		// アニメーション：相対Ｘ・中心
			m_AnimeX = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_AY:		// アニメーション」相対Ｙ・中心
			m_AnimeY = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_ASX:		// アニメーションパターンＸ拡大率
			m_AnimeSX = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_ASY:		// アニメーションパターンＹ拡大率
			m_AnimeSY = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_AD:		// アニメーションパターンの傾き
			m_AnimeAngle = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_AA:		// アニメーションパターンのα値
			m_AnimeAlpha = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_APTN:	// アニメーションパターン番号
			m_AnimePtn[0] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_APTN2:	// アニメーションパターン番号
			m_AnimePtn[1] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CX:		// クリッピング枠：相対Ｘ座標中心
			m_ClipX = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CY:		// クリッピング枠：相対Ｙ座標中心
			m_ClipY = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CW:		// クリッピング枠：横幅
			m_ClipW = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CH:		// クリッピング枠：縦幅
			m_ClipH = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CR:		// クリッピング枠の色：赤成分
			m_ClipRed = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CG:		// クリッピング枠の色：緑成分
			m_ClipGreen = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CB:		// クリッピング枠の色：青成分
			m_ClipBlue = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_CA:		// クリッピング枠の色：α成分
			m_ClipAlpha = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR0:		// 汎用レジスタ０番
			m_Gr[0] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR1:		// 汎用レジスタ１番
			m_Gr[1] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR2:		// 汎用レジスタ２番
			m_Gr[2] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR3:		// 汎用レジスタ３番
			m_Gr[3] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR4:		// 汎用レジスタ４番
			m_Gr[4] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR5:		// 汎用レジスタ５番
			m_Gr[5] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR6:		// 汎用レジスタ６番
			m_Gr[6] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR7:		// 汎用レジスタ７番
			m_Gr[7] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR8:		// 汎用レジスタ８番
			m_Gr[8] = *(int *)(&pCmd[2]);
		break;

		case EXANM_REG_GR9:		// 汎用レジスタ９番
			m_Gr[9] = *(int *)(&pCmd[2]);
		break;

		default:
			PbgError("EnemyData::MoveRegister() : 不正なレジスタＩＤ");
		return FALSE;
	}
}


// スタックに積む //
IBOOL CAnimeTask::AStk_Push(int Value)
{
	if(m_CallSP >= ENEMY_CSTK_MAX){
		PbgError("CAnimeTask::AStk_Push() : スタックオーバーフロー");
		return FALSE;
	}

	m_CallStack[m_CallSP] = Value;	// 値を積んで
	m_CallSP++;						// スタックポインタを移動

	return TRUE;
}


// スタックから取り出し //
IINT CAnimeTask::AStk_Pop(void)
{
	if(m_CallSP <= 0){
		PbgError("CAnimeTask::AStk_Pop() : スタックアンダーフロー");
		return 0;
	}

	m_CallSP--;						// スタックポインタを移動
	return m_CallStack[m_CallSP];	// スタックトップを返す
}


// ｎ回Popして、最後に取り出した値を返す //
IINT CAnimeTask::AStk_PopN(int n)
{
	if(n > m_CallSP){
		PbgError("CAnimeTask::AStk_PopN() : スタックアンダーフロー");
		return 0;
	}

	m_CallSP -= n;
	return m_CallStack[m_CallSP];	// スタックトップを返す
}


// スタックトップをデクリメント            //
// トップが０ならば POP して FALSE を返し、//
//       非０ならば DEC して TRUE  を返す  //
IINT CAnimeTask::AStk_DecTop(void)
{
	int temp;
	int	Index;

	// スタックトップは CallSP - 1 の指す要素である //
	Index = m_CallSP - 1;

	temp = m_CallStack[Index];
	if(0 == temp){
		m_CallSP = Index;
		return FALSE;
	}

	m_CallStack[Index]--;

	return TRUE;
}
