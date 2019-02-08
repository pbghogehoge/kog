/*
 *   Enemy.cpp   : 敵管理構造体
 *
 */

#include "CEnemyAnime.h"
#include "Enemy.h"
#include "CalcStack.h"
#include "ScrBase.h"
#include "ECL.h"
#include "SCL.h"
#include "SECtrl.h"



// ＥＣＬ命令を１フレーム分だけデコードする  //
// 戻り値 : FALSE この敵を削除する必要がある //
FBOOL EnemyData::ParseECLCmd(CEnemyCtrl *pParent, BYTE TargetList)
{
	EnemyData	*pChInfo;
	BYTE		*pCurrent;
	BYTE		angle, child;
	DWORD		param;
	int			temp, i;
	int			dx, dy;

	// 一応、スタックの初期化を行う //
	CalcStack::Reset();

	// スクリプト解析系の関数に限り、goto の使用を許可する //
Start:
#ifdef PBG_DEBUG
	if(GetAsyncKeyState(VK_F11) & 0x8000){
		char	buf[1024];
		wsprintf(buf,	"ECL 解析中の停止要求を受理します\r\n"
			"thisPtr(%u)  Offset(%u)", this, CSCLDecoder::Ptr2Offset(pCmd));
		PbgLog(buf);
		return FALSE;
	}
#endif

	pCurrent = pCmd;

	switch(pCurrent[0]){
///////////////////////////////////////////////////////////////////////////////
//                       ＥＣＬ固有の命令セット                              //
///////////////////////////////////////////////////////////////////////////////
	case SCR_CALL:		// プロシージャを呼び出す     Cmd(1) + Addr(4)
		AStk_Push(CSCLDecoder::Ptr2Offset(pCurrent + 1 + 4));
		pCmd = CSCLDecoder::Offset2Ptr(*(int *)(pCurrent + 1));
	goto Start;

	case SCR_ATK:		// 弾発射スレッドを発生させる Cmd(1) + dx(2) + dy(2) + Addr(4)
		dx   = *(short *)(&pCurrent[1+0]);	// ｘ座標・差分初期値
		dy   = *(short *)(&pCurrent[1+2]);	// ｙ座標・差分初期値
		temp = *(int   *)(&pCurrent[1+4]);	// 命令開始オフセット

		pParent->SetAtkObject(this, dx<<8, dy<<8, CSCLDecoder::Offset2Ptr(temp), 0);

		pCmd += (1 + 2 + 2 + 4);	// Cmd(1) + dx(2) + dy(2) + Addr(4)
	goto Start;

	case SCR_ATK2:		// 弾発射スレッドを発生させる Cmd(1) + dx(2) + dy(2) + Param(4) + Addr(4)
		dx    = *(short *)(&pCurrent[1+0]);	// ｘ座標・差分初期値
		dy    = *(short *)(&pCurrent[1+2]);	// ｙ座標・差分初期値
		param = *(DWORD *)(&pCurrent[1+4]);	// 汎用パラメータ
		temp  = *(int   *)(&pCurrent[1+8]);	// 命令開始オフセット

		pParent->SetAtkObject(this, dx<<8, dy<<8, CSCLDecoder::Offset2Ptr(temp), param);

		pCmd += (1 + 2 + 2 + 4 + 4);	// Cmd(1) + dx(2) + dy(2) + Addr(4)
	goto Start;

	case SCR_ATKNP:		// 親無し攻撃オブジェクト作成
		temp = *(int *)(&pCurrent[1+0]);	// 命令開始オフセット

		pParent->SetAtkObject(NULL, this->x, this->y, CSCLDecoder::Offset2Ptr(temp), 0);

		pCmd += (1 + 4);	// Cmd(1) + Addr(4)
	goto Start;

	case SCR_FATK:
		temp = *(int *)(&pCurrent[1]);
		pFinalAttack = CSCLDecoder::Offset2Ptr(temp);
		pCmd += (1 + 4);	// Cmd(1) + Addr(4)
	goto Start;

	case SCR_KILL:
#pragma message(__FILE__"こんなんで、いいんか？")
		if(!(this->Flag & EFLG_HIT_ON)) this->size /= 2;

		// 殺しフラグを立てる //
		this->Flag |= (EFLG_KILL | EFLG_HIT_ON);
		this->hp    = 0;
	return FALSE;

	case SCR_ESET:		// 敵セット                   Cmd(1) + Addr(4)
		temp = *(int *)(&pCurrent[1]);
		pParent->SetEnemy(this->x, this->y, temp, this->Gr[0], TargetList);
		pCmd += (1 + 4);
	goto Start;

	case SCR_CHILD:		// 子をセット Cmd(1) + ID(1) + Addr(4)
		child = pCurrent[1];
		temp = *(int *)(&pCurrent[2]);

		pParent->SetChild(temp, this, this->Gr[0], child, TargetList);

		pCmd += (1 + 1 + 4);
	goto Start;

	case SCR_CHGTASK:		// タスクを切り替える Cmd(1) + ID(1) + Addr(4)
		child = pCurrent[1];
		temp = *(int *)(&pCurrent[2]);

		pChInfo = this->pChild[child];
		if(pChInfo){	// 子が生きている場合のみ実行
			pChInfo->pCmd     = CSCLDecoder::Offset2Ptr(temp);
			pChInfo->Count    = 0;
			pChInfo->CallSP   = 0;
			pChInfo->RepCount = 0;
		}

		pCmd += (1 + 1 + 4);
	goto Start;

	case SCR_WAITATOBJ:	// 攻撃オブジェクト完了待ち
		if(pParent->IsExistAtkObj(this)){
			return TRUE;
		}
		else{
			pCmd++;
		}
	goto Start;

	case SCR_RET:		// プロシージャからの復帰     Cmd(1) + pop(1)
		temp = AStk_PopN(pCurrent[1] + 1);	// +1 は CALL 時に Push した分ね
		if(temp <= 0) goto Error;

		pCmd = CSCLDecoder::Offset2Ptr(temp);
	goto Start;


	case SCR_ANM:		// アニメーション設定  Cmd(1) + Ptn(1) + Spd(1)
	{
		EAnimePtn		*ptn;

		ptn = CEnemyAnimeContainer::GetPointer(pCurrent[1]);
		if(ptn != this->pAnimePtn){
			this->pAnimePtn = ptn;
			this->AnmCount  = 0;
			this->Count     = 0;
		}

		this->AnmSpd    = (char)pCurrent[1 + 1];

		pCmd += (1 + 1 + 1);	// Cmd(1) + Ptn(1) + Spd(1);
	}
	goto Start;


	case SCR_PSE:
		PlaySE(pCurrent[1], this->x);
		pCmd += (1 + 1);		// Cmd(1) + SndID(1)
	goto Start;

	case SCR_EFC:	// エフェクト発生  Cmd(1) + dx(2) + dy(2) + type(1)
		dx   = this->x + (*(short *)&pCurrent[1 + 0]);
		dy   = this->y + (*(short *)&pCurrent[1 + 2]);
		temp = pCurrent[1 + 4];

		switch(temp){
		case FRG_POINT:
			pParent->GetFragmentEfcPtr()->SetPointEfc(dx, dy, 1);
		break;

		case FRG_SMOKE_W:
			pParent->GetFragmentEfcPtr()->SetSmokeEfc(dx, dy, FRG_SMOKE_W);
		break;

		case FRG_SMOKE_P:
			pParent->GetFragmentEfcPtr()->SetSmokeEfc(dx, dy, FRG_SMOKE_P);
		break;

		default:
#ifdef PBG_DEBUG
		PbgError("SCR_EFC : 未実装のエフェクトＩＤでっせ");
#endif
		break;
		}

		pCmd += (1 + 2 + 2 + 1);
	goto Start;

	case SCR_MDMG:	// マッドネスゲージにダメージ
		temp = *(WORD *)(&pCmd[1]);
		pParent->MadnessGaugeDamage(temp);
		pCmd += (1 + 2);
	goto Start;


	case SCR_NOP:		// 停止命令 Cmd(1) + Time(2)
		// 命令の最初の読み込みならば、停止カウント数の取得 //
		// そうでなければ、値をデクリメントする             //
		if(0 == this->RepCount) this->RepCount = *(WORD *)(&pCurrent[1]);
		else                    this->RepCount--;

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2);	// Cmd(1) + Time(2)
			goto Start;			// それでは、次の命令も実行しましょう
		}
	return TRUE;		// 正常終了


	// 停止命令 親同期 Cmd(1) + Time(2) //
	// break (gr6, gr7)                 //
	case SCR_PNOP:
		// 命令の最初の読み込みならば、停止カウント数の取得 //
		// そうでなければ、値をデクリメントする             //
		if(0 == this->RepCount){
			this->RepCount = *(WORD *)(&pCurrent[1]);
			this->Gr[6]    = this->x - this->pParent->x;
			this->Gr[7]    = this->y - this->pParent->y;
		}
		else{
			this->RepCount--;
			this->x = this->pParent->x + this->Gr[6];
			this->y = this->pParent->y + this->Gr[7];
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2);	// Cmd(1) + Time(2)
			goto Start;			// それでは、次の命令も実行しましょう
		}
	return TRUE;		// 正常終了


	// 直線移動     Cmd(1) + v0(2) + t(2) //
	// Uses RepCount / Break (v, vx, vy)  //
	// 速度代入も兼ねている点に注意すべし //
	case SCR_MOV:
		// 命令の最初の読み込みならば、直進カウント数の取得 //
		// そうでなければ、値をデクリメントし、直進する     //
		if(0 == this->RepCount){
			temp = this->v = *(short *)(&pCurrent[1+0]);	// 初速度
			this->RepCount = *(WORD  *)(&pCurrent[1+2]);	// 繰り返し回数

			angle    = this->d;				// 移動する方向
			this->vx = CosL(angle, temp);	// 速度のＸ成分
			this->vy = SinL(angle, temp);	// 速度のＹ成分
		}
		else{
			this->RepCount--;		// 残り時間の減少

			this->x += this->vx;	// Ｘ方向の移動
			this->y += this->vy;	// Ｙ方向の移動
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2 + 2);	// Cmd(1) + v0(2) + t(2)
			goto Start;				// 次の命令を実行する
		}
	return TRUE;


	// 直線移動 親同期    Cmd(1) + v0(2) + t(2)    //
	// Uses RepCount / Break (v, vx, vy, gr6, gr7) //
	// 速度代入も兼ねている点に注意すべし          //
	case SCR_PMOV:
		// 命令の最初の読み込みならば、直進カウント数の取得 //
		// そうでなければ、値をデクリメントし、直進する     //
		if(0 == this->RepCount){
			temp = this->v = *(short *)(&pCurrent[1+0]);	// 初速度
			this->RepCount = *(WORD  *)(&pCurrent[1+2]);	// 繰り返し回数

			angle    = this->d;				// 移動する方向
			this->vx = CosL(angle, temp);	// 速度のＸ成分
			this->vy = SinL(angle, temp);	// 速度のＹ成分

			this->Gr[6] = this->x - this->pParent->x;
			this->Gr[7] = this->y - this->pParent->y;
		}
		else{
			this->RepCount--;		// 残り時間の減少

			this->Gr[6] += this->vx;
			this->Gr[7] += this->vy;

			this->x = this->Gr[6] + this->pParent->x;	// Ｘ方向の移動
			this->y = this->Gr[7] + this->pParent->y;	// Ｙ方向の移動
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2 + 2);	// Cmd(1) + v0(2) + t(2)
			goto Start;				// 次の命令を実行する
		}
	return TRUE;


	// 加速付き移動 Cmd(1) + v0(2) + a(2)  + t(2) //
	// Uses RepCount / Break (v, a)               //
	case SCR_ACC:
		// 命令の最初の読み込みならば、直進カウント数の取得 //
		// そうでなければ、値をデクリメントして、加速する   //
		if(0 == this->RepCount){
			this->v        = *(short *)(&pCurrent[1+0]);	// 初速度
			this->a        = *(short *)(&pCurrent[1+2]);	// 初速度
			this->RepCount = *(WORD  *)(&pCurrent[1+4]);	// 繰り返し回数
		}
		else{
			this->RepCount--;				// 残り時間の減少

			// 加速を反映する //
			temp  = this->v;	//
			angle = this->d;	//

			this->v += this->a;
			this->x += CosL(angle, temp);	// Ｘ方向の移動
			this->y += SinL(angle, temp);	// Ｙ方向の移動
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2 + 2 + 2);	// Cmd(1) + v0(2) + a(2) + t(2)
			goto Start;				// 次の命令を実行する
		}
	return TRUE;


	// 加速付き移動 親同期    Cmd(1) + v0(2) + a(2)  + t(2) //
	// Uses RepCount / Break (v, a, gr6, gr7)               //
	case SCR_PACC:
		// 命令の最初の読み込みならば、直進カウント数の取得 //
		// そうでなければ、値をデクリメントして、加速する   //
		if(0 == this->RepCount){
			this->v        = *(short *)(&pCurrent[1+0]);	// 初速度
			this->a        = *(short *)(&pCurrent[1+2]);	// 初速度
			this->RepCount = *(WORD  *)(&pCurrent[1+4]);	// 繰り返し回数

			this->Gr[6] = this->x - this->pParent->x;
			this->Gr[7] = this->y - this->pParent->y;
		}
		else{
			this->RepCount--;				// 残り時間の減少

			// 加速を反映する //
			temp  = this->v;	//
			angle = this->d;	//

			this->v += this->a;
			this->Gr[6] += CosL(angle, temp);	// Ｘ方向の差分値更新
			this->Gr[7] += SinL(angle, temp);	// Ｙ方向の差分値更新

			this->x = this->Gr[6] + this->pParent->x;	// Ｘ方向の移動
			this->y = this->Gr[7] + this->pParent->y;	// Ｙ方向の移動
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2 + 2 + 2);	// Cmd(1) + v0(2) + a(2) + t(2)
			goto Start;				// 次の命令を実行する
		}
	return TRUE;


	// 回転付き移動 Cmd(1) + v0(2) + vd(1) + t(2) //
	// Uses RepCount / Break (v, vd)              //
	case SCR_ROL:
		// 命令の最初の読み込みならば、直進カウント数の取得 //
		// そうでなければ、値をデクリメントする             //
		if(0 == this->RepCount){
			this->v        = *(short *)(&pCurrent[1+0]);	// 初速度
			this->vd       = *(char  *)(&pCurrent[1+2]);	// 角速度
			this->RepCount = *(WORD  *)(&pCurrent[1+3]);	// 繰り返し回数

			// 左右反転フラグが立っているか？ //
			if(this->Flag & EFLG_RLCHG_ON){
				if(this->x < pParent->GetMidX()){	// 中心より左側
					this->vd = -(this->vd);
				}
			}
		}
		else{
			this->RepCount--;			// 残り時間の減少

			// 回転を反映する //
			angle = this->d;
			temp  = this->v;

			this->d += this->vd;
			this->x += CosL(angle, temp);	// Ｘ方向の移動
			this->y += SinL(angle, temp);	// Ｙ方向の移動
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2 + 1 + 2);	// Cmd(1) + v0(2) + vd(1) + t(2)
			goto Start;					// 次の命令を実行する
		}
	return TRUE;


	// 回転付き移動 親同期  Cmd(1) + v0(2) + vd(1) + t(2) //
	// Uses RepCount / Break (v, vd, gr6, gr7)            //
	case SCR_PROL:
		// 命令の最初の読み込みならば、直進カウント数の取得 //
		// そうでなければ、値をデクリメントする             //
		if(0 == this->RepCount){
			this->v        = *(short *)(&pCurrent[1+0]);	// 初速度
			this->vd       = *(char  *)(&pCurrent[1+2]);	// 角速度
			this->RepCount = *(WORD  *)(&pCurrent[1+3]);	// 繰り返し回数

			this->Gr[6] = this->x - this->pParent->x;
			this->Gr[7] = this->y - this->pParent->y;

			// 左右反転フラグが立っているか？ //
			if(this->Flag & EFLG_RLCHG_ON){
				if(this->x < pParent->GetMidX()){	// 中心より左側
					this->vd = -(this->vd);
				}
			}
		}
		else{
			this->RepCount--;			// 残り時間の減少

			// 回転を反映する //
			angle = this->d;
			temp  = this->v;

			this->d += this->vd;
			this->Gr[6] += CosL(angle, temp);	// Ｘ方向差分の更新
			this->Gr[7] += SinL(angle, temp);	// Ｙ方向差分の更新

			this->x = this->Gr[6] + this->pParent->x;	// Ｘ方向の移動
			this->y = this->Gr[7] + this->pParent->y;	// Ｙ方向の移動
		}

		// 終了条件を満たしている場合 //
		if(0 == this->RepCount){
			pCmd += (1 + 2 + 1 + 2);	// Cmd(1) + v0(2) + vd(1) + t(2)
			goto Start;					// 次の命令を実行する
		}
	return TRUE;


	case SCR_DEGS:		// 進行方向を自機の方向にセットする
		dx      = pParent->GetTargetX();				// Ｘ座標を取得
		dy      = pParent->GetTargetY();				// Ｙ座標を取得
		this->d = Atan8(dx-(this->x), dy-(this->y));
		pCmd++;
	goto Start;


///////////////////////////////////////////////////////////////////////////////
//                       メンバ変数操作系命令                                //
//                                                                           //
//      SCR_PUSHR  :  [後] スタックにレジスタの内容を積む                    //
//      SCR_POPR   :  [後] レジスタにスタックの内容をコピー                  //
//      SCR_PARENT :  [後] 親レジスタをスタックに積む                        //
///////////////////////////////////////////////////////////////////////////////
	case ECL_REG_X:		// 現在のＸ座標
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->x);
		else if(SCR_POPR  == pCurrent[1]) this->x = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->x);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_Y:		// 現在のＹ座標
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->y);
		else if(SCR_POPR  == pCurrent[1]) this->y = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->y);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_V:		// 速度
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->v);
		else if(SCR_POPR  == pCurrent[1]) this->v = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->v);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_HP:	// ＨＰ残量
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->hp);
		else if(SCR_POPR  == pCurrent[1]) this->hp = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->hp);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_COUNT:	// カウンタ(できればReadOnlyとしたいが...)
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Count);
		else if(SCR_POPR  == pCurrent[1]) this->Count = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Count);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_SCORE:	// スコア
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Score);
		else if(SCR_POPR  == pCurrent[1]) this->Score = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Score);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR0:	// 汎用レジスタ０番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[0]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[0] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[0]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR1:	// 汎用レジスタ１番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[1]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[1] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[1]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR2:	// 汎用レジスタ２番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[2]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[2] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[2]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR3:	// 汎用レジスタ３番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[3]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[3] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[3]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR4:	// 汎用レジスタ４番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[4]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[4] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[4]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR5:	// 汎用レジスタ５番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[5]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[5] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[5]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR6:	// 汎用レジスタ６番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[6]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[6] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[6]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_GR7:	// 汎用レジスタ７番
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Gr[7]);
		else if(SCR_POPR  == pCurrent[1]) this->Gr[7] = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Gr[7]);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_FLAG:	// 属性フラグ
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Flag);
		else if(SCR_POPR  == pCurrent[1]) this->Flag = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Flag);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_D:		// 進行方向
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->d);
		else if(SCR_POPR  == pCurrent[1]) this->d = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->d);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_ALPHA:		// α値
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(this->Alpha);
		else if(SCR_POPR  == pCurrent[1]) this->Alpha = CalcStack::Pop();
		else if(SCR_PARENT== pCurrent[1]) CalcStack::Push(this->pParent->Alpha);
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_XMID:
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(pParent->GetMidX());
		else if(SCR_POPR  == pCurrent[1]){
			PbgError("定数レジスタ X_MID への POP 要求");
			return FALSE;
		}
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_MX:
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(pParent->GetTargetX());
		else if(SCR_POPR  == pCurrent[1]){
			PbgError("定数レジスタ mx への POP 要求");
			return FALSE;
		}
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_MY:
		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(pParent->GetTargetY());
		else if(SCR_POPR  == pCurrent[1]){
			PbgError("定数レジスタ my への POP 要求");
			return FALSE;
		}
		else goto Error;
		pCmd += 2;
	goto Start;

	case ECL_REG_NUMCHILD:
		for(i=temp=0; i<ENEMY_CHILD_MAX; i++){
			if(this->pChild[i]) temp++;
		}

		if(     SCR_PUSHR == pCurrent[1]) CalcStack::Push(temp);
		else if(SCR_POPR  == pCurrent[1]){
			PbgError("定数レジスタ numchild への POP 要求");
			return FALSE;
		}
		else goto Error;
		pCmd += 2;
	goto Start;

	case SCR_MOVC:		// [中] レジスタに定数の代入を行う
		if(FALSE == MoveRegister(pCurrent)) goto Error;
		pCmd += (1 + 1 + 4);	// cmd(1) + RegID(1) + Const(4)
	goto Start;


	case SCR_PUSHC:		// [前] 定数をスタックにＰＵＳＨする
		CalcStack::Push(*(int *)(&pCurrent[1]));
		pCmd += (1 + 4);		// cmd(1) + Const(4)
	goto Start;



///////////////////////////////////////////////////////////////////////////////
//                    スクリプトがサポートすべき命令セット                   //
///////////////////////////////////////////////////////////////////////////////
	case SCR_TJMP:		// [前] POP して真ならばジャンプする
		if(CalcStack::Pop()){
			pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_FJMP:		// [前] POP して偽ならばジャンプする
		if(!(CalcStack::Pop())){
			pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_JMP:		// [前] 無条件ジャンプ
		pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
	goto Start;


	case SCR_OJMP:		// [前] POP して真ならPUSH(TRUE),  JMP
		if(CalcStack::Top()){
			pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_AJMP:		// [前] POP して偽ならPUSH(FALSE), JMP
		if(!(CalcStack::Top())){
			pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_EXIT:		// [前] 終了する
		// ここでポインタを進めると危険なので、何もしません //
	return FALSE;


	case SCR_LPOP:		// [前] 演算スタックトップ->JMP スタックトップ
		AStk_Push(CalcStack::Pop());
		pCmd += 1;			// cmd(1)
	goto Start;


	case SCR_LJMP:		// [前] ０ならジャンプ、真ならデクリメント
		if(FALSE == AStk_DecTop()){
			pCmd = CSCLDecoder::Offset2Ptr(*(int *)(&pCurrent[1]));
		}
		else{
			pCmd += (1 + 4);	// cmd(1) + AddrOffset(4)
		}
	goto Start;


	case SCR_ADD:		// [前] Push(Pop(1) + Pop(0))
		CalcStack::Add();
		pCmd++;
	goto Start;

	case SCR_SUB:		// [前] Push(Pop(1) - Pop(0))
		CalcStack::Sub();
		pCmd++;
	goto Start;

	case SCR_MUL:		// [前] Push(Pop(1) * Pop(0))
		CalcStack::Mul();
		pCmd++;
	goto Start;

	case SCR_DIV:		// [前] Push(Pop(1) / Pop(0))
		CalcStack::Div();
		pCmd++;
	goto Start;

	case SCR_MOD:		// [前] Push(Pop(1) % Pop(0))
		CalcStack::Mod();
		pCmd++;
	goto Start;

	case SCR_NEG:		// [前] Push(-Pop(0))
		CalcStack::Neg();
		pCmd++;
	goto Start;

	case SCR_SINL:		// [前] Push(sinl(Pop(1), Pop(0))
		CalcStack::SinL();
		pCmd++;
	goto Start;

	case SCR_COSL:		// [前] Push(cosl(Pop(1), Pop(0))
		CalcStack::CosL();
		pCmd++;
	goto Start;

	case SCR_RND:		// [前] Push(rnd() % Pop(0))
		CalcStack::Rnd();
		pCmd++;
	goto Start;

	case SCR_ATAN:		// [前] Push(atan(Pop(1), Pop(0))
		CalcStack::Atan();
		pCmd++;
	goto Start;

	case SCR_MAX:		// [前] Push( max(Pop(0), Pop(1)) )
		CalcStack::Max();
		pCmd++;
	goto Start;

	case SCR_MIN:		// [前] Push( min(Pop(0), Pop(1)) )
		CalcStack::Min();
		pCmd++;
	goto Start;

	case SCR_EQUAL:		// [前] Push(Pop(1) == Pop(0))
		CalcStack::Equal();
		pCmd++;
	goto Start;

	case SCR_NOTEQ:		// [前] Push(Pop(1) != Pop(0))
		CalcStack::NotEq();
		pCmd++;
	goto Start;

	case SCR_ABOVE:		// [前] Push(Pop(1) >  Pop(0))
		CalcStack::Above();
		pCmd++;
	goto Start;

	case SCR_LESS:		// [前] Push(Pop(1) <  Pop(0))
		CalcStack::Less();
		pCmd++;
	goto Start;

	case SCR_ABOVEEQ:	// [前] Push(Pop(1) >= Pop(0))
		CalcStack::AboveEq();
		pCmd++;
	goto Start;

	case SCR_LESSEQ:	// [前] Push(Pop(1) <= Pop(0))
		CalcStack::LessEq();
		pCmd++;
	goto Start;

	default:
	goto Error;
	}

Error:
	PbgError("ＥＣＬエラー : 未定義の命令コード or レジスタ");
	return FALSE;	// 失敗したので、終了要求を出力する
}


// レジスタに定数を代入する //
IBOOL EnemyData::MoveRegister(BYTE *pCmd)
{
	// BYTE pCmd[0] : SCR_MOVC
	// BYTE pCmd[1] : RegsID
	// int  pCmd[2] : Constant

	switch(pCmd[1]){
		case ECL_REG_X:		// 現在のＸ座標
			this->x = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_Y:		// 現在のＹ座標
			this->y = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_V:		// 速度
			this->v = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_HP:	// ＨＰ残量
			this->hp = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_COUNT:	// カウンタ(できればReadOnlyとしたいが...)
			this->Count = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_SCORE:	// スコア
			this->Score = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR0:	// 汎用レジスタ０番
			this->Gr[0] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR1:	// 汎用レジスタ１番
			this->Gr[1] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR2:	// 汎用レジスタ２番
			this->Gr[2] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR3:	// 汎用レジスタ３番
			this->Gr[3] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR4:	// 汎用レジスタ４番
			this->Gr[4] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR5:	// 汎用レジスタ５番
			this->Gr[5] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR6:	// 汎用レジスタ６番
			this->Gr[6] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_GR7:	// 汎用レジスタ７番
			this->Gr[7] = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_FLAG:	// 属性フラグ
			this->Flag = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_D:		// 進行方向
			this->d = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_ALPHA:	// α値
			this->Alpha = *(int *)(&pCmd[2]);
		return TRUE;

		case ECL_REG_XMID:
			PbgError("EnemyData::MoveRegister() : 定数レジスタ(X_MID) への代入");
		return FALSE;

		case ECL_REG_MX:
			PbgError("EnemyData::MoveRegister() : 定数レジスタ(mx) への代入");
		return FALSE;

		case ECL_REG_MY:
			PbgError("EnemyData::MoveRegister() : 定数レジスタ(my) への代入");
		return FALSE;

		case ECL_REG_NUMCHILD:
			PbgError("EnemyData::MoveRegister() : 定数レジスタ(numchild) への代入");
		return FALSE;

		default:
			PbgError("EnemyData::MoveRegister() : 不正なレジスタＩＤ");
		return FALSE;
	}
}


// アドレススタック関連(スタックに積む) //
IBOOL EnemyData::AStk_Push(int Value)
{
	if(this->CallSP >= ENEMY_CSTK_MAX){
		PbgError("EnemyData::AStk_Push() : スタックオーバーフロー");
		return FALSE;
	}

	this->CallStack[this->CallSP] = Value;	// 値を積んで
	this->CallSP++;							// スタックポインタを移動

	return TRUE;
}


// アドレススタック関連(スタックから取り出し) //
IINT EnemyData::AStk_Pop(void)
{
	if(this->CallSP <= 0){
		PbgError("EnemyData::AStk_Pop() : スタックアンダーフロー");
		return 0;
	}

	this->CallSP--;							// スタックポインタを移動
	return this->CallStack[this->CallSP];	// スタックトップを返す
}


// ｎ回Popして、最後に取り出した値を返す //
IINT EnemyData::AStk_PopN(int n)
{
	if(n > this->CallSP){
		PbgError("EnemyData::AStk_PopN() : スタックアンダーフロー");
		return 0;
	}

	this->CallSP -= n;
	return this->CallStack[this->CallSP];	// スタックトップを返す
}


// アドレススタック関連(スタックトップをデクリメント) //
// トップが０ならば POP して FALSE を返し、           //
//       非０ならば DEC して TRUE  を返す             //
IBOOL EnemyData::AStk_DecTop(void)
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
