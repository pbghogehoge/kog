/*
 *   CGrpEffect.h   : CDIB32用エフェクト
 *
 */

#ifndef CGRPEFFECT_INCLUDED
#define CGRPEFFECT_INCLUDED "CDIB32用エフェクト : Version 0.01 : Update 2001/03/04"

/*  [更新履歴]
 *    Version 0.01 : 2001/03/04 : モザイクエフェクトを追加
 */



#include "CDIB32.h"
#include "CWaterEfc.h"



namespace Pbg {



/***** [クラス定義] *****/

class CTransTable8;

class CDIB32Effect : public CDIB32 {
public:
	// 指定矩形内にモザイクをかける                             //
	// 引数  rcTarget : エフェクト対象となる矩形(NULL 時は全体) //
	//       Size     : モザイクのサイズ                        //
	//       Alpha    : [-256, 0) 徐々に暗くする                //
	//                : 0         そのままの明るさ              //
	//                : (0, 256]  徐々に明るくする              //
	FBOOL EfcMosaic(const RECT *pTarget, int Size, int Alpha=0);


	// 指定された点から放射状にモーションブラーをかける         //
	// 引数  rcTarget : エフェクト対象となる矩形(NULL 時は全体) //
	//       ox, oy   : エフェクトの中心となる点                //
	//       Alpha    : [-256, 0) 徐々に暗くする                //
	//                : 0         そのままの明るさ              //
	//                : (0, 256]  徐々に明るくする              //
	FBOOL EfcMotionBlur(const RECT *pTarget, int ox, int oy, int Alpha=0);


	// テーブルに従って、変形を行う       //
	// 引数 (ox, oy) : 開始点の座標       //
	//        pSrc   : 元画像             //
	//        pTable : 変換テーブルクラス //
	FBOOL CDIB32Effect::EfcTransform(	int				ox
									 ,	int				oy
									 ,	CDIB32			*pSrc
									 ,	CTransTable8	*pTable);


	CDIB32Effect();		// コンストラクタ
	~CDIB32Effect();	// デストラクタ


private:
	// このＤＩＢの範囲に収まるようにクリッピングを掛ける //
	// 戻り値  TRUE  : 正常にクリッピングされた
	//         FALSE : 指定された矩形は完全に範囲外である
	FBOOL ClipRect(RECT *pDest, const RECT *pSrc);

	FVOID MosaicStd(RECT *pTarget, int Size, int Alpha);	// 非ＭＭＸモザイク
	FVOID MosaicMMX(RECT *pTarget, int Size, int Alpha);	// ＭＭＸモザイク

	FVOID MotionBlurStd(RECT *pTarget, int ox, int oy, int Alpha);	// 非ＭＭＸブラー
	FVOID MotionBlurMMX(RECT *pTarget, int ox, int oy, int Alpha);	// ＭＭＸブラー
};



} // namespace Pbg



#endif
