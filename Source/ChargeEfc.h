/*
 *   ChargeEfc.h   : 溜め完了エフェクト描画
 *
 */

#ifndef CCHARGEEFC_INCLUDED
#define CCHARGEEFC_INCLUDED "溜め完了エフェクト描画 : Version 0.01 : Update 2001/07/22"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/22 : 制作開始
 */



#include "Gian2001.h"



/***** [クラス定義] *****/

// 溜めエフェクト描画用クラス //
class CChargeEfc {
public:
	FVOID Initialize(void);		// 初期化する
	FVOID Set(BYTE Level);		// エフェクトをセットする
	FVOID SetGBFinished(void);	// ガードブレイク終了エフェクトをセットする
	FVOID Move(void);			// １フレーム分動作させる
	FVOID Draw(void);			// 描画する

	CChargeEfc(int *pX, int *pY);	// コンストラクタ
	~CChargeEfc();					// デストラクタ


private:
	FVOID DrawChargeEfc(void);		// 溜め完了エフェクト描画
	FVOID DrawExtraString(void);	// 文字列 (LvXX CHARGE OK) の表示


	// 頂点をセットする //
	static FVOID Setup3DPoint(D3DTLVERTEX	*tlv	// 頂点バッファ
							, int			x		// 中心となるＸ座標(非x256)
							, int			y		// 中心となるＹ座標(非x256)
							, int			l		// 中心からの距離(非x256)
							, DWORD			c		// 頂点色
							, BYTE			rx		// Ｘ軸に対する回転
							, BYTE			ry		// Ｙ軸に対する回転
							, BYTE			rz);	// Ｚ軸に対する回転


	DWORD		m_Count;	// 進行状態カウンタ
	DWORD		m_Level;	// 現在の溜め完了レベル

	BYTE		m_rx;		// Ｘ軸に対する回転
	BYTE		m_ry;		// Ｙ軸に対する回転
	BYTE		m_rz;		// Ｚ軸に対する回転

	DWORD		m_GBEfcCount;	// ガードブレイク終了エフェクト(０なら無効)

	const int	*m_pX;		// 対象のＸ座標
	const int	*m_pY;		// 対象のＹ座標
};



#endif
