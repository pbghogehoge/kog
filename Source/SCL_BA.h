/*
 *   SCL_BA.h   : ボスモード用SCL
 *
 */

#ifndef SCL_BA_INCLUDED
#define SCL_BA_INCLUDED "ボスモード用SCL : Version 0.01 : Update 2002/01/22"

/*  [更新履歴]
 *    Version 0.01 : 2002/01/22 : 制作開始
 */



#include "SCL.h"



/***** [クラス定義] *****/

// ＳＣＬ展開(for BossAttack Mode) //
class CSCLBADecoder : public CSCLDecoder {
public:
	BOOL Initialize(Pbg::CRnd *pRnd);	// バッファ内のデータをリセットする
	void Proceed(void);					// １フレーム分バッファを進める

	// ＳＣＬデータをロードする //
	static BOOL LoadBA(	char			*pFileID,	// ファイルＩＤ
						CHARACTER_ID	P1);		// プレイヤー１の使用キャラ


	CSCLBADecoder(RECT *rcTargetX256, CEnemyCtrl *pEnemyCtrl);	// コンストラクタ
	~CSCLBADecoder();											// デストラクタ


private:
	BYTE		m_BossID[9];

	DWORD		m_BACount;
};



#endif
