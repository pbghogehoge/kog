/*
 *   PbgError.h   : エラー処理
 *
 */

#ifndef PBGERROR_INCLUDED
#define PBGERROR_INCLUDED "エラー処理   : Version 0.06 : Update 2001/10/30"

/*  [更新履歴]
 *    Version 0.06 : 2001/10/30 : Win2k で死んだのでデバッグ
 *
 *    Version 0.05 : 2001/03/04 : .SetFilename() にカレントディレクトリを修正
 *                              : する機能を追加した
 *
 *    Version 0.04 : 2001/01/24 : STDIO 系関数をＡＰＩに変更
 *    Version 0.03 : 2001/01/18 : 微調整
 *    Version 0.02 : 2001/01/16 : 参照カウンタによるメッセージ制御の追加
 *    Version 0.01 : 2001/01/15 : 行番号・ファイル名出力に対応した
 */



#include "PbgType.h"



/***** [ マクロ ] *****/
#ifdef PBG_DEBUG
	#include <comdef.h>

	// エラー出力＆ログ出力(デバッグ用) //
	#define PbgError(msg)		PbgEx::CError.PushError(__FILE__, __LINE__, msg)
	#define PbgDebugLog(msg)	PbgEx::CError.PushMessage(msg);

	// ＣＯＭ用エラーメッセージ //
	#define PbgComError(hr, msg)			\
	{										\
		_com_error		e(hr);				\
		char			temp[1024];			\
		strcpy(temp, msg);					\
		strcat(temp, "\r\n  [原因] : ");	\
		strcat(temp, e.ErrorMessage());		\
		PbgError(temp);						\
	}

#else
	// エラー出力＆ログ出力(通常版) //
	#define PbgError(msg)			PbgEx::CError.PushMessage(msg)
	#define PbgDebugLog(msg)		// 無効化する

	#define PbgComError(hr, msg)	PbgEx::CError.PushMessage(msg)
#endif

// 関数名付きエラーメッセージ支援用マクロ //
#define PbgErrorInit(FunctionName)						\
	const char	*_PBGERR_FuncName  = FunctionName" : ";	\
	char		_PBGERR_ErrBuffer[MAX_PATH];


// 関数名付きエラーメッセージ出力 //
#define PbgErrorEx(msg)								\
{													\
	strcpy(_PBGERR_ErrBuffer, _PBGERR_FuncName);	\
	strcat(_PBGERR_ErrBuffer, msg);					\
	PbgError(_PBGERR_ErrBuffer);					\
}

// ログを出力する(通常メッセージ) //
#define PbgLog(msg)			PbgEx::CError.PushMessage(msg)
#define PbgLog2(msg)		PbgEx::CError.PushMessage2(msg)

// ログ、エラーの出力先ファイル名を指定する //
#define PbgLogName(name)	PbgEx::CError.SetFilename(name, TRUE)



/***** [クラス定義] *****/
class CErrorCtrl {
public:
	// エラー出力先のファイル名を指定する //
	BOOL SetFilename(const char *FileName, BOOL DeleteOldFile);

	// エラーを出力する(追加モードで) //
	BOOL PushError(char *SrcName, DWORD Line, char *Message);

	// メッセージを出力する(追加モードで) //
	BOOL PushMessage(char *Message);
	BOOL PushMessage2(char *Message);	// ラインの追加無し

	// メッセージボックスに全エラー内容を表示する //
	void MsgBox(void);

	CErrorCtrl();		// コンストラクタ
	~CErrorCtrl();		// デストラクタ

private:
	char	m_FileName[MAX_PATH];	// ファイル名
	DWORD	m_PushCount;			// プッシュした回数
	BOOL	m_RefCount;				// 参照カウンタ
};



/***** [グローバル変数] *****/

// エラー処理は名前空間 PbgEx に含まれる //
namespace PbgEx {
	extern CErrorCtrl		CError;
}



#endif
