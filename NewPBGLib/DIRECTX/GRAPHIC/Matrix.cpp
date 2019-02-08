/*
 *   Matrix.cpp   : 行列管理
 *
 */

#include "Matrix.h"
#include "PbgMath.h"



// 零行列にする //
FVOID CMatrix::Zero(void)
{
	ZEROMEM(m_Matrix.m);
}


// 単位行列にする //
FVOID CMatrix::Identity(void)
{
	m_Matrix._11 = m_Matrix._22 = m_Matrix._33 = m_Matrix._44 = 1.0;

	               m_Matrix._12 = m_Matrix._13 = m_Matrix._14 =
	m_Matrix._21 =                m_Matrix._23 = m_Matrix._24 =
	m_Matrix._31 = m_Matrix._32 =                m_Matrix._34 =
	m_Matrix._41 = m_Matrix._42 = m_Matrix._43                = 0.0;
}
/*
	FVOID SetRotateX(D3DVALUE angle);	// Ｘ軸回転を行うための行列を作成
	FVOID SetRotateY(D3DVALUE angle);	// Ｙ軸回転を行うための行列を作成
	FVOID SetRotateZ(D3DVALUE angle);	// Ｚ軸回転を行うための行列を作成

	// 平行移動 //
	FVOID Translate(D3DVALUE dx, D3DVALUE dy, D3DVALUE dz);
*/

// Ｘ軸回転を行うための行列を作成 //
FVOID CMatrix::SetRotateX(D3DVALUE angle)
{
	D3DVALUE		s, c;

//	s = D3DVAL(SinM(angle)) / D3DVAL(256);
//	c = D3DVAL(CosM(angle)) / D3DVAL(256);
	s = sinf(angle);
	c = cosf(angle);

	m_Matrix._11 = m_Matrix._44 = 1.0;
	m_Matrix._22 = m_Matrix._33 = c;
	m_Matrix._23 = -s;
	m_Matrix._32 =  s;

                   m_Matrix._12 = m_Matrix._13 = m_Matrix._14 =
	m_Matrix._21 =                               m_Matrix._24 =
	m_Matrix._31 =                               m_Matrix._34 =
	m_Matrix._41 = m_Matrix._42 = m_Matrix._43                = 0.0;
}


// Ｙ軸回転を行うための行列を作成 //
FVOID CMatrix::SetRotateY(D3DVALUE angle)
{
	D3DVALUE		s, c;

//	s = D3DVAL(SinM(angle)) / D3DVAL(256);
//	c = D3DVAL(CosM(angle)) / D3DVAL(256);
	s = sinf(angle);
	c = cosf(angle);

	m_Matrix._22 = m_Matrix._44 = 1.0;
	m_Matrix._11 = m_Matrix._33 = c;
	m_Matrix._13 = -s;
	m_Matrix._31 =  s;

                   m_Matrix._12                = m_Matrix._14 =
	m_Matrix._21 =                m_Matrix._23 = m_Matrix._24 =
	               m_Matrix._32                = m_Matrix._34 =
	m_Matrix._41 = m_Matrix._42 = m_Matrix._43                = 0.0;
}


// Ｚ軸回転を行うための行列を作成 //
FVOID CMatrix::SetRotateZ(D3DVALUE angle)
{
	D3DVALUE		s, c;

//	s = D3DVAL(SinM(angle)) / D3DVAL(256);
//	c = D3DVAL(CosM(angle)) / D3DVAL(256);
	s = sinf(angle);
	c = cosf(angle);

	m_Matrix._33 = m_Matrix._44 = 1.0;
	m_Matrix._11 = m_Matrix._22 = c;
	m_Matrix._21 = -s;
	m_Matrix._12 =  s;

                                  m_Matrix._13 = m_Matrix._14 =
	                              m_Matrix._23 = m_Matrix._24 =
	m_Matrix._31 = m_Matrix._32                = m_Matrix._34 =
	m_Matrix._41 = m_Matrix._42 = m_Matrix._43                = 0.0;
}


// Ｘ軸回転を行うための行列を掛ける //
FVOID CMatrix::MulRotateX(D3DVALUE angle)
{
	CMatrix		temp;

	temp.SetRotateX(angle);
	this->Multiply(temp);
}


// Ｙ軸回転を行うための行列を掛ける //
FVOID CMatrix::MulRotateY(D3DVALUE angle)
{
	CMatrix		temp;

	temp.SetRotateY(angle);
	this->Multiply(temp);
}


// Ｚ軸回転を行うための行列を掛ける //
FVOID CMatrix::MulRotateZ(D3DVALUE angle)
{
	CMatrix		temp;

	temp.SetRotateZ(angle);
	this->Multiply(temp);
}


// 平行移動を行う //
FVOID CMatrix::AddTranslate(D3DVALUE dx, D3DVALUE dy, D3DVALUE dz)
{
	m_Matrix._41 += dx;	//D3DVAL(x256) / D3DVAL(256);
	m_Matrix._42 += dy;	//D3DVAL(y256) / D3DVAL(256);
	m_Matrix._43 += dz;	//D3DVAL(z256) / D3DVAL(256);
/*
	m_Matrix._11 = m_Matrix._22 = m_Matrix._33 = m_Matrix._44 = 1.0;

	               m_Matrix._12 = m_Matrix._13 = m_Matrix._14 =
	m_Matrix._21 =                m_Matrix._23 = m_Matrix._24 =
	m_Matrix._31 = m_Matrix._32 =                m_Matrix._34 = 0.0;
*/
}


// 行列の積を求める                    //
// m_Matrix = m_Matrix * rmat.m_Matrix //
FVOID CMatrix::Multiply(CMatrix &rmat)
{
	int			i, j, k;
	CMatrix		tmp;

	tmp.Zero();

	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			for(k=0; k<4; k++){
				tmp(i, j) += m_Matrix(k, j) * rmat(i, k);
			}
		}
	}

	m_Matrix = tmp.m_Matrix;
}
