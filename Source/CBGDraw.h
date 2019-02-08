/*
 *   CBGDraw.h   : 背景の描画
 *
 */

#ifndef CBGDRAW_INCLUDED
#define CBGDRAW_INCLUDED "背景の描画   : Version 0.01 : Update 2001/03/20"

/*  [更新履歴]
 *    Version 0.01 : 2001/03/20 : 仮決定
 */



#include "Gian2001.h"



/***** [ 定数 ] *****/
#define BG_NUM_DIVISION		3							// 分割数
#define BG_WIDTH			BG_NUM_DIVISION				// 背景の幅(最大)
#define BG_HEIGHT			BG_NUM_DIVISION				// 背景の高さ(最大)
#define BG_NUM_VERTICES		200							// 頂点数
#define BG_Z_DEFAULT		(256 * 256)					// Ｚ座標原点



/***** [クラス定義] *****/
class CBGDraw {
public:
	void Initialize(void);		// 座標関連の初期化
	void Move(void);			// １フレーム進行させる
	void Draw(void);			// 描画する

	// 背景テクスチャへの合成色を指定する //
	void SetBlendColor(BYTE r, BYTE g, BYTE b, DWORD Count);

	// ボス出現用エフェクト //
	void SetBossAlive(BOOL bBossAlive);

	void SetSpeed(int Speed);			// スクロールスピードをセットする
	void SetAngleA(BYTE Angle);			// 進行方向を変化させる(絶対指定)
	void SetAngleR(char dAngle);		// 進行方向を変化させる(相対指定)
	void SetZoom(int NewZ, int Speed);	// Ｚ座標を変化させる

	FINT GetTargetWidth(void);	// 画面幅の取得
	FINT GetTargetOx(void);		// 画面中央の取得

	CBGDraw(RECT *rcTargetX256);	// コンストラクタ
	~CBGDraw();						// デストラクタ


private:
	int		m_WindowOx;		// グラフィック座標におけるＸ原点
	int		m_WindowOy;		// グラフィック座標におけるＹ原点
	int		m_WindowWidth;	// グラフィック座標・ウィンドウの幅
	int		m_WindowHeight;	// グラフィック座標・ウィンドウの高さ

	int		m_CurrentX;		// 現在のＸ座標
	int		m_CurrentY;		// 現在のＹ座標
	int		m_CurrentZ;		// 現在のＺ座標
	int		m_TargetZ;		// 目標のＺ座標

	int		m_Speed;		// ＸＹ平面上の移動速度
	int		m_ZSpeed;		// Ｚ軸方向の移動速度

	DWORD	m_BlendStart;	// 合成モード開始カウント
	DWORD	m_BlendCount;	// 色合成用カウンタ
	DWORD	m_CurrentColor;	// 現在の合成色

	BYTE	m_StartRed;		// 開始時の合成色(赤)
	BYTE	m_StartGreen;	// 開始時の合成色(緑)
	BYTE	m_StartBlue;	// 開始時の合成色(青)

	BYTE	m_TargetRed;	// 目標合成色(赤)
	BYTE	m_TargetGreen;	// 目標合成色(緑)
	BYTE	m_TargetBlue;	// 目標合成色(青)

	BYTE	m_BossFade;		// ボス出現時の画面フェード用
	BYTE	m_BossAlive;	// ボスが生きていれば非ゼロ

	BYTE	m_Angle;		// 進行方向

	static D3DTLVERTEX		m_RenderList[BG_NUM_VERTICES];
};



#endif
