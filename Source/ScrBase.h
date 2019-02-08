/*
 *   ScrBase.h   : 基本スクリプト命令
 *
 */

#ifndef SCRBASE_INCLUDED
#define SCRBASE_INCLUDED "基本スクリプト命令 : Version 0.01 : Update 2001/03/11"

/*  [更新履歴]
 *    Version 0.01 : 2001/03/11 : 調整中
 */



/***** [ 定数 ] *****/

// ＳＣＬ専用命令 //
#define SCR_NOP			0x30	// 停止命令[SCL/ECL/TCL/ExAnm]   Cmd(1) + t(2)
#define SCR_SET			0x31	// セット命令 Cmd(1) + (x(2), y(2), Addr(4))



// テクスチャ定義専用命令 //

// テクスチャのロード     : RegID(1) + Cmd(1) + Num(1) + string + \0  //
// この命令は続く ANIME 命令の対象となるテクスチャを指定する //
#define SCR_LOAD		0x40

// 矩形型の代入命令(絶対) : Cmd(1) + Index(1) + rect(2Byte * 4) //
#define SCR_RECT		0x41

// アニメーションの定義   : Cmd(1) + Index(1) + (n(1), ID[n]) //
// 上に示したように、LOAD 命令にてテクスチャは示される        //
#define SCR_ANIME		0x42


// 2001/10/11 : 廃止されました //
//// 条件付きテクスチャロード : RegID(1) + Cmd(1) + SubID(1) + string + \0 //
//// #define SCR_LOAD2		0x43


// 条件付きテクスチャロード : Cmd(1) + string + \0 //
#define SCR_LOADEX		0x44

#define SCR_STOP		0x45	// アニメーション停止モード Cmd(1)



// ＥＣＬ専用命令 //
#define SCR_CALL		0x50	// プロシージャを呼び出す   Cmd(1) + Addr(4)
#define SCR_ATK			0x51	// 攻撃スレッドを発生させる Cmd(1) + dx(2) + dy(2) + Addr(4)
#define SCR_ESET		0x52	// 敵セット                 Cmd(1) + Addr(4)
#define SCR_RET			0x53	// プロシージャからの復帰   Cmd(1) + pop(1)
#define SCR_ANM			0x54	// アニメーション設定  Cmd(1) + Ptn(1) + Spd(1)
#define SCR_FATK		0x55	// 死亡時に発動する攻撃コマンド Cmd(1) + Addr(4)
#define SCR_ATKNP		0x56	// 親無しオブジェクトセット Cmd(1) + Addr(4)

#define SCR_MOV			0x57	// 直線移動     Cmd(1) + v0(2) + t(2)
#define SCR_ACC			0x58	// 加速付き移動 Cmd(1) + v0(2) + a(2)  + t(2)
#define SCR_ROL			0x59	// 回転付き移動 Cmd(1) + v0(2) + vd(1) + t(2)
#define SCR_WAITATOBJ	0x5a	// 攻撃オブジェクトの終了を待つ Cmd(1)

#define SCR_PSE			0x5b	// [ECL/TCL] 効果音を再生する Cmd(1) + ID(1)
#define SCR_KILL		0x5c	// 敵を殺して爆発させる(fatk 有効) Cmd(1)

#define SCR_MDMG		0x5d	// マッドネスゲージに対するダメージ Cmd(1) + Damage(2)

#define SCR_CHILD		0x5e	// 子を発生させる   Cmd(1) + ID(1) + Addr(4)
#define SCR_CHGTASK		0x5f	// 子のタスクを変更 Cmd(1) + ID(1) + Addr(4)
#define SCR_PARENT		0x60	// 親のデータ参照   Cmd(1) + RegsID(1)

#define SCR_PMOV		0x61	// 親と同期・直線移動   SCR_MOV に同じ / break gr6, gr7
#define SCR_PACC		0x62	// 親と同期・加速移動   SCR_ACC に同じ / break gr6, gr7
#define SCR_PROL		0x63	// 親と同期・回転移動   SCR_ROL に同じ / break gr6, gr7
#define SCR_PNOP		0x64	// 親と同期・一時停止   SCR_NOP に同じ / braek gr6, gr7

#define SCR_ATK2		0x65	// 攻撃オブジェクトセット(Param)
								// Cmd(1) + dx(2) + dy(2) + Param(4) + Addr(4)

#define SCR_EFC			0x66	// エフェクト発生
								// Cmd(1) + dx(2) + dy(2) + type(1)


// ＴＣＬ専用命令 //
#define SCR_TAMA		0x70	// 弾発射モードで動作                  Cmd(1)
#define SCR_LASER		0x71	// レーザー発射モードで動作            Cmd(1)
#define SCR_DEGE		0x72	// 角度を敵の進行方向にセットする      Cmd(1)
#define SCR_DEGS		0x73	// 角度を自機方向にセットする[TCL/ECL] Cmd(1)
#define SCR_LLCHARGE	0x74	// レーザーを溜め状態にする    Cmd(1)
#define SCR_LLOPEN		0x75	// レーザーをオープンする      Cmd(1) + w(4)
#define SCR_LLCLOSE		0x76	// レーザーを閉じる            Cmd(1)
#define SCR_HLASER		0x77	// ホーミングレーザーを発射 Cmd(1)
#define SCR_LSPHERE		0x78	// ライトニング・すふぃぁ   Cmd(1)
#define SCR_RLASER		0x79	// ラウンドレーザーを発射   Cmd(1)
#define SCR_CROSS		0x7a	// 十字架を発動させる       Cmd(1)
#define SCR_FLOWER		0x7b	// 花を発生させる           Cmd(1)
#define SCR_GFIRE		0x7c	// Ｇ．ＦＩＲＥ             Cmd(1)
#define SCR_IONRING		0x7d	// イオンリング             Cmd(1)


// ＴＡＬＫ専用命令 //
#define SCR_TALKMSG		0x90	// [TALK] メッセージ挿入  Cmd(1) + msg(文字列長+1)
#define SCR_TALKKEY		0x91	// [TALK] キー入力待ち @  Cmd(1)
#define SCR_TALKNEWL	0x92	// [TALK] 改行要請     ;  Cmd(1)
#define SCR_TALKWAIT	0x93	// [TALK] 待ち命令     $  Cmd(1) + t(2) Skip可能



// ＥｘＡｎｍ専用命令 //
#define SCR_TASK		0xa0	// [ExAnm] タスクを発生させる Cmd(1) + Addr(4)
								// 注意：スクリプト上の表記は Set となる

#define SCR_TEXMODE		0xa1	// [ExAnm] モード変更を行う Cmd(1) + Mode(1)
								// 通常・加算半透明 / Single・Double テクスチャモード



// 標準命令(必ずサポートしている必要がある) //
#define SCR_PUSHR		0xc0	// [後] スタックにレジスタの内容を積む
#define SCR_POPR		0xc1	// [後] レジスタにスタックの内容をコピー
#define SCR_MOVC		0xc2	// [中] レジスタに定数の代入を行う(MOVC REG CONST)
//#define SCR_MOVR		0xc3	// [後] レジスタにレジスタの代入を行う

#define SCR_PUSHC		0xc4	// [前] 定数をスタックにＰＵＳＨする
#define SCR_TJMP		0xc5	// [前] POP して真ならばジャンプする
#define SCR_FJMP		0xc6	// [前] POP して偽ならばジャンプする
#define SCR_JMP			0xc7	// [前] 無条件ジャンプ
#define SCR_OJMP		0xca	// [前] POP して真ならPUSH(TRUE),  JMP
#define SCR_AJMP		0xcb	// [前] POP して偽ならPUSH(FALSE), JMP
#define SCR_EXIT		0xcc	// [前] 終了する
#define SCR_LPOP		0xcd	// [前] 演算スタックトップ->JMP スタックトップ
#define SCR_LJMP		0xce	// [前] ０ならジャンプ、真ならデクリメント

#define SCR_ADD			0xd0	// [前] Push(Pop(1) + Pop(0))
#define SCR_SUB			0xd1	// [前] Push(Pop(1) - Pop(0))
#define SCR_MUL			0xd2	// [前] Push(Pop(1) * Pop(0))
#define SCR_DIV			0xd3	// [前] Push(Pop(1) / Pop(0))
#define SCR_MOD			0xd4	// [前] Push(Pop(1) % Pop(0))
#define SCR_NEG			0xd5	// [前] Push(-Pop(0))

#define SCR_SINL		0xd6	// [前] Push(sinl(Pop(1), Pop(0))
#define SCR_COSL		0xd7	// [前] Push(cosl(Pop(1), Pop(0))
#define SCR_RND			0xd8	// [前] Push(rnd() % Pop(0))
#define SCR_ATAN		0xd9	// [前] Push(atan(Pop(1), Pop(0))

#define SCR_EQUAL		0xda	// [前] Push(Pop(1) == Pop(0))
#define SCR_NOTEQ		0xdb	// [前] Push(Pop(1) != Pop(0))
#define SCR_ABOVE		0xdc	// [前] Push(Pop(1) >  Pop(0))
#define SCR_LESS		0xdd	// [前] Push(Pop(1) <  Pop(0))
#define SCR_ABOVEEQ		0xde	// [前] Push(Pop(1) >= Pop(0))
#define SCR_LESSEQ		0xdf	// [前] Push(Pop(1) <= Pop(0))

#define SCR_MAX			0xe0	// [前] Push( max(Pop(0), Pop(1)) )
#define SCR_MIN			0xe1	// [前] Push( min(Pop(0), Pop(1)) )



#endif
