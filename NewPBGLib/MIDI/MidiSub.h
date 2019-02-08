/*
 *   MidiSub.h   : MIDI 管理・サブ
 *
 */

#ifndef MIDISUB_INCLUDED
#define MIDISUB_INCLUDED "MIDI 管理・サブ : Version 0.02 : Update 2001/06/06"

/*  [更新履歴]
 *    Version 0.02 : 2001/06/06 : テンポ補正の追加
 *    Version 0.01 : 2001/05/30 : 製作開始
 */



#include "PbgType.h"



/***** [構造体] *****/

// ＳＭＦヘッダ //
typedef struct tagSMFHeader {
	DWORD		MThd;	// そのまま "MThd"
	DWORD		Size;	// サイズ
} SMFHeader;

// ＳＭＦ情報 //
typedef struct tagSMFMainInfo {
	WORD		Format;		// フォーマット
	WORD		Track;		// トラック数
	WORD		TimeBase;	// タイムベース
} SMFMainInfo;

// ＳＭＦトラックのヘッダ //
typedef struct tagSMFTrack {
	DWORD		MTrk;		// そのまま "MTrk"
	DWORD		Size;		// サイズ
} SMFTrack;



/***** [インライン関数] *****/

// ２バイトデータを変換する //
static IWORD ConvWord(WORD data)
{
	WORD temp;

	((BYTE *)&temp)[0] = ((BYTE *)&data)[1];
	((BYTE *)&temp)[1] = ((BYTE *)&data)[0];

	return temp;
}

// ４バイトデータを変換する //
static IDWORD ConvDWord(DWORD data)
{
	DWORD temp;

	((BYTE *)&temp)[0] = ((BYTE *)&data)[3];
	((BYTE *)&temp)[1] = ((BYTE *)&data)[2];
	((BYTE *)&temp)[2] = ((BYTE *)&data)[1];
	((BYTE *)&temp)[3] = ((BYTE *)&data)[0];

	return temp;
}

// ウェイトカウントを求め、ポインタ前進 //
static IDWORD GetWaitCount(LPBYTE *data)
{
	BYTE	temp;
	DWORD	ret = 0;

	do{
		temp = **data;
		++*data;
		ret = (ret<<7)+(temp&0x7f);
	} while(temp&0x80);

	return ret;
}


// ショートメッセージ出力 //
static IDWORD SendShortMsg(HMIDIOUT hm, BYTE m1, BYTE m2, BYTE m3)
{
	static BYTE data[4];

	data[0] = m1;
	data[1] = m2;
	data[2] = m3;

	midiOutShortMsg(hm, *((DWORD *)data));
}



#endif
