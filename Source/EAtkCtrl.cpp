/*
 *   EAtkCtrl.cpp   : 敵の攻撃管理
 *
 */

#include "EAtkCtrl.h"
#include "CalcStack.h"
#include "ScrBase.h"
#include "ECL.h"
#include "SCL.h"
#include "TCL.h"
#include "CEnemyLLaser.h"
#include "SECtrl.h"

#include "RndCtrl.h"



// １フレームだけ動作させる //
FBOOL EnemyAtkCtrl::ParseECLCmd(CEnemyCtrl *pParent)
{
	BYTE	*pCurrent;
	int		sx, sy, w;


	// スタックの初期化 //
	CalcStack::Reset();

	// スクリプト解析系の関数に限り、goto の使用を許可する //
Start:
#ifdef PBG_DEBUG
	if(GetAsyncKeyState(VK_F11) & 0x8000){
		char	buf[1024];
		wsprintf(buf,	"TCL 解析中の停止要求を受理します\r\n"
			"thisPtr(%u)  Offset(%u)", this, CSCLDecoder::Ptr2Offset(m_pTCLCmd));
		PbgLog(buf);
		return FALSE;
	}
#endif

	pCurrent = m_pTCLCmd;

	if(m_pParent){
		m_Cmd.ox = m_pParent->x + m_dx;
		m_Cmd.oy = m_pParent->y + m_dy;
	}
	else{
		m_Cmd.ox = m_dx;
		m_Cmd.oy = m_dy;
	}

	switch(pCurrent[0]){
///////////////////////////////////////////////////////////////////////////////
//                       ＴＣＬ固有の命令セット                              //
///////////////////////////////////////////////////////////////////////////////
	case SCR_TAMA:		// 弾による攻撃d
		pParent->GetEnemyTamaPtr()->Set(m_Cmd);
		m_pTCLCmd++;
	goto Start;


	case SCR_LASER:		// レーザーによる攻撃
		pParent->GetEnemySLaserPtr()->Set(m_Cmd);
		m_pTCLCmd++;
	goto Start;


	case SCR_HLASER:	// ホーミングレーザーによる攻撃
		pParent->GetEnemyHLaserPtr()->Set(m_Cmd);
		m_pTCLCmd++;
	goto Start;

	case SCR_LSPHERE:	// ライトニングスフィアによる攻撃
/*		{
			char	temp[100];
			wsprintf(temp, "n = %d", m_Cmd.Num);
			PbgError(temp);
		}*/
		pParent->GetEnemyLSphere()->Set(m_Cmd.Num);
		m_pTCLCmd++;
	goto Start;

	case SCR_RLASER:	// ラウンドレーザーによる攻撃
		pParent->GetEnemyExShot()->SetRLaser(m_Cmd);
		m_pTCLCmd++;
	goto Start;

	case SCR_CROSS:		// 無数の十字架による攻撃
		pParent->GetEnemyExShot()->SetCross(m_Cmd);
		m_pTCLCmd++;
	goto Start;

	case SCR_FLOWER:	// 花による攻撃
		pParent->GetEnemyExShot()->SetFlower(m_Cmd);
		m_pTCLCmd++;
	goto Start;

	case SCR_GFIRE:		// ゲイツなにがしを放出
		pParent->GetEnemyExShot()->SetGFire(m_Cmd);
		m_pTCLCmd++;
	goto Start;

	case SCR_IONRING:		// イオンリング
		pParent->GetEnemyExShot()->SetIonRing(m_Cmd);
		m_pTCLCmd++;
	goto Start;

	case SCR_DEGE:	// 角度を親と同じにする
		m_Cmd.Angle = m_pParent->d;
		m_pTCLCmd++;
	goto Start;


	case SCR_DEGS:	// 角度を自機と同じにする
		sx = pParent->GetTargetX();		// Ｘ座標を取得
		sy = pParent->GetTargetY();		// Ｙ座標を取得
		m_Cmd.Angle = Atan8(sx-m_Cmd.ox, sy-m_Cmd.oy);
		m_pTCLCmd++;
	goto Start;

	case SCR_LLOPEN:
		w = *(int *)(&pCurrent[1]);
		pParent->GetEnemyLLaserPtr()->OnOpen(this, w);
		m_pTCLCmd += 5;
	goto Start;

	case SCR_LLCLOSE:
		pParent->GetEnemyLLaserPtr()->OnClose(this);
		m_pTCLCmd++;
	goto Start;

	case SCR_LLCHARGE:
		pParent->GetEnemyLLaserPtr()->OnCharge(this);
		m_pTCLCmd++;
	goto Start;

	case SCR_NOP:		// 停止命令 Cmd(1) + Time(2)
		// 命令の最初の読み込みならば、停止カウント数の取得 //
		// そうでなければ、値をデクリメントする             //
		if(0 == this->RepCount) this->RepCount = *(WORD *)(&pCurrent[1]);
		else                    this->RepCount--;

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			m_pTCLCmd += (1 + 2);	// Cmd(1) + Time(2)
			goto Start;			// それでは、次の命令も実行しましょう
		}
	return TRUE;		// 正常終了


	case SCR_PSE:
		PlaySE(pCurrent[1], m_Cmd.ox);
		m_pTCLCmd += (1 + 1);		// Cmd(1) + SndID(1)
	goto Start;


///////////////////////////////////////////////////////////////////////////////
//                       メンバ変数操作系命令                                //
//                                                                           //
//      SCR_PUSHR  :  [後] スタックにレジスタの内容を積む                    //
//      SCR_POPR   :  [後] レジスタにスタックの内容をコピー                  //
///////////////////////////////////////////////////////////////////////////////
	case TCL_REG_X:		// 弾発射を行う中心Ｘ座標
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_dx);
		else if(SCR_POPR  == pCurrent[1]) m_dx = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_Y:		// 弾発射を行う中心Ｙ座標
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_dy);
		else if(SCR_POPR  == pCurrent[1]) m_dy = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_CMD:	// 弾の発射形状(扇、ランダム等)
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Cmd);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Cmd = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_V:		// 弾の速度
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Speed);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Speed = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_A:		// 弾の加速度
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Accel);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Accel = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_DW:	// 弾の角度差
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.wAngle);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.wAngle = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_NDW:	// 連弾時の弾のズレ
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.NdAngle);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.NdAngle = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_N:		// 弾数
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Num);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Num = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_NS:	// 連弾数
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.NumS);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.NumS = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_D:		// 基本角
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Angle);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Angle = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_VD:	// 角速度
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.vAngle);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.vAngle = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_C:		// 弾の色＆形状
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Color);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Color = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_REP:	// 弾の繰り返しパラメータ
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Rep);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Rep = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_TYPE:	// 弾の種類(通常、ホーミング)
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Type);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Type = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_OPT:	// 弾のオプション(反射Ｘ等)
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Option);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Option = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_ANM:	// 弾のアニメーションスピード
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.AnmSpd);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.AnmSpd = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_L:		// レーザーの長さ
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Length);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Length = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_PARAM:		// ATK2 用パラメータ
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(m_Cmd.Param);
		else if(SCR_POPR  == pCurrent[1]) m_Cmd.Param = CalcStack::Pop();
		else goto Error;
		m_pTCLCmd += 2;
	goto Start;

	// 親の汎用レジスタ //
	case TCL_REG_PGR0: case TCL_REG_PGR1: case TCL_REG_PGR2: case TCL_REG_PGR3:
	case TCL_REG_PGR4: case TCL_REG_PGR5: case TCL_REG_PGR6: case TCL_REG_PGR7:
		if(NULL == m_pParent){
#ifdef PBG_DEBUG
			PbgError("親の存在しない AtkObj が汎用レジスタにアクセスしようとしました");
#endif
			return FALSE;
		}

		if(SCR_PUSHR == pCurrent[1]){
			CalcStack::Push(m_pParent->Gr[pCurrent[0]-TCL_REG_PGR0]);
		}
		else if(SCR_POPR  == pCurrent[1]){
			m_pParent->Gr[pCurrent[0]-TCL_REG_PGR0] = CalcStack::Pop();
		}
		else goto Error;

		m_pTCLCmd += 2;
	goto Start;

	// 親の角度 //
	case TCL_REG_PD:
		if(NULL == m_pParent){
#ifdef PBG_DEBUG
			PbgError("親の存在しない AtkObj が汎用レジスタにアクセスしようとしました");
#endif
			return FALSE;
		}

		if(SCR_PUSHR == pCurrent[1]){
			CalcStack::Push(m_pParent->d);
		}
		else if(SCR_POPR  == pCurrent[1]){
			m_pParent->d = CalcStack::Pop();
		}
		else goto Error;

		m_pTCLCmd += 2;
	goto Start;

	case TCL_REG_PARENT:
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push((DWORD)m_pParent);
		else if(SCR_POPR  == pCurrent[1]){
			PbgError("親ポインタへの代入は出来ません");
			return FALSE;
		}
		else goto Error;

		m_pTCLCmd += 2;
	goto Start;

	case SCR_MOVC:		// [中] レジスタに定数の代入を行う
		if(FALSE == MoveRegister(pCurrent)) goto Error;
		m_pTCLCmd += (1 + 1 + 4);	// cmd(1) + RegID(1) + Const(4)
	goto Start;


	case SCR_PUSHC:		// [前] 定数をスタックにＰＵＳＨする
		CalcStack::Push(*(int *)(&pCurrent[1]));
		m_pTCLCmd += (1 + 4);		// cmd(1) + Const(4)
	goto Start;



///////////////////////////////////////////////////////////////////////////////
//                    スクリプトがサポートすべき命令セット                   //
///////////////////////////////////////////////////////////////////////////////
	case SCR_TJMP:		// [前] POP して真ならばジャンプする
		if(CalcStack::Pop()){
			m_pTCLCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pTCLCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_FJMP:		// [前] POP して偽ならばジャンプする
		if(!(CalcStack::Pop())){
			m_pTCLCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pTCLCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_JMP:		// [前] 無条件ジャンプ
		m_pTCLCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
	goto Start;


	case SCR_OJMP:		// [前] POP して真ならPUSH(TRUE),  JMP
		if(CalcStack::Top()){
			m_pTCLCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pTCLCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_AJMP:		// [前] POP して偽ならPUSH(FALSE), JMP
		if(!(CalcStack::Top())){
			m_pTCLCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pTCLCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_EXIT:		// [前] 終了する
		// ここでポインタを進めると危険なので、何もしません //
	return FALSE;


	case SCR_LPOP:		// [前] 演算スタックトップ->JMP スタックトップ
		AStk_Push(CalcStack::Pop());
		m_pTCLCmd += 1;			// cmd(1)
	goto Start;


	case SCR_LJMP:		// [前] ０ならジャンプ、真ならデクリメント
		if(FALSE == AStk_DecTop()){
			m_pTCLCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			m_pTCLCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_ADD:		// [前] Push(Pop(1) + Pop(0))
		CalcStack::Add();
		m_pTCLCmd++;
	goto Start;

	case SCR_SUB:		// [前] Push(Pop(1) - Pop(0))
		CalcStack::Sub();
		m_pTCLCmd++;
	goto Start;

	case SCR_MUL:		// [前] Push(Pop(1) * Pop(0))
		CalcStack::Mul();
		m_pTCLCmd++;
	goto Start;

	case SCR_DIV:		// [前] Push(Pop(1) / Pop(0))
		CalcStack::Div();
		m_pTCLCmd++;
	goto Start;

	case SCR_MOD:		// [前] Push(Pop(1) % Pop(0))
		CalcStack::Mod();
		m_pTCLCmd++;
	goto Start;

	case SCR_NEG:		// [前] Push(-Pop(0))
		CalcStack::Neg();
		m_pTCLCmd++;
	goto Start;

	case SCR_SINL:		// [前] Push(sinl(Pop(1), Pop(0))
		CalcStack::SinL();
		m_pTCLCmd++;
	goto Start;

	case SCR_COSL:		// [前] Push(cosl(Pop(1), Pop(0))
		CalcStack::CosL();
		m_pTCLCmd++;
	goto Start;

	case SCR_RND:		// [前] Push(rnd() % Pop(0))
		CalcStack::Rnd();
		m_pTCLCmd++;
	goto Start;

	case SCR_ATAN:		// [前] Push(atan(Pop(1), Pop(0))
		CalcStack::Atan();
		m_pTCLCmd++;
	goto Start;

	case SCR_MAX:		// [前] Push( max(Pop(0), Pop(1)) )
		CalcStack::Max();
		m_pTCLCmd++;
	goto Start;

	case SCR_MIN:		// [前] Push( min(Pop(0), Pop(1)) )
		CalcStack::Min();
		m_pTCLCmd++;
	goto Start;

	case SCR_EQUAL:		// [前] Push(Pop(1) == Pop(0))
		CalcStack::Equal();
		m_pTCLCmd++;
	goto Start;

	case SCR_NOTEQ:		// [前] Push(Pop(1) != Pop(0))
		CalcStack::NotEq();
		m_pTCLCmd++;
	goto Start;

	case SCR_ABOVE:		// [前] Push(Pop(1) >  Pop(0))
		CalcStack::Above();
		m_pTCLCmd++;
	goto Start;

	case SCR_LESS:		// [前] Push(Pop(1) <  Pop(0))
		CalcStack::Less();
		m_pTCLCmd++;
	goto Start;

	case SCR_ABOVEEQ:	// [前] Push(Pop(1) >= Pop(0))
		CalcStack::AboveEq();
		m_pTCLCmd++;
	goto Start;

	case SCR_LESSEQ:	// [前] Push(Pop(1) <= Pop(0))
		CalcStack::LessEq();
		m_pTCLCmd++;
	goto Start;

	default:
	goto Error;
	}

Error:
	PbgError("ＴＣＬエラー : 未定義の命令コード or レジスタ");
	return FALSE;	// 失敗したので、終了要求を出力する
}


// 太レーザーを接続 [ LL 呼び出し用 ] //
FVOID EnemyAtkCtrl::ConnectLLaser(LLaserData *pLaser)
{
	m_pLLaser         = pLaser;
	pLaser->m_pTarget = this;
}


// 太レーザーの切り離し //
FVOID EnemyAtkCtrl::DisconnectLLaser(CEnemyLLaser *pLLaser)
{
	if(m_pLLaser){
		pLLaser->OnClose(this);
		m_pLLaser = NULL;
	}
}


// ポインタ取得 [ LL 呼び出し用 ] //
LLaserData *EnemyAtkCtrl::GetLLaserPtr(void)
{
	return m_pLLaser;
}


// ポインタ無効化 [ LL 呼び出し用 ] //
FVOID EnemyAtkCtrl::DisableLLaserPtr(void)
{
	m_pLLaser = NULL;
}


// レジスタに定数を代入する //
IBOOL EnemyAtkCtrl::MoveRegister(BYTE *pCmd)
{
	// BYTE pCmd[0] : SCR_MOVC
	// BYTE pCmd[1] : RegsID
	// int  pCmd[2] : Constant

	switch(pCmd[1]){
		case TCL_REG_X:		// 弾発射を行う中心Ｘ座標
			this->m_dx = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_Y:		// 弾発射を行う中心Ｙ座標
			this->m_dy = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_CMD:	// 弾の発射形状(扇、ランダム等)
			this->m_Cmd.Cmd = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_V:		// 弾の速度
			this->m_Cmd.Speed = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_A:		// 弾の加速度
			this->m_Cmd.Accel = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_DW:	// 弾の角度差
			this->m_Cmd.wAngle = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_NDW:	// 連弾時の弾のズレ
			this->m_Cmd.NdAngle = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_N:		// 弾数
			this->m_Cmd.Num = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_NS:	// 連弾数
				this->m_Cmd.NumS = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_D:		// 基本角
			this->m_Cmd.Angle = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_VD:	// 角速度
			this->m_Cmd.vAngle = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_C:		// 弾の色＆形状
			this->m_Cmd.Color = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_REP:	// 弾の繰り返しパラメータ
			this->m_Cmd.Rep = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_TYPE:	// 弾の種類(通常、ホーミング)
			this->m_Cmd.Type = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_OPT:	// 弾のオプション(反射Ｘ等)
			this->m_Cmd.Option = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_ANM:	// 弾のアニメーションスピード
			this->m_Cmd.AnmSpd = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_L:		// レーザーの長さ
			this->m_Cmd.Length = *(int *)(&pCmd[2]);
		return TRUE;

		case TCL_REG_PARAM:		// ATK2 用パラメータ
			this->m_Cmd.Param = *(int *)(&pCmd[2]);
		return TRUE;

		// 親の汎用レジスタ //
		case TCL_REG_PGR0: case TCL_REG_PGR1: case TCL_REG_PGR2: case TCL_REG_PGR3:
		case TCL_REG_PGR4: case TCL_REG_PGR5: case TCL_REG_PGR6: case TCL_REG_PGR7:
			if(this->m_pParent){
				this->m_pParent->Gr[pCmd[1] - TCL_REG_PGR0] = *(int *)(&pCmd[2]);
			}
#ifdef PBG_DEBUG
			else{
				PbgError("親の存在しない AtkObj が汎用レジスタに代入しようとしました");
				return FALSE;
			}
#endif
		return TRUE;

		case TCL_REG_PD:
			if(this->m_pParent){
				this->m_pParent->d = *(int *)(&pCmd[2]);
			}
#ifdef PBG_DEBUG
			else{
				PbgError("親の存在しない AtkObj が汎用レジスタに代入しようとしました");
				return FALSE;
			}
#endif
		return TRUE;

		case TCL_REG_PARENT:
#ifdef PBG_DEBUG
			PbgError("親ポインタへの代入は許されません");
#endif
		return TRUE;

		default:
			PbgError("EnemyData::MoveRegister() : 不正なレジスタＩＤ");
		return FALSE;
	}
}


// スタックに積む //
IBOOL EnemyAtkCtrl::AStk_Push(int Value)
{
	if(this->CallSP >= ATTACK_CSTK_MAX){
		PbgError("EnemyData::AStk_Push() : スタックオーバーフロー");
		return FALSE;
	}

	this->CallStack[this->CallSP] = Value;	// 値を積んで
	this->CallSP++;							// スタックポインタを移動

	return TRUE;
}


// スタックから取り出し //
IINT EnemyAtkCtrl::AStk_Pop(void)
{
	if(this->CallSP <= 0){
		PbgError("EnemyData::AStk_Pop() : スタックアンダーフロー");
		return 0;
	}

	this->CallSP--;							// スタックポインタを移動
	return this->CallStack[this->CallSP];	// スタックトップを返す
}


// ｎ回Popして、最後に取り出した値を返す //
IINT EnemyAtkCtrl::AStk_PopN(int n)
{
	if(n > this->CallSP){
		PbgError("EnemyData::AStk_PopN() : スタックアンダーフロー");
		return 0;
	}

	this->CallSP -= n;
	return this->CallStack[this->CallSP];	// スタックトップを返す
}


// スタックトップをデクリメント                       //
// トップが０ならば POP して FALSE を返し、           //
//       非０ならば DEC して TRUE  を返す             //
IINT EnemyAtkCtrl::AStk_DecTop(void)
{
	int temp;
	int	Index;

	// スタックトップは CallSP - 1 の指す要素である //
	Index = this->CallSP - 1;

	temp = this->CallStack[Index];
	if(0 == temp){
		this->CallSP = Index;
		return FALSE;
	}

	this->CallStack[Index]--;

	return TRUE;
}
