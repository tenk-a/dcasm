///	@file	test_02.s

#define OP_PUSH_IMM	0xF1
#define OP_POP		0xF2

#define OP_GOTO		0xF3
#define	OP_EQ_GOTO	0xF4
#define OP_CALL		0xF5
#define OP_RETURN	0xF6
#define OP_TEST_1	0xF7
#define OP_TEST_2	0xF8
#define OP_END		0xF9



#define CodeOp(op)	dc.l	op


#macro	Macro_Test_01(arg1,arg2, arg3)
	dc.l	0, arg1,arg2,arg3
#endmacro


#macro	Macro_Test_02(arg1,arg2,arg3)
	#if (arg1) & 1
		dc.l	OP_TEST_1, arg2, 0, 0
	#else
		dc.l	OP_TEST_2, arg3, 0, 0
	#endif
#endmacro


#macro	Macro_Test_03(arg1,arg2,arg3)
	#local	L1, L2
	dc.l	OP_PUSH_IMM, arg1
	dc.l	OP_EQ_GOTO, L1
	dc.l	OP_TEST_1, arg2
	dc.l	OP_GOTO, L2
L1:
	dc.l	OP_TEST_2, arg3
	dc.l	OP_TEST_2, 0
L2:
#endmacro

//#error	ERROR !
//#print	test, print

// -----------------------------------------------------------------
	// 自動生成されたローカルラベルを外部に出す
	xdef	_LCL_0, _LCL_1, _LCL_2, _LCL_3

	org	0
	dc.b	"SCE", 0
	dc.l	SCE_START
	dc.l	SCE_END - SCE_START
	dc.l	TEXT_END - TEXT_START

SCE_START:
	Macro_Test_01(1,2,3)
	Macro_Test_02(1, 0x1000, 0x2000)
	Macro_Test_02 0, 0x3000, 0x4000
	Macro_Test_03(3, 0x5000, 0x6000)
	Macro_Test_03 0, 0x7000, 0x8000
	CodeOp(OP_RETURN)
	CodeOp OP_END
	#rept 8
		dc.b	0xff
	#endrept
SCE_END:
TEXT_START:
	stri
		Hello world!
		This is a pen.
		?
	stri
        #ipr A=Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday
		@##A
        #endipr
TEXT_END:
	end
