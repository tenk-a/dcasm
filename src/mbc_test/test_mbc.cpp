/**
 *  @file test_mbc.cpp
 *  @brief mbc.c �̃e�X�g
 *  @note
 *      - �y���U���ȃ`�F�b�N. �Ƃ肠�����g���Ă݂����x.
 *      - ���E�������͖��`�F�b�N. �����Ƃ����`�F�b�N����Ȃ��̂Œ���.
 */
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define USE_UNITST
#define UNUSE_UNITST_STRSTREAM
#include "unitst.h"

#include "../mbc.h"



UNITST_TEST(mbc_test_win_cp932) {
    // ���{��windows��p�̃`�F�b�N.
    UNITST_EQ(mbs_islead('a') , 0);
    UNITST_EQ(mbs_islead('�') , 0);
    UNITST_EQ(mbs_islead(0x81) , 1);
    const char*  str1 = "t�e";
    const char*  s1   = str1;
    unsigned c = mbs_peekc(s1);
    UNITST_EQ(c, 't');
    c = mbs_getc(&s1);
    UNITST_EQ(c, 't');
    c = mbs_peekc(s1);
    UNITST_EQ(c, 0x8365);
    c = mbs_getc(&s1);
    UNITST_EQ(c, 0x8365);

    c = mbs_getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = mbs_inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = mbs_inc(s1);
    UNITST_EQ(s1, str1+1+2);

    char buf[1024];
    char* d = buf;
    mbs_putc(&d, 't');
    mbs_putc(&d, 0x8365);
    mbs_putc(&d, 0);
    UNITST_EQ_STR(buf, "t�e");

    d = buf;
    mbs_setc(d+0, 0x8365);
    mbs_setc(d+2, 'T');
    mbs_setc(d+3, 0);
    UNITST_EQ_STR(buf, "�eT");

    UNITST_EQ(mbs_len1("�e"),2);
    UNITST_EQ(mbs_len1("A"), 1);
    UNITST_EQ(mbs_chrLen(0x8365), 2);
    UNITST_EQ(mbs_chrLen('A')   , 1);
    UNITST_EQ(mbs_chrWidth('A') , 1);
    UNITST_EQ(mbs_chrWidth(0x8365), 2);

    UNITST_EQ(mbs_strLen("t�e"), 3);
    UNITST_EQ(mbs_adjust_size("t�e", 2+1), 1);
    UNITST_EQ(mbs_adjust_size("t�e", 3+1), 3);

    UNITST_EQ(mbs_sizeToWidth("test�e�X�g", 10), 10);
    UNITST_EQ(mbs_sizeToChrs ("test�e�X�g", 10),  7);
    UNITST_EQ(mbs_chrsToSize ("test�e�X�g",  6),  8);
    UNITST_EQ(mbs_chrsToWidth("test�e�X�g",  6),  8);
    UNITST_EQ(mbs_widthToSize("test�e�X�g",  8),  8);
    UNITST_EQ(mbs_widthToChrs("test�e�X�g",  8),  6);

    UNITST_EQ_STR(mbs_cpy(buf, 6+1, "test�e�X�g"), "test�e");
    UNITST_EQ_STR(mbs_cpy(buf, 5+1, "test�e�X�g"), "test");
    UNITST_EQ_STR(mbs_cat(buf, 8+1, "�e�X�g"    ), "test�e�X");

    UNITST_EQ_STR(mbs_cpyNC(buf, 10+1, "test�e�X�g", 5), "test�e");
    UNITST_EQ_STR(mbs_catNC(buf, 10+1, "�X�g�Ă���", 2), "test�e�X�g");

    UNITST_EQ_STR(mbs_cpyWidth(buf, 10+1, "test�e�X�g", 6), "test�e");
    UNITST_EQ_STR(mbs_catWidth(buf, 10+1, "�X�g�Ă���", 4), "test�e�X�g");
}


// utf8. ���Ƃ��đ�G�c�ɓ��{�̔��p/�S�p���l��.
UNITST_TEST(mbc_test_japan_utf8) {
    const Mbc_Env* m = mbc_env_create("japan.utf8");
    UNITST_EQ(m->islead('a') , 0);
    UNITST_EQ(m->islead(0x80) , 1);
    const char*  str1 = "t\xE3\x83\x86";    // "t�e"
    const char*  s1   = str1;
    unsigned c = m->peekc(s1);
    UNITST_EQ(c, 't');
    c = m->getc(&s1);
    UNITST_EQ(c, 't');
    c = m->peekc(s1);
    UNITST_EQ(c, 0x30C6);
    c = m->getc(&s1);
    UNITST_EQ(c, 0x30C6);

    c = m->getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1+3);

    char buf[1024];
    char* d = buf;
    m->putc(&d, 't');
    m->putc(&d, 0x30C6);
    m->putc(&d, 0);
    UNITST_EQ_STR(buf, str1);

    d = buf;
    m->setc(d+0, 0x30C6);
    m->setc(d+3, 'T');
    m->setc(d+4, 0);
    UNITST_EQ_STR(buf, "\xE3\x83\x86T");

    UNITST_EQ(m->len1("\xE3\x83\x86"),3);
    UNITST_EQ(m->len1("A"), 1);
    UNITST_EQ(m->chrLen(0x30C6), 3);
    UNITST_EQ(m->chrLen('A')   , 1);
    UNITST_EQ(m->chrWidth('A') , 1);
    UNITST_EQ(m->chrWidth(0x30C6), 2);

    UNITST_EQ(m->strLen(str1), 4);
    UNITST_EQ(m->adjust_size(str1, 3+1), 1);
    UNITST_EQ(m->adjust_size(str1, 4+1), 4);

    UNITST_EQ(m->sizeToWidth("t\xE3\x83\x86",  4),  3);
    UNITST_EQ(m->sizeToChrs ("t\xE3\x83\x86",  4),  2);
    UNITST_EQ(m->chrsToSize ("t\xE3\x83\x86",  2),  4);
    UNITST_EQ(m->chrsToWidth("t\xE3\x83\x86",  2),  3);
    UNITST_EQ(m->widthToSize("t\xE3\x83\x86",  3),  4);
    UNITST_EQ(m->widthToChrs("t\xE3\x83\x86",  3),  2);

    UNITST_EQ_STR(m->cpy(buf, 7+1, "test\xE3\x83\x86"), "test\xE3\x83\x86");
    UNITST_EQ_STR(m->cpy(buf, 6+1, "test\xE3\x83\x86"), "test");
    UNITST_EQ_STR(m->cat(buf, 8+1, "\xE3\x83\x86ST"  ), "test\xE3\x83\x86S");

    UNITST_EQ_STR(m->cpyNC(buf, 20+1, "test\xE3\x83\x86st", 5), "test\xE3\x83\x86");
    UNITST_EQ_STR(m->catNC(buf, 1024, "\xE3\x83\x86\xE3\x83\x86", 2), "test\xE3\x83\x86\xE3\x83\x86\xE3\x83\x86");

    UNITST_EQ_STR(m->cpyWidth(buf, 20+1, "test\xE3\x83\x86st", 6), "test\xE3\x83\x86");
    UNITST_EQ_STR(m->catWidth(buf, 1024, "\xE3\x83\x86\xE3\x83\x86", 4), "test\xE3\x83\x86\xE3\x83\x86\xE3\x83\x86");
}


// utf8. �����̕������ׂ�1�Ōv�Z.
UNITST_TEST(mbc_test_notjp_utf8) {
    const Mbc_Env* m = mbc_env_create("en.utf8");
    UNITST_EQ(m->islead('a') , 0);
    UNITST_EQ(m->islead(0x80) , 1);
    const char*  str1 = "t\xE3\x83\x86";    // "t�e"
    const char*  s1   = str1;
    unsigned c = m->peekc(s1);
    UNITST_EQ(c, 't');
    c = m->getc(&s1);
    UNITST_EQ(c, 't');
    c = m->peekc(s1);
    UNITST_EQ(c, 0x30C6);
    c = m->getc(&s1);
    UNITST_EQ(c, 0x30C6);

    c = m->getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1+3);

    char buf[1024];
    char* d = buf;
    m->putc(&d, 't');
    m->putc(&d, 0x30C6);
    m->putc(&d, 0);
    UNITST_EQ_STR(buf, str1);

    d = buf;
    m->setc(d+0, 0x30C6);
    m->setc(d+3, 'T');
    m->setc(d+4, 0);
    UNITST_EQ_STR(buf, "\xE3\x83\x86T");

    UNITST_EQ(m->len1("\xE3\x83\x86"),3);
    UNITST_EQ(m->len1("A"), 1);
    UNITST_EQ(m->chrLen(0x30C6), 3);
    UNITST_EQ(m->chrLen('A')   , 1);
    UNITST_EQ(m->chrWidth('A') , 1);
    UNITST_EQ(m->chrWidth(0x30C6), 1);

    UNITST_EQ(m->strLen(str1), 4);
    UNITST_EQ(m->adjust_size(str1, 3+1), 1);
    UNITST_EQ(m->adjust_size(str1, 4+1), 4);

    UNITST_EQ(m->sizeToWidth("t\xE3\x83\x86",  4),  2);
    UNITST_EQ(m->sizeToChrs ("t\xE3\x83\x86",  4),  2);
    UNITST_EQ(m->chrsToSize ("t\xE3\x83\x86",  2),  4);
    UNITST_EQ(m->chrsToWidth("t\xE3\x83\x86",  2),  2);
    UNITST_EQ(m->widthToSize("t\xE3\x83\x86",  2),  4);
    UNITST_EQ(m->widthToChrs("t\xE3\x83\x86",  2),  2);

    UNITST_EQ_STR(m->cpy(buf, 7+1, "test\xE3\x83\x86"), "test\xE3\x83\x86");
    UNITST_EQ_STR(m->cpy(buf, 6+1, "test\xE3\x83\x86"), "test");
    UNITST_EQ_STR(m->cat(buf, 8+1, "\xE3\x83\x86ST"  ), "test\xE3\x83\x86S");

    UNITST_EQ_STR(m->cpyNC(buf, 20+1, "test\xE3\x83\x86st", 5), "test\xE3\x83\x86");
    UNITST_EQ_STR(m->catNC(buf, 1024, "\xE3\x83\x86\xE3\x83\x86", 2), "test\xE3\x83\x86\xE3\x83\x86\xE3\x83\x86");

    UNITST_EQ_STR(m->cpyWidth(buf, 20+1, "test\xE3\x83\x86st", 5), "test\xE3\x83\x86");
    UNITST_EQ_STR(m->catWidth(buf, 1024, "\xE3\x83\x86\xE3\x83\x86", 2), "test\xE3\x83\x86\xE3\x83\x86\xE3\x83\x86");
}



// euc-jp.
UNITST_TEST(mbc_test_japan_eucjp) {
    const Mbc_Env* m = mbc_env_create("japan.EUCJP");
    UNITST_EQ(m->islead('a') , 0);
    UNITST_EQ(m->islead(0x81) , 0);
    UNITST_EQ(m->islead(0xA1) , 1);
    const char*  str1 = "t\xa5\xc6";    // "t�e"
    const char*  s1   = str1;
    unsigned c = m->peekc(s1);
    UNITST_EQ(c, 't');
    c = m->getc(&s1);
    UNITST_EQ(c, 't');
    c = m->peekc(s1);
    UNITST_EQ(c, 0xA5C6);
    c = m->getc(&s1);
    UNITST_EQ(c, 0xA5C6);

    c = m->getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1+2);

    char buf[1024];
    char* d = buf;
    m->putc(&d, 't');
    m->putc(&d, 0xA5C6);
    m->putc(&d, 0);
    UNITST_EQ_STR(buf, str1);

    d = buf;
    m->setc(d+0, 0xA5C6);
    m->setc(d+2, 'T');
    m->setc(d+3, 0);
    UNITST_EQ_STR(buf, "\xA5\xC6T");

    UNITST_EQ(m->len1("\xA5\xC6"),2);
    UNITST_EQ(m->len1("A"), 1);
    UNITST_EQ(m->chrLen(0xA5C6), 2);
    UNITST_EQ(m->chrLen('A')   , 1);
    UNITST_EQ(m->chrWidth('A') , 1);
    UNITST_EQ(m->chrWidth(0x8eB1) , 1);
    UNITST_EQ(m->chrWidth(0xA5C6), 2);

    UNITST_EQ(m->strLen(str1), 3);
    UNITST_EQ(m->adjust_size(str1, 2+1), 1);
    UNITST_EQ(m->adjust_size(str1, 3+1), 3);

    UNITST_EQ(m->sizeToWidth("t\xA5\xC6",  3),  3);
    UNITST_EQ(m->sizeToChrs ("t\xA5\xC6",  3),  2);
    UNITST_EQ(m->chrsToSize ("t\xA5\xC6",  2),  3);
    UNITST_EQ(m->chrsToWidth("t\xA5\xC6",  2),  3);
    UNITST_EQ(m->widthToSize("t\xA5\xC6",  3),  3);
    UNITST_EQ(m->widthToChrs("t\xA5\xC6",  3),  2);

    UNITST_EQ_STR(m->cpy(buf, 6+1, "test\xA5\xC6"), "test\xA5\xC6");
    UNITST_EQ_STR(m->cpy(buf, 5+1, "test\xA5\xC6"), "test");
    UNITST_EQ_STR(m->cat(buf, 7+1, "\xA5\xC6ST"  ), "test\xA5\xC6S");

    UNITST_EQ_STR(m->cpyNC(buf, 20+1, "test\xA5\xC6st", 5), "test\xA5\xC6");
    UNITST_EQ_STR(m->catNC(buf, 1024, "\xA5\xC6\xA5\xC6", 2), "test\xA5\xC6\xA5\xC6\xA5\xC6");

    UNITST_EQ_STR(m->cpyWidth(buf, 20+1, "test\xA5\xC6st", 6), "test\xA5\xC6");
    UNITST_EQ_STR(m->catWidth(buf, 1024, "\xA5\xC6\xA5\xC6", 4), "test\xA5\xC6\xA5\xC6\xA5\xC6");
}



// big5: �Ȃ�̕������͂���Ȃ�����ǁA�����R�[�h�͈͂̂P����.
UNITST_TEST(mbc_test_big5) {
    const Mbc_Env* m = mbc_env_create(".BIG5");
    UNITST_EQ(m->islead('a') , 0);
    UNITST_EQ(m->islead(0x81) , 0);
    UNITST_EQ(m->islead(0xA1) , 1);
    const char*  str1 = "t\xA1\x41";
    const char*  s1   = str1;
    unsigned c = m->peekc(s1);
    UNITST_EQ(c, 't');
    c = m->getc(&s1);
    UNITST_EQ(c, 't');
    c = m->peekc(s1);
    UNITST_EQ(c, 0xA141);
    c = m->getc(&s1);
    UNITST_EQ(c, 0xA141);

    c = m->getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1+2);

    char buf[1024];
    char* d = buf;
    m->putc(&d, 't');
    m->putc(&d, 0xA141);
    m->putc(&d, 0);
    UNITST_EQ_STR(buf, str1);

    d = buf;
    m->setc(d+0, 0xA141);
    m->setc(d+2, 'T');
    m->setc(d+3, 0);
    UNITST_EQ_STR(buf, "\xA1\x41T");

    UNITST_EQ(m->len1("\xA1\x41"),2);
    UNITST_EQ(m->len1("A"), 1);
    UNITST_EQ(m->chrLen(0xA141), 2);
    UNITST_EQ(m->chrLen('A')   , 1);
    UNITST_EQ(m->chrWidth('A') , 1);
    UNITST_EQ(m->chrWidth(0xA141), 2);

    UNITST_EQ(m->strLen(str1), 3);
    UNITST_EQ(m->adjust_size(str1, 2+1), 1);
    UNITST_EQ(m->adjust_size(str1, 3+1), 3);

    UNITST_EQ(m->sizeToWidth("t\xA1\x41",  3),  3);
    UNITST_EQ(m->sizeToChrs ("t\xA1\x41",  3),  2);
    UNITST_EQ(m->chrsToSize ("t\xA1\x41",  2),  3);
    UNITST_EQ(m->chrsToWidth("t\xA1\x41",  2),  3);
    UNITST_EQ(m->widthToSize("t\xA1\x41",  3),  3);
    UNITST_EQ(m->widthToChrs("t\xA1\x41",  3),  2);

    UNITST_EQ_STR(m->cpy(buf, 6+1, "test\xA1\x41"), "test\xA1\x41");
    UNITST_EQ_STR(m->cpy(buf, 5+1, "test\xA1\x41"), "test");
    UNITST_EQ_STR(m->cat(buf, 7+1, "\xA1\x41ST"  ), "test\xA1\x41S");

    UNITST_EQ_STR(m->cpyNC(buf, 20+1, "test\xA1\x41st", 5), "test\xA1\x41");
    UNITST_EQ_STR(m->catNC(buf, 1024, "\xA1\x41\xA1\x41", 2), "test\xA1\x41\xA1\x41\xA1\x41");

    UNITST_EQ_STR(m->cpyWidth(buf, 20+1, "test\xA1\x41st", 6), "test\xA1\x41");
    UNITST_EQ_STR(m->catWidth(buf, 1024, "\xA1\x41\xA1\x41", 4), "test\xA1\x41\xA1\x41\xA1\x41");
}



// gbk: �Ȃ�̕������͂���Ȃ�����ǁA�����R�[�h�͈͂̂P�����̂���.
UNITST_TEST(mbc_test_gbk) {
    const Mbc_Env* m = mbc_env_create(".GBK");
    UNITST_EQ(m->islead('a') , 0);
    UNITST_EQ(m->islead(0x81) , 1);
    UNITST_EQ(m->islead(0xA1) , 1);
    const char*  str1 = "t\xA1\x41";
    const char*  s1   = str1;
    unsigned c = m->peekc(s1);
    UNITST_EQ(c, 't');
    c = m->getc(&s1);
    UNITST_EQ(c, 't');
    c = m->peekc(s1);
    UNITST_EQ(c, 0xA141);
    c = m->getc(&s1);
    UNITST_EQ(c, 0xA141);

    c = m->getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1+2);

    char buf[1024];
    char* d = buf;
    m->putc(&d, 't');
    m->putc(&d, 0xA141);
    m->putc(&d, 0);
    UNITST_EQ_STR(buf, str1);

    d = buf;
    m->setc(d+0, 0xA141);
    m->setc(d+2, 'T');
    m->setc(d+3, 0);
    UNITST_EQ_STR(buf, "\xA1\x41T");

    UNITST_EQ(m->len1("\xA1\x41"),2);
    UNITST_EQ(m->len1("A"), 1);
    UNITST_EQ(m->chrLen(0xA141), 2);
    UNITST_EQ(m->chrLen('A')   , 1);
    UNITST_EQ(m->chrWidth('A') , 1);
    UNITST_EQ(m->chrWidth(0xA141), 2);

    UNITST_EQ(m->strLen(str1), 3);
    UNITST_EQ(m->adjust_size(str1, 2+1), 1);
    UNITST_EQ(m->adjust_size(str1, 3+1), 3);

    UNITST_EQ(m->sizeToWidth("t\xA1\x41",  3),  3);
    UNITST_EQ(m->sizeToChrs ("t\xA1\x41",  3),  2);
    UNITST_EQ(m->chrsToSize ("t\xA1\x41",  2),  3);
    UNITST_EQ(m->chrsToWidth("t\xA1\x41",  2),  3);
    UNITST_EQ(m->widthToSize("t\xA1\x41",  3),  3);
    UNITST_EQ(m->widthToChrs("t\xA1\x41",  3),  2);

    UNITST_EQ_STR(m->cpy(buf, 6+1, "test\xA1\x41"), "test\xA1\x41");
    UNITST_EQ_STR(m->cpy(buf, 5+1, "test\xA1\x41"), "test");
    UNITST_EQ_STR(m->cat(buf, 7+1, "\xA1\x41ST"  ), "test\xA1\x41S");

    UNITST_EQ_STR(m->cpyNC(buf, 20+1, "test\xA1\x41st", 5), "test\xA1\x41");
    UNITST_EQ_STR(m->catNC(buf, 1024, "\xA1\x41\xA1\x41", 2), "test\xA1\x41\xA1\x41\xA1\x41");

    UNITST_EQ_STR(m->cpyWidth(buf, 20+1, "test\xA1\x41st", 6), "test\xA1\x41");
    UNITST_EQ_STR(m->catWidth(buf, 1024, "\xA1\x41\xA1\x41", 4), "test\xA1\x41\xA1\x41\xA1\x41");
}



// gb18030: �Ȃ�̕������͂���Ȃ�����ǁA�����R�[�h�͈͂̂P�����̂���.
UNITST_TEST(mbc_test_gb18030) {
    const Mbc_Env* m = mbc_env_create(".GBK");
    UNITST_EQ(m->islead('a') , 0);
    UNITST_EQ(m->islead(0x81) , 1);
    UNITST_EQ(m->islead(0xA1) , 1);
    const char*  str1 = "t\xA1\x31\xA1\x31";
    const char*  s1   = str1;
    unsigned c = m->peekc(s1);
    UNITST_EQ(c, 't');
    c = m->getc(&s1);
    UNITST_EQ(c, 't');
    c = m->peekc(s1);
    UNITST_EQ(c, 0xA131A131);
    c = m->getc(&s1);
    UNITST_EQ(c, 0xA131A131);

    c = m->getc(&s1);
    UNITST_EQ(c, 0);

    s1 = str1;
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1);
    s1 = m->inc(s1);
    UNITST_EQ(s1, str1+1+4);

    char buf[1024];
    char* d = buf;
    m->putc(&d, 't');
    m->putc(&d, 0xA131A131);
    m->putc(&d, 0);
    UNITST_EQ_STR(buf, str1);

    d = buf;
    m->setc(d+0, 0xA131A131);
    m->setc(d+4, 'T');
    m->setc(d+5, 0);
    UNITST_EQ_STR(buf, "\xA1\x31\xA1\x31T");

    UNITST_EQ(m->len1("\xA1\x31\xA1\x31"),4);
    UNITST_EQ(m->len1("A"), 1);
    UNITST_EQ(m->chrLen(0xA131A131), 4);
    UNITST_EQ(m->chrLen('A')   , 1);
    UNITST_EQ(m->chrWidth('A') , 1);
    UNITST_EQ(m->chrWidth(0xA141), 2);

    UNITST_EQ(m->strLen(str1), 5);
    UNITST_EQ(m->adjust_size(str1, 2+1), 1);
    UNITST_EQ(m->adjust_size(str1, 5+1), 5);

    UNITST_EQ(m->sizeToWidth("t\xA1\x31\xA1\x31",  5),  3);
    UNITST_EQ(m->sizeToChrs ("t\xA1\x31\xA1\x31",  5),  2);
    UNITST_EQ(m->chrsToSize ("t\xA1\x31\xA1\x31",  2),  5);
    UNITST_EQ(m->chrsToWidth("t\xA1\x31\xA1\x31",  2),  3);
    UNITST_EQ(m->widthToSize("t\xA1\x31\xA1\x31",  3),  5);
    UNITST_EQ(m->widthToChrs("t\xA1\x31\xA1\x31",  3),  2);

    UNITST_EQ_STR(m->cpy(buf, 8+1, "test\xA1\x31\xA1\x31"), "test\xA1\x31\xA1\x31");
    UNITST_EQ_STR(m->cpy(buf, 7+1, "test\xA1\x31\xA1\x31"), "test");
    UNITST_EQ_STR(m->cat(buf, 9+1, "\xA1\x31\xA1\x31ST"  ), "test\xA1\x31\xA1\x31S");

    UNITST_EQ_STR(m->cpyNC(buf, 20+1, "test\xA1\x31\xA1\x31st", 5), "test\xA1\x31\xA1\x31");
    UNITST_EQ_STR(m->catNC(buf, 1024, "\xA1\x31\xA1\x31\xA1\x31\xA1\x31", 2), "test\xA1\x31\xA1\x31\xA1\x31\xA1\x31\xA1\x31\xA1\x31");

    UNITST_EQ_STR(m->cpyWidth(buf, 20+1, "test\xA1\x31\xA1\x31st", 6), "test\xA1\x31\xA1\x31");
    UNITST_EQ_STR(m->catWidth(buf, 1024, "\xA1\x31\xA1\x31\xA1\x31\xA1\x31", 4), "test\xA1\x31\xA1\x31\xA1\x31\xA1\x31\xA1\x31\xA1\x31");
}


