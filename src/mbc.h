/*
 *  @file   mbc.h
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef MBC_H_INCLUDED__
#define MBC_H_INCLUDED__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if 1 //!defined(MBC_USE_JIS_CONV) && !defined(MBC_UNUSE_MBC_JIS)
#define MBC_USE_JIS_CONV
//#define MBC_USE_X213_SJIS_EUCJP
#endif

#ifdef __cplusplus
extern "C" {
#endif

// code page (win)
typedef enum mbc_cp_t {
    MBC_CP_NONE     =     0,
    MBC_CP_UTF8     = 65001,
    MBC_CP_UTF16LE  =  1200,
    MBC_CP_UTF16BE  =  1201,
    MBC_CP_UTF32LE  = 12000,
    MBC_CP_UTF32BE  = 12001,
    MBC_CP_1BYTE    =   437/*kari*/,
    MBC_CP_SJIS     =   932,
    MBC_CP_EUCJP    = 51932/*20932*/,
 #ifdef _WIN32
    MBC_CP_DBC      =     1,
 #endif
} mbc_cp_t;

typedef struct mbc_enc_st {
    mbc_cp_t  cp;
    unsigned (*isLead)(unsigned c);
    unsigned (*checkChr)(unsigned c);
    unsigned (*getChr)(char const** str);
    unsigned (*peekChr)(char const* str);
    char*    (*chrNext)(char const* str);
    char*    (*setChr)(char* dst, char* e, unsigned c);
    unsigned (*len1)(char const* pChr);
    unsigned (*chrBytes)(unsigned chr);
    unsigned (*chrWidth)(unsigned chr);
    size_t   (*adjustSize)(char const* str, size_t size);
    int      (*cmp)(char const* lp, char const* rp);
    int      (*checkEncoding)(char const* str, size_t size, int lastBrokenOk);
} mbc_enc_st;

typedef struct mbc_enc_st const* mbc_enc_t;

extern mbc_enc_t const   mbc_enc_utf8;
extern mbc_enc_t const   mbc_enc_utf16le;
extern mbc_enc_t const   mbc_enc_utf16be;
extern mbc_enc_t const   mbc_enc_utf32le;
extern mbc_enc_t const   mbc_enc_utf32be;
extern mbc_enc_t const   mbc_enc_asc;

extern mbc_enc_t const   mbc_enc_cp932;
extern mbc_enc_t const   mbc_enc_eucjp;

#ifdef _WIN32
extern mbc_enc_t const   mbc_enc_dbc;
#endif

static inline mbc_cp_t mbc_encCP(mbc_enc_t mbc) { return mbc->cp; }
static inline unsigned mbc_isLead(mbc_enc_t mbc, unsigned c) { return mbc->isLead(c); }
static inline unsigned mbc_checkChr(mbc_enc_t mbc, unsigned c) { return mbc->checkChr(c); }
static inline unsigned mbc_getChr(mbc_enc_t mbc, char const** s) { return mbc->getChr(s); }
static inline unsigned mbc_peekChr(mbc_enc_t mbc, char const* s) { return mbc->peekChr(s); }
static inline char*    mbc_strChrNext(mbc_enc_t mbc, char const* s) { return mbc->chrNext(s); }
static inline char*    mbc_strSetChr(mbc_enc_t mbc, char* dst, char* e, unsigned c) { return mbc->setChr(dst, e, c); }
static inline unsigned mbc_strLen1(mbc_enc_t mbc, char const* s) { return mbc->len1(s); }
static inline unsigned mbc_chrBytes(mbc_enc_t mbc, unsigned c) { return mbc->chrBytes(c); }
static inline unsigned mbc_chrWidth(mbc_enc_t mbc, unsigned c) { return mbc->chrWidth(c); }
static inline size_t   mbc_strAdjustSize(mbc_enc_t mbc, char const* s, size_t sz) { return mbc->adjustSize(s, sz); }
static inline int      mbc_strCmp(mbc_enc_t mbc, char const* l, char const* r) { return mbc->cmp(l, r); }

mbc_enc_t mbc_cpToEnc(mbc_cp_t cp);
int     mbc_checkEnc(mbc_enc_t mbc, char const* str, size_t size, int lastBrokenOk);
size_t  mbc_strChrsToBytes( mbc_enc_t mbc, char const* str, size_t chrs);
size_t  mbc_strBytesToChrs( mbc_enc_t mbc, char const* str, size_t size);
size_t  mbc_strBytesToWidth(mbc_enc_t mbc, char const* str, size_t size);
size_t  mbc_strChrsToWidth( mbc_enc_t mbc, char const* str, size_t chrs);
size_t  mbc_strWidthToBytes(mbc_enc_t mbc, char const* str, size_t width);
size_t  mbc_strWidthToChrs( mbc_enc_t mbc, char const* str, size_t width);
size_t  mbc_strCpy(  mbc_enc_t mbc, char dst[], size_t dstSz,char const* src);
size_t  mbc_strLCpy( mbc_enc_t mbc, char dst[], size_t dstSz, char const* src, size_t l);
size_t  mbc_strCat(  mbc_enc_t mbc, char dst[], size_t dstSz, char const* src);
size_t  mbc_strCpyNC(mbc_enc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc);
size_t  mbc_strCatNC(mbc_enc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc);
size_t  mbc_strCpyWidth(mbc_enc_t mbc, char dst[], size_t dstSz, char const* src, size_t width);
size_t  mbc_strCatWidth(mbc_enc_t mbc, char dst[], size_t dstSz, char const* src, size_t width);

size_t  mbc_strCountCapa(mbc_enc_t dstMbc, mbc_enc_t srcMbc, char const* src, size_t srcSz);
size_t  mbc_strConv(mbc_enc_t dstMbc, char dst[], size_t dstSz, mbc_enc_t srcMbc, char const* src, size_t srcSz);
size_t  mbc_strConvUnicode(mbc_enc_t dstMbc, char dst[], size_t dstSz, mbc_enc_t srcMbc, char const* src, size_t srcSz);

/// 0:non 1:utf8-BOM 2:utf16le-BOM 3:utf16be-BOM 4:utf32le-BOM 5:utf32be-BOM
mbc_enc_t mbc_checkUnicodeBOM(char const* src, size_t len);
unsigned  mbc_checkUnicodeBOMi(char const* src, size_t len);
unsigned  mbc_getBOMbytes(char const* src, size_t len);
int       mbc_cpToUnicodeIdx(mbc_cp_t cp);
static inline int mbc_cpIsUnicode(mbc_cp_t cp) { return mbc_cpToUnicodeIdx(cp) > 0; }

#ifdef __cplusplus
mbc_enc_t mbc_autoEncodeCheck(char const* src, size_t len, int brokenEndChOk = 1, mbc_enc_t const * tbl = 0, size_t tblN = 0);
char*     mbc_strConvMalloc(mbc_enc_t dstMbc, mbc_enc_t srcMbc, char const* src, size_t srcSz, size_t* pDstSz = 0);
int       mbc_checkUTF8(char const* src, size_t len, int lastBrokenOk = 1); ///< 0:not  1:ascii(<=7f) >=2:utf8
#else
mbc_enc_t mbc_autoEncodeCheck(char const* src, size_t len, int brokenEndChOk, mbc_enc_t const * tbl, size_t tblN);
char*     mbc_strConvMalloc(mbc_enc_t dstMbc, mbc_enc_t srcMbc, char const* src, size_t srcSz, size_t* pDstSz);
int       mbc_checkUTF8(char const* src, size_t len, int lastBrokenOk);     ///< 0:not  1:ascii(<=7f) >=2:utf8
#endif

#ifdef _WIN32
mbc_enc_t mbc_enc_makeDBC(mbc_enc_st* mbcEnv, mbc_cp_t cp);
#endif

#ifdef MBC_USE_JIS_CONV
size_t  mbc_strConvJisType(mbc_enc_t dstEnc, char dst[], size_t dstSz
                , mbc_enc_t srcEnc, char const* src, size_t srcSz);
#endif

char*   mbc_strUpLow(mbc_enc_t mbc, char str[], unsigned flags);

#ifdef __cplusplus
}
#endif


// ---------------------------------------------------------------------------

#ifdef __cplusplus

template<class V>
mbc_enc_t mbc_autoEncodeCheck(V const& v, bool brokenEndChOk=1, mbc_enc_t const* tbl=NULL, size_t tblNum=0)
{
    //assert(sizeof(v[0]) == 1 || sizeof(v[0]) == 2 || sizeof(v[0]) == 4);
    return  mbc_autoEncodeCheck(&v[0], v.size()*sizeof(v[0]), brokenEndChOk, tbl, tblNum);
}

template<class D, class S>
D& mbc_convEnc(mbc_enc_t dstEnc, D& dst, mbc_enc_t srcEnc, S const& src)
{
    dst.resize(0);
    if (srcEnc == NULL)
        srcEnc = mbc_enc_utf8;
    size_t sz     = src.size()*sizeof(src[0]);
    char const* s = (char const*)&src[0];
    size_t l  = mbc_strCountCapa(dstEnc, srcEnc, s, sz);
    if (l > 0) {
        size_t dsz = (l + sizeof(dst[0]) - 1) / sizeof(dst[0]);
        dst.resize(dsz+1);
        l = mbc_strConv(dstEnc, (char*)&dst[0], l, srcEnc, s, sz);
        dsz = (l + sizeof(dst[0]) - 1) / sizeof(dst[0]);
        dst[dsz] = 0;
        dst.resize(dsz);
    }
    return dst;
}

#endif  // __cplusplus

#endif  /* MBC_H_INCLUDED__ */
