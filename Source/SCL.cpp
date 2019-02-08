/*
 *   SCL.cpp   : 敵配置スクリプト
 *
 */

#include "PbgMem.h"				// メモリ管理
#include "LzUty\\LzDecode.h"
#include "PbgError.h"			// エラー吐き出し

#include "SCL.h"			// ＳＣＬ定数
#include "ScrBase.h"		// スクリプト定数
#include "CEnemyAnime.h"	// 敵のアニメーション管理
#include "CAtkGrpDraw.h"	// アニメーション描画



/***** [スタティックメンバ] *****/
SCLHeader *CSCLDecoder::m_pDataBuffer = NULL;	// データ格納地点(Player間で共有)
DWORD      CSCLDecoder::m_RefCount    = 0;		// 参照カウント
int        CSCLDecoder::m_Level1Exit;			// Level 1 脱出のタイミング
int        CSCLDecoder::m_Level2Exit;			// Level 2 脱出のタイミング
int        CSCLDecoder::m_Level3Exit;			// Level 3 脱出のタイミング


// コンストラクタ //
CSCLDecoder::CSCLDecoder(RECT *rcTargetX256, CEnemyCtrl *pEnemyCtrl)
{
	// 現在のＳＣＬアドレスを初期化する                       //
	// すでにデータがロードされている場合は、そいつに合わせる //
	if(m_pDataBuffer) m_pCurrentSCL = (BYTE *)(m_pDataBuffer + 1);
	else              m_pCurrentSCL = NULL;

	m_NopCount = 0;		// ＮＯＰ命令の待ち時間
	m_Level    = 0;		// 現在のレベル
	m_Target   = 0;		// 現在のレベルの進行状況
	m_Count    = 0;		// 経過フレーム数

	m_RefCount += 1;

	// 敵の配置における中心座標を求める //
	m_ox = (rcTargetX256->right  + rcTargetX256->left) / 2;	// ｘ座標中心
	m_oy = 0;//(rcTargetX256->bottom + rcTargetX256->top)  / 2;	// ｙ座標中心

	// 敵管理クラスに接続する //
	m_pEnemyCtrl = pEnemyCtrl;
}


// デストラクタ //
CSCLDecoder::~CSCLDecoder()
{
	m_RefCount -= 1;

	// 参照カウンタが０になった場合 //
	if(0 == m_RefCount){
		// バッファが存在していれば、データを解放する //
		if(NULL != m_pDataBuffer) MemFree(m_pDataBuffer);
	}
}


// バッファ内のデータをリセットする //
BOOL CSCLDecoder::Initialize(int Level)
{
	// バッファが生成されていない場合は失敗 //
	if(NULL == m_pDataBuffer) return FALSE;

	// ＳＣＬ先頭領域までジャンプする //
	m_pCurrentSCL = (BYTE *)m_pDataBuffer;

	// レベルにより開始アドレスが変化する //
	switch(Level){
		case 1:		m_pCurrentSCL += m_pDataBuffer->SCL_Lv1[0];		break;
		case 2:		m_pCurrentSCL += m_pDataBuffer->SCL_Lv2[0];		break;
		case 3:		m_pCurrentSCL += m_pDataBuffer->SCL_Lv3[0];		break;
		case 4:		m_pCurrentSCL += m_pDataBuffer->SCL_Lv4[0];		break;

		// 指定されたレベルは不正である //
		default:	return FALSE;
	}

	// 諸変数の初期化 //
	m_NopCount = 0;			// ＮＯＰ命令の待ち時間
	m_Level    = Level;		// 現在のレベル
	m_Target   = 0;			// 現在のレベルの進行状況
	m_Count    = 0;			// 経過フレーム数

	m_Level1Exit = 0;	// レベル１の脱出タイミング
	m_Level2Exit = 0;	// レベル２の脱出タイミング
	m_Level3Exit = 0;	// レベル３の脱出タイミング

	return TRUE;
}


// １フレーム分バッファを進める //
void CSCLDecoder::Proceed(void)
{
	int		x, y;
	int		ox, oy;
	int		Addr;
	BYTE	*pCmd;

#ifdef PBG_DEBUG
	if(NULL == m_pDataBuffer){
		PbgError("CSCLDecoder::Proceed() : データがロードされていませんぜ");
		return;
	}

	if(NULL == m_pCurrentSCL){
		PbgError("CSCLDecoder::Proceed() : Initialize() を呼ばないとだめですよぉ");
		return;
	}
#endif


	// カウンタをインクリメントする //
	m_Count++;

	// 中心座標を求める(多少の高速化を図る) //
	ox = m_ox;	// 中心のＸ座標
	oy = m_oy;	// 中心のＹ座標

	while(1){

#ifdef PBG_DEBUG
	if(GetAsyncKeyState(VK_F11) & 0x8000){
		char	buf[1024];
		wsprintf(buf,	"SCL 解析中の停止要求を受理します\r\n"
			"thisPtr(%u)  Offset(%u)", this, CSCLDecoder::Ptr2Offset(pCmd));
		PbgLog(buf);
		return;
	}
#endif
		// ポインタ周りを少々高速化する                       //
		// なお、この変数は、値の参照に用いるので、ポインタを //
		// 前進させる場合には間違って使用しない事！！         //
		pCmd = m_pCurrentSCL;

		// 命令コードを抽出 //
		switch(pCmd[0]){
		case SCR_NOP:	// [ 停止命令 ]
			// １回目の呼び出しの場合、カウントをリセットする //
			if(0 == m_NopCount) m_NopCount = *(WORD *)(&pCmd[1]);
			else                m_NopCount-= 1;		// 残り時間減少

			// 残り時間が０になったので、命令を進める //
			if(0 == m_NopCount){
				m_pCurrentSCL += (1 + 2);
				break;
			}
		return;		// NOP 命令はここで終了

		case SCR_SET:	// [ 敵セット命令 ]
			x    = *(short *)(&pCmd[1 + 0]);		// Ｘ座標
			y    = *(short *)(&pCmd[1 + 2]);		// Ｙ座標
			Addr = *(int   *)(&pCmd[1 + 2 + 2]);	// 開始アドレス

			// ８ビット左シフトしているのは、与えられた座標が //
			// 画面上の座標で、２５６倍されていない為である   //
			x = ox + (x << 8);	// ｘ座標を取得
			y = oy + (y << 8);	// ｙ座標を取得

			m_pEnemyCtrl->SetEnemy(x, y, Addr, 0, ENEMY_NORMAL);

			m_pCurrentSCL += (1 + 2 + 2 + 4);
		break;		// もう一回まわって

		case SCR_EXIT:	// [ 終了命令 ]
			// 敵が全滅している場合 //
			// 2001/09/21 : Boss 用に修正(GetActiveData->IsNormalEnemyAlive)
//			if(0 == m_pEnemyCtrl->GetActiveData()){
			if(FALSE == m_pEnemyCtrl->IsNomalEnemyAlive()){
				JumpNextBuffer();	// 次のバッファに進む
				break;				// もう一回まわる
			}
		return;		// もうちょっと、待つ

		default:
			PbgError("ＳＣＬ スクリプトエラー : 未定義の命令です");
		return;
		}
	}
}


// 相手から送られてきた攻撃をセットする(Lv 1) //
void CSCLDecoder::SetExAttackLv1(BYTE PlayerID, BYTE AtkLv)
{
	int		Addr;

#ifdef PBG_DEBUG
	if(PlayerID >= NUM_CHARACTERS){
		PbgError("ＳＣＬ攻撃セット：プレイヤーＩＤが不正です");
	}
#endif

	Addr = m_pDataBuffer->Lv1Attack[PlayerID];

	if(-1 == Addr){
#ifdef PBG_DEBUG
		char	buf[100];
		wsprintf(buf, "プレイヤー %d の攻撃Lv1 は未定義です", PlayerID);
		PbgError(buf);
#endif

		return;
	}

	m_pEnemyCtrl->SetEnemy(m_ox, m_oy, Addr, AtkLv, ENEMY_EXATK);
}


// 相手から送られてきた攻撃をセットする(Lv 2) //
void CSCLDecoder::SetExAttackLv2(BYTE PlayerID, BYTE AtkLv)
{
	int		Addr;

#ifdef PBG_DEBUG
	if(PlayerID >= NUM_CHARACTERS){
		PbgError("ＳＣＬ攻撃セット：プレイヤーＩＤが不正です");
	}
#endif

	Addr = m_pDataBuffer->Lv2Attack[PlayerID];

	if(-1 == Addr){
#ifdef PBG_DEBUG
		char	buf[100];
		wsprintf(buf, "プレイヤー %d の攻撃Lv2 は未定義です", PlayerID);
		PbgError(buf);
#endif

		return;
	}

	m_pEnemyCtrl->SetEnemy(m_ox, m_oy, Addr, AtkLv, ENEMY_EXATK);
}


// 相手から送られてきた攻撃をセットする(Boss) //
void CSCLDecoder::SetBossAttack(BYTE PlayerID, BYTE AtkLv)
{
	int		Addr;

#ifdef PBG_DEBUG
	if(PlayerID >= NUM_CHARACTERS){
		PbgError("ＳＣＬ攻撃セット：プレイヤーＩＤが不正です");
	}
#endif

	Addr = m_pDataBuffer->BossAddr[PlayerID];

	if(-1 == Addr){
#ifdef PBG_DEBUG
		char	buf[100];
		wsprintf(buf, "プレイヤー %d のボスアタック は未定義です", PlayerID);
		PbgError(buf);
#endif

		return;
	}

	// ボスをセットする //
	m_pEnemyCtrl->SetBoss(Addr, AtkLv);
}


// Combo //
void CSCLDecoder::SetComboAttack(BYTE PlayerID)
{
	int		Addr;

#ifdef PBG_DEBUG
	if(PlayerID >= NUM_CHARACTERS){
		PbgError("ＳＣＬ攻撃セット：プレイヤーＩＤが不正です");
	}
#endif

	Addr = m_pDataBuffer->ComboAddr[PlayerID];

	if(-1 == Addr){
#ifdef PBG_DEBUG
		char	buf[100];
		wsprintf(buf, "プレイヤー %d の攻撃Lv2 は未定義です", PlayerID);
		PbgError(buf);
#endif

		return;
	}

	m_pEnemyCtrl->SetEnemy(m_ox, m_oy, Addr, 11, ENEMY_EXATK);
}


// 条件ロードテクスチャのエントリポイントを取得する //
LoadTextureEntry *CSCLDecoder::GetTextureEntry(int CharID)
{
	if(NULL == m_pDataBuffer) return NULL;
	if(CharID < 0 || CharID >= NUM_CHARACTERS) return NULL;

	return &(m_pDataBuffer->LTEntry[CharID]);
}


// アニメーション定義の開始位置を取得する //
BYTE *CSCLDecoder::GetExAnmAddr(int CharID, BYTE Level)
{
	int			Addr;

	if(NULL == m_pDataBuffer) return NULL;
	if(CharID < 0 || CharID >= NUM_CHARACTERS) return NULL;

	switch(Level){
		case AGD_LV1:	// レベル１攻撃
			Addr = m_pDataBuffer->ExAnmLv1[CharID];
		break;

		case AGD_LV2:	// レベル２攻撃
			Addr = m_pDataBuffer->ExAnmLv2[CharID];
		break;

		case AGD_BOSS:	// ボスアタック
			Addr = m_pDataBuffer->ExAnmBoss[CharID];
		break;

		case AGD_WON:	// 勝ち
			Addr = m_pDataBuffer->ExAnmWin[CharID];
		break;

		default:
		return NULL;
	}

	if(-1 == Addr) return NULL;
	else           return Offset2Ptr(Addr);
}


// ＳＣＬデータをロードする //
BOOL CSCLDecoder::Load(char *pFileID		// ファイルＩＤ
					 , CHARACTER_ID P1		// プレイヤー１の使用キャラ
					 , CHARACTER_ID P2)		// プレイヤー２の使用キャラ
{
	BYTE				*pTextureBlock;
	int					TexInitAddr;

	// テクスチャ以外のロードを行う //
	if(FALSE == LoadWithoutTexture(pFileID)) return FALSE;


///////////////////////////////////////////////////////////////////////////////
//              では、テクスチャのロードを行いましょう                       //
///////////////////////////////////////////////////////////////////////////////

	// テクスチャ初期化関数の開始オフセットを格納 //
	TexInitAddr = m_pDataBuffer->TexInitializer;

	// アドレス確定(キャストに注意 : オフセットはバイト指定) //
	pTextureBlock = ((BYTE *)m_pDataBuffer) + TexInitAddr;
//PbgError("CSCLDecoder::LoadAnime Enter");

	// テクスチャのロード //
	if(FALSE == CEnemyAnimeContainer::Load(
					pTextureBlock, P1, P2)){
		return FALSE;
	}
//PbgError("CSCLDecoder::LoadAnime");

	return TRUE;
}


FBOOL CSCLDecoder::LoadWithoutTexture(char *pFileID)	// ファイルＩＤ
{
	Pbg::LzDecode		Decode;

	// すでにデータがロードされている場合は、解放する //
	if(NULL != m_pDataBuffer) MemFree(m_pDataBuffer);

	// ファイルのオープン //
	if(FALSE == Decode.Open("ENEMY.DAT")) return FALSE;

	// 解凍する //
	m_pDataBuffer = (SCLHeader *)Decode.Decode(pFileID);
	if(NULL == m_pDataBuffer) return FALSE;

	// 一応、(明示的に)クローズしましょう //
	Decode.Close();

	m_Level1Exit = 0;	// レベル１の脱出タイミング
	m_Level2Exit = 0;	// レベル２の脱出タイミング
	m_Level3Exit = 0;	// レベル３の脱出タイミング

	return TRUE;
}


// ＳＣＬの敵配置テーブルをシャッフルする //
FBOOL CSCLDecoder::ShuffleSCLData(Pbg::CRnd *pRnd)
{
	if(NULL == m_pDataBuffer) return FALSE;

	// あとは、テンプレート関数におまかせ //
	Pbg::Shuffle(m_pDataBuffer->SCL_Lv1, m_pDataBuffer->NumLv1SCL, pRnd);	// Level 1
	Pbg::Shuffle(m_pDataBuffer->SCL_Lv2, m_pDataBuffer->NumLv2SCL, pRnd);	// Level 2
	Pbg::Shuffle(m_pDataBuffer->SCL_Lv3, m_pDataBuffer->NumLv3SCL, pRnd);	// Level 3
	Pbg::Shuffle(m_pDataBuffer->SCL_Lv4, m_pDataBuffer->NumLv4SCL, pRnd);	// Level 3

	return TRUE;
}


// 次の編隊ブロックに進む //
FVOID CSCLDecoder::JumpNextBuffer(void)
{
	int			t;

	// ＳＣＬ先頭領域までジャンプする //
	m_pCurrentSCL = (BYTE *)m_pDataBuffer;

	// 参照する場所を変更する //
	m_Target++;

	// レベルにより開始アドレスが変化する //
	switch(m_Level){
		case 1:
			t = m_Target % m_pDataBuffer->NumLv1SCL;
			if(m_Count > 20 * 60){
				if(m_Target >= m_Level1Exit){
					m_Level1Exit   = m_Target;
					m_Target       = 0;
					m_Level        = 2;
					m_pCurrentSCL += m_pDataBuffer->SCL_Lv2[0];
					return;
				}
			}

			m_pCurrentSCL += m_pDataBuffer->SCL_Lv1[t];
		break;

		case 2:
			t = m_Target % m_pDataBuffer->NumLv2SCL;
			if(m_Count > 40 * 60){
				if(m_Target >= m_Level2Exit){
					m_Level2Exit   = m_Target;
					m_Target       = 0;
					m_Level        = 3;
					m_pCurrentSCL += m_pDataBuffer->SCL_Lv3[0];
					return;
				}
			}

			m_pCurrentSCL += m_pDataBuffer->SCL_Lv2[t];
		break;

		case 3:
			t = m_Target % m_pDataBuffer->NumLv3SCL;
			if(m_Count > 60 * 60){
				if(m_Target >= m_Level3Exit){
					m_Level3Exit   = m_Target;
					m_Target       = 0;
					m_Level        = 4;
					m_pCurrentSCL += m_pDataBuffer->SCL_Lv4[0];
					return;
				}
			}

			m_pCurrentSCL += m_pDataBuffer->SCL_Lv3[t];
		break;

		case 4:
			t = m_Target % m_pDataBuffer->NumLv4SCL;
			m_pCurrentSCL += m_pDataBuffer->SCL_Lv4[t];
		break;
	}
}
