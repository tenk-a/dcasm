/**
 *  @file   tree.h
 *  @brief  avl tree
 *  @author Masashi KITAMURA
 *  @date   1993-2017
 *  @note
 *      Boost Software License Version 1.0
 */
#ifndef __TREE_H__
#define __TREE_H__

typedef struct TREE_NODE {
    struct TREE_NODE*   link[2];
    void*               element;
    short               avltFlg;
} TREE_NODE;

typedef void* (*TREE_NEW)(void*);
typedef void  (*TREE_DEL)(void*);
typedef int   (*TREE_CMP)(void*, void*);
typedef void* (*TREE_MALLOC)(unsigned);
typedef void  (*TREE_FREE)(void*);

typedef struct TREE {
    TREE_NODE*  root;
    TREE_NODE*  node;
    int         flag;
    TREE_NEW    newElement;
    TREE_DEL    delElement;
    TREE_CMP    cmpElement;
    TREE_MALLOC malloc;
    TREE_FREE   free;
} TREE;

TREE* TREE_Make(TREE_NEW newElement,TREE_DEL delElement,TREE_CMP cmpElement, TREE_MALLOC funcMalloc, TREE_FREE funcFree);
void* TREE_Insert(TREE* tree, void* e);
void* TREE_Search(TREE* tree, void* p);
int   TREE_Delete(TREE* tree, void* e); /* 要素を木から削除 */
void  TREE_Clear(TREE* tree);
void  TREE_DoAll(TREE* tree, void (*func)(void*));

void  TREE_ToDList(TREE* tp);
void* TREE_DListFirst(TREE* tp);
void* TREE_DListNext(TREE* tp);
void  TREE_DListClear(TREE* tp);

#endif  /* __TREE_H__ */
