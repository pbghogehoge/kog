/*
 *   CFont.cpp   : フォント
 *
 */

#include "CFont.h"
#include "PbgMem.h"



namespace Pbg {



// コンストラクタ //
CFont::CFont()
{
	m_FontColor    = RGB(255, 255, 255);	// 文字の色
	m_BkBlendColor = 0;						// 背景の合成色
	m_FontSize     = 24;					// フォントサイズ
	m_pFontName    = NULL;					// フォントの名前
	m_pString      = NULL;					// 表示する文字列

	m_bNeedUpdate = TRUE;				// 更新が必要か
}


// デストラクタ //
CFont::~CFont()
{
	// 文字列格納用のメモリを解放する                                   //
	// 参考：文字列そのものは配列管理ではなく\0で切って管理しているので //
	//       解放はこれだけで十分である                                 //
	if(m_pString) MemFree(m_pString);
}


// 文字列をセットする //
BOOL CFont::SetText(char *pString)
{
	int		len;
	int		i;

	// すでに、文字列が割り当てられている場合、解放する //
	if(m_pString) MemFree(m_pString);

	len = strlen(pString) + 2;			// 文字列長を求める(元の値+ヌル２つ)
	m_pString = (char *)MemAlloc(len);	// 文字列格納用にメモリを確保
	if(NULL == m_pString) return FALSE;	// メモリを解放する

	// 改行を \0 に変換しながら文字列のコピーを行う //
	for(i=0; i<len-2; i++){
		if('\n' == pString[i]) m_pString[i] = '\0';			// ヌルに変換
		else                   m_pString[i] = pString[i];	// そのまま埋め込み
	}

	// 文字列の終端を \0\0 にする //
	pString[len-1] = pString[len-2] = '\0';

	m_bNeedUpdate = TRUE;		// 更新要求を出す

	return TRUE;		// 正常終了
}


// フォントのサイズをセットする //
BOOL CFont::SetSize(int Size)
{
	// サイズが不正 //
	if(Size <= 0) return FALSE;

	m_FontSize    = Size;	// サイズを変更する
	m_bNeedUpdate = TRUE;	// 更新要求

	return TRUE;	// 正常終了
}


// 文字色をセットする //
BOOL CFont::SetColor(COLORREF Color)
{
	m_FontColor   = Color;		// 色をセットする
	m_bNeedUpdate = TRUE;		// 更新要求

	return TRUE;	// 正常終了
}


// 背景の合成色をセットする //
BOOL CFont::SetBkBlendColor(COLORREF Color)
{
	m_BkBlendColor = Color;		// 色をセット
	m_bNeedUpdate  = TRUE;		// 更新要求

	return TRUE;	// 正常終了
}


// フォント名をセットする //
BOOL CFont::SetFontID(BYTE FontID)
{
	static char szMS_GOTHIC[] = "ＭＳ ゴシック";
	static char szMS_MINCHO[] = "ＭＳ 明朝";

	// 指定されたＩＤによって、フォント名へのポインタを変更する //
	switch(FontID){
		case FONTID_MS_GOTHIC:	// ＭＳゴシック
			m_pFontName = szMS_GOTHIC;
		break;

		case FONTID_MS_MINCHO:	// ＭＳ明朝
			m_pFontName = szMS_MINCHO;
		break;

		case FONTID_DEFAULT:	default:	// デフォルトのフォント
			m_pFontName = NULL;
		break;
	}

	m_bNeedUpdate = TRUE;	// 更新要求

	return TRUE;	// 正常終了
}


// フォント情報を更新する //
BOOL CFont::Update(void)
{
	HFONT		hFont;			// フォントハンドル
	HFONT		hOldFont;		// 一つ古いオブジェクト
	LONG		Width, Height;	// ＤＩＢの幅と高さ
	LONG		Pitch;			// １ラインの高さ
	DWORD		x, y;			// 文字列の書き込み左上
	int			i, j;			// ループ用
	int			Length;			// 文字列長
	char		*pText;			// 文字列をたどる
	HDC			hDC;			// このフォント用ＤＩＢのデバイスコンテキスト
	DWORD		*p1, *p2, *p3;	// α情報作成用のポインタ
	DWORD		Alpha, Beta;	// 得られたα値
	RGBQUAD		*pRGBTemp;		// 色のセット用

	RGBQUAD		CTable[9+1];	// アンチェリ用テーブル
	RGBQUAD		*pTable;		// フォント色(へのポインタ)
	RGBQUAD		*pBack;			// 背景色(へのポインタ)


	// すでに更新は完了している //
	if(FALSE == m_bNeedUpdate) return TRUE;

	// フォントを作成する //
	hFont = CreateFont(m_FontSize * 3, 0	// フォントの幅と高さ(サイズは２倍)
					, 0						// 文字送り方向の角度
					, 0						// ベースラインの角度
					, FW_NORMAL				// フォントの太さ
					, FALSE					// イタリックにするか
					, FALSE					// 下線を引くかどうか
					, FALSE					// 打ち消し線を引くかどうか
					, SHIFTJIS_CHARSET			// 文字セット(Shift JIS)
					, OUT_DEFAULT_PRECIS		// 出力精度
					, CLIP_DEFAULT_PRECIS		// クリッピング精度
					, NONANTIALIASED_QUALITY	// 出力品質
					, FIXED_PITCH|FF_MODERN		// フォントのピッチとファミリ
					, m_pFontName				// フォント名へのポインタ
			);
	if(NULL == hFont) return FALSE;

	// テキスト格納に必要なサイズを調べる //
	if(FALSE == GetTextSize(hFont, &Width, &Height, &Pitch)){
		DeleteObject(hFont);
		return FALSE;
	}

	// ＤＩＢを作成する(前のＤＩＢの解放は CDIB32 に委託) //
	if(FALSE == Create(Width, Height)){
		DeleteObject(hFont);
	}

	hDC = GetDC();

	// フォントを接続 //
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	SetTextColor(hDC, m_FontColor);	// 文字色をセットする
	SetBkMode(hDC, TRANSPARENT);	// 透過モードに設定

	x      =  0;	// 文字列の描画開始: Ｘ座標
	y      =  0;	// 文字列の描画開始: Ｙ座標
	Length = -1;	// 一応、初期化する(初期加算値が０になるように)

	// 文字列リストの終端は \0\0 である点に注意 //
	for(pText=m_pString; *pText!='\0'; pText+=(Length+1)){
		Length = strlen(pText);					// 文字列長を算出
		TextOut(hDC, x, y, pText, Length);

		y += Pitch;		// 次の行へ
	}

	// 参考：ここで行われるフォントの処理では、９点の加重和平均を             //
	//       求めることで実現するので、処理の後で出現する色数は９＋１色となる //
	//       (＋１は透過色)                                                   //

	// 透明色の初期化(Alpha = 0) //
	CTable[0].rgbRed      = 0;		// 赤
	CTable[0].rgbGreen    = 0;		// 緑
	CTable[0].rgbBlue     = 0;		// 青
	CTable[0].rgbReserved = 0;		// コレは無視していい

	pTable = (RGBQUAD *)(&m_FontColor);		// フォントの色
	pBack  = (RGBQUAD *)(&m_BkBlendColor);	// フォントの背景ブレンド色

	for(i=1; i<10; i++){
		Alpha = 256 * i / 9;	// 文字α値
		Beta  = 256 - Alpha;	// 背景のα値

		// ＲＧＢ値をそれぞれ求める //
		CTable[i].rgbRed   = min(255, ((pTable->rgbRed   * Alpha) + (pBack->rgbRed   * Beta)) >> 8);
		CTable[i].rgbGreen = min(255, ((pTable->rgbGreen * Alpha) + (pBack->rgbGreen * Beta)) >> 8);
		CTable[i].rgbBlue  = min(255, ((pTable->rgbBlue  * Alpha) + (pBack->rgbBlue  * Beta)) >> 8);

		// これは単純にゼロ初期化を //
		CTable[i].rgbReserved = 0;
	}

	// ９点の加重和平均を求める //
	for(j=0; j<Height; j+=3){
		// 上から順に１列目、２列目、３列目となる //
		p1 = (DWORD *)(((BYTE *)GetTarget()) + GetPitch() * j);
		p2 = (DWORD *)(((BYTE *)p1) + GetPitch());
		p3 = (DWORD *)(((BYTE *)p2) + GetPitch());

		// 書き込み先へのポインタ //
		pRGBTemp = (RGBQUAD *)(((BYTE *)GetTarget()) + (GetPitch() * j)/3);

		// １列分の処理を行う(読み込みは３列) //
		for(i=0; i<Width; i+=3){
			pTable = CTable;

			if(*p1)     pTable++;
			if(*(p1+1)) pTable++;
			if(*(p1+2)) pTable++;
			if(*p2)     pTable++;
			if(*(p2+1)) pTable++;
			if(*(p2+2)) pTable++;
			if(*p3)     pTable++;
			if(*(p3+1)) pTable++;
			if(*(p3+2)) pTable++;

			*pRGBTemp = *pTable;	// 処理後のデータを書き込む
			p1+=3;					// １列目の前進
			p2+=3;					// ２列目の前進
			p3+=3;					// ３列目の前進
			pRGBTemp++;				// 書き込み先の前進
		}
	}

	SelectObject(hDC, hOldFont);	// フォントを元に戻す
	DeleteObject(hFont);			// フォントを削除する

	// 更新完了 //
	m_bNeedUpdate = FALSE;

	return TRUE;
}


// 指定されたフォントで描画するのに必要なサイズを求める //
// pWidth, pHeight : ＤＩＢのサイズ格納先へのポインタ   //
// pLine           : 縦幅の最大値格納先へのポインタ     //
BOOL CFont::GetTextSize(HFONT hFont, LONG *pWidth, LONG *pHeight, LONG *pLine)
{
	HDC			hDC;		// デバイスコンテキスト
	HFONT		hOldFont;	// 昔のフォント(リソースを守れ！)
	SIZE		Size;		// サイズ格納用の構造体だってさ
	char		*pText;		// 文字列をたどる為に使用する
	int			Length;		// 文字列長の格納用

	// 幅と高さを０初期化する //
	*pWidth  =  0;	// 幅
	*pHeight =  0;	// 高さ
	*pLine   =  0;	// ピッチ
	Length   = -1;	// 一応、初期化する(初期加算値が０になるように)

	// パラメータのチェック //
	if((NULL == pWidth) || (NULL == pHeight)) return FALSE;	// 幅＆高さの格納先
	if(NULL == hFont)                         return FALSE;	// フォント

	// いわゆる初期化作業の第２段階じゃ //
	hDC      = CreateCompatibleDC(NULL);		// デバイスコンテキスト取得
	hOldFont = (HFONT)SelectObject(hDC, hFont);	// フォントを接続

	// 文字列リストの終端は \0\0 である点に注意 //
	for(pText=m_pString; *pText!='\0'; pText+=(Length+1)){
		Length = strlen(pText);								// 文字列長を算出
		GetTextExtentPoint32(hDC, pText, Length, &Size);

		// GetTextExtentPoint32 で１ドットのずれが生じる事があるらしいので、//
		// １ドットだけ大きくとることにする(ＧＤＩのバグ？)         //
		(*pWidth)   = max(Size.cx, (*pWidth));	// 幅の最大値を求める
		(*pLine)    = max(Size.cy, (*pLine));	// 高さの最大値を求める
		(*pHeight) += (Size.cy + 1);			// 高さを加算する
	}

	// 上に挙げたことと同じ理由で、幅、高さに修正を加える //
	// なお、高さと幅は３の倍数になるようにする           //
	(*pWidth)  = ((*pWidth)  / 3) * 3;
	(*pHeight) = ((*pHeight) / 3) * 3;
	//(*pWidth)  = ((*pWidth) +2) & (~1);	// (ＤＩＢの幅   ＋ ２) & (~1)
	//(*pHeight) = ((*pHeight)+2) & (~1);	// (ＤＩＢの高さ ＋ ２) & (~1)

	// 後始末する //
	SelectObject(hDC, hOldFont);
	DeleteDC(hDC);

	return TRUE;
}



}
