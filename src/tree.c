/**
 *  @file   tree.h
 *  @brief  avl tree
 *  @author Masashi KITAMURA
 *  @date   1993-2017
 *  @note
 *      Boost Software License Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

#if 0
  #define MSGF(x)   (printf x)
#else
  #define MSGF(x)
#endif

/*---------------------------------------------------------------------------*/

static TREE*        curTree;
static void*        elem;

static TREE_CMP     cmpElement;
static TREE_DEL     delElement;
static TREE_NEW     newElement;
static TREE_MALLOC  funcMalloc;
static TREE_FREE    funcFree;

/** 二分木を作成します。引数は、要素の作成,削除,比較の関数ポインタ
 */
TREE*   TREE_Make(TREE_NEW newElement,TREE_DEL delElement,TREE_CMP cmpElement, TREE_MALLOC funcMalloc, TREE_FREE funcFree)
{
    TREE*   p;

    if (funcMalloc == NULL)
        return NULL;
    p = funcMalloc(sizeof(TREE));
    if (p) {
        p->root = NULL;
        p->node = NULL;
        p->flag = 0;
        p->newElement  = newElement;
        p->delElement  = delElement;
        p->cmpElement  = cmpElement;
        p->malloc      = funcMalloc;
        p->free        = funcFree;
    }
    return p;
}

/*----------------------------------*/
#if 0

static int InsertNode(TREE_NODE* pp, int  lrFlg)
{
    int l,r,nl,nr;
    int  b;
    TREE_NODE* p,*lp,*lrp,*lrlp,*lrrp;

    /* MSGF(("pp=%p  ",pp)); */
    if (pp == NULL)
        return 0;
    p = pp->link[lrFlg];
    /* MSGF(("p=%p  ",p)); */
    if (p == NULL) {
        curTree->node = p = pp->link[lrFlg] = funcMalloc(sizeof(TREE_NODE));
        if (p == NULL)
            return 0;
        memset(p,0x00,sizeof(TREE_NODE));
      /*
        p->link[0] = p->link[1] = NULL;
        p->avltFlg = 0;
      */
        curTree->flag = 1;  /* 新たに作成された */
        p->element = newElement(elem);
        /* MSGF(("elem=%p\n",p->element));*/
        return 3;
    }
    b = cmpElement(elem,p->element);
    /* MSGF(("b=%d\n",b));*/
    if (b == 0) {
        curTree->node = p;
        return 0;
    }
    if (b < 0) {
        l = 0;  r = 1;  nl = 1; nr = 2;
    } else {
        l = 1;  r = 0;  nl = 2; nr = 1;
    }

    b = InsertNode(p,l);
    if (b == 0) {
        return 0;
    }
    if (p->avltFlg == nr) {
        p->avltFlg = 0;
        return 0;
    } else if (p->avltFlg == 0) {
        p->avltFlg = nl;
        return p->avltFlg;
    }
    if (b == nl) {
        lp          = p->link[l];
        lrp         = lp->link[r];
        p->link[l]  = lrp;
        p->avltFlg  = 0;
        lp->link[r] = p;
        lp->avltFlg = 0;
        pp->link[lrFlg] = lp;
    } else if (b == nr) {
        lp          = p->link[l];
        lrp         = lp->link[r];
        lrlp        = lrp->link[l];
        lrrp        = lrp->link[r];
        pp->link[lrFlg] = lrp;
        p->link[l]  = lrrp;
        lp->link[r] = lrlp;
        lrp->link[l]= lp;
        lrp->link[r]= p;
        lp->avltFlg = p->avltFlg = 0;
        if (lrp->avltFlg == nl)
            p->avltFlg = nr;
        else if (lrp->avltFlg == nr)
            lp->avltFlg = nl;
        lrp->avltFlg = 0;
    } else {
        printf("PRGERR:InsertNode avltFlg = 3\n");
        exit(1);
    }
    return 0;
}


void *TREE_Insert(TREE* tree, void *e)
    /* 要素を木に挿入 */
{
    static TREE_NODE tmp;

    curTree     = tree;
    funcMalloc  = tree->malloc;
    cmpElement  = tree->cmpElement;
    newElement  = tree->newElement;
    tmp.link[0] = tree->root;
    curTree->flag = 0;
    curTree->node = NULL;
    elem        = e;
    InsertNode(&tmp, 0);
    tree->root = tmp.link[0];
    if (curTree->node)
        return curTree->node->element;
    return NULL;
}

#else

static int  insRebalance(TREE_NODE** pp, int dir)
{
    int ndr, pt_dir, pt_ndr,grown;
    TREE_NODE* ap,*bp,*cp;

    ndr = (dir == 0) ? 1 : 0;
    pt_dir = (1<<dir);
    pt_ndr = (1<<ndr);

    grown = 0;
    ap = *pp;
    if (ap->avltFlg == pt_ndr) {        /* 元々偏っていれば、偏った側より１減るのでバランスになる*/
        ap->avltFlg =  0;
    } else if (ap->avltFlg == 0) {      /* 元々バランス状態ならば */
        ap->avltFlg |= pt_dir;          /* 削除の反対側に偏る */
        grown = 1;
    } else {                            /* 木の再構成 */
        bp = ap->link[dir];
        if (bp->avltFlg == pt_dir) {    /* 一回転 */
            ap->link[dir] = bp->link[ndr];
            bp->link[ndr] = ap;
            ap->avltFlg = 0;
            bp->avltFlg = 0;
            *pp = bp;
        } else if (bp->avltFlg == pt_ndr) {         /* 二回転 */
            cp = bp->link[ndr];
            ap->link[dir] = cp->link[ndr];
            bp->link[ndr] = cp->link[dir];
            cp->link[ndr] = ap;
            cp->link[dir] = bp;
            if (cp->avltFlg != pt_ndr) {
                bp->avltFlg =  0;
            } else {
                bp->avltFlg =  pt_dir;
            }
            if (cp->avltFlg != pt_dir) {
                ap->avltFlg =  0;
            } else {
                ap->avltFlg =  pt_ndr;
            }
            cp->avltFlg = 0;
            *pp = cp;
        } else {
            ;   /* これはありえない */
        }
    }
    return grown;
}

static int  insNode(TREE_NODE** pp)
{
    int grown,b,g;

    grown = 0;
    if (pp == NULL)
        return 0;
    if (*pp == NULL) {
        curTree->node = *pp = funcMalloc(sizeof(TREE_NODE));
        if (*pp == NULL)
            return 0;
        memset(*pp, 0x00, sizeof(TREE_NODE));
        curTree->flag = 1;  /* 新たに作成された */
        (*pp)->element = newElement(elem);
        /* MSGF(("elem=%d\n",(*pp)->element));*/
        grown = 1;
        return grown;
    }
    b = cmpElement(elem, (*pp)->element);
    /* MSGF(("b=%d\n",b));*/
    if (b == 0) {
        curTree->node = (*pp);
        return 0;
    }
  #if 1
    b = (b > 0) ? 1 : 0;
    g = insNode( & ((*pp)->link[b]) );
    if (g)
        grown = insRebalance(pp, b);
  #else
    if (b < 0) {
        g = insNode( & ((*pp)->link[0/*left*/]) );
        if (g)
            grown = insRebalance(pp, 0);
    } else if (b > 0) {
        g = insNode( & ((*pp)->link[1/*right*/]) );
        if (g)
            grown = insRebalance(pp, 1/*right*/);
    }
  #endif
    return grown;
}


/** 要素を木に挿入
 */
void *TREE_Insert(TREE* tree, void* e)
{
    curTree     = tree;
    funcMalloc  = tree->malloc;
    cmpElement  = tree->cmpElement;
    newElement  = tree->newElement;
    curTree->flag = 0;
    curTree->node = NULL;
    elem        = e;
    insNode(&tree->root);
    if (curTree->node)
        return curTree->node->element;
    return NULL;
}



#endif


/*----------------------------------*/
#if 0
static TREE_NODE* SearchElement(TREE_NODE* np, void *p)
{
    int n;

    if (np == NULL)
        return NULL;
    if ((n = cmpElement(p,np->element)) == 0)
        return np;
    if (n < 0 && np->link[0])
        return SearchElement(np->link[0],p);
    else if (np->link[1])
        return SearchElement(np->link[1],p);
    return NULL;
}

void *TREE_Search(TREE* tree, void *p)
    /* 木から要素を探す */
{
    TREE_NODE* np;

    cmpElement  = tree->cmpElement;
    tree->node = np = SearchElement(tree->root, p);
    if (np == NULL)
        return NULL;
    return np->element;
}
#else

/** 木から要素を探す
 */
void *TREE_Search(TREE* tree, void *e)
{
    TREE_NODE* np;
    int  n;

    cmpElement  = tree->cmpElement;
    np = tree->root;
    while (np) {
        n = cmpElement(e, np->element);
        if (n < 0)
            np = np->link[0];
        else if (n > 0)
            np = np->link[1];
        else
            break;
    }
    tree->node = np;
    if (np == NULL)
        return NULL;
    return np->element;
}

#endif

/*----------------------------------*/
#if 10

/** 削除で木のバランスを保つための処理
 */
static int      delRebalance(TREE_NODE** pp, int dir)
{
    int         shrinked, ndr, pt_dir, pt_ndr;
    TREE_NODE*  ap;
    TREE_NODE*  bp;
    TREE_NODE*  cp;

    ndr = (dir == 0) ? 1 : 0;
    pt_dir = (1<<dir);
    pt_ndr = (1<<ndr);

    ap = *pp;
    if (ap->avltFlg == 0) {             /* 元々バランス状態ならば */
        ap->avltFlg |= pt_ndr;          /* 削除の反対側に偏る */
        shrinked    =  0;
    } else if (ap->avltFlg == pt_dir) { /* 元々偏っていれば、偏った側より１減るのでバランスになる*/
        ap->avltFlg =  0;
        shrinked    =  1;
    } else {                            /* 木の再構成 */
        bp = ap->link[ndr];
        if (bp->avltFlg != pt_dir) {    /* 一回転 */
            ap->link[ndr] = bp->link[dir];
            bp->link[dir] = ap;
            if (bp->avltFlg == 0) {
                ap->avltFlg = pt_ndr;
                bp->avltFlg = pt_dir;
                shrinked = 0;
            } else {
                ap->avltFlg = 0;
                bp->avltFlg = 0;
                shrinked = 1;
            }
            *pp = bp;
        } else {                        /* 二回転 */
            cp = bp->link[dir];
            ap->link[ndr] = cp->link[dir];
            bp->link[dir] = cp->link[ndr];
            cp->link[dir] = ap;
            cp->link[ndr] = bp;
            if (cp->avltFlg != pt_ndr) {
                ap->avltFlg =  0;
            } else {
                ap->avltFlg =  pt_dir;
            }
            if (cp->avltFlg != pt_dir) {
                bp->avltFlg =  0;
            } else {
                bp->avltFlg =  pt_ndr;
            }
            cp->avltFlg = 0;
            *pp = cp;
            shrinked = 1;
        }
    }
    return shrinked;
}


int delExtractMax(TREE_NODE** pp, TREE_NODE** qq)
{
    int     s, shrinked;
    enum {L=0,R=1};

    if ((*pp)->link[R] == NULL) {
        *qq = *pp;
        *pp = (*pp)->link[L];
        shrinked = 1;
    } else {
        shrinked = 0;
        s = delExtractMax(&((*pp)->link[R]), qq);
        if (s)
            shrinked = delRebalance(pp, R);
    }
    return shrinked;
}

static int  DeleteNode(TREE_NODE** pp)
{
    int         c,s,shrinked;
    TREE_NODE*  p;
    TREE_NODE*  t;
    enum {L=0,R=1};

    shrinked = 0;
    p = *pp;
    if (p == NULL) {
        return -1;      /* 削除すべき node が見つからなか */
        /*printf ("PRGERR: AVL-TREE DELETE\n");*/
        /*exit(1);*/
    }
    c = cmpElement(elem,p->element);
    if (c < 0) {
        s = DeleteNode(&p->link[L]);
        if (s < 0)
            return -1;
        if (s)
            shrinked = delRebalance(&p, L);
    } else if (c > 0) {
        s = DeleteNode(&p->link[R]);
        if (s < 0)
            return -1;
        if (s)
            shrinked = delRebalance(&p, R);
    } else {
        if (p->link[L] == NULL) {
            t = p;
            p = p->link[R];
            delElement(t->element);
            funcFree(t);
            shrinked = 1;
        } else {
            s = delExtractMax(&p->link[L], &t);
            t->link[L] = p->link[L];
            t->link[R] = p->link[R];
            t->avltFlg = p->avltFlg;
            delElement(p->element);
            funcFree(p);
            p = t;
            if (s)
                shrinked = delRebalance(&p, L);
        }
    }
    *pp = p;
    return shrinked;
}

/** 要素を木から削除
 */
int TREE_Delete(TREE* tree, void* e)
{
    int     c;
    curTree     = tree;
    funcFree    = tree->free;
    cmpElement  = tree->cmpElement;
    delElement  = tree->delElement;
    elem        = e;
    c = DeleteNode(&tree->root);
    if (c < 0)
        return -1;  /* 削除すべきものがみくからなかった */
    return 0;
}


#endif



/*----------------------------------*/
static void DelAllNode(TREE_NODE* np)
{
    if (np == NULL)
        return;
    if (np->link[0])
        DelAllNode(np->link[0]);
    if (np->link[1])
        DelAllNode(np->link[1]);
    if (delElement)
        delElement(np->element);
    funcFree(np);
    return;
}

/** 木を消去する
 */
void TREE_Clear(TREE* tree)
{
    delElement  = tree->delElement;
    funcFree    = tree->free;
    DelAllNode(tree->root);
    funcFree(tree);
    return;
}



/*---------------------------------------------------------------------------*/
#if 1

static void DoElement(TREE_NODE* np, void (*DoElem)(void*))
{
    if (np == NULL)
        return;
    if (np->link[0])
        DoElement(np->link[0],DoElem);
    DoElem(np->element);
    if (np->link[1])
        DoElement(np->link[1],DoElem);
    return;
}

/** 木のすべての要素について func(void *) を実行.
 *  funcには要素へのポインタが渡される
 */
void TREE_DoAll(TREE* tree, void (*func)(void*))
{
    DoElement(tree->root,func);
}

#else
static void (*DoElem)(void *);

static void DoElement(TREE_NODE* np)
{
    if (np == NULL)
        return;
    if (np->link[0])
        DoElement(np->link[0]);
    DoElem(np->element);
    if (np->link[1])
        DoElement(np->link[1]);
    return;
}

/** 木のすべての要素について func(void *) を実行.
 *  funcには要素へのポインタが渡される
 */
void TREE_DoAll(TREE* tree, void (*func)(void *))
{
    DoElem = func;
    DoElement(tree->root);
}
#endif



#if 0
/*---------------------------------------------------------------------------*/
static TREE_NODE* listCur;
static TREE_NODE* listTop;

static TREE_NODE* Tree2dlist_sub(TREE_NODE* dp)
{
    if (dp == NULL)
        return NULL;
    if (dp->link[0] == NULL && listTop == NULL) {
        listCur = listTop = dp;
    } else {
        if (dp->link[0])
            Tree2dlist_sub(dp->link[0]);
        if (listTop == NULL) {
            printf("PRGERR:tree2list がおかしいぞ!");
            exit(1);
        }
        listCur->link[1] = dp;
        dp->link[0] = listCur;
        listCur = dp;
    }
    if (dp->link[1])
        Tree2dlist_sub(dp->link[1]);
    return dp;
}

/** 二分木を双方向リストに変換
 */
void TREE_ToDList(TREE* tp)
{
    listTop = NULL;
    listCur = NULL;
    Tree2dlist_sub(tp->root);
    tp->node = tp->root = listTop;
}

/** 現在位置を双方向リストの先頭に移す
 */
void *TREE_DListFirst(TREE* tp)
{
    tp->node = tp->root;
    if (tp->node)
        return tp->node->element;
    return NULL;
}

/** 次の要素へ移動
 */
void *TREE_DListNext(TREE* tp)
{
    if (tp->node) {
        tp->node = tp->node->link[1];
        if (tp->node)
            return tp->node->element;
    }
    return NULL;
}

/** 双方向リスト(もともとは木）を消去
 */
void TREE_DListClear(TREE* tp)
{
    TREE_NODE* p;
    TREE_NODE* q;

    p = tp->root;
    while (p) {
        q = p->link[1];
        if (delElement)
            delElement(p->element);
        funcFree(p);
        p = q;
    }
    funcFree(tp);
}


/*---------------------------------------------------------------------------*/
#endif
