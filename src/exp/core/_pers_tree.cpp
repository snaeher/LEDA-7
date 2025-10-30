/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _pers_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/pers_tree.h>

#define LEFT 0
#define RIGHT 1
#define f_isred(p)   ((p)->f_color)
#define c_isred(p)   ((p)->c_color)
#define f_mred(p)    ((p)->f_color=1)
#define c_mred(p)    ((p)->c_color=1)
#define f_mblack(p)  ((p)->f_color=0)
#define c_mblack(p)  ((p)->c_color=0)
#define f_lchild(p)  ((p)->f_link[0])
#define c_lchild(p)  ((p)->c_link[0])
#define f_rchild(p)  ((p)->f_link[1])
#define c_rchild(p)  ((p)->c_link[1])
#define f_parent(p)  ((p)->f_link[2])
#define c_parent(p)  ((p)->c_link[2])
#define f_isleaf(p)  (f_lchild(p)==(p))
#define c_isleaf(p)  (c_lchild(p)==(p))

LEDA_BEGIN_NAMESPACE

void pers_rb_tree::f_rbclear(F_pers_tree_node* p)
{ if (!(f_isleaf(p)))
    { f_rbclear(f_lchild(p));
      f_rbclear(f_rchild(p));
     }
  delete p;
 }

void pers_rb_tree::c_rbclear(C_pers_tree_node* p)
{ if (!(c_isleaf(p)))
    { c_rbclear(c_lchild(p));
      c_rbclear(c_rchild(p));
     }
  delete p;
}


F_pers_tree_node* pers_rb_tree::f_rbsearch(F_pers_tree_node* p, Version i) const
{
  F_pers_tree_node *q;
  
  q = f_rchild(p);
  while (!(f_isleaf(q)))
  {  
    if (i == q->ver_stamp || vless(i, q->ver_stamp))
      q = f_lchild(q);
    else
      q = f_rchild(q);
  }
  return(q);
}

C_pers_tree_node* pers_rb_tree::c_rbsearch(C_pers_tree_node* p, Version i) const
{
  C_pers_tree_node *q;
  
  q = c_rchild(p);
  while (!(c_isleaf(q)))
  {
    if (i == q->vers_stamp || vless(i, q->vers_stamp))
      q = c_lchild(q);
    else
      q = c_rchild(q);
  }
  return(q);
}

F_pers_tree_node * pers_rb_tree::f_nleaf(pers_tree_node* newvalue, Version i)
{
  F_pers_tree_node* result = new F_pers_tree_node;

  f_mblack(result);
  result->ver_stamp = i;
  f_lchild(result) = f_rchild(result) = result;
  result->poin_value = newvalue;
  return(result);
}

C_pers_tree_node * pers_rb_tree::c_nleaf(int newvalue, Version i)
{
  C_pers_tree_node* result = new C_pers_tree_node;

  c_mblack(result);
  result->vers_stamp = i;
  c_lchild(result) = c_rchild(result) = result;
  result->col_value = newvalue;
  return(result);
}

F_pers_tree_node * pers_rb_tree::f_nnode(F_pers_tree_node* c1, F_pers_tree_node* c2)
{
  F_pers_tree_node* result = new F_pers_tree_node;

  f_mred(result);
  if (vless(c1->ver_stamp, c2->ver_stamp))
  {
    f_lchild(result) = c1;
    c1->f_right = 0;
    f_rchild(result) = c2;
    c2->f_right = 1;
  }
  else
  {
    f_lchild(result) = c2;
    c2->f_right = 0;
    f_rchild(result) = c1;
    c1->f_right = 1;
  }
  f_parent(c1) = f_parent(c2) = result;
  result->ver_stamp = (f_lchild(result))->ver_stamp;
  result->poin_value = NULL;
  return(result);
}

C_pers_tree_node* pers_rb_tree::c_nnode(C_pers_tree_node* c1, C_pers_tree_node* c2)
{
  C_pers_tree_node* result = new C_pers_tree_node;

  c_mred(result);
  if (vless(c1->vers_stamp, c2->vers_stamp))
  {
    c_lchild(result) = c1;
    c1->c_right = 0;
    c_rchild(result) = c2;
    c2->c_right = 1;
  }
  else
  {
    c_lchild(result) = c2;
    c2->c_right = 0;
    c_rchild(result) = c1;
    c1->c_right = 1;
  }
  c_parent(c1) = c_parent(c2) = result;
  result->vers_stamp = (c_lchild(result))->vers_stamp;
  result->col_value = 0;  
  return(result);
}

void  pers_rb_tree::f_single_rot(F_pers_tree_node* top_node, int dir)
{
  F_pers_tree_node *temp, *q;

  q = top_node->f_link[1-dir];
  top_node->f_link[1-dir] = temp = q->f_link[dir];
  temp->f_right = 1 - dir;
  f_parent(temp) = top_node;
  q->f_link[dir] = top_node;
  temp = f_parent(top_node);
  f_parent(top_node) = q;
  q->f_right = top_node->f_right;
  temp->f_link[top_node->f_right] = q;
  f_parent(q) = temp;
  top_node->f_right = dir;
  return;
}

void  pers_rb_tree::c_single_rot(C_pers_tree_node* top_node, int dir)
{
  C_pers_tree_node *temp, *q;

  q = top_node->c_link[1-dir];
  top_node->c_link[1-dir] = temp = q->c_link[dir];
  temp->c_right = 1 - dir;
  c_parent(temp) = top_node;
  q->c_link[dir] = top_node;
  temp = c_parent(top_node);
  c_parent(top_node) = q;
  q->c_right = top_node->c_right;
  temp->c_link[top_node->c_right] = q;
  c_parent(q) = temp;
  top_node->c_right = dir;
  return;
}


void  pers_rb_tree::f_double_rot(F_pers_tree_node* top_node, int dir)
{
  F_pers_tree_node *q, *r, *temp;

  q = top_node->f_link[1-dir];
  r = q->f_link[dir];
  top_node->f_link[1-dir] = temp = r->f_link[dir];
  temp->f_right = 1 - dir;
  f_parent(temp) = top_node;
  q->f_link[dir] = (temp = r->f_link[1-dir]);
  temp->f_right = dir;
  f_parent(temp) = q;
  temp = f_parent(top_node);
  r->f_right = top_node->f_right;
  r->f_link[dir] = top_node;
  f_parent(top_node) = r;
  top_node->f_right = dir;
  r->f_link[1-dir] = q;
  f_parent(q) = r;
  temp->f_link[r->f_right] = r;
  f_parent(r) = temp;
  return;
}

void  pers_rb_tree::c_double_rot(C_pers_tree_node* top_node, int dir)
{
  C_pers_tree_node *q, *r, *temp;

  q = top_node->c_link[1-dir];
  r = q->c_link[dir];
  top_node->c_link[1-dir] = temp = r->c_link[dir];
  temp->c_right = 1 - dir;
  c_parent(temp) = top_node;
  q->c_link[dir] = (temp = r->c_link[1-dir]);
  temp->c_right = dir;
  c_parent(temp) = q;
  temp = c_parent(top_node);
  r->c_right = top_node->c_right;
  r->c_link[dir] = top_node;
  c_parent(top_node) = r;
  top_node->c_right = dir;
  r->c_link[1-dir] = q;
  c_parent(q) = r;
  temp->c_link[r->c_right] = r;
  c_parent(r) = temp;
  return;
}


int pers_rb_tree::f_insert(F_pers_tree_node* head, pers_tree_node* newvalue, Version i)
{
  int aux_bool;
  F_pers_tree_node *p, *q, *r, *aux_node;

  if (f_rchild(head) == NULL)
  {
    p = f_nleaf(newvalue, i);
    p->f_right = 1;
    f_parent(p) = head;
    f_rchild(head) = p;
    return(0);
  }
  p = f_rbsearch(head, i);
  if (p->ver_stamp == i)
  {
    p->poin_value = newvalue;
    return(1);
  }
  q = f_nleaf(newvalue, i);
  aux_bool = p->f_right;
  aux_node = f_parent(p);
  r = f_nnode(p, q);
  f_parent(r) = aux_node;
  r->f_right = aux_bool;
  aux_node->f_link[aux_bool] = r;
  if (!f_isred(aux_node))
  {
    f_mblack(f_rchild(head));
    return(0);
  }
  q = aux_node;
  p = f_parent(q);
  while ((f_isred(f_lchild(p))) && (f_isred(f_rchild(p))))
  {
    f_mblack(f_lchild(p));
    f_mblack(f_rchild(p));
    f_mred(p);
    r = p;
    q = f_parent(r);
    if (!f_isred(q))
    {
      f_mblack(f_rchild(head));
      return(0);
    }
    p = f_parent(q);
  }
  if (q->f_right == r->f_right)
  {
    f_mred(p);
    f_mblack(q);
    f_single_rot(p, 1-r->f_right);
  }
  else
  {
    f_mblack(r);
    f_mred(p);
    f_double_rot(p, r->f_right);
  }
  f_mblack(f_rchild(head));
  return(0);
}

int pers_rb_tree::c_insert(C_pers_tree_node* head, int newvalue, Version i)
{
  int aux_bool;
  C_pers_tree_node *p, *q, *r, *aux_node;

  if (c_rchild(head) == NULL)
  {
    p = c_nleaf(newvalue, i);
    p->c_right = 1;
    c_parent(p) = head;
    c_rchild(head) = p;
    return(0);
  }
  p = c_rbsearch(head, i);
  if (p->vers_stamp == i)
  {
    p->col_value = newvalue;
    return(1);
  }
  q = c_nleaf(newvalue, i);
  aux_bool = p->c_right;
  aux_node = c_parent(p);
  r = c_nnode(p, q);
  c_parent(r) = aux_node;
  r->c_right = aux_bool;
  aux_node->c_link[aux_bool] = r;
  if (!c_isred(aux_node))
  {
    c_mblack(c_rchild(head));
    return(0);
  }
  q = aux_node;
  p = c_parent(q);
  while ((c_isred(c_lchild(p))) && (c_isred(c_rchild(p))))
  {
    c_mblack(c_lchild(p));
    c_mblack(c_rchild(p));
    c_mred(p);
    r = p;
    q = c_parent(r);
    if (!c_isred(q))
    {
      c_mblack(c_rchild(head));
      return(0);
    }
    p = c_parent(q);
  }
  if (q->c_right == r->c_right)
  {
    c_mred(p);
    c_mblack(q);
    c_single_rot(p, 1-r->c_right);
  }
  else
  {
    c_mblack(r);
    c_mred(p);
    c_double_rot(p, r->c_right);
  }
  c_mblack(c_rchild(head));
  return(0);
}



/* insert the new version in the appropriate position and update the 
 * version list
 */

void pers_rb_tree::del_version(Version) 
{ if (--v_list->count ==0) del_tree(); }

Version pers_rb_tree::copy_version(Version i) { return new_version(i); }

Version pers_rb_tree::new_version(Version i)
{
  Version succ = v_list->vl.succ(i);
  
  ver_node p = new VER_pers_tree_node;

  if (succ != nil)
     p->ser_num = (v_list->vl[i]->ser_num + v_list->vl[succ]->ser_num) / 2.0;
  else 
     p->ser_num = v_list->vl[i]->ser_num + 1000;


  p->popul = v_list->vl[i]->popul;
  p->acc_pointer = v_list->vl[i]->acc_pointer;
  v_list->count++;
  return v_list->vl.insert(p,i);
}

/* implementation of the update step (change of left or right child)
 * for a specific persistent node and update operation 
 */
 
void pers_rb_tree::update(F_pers_tree_node* p, pers_tree_node* newvalue, Version i)
{ 
  F_pers_tree_node *p1, *i1, *i2;

  Version ip = v_list->vl.succ(i);

  if (f_insert(p, newvalue, i) == 1) return;

  if (ip == nil || vless(ip,p->ver_stamp)) return;

  p1 = f_rbsearch(p, i);
  if (p1->f_right)
  {
    i1 = f_lchild(f_parent(p1));
    if (f_isleaf(i1))
      goto la;
    else
      i1 = f_rchild(i1);
la: while (p1 != f_rchild(p) && p1->f_right)
      p1 = f_parent(p1);
    if (p1 == f_rchild(p))
      i2 = NULL;
    else
    {
      i2 = f_rchild(f_parent(p1));
      while (!f_isleaf(i2))
        i2 = f_lchild(i2);
    }
  }
  else
  {
    i2 = f_rchild(f_parent(p1));
    if (f_isleaf(i2))
      goto m;
    else
      i2 = f_lchild(i2);
 m: while (p1->f_right == 0)
      p1 = f_parent(p1);
    i1 = f_lchild(f_parent(p1));
    while (!f_isleaf(i1))
      i1 = f_rchild(i1);
  }
  if (i2 == NULL || vless(ip, i2->ver_stamp))
     f_insert(p, i1->poin_value, ip);
}
   

/* implementation of the update step (change of color) for a specific
 * persistent node and update operation 
 */
 
void pers_rb_tree::up_col(C_pers_tree_node* p, int newvalue, Version i)
{ 
  C_pers_tree_node *p1, *i1, *i2;
  
  Version ip = v_list->vl.succ(i);

  if (c_insert(p, newvalue, i) == 1) return;

  if (ip ==nil  || vless(ip,p->vers_stamp)) return;

  p1 = c_rbsearch(p, i);
  if (p1->c_right)
  {
    i1 = c_lchild(c_parent(p1));
    if (c_isleaf(i1))
      goto lb;
    else
      i1 = c_rchild(i1);
lb: while (p1 != c_rchild(p) && p1->c_right)
      p1 = c_parent(p1);
    if (p1 == c_rchild(p))
      i2 = NULL;
    else
    {
      i2 = c_rchild(c_parent(p1));
      while (!c_isleaf(i2))
        i2 = c_lchild(i2);
    }
  }
  else
  {
    i2 = c_rchild(c_parent(p1));
    if (c_isleaf(i2))
      goto ma;
    else
      i2 = c_lchild(i2);
ma: while (p1->c_right == 0)
      p1 = c_parent(p1);
    i1 = c_lchild(c_parent(p1));
    while (!c_isleaf(i1))
      i1 = c_rchild(i1);
  }
  if (i2 == NULL || vless(ip, i2->vers_stamp))
     c_insert(p, i1->col_value, ip);
}
   

/* implementation of the access step for a specific persistent node 
 * and version 
 */

pers_tree_node * pers_rb_tree::acc_step(F_pers_tree_node *head, Version i) const
{
  F_pers_tree_node *q, *i1;
 
  q = f_rbsearch(head, i);
  if (q->ver_stamp == i)
    return(q->poin_value);
  if (vless(q->ver_stamp, i))
    return(q->poin_value);
  if (q->f_right)
  {
     i1 = f_lchild(f_parent(q));
     if (f_isleaf(i1))
       goto t;
     else
       i1 = f_rchild(i1);
  }
  else
  {
    while (q->f_right == 0)
      q = f_parent(q);
    i1 = f_lchild(f_parent(q));
    while (!f_isleaf(i1))
      i1 = f_rchild(i1);
  }
t:return(i1->poin_value);
}
  


/* find out whether a given persistent node is red or not in a 
 * specific version 
 */

int pers_rb_tree::isred(pers_tree_node* p, Version i) const
{
  C_pers_tree_node *head, *q, *i1;
  
  if (isleaf(p))
    return(0);
  head = p->red;
  q = c_rbsearch(head, i);
  if (q->vers_stamp == i)
    return(q->col_value);
  if (vless(q->vers_stamp, i))
    return(q->col_value);
  if (q->c_right)
  {  
     i1 = c_lchild(c_parent(q));
     if (c_isleaf(i1))
       goto s;
     else
       i1 = c_rchild(i1);
  }
  else
  {
    while (q->c_right == 0)
      q = c_parent(q);
    i1 = c_lchild(c_parent(q));
    while (!c_isleaf(i1))
      i1 = c_rchild(i1);
  }
s:return(i1->col_value);
}

 
/* create a new leaf and initialize the fields with the appropriate values */
 
pers_tree_node* pers_rb_tree::newleaf(void* val, void* inf,pers_tree_node* pred,
                                                           pers_tree_node* succ,
                                                           Version i)
{
  pers_tree_node *result;
  F_pers_tree_node *res1, *res2;
  C_pers_tree_node *res3;
  
  result = new pers_tree_node;
  res1   = new F_pers_tree_node;
  res2   = new F_pers_tree_node;
  res3   = new C_pers_tree_node;
   
    result->key = val;
    result->inf = inf;
    result->parent = NULL;
    result->right = 1;
    result->is_leaf = 1;
    result->copy = NULL;

    result->next = v_list->used;
    v_list->used = result;

    res1->ver_stamp = res2->ver_stamp = res3->vers_stamp = i;

    f_lchild(res1) = f_rchild(res1) = f_parent(res1) = NULL;
    f_lchild(res2) = f_rchild(res2) = f_parent(res2) = NULL;
    c_lchild(res3) = c_rchild(res3) = c_parent(res3) = NULL;

    res1->f_right = res2->f_right = res3->c_right = 1;
    res1->f_color = res2->f_color = res3->c_color = 0;

    res1->poin_value = res2->poin_value = NULL; 
    res3->col_value = 0;  

    f_insert(res1, pred, i);
    f_insert(res2, succ, i);
    c_insert(res3, 0, i);

    result->link[0] = res1;
    result->link[1] = res2;
    result->red     = res3;

    return result;
}

/* create a new persistent node and initialize its fields with the
 * appropriate values 
 */
 
pers_tree_node* pers_rb_tree::newnode(pers_tree_node* c1, pers_tree_node* c2, Version i)
{
  // c1 and c2 are leaves

  pers_tree_node *result;
  F_pers_tree_node *res1, *res2,  *res4;   // s.n. : res4 pointer to leaf (copy)
  C_pers_tree_node *res3;
  
  result = new pers_tree_node;
  res1   = new F_pers_tree_node;
  res2   = new F_pers_tree_node;
  res3   = new C_pers_tree_node;
  res4   = new F_pers_tree_node;

    result->parent = NULL;

    result->next = v_list->used;
    v_list->used = result;

    res1->ver_stamp = res2->ver_stamp = res3->vers_stamp = i;

    f_lchild(res1) = f_rchild(res1) = f_parent(res1) = NULL;
    f_lchild(res2) = f_rchild(res2) = f_parent(res2) = NULL;
    f_lchild(res4) = f_rchild(res4) = f_parent(res4) = NULL;
    c_lchild(res3) = c_rchild(res3) = c_parent(res3) = NULL;

    res1->f_right = res2->f_right = res3->c_right = res4->f_right = 1;
    res1->f_color = res2->f_color = res3->c_color = res4->f_color = 0;

    res1->poin_value = res2->poin_value = res4->poin_value = NULL; 
    res3->col_value = 1;  

    c_insert(res3, 1, i);

    if (cmp_keys(c1->key, c2->key) < 1)
    {
      f_insert(res1, c1, i);
      f_insert(res2, c2, i);
      f_insert(res4, c1, i);
      result->key = c1->key;
    }
    else
    {
      f_insert(res1, c2, i);
      f_insert(res2, c1, i);
      f_insert(res4, c2, i);
      result->key = c2->key;
    }

    result->link[0] = res1;
    result->link[1] = res2;
    result->red     = res3;
    result->copy    = res4;
    result->is_leaf = 0;

    return(result);
}

  
/* implementation of the single rotation for the fully persistent
 * red-black tree
 */

pers_tree_node* pers_rb_tree::single_rot(pers_tree_node* top_node, int dir, Version i)
{
  pers_tree_node *temp, *q, *newroot;
  
  newroot = NULL;
  q = child(1 - dir, top_node, i);
  temp = child(dir, q, i);
  update(top_node->link[1 - dir], temp, i);
  update(q->link[dir], top_node, i);
  if ((temp = top_node->parent) == NULL)
    newroot = q;
  top_node->parent = q;
  if (temp != NULL)
    update(temp->link[top_node->right], q, i);
  top_node->right = dir;
  return(newroot);
}

/* implementation of the double rotation for the fully persistent
 * red-black tree
 */

pers_tree_node* pers_rb_tree::double_rot(pers_tree_node* top_node, int dir, Version i)
{
  pers_tree_node *q, *r, *temp, *newroot;
  
  newroot = NULL;
  q = child(1 - dir, top_node, i);
  r = child(dir, q, i);
  temp = child(dir, r, i);
  update(top_node->link[1 - dir], temp, i);
  temp = child(1 - dir, r, i);
  update(q->link[dir], temp, i);
  if ((temp = top_node->parent) == NULL)
    newroot = r;
  update(r->link[dir], top_node, i);
  update(r->link[1 - dir], q, i);
  if (temp != NULL)
    update(temp->link[top_node->right], r, i);
  return(newroot);
}

/* the root is colored black after each  update operation */

void pers_rb_tree::m_b_root(Version i)
{
  pers_tree_node *p;

  p = v_list->vl[i]->acc_pointer;

  if (p != NULL && !isleaf(p) && isred(p, i))
     up_col(p->red, 0, i);
}




//------------------------------------------------------------------------------
// member functions
//------------------------------------------------------------------------------


void pers_rb_tree::init_tree()
{
   /* create dummy (empty) version 0 */
   ver_node v = new VER_pers_tree_node;
   v->ser_num =  0;
   v->popul   =  0;
   v->acc_pointer = NULL;
   v_list = new V_LIST;
   v_list->vl.append(v);
   v_list->count = 1;

   v_list->used = NULL;

}

pers_tree_node* pers_rb_tree::search(void *val, pers_tree_node*& copy, Version i) const
{
  pers_tree_node *p, *q;

  copy = NULL;
  
  if ((p = v_list->vl[i]->acc_pointer) == NULL)
    return(NULL);

  p->parent = NULL;
  p->right = 1;
  while (!isleaf(p))
  {
    int v = cmp_keys(val, get_key(p,i));

    if (v < 1)
    { if (v==0) copy = p;
      q = p;
      p = child(0, p, i);
      p->parent = q;
      p->right = 0;
     }
    else
    { q = p;
      p = child(1, p, i);
      p->parent = q;
      p->right = 1;
     }
   }

   return p;
}


 
Version  pers_rb_tree::del(void *val, Version i1)
{
  pers_tree_node *pos_del, *par1, *par2, *root, *newroot, *temp, *copy;

  Version i  = new_version(i1);
  
  if ((pos_del = search(val, copy, i1)) == NULL) return i; //empty tree

  if (cmp_keys(pos_del->key, val) != 0) return i;  // key not found


  if ((--(v_list->vl[i]->popul)) == 0)
  {
    v_list->vl[i]->acc_pointer = NULL;
    return i;
  }


  //update links to neighbor leaves

  pers_tree_node* pred = child(0,pos_del,i);
  pers_tree_node* succ = child(1,pos_del,i);

  if (pred) update(pred->link[1],succ,i);
  if (succ) update(succ->link[0],pred,i);


  root = v_list->vl[i]->acc_pointer;

  if ((par1 = pos_del->parent) == root)
  {
    v_list->vl[i]->acc_pointer = sibling(pos_del, i);
    goto end;
   }

  par2 = par1->parent;
  pos_del = sibling(pos_del, i);
  update(par2->link[par1->right], pos_del, i);
  pos_del->parent = par2;
  pos_del->right = par1->right;

  if (copy != NULL && copy != par1)  // we have to overwrite copy  by pred
  {
    update(copy->copy, pred, i);

   }
  

  if (isred(par1, i))
    goto end;
  if (isred(pos_del, i))
  {
    up_col(pos_del->red, 0, i);
    goto end;
  }
  par1 = sibling(pos_del, i);
  while ((!isred(par1, i)) && (!isred(child(0, par1, i), i)) &&
         (!isred(child(1, par1, i), i)))
  {
    up_col(par1->red, 1, i);
    pos_del = pos_del->parent;
    if (isred(pos_del, i))
    {
      up_col(pos_del->red, 0, i);
      goto end;
    }
    if (pos_del == root)
      goto end;
    else
      par1 = sibling(pos_del, i);
  }
  par2 = pos_del->parent;
  if (isred(par1, i))
  {
    up_col(par2->red, 1, i);
    up_col(par1->red, 0,i);
    if ((newroot = single_rot(par2, pos_del->right, i)) != NULL)
      v_list->vl[i]->acc_pointer = newroot;
    par1 = sibling(pos_del,i);
    if ((!isred(child(0, par1, i), i)) && (!isred(child(1, par1, i), i)))
    {
      up_col(par1->red, 1, i);
      up_col((pos_del->parent)->red, 0, i);
      goto end;
    }
  }
  par2 = pos_del->parent;
  if (!pos_del->right)
    if (isred(child(0, par1, i), i))
    {
      temp = child(0, par1, i);
      up_col(temp->red, isred(par2, i), i);
      up_col(par2->red, 0, i);
      newroot = double_rot(par2, LEFT, i);
    }
    else
    {
      up_col(par1->red, isred(par2, i), i);
      up_col(par2->red, 0, i);
      temp = child(1, par1, i);
      up_col(temp->red, 0, i);
      newroot = single_rot(par2, LEFT, i);
    }
  else
    if (isred(child(0, par1, i), i))
    {
      up_col(par1->red, isred(par2, i), i);
      up_col(par2->red, 0, i);
      temp = child(0, par1, i);
      up_col(temp->red, 0, i);
      newroot = single_rot(par2, RIGHT, i);
    }
    else
    {
      temp = child(1, par1, i);
      up_col(temp->red, isred(par2, i), i);
      up_col(par2->red, 0, i);
      newroot = double_rot(par2, RIGHT, i);
    }
  if (newroot != NULL)
    v_list->vl[i]->acc_pointer = newroot;

end:
  m_b_root(i);
  return i;
}


Version pers_rb_tree::insert(void *val, void* inf, Version i1)
{
  int aux_bool; 
  pers_tree_node *p, *q, *r, *aux_node, *root, *newroot, *temp;

  Version i = new_version(i1);

  p = search(val, i1);

  if (p && cmp_keys(p->key, val) == 0) 
  { // key already there change inf
    return change_inf(p,inf,i1);
   }

  copy_key(val);
  copy_inf(inf);
  
  if (p == NULL)   // empty tree
  { p = newleaf(val, inf, nil, nil, i);
    v_list->vl[i]->acc_pointer = p;
    v_list->vl[i]->popul = 1;
    goto end;
   }

  (v_list->vl[i]->popul)++;

  if (cmp_keys(val, p->key) > 0)   // new rightmost leaf 
    { q = newleaf(val,inf, p, nil, i);   
      update(p->link[1], q, i);
     }
  else // new  leaf before  p 
    { pers_tree_node * pred = child(0,p,i);
      q = newleaf(val,inf, pred, p, i);   
      update(p->link[0], q, i);
      if (pred) update(pred->link[1], q, i);
     }

  aux_bool = p->right;
  r = newnode(p, q, i);

  if (p->parent == NULL)
  {
    v_list->vl[i]->acc_pointer = r;
    goto end;
  }
  aux_node = p->parent;
  r->right = aux_bool;
  update(aux_node->link[aux_bool], r, i);

  if (!isred(aux_node, i))
     goto end;

  q = aux_node;
  p = q->parent;
  root = v_list->vl[i]->acc_pointer;
  while ((isred(child(0, p, i), i)) && (isred(child(1, p, i), i)))
  {
    temp = child(0, p, i);
    up_col(temp->red, 0, i);
    temp = child(1, p, i);
    up_col(temp->red, 0, i);
    up_col(p->red, 1, i);
    if (p == root)
      goto end;
    r = p;
    q = r->parent;
    if (!isred(q, i))
      goto end;
    p = q->parent;
  }
  if (q->right == r->right)
  {
    up_col(p->red, 1, i);
    up_col(q->red, 0, i);
    newroot = single_rot(p, 1 - r->right, i);
  }
  else
  {
    up_col(r->red, 0, i);
    up_col(p->red, 1, i);
    newroot = double_rot(p, r->right, i);
  }

  if (newroot != NULL)
    v_list->vl[i]->acc_pointer = newroot;

end:
  m_b_root(i);
  return i;
}


Version pers_rb_tree::change_inf(pers_tree_node* p, void* inf, Version i1)
{
  Version i = new_version(i1);

  p = search(p->key, i1);  // setting parent pointers

  pers_tree_node* pred = child(0,p,i);
  pers_tree_node* succ = child(1,p,i);

  copy_inf(inf);
  pers_tree_node* q = newleaf(p->key,inf, pred, succ, i);   


  if (pred) update(pred->link[1],q,i);
  if (succ) update(succ->link[0],q,i);

  q->right = p->right;
  update(p->parent->link[q->right], q, i);

  return i;

}



pers_tree_node* pers_rb_tree::min(Version v) const
{ pers_tree_node* r = v_list->vl[v]->acc_pointer;
  if (r == nil) return nil;
  while ( ! isleaf(r)) r = child(0,r,v);
  return r;
}  

pers_tree_node* pers_rb_tree::max(Version v) const
{ pers_tree_node* r = v_list->vl[v]->acc_pointer;
  if (r == nil) return nil;
  while ( ! isleaf(r)) r = child(1,r,v);
  return r;
}  

void pers_rb_tree::print(pers_tree_node *p, Version v) const
{ if (p)
  { if (isleaf(p))  
    { print_key(p->key);
      cout << " ";
     }
    else
     { print(child(0, p, v), v);
       print(child(1, p, v), v);
      }
   }
}  

void pers_rb_tree::del_tree()
{ 
  while (v_list->used)
  { pers_tree_node* p = v_list->used;
    v_list->used = v_list->used->next;

    f_rbclear(f_rchild(p->link[0]));
    delete p->link[0];

    f_rbclear(f_rchild(p->link[1]));
    delete p->link[1];

    if (p->copy != nil)
    {
      f_rbclear(f_rchild(p->copy));
      delete p->copy;
     }

    c_rbclear(c_rchild(p->red));
    delete p->red;

    if (isleaf(p))
    { clear_key(p->key);
      clear_inf(p->inf);
     }

    delete p;
   }

   ver_node q;
   forall(q, v_list->vl) delete q;

   delete v_list;
 }  


pers_tree_node*  pers_rb_tree::lookup(void *val,Version v) const
{ pers_tree_node* p = search(val,v);
  return (p && cmp_keys(key(p), val) == 0) ? p : nil;
 }

pers_tree_node* pers_rb_tree::locate(void *val,Version v) const
{ pers_tree_node* p = search(val,v);
  return ( p && cmp_keys(key(p), val) >= 0) ?  p : nil;
 }

pers_tree_node* pers_rb_tree::locate_pred(void *val,Version v) const
{ pers_tree_node* p = search(val,v);
  if (p==0) return nil;
  if (cmp_keys(key(p), val) <= 0)  return p;
  return child(0,p,v);

}




void pers_rb_tree::draw(DRAW_NODE_FCT draw_node,
                        DRAW_EDGE_FCT draw_edge, 
                        Version v, pers_tree_node* r, 
                        double x1, double x2, double y, 
                        double ydist, double last_x) const
{ 

  double x = (x1+x2)/2;

  if (r==NULL) return;

  if (last_x != 0) draw_edge(last_x,y+ydist,x,y);

  if (isleaf(r)) 
     draw_node(x,y,r->key);
  else
    { draw_node(x,y,get_key(r,v));
      draw(draw_node,draw_edge,v,child(0,r,v),x1,x,y-ydist,ydist,x);
      draw(draw_node,draw_edge,v,child(1,r,v),x,x2,y-ydist,ydist,x);
     }
}

void pers_rb_tree::draw(DRAW_NODE_FCT draw_node,
                        DRAW_EDGE_FCT draw_edge, 
                        Version v, double x1, double x2, 
                                   double y, double ydist) const
{ draw(draw_node,draw_edge,v,v_list->vl[v]->acc_pointer,x1,x2,y,ydist,0); }

LEDA_END_NAMESPACE
