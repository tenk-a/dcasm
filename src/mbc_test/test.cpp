/**
 *	@file	test.cpp
 *	@brief
 *	@author	tenk*
 *	@date	2010-??
 *	@note
 *		fname.h のテスト.
 */

#define USE_UNITST
#define UNUSE_UNITST_STRSTREAM
#include "unitst.h"
#include "../mbc.h"

/** メイン.
 */
int main()
{
	mbs_init();
	UNITST_RUN_ALL();
	// fullpath_smp();
	return 0;
}
