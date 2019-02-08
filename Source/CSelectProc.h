/*
 *   CSelectProc.h   : 選択画面
 *
 */

#ifndef CSELECTPROC_INCLUDED
#define CSELECTPROC_INCLUDED "選択画面     : Version 0.01 : Update 2001/07/20"

/*  [更新履歴]
 *    Version 0.01 : 2001/07/20 : 制作開始
 */



#include "Gian2001.h"
#include "CGameMain.h"
#include "CScene.h"



/***** [クラス定義] *****/

// キャラクタ情報表示用構造体 //
class SPCharInfo {
public:
	// その名の通り、初期化 //
	FVOID Initialize(BYTE PlayerID, CHARACTER_ID CharID);

	FVOID Proceed(WORD KeyCode, int NumCharacters);	// 更新処理
	FVOID DrawCharName(int ox, int oy);				// キャラクタ名の描画

	// キャラクタＩＤを取得する //
	CHARACTER_ID GetCharacterID(void);

	SPCharInfo();		// コンストラクタ
	~SPCharInfo();		// デストラクタ

	BYTE	m_SelectedAlpha;	// 選択されていれば非ゼロ
	int		m_DispParams[5];	// 各種パラメータ
	int		m_BinTexY;			// バイナリテクスチャの始点Ｙ座標
	BYTE	m_Angle;			// ぐるぐる回っている奴の角度
	BYTE	m_AutoRepCount;		// オートリピート用カウンタ
	BOOL	m_bRandomSelect;	// ランダムセレクトフラグ


private:
	FVOID Select(CHARACTER_ID CharID);		// キャラクタ選択
	FVOID InitCharParams(void);				// パラメータの最大値をセット

	DWORD	m_BinTexMode;	// バイナリテクスチャの動作モード
	DWORD	m_BinTexCount;	// バイナリテクスチャの動作モード

	int		m_Params[5];	// 各種パラメータ

	CHARACTER_ID	m_CharacterID;	// キャラクターＩＤ
	BYTE			m_PlayerID;		// プレイヤーＩＤ

//	Pbg::CFont			m_CharName;
	Pbg::CGrpSurface	*m_pNameSurf;

	Pbg::CRnd			m_Rnd;

	static BOOL		m_bInitialized;		// 静的メンバが初期化されていれば真
	static int		m_MaxParam[5];		// 各パラメータの最大値
};


// 選択画面管理クラス //
class CSelectProc : public CScene {
public:
	IBOOL      Initialize(void);	// 初期化をする
	PBG_PROCID Move(void);			// １フレーム分だけ動作させる
	IVOID      Draw(void);			// 描画する

	CSelectProc(CGameInfo *pSys);	// コンストラクタ
	~CSelectProc();					// デストラクタ


private:
	FVOID DrawWeapons(int sx, CHARACTER_ID CharID);

	FVOID SetCharacterID(void);		// キャラクタ選択を反映する

	FVOID DrawBinTexture(void);		// バイナリ文字列の描画を行う
	FVOID DrawBlackWindow(void);	// 黒の半透明窓を描画する
	FVOID DrawCharacter(void);		// キャラクタの顔を描画する
	FVOID DrawParams(void);			// パラメータを描画する

	SPCharInfo			m_CharInfo[2];	// キャラクタ情報
	Pbg::CGrpSurface	*m_pFrame;		// 選択画面枠
	Pbg::CGrpSurface	*m_pSE;			// うえぽん
	BOOL				m_KeyRep;		// 推移時のキー押し続け対策
	CGameInfo			*m_pSys;
};



#endif
