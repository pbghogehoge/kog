/*
 *   CPUCtrl.cpp   : ＣＰＵの動作
 *
 */

#include "CPUCtrl.h"
#include "ChargeDef.h"
#include "PrivateCPUCtrl.h"
#include "TamaSize.h"
#include <limits.h>

#include "RndCtrl.h"




// コンストラクタ //
CCPUCtrl::CCPUCtrl()
{
	// 何もしないよ //
}


// デストラクタ //
CCPUCtrl::~CCPUCtrl()
{
}


// レベルの初期化を行う //
FVOID CCPUCtrl::CPUInitialize(CPULEVEL GameLevel)
{
	m_GameLevel    = GameLevel;			// 難易度による基本レベル
	m_CurrentLevel = 0;					// 現在のＣＰＵ思考ルーチンレベル
	m_Move         = CPUMOVE_TARGET;	// 移動ルーチン
	m_WaitCount    = 0;					// 停止要求の待ち時間
	m_CheckRange   = 64 * 256;			// 探索範囲

	// 次に発動しようとしている溜めレベルをセット //
	m_ChargeLevel = CHG_LVHALF;
	//SelectChargeLevel();

	m_TargetX = *(m_pPlayerCtrl->GetX_Ptr());	// 目標とするＸ座標を取得する
	m_TargetY = *(m_pPlayerCtrl->GetY_Ptr());	// 目標とするＹ座標を取得する
}


// レベルを変更する //
FVOID CCPUCtrl::CPULevelUP(int DeltaLevel)
{
}


// 目標座標への移動 //
IWORD CCPUCtrl::MoveToTarget(int mx, int my, int tx, int ty)
{
	CEnemyCtrl::Iterator	EnemyIter;
	int						size, ex;
	WORD					KeyCode;

	ForEachFrontPtr(m_pEnemyCtrl, 0, EnemyIter){
		ex   = EnemyIter->x;
		size = EnemyIter->size + 8 * 256;

		// 移動中に、攻撃可能な敵が現れた場合 //
		if(HitCheckFast(ex, mx, size)){
			m_TargetX = ex;
			return 0;
		}
	}

	if(mx < tx) KeyCode = KEY_RIGHT;
	else        KeyCode = KEY_LEFT;

	if(!HitCheckFast(my, ty, 16 * 256)){
		if(my < ty) KeyCode |= KEY_DOWN;
		else        KeyCode |= KEY_UP;
	}

	return KeyCode;
}


// 新しい目標を決める //
IVOID CCPUCtrl::SelectNewTarget(int mx, int my)
{
	CEnemyCtrl::Iterator	EnemyIter;
	int						dx, tx, ty;
	int						MinY, MaxY, StartY;
	int						mindx;
	int						mintx;


	mindx = 640 * 256;
	ty    = 0;


	// もっとも近くにいる敵を検索する //
	ForEachFrontPtr(m_pEnemyCtrl, 0, EnemyIter){
		tx = EnemyIter->x;
		dx = abs(tx - mx);

		if(mindx > dx){
			mintx = tx;
			mindx = dx;
			ty    = EnemyIter->y;
		}
	}


	MinY   = m_pPlayerCtrl->GetYMin();
	MaxY   = m_pPlayerCtrl->GetYMax();
	StartY = m_pPlayerCtrl->GetStartY();

	if(my < (MinY + MaxY)/2){
		m_TargetY = StartY + RndEx() % (MaxY - StartY);
	}
	else if(ty){
		m_TargetY = ty + 180 * 256;//(my + 2*ty) / 3;
//		dy        = (MaxY - StartY);
//		m_TargetY = StartY - dy + RndEx() % (dy * 2);
	}

	if(640 * 256 == mindx){
		int		MinX = m_pPlayerCtrl->GetXMin();
		int		MaxX = m_pPlayerCtrl->GetXMax();

		m_TargetX = MinX + RndEx() % (MaxX - MinX);
		m_Move    = CPUMOVE_WAITENEMY;
	}
	else{
		m_TargetX = mintx;
		m_Move = CPUMOVE_TARGET;
	}
}


// 敵弾を避ける方向にキーコードを変換する(ret : 変換後のキーコード) //
IWORD CCPUCtrl
	::ConvertEvadeKeyCode(int mx, int my, WORD KeyCode)
{
	const int  EtStart = (ETAMA_NORMAL8 >> 4);	// 当たり判定有りの敵弾先頭
	const int  EtEnd   = (ETAMA_ANIME32 >> 4);	// 当たり判定有りの敵弾終端

	// キーボード用マスク //
	const WORD KeyMask = (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT | KEY_SHIFT);


	CEnemyTama					*pEnemyTama;
	CEnemyTama::Iterator		ETamaIter;
	TamaData					*pTama;
	HitCheckStruct				HCheck[ENEMYTAMA_MAX], *pCheck;
	int							ShaveSize;
	int							HitSize;
	int							x, y;
	int							i, j, n;
	int							CheckRange;

	int WeightTable[9] = {
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
	};

	BOOL						bHit;

	pCheck = HCheck;	// データの挿入先を先頭にセットする
	bHit   = FALSE;		// 敵弾ヒットフラグを無効化

	CheckRange = m_CheckRange;						// 探索範囲
	pEnemyTama = m_pEnemyCtrl->GetEnemyTamaPtr();	// 敵弾管理クラス取得


	// まずは、移動しない場合の当たり判定を行いつつ、//
	// 当たり判定処理を行う弾数を絞り込む            //
	for(i=EtStart; i<=EtEnd; i++){
		switch(i){
			case(ETAMA_NORMAL8  >> 4):	// ８×８ まる弾
			case(ETAMA_ANGLE8   >> 4):	// ８×８ 角度同期
			case(ETAMA_ANIME8   >> 4):	// ８×８ アニメーション
				ShaveSize = ETAMA8_HITSIZE * 6;
				HitSize   = ETAMA8_HITSIZE;
			break;

			case(ETAMA_NORMAL16 >> 4):	// 16×16 まる弾
			case(ETAMA_ANGLE16  >> 4):	// 16×16 角度同期
			case(ETAMA_ANIME16  >> 4):	// 16×16 アニメーション
				ShaveSize = ETAMA16_HITSIZE * 6;
				HitSize   = ETAMA16_HITSIZE;
			break;

			case(ETAMA_NORMAL24 >> 4):	// 24×24 まる弾
			case(ETAMA_ANGLE24  >> 4):	// 24×24 角度同期
			case(ETAMA_ANIME24  >> 4):	// 24×24 アニメーション
				ShaveSize = ETAMA24_HITSIZE * 6;
				HitSize   = ETAMA24_HITSIZE;
			break;

			case(ETAMA_NORMAL32 >> 4):	// 32×32 まる弾
			case(ETAMA_ANGLE32  >> 4):	// 32×32 角度同期
			case(ETAMA_ANIME32  >> 4):	// 32×32 アニメーション
				ShaveSize = ETAMA32_HITSIZE * 6;
				HitSize   = ETAMA32_HITSIZE;
			break;

			default:
				#ifdef PBG_DEBUG
					PbgError("CCPUCtrl::ConvertEvadeKeyCode() : そいつぁ、内部エラーでっせ");
				#endif
			return 0;
		}

		ForEachFrontPtr(pEnemyTama, i, ETamaIter){
			// イテレータ -> ポインタ への変換 //
			pTama = ETamaIter.GetPtr();

			// 当たり判定の絞り込み //
			if(HitCheckFast(my, pTama->y, CheckRange)		// Ｙ方向
			&& HitCheckFast(mx, pTama->x, CheckRange)){		// Ｘ方向
				pCheck->x        = pTama->x;		// Ｘ座標
				pCheck->y        = pTama->y;		// Ｙ座標
				pCheck->d        = pTama->Angle;	// 進行方向
				pCheck->HitSize  = HitSize;			// カスり判定サイズ
				pCheck->IsShaved = pTama->IsShaved;	// すでにカスっていれば真
/*
				//                  | 0 1 2 |  [上段] //
				// WeightTableIndex | 3 4 5 |  [中段] //
				//                  | 6 7 8 |  [下段] //
				if(HitCheckFast(my, pTama->y, ShaveSize)){	// 中段
					if(HitCheckFast(mx, pTama->x, ShaveSize)) WeightTable[4]++;
					else if(mx > pTama->x)                    WeightTable[3]++;
					else                                      WeightTable[5]++;
				}
				else if(my > pTama->y){						// 上段
					if(HitCheckFast(mx, pTama->x, ShaveSize)) WeightTable[1]++;
					else if(mx > pTama->x)                    WeightTable[0]++;
					else                                      WeightTable[2]++;
				}
				else{									// 下段
					if(HitCheckFast(mx, pTama->x, ShaveSize)) WeightTable[7]++;
					else if(mx > pTama->x)                    WeightTable[6]++;
					else                                      WeightTable[8]++;
				}
*/
				pCheck++;
			}
		}
	}

	n      = pCheck - HCheck;
	pCheck = HCheck;

	if((n < 4) && (CheckRange < 300 * 256)){
		m_CheckRange += 256 * 2;
	}
	else if(CheckRange > 64 * 256){
		m_CheckRange -= 256 * 4;
	}
/*
	if(n){
		int MaxWeight  = INT_MIN;
		int MaxTableID = 0;

		for(i=0; i<9; i++){
			if(WeightTable[i] > MaxWeight){
				MaxWeight  = WeightTable[i];
				MaxTableID = i;
			}
		}

		switch(MaxTableID){
		case 0: case 6:
		return KEY_LEFT | (KeyCode & KEY_SHOT);

		case 2: case 8:
		return KEY_RIGHT | (KeyCode & KEY_SHOT);

		default:
		break;
		}
	}
*/
/*
	// 与えられた入力によって、移動したと仮定する //
	x = mx;		// 元となるＸ座標
	y = my;		// 元となるＹ座標
	m_pPlayerCtrl->SetXYFromKeyCode(x, y, KeyCode);

	bHit = FALSE;
	for(i=0; i<n; i++, pCheck++){
		HitSize = pCheck->HitSize;

		// 指定された方向に移動すると敵弾にヒットする場合 //
		if(HitCheckFast(pCheck->y, y, HitSize)
		&& HitCheckFast(pCheck->x, x, HitSize)){
			bHit = TRUE;
			// 移動しなければヒットしない場合、フラグを外して return        //
			// そうでなければ、別の移動方向の探索のために、ループから抜ける //
//			if(FALSE == bHit) return KeyCode & (~KeyMask);
//			else              break;
		}
	}
*/
	WORD MoveKeyTable[17] = {
		0,

		KEY_UP    | KEY_SHIFT,
		KEY_DOWN  | KEY_SHIFT,
		KEY_LEFT  | KEY_SHIFT,
		KEY_RIGHT | KEY_SHIFT,

		KEY_DOWN | KEY_LEFT  | KEY_SHIFT,
		KEY_DOWN | KEY_RIGHT | KEY_SHIFT,
		KEY_UP   | KEY_LEFT  | KEY_SHIFT,
		KEY_UP   | KEY_RIGHT | KEY_SHIFT,

		KEY_DOWN,
		KEY_UP,
		KEY_LEFT,
		KEY_RIGHT,

		KEY_DOWN | KEY_LEFT,
		KEY_DOWN | KEY_RIGHT,
		KEY_UP   | KEY_LEFT,
		KEY_UP   | KEY_RIGHT,
	};

	int MaxValue  = INT_MIN;	// 最大の評価値
	int MaxTarget = 0;			// 最大の評価値となる移動方向
	int Sum;					// 現在の方向の評価値合計
//	int Sum2;
	int dir, dx, dy;

//	Sum2 = -1;

	for(j=0; j<17; j++){
		// 与えられた入力によって、移動したと仮定する //
		x       = mx;				// 元となるＸ座標
		y       = my;				// 元となるＹ座標
		pCheck  = HCheck;			// 探索の対象
		Sum     = 0;				// 評価値の合計

		m_pPlayerCtrl->SetXYFromKeyCode(x, y, MoveKeyTable[j]);

		// 評価値の計算を始める //
		for(i=0; i<n; i++, pCheck++){
			HitSize = pCheck->HitSize;	// 当たり判定サイズ
/*
			// 指定された方向に移動すると敵弾にヒットする場合 //
			if(HitCheckFast(pCheck->y, y, HitSize)
			&& HitCheckFast(pCheck->x, x, HitSize)){
				// この方向の移動では決して回避できないので、ループから抜ける //
				break;
			}
*/

			// 評価値の計算 //
			dx  = x - (pCheck->x);					// Ｘ方向差分
			dy  = y - (pCheck->y);					// Ｙ方向差分
			dir = abs(Atan8(dx, dy) - pCheck->d);	// 角度評価

			dx = abs(dx);
			dy = abs(dy);

			if((dx < HitSize) && (dy < HitSize)){
				Sum = INT_MIN;
				break;
			}

			HitSize = HitSize * 4;

			if((dx < HitSize) && (dy < HitSize)){
				if(pCheck->IsShaved){
					dir = abs(dir - 96);
					Sum += (128-dir);
				}
				else if(y > pCheck->y){
					dir = abs(dir - 128);
					Sum += (128-dir);
				}
				else{
					dir = abs(dir - 64);
					Sum += (128-dir)/2;
				}
			}
			else{
				dir = abs(dir - 10);
				Sum += (128-dir)/2;
			}
		}

		// より評価値が大きなものが見つかった場合 //
		if(Sum > MaxValue){
			MaxValue  = Sum;
			MaxTarget = j;
		}
//		if(MoveKeyTable[j] == (KeyCode & KeyMask)){
//			Sum2 = Sum;
//		}
	}

	if(INT_MIN == MaxValue){
		return KEY_BOMB;//MoveKeyTable[MaxTarget];
	}
	else if(KeyCode & KeyMask){
		return KeyCode;
	}
	else{
		return MoveKeyTable[MaxTarget] | (KeyCode & (KEY_SHOT));
	}
}


// キーコードを取得する //
FWORD CCPUCtrl::CPUGetKeyCode(void)
{
	int			mx, my, tx, ty;
	DWORD		CMax, CNow;
	WORD		KeyCode;

	KeyCode = 0;

	mx = *(m_pPlayerCtrl->GetX_Ptr());	// 現在のＸ座標を取得する
	my = *(m_pPlayerCtrl->GetY_Ptr());	// 現在のＹ座標を取得する
	tx = m_TargetX;		// 目標とするＸ座標を取得する
	ty = m_TargetY;		// 目標とするＹ座標を取得する

	CNow = m_pPlayerCtrl->CGaugeGetCurrent();
	CMax = m_pPlayerCtrl->GetCGaugeMax();


	switch(m_Move){
		case CPUMOVE_TARGET:
			if(HitCheckFast(mx, tx, 16 * 256)){
				m_Move      = CPUMOVE_STOP;
				m_WaitCount = 48;
			}
			else{
				KeyCode |= MoveToTarget(mx, my, tx, ty);
			}
		break;

		case CPUMOVE_STOP:
			if(m_WaitCount) m_WaitCount--;
			else            SelectNewTarget(mx, my);
		break;

		case CPUMOVE_WAITENEMY:
			if(m_pEnemyCtrl->GetActiveData() || HitCheckFast(mx, tx, 16 * 256)){
				SelectNewTarget(mx, my);
			}
			else{
				MoveToTarget(mx, my, tx, ty);
			}
		break;
	}


	switch(m_ChargeLevel){
		case CHG_LVHALF:
			if(CNow >= CGAUGE_LV1HALF-1) m_ChargeLevel = CHG_WAIT;
			KeyCode |= KEY_SHOT;
		break;

		case CHG_LV1:
			if(CNow >= CGAUGE_LEVEL1) m_ChargeLevel = CHG_WAIT;
			KeyCode |= KEY_SHOT;
		break;

		case CHG_LV2:
			if(CMax >= CGAUGE_LEVEL2){
				if(CNow >= CGAUGE_LEVEL2-1) m_ChargeLevel = CHG_WAIT;
				KeyCode |= KEY_SHOT;
			}
			else if((RndEx()>>2) % 2) KeyCode |= KEY_SHOT;
		break;

		case CHG_LV3:
			if(CMax >= CGAUGE_LEVEL3){
				if(CNow >= CGAUGE_LEVEL3-1) m_ChargeLevel = CHG_WAIT;
				KeyCode |= KEY_SHOT;
			}
			else if((RndEx()>>2) % 2) KeyCode |= KEY_SHOT;
		break;

		case CHG_LV4:
			if(CMax >= CGAUGE_LEVEL4){
				if(CNow >= CGAUGE_LEVEL4-1) m_ChargeLevel = CHG_WAIT;
				KeyCode |= KEY_SHOT;
			}
			else if((RndEx()>>2) % 2) KeyCode |= KEY_SHOT;
		break;

		case CHG_WAIT:
			SelectChargeLevel();
		break;
	}

	return ConvertEvadeKeyCode(mx, my, KeyCode);
}


// 溜めレベルをセットする //
FVOID CCPUCtrl::SelectChargeLevel(void)
{
	// 現在はランダムかつ適当な実装である //
	switch((RndEx() >> 1) % 5){
		case 0:		m_ChargeLevel = CHG_LVHALF;		break;	// レベル 1/2
		case 1:		m_ChargeLevel = CHG_LV1;		break;	// レベル １
		case 2:		m_ChargeLevel = CHG_LV2;		break;	// レベル ２
		case 3:		m_ChargeLevel = CHG_LV3;		break;	// レベル ３

		case 4:		m_ChargeLevel = CHG_LV4;		break;	// レベル ４
//		case 4:		m_ChargeLevel = CHG_LV1;		break;	// レベル ４
	}
}
