/*
 *   CWaterEfc.h   : 水系エフェクト
 *
 */

#ifndef CWATEREFC_INCLUDED
#define CWATEREFC_INCLUDED "水系エフェクト : Version 0.01 : Update 2001/09/08"

/*  [更新履歴]
 *    Version 0.01 : 2001/09/08 : 制作開始
 */



#include "CTransTable.h"



namespace Pbg {



/***** [クラス定義] *****/

// 水系エフェクト //
class CWaterEfc {
public:
	FBOOL Create(int Width, int Height);			// 作成する
	FVOID Proceed(void);							// １フレーム進める
	FVOID SetRain(int x, int y, BYTE Power);		// 雨粒を降らせる
	FBOOL IsFinished(void){ return m_bFinished;};	// 終了していれば真を返す

	// 現在アクティブな変換テーブルを取得する //
	CTransTable8 *GetTransTable(void){
		return &m_Table[m_Stage];
	}

	CWaterEfc();	// コンストラクタ
	~CWaterEfc();	// デストラクタ


private:
	CTransTable8	m_Table[2];		// 変換テーブル
	int				m_Stage;		// どちらのテーブルを使用するか
	BOOL			m_bFinished;	// エフェクトが完了していれば真
};



} // namespace Pbg



#endif
