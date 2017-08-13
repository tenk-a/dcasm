/**
 *	@file	test_01.s
 *	@brief	dcasm test 1
 */

#define	NN_00	0x00
#define NN_11	0x11
#define	NN_22	0x22
#define NN_33	0x33
#define NN_44	0x44
#define NN_55	0x55
#define NN_66	0x66
#define NN_77	0x77
#define NN_88	0x88
#define NN_99	0x99
#define NN_AA	0xAA
#define NN_BB	0xBB
#define NN_CC	0xcc
#define NN_DD	0xDd
#define NN_EE	0xeE
#define NN_FF	0xff

#define Square(x)	((x) * (x))
#define BB(a,b)		(((a)<<8)|(b))
#define BBBB(a,b,c,d)	(((a)<<24)|((b)<<16)|((c)<<8)|(d))
#define WW(a,b)		(((a)<<16)|(b))
#define WWWW(a,b,c,d)	(((a)<<48)|((b)<<32)|((c)<<16)|(d))
//#define DD(a,b)	(((a)<<32)|(b))
#define DD(a,b)		((a)*0x100000000 + (b))


#define ABCDEFGHIJKLMNOPQRSTUVWXYZ	(-1)
#define	abcdefghijklmnopqrstuvwxyz	(-67438087)


	xdef	START
	xdef	FOO_1, FOO_2, FOO_3, FOO_4, FOO_5
	xdef	BAR_B, BAR_W, BAR_D, BAR_Q

	org	0

START:
FOO_1:
	dc.b	0x00
	dc.b	0x01
	dc.w	0x0203
	dc.l	0x04050607
	dc.q	0x08090a0b0c0d0e0f

FOO_2:
	db	0
	db	1
	dw	515
	dd	67438087
	dq	579005069656919567

FOO_3:
	dc.b	-0x00					// 00
	dc.b	-0x01					// ff
	dc.w	-BB(0x02,0x03)				// fdfd
	dc.l	-BBBB(0x04,0x05,0x06,0x07)		// FBFAF9F9
	dc.q	-WWWW(0x0809,0x0a0b,0x0c0d,0x0e0f)	// F7F6F5F4F3F2F1F1

FOO_4:
	db	-0
	db	ABCDEFGHIJKLMNOPQRSTUVWXYZ
	dw	-515
	dd	abcdefghijklmnopqrstuvwxyz
	dq	-579005069656919567

FOO_5:
	//	0   1   2    3   4     5     6      7      8      9     10    11     12               13          14                                      15
	dc.b	0,  1, 1+1, 4-1, 2*2, 25/5, 16%10, 4|2|1,  1<<3, 15&9,  8^2,  22>>1, (1+4-2)*2*10/5, 13*(10==10), ((0!=1)+(3>1)+(2<5)+(7>=7)+(9<=9)+2)*2, (255&15)|5

BAR_B:
	db	NN_00, NN_11, NN_22, NN_33, NN_44, NN_55, NN_66, NN_77, NN_88, NN_99, NN_AA, NN_BB, NN_CC, NN_DD, NN_EE, NN_FF
BAR_W:
	dw	NN_00, NN_11*1, (NN_22*2)/2, NN_33+3-3, 255&(NN_44+0xff00), Square(NN_55)/0x55, NN_66, NN_77, NN_88, NN_99, NN_AA, NN_BB, NN_CC, NN_DD, NN_EE, NN_FF
BAR_D:
	dd	Square(Square(Square(1+1)))-256+NN_00, NN_11, NN_22, NN_33, NN_44, NN_55, NN_66, NN_77, NN_88, NN_99, NN_AA, NN_BB, NN_CC, NN_DD, NN_EE, NN_FF
BAR_Q:
	dq	NN_00, NN_11, NN_22, NN_33, NN_44, NN_55, NN_66, NN_77, NN_88, NN_99, NN_AA, NN_BB, NN_CC, NN_DD, NN_EE, NN_FF

BAZ_1:
	dc.b	"0123456789ABCDEF"
	dc.w	"01234567"
	dc.l	"0123"
	dc.q	"01"


BAZ_2:
	dc.q	DD(0xF0E0D0C0, 0xB0A09080), WWWW(0x7060,0x5040,0x3020,0x1000)
