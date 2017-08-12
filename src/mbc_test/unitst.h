/**
 *	@file	unitst.h
 *	@brief	c++�p�� �ȈՂ� �P�̃e�X�g ����.
 *	@date	2004-2010
 *	@author	tenk* (Masashi KITAMURA)
 *	@note
 *	- ���Ƃ��� D����� unittest �ɐG������č�����㕨.
 *	- int=32,long long=64 ���O��
 *	- �g����.<br>
 *		�R���p�C���̃I�v�V�������ŗ\�� USE_UNITST���`.
 *		�e�X�g������ class Foo ������󋵂�,
 *		<pre>
 *		class Foo {
 *			....
 *		};
 *		UNITTEST(Foo_Test) {
 *			Foo* foo = new Foo();
 *			UNITST_ASSERT_PTR(foo);
 *			....
 *		};
 *		</pre>
 *		�̂悤�ɂ��āA�e��e�X�g���s�����[�`���L�q.
 *		�v���O�����N����(main,WinMain�t��)�̕K�v�ŏ����̏������̌�ŁA<br>
 *		UNITST_RUN_ALL();						<br>
 *		���ĂԂƊe�e�X�g�����s�B
 *	- �o�^�ł���e�X�g���[�`���̐��͓K����512�ɐݒ�.
 *		�\�� UNITST_MAX �ɒl��ݒ肷��΂��̒l���ő吔�ɂ���.
 *	- �G���[�����Ɍo�߃��O�o�͂��Ȃ��������ꍇ�́AUSE_UNITST�̕ς���
 *		USE_UNITST_NOLOG  ���`���邱��.
 *	- ��O�֌W���g��Ȃ��ꍇ�͗\��
 *		UNUSE_UNITST_THROW ���`.
 *	  ������ ASSERT�nfailed�ł́A�p���ł������̏��exit(1)�I���ɂȂ�.
 *	- USE_UNITST_STRSTREAM ���`����� UNITST_EQ,NE,LT,LE,GT,GE,LIMIT
 *	  �� strstream ���g���A��{�^�ȊO�̕ϐ����e���\��.
 *	- (vc�ȊO��)�R���p�C���E�o�[�W�����`�F�b�N�͓K��.
 *	- Public Domain Software
 */

#ifndef UNITST_H
#define UNITST_H


#ifndef __cplusplus		// C�̎��͎g���Ȃ�. ���A�Ȃ�ׂ��G���[�ɂ�����������
#undef USE_UNITST
#undef USE_UNITST_NOLOG
#endif

#if defined USE_UNITST_NOLOG && !(defined USE_UNITST)
#define USE_UNITST		USE_UNITST_NOLOG
#endif


#define UNITTEST(T)		UNITST_TEST(T)			// �������݊�.


#ifndef USE_UNITST	// ########################################################
// �e�X�g���Ȃ��Ƃ�.
// ############################################################################

#define UNITST_RUN_ALL()		// main��WinMain���̏����[�ɌĂяo���̑z��.

#define UNITST_ADD_FUNC(F)

#ifdef __cplusplus
#define UNITST_TEST(T)		template<typename DMY> 	void uniTtesT_dmy_##T()
#define UNITST_FRIEND(T)	friend class T		// ����@�����A�O�̂��߂̂���.
#else
#define UNITST_TEST(T)		static inline void uniTtesT_dmy_##T()
#endif


#define UNITST_STATIC_ASSERT(cc)
#define UNITST_ABORT()
#define UNITST_PRINTF(x)
#define UNITST_IS_RAM_PTR(p)		(1)

#define UNITST_RETURN()
#define UNITST_ADD_FAILUSE()
#define UNITST_FAIL()

#define UNITST_ASSERT(b)
#define UNITST_ASSERT_PTR(a)
#define UNITST_ASSERT_PTR0(a)
#define UNITST_ASSERT_MSGF(b, msg)
#define UNITST_ASSERT_MSG(b, m)
#define UNITST_ASSERT_THROW(exp,E)
#define UNITST_ASSERT_ANY_THROW(e)
#define UNITST_ASSERT_NO_THROW(e)

#define UNITST_CHECK(b)
#define UNITST_CHECK_PTR(a)
#define UNITST_CHECK_PTR0(a)
#define UNITST_CHECK_MSGF(b, msg)
#define UNITST_CHECK_MSG(b, m)
#define UNITST_CHECK_THROW(exp, E)
#define UNITST_CHECK_ANY_THROW(exp)
#define UNITST_CHECK_NO_THROW(exp)

#define UNITST_LIMIT(a, mi, ma)
#define UNITST_EQ(a, b)
#define UNITST_NE(a, b)
#define UNITST_LT(a, b)
#define UNITST_LE(a, b)
#define UNITST_GT(a, b)
#define UNITST_GE(a, b)

#define UNITST_LIM_BOOL(a)
#define UNITST_LIM_PTR(a, mi, ma)

#define UNITST_EQ_DELTA_F(a,b,d)
#define UNITST_EQ_DELTA_D(a,b,d)

#define UNITST_EQ_STR(a,b)
#define UNITST_EQ_STRN(a,b,n)
#define UNITST_EQ_STRI(a,b)
#define UNITST_EQ_STRNI(a,b,n)

#define UNITST_EQ_WSTR(a,b)
#define UNITST_EQ_WSTRN(a,b,n)
#define UNITST_EQ_WSTRI(a,b)
#define UNITST_EQ_WSTRNI(a,b,n)

#define UNITST_EQ_STRING(a,b)
#define UNITST_EQ_CSTRING(a,b)

#define UNITST_EQ_MEM(a, b, l)
#define UNITST_NE_MEM(a, b, l)
#define UNITST_EQ_CLCTN(a,a2,b)


#else	// ####################################################################
// �e�X�g����Ƃ�
// ############################################################################

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifdef USE_UNITST_STRSTREAM
#include <strstream>
#endif


// ============================================================
// �o�^�ł���e�X�g(���[�`��)�̍ő吔�̐ݒ�.

#ifdef UNITST_MAX						// �\��UNITST_MAX���ݒ�ς݂Ȃ炻����A
#else									// ������ꍇ�͂����̒l(512)��
  #define UNITST_MAX	512				///< UNITTEST�̍ő吔.
#endif


// ============================================================
// �ȈՂȃf�o�b�O��p�T�u���[�`�� �� �R���p�C���̒��날�킹.

#define UNITST_STR_CAT(a,b)		UNITST_STR_CAT_2(a,b)
#define UNITST_STR_CAT_2(a,b)	a##b

#ifdef DBG_H	// ----------------------------------------
 // dbg.h ���g���Ă���ꍇ�͂�����ɏ������
 #define UNITST_STATIC_ASSERT(cc)	DBG_STATIC_ASSERT(cc)
 #define UNITST_ABORT()				ERR_ABORT()
 #define UNITST_PRINTF(x)			ERR_PRINTF(x)
 #define UNITST_IS_RAM_PTR(p)		ERR_IS_RAM_PTR(p)

#else			// ----------------------------------------
 #include <stdio.h>
 #include <stdarg.h>

 #if defined _MSC_VER
  #define UNITST_STATIC_ASSERT(cc)	struct UNITST_STR_CAT(STATIC_ASSERT_CHECK_ST,__LINE__) { char dmy[2*((cc)!=0) - 1];};	\
									enum { UNITST_STR_CAT(STATIC_ASSERT_CHECK,__LINE__) = sizeof( UNITST_STR_CAT(STATIC_ASSERT_CHECK_ST,__LINE__) ) }
 #else
  #define UNITST_STATIC_ASSERT(cc)	typedef char UNITST_STR_CAT(STATIC_ASSERT_CHECK__LINE_,__LINE__)[(cc)? 1/*OK*/ : -1/*ERROR*/]
 #endif

 #if defined UNITST_ABORT	// ��`�ς݂Ȃ�A�Ē�`���Ȃ�.
 #elif defined _WINDOWS		// GUI�A�v���̎�
  #define UNITST_ABORT()		_CrtDbgBreak()
 #else						// �R�}���h���C���E�A�v���̂Ƃ�.
  #define UNITST_ABORT()		exit(1) 		// ((*(char*)0) = 0)
 #endif

 #if defined UNITST_PRINTF	// ��`�ς݂Ȃ�Ē�`���Ȃ�.
 #elif defined _WINDOWS		// GUI�A�v���̎�
  #define UNITST_PRINTF(x) 		unitst_printf x
  inline void unitst_printf(const char* f, ...) {
	char b[0x4000];va_list a;va_start(a,f);_vsnprintf(b,sizeof b,f,a); va_end(a); OutputDebugString(b);
  }
 #else						// �R�}���h���C���E�A�v���̂Ƃ�.
  #define UNITST_PRINTF(x) 		unitst_printf x
  inline void unitst_printf(const char* f, ...) {
	va_list a; va_start(a,f); vfprintf(stderr,f,a); va_end(a);
  }
 #endif

 #if defined UNITST_IS_RAM_PTR	// ��`�ς݂Ȃ�Ē�`���Ȃ�.
 #elif (defined _WIN64) || (defined __WORDSIZE && __WORDSIZE == 64) || (defined _M_AMD64)
  #define UNITST_IS_RAM_PTR(p) 	((size_t)(ptrdiff_t)(p) >= 0x10000 && (size_t)(ptrdiff_t)(p) <= 0xFFFFFFFF00000000LL)
 #elif defined _WIN32
  #define UNITST_IS_RAM_PTR(p) 	((size_t)(ptrdiff_t)(p) >= 0x10000 && (size_t)(ptrdiff_t)(p) <= 0xFFFF0000)
 #else
	inline bool UNITST_is_ram_ptr(const char* p) {
		return (sizeof(void*) >= 4) ? ((p >= (char*)(ptrdiff_t)0x1000 && p <= (char*)((ptrdiff_t)-0x1000)))
			  : 					  (p != 0) ;
	}
  #define UNITST_IS_RAM_PTR(p) 	UNITST_is_ram_ptr((const char*)p)
 #endif
#endif			// ----------------------------------------



// ============================================================================
// ���̃w�b�_���݂̂Ŏg���֐���}�N��

#ifdef _WIN32
//#include <windows.h>
#define UNITST_LONGLONG		__int64
#define UNITST_ULONGLONG	unsigned __int64
#define UNITST_snprintf		_snprintf
#else
#define UNITST_LONGLONG		long long
#define UNITST_ULONGLONG	unsigned long long
#define UNITST_snprintf		snprintf
#endif

#define UNITST_toSsz		64

// ���l�����񉻗p�̏���
inline char* UNITST_toS(char buf[], const char* s)	  { UNITST_snprintf(buf, UNITST_toSsz, "%s" , s); return buf; }
inline char* UNITST_toS(char buf[], const wchar_t* s) { UNITST_snprintf(buf, UNITST_toSsz, "%ls", s); return buf; }
#if 0
inline char* UNITST_toS(char buf[], char c) 		  { UNITST_snprintf(buf, UNITST_toSsz, "%c" , c); return buf; }
inline char* UNITST_toS(char buf[], wchar_t c)		  { UNITST_snprintf(buf, UNITST_toSsz, "%lc", c); return buf; }
inline char* UNITST_toS(char buf[], signed char v)	  { UNITST_snprintf(buf, UNITST_toSsz, "%d(%#x)", v,v); return buf; }
inline char* UNITST_toS(char buf[], unsigned char v)  { UNITST_snprintf(buf, UNITST_toSsz, "%d(%#x)", v,v); return buf; }
inline char* UNITST_toS(char buf[], short v)		  { UNITST_snprintf(buf, UNITST_toSsz, "%d(%#x)", v,v); return buf; }
inline char* UNITST_toS(char buf[], unsigned short v) { UNITST_snprintf(buf, UNITST_toSsz, "%u(%#x)", v,v); return buf; }
#endif
inline char* UNITST_toS(char buf[], int v)			  { UNITST_snprintf(buf, UNITST_toSsz, "%d(%#x)", v,v); return buf; }
inline char* UNITST_toS(char buf[], unsigned v) 	  { UNITST_snprintf(buf, UNITST_toSsz, "%u(%#x)", v,v); return buf; }
inline char* UNITST_toS(char buf[], long v) 		  { UNITST_snprintf(buf, UNITST_toSsz, "%ld(%#lx)",v,v); return buf; }
inline char* UNITST_toS(char buf[], unsigned long v)  { UNITST_snprintf(buf, UNITST_toSsz, "%lu(%#lx)",v,v); return buf; }
inline char* UNITST_toS(char buf[], float v)		  { UNITST_snprintf(buf, UNITST_toSsz, "%g" , double(v)); return buf;}
inline char* UNITST_toS(char buf[], double v)		  { UNITST_snprintf(buf, UNITST_toSsz, "%g" , v); return buf; }
inline char* UNITST_toS(char buf[], long double v)	  { UNITST_snprintf(buf, UNITST_toSsz, "%Lg", v); return buf; }

inline char* UNITST_toS(char buf[], UNITST_LONGLONG v) {
	if (v >= -(UNITST_LONGLONG)(2147483648) && v <= (UNITST_LONGLONG)(2147483647))
		UNITST_snprintf(buf, UNITST_toSsz, "%d(%#x)", int(v), unsigned(v) );
	else
		UNITST_snprintf(buf, UNITST_toSsz, "0X%x%08x", unsigned(v >> 32), unsigned(v) );
	return buf;
}

inline char* UNITST_toS(char buf[], UNITST_ULONGLONG v) {
	if (v <= (UNITST_ULONGLONG)0xffffffff)
		UNITST_snprintf(buf, UNITST_toSsz, "%u(%#x)", unsigned(v), unsigned(v) );
	else
		UNITST_snprintf(buf, UNITST_toSsz, "0x%x%08x", unsigned(v >> 32), unsigned(v) );
	return buf;
}

#ifdef USE_UNITST_STRSTREAM
template<class T>
inline char* UNITST_toS(char buf[], const T& t) {
	std::strstream s(buf,UNITST_toSsz);
	s << t << std::ends;
	return buf;
}
#else
template<class T>
inline char* UNITST_toS(char buf[], const T&) {
	buf[0] = 0;
	return buf;
}
#endif

/// �͈̓`�F�b�N�ł̃G���[�\������.
inline int UNITST_lim_pri(
		const char* fname, unsigned    line,
		const char* a	 , const char* a2,
		const char* mi	 , const char* mi2,
		const char* ma	 , const char* ma2)
{
	char buf[ UNITST_toSsz * 3 ];
	buf[0] = 0;
	if (mi2[0] || ma2[0])
		UNITST_snprintf(buf, sizeof buf, "(%s .. %s)", mi2, ma2);
	UNITST_PRINTF(("%s (%d): %s%s%s, Out of range[%s .. %s] %s\n"
					, fname, line
					, a, a2[0] ? "=" : "", a2
					, mi, ma
					, buf
				));
	return 1;
}

/// �͈̓`�F�b�N�ł̃G���[�\���}�N��.
#define UNITST_limit(a, mi, ma) do {										\
		if (UNITST_NOT((mi) <= (a) && (a) <= (ma))) {						\
			char a2[UNITST_toSsz], mi2[UNITST_toSsz], ma2[UNITST_toSsz];	\
			UNITST_lim_pri(__FILE__, __LINE__								\
							, #a,  UNITST_toS(a2 ,a)						\
							, #mi, UNITST_toS(mi2,mi)						\
							, #ma, UNITST_toS(ma2,ma));						\
		}																	\
	} while(0)


/// ��r�ł̃G���[�\������.
inline int UNITST_cmp_pri(
		const char* fname, unsigned    line,
		const char* a	 , const char* a2,
		const char* cc	 ,
		const char* b	 , const char* b2)
{
	UNITST_PRINTF(("%s (%d): [%s%s%s] %s [%s%s%s]  failed.\n"
					, fname, line
					, a, a2[0] ? ":" : "", a2
					, cc
					, b, b2[0] ? ":" : "", b2
				));
	return 1;
}

/// ��r�ł̃G���[�\���}�N��.
#define UNITST_cmp(a, cc, b) do {											\
		if (UNITST_NOT((a) cc (b))) {										\
			char as[UNITST_toSsz], bs[UNITST_toSsz];						\
			UNITST_cmp_pri(__FILE__, __LINE__								\
				, #a, UNITST_toS(as,a), #cc, #b, UNITST_toS(bs,b));			\
		}																	\
	} while(0)


// UNITST_EQ_STRING(a,b) �� std::string��"������"�̔�r���ʕ\���p.
template<class S> inline
typename S::const_pointer UNITST_string2c_str(const S&	  s) { return s.c_str();}
inline const char*		  UNITST_string2c_str(const char* s) { return s; }
inline const wchar_t*	  UNITST_string2c_str(const wchar_t* s) { return s; }


// ===========================================================================
// �e�X�g�o�^�}�N��.

/// �o�^���Ă���e�X�g���[�`���̎��s.
#define UNITST_RUN_ALL()	UNItTeST_<>::runAll()


/// �P�̃e�X�g���[�`�����L�q���邽�߂̃}�N��. (D����𑽏��ӎ�)
#define UNITST_TEST(T)							\
	class T {									\
		static void unittest();					\
	public:										\
		T() {									\
			UNItTeST_<>::add(&unittest, #T);	\
		}										\
	};											\
	static T uniTtEst_##T;						\
	void T::unittest()


/// �e�X�g���[�`���̊֐����w�肵�ēo�^����}�N��.
#define UNITST_ADD_FUNC(F)						\
	struct CUniTst_##F {						\
		CUniTst_##F() {							\
			UNItTeST_<>::add(F, #F);			\
		}										\
	};											\
	static CUniTst_##F uniTtEst_##F


/// �e�X�g�ΏۃN���X�̒��g��M�肽���ꍇ�ɁA�ΏۃN���X���ɋL�q.
/// ����Ȃ��Ƃ���ƒP�̃e�X�g�łȂ��Ȃ����Ⴄ�����.
#define UNITST_FRIEND(T)	friend class T



// ============================================================
/// �e�X�g��fail�œr���������鎞�̔��ʗp.
struct UNITST_Fail_Exception { };


// ============================================================
/// ���j�b�g�e�X�g�̊Ǘ��N���X
template<int NN = UNITST_MAX>
class UNItTeST_ {
public:
	static void add(void (*fnc)(), const char* name);
	static void runAll();

	static bool cc(bool b) { ++cur_->total_; cur_->errs_ += !b; return b; }

private:
	//static void (*tests_[NN])();		// �֐��ւ̃|�C���^����vc��������...
	struct Test1 {
		void			(*func_)();		///< �e�X�g�֐�.
		const char*		name_;			///< �e�X�g��.
		unsigned		total_;			///< �`�F�b�N���ڂ̐�.
		unsigned		errs_;			///< ���s�����`�F�b�N�̐�.
	};
	static Test1		tests_[NN];		///< 1�̃e�X�g�̏��.
	static unsigned 	testSize_;		///< �e�X�g�̐�.
	static Test1*		cur_;			///< ���ݏ������̃e�X�g.
};

template<int NN> typename UNItTeST_<NN>::Test1	UNItTeST_<NN>::tests_[NN];
template<int NN> unsigned 						UNItTeST_<NN>::testSize_ = 0;
template<int NN> typename UNItTeST_<NN>::Test1*	UNItTeST_<NN>::cur_ = &UNItTeST_<NN>::tests_[NN-1];


/** �e�X�g���[�`���̓o�^.
 */
template<int NN> void UNItTeST_<NN>::add(void (*fnc)(), const char* name) {
	if (testSize_ < NN) {
	  #if 1
		for (unsigned i = 0; i < testSize_; ++i) {
			Test1& f1 = tests_[i];
			if (f1.func_ == fnc || strcmp(f1.name_, name) == 0) {
				UNITST_PRINTF(("[UNITST] %s is already registered.\n", name));
				return;
			}
		}
	  #endif
		Test1&	t = tests_[testSize_++];
		t.func_   = fnc;
		t.name_	  = name;
		t.total_  = 0;
		t.errs_   = 0;
	} else {
		UNITST_PRINTF(("too many UNITST.(%d)\n", NN));
		UNITST_ABORT();
	}
}



/** �o�^���Ă���e�X�g���[�`���̎��s.
 */
template<int NN> void UNItTeST_<NN>::runAll() {
	unsigned failed = 0;
  #ifndef USE_UNITST_NOLOG
	UNITST_PRINTF(("### UNITST: %4d tests   (%s %s) ###\n", testSize_, __DATE__, __TIME__));
  #endif
	for (unsigned i = 0; i < testSize_; i++) {
		cur_	= &tests_[i];
		const char* name = cur_->name_;
	  #ifndef USE_UNITST_NOLOG
		UNITST_PRINTF(("[test]    %s\n", name));
	  #endif
	  #ifdef UNUSE_UNITST_THROW
		cur_->func_();
	  #else
		try {
			cur_->func_();
		} catch (UNITST_Fail_Exception) {
			++cur_->errs_, ++cur_->total_;
		} catch (...) {
			++cur_->errs_, ++cur_->total_;
			UNITST_PRINTF(("\tERROR: %s threw something.\n", name));
		}
	  #endif
		if (cur_->errs_) {
			++failed;
			UNITST_PRINTF(("<FAILED>  %s (error:%d/%d)\n", name, cur_->errs_, cur_->total_));
		}
	  #ifndef USE_UNITST_NOLOG
		else {
			UNITST_PRINTF((" (ok)     %s\n", name));
		}
	  #endif
	}

	if (failed) {
		UNITST_PRINTF(("=== %d tests, %d failed\n\n", testSize_, failed));
		UNITST_ABORT();
	}
  #ifndef USE_UNITST_NOLOG
	else {
		UNITST_PRINTF(("=== %d tests  ok\n\n", testSize_));
	}
  #endif
}


#define UNITST_CHECK_COUNT(b)		(UNItTeST_<>::cc(b))		///< �������������Ă�����true��Ԃ�.
#define UNITST_NOT(b)				(!UNItTeST_<>::cc(b))		///< �������������ĂȂ�������true��Ԃ�.




// ============================================================================
// ============================================================================
// UNITTEST(T) ���ɋL�q����A�`�F�b�N�p�̃}�N��.
// ============================================================================

#ifndef UNUSE_UNITST_THROW		// ��O����������̂Ƃ�.
#define UNITST_RETURN()				(throw UNITST_Fail_Exception())		///< ���̃e�X�g�𒆒f����.(���̃e�X�g��)
#else							// ��O�����������̂Ƃ�.
#define UNITST_RETURN()				UNITST_ABORT()						///< �e�X�g�𒆒f����
#endif

#define UNITST_ADD_FAILUSE()		UNITST_CHECK_COUNT(0)
#define UNITST_FAIL()				do {UNITST_PRINTF(("%s (%d): fail.\n",__FILE__,__LINE__));UNITST_RETURN();}while(0)

// UNITST_ASSERT�n: �����`�F�b�N���A�U�Ȃ烁�b�Z�[�W�o�͂��āA���̃e�X�g�𒆒f.
#define UNITST_ASSERT(b)			do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed.\n", __FILE__, __LINE__, #b)); UNITST_RETURN();} } while (0)
#define UNITST_ASSERT_PTR(a)		do {if (UNITST_NOT(UNITST_IS_RAM_PTR(a))) { UNITST_PRINTF(("%s (%d): %s(%p) is bad pointer.\n", __FILE__, __LINE__, #a, (a))); UNITST_RETURN();} } while (0)
#define UNITST_ASSERT_PTR0(a)		do {if (UNITST_NOT(UNITST_IS_RAM_PTR(a) || !(a))) { UNITST_PRINTF(("%s (%d): %s(%p) is bad pointer.\n", __FILE__,__LINE__,#a,(a))); UNITST_RETURN();} } while (0)
#define UNITST_ASSERT_MSGF(b, msg)	do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed. ", __FILE__, __LINE__, #b)); UNITST_PRINTF(msg); UNITST_RETURN();} } while (0)
#if __STDC_VERSION__ >= 199901L || (_MSC_VER >= 1400) || (__GNUC__ >= 3) || (__BORLANDC__ >= 0x580) || (__WATCOMC__ >= 1260)	//|| (__SC__ >= 850)
#define UNITST_ASSERT_MSG(b, ...)	do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed. ", __FILE__, __LINE__, #b)); UNITST_PRINTF((__VA_ARGS__)); UNITST_RETURN(); } } while (0)
#else
#define UNITST_ASSERT_MSG(b, m)		do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed. ", __FILE__, __LINE__, #b)); UNITST_PRINTF(("%s", m)); UNITST_RETURN(); } } while (0)
#endif
#ifndef UNUSE_UNITST_THROW		// ��O����������̂Ƃ�.
#define UNITST_ASSERT_THROW(exp,E)	do { bool f=0;f; try { exp; } catch (E) { f=1; } catch (...) {} if (UNITST_NOT(f)) { UNITST_PRINTF(("%s (%d): (%s) threw not %s.\n", __FILE__, __LINE__, #exp, #E)); UNITST_RETURN();} } while (0)
#define UNITST_ASSERT_ANY_THROW(e)	do { bool f=0;f; try { e; } catch (...) { f=1; } if (UNITST_NOT(f)) { UNITST_PRINTF(("%s (%d): (%s) threw not...\n", __FILE__, __LINE__, #e)); UNITST_RETURN();} } while (0)
#define UNITST_ASSERT_NO_THROW(e)	do { try { e; } catch (...) { UNITST_PRINTF(("%s (%d): (%s) threw something.\n", __FILE__, __LINE__, #e)); UNITST_RETURN();} } while (0)
#else							// ��O�����������̂Ƃ�.
#define UNITST_ASSERT_THROW(exp, E)
#define UNITST_ASSERT_ANY_THROW(e)
#define UNITST_ASSERT_NO_THROW(e)
#endif

// UNITST_CHECK�n: �����`�F�b�N���A�U�Ȃ烁�b�Z�[�W�o��. �e�X�g���̂͌p��.
#define UNITST_CHECK(b)				do {if (UNITST_NOT(b)) UNITST_PRINTF(("%s (%d): [%s] failed.\n", __FILE__, __LINE__, #b)); } while (0)
#define UNITST_CHECK_PTR(a)			do {if (UNITST_NOT(UNITST_IS_RAM_PTR(a))) UNITST_PRINTF(("%s (%d): %s(%p) is bad pointer.\n", __FILE__, __LINE__, #a, (a))); } while (0)
#define UNITST_CHECK_PTR0(a)		do {if (UNITST_NOT(UNITST_IS_RAM_PTR(a) || (a) == 0)) UNITST_PRINTF(("%s (%d): %s(%p) is bad pointer.\n", __FILE__, __LINE__, #a, (a))); } while (0)
#define UNITST_CHECK_MSGF(b, msg)	do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed. ", __FILE__, __LINE__, #b)); UNITST_PRINTF(msg); } } while (0)
#if __STDC_VERSION__ >= 199901L || (_MSC_VER >= 1400) || (__GNUC__ >= 3) || (__BORLANDC__ >= 0x580) || (__WATCOMC__ >= 1260)	//|| (__SC__ >= 850)
#define UNITST_CHECK_MSG(b, ...)	do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed. ", __FILE__, __LINE__, #b)); UNITST_PRINTF((__VA_ARGS__)); } } while (0)
#else
#define UNITST_CHECK_MSG(b, m)		do {if (UNITST_NOT(b)) { UNITST_PRINTF(("%s (%d): [%s] failed. ", __FILE__, __LINE__, #b)); UNITST_PRINTF(("%s", m)); } } while (0)
#endif

#ifndef UNUSE_UNITST_THROW		// ��O����������̂Ƃ�.
#define UNITST_CHECK_THROW(exp, E)	do { bool f=0;f; try { exp; } catch (E) { f=1; } catch (...) {} if (UNITST_NOT(f)) UNITST_PRINTF(("%s (%d): (%s) threw not %s.\n", __FILE__, __LINE__, #exp, #E)); } while (0)
#define UNITST_CHECK_ANY_THROW(exp)	do { bool f=0;f; try { exp; } catch (...) { f=1; } if (UNITST_NOT(f)) UNITST_PRINTF(("%s (%d): (%s) threw not...\n", __FILE__, __LINE__, #exp)); } while (0)
#define UNITST_CHECK_NO_THROW(exp)	do { try { exp; } catch (...) { UNITST_PRINTF(("%s (%d): (%s) threw something.\n", __FILE__, __LINE__, #exp)); } } while (0)
#else							// ��O�����������̂Ƃ�.
#define UNITST_CHECK_THROW(exp, E)
#define UNITST_CHECK_ANY_THROW(exp)
#define UNITST_CHECK_NO_THROW(exp)
#endif


// �ϐ��͈̔̓`�F�b�N. �͈͊O�Ȃ烁�b�Z�[�W�\��. (�����͌p��)
// a,b �̊֌W���`�F�b�N. �������Ă��Ȃ��ꍇ���b�Z�[�W�\��. (�����͌p��)

#define UNITST_LIMIT(a, mi, ma) 	UNITST_limit(a, mi, ma)
#define UNITST_EQ(a, b)				UNITST_cmp(a, == , b)
#define UNITST_NE(a, b)				UNITST_cmp(a, != , b)
#define UNITST_LT(a, b)				UNITST_cmp(a, <  , b)
#define UNITST_LE(a, b)				UNITST_cmp(a, <= , b)
#define UNITST_GT(a, b)				UNITST_cmp(a, >  , b)
#define UNITST_GE(a, b)				UNITST_cmp(a, >= , b)

#define UNITST_LIM_BOOL(a) 			UNITST_limit(a, 0, 1)
#define UNITST_LIM_PTR(a, mi, ma)	do {if (UNITST_NOT((char*)(mi) <=(char*)(a) && (char*)(a) <=(char*)(ma))) UNITST_PRINTF(("%s (%d): %s=%#p, Out of range[%p..%p]\n",__FILE__, __LINE__, #a, (a), (mi), (ma) ));} while(0)

#define UNITST_EQ_MEM(a, b, l)		do {const void* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_|| (A_&&B_&& memcmp(  A_,B_,(l)) == 0))) UNITST_PRINTF(("%s (%d): [%s] == [%s](%dbytes) failed.\n", __FILE__, __LINE__, #a, #b, (l))); } while (0)
#define UNITST_NE_MEM(a, b, l)		do {const void* A_=(a),*B_=(b); if (UNITST_NOT(A_!=B_&& (!A_||!B_|| memcmp(  A_,B_,(l))))) UNITST_PRINTF(("%s (%d): [%s] != [%s](%dbytes) failed.\n", __FILE__, __LINE__, #a, #b, (l))); } while (0)

#define UNITST_EQ_CLCTN(a,a2,b)		do {if (UNITST_NOT(std::equal(a,a2,b) )) UNITST_PRINTF(("%s (%d): {%s,%s} == {%s,...} failed.\n", __FILE__, __LINE__, #a, #a2, #b)); } while (0)

#define UNITST_EQ_DELTA_F(a,b,d)	do {const float  *aa=&(a);aa; if (UNITST_NOT((a) >= (b)-(d) && (a) <= (b)+(d))) UNITST_PRINTF(("%s (%d): [%s:%g] == [%s:%g] failed.\n", __FILE__, __LINE__, #a, (double)(a), #b, (double)(b) )); } while (0)
#define UNITST_EQ_DELTA_D(a,b,d)	do {const double *aa=&(a);aa; if (UNITST_NOT((a) >= (b)-(d) && (a) <= (b)+(d))) UNITST_PRINTF(("%s (%d): [%s:%g] == [%s:%g] failed.\n", __FILE__, __LINE__, #a, (double)(a), #b, (double)(b) )); } while (0)

#define UNITST_EQ_STR(a,b)			do {const char*    A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !strcmp 	(A_,B_	  )))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] failed.\n"		 ,__FILE__,__LINE__,#a,A_,#b,B_	   )); } while (0)
#define UNITST_EQ_STRN(a,b,n)		do {const char*    A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !strncmp	(A_,B_,(n))))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] len=%d failed.\n"  ,__FILE__,__LINE__,#a,A_,#b,B_,(n))); } while (0)
#define UNITST_EQ_WSTR(a,b) 	 	do {const wchar_t* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !wcscmp 	(A_,B_	  )))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] failed.\n" 	 ,__FILE__,__LINE__,#a,A_,#b,B_	   )); } while (0)
#define UNITST_EQ_WSTRN(a,b,n)		do {const wchar_t* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !wcsncmp	(A_,B_,(n))))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] len=%d failed.\n",__FILE__,__LINE__,#a,A_,#b,B_,(n))); } while (0)
#if defined _WIN32
#define UNITST_EQ_STRI(a,b)  		do {const char*    A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !stricmp	(A_,B_	  )))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] failed.\n"		 ,__FILE__,__LINE__,#a,A_,#b,B_	   )); } while (0)
#define UNITST_EQ_STRNI(a,b,n)		do {const char*    A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !strnicmp	(A_,B_,(n))))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] len=%d failed.\n"  ,__FILE__,__LINE__,#a,A_,#b,B_,(n))); } while (0)
#define UNITST_EQ_WSTRI(a,b)  		do {const wchar_t* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !_wcsicmp	(A_,B_	  )))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] failed.\n"		 ,__FILE__,__LINE__,#a,A_,#b,B_	   )); } while (0)
#define UNITST_EQ_WSTRNI(a,b,n)		do {const wchar_t* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !_wcsnicmp	(A_,B_,(n))))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] len=%d failed.\n",__FILE__,__LINE__,#a,A_,#b,B_,(n))); } while (0)
#else
#define UNITST_EQ_STRI(a,b)  		do {const char*    A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !strcasecmp (A_,B_	  )))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] failed.\n"		 ,__FILE__,__LINE__,#a,A_,#b,B_	   )); } while (0)
#define UNITST_EQ_STRNI(a,b,n)		do {const char*    A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !strncasecmp(A_,B_,(n))))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] len=%d failed.\n"  ,__FILE__,__LINE__,#a,A_,#b,B_,(n))); } while (0)
#define UNITST_EQ_WSTRI(a,b)  		do {const wchar_t* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !wcscasecmp (A_,B_	  )))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] failed.\n"		 ,__FILE__,__LINE__,#a,A_,#b,B_	   )); } while (0)
#define UNITST_EQ_WSTRNI(a,b,n)		do {const wchar_t* A_=(a),*B_=(b); if (UNITST_NOT(A_==B_||(A_&&B_&& !wcsncasecmp(A_,B_,(n))))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] len=%d failed.\n",__FILE__,__LINE__,#a,A_,#b,B_,(n))); } while (0)
#endif

#define UNITST_EQ_STRING(a,b)		do { if (UNITST_NOT(a == b)) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] failed.\n",__FILE__,__LINE__,#a,UNITST_string2c_str(a),#b,UNITST_string2c_str(b))); } while (0)
#define UNITST_EQ_WSTRING(a,b)		do { if (UNITST_NOT(a == b)) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] failed.\n",__FILE__,__LINE__,#a,UNITST_string2c_str(a),#b,UNITST_string2c_str(b))); } while (0)

#ifdef UNICODE
#define UNITST_EQ_CSTRING(a,b)		do { if (UNITST_NOT(CString(a) == CString(b))) UNITST_PRINTF(("%s (%d): [%s:%ls] == [%s:%ls] failed.\n",__FILE__,__LINE__,#a,LPCTSTR(a),#b,LPCTSTR(b))); } while (0)
#else
#define UNITST_EQ_CSTRING(a,b)		do { if (UNITST_NOT(CString(a) == CString(b))) UNITST_PRINTF(("%s (%d): [%s:%s] == [%s:%s] failed.\n",__FILE__,__LINE__,#a,LPCTSTR(a),#b,LPCTSTR(b))); } while (0)
#endif


#endif	// ####################################################################
// ############################################################################

// ============================================================================

// TCHAR �Ή�.
#ifdef UNICODE
#define UNITST_EQ_TSTR(a,b)			UNITST_EQ_WSTR(a,b)
#define UNITST_EQ_TSTRI(a,b)		UNITST_EQ_WSTRI(a,b)
#define UNITST_EQ_TSTRN(a,b,n)		UNITST_EQ_WSTRN(a,b,n)
#define UNITST_EQ_TSTRNI(a,b,n)		UNITST_EQ_WSTRNI(a,b,n)
#else
#define UNITST_EQ_TSTR(a,b)			UNITST_EQ_STR(a,b)
#define UNITST_EQ_TSTRI(a,b)		UNITST_EQ_STRI(a,b)
#define UNITST_EQ_TSTRN(a,b,n)		UNITST_EQ_STRN(a,b,n)
#define UNITST_EQ_TSTRNI(a,b,n)		UNITST_EQ_STRNI(a,b,n)
#endif


// ============================================================================
// ���o�[�W�����E�݊�.

#define UNITST_LIM_I(a, mi, ma) 	UNITST_LIMIT(a, mi, ma)
#define UNITST_LIM_LL(a, mi, ma) 	UNITST_LIMIT(a, mi, ma)
#define UNITST_LIM_F(a, mi, ma) 	UNITST_LIMIT(a, mi, ma)
#define UNITST_LIM_D(a, mi, ma) 	UNITST_LIMIT(a, mi, ma)
#define UNITST_LIM_LD(a, mi, ma) 	UNITST_LIMIT(a, mi, ma)

#define UNITST_EQ_I(a, b)			UNITST_EQ(a, b)
#define UNITST_NE_I(a, b)			UNITST_NE(a, b)
#define UNITST_LT_I(a, b)			UNITST_LT(a, b)
#define UNITST_LE_I(a, b)			UNITST_LE(a, b)
#define UNITST_GT_I(a, b)			UNITST_GT(a, b)
#define UNITST_GE_I(a, b)			UNITST_GE(a, b)

#define UNITST_EQ_LL(a, b)			UNITST_EQ(a, b)
#define UNITST_NE_LL(a, b)			UNITST_NE(a, b)
#define UNITST_LT_LL(a, b)			UNITST_LT(a, b)
#define UNITST_LE_LL(a, b)			UNITST_LE(a, b)
#define UNITST_GT_LL(a, b)			UNITST_GT(a, b)
#define UNITST_GE_LL(a, b)			UNITST_GE(a, b)

#define UNITST_EQ_F(a, b)			UNITST_EQ(a, b)
#define UNITST_NE_F(a, b)			UNITST_NE(a, b)
#define UNITST_LT_F(a, b)			UNITST_LT(a, b)
#define UNITST_LE_F(a, b)			UNITST_LE(a, b)
#define UNITST_GT_F(a, b)			UNITST_GT(a, b)
#define UNITST_GE_F(a, b)			UNITST_GE(a, b)

#define UNITST_EQ_D(a, b)			UNITST_EQ(a, b)
#define UNITST_NE_D(a, b)			UNITST_NE(a, b)
#define UNITST_LT_D(a, b)			UNITST_LT(a, b)
#define UNITST_LE_D(a, b)			UNITST_LE(a, b)
#define UNITST_GT_D(a, b)			UNITST_GT(a, b)
#define UNITST_GE_D(a, b)			UNITST_GE(a, b)

#define UNITST_EQ_LD(a, b)			UNITST_EQ(a, b)
#define UNITST_NE_LD(a, b)			UNITST_NE(a, b)
#define UNITST_LT_LD(a, b)			UNITST_LT(a, b)
#define UNITST_LE_LD(a, b)			UNITST_LE(a, b)
#define UNITST_GT_LD(a, b)			UNITST_GT(a, b)
#define UNITST_GE_LD(a, b)			UNITST_GE(a, b)

#endif	// UNITST_H
