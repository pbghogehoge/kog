/*
 *   CRnd.cpp   : 乱数発生
 *
 */

#include "CRnd.h"



namespace Pbg {



// コンストラクタ //
CRnd::CRnd()
{
	Randomize();
}


// デストラクタ //
CRnd::~CRnd()
{
}


#define M			397
#define MatrixA		0x9908b0df
#define UpperMask	0x80000000
#define LowerMask	0x7fffffff

#define TemperingMaskB		0x9d2c5680
#define TemperingMaskC		0xefc60000
#define TemperingShiftU(y)	(y >> 11)
#define TemperingShiftS(y)	(y << 7)
#define TemperingShiftT(y)	(y << 15)
#define TemperingShiftL(y)	(y >> 18)


// 乱数を取得 //
FDWORD CRnd::Get(void)
{
	int				kk;
	DWORD			y;
	DWORD			*pVector;
	static DWORD	mag01[2] = {0, MatrixA};

	pVector = m_MTVector;

	if(m_MTCounter >= MTVECTOR_SIZE){
		for(kk=0; kk<MTVECTOR_SIZE-M; kk++){
			y = (pVector[kk] & UpperMask) | (pVector[kk+1] & LowerMask);
			pVector[kk] = pVector[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
		}

		for(; kk<MTVECTOR_SIZE-1; kk++){
			y = (pVector[kk] & UpperMask) | (pVector[kk+1] & LowerMask);
			pVector[kk] = pVector[kk+(M-MTVECTOR_SIZE)] ^ (y >> 1) ^ mag01[y & 0x1];
		}

		y = (pVector[MTVECTOR_SIZE-1] & UpperMask) | (pVector[0] & LowerMask);
		pVector[MTVECTOR_SIZE-1] = pVector[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

		m_MTCounter = 0;
	}

	y  = pVector[m_MTCounter++];
	y ^= TemperingShiftU(y);
	y ^= TemperingShiftS(y) & TemperingMaskB;
	y ^= TemperingShiftT(y) & TemperingMaskC;
	y ^= TemperingShiftL(y);
	
	return y;
}


// 種をセットする //
FVOID CRnd::SetSeed(DWORD Seed)
{
	int			i;

	for(i=0; i<MTVECTOR_SIZE; i++){
		m_MTVector[i]  = Seed & 0xffff0000;
		Seed           = 69069 * Seed + 1;
		m_MTVector[i] |= ((Seed & 0xffff0000) >> 16);
		Seed           = 69069 * Seed + 1;
	}

	m_MTCounter = MTVECTOR_SIZE;
}


// タイマによる種のセット //
FVOID CRnd::Randomize(void)
{
	SetSeed(timeGetTime());
}



} // namespace Pbg
