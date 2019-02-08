/*
 *   PbgCPU.cpp   : ＣＰＵ情報取得
 *
 */

#include "PbgCPU.h"



namespace Pbg {



/***** [スタティックメンバ] *****/

BOOL CCPUCheck::m_bEnableMMX;		// MMX が使用できれば真



/***** [グローバル変数] *****/

static CCPUCheck	g_CPUCheck;



// コンストラクタ //
CCPUCheck::CCPUCheck()
{
	BOOL		Flag;

	// ＭＭＸ命令が使用できるかどうかを調べる //
	_asm{
		PUSHFD
		POP		EAX
		MOV		EBX, EAX
		XOR		EAX, 0x200000
		PUSH	EAX
		POPFD
		PUSHFD
		POP		EAX
		CMP		EAX, EBX
		JZ		DISABLE_MMX		; 486 以前のＣＰＵだと厳しすぎます

		MOV		EAX, 1			; Version 情報の取得
		CPUID					; VC++6.0 でサポートされた
		AND		EDX, 0x800000	; ＭＭＸのサポートビットを調べる
		JZ		DISABLE_MMX		; 駄目

		MOV		Flag, TRUE		; 真をセット
		JMP		CHECK_EXIT		; 終了

	DISABLE_MMX:	// ＭＭＸ命令は使用できない
		MOV		Flag, FALSE

	CHECK_EXIT:		// チェック終了
	}

	m_bEnableMMX = Flag;
}


// デストラクタ //
CCPUCheck::~CCPUCheck()
{
	// 特に何もする事がない //
}



} // namespace PBG
