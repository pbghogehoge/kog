/*
 *   PbgError.cpp   : エラー処理
 *
 */

#include "PbgError.h"
#include "PbgFile.h"
#include "PbgMem.h"
//#include <stdio.h>



// コンストラクタ //
CErrorCtrl::CErrorCtrl()
{
	ZEROMEM(m_FileName);	// ファイル名
	m_PushCount = 0;		// 出力した回数
	m_RefCount  = 0;		// 参照した回数
}


// デストラクタ
CErrorCtrl::~CErrorCtrl()
{
	// エラー出力がある場合は、メッセージボックス //
	if(0 != m_PushCount) MsgBox();
}


// エラー出力先のファイル名を指定する //
BOOL CErrorCtrl::SetFilename(const char *FileName, BOOL DeleteOldFile)
{
	Pbg::CPbgFile	File;			// ファイル管理クラス
	char			buf[1024];		// 書き込み用バッファ
	SYSTEMTIME		SysTime;		// 現在時刻の格納先

	char	ModulePath[MAX_PATH];	// 起動した輩の名前
	char	TargetName[MAX_PATH];	// 出力対象となるファイル名
	char	*p;

	ZEROMEM(m_FileName);	// ファイル名を無効化
	m_PushCount = 0;	// 出力した回数をゼロ初期化
	m_RefCount  = 0;		// 参照した回数もゼロ初期化

	GetModuleFileName(NULL, ModulePath, MAX_PATH);

	for(p=ModulePath+strlen(ModulePath); p>ModulePath; p--){
		if('\\' == *p){
			*(p+1) = '\0';
			break;
		}
	}


	// カレントディレクトリをプログラムの存在するフォルダに変更する     //
	// 注意：これは、ファイルをドロップした場合にカレントディレクトリが //
	//       変更されてしまう事への対策である                           //
	SetCurrentDirectory(ModulePath);

	strcpy(TargetName, ModulePath);
	strcat(TargetName, FileName);

	// 前のファイルを消去するフラグが立っている場合 //
	if(DeleteOldFile) DeleteFile(TargetName);

	// ファイルをオープンする //
	if(FALSE == File.Open(TargetName, "a")) return FALSE;

	// ファイルがオープンできたので、名前を覚えておこう
	strcpy(m_FileName, TargetName);

	// 今回のログの目印用に線を引っ張っておきましょう //
	wsprintf(buf, "\r\n****************************************"
					"***************************************\r\n");
	File.WriteStr(buf);

	// 時刻を取得し、書式化して、ファイルに書き込む //
	GetLocalTime(&SysTime);
	wsprintf(buf, "\t\t起動時刻 : %4d/%2d/%2d   ( %02d:%02d:%02d )\r\n",
				SysTime.wYear, SysTime.wMonth, SysTime.wDay,
				SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	File.WriteStr(buf);

	wsprintf(buf, "\t\t起動パス : %s\r\n\r\n", ModulePath);
	File.WriteStr(buf);

	// ファイルをクローズする //
	File.Close();

	// ファイル名のセットに成功 //
	return TRUE;
}


// 直線用... //
static char *Line80 = "----------------------------------------"
						"---------------------------------------\r\n";


// エラーを出力する(追加モードで) //
BOOL CErrorCtrl::PushError(char *SrcName, DWORD Line, char *Message)
{
//	FILE		*fp;							// ファイルポインタ
	Pbg::CPbgFile	File;
	char			buf[1024];
	char			*p;								// ファイル名のパス削除用
	char			*Unknown = "ファイル名不明";	// ダミーファイル名

	// オープンするファイルが決まっていない //
	if('\0' == m_FileName[0]) return FALSE;

	// ファイルをオープンする //
	if(FALSE == File.Open(m_FileName, "a")) return FALSE;
//	fp = fopen(m_FileName, "a");
//	if(NULL == fp) return FALSE;	// オープンに失敗だ

	// エラーの書き込み回数をインクリメントする //
	m_PushCount++;

	// 参照回数をインクリメントする //
	m_RefCount++;

	// ファイル名(__FILE__ だから、一般にフルパス)のパスを削除する //
	if(SrcName){
		for(p=SrcName+strlen(SrcName); p>SrcName; p--){
			if('\\' == *p){
				p++;
				break;
			}
		}
	}
	// ファイル名が NULL で指定されていた場合 //
	else{
		// ファイル名の指定が無かったので、
		p = Unknown;
	}

	// エラーを各種情報とともに出力する //
	wsprintf(buf, "%s%2d : [%d 行] \"%s\" にエラーがあるのだ\r\n%s\r\n\r\n"
			, Line80, m_PushCount, Line, p, Message);
	File.WriteStr(buf);

	// ファイルをクローズする //
	File.Close();
	//fclose(fp);

	// 正常終了 //
	return TRUE;
}


// メッセージを出力する(追加モードで) //
BOOL CErrorCtrl::PushMessage(char *Message)
{
//	FILE		*fp;	// ファイルポインタ
	Pbg::CPbgFile		File;
	char				buf[1024];

	if('\0' == m_FileName[0]) return FALSE;

	// ファイルをオープンする //
	if(FALSE == File.Open(m_FileName, "a")) return FALSE;

	// 参照回数をインクリメント //
	m_RefCount++;

	// ライン＆メッセージの出力 //
	wsprintf(buf, "%s%s\r\n\r\n", Line80, Message);
	File.WriteStr(buf);

	// ファイルをクローズする //
	File.Close();

	// 正常終了 //
	return TRUE;
}


BOOL CErrorCtrl::PushMessage2(char *Message)
{
	Pbg::CPbgFile		File;
	char				buf[1024];

	// 開くファイルが決まっていない //
	if('\0' == m_FileName[0]) return FALSE;

	// ファイルをオープンする //
	if(FALSE == File.Open(m_FileName, "a")) return FALSE;

	// 参照回数をインクリメントする //
	m_RefCount++;

	// メッセージの出力 //
	wsprintf(buf, "%s\r\n", Message);
	File.WriteStr(buf);

	// ファイルをクローズする //
	File.Close();

	// 正常終了 //
	return TRUE;
}


// メッセージボックスに全エラー内容を表示する //
void CErrorCtrl::MsgBox(void)
{
//	FILE		*fp;
	Pbg::CPbgFile	File;
	char			buf[2048];
	char			*p;
	DWORD			size;
	int				count = 0;

	char		msg[] =
		">>>> メッセージが３０行を越えましたよぉ <<<<\r\n";

	// ファイル名が決まっていない場合 //
	if('\0' == m_FileName[0]) return;

	// 参照の回数が０回の場合 //
//	if(0 == m_RefCount) return;

	// ファイルを読み込みモード(他とは異なる)で開く //
	if(FALSE == File.Open(m_FileName, "r")) return;

	// 出力用バッファにメッセージを流す //
	ZEROMEM(buf);									// ゼロ初期化
	File.Read(buf, 2048-strlen(msg)-10, &size);
	buf[size] = '\0';

	// ファイルをクローズする //
	File.Close();
	//fclose(fp);

	// ３０行に納める //
	for(p=buf; *p!='\0'; p++){		// バッファの終端まで検索
		if(*p == '\n'){				// 改行発見

			if((++count) > 30){		// ３０行以上の場合、
				strcpy(p+1, msg);	// それを知らせるメッセージを埋め込んで
				break;				// ループから抜ける
			}

		}
	}

	// メッセージボックスを表示 //
	::MessageBox(NULL, buf, m_FileName, MB_OK|MB_ICONWARNING);
}
