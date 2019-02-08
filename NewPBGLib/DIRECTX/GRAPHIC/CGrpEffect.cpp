/*
 *   CGrpEffect.cpp   : CDIB32用エフェクト
 *
 */

#include "CTransTable.h"
#include "CGrpEffect.h"
#include "PbgError.h"
#include "PbgCPU.h"



namespace Pbg {



// コンストラクタ //
CDIB32Effect::CDIB32Effect()
{
	// 何もしません //
}


// デストラクタ //
CDIB32Effect::~CDIB32Effect()
{
	// 何もしません //
}


// 指定矩形内にモザイクをかける                             //
// 引数  rcTarget : エフェクト対象となる矩形(NULL 時は全体) //
//       Size     : モザイクのサイズ                        //
//       Alpha    : [-256, 0) 徐々に暗くする                //
//                : 0         そのままの明るさ              //
//                : (0, 256]  徐々に明るくする              //
FBOOL CDIB32Effect::EfcMosaic(const RECT *pTarget, int Size, int Alpha)
{
	RECT		rcClipped;

#ifdef PBG_DEBUG
	// エラーログ用の関数名をセット //
	PbgErrorInit("CDIB32Effect::EfcMosaic()");

	// ＤＩＢはまだ、作成されていない //
	if(NULL == m_pData){
		PbgErrorEx("ＤＩＢが作成されていません");
		return FALSE;
	}

	if(Alpha < -256 || Alpha > 256){
		PbgErrorEx("引数 Alpha は [-256, 256] の範囲で指定するのぢゃ");
	}
#endif

	// モザイクのサイズが不正である                           //
	// 注意：ちなみにサイズ１の場合、モザイク化する意味が無い //
	if(Size <= 1) return FALSE;

	// αの値を範囲内に制限する //
	if(     Alpha < -256) Alpha = -256;
	else if(Alpha >  256) Alpha =  256;

	// 範囲内に収まるようにクリッピングを掛ける //
	if(FALSE == ClipRect(&rcClipped, pTarget)) return TRUE;

	if(CCPUCheck::IsEnableMMX()) MosaicMMX(&rcClipped, Size, Alpha);
	else                         MosaicStd(&rcClipped, Size, Alpha);

	return TRUE;
}


// 指定された点から放射状にモーションブラーをかける         //
// 引数  rcTarget : エフェクト対象となる矩形(NULL 時は全体) //
//       ox, oy   : エフェクトの中心となる点                //
//       Alpha    : [-256, 0) 徐々に暗くする                //
//                : 0         そのままの明るさ              //
//                : (0, 256]  徐々に明るくする              //
FBOOL CDIB32Effect::EfcMotionBlur(const RECT *pTarget, int ox, int oy, int Alpha)
{
	RECT		rcClipped;

#ifdef PBG_DEBUG
	// エラーログ用の関数名をセット //
	PbgErrorInit("CDIB32Effect::EfcMotionBlur()");

	// ＤＩＢはまだ、作成されていない //
	if(NULL == m_pData){
		PbgErrorEx("ＤＩＢが作成されていません");
		return FALSE;
	}

	if(Alpha < -256 || Alpha > 256){
		PbgErrorEx("引数 Alpha は [-256, 256] の範囲で指定するのぢゃ");
	}
#endif

	// αの値を範囲内に制限する //
	if(     Alpha < -256) Alpha = -256;
	else if(Alpha >  256) Alpha =  256;

	// 範囲内に収まるようにクリッピングを掛ける //
	if(FALSE == ClipRect(&rcClipped, pTarget)) return TRUE;


	// 指定された点を範囲内に納める //
	if(     ox <  rcClipped.left ) ox = rcClipped.left;
	else if(ox >= rcClipped.right) ox = rcClipped.right - 1;

	if(oy < rcClipped.top)          oy = rcClipped.top;
	else if(oy >= rcClipped.bottom) oy = rcClipped.bottom - 1;


	if(CCPUCheck::IsEnableMMX()) MotionBlurMMX(&rcClipped, ox, oy, Alpha);
	else                         MotionBlurStd(&rcClipped, ox, oy, Alpha);

	return TRUE;
}


// テーブルに従って、変形を行う       //
// 引数 (ox, oy) : 開始点の座標       //
//        pSrc   : 元画像             //
//        pTable : 変換テーブルクラス //
FBOOL CDIB32Effect::EfcTransform(	int				ox
								 ,	int				oy
								 ,	CDIB32			*pSrc
								 ,	CTransTable8	*pTable)
{
	int			x, y, w, h, dx, dy;
	RECT		rcSrc, rcClipped;
	DWORD		*pDestData, *pSrcData;
	DWORD		DestPitch, SrcPitch;

	SetRect(&rcSrc, ox, oy, ox+pTable->GetWidth(), oy+pTable->GetHeight());
	if(FALSE == ClipRect(&rcClipped, &rcSrc)) return TRUE;

	w = rcClipped.right  - rcClipped.left - 1;
	h = rcClipped.bottom - rcClipped.top  - 1;

	DestPitch = m_dwWidth;
	SrcPitch  = pSrc->GetWidth();
	pDestData = m_pData           + rcClipped.left + (rcClipped.top * DestPitch);
	pSrcData  = pSrc->GetTarget();// + rcClipped.left + (rcClipped.top * Pitch);

	for(y=0; y<h; y++){
		for(x=0; x<w; x++){
			dx = dy = - ( *(pTable->GetPtrFromY(y) + x) );
			dx += *(pTable->GetPtrFromY(y) + x + 1);
			dy += *(pTable->GetPtrFromY(y + 1) + x);

			dx = (dx << 0) + x;
			dy = (dy << 0) + y;
/*
			if(     dx <  rcClipped.left)  dx = rcClipped.left;
			else if(dx >= rcClipped.right) dx = rcClipped.right - 1;

			if(     dy <  rcClipped.top)    dy = rcClipped.top;
			else if(dy >= rcClipped.bottom) dy = rcClipped.bottom - 1;
*/
			if(     dx <  0) dx = 0;
			else if(dx >= w) dx = w-1;

			if(     dy <  0) dy = 0;
			else if(dy >= h) dy = h-1;

			*(pDestData + x + (y * DestPitch)) = *(pSrcData + dx + (dy * SrcPitch));
		}
	}
/*
		lpM = (BYTE*)lpMap->GetlpScanLine(y);
		for(int x = 0;x < (int)m_dwWidth - 1;x ++)
		{
//			Diffx = *((BYTE*)(lpMap->GetlpScanLine(y)) + x + 1) - *((BYTE*)(lpMap->GetlpScanLine(y)) + x);
//			Diffy = *((BYTE*)(lpMap->GetlpScanLine(y + 1)) + x) - *((BYTE*)(lpMap->GetlpScanLine(y)) + x);


			Diffx = *(lpM + x + 1) - *(lpM + x);
			Diffy = *(lpM + lpMap->GetPitch() + x) - *(lpM + x);

			Desx = (Diffx << level) + x;
			Desy = (Diffy << level) + y;

			Desx = (Desx < 0) ? 0 : (Desx > (int)m_dwWidth - 1) ? m_dwWidth - 1 : Desx;
			Desy = (Desy < 0) ? 0 : (Desy > (int)m_dwHeight - 1) ? m_dwHeight - 1: Desy;

			*(lpD ++) = *((DWORD*)lpSrc->GetlpScanLine(Desy) + Desx);
		}
		lpD ++;
*/
	return TRUE;
}


// 非ＭＭＸモザイク //
FVOID CDIB32Effect::MosaicStd(RECT *pTarget, int Size, int Alpha)
{
	RGBQUAD			*pStart;	//
	RGBQUAD			*pBlock;	// 現在のブロックの右上
	RGBQUAD			*pDIB;		// 書き込み先のポインタ
	RGBQUAD			Data;		// ブロックの塗りつぶし色

	DWORD			Pitch;		// DWORD 単位でのピッチ
//	DWORD			Next;		// 次のブロックへの加算値
	DWORD			BLine;		// ブロック内の次のラインへの加算値
	DWORD			x, y;		// 縦＆横のループ用変数
	DWORD			bwidth;		// 現在のブロックの横幅
	DWORD			bheight;	// 現在のブロックの縦幅
	DWORD			nPixels;	// ブロック内のピクセル数
	DWORD			r, g, b;	// ｒｇｂ値(平均を求めるための総和)

	int				w, h;		// 対象となる矩形の幅＆高さ
	int				sx, sy;		// 開始ｘ,ｙ座標
	int				i, j;		// 全体のループ用変数
	int				ShiftX;		// 開始時のシフト値
	int				ShiftY;		// 開始時のシフト値


	Pitch  = m_dwWidth;
//	Next   = Pitch * Size;
	pStart = (RGBQUAD *)m_pData;//(m_pData + pTarget->left + (Pitch * pTarget->top));

	sx = pTarget->left;		// 描画矩形の左端
	sy = pTarget->top;		// 描画矩形の上端
	w  = pTarget->right;	// 描画矩形の右端＋１
	h  = pTarget->bottom;	// 描画矩形の下端＋１

	ShiftX = ((w - sx) >> 1) % Size;
	if(ShiftX) ShiftX = Size - ShiftX;

	ShiftY = ((h - sy) >> 1) % Size;
	if(ShiftY) ShiftY = Size - ShiftY;

	for(j=sy-ShiftY; j<h; j+=Size){
		for(i=sx-ShiftX; i<w; i+=Size){
			if(i < sx){
				bwidth = Size - ShiftX;
				if(bwidth <= 0) continue;	// 描画の必要なし
				pBlock = pStart + sx;
			}
			else{
				bwidth = (w-i < Size) ? w-i : Size;	// ブロックの幅
				pBlock = pStart + i;
			}

			if(j < sy){
				bheight = Size - ShiftY;
				if(bheight <= 0) continue;	// 描画の必要なし
				pBlock += (sy * Pitch);
			}
			else{
				bheight = (h-j < Size) ? h-j : Size;	// ブロックの高さ
				pBlock += (j * Pitch);
			}

			BLine   = Pitch - bwidth;				// ブロックの次のライン

			// ピクセル数 //
			nPixels = bwidth * bheight;

			// ブロックの右上の座標を求める //
			//pBlock = pStart + i + j * Pitch;

			// ピクセルのＲＧＢ値の合計を求める //
			pDIB = pBlock;	// 初期・描画先
			r = g = b = 0;	// 総和の格納先
			for(y=0; y<bheight; y++){
				for(x=0; x<bwidth; x++){
					r += (pDIB->rgbRed);	// 赤の値
					g += (pDIB->rgbGreen);	// 緑の値
					b += (pDIB->rgbBlue);	// 青の値

					pDIB++;
				}
				pDIB += BLine;
			}

			// 非ゼロなので、α値を反映する //
			if(Alpha){
				r = (r * (Alpha + 256)) >> 8;		// 赤
				g = (g * (Alpha + 256)) >> 8;		// 緑
				b = (b * (Alpha + 256)) >> 8;		// 青
			}

			// ピクセルのＲＧＢの平均を求める //
			Data.rgbRed   = (BYTE)min(255, r / nPixels);	// 赤の平均を飽和
			Data.rgbGreen = (BYTE)min(255, g / nPixels);	// 緑の平均を飽和
			Data.rgbBlue  = (BYTE)min(255, b / nPixels);	// 青の平均を飽和

			// 求めた結果をブロックに書き込む //
			pDIB = pBlock;
			for(y=0; y<bheight; y++){
				for(x=0; x<bwidth; x++){
					*pDIB = Data;
					pDIB++;
				}
				pDIB += BLine;
			}
		}
	}
}


// ＭＭＸモザイク //
FVOID CDIB32Effect::MosaicMMX(RECT *pTarget, int Size, int Alpha)
{
	// 未実装であります //
	MosaicStd(pTarget, Size, Alpha);
}


IDWORD GetBlurOffset(int Src, int Min, int Max, int Mid, int Len)
{
	int		temp;

	temp = Src + (Len * (Mid - Src)) / 49;

	if(temp <  Min) return Min;
	if(temp >= Max) return Max - 1;

	return temp;
}
DWORD	SrcXOffset[640][5];
DWORD	SrcYOffset[480][5];


// 非ＭＭＸブラー //
FVOID CDIB32Effect::MotionBlurStd(RECT *pTarget, int ox, int oy, int Alpha)
{
	int			i, j;
	int			XMin, YMin, XMax, YMax;
	DWORD		DibWidth;

	RGBQUAD		*pDIB;		// ＤＩＢ先頭へのポインタ
	RGBQUAD		*pCurrent;	// 現在の書き込み先
	RGBQUAD		*pSource;	// 現在の読み込み先
	int			r, g, b;	// 書き込む色
	int			d;

	XMin = pTarget->left;		XMax = pTarget->right;
	YMin = pTarget->top;		YMax = pTarget->bottom;

	// 範囲チェック //
	if((XMax - XMin) > 640) return;
	if((YMax - YMin) > 480) return;


	// 元画像の幅を求める(ピッチでは無いのに注意) //
	DibWidth = GetWidth();

	// 書き込み先へのポインタ //
	pDIB = (RGBQUAD *)GetTarget();

	// 中心からちょこんとずらす為の値 //
	d = min(GetWidth(), GetHeight()) / 40;

/*
	char buf[1024];
	wsprintf(buf, "(%d, %d)  (%d, %d, %d, %d)"
					, ox, oy
					, pTarget->left
					, pTarget->top
					, pTarget->right
					, pTarget->bottom);
	PbgError(buf);
*/

	// Ｘ方向のテーブルを生成する //
	for(i=XMin; i<ox; i++){
		SrcXOffset[i][0] = GetBlurOffset(i, XMin, XMax, ox+d, 1);
		SrcXOffset[i][1] = GetBlurOffset(i, XMin, XMax, ox+d, 2);
		SrcXOffset[i][2] = GetBlurOffset(i, XMin, XMax, ox+d, 3);
		SrcXOffset[i][3] = GetBlurOffset(i, XMin, XMax, ox+d, 4);
		SrcXOffset[i][4] = GetBlurOffset(i, XMin, XMax, ox+d, 6);
	}
	for(i=ox; i<XMax; i++){
		SrcXOffset[i][0] = GetBlurOffset(i, XMin, XMax, ox-d, 1);
		SrcXOffset[i][1] = GetBlurOffset(i, XMin, XMax, ox-d, 2);
		SrcXOffset[i][2] = GetBlurOffset(i, XMin, XMax, ox-d, 3);
		SrcXOffset[i][3] = GetBlurOffset(i, XMin, XMax, ox-d, 4);
		SrcXOffset[i][4] = GetBlurOffset(i, XMin, XMax, ox-d, 6);
	}

	// Ｙ方向のテーブルを生成する //
	for(i=YMin; i<oy; i++){
		SrcYOffset[i][0] = GetBlurOffset(i, YMin, YMax, oy+d, 1) * DibWidth;
		SrcYOffset[i][1] = GetBlurOffset(i, YMin, YMax, oy+d, 2) * DibWidth;
		SrcYOffset[i][2] = GetBlurOffset(i, YMin, YMax, oy+d, 3) * DibWidth;
		SrcYOffset[i][3] = GetBlurOffset(i, YMin, YMax, oy+d, 4) * DibWidth;
		SrcYOffset[i][4] = GetBlurOffset(i, YMin, YMax, oy+d, 6) * DibWidth;
	}
	for(i=oy; i<YMax; i++){
		SrcYOffset[i][0] = GetBlurOffset(i, YMin, YMax, oy-d, 1) * DibWidth;
		SrcYOffset[i][1] = GetBlurOffset(i, YMin, YMax, oy-d, 2) * DibWidth;
		SrcYOffset[i][2] = GetBlurOffset(i, YMin, YMax, oy-d, 3) * DibWidth;
		SrcYOffset[i][3] = GetBlurOffset(i, YMin, YMax, oy-d, 4) * DibWidth;
		SrcYOffset[i][4] = GetBlurOffset(i, YMin, YMax, oy-d, 6) * DibWidth;
	}

	// 左上 //
	for(j=oy-1; j>=YMin; j--){
		pCurrent = pDIB + j * DibWidth + ox - 1;

		for(i=ox-1; i>=XMin; i--){
			r = pCurrent->rgbRed   * 3;
			g = pCurrent->rgbGreen * 3;
			b = pCurrent->rgbBlue  * 3;

			pSource = pDIB + SrcXOffset[i][0] + SrcYOffset[j][0];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][1] + SrcYOffset[j][1];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][2] + SrcYOffset[j][2];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][3] + SrcYOffset[j][3];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][4] + SrcYOffset[j][4];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			if(Alpha){
				r = (r * (Alpha + 256)) >> 8;		// 赤
				g = (g * (Alpha + 256)) >> 8;		// 緑
				b = (b * (Alpha + 256)) >> 8;		// 青
			}

			pCurrent->rgbRed   = min(255, r / 8);
			pCurrent->rgbGreen = min(255, g / 8);
			pCurrent->rgbBlue  = min(255, b / 8);

			pCurrent--;
		}
	}

	// 右上 //
	for(j=oy-1; j>=YMin; j--){
		pCurrent = pDIB + j * DibWidth + ox;

		for(i=ox; i<XMax; i++){
			r = pCurrent->rgbRed   * 3;
			g = pCurrent->rgbGreen * 3;
			b = pCurrent->rgbBlue  * 3;

			pSource = pDIB + SrcXOffset[i][0] + SrcYOffset[j][0];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][1] + SrcYOffset[j][1];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][2] + SrcYOffset[j][2];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][3] + SrcYOffset[j][3];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][4] + SrcYOffset[j][4];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			if(Alpha){
				r = (r * (Alpha + 256)) >> 8;		// 赤
				g = (g * (Alpha + 256)) >> 8;		// 緑
				b = (b * (Alpha + 256)) >> 8;		// 青
			}

			pCurrent->rgbRed   = min(255, r / 8);
			pCurrent->rgbGreen = min(255, g / 8);
			pCurrent->rgbBlue  = min(255, b / 8);

			pCurrent++;
		}
	}

	// 右下 //
	for(j=oy; j<YMax; j++){
		pCurrent = pDIB + j * DibWidth + ox - 1;

		for(i=ox-1; i>=XMin; i--){
			r = pCurrent->rgbRed   * 3;
			g = pCurrent->rgbGreen * 3;
			b = pCurrent->rgbBlue  * 3;

			pSource = pDIB + SrcXOffset[i][0] + SrcYOffset[j][0];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][1] + SrcYOffset[j][1];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][2] + SrcYOffset[j][2];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][3] + SrcYOffset[j][3];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][4] + SrcYOffset[j][4];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			if(Alpha){
				r = (r * (Alpha + 256)) >> 8;		// 赤
				g = (g * (Alpha + 256)) >> 8;		// 緑
				b = (b * (Alpha + 256)) >> 8;		// 青
			}

			pCurrent->rgbRed   = min(255, r / 8);
			pCurrent->rgbGreen = min(255, g / 8);
			pCurrent->rgbBlue  = min(255, b / 8);

			pCurrent--;
		}
	}

	// 左下 //
	for(j=oy; j<YMax; j++){
		pCurrent = pDIB + j * DibWidth + ox;

		for(i=ox; i<XMax; i++){
			r = pCurrent->rgbRed   * 3;
			g = pCurrent->rgbGreen * 3;
			b = pCurrent->rgbBlue  * 3;

			pSource = pDIB + SrcXOffset[i][0] + SrcYOffset[j][0];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][1] + SrcYOffset[j][1];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][2] + SrcYOffset[j][2];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][3] + SrcYOffset[j][3];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			pSource = pDIB + SrcXOffset[i][4] + SrcYOffset[j][4];
			r += pSource->rgbRed;
			g += pSource->rgbGreen;
			b += pSource->rgbBlue;

			if(Alpha){
				r = (r * (Alpha + 256)) >> 8;		// 赤
				g = (g * (Alpha + 256)) >> 8;		// 緑
				b = (b * (Alpha + 256)) >> 8;		// 青
			}

			pCurrent->rgbRed   = min(255, r / 8);
			pCurrent->rgbGreen = min(255, g / 8);
			pCurrent->rgbBlue  = min(255, b / 8);

			pCurrent++;
		}
	}
}


// ＭＭＸブラー //
FVOID CDIB32Effect::MotionBlurMMX(RECT *pTarget, int ox, int oy, int Alpha)
{
	MotionBlurStd(pTarget, ox, oy, Alpha);
}


// このＤＩＢの範囲に収まるようにクリッピングを掛ける //
// 戻り値  TRUE  : 正常にクリッピングされた
//         FALSE : 指定された矩形は完全に範囲外である
FBOOL CDIB32Effect::ClipRect(RECT *pDest, const RECT *pSrc)
{
	int		w, h;

	// ＮＵＬＬの場合は、ＤＩＢ全体の指定となる //
	if(NULL == pSrc){
		SetRect(pDest, 0, 0, m_dwWidth, m_dwHeight);
		return TRUE;
	}

	w = m_dwWidth;
	h = m_dwHeight;

	// 範囲内に存在しているかどうかをチェックする //
	if((pSrc->right < 0  ) || (pSrc->bottom < 0  )
	|| (pSrc->left  > w+1) || (pSrc->top    > h-1)) return FALSE;

	// 左端クリッピング //
	if(pSrc->left < 0) pDest->left = 0;
	else               pDest->left = pSrc->left;

	// 上端クリッピング //
	if(pSrc->top < 0) pDest->top = 0;
	else              pDest->top = pSrc->top;

	// 右端クリッピング //
	if(pSrc->right > w) pDest->right = w;
	else                pDest->right = pSrc->right;

	// 下端クリッピング //
	if(pSrc->bottom > h) pDest->bottom = h;
	else                 pDest->bottom = pSrc->bottom;

	return TRUE;
}



} // namespace Pbg
