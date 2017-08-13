/**
 *  @file   strexpr.h
 *  @brief  ����������v�Z
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1996-2017
 *  @note
 *      Boost Software License Version 1.0
 */
#ifndef STREXPER_H
#define STREXPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t     strexpr_val_t;
//typedef long strexpr_val_t;

int  StrExpr(char const *s, char const** s_nxt, strexpr_val_t* val);

void StrExpr_SetNameChkFunc(int (*name2valFnc)(char const* name, strexpr_val_t* valp));
    /* name2valFnc �́A���O���n����A����Ȃ�0��Ԃ����̖��O�̒l�� *valp�ɂ����. �ُ�Ȃ�-1��Ԃ��֐���ݒ肷�邱�� */

void StrExpr_SetMbcMode(int mode);


#ifdef __cplusplus
}
#endif

#endif  // STREXPER_H
