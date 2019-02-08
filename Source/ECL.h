/*
 *   ECL.h   : 敵操作言語
 *
 */

#ifndef ECL_INCLUDED
#define ECL_INCLUDED "敵操作言語   : Version 3.00 : Update 2001/03/11"

/*  [更新履歴]
 *    Version 3.00 : 2001/03/11 : 整備中...
 */



/***** [ 定数 ] *****/
#define ECL_REG_X		0x00	// 現在のＸ座標
#define ECL_REG_Y		0x01	// 現在のＹ座標
#define ECL_REG_V		0x02	// 速度
#define ECL_REG_HP		0x03	// ＨＰ残量
#define ECL_REG_COUNT	0x04	// カウンタ(できればReadOnlyとしたいが...)
#define ECL_REG_SCORE	0x05	// スコア
#define ECL_REG_GR0		0x06	// 汎用レジスタ０番
#define ECL_REG_GR1		0x07	// 汎用レジスタ１番
#define ECL_REG_GR2		0x08	// 汎用レジスタ２番
#define ECL_REG_GR3		0x09	// 汎用レジスタ３番
#define ECL_REG_GR4		0x0a	// 汎用レジスタ４番
#define ECL_REG_GR5		0x0b	// 汎用レジスタ５番
#define ECL_REG_GR6		0x0c	// 汎用レジスタ６番
#define ECL_REG_GR7		0x0d	// 汎用レジスタ７番
#define ECL_REG_FLAG	0x0e	// 属性フラグ
#define ECL_REG_D		0x0f	// 進行方向

// read only Regs /
#define ECL_REG_XMID		0x10	// Ｘ座標中心
#define ECL_REG_MX			0x11	// 自機Ｘ座標
#define ECL_REG_MY			0x12	// 自機Ｙ座標
#define ECL_REG_ALPHA		0x13	// α値
#define ECL_REG_NUMCHILD	0x14	// 生きている子の数(コスト高)



#define ECL_NUMREGS		21		// レジスタの本数



#endif
