/*
 *   TALK.h   : 会話命令用定数
 *
 */

#ifndef TALK_INCLUDED
#define TALK_INCLUDED "会話命令用定数 : Version 0.01 : Update 2001/05/28"

/*  [更新履歴]
 *    Version 0.01 : 2001/05/28 : 製作開始
 */



#include "PbgType.h"



/***** [ 定数 ] *****/

// 会話指定用定数 //
#define TALK_VIVIT		0x00	// びびっと指定用定数
#define TALK_STG1BOSS	0x01	// ミリア指定用定数
#define TALK_STG2BOSS	0x02	// めい＆まい指定用定数
#define TALK_STG3BOSS	0x03	// ゲイツ指定用定数
#define TALK_STG4BOSS	0x04	// マリー指定用定数
#define TALK_STG5BOSS	0x05	// エーリッヒ指定用定数
#define TALK_DEFAULT	0x06	// デフォルト指定用定数

#define TALK_NUMKIND	7		// 会話指定用定数の種類
#define TALK_PTNS		32		// 会話ランダム選出パターン数(最大)



/***** [構造体] *****/

// 会話の開始アドレス定義 //
#pragma pack(push, PACK_tagTalkEntryInfo)
#pragma pack(4)
typedef struct tagTalkEntryInfo {
	DWORD	m_Address[TALK_NUMKIND][TALK_PTNS];		// 開始アドレステーブル
	DWORD	m_NumPtns[TALK_NUMKIND];				// 会話パターン
} TalkEntryInfo;
#pragma pack(pop, PACK_tagTalkEntryInfo)



#endif
