/*
 *   Matrix.h   : 行列管理
 *
 */

#ifndef MATRIX_INCLUDED
#define MATRIX_INCLUDED "行列管理     : Version 0.01 : Update 2001/10/01"

/*  [注意事項]
 *    このクラスでは、コンストラクタによる初期化を行わない
 *    必要に応じて、Zero(), Identity() 等を呼び出すこと
 */

/*  [更新履歴]
 *    Version 0.01 : 2001/10/01 : 制作開始
 */



#include "PbgType.h"



/***** [クラス定義] *****/

// 行列管理クラス //
class CMatrix {
public:
	FVOID Zero(void);		// 零行列にする
	FVOID Identity(void);	// 単位行列にする

	FVOID SetRotateX(D3DVALUE angle);	// Ｘ軸回転を行うための行列を作成
	FVOID SetRotateY(D3DVALUE angle);	// Ｙ軸回転を行うための行列を作成
	FVOID SetRotateZ(D3DVALUE angle);	// Ｚ軸回転を行うための行列を作成

	FVOID MulRotateX(D3DVALUE angle);	// Ｘ軸回転を行うための行列を掛ける
	FVOID MulRotateY(D3DVALUE angle);	// Ｙ軸回転を行うための行列を掛ける
	FVOID MulRotateZ(D3DVALUE angle);	// Ｚ軸回転を行うための行列を掛ける

	// 平行移動を行う //
	FVOID AddTranslate(D3DVALUE dx, D3DVALUE dy, D3DVALUE dz);


	// 行列の積を求める                    //
	// m_Matrix = m_Matrix * rmat.m_Matrix //
	FVOID Multiply(CMatrix &rmat);

	// 括弧演算子 //
    D3DVALUE& operator()(int r, int c) { return m_Matrix(r, c); };
    const D3DVALUE& operator()(int r, int c) const { return m_Matrix(r, c); };

	// 行列の取得 //
	D3DMATRIX *GetMatrix(void){ return &m_Matrix; };


private:
	D3DMATRIX		m_Matrix;
};



#endif
