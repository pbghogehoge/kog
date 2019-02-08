/*
 *   TCL.h   : 弾操作言語
 *
 */

#ifndef TCL_INCLUDED
#define TCL_INCLUDED "弾操作言語   : Version 0.01 : Update 2001/03/11"

/*  [更新履歴]
 *    Version 0.01 : 2001/03/11 : 調整中
 */



/***** [ 定数 ] *****/

// レジスタＩＤ(代入命令としても使用する) //
#define TCL_REG_X		0x00	// 弾発射を行う中心Ｘ座標
#define TCL_REG_Y		0x01	// 弾発射を行う中心Ｙ座標
#define TCL_REG_CMD		0x02	// 弾の発射形状(扇、ランダム等)
#define TCL_REG_V		0x03	// 弾の速度
#define TCL_REG_A		0x04	// 弾の加速度
#define TCL_REG_DW		0x05	// 弾の角度差
#define TCL_REG_NDW		0x06	// 連弾時の弾のズレ
#define TCL_REG_N		0x07	// 弾数
#define TCL_REG_NS		0x08	// 連弾数
#define TCL_REG_D		0x09	// 基本角
#define TCL_REG_VD		0x0a	// 扇＆ランダム時に使用する角度差
#define TCL_REG_C		0x0b	// 弾の色＆形状
#define TCL_REG_REP		0x0c	// 弾の繰り返しパラメータ
#define TCL_REG_TYPE	0x0d	// 弾の種類(通常、ホーミング)
#define TCL_REG_OPT		0x0e	// 弾のオプション(反射Ｘ等)
#define TCL_REG_ANM		0x0f	// 弾のアニメーションスピード
#define TCL_REG_L		0x10	// レーザーの長さ

#define TCL_REG_PGR0	0x11	// 親の汎用レジスタ０番
#define TCL_REG_PGR1	0x12	// 親の汎用レジスタ１番
#define TCL_REG_PGR2	0x13	// 親の汎用レジスタ２番
#define TCL_REG_PGR3	0x14	// 親の汎用レジスタ３番
#define TCL_REG_PGR4	0x15	// 親の汎用レジスタ４番
#define TCL_REG_PGR5	0x16	// 親の汎用レジスタ５番
#define TCL_REG_PGR6	0x17	// 親の汎用レジスタ６番
#define TCL_REG_PGR7	0x18	// 親の汎用レジスタ７番
#define TCL_REG_PD		0x19	// 親の角度
#define TCL_REG_PARENT	0x1a	// 親へのポインタ(非ゼロなら親がいる)

#define TCL_REG_PARAM	0x1b	// ATK2 でのパラメータ

#define TCL_NUMREGS		28		// レジスタの本数



#endif
