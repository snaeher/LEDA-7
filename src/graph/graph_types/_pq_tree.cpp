/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _pq_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------
// PQ_TREES
//
// R. Hesse, E. Kalliwoda, D. Ambras (1996/97)
//
//------------------------------------------------------------------------

#include <LEDA/graph/pq_tree.h>
#include <LEDA/core/queue.h>

LEDA_BEGIN_NAMESPACE

enum {
  D_NODE_INVALID  = 0,
  D_NODE_PNODE    = 1,
  D_NODE_QNODE    = 2,
  D_NODE_LEAF     = 3,
  D_NODE_DIR      = 4,

  D_NODE_UNMARKED = 6,
  D_NODE_UNBLOCKED= 7,
  D_NODE_BLOCKED  = 8,
  D_NODE_QUEUED   = 9,

  D_NODE_EMPTY    = 10,
  D_NODE_FULL     = 11,
  D_NODE_PARTIAL  = 12,
  D_NODE_DOUBLE_PARTIAL  =13 };


pq_node_struct::pq_node_struct()
{
  leaf_index  = 0;
  child_count = 0;

  parent          = NULL;
  link_one_side   = NULL;
  link_other_side = NULL;
  right_most      = NULL;
  left_most       = NULL;

  type          = D_NODE_PNODE;
  parent_type   = D_NODE_QNODE;

  node_reset();
}



void pq_node_struct::node_reset()
{
  mark   = D_NODE_UNMARKED;
  status = D_NODE_EMPTY;        // vielleicht auf FULL umstellen ***

  pert_leaf_count  = 0;
  pert_child_count = 0;
  full_child_count = 0;

  part_child1 = NULL;
  part_child2 = NULL;
}




pq_tree::pq_tree(int lsize)
{ 
  successful            = true;
  too_many_part         = false;
  leaves_size           = lsize;
  blocked_chain_count   = 0;
  blocked_nodes_count   = 0;

  root                  = NULL;
  pseudo_root           = new pq_node_struct;
  pseudo_root->leaf_index = 2;

  if (lsize > 0)  leaves_init(lsize+1);
    else  leaves = NULL;
}


//------------------------------------------------------------------------
// TEMPLATES
//------------------------------------------------------------------------


void pq_tree::fix_part_child_direction(pq_node x, bool part_child2_too)
{
 pq_node       k;
 pq_node       child1= x->part_child1;
 pq_node       child2= x->part_child2;

 if (child1->left_most->status == D_NODE_FULL)
 { k= child1->left_most;
   child1->left_most  = child1->right_most;
   child1->right_most = k;
 } 

 if (part_child2_too)
   if (child2->right_most->status == D_NODE_FULL)
   {  k= child2->left_most;
      child2->left_most  = child2->right_most;
      child2->right_most = k;
   }
}




//inline 
void pq_tree::append_as_right_child(pq_node parent, pq_node child)
{
  pq_node  l= parent->right_most;

  if (l->link_one_side)  
      l->link_other_side = child;
  else  
      l->link_one_side = child;

  child->link_one_side  = l;
  child->link_other_side= NULL;

  parent->right_most = child;
}




pq_node pq_tree::hang_down_full_children(pq_node x, pq_node z)
{
  int       x_full_child_count= x->full_child_count;
  pq_node  y;


    if (x_full_child_count == 0)  return NULL;

    if (x_full_child_count == 1)
    {
       y= remove_from_siblings(x, x->right_most);
       y->parent = z;  
       y->parent_type = z->type;
       append_as_right_child(z, y);

       z->child_count++;
       z->full_child_count++;
       z->pert_leaf_count = x->pert_leaf_count;
       x->full_child_count = 0;
       x->child_count--;

       return y;
    } 



  pq_node  l;

    y = new pq_node_struct;
    y->parent = z;  
    y->parent_type = z->type;
    y->status = D_NODE_FULL;
    processed.push(y);

    y->left_most = x->right_most;
    y->right_most= x->right_most;

    y->left_most->parent = y;
    l = NULL;
    for(int i = 1; i < x_full_child_count; i++)
    { go_to_sibling(y->left_most, l);
      y->left_most->parent = y;
    }



    pq_node  k, sibling;
    pq_node  y_left= y->left_most;

    if (y_left->link_one_side == l)  
    {  k= y_left->link_other_side;
          y_left->link_other_side = NULL;
    }
    else  
    {  k= y_left->link_one_side;
          y_left->link_one_side = NULL;
    }

    if (x != z)
    { x->right_most = k;
      append_as_right_child(z, y);
      sibling= NULL;
    }
    else
    { x->right_most = y;
      y->link_one_side = k;
      sibling= y;
    }

    if (k->link_one_side == y_left)  
      k->link_one_side = sibling;
    else 
      k->link_other_side = sibling;



    y->child_count      = y->full_child_count = x->full_child_count;

    x->full_child_count = 0;
    x->child_count     -= y->child_count;

    y->pert_leaf_count  = x->pert_leaf_count;
    if (x->part_child1)
      y->pert_leaf_count -= x->part_child1->pert_leaf_count;
    if (x->part_child2)
      y->pert_leaf_count -= x->part_child2->pert_leaf_count;

    z->child_count++;
    z->full_child_count++;
    z->pert_leaf_count += y->pert_leaf_count;

  return y;
}





pq_node pq_tree::remove_from_siblings(pq_node parent, pq_node child)
{
  if (child->link_one_side)
  { if (child->link_one_side->link_one_side == child)  
        child->link_one_side->link_one_side =  child->link_other_side; 
    else  
        child->link_one_side->link_other_side= child->link_other_side;
   }

  if (child->link_other_side)
  { if (child->link_other_side->link_one_side == child)
        child->link_other_side->link_one_side =  child->link_one_side;
    else 
        child->link_other_side->link_other_side= child->link_one_side;
   }


  if (parent->right_most == child)
    parent->right_most = child->link_one_side ?
                      child->link_one_side : child->link_other_side;

  if (parent->left_most == child)
    parent->left_most = child->link_one_side ?
                     child->link_one_side : child->link_other_side;


  if (parent->part_child1 == child)
  { parent->part_child1= parent->part_child2;
    parent->part_child2= NULL;
  }
  if (parent->part_child2 == child)  parent->part_child2= NULL;

 return child;
}



void pq_tree::replace_in_siblings(pq_node x, pq_node z)
{
  z->parent = x->parent; 
  z->parent_type = x->parent_type;

  z->link_one_side = x->link_one_side;
  z->link_other_side = x->link_other_side;

  if (z->link_one_side)  
  { if (z->link_one_side->link_one_side == x) 
        z->link_one_side->link_one_side = z; 
    else  
        z->link_one_side->link_other_side = z;
   }

  if (z->link_other_side)  
  { if (z->link_other_side->link_one_side == x)  
        z->link_other_side->link_one_side = z; 
    else  
        z->link_other_side->link_other_side = z;
   }

  if (z->parent)
  { if (z->parent->right_most == x)  z->parent->right_most = z;
    if (z->parent->left_most  == x)  z->parent->left_most  = z;
  }
}




bool pq_tree::set_as_part_child(pq_node x)
{
   x->status = D_NODE_PARTIAL;

   if (x->parent->part_child1)
   { if (x->parent->part_child2)
     { too_many_part = true;  
       return false;
     }
     else  x->parent->part_child2 = x;
   }
   else  x->parent->part_child1 = x;

 return true;
}




//inline 
void pq_tree::append_as_left_child(pq_node parent, pq_node child)
{
  pq_node  l= parent->left_most;

  child->parent = parent;
  child->parent_type = D_NODE_QNODE;            // ist immer so

  if (l->link_one_side)  
      l->link_other_side = child;
  else  
      l->link_one_side = child;

  child->link_one_side  = NULL;
  child->link_other_side= l;

  parent->left_most = child;
}



void pq_tree::delete_part_node_parent(pq_node x)
{
  replace_in_siblings(x, x->part_child1);

  if (x == root)
  { root= x->part_child1;
    root->parent = NULL;
    root->link_one_side = NULL;
    root->link_other_side = NULL;
  }

  delete x;
}





bool pq_tree::template_PQL1(pq_node x, bool is_pseudo_root)
{

#ifdef _DEBUG_PQ_TREE
  if (x->type == D_NODE_PNODE)  cout << "P1";
    else
    if (x->type == D_NODE_QNODE)  cout << "Q1";
      else  cout << "L1";
#endif


  if (x->type == D_NODE_QNODE)
  { pq_node  k;
    pq_node  last_k= NULL;

    if (x->part_child1) return false;
    if (x == pseudo_root) return true;

    k = x->right_most;
    while (k)
    { if (k->type != D_NODE_DIR && k->status == D_NODE_EMPTY)
        return false;
      go_to_sibling(k, last_k);
    }
  }
  else
    if (x->full_child_count != x->child_count) return false;



  if (x->parent_type == D_NODE_PNODE &&  x->parent &&
      x->parent->right_most != x )
  {
    remove_from_siblings(x->parent, x);
    append_as_right_child(x->parent, x);
  }

  x->status = D_NODE_FULL;
  processed.push(x);

  if (is_pseudo_root) 
  {
    pseudo_root->left_most  = x;
    pseudo_root->right_most = x;
  }
  else
    if (x->parent)  x->parent->full_child_count++;

  return true;
}





bool pq_tree::template_P3(pq_node x, bool is_pseudo_root)
{
  pq_node    y, z;

#ifdef _DEBUG_PQ_TREE
  if (is_pseudo_root)  cout << "P2";
    else  cout << "P3";
  assert(x != root);
  cons_pq_tree(root,"b","P2/3"); 
#endif

  if (x->part_child1)  return false;

  if (x->full_child_count > 1)
    y= hang_down_full_children(x, x);
  else
    y= x->right_most;

  if (is_pseudo_root)
  {
    pseudo_root->left_most  = y;
    pseudo_root->right_most = y;
    x->node_reset();

    return true;
  }



  if (x->child_count > 2)
  { 
    z = new pq_node_struct;
    z->mark = D_NODE_UNBLOCKED;

                    //for correct blocked chain handling in reduce():

    if (x == pseudo_root->left_most)  pseudo_root->left_most = z;
    if (x == pseudo_root->right_most) pseudo_root->right_most = z;
   
    replace_in_siblings(x, z);

    x->right_most = y->link_one_side ?
                    y->link_one_side : y->link_other_side;

    if (x->right_most->link_one_side == y) 
       x->right_most->link_one_side = NULL;
    else  
       x->right_most->link_other_side = NULL;

    x->child_count--;
    x->node_reset(); 




    x->parent = y->parent = z;
    z->left_most = x;
    z->right_most = y;

    x->link_one_side  = NULL;
    x->link_other_side= y;

    y->link_one_side  = NULL; 
    y->link_other_side= x;

    z->pert_leaf_count  = x->pert_leaf_count;
    z->full_child_count = 1;
    z->child_count = 2;
  }
  else  z = x;

  z->type = D_NODE_QNODE;
  z->left_most->parent_type = D_NODE_QNODE;
  z->right_most->parent_type= D_NODE_QNODE;

 return set_as_part_child(z);
}






bool pq_tree::template_P5(pq_node x, bool is_pseudo_root)
{
  pq_node y, k, z;

#ifdef _DEBUG_PQ_TREE
  if (is_pseudo_root)  cout << "P4";
    else  cout << "P5";
  assert(x != root);
  cons_pq_tree(root,"b","P5.0");
#endif


  if (x->part_child2 || too_many_part )  return false;

// if (!x->part_child1 || x->part_child2 || too_many_part )
//   return false;
// "|| too_many_part" muss bleiben, x->part_child1 darf nicht  !?

  fix_part_child_direction(x);
  
  z = x->part_child1;

  y= hang_down_full_children(x, z);     // Teste, ob es volle Kinder gibt ? ***
                                        // ( sonst y= NULL )
  if (is_pseudo_root)                   // it's _P4
  {
    pseudo_root->right_most = y;

        // suche letztes volles Kind:

    k = y->link_one_side ?
        y->link_one_side : y->link_other_side;

    while (k->status == D_NODE_FULL || k->type == D_NODE_DIR)
      go_to_sibling(k, y);

    pseudo_root->left_most = y;         // y= letztes volles Kind

    z->node_reset();

    if (x->child_count == 1)
      delete_part_node_parent(x);
    else  
      x->node_reset();

    return true;
  }




  if (x->child_count > 2)
  {
    z->pert_leaf_count = x->pert_leaf_count;
    x->child_count--;
    x->pert_leaf_count = 0;  

    x->part_child1 = NULL;
    remove_from_siblings(x, z);
    replace_in_siblings(x, z);
    set_as_part_child(z);
    append_as_left_child(z, x);

    x->node_reset(); 
  }



  else
  {
      if (x->child_count == 2)
      {
        y = (x->right_most == z) ?
             x->left_most : x->right_most;
        append_as_left_child(z, y);
      }


      // for correct blocked chain handling in reduce():

      z->mark = D_NODE_UNBLOCKED;
      if (x == pseudo_root->left_most)  pseudo_root->left_most = z;
      if (x == pseudo_root->right_most) pseudo_root->right_most = z;

      replace_in_siblings(x, z);
      set_as_part_child(z);

      delete x;
  }

  return !too_many_part;
}




bool pq_tree::template_P6(pq_node x)
{

  #ifdef _DEBUG_PQ_TREE
    cout << "P6";
  #endif

 pq_node k, l;
 
  if (!x->part_child2 || too_many_part)  return false;
 
  fix_part_child_direction(x, true);
  hang_down_full_children(x, x->part_child1);


  l = x->part_child1->right_most;
  k = l->link_one_side ?
      l->link_one_side : l->link_other_side;

  while (k->status == D_NODE_FULL || k->type == D_NODE_DIR)
    go_to_sibling(k, l);
  pseudo_root->left_most = l;           // linkestes volles Kind von x->part1
  l->parent = x->part_child1;


  l = x->part_child2->left_most;
  k = l->link_one_side ?
      l->link_one_side : l->link_other_side;

  while (k->status == D_NODE_FULL || k->type == D_NODE_DIR)
    go_to_sibling(k, l);
  pseudo_root->right_most = l;          // rechtestes volles Kind von x->part2



  k= x->part_child1; 
  l= remove_from_siblings(x, x->part_child2);
  x->child_count--;

  if (k->right_most->link_one_side)
     k->right_most->link_other_side = l->left_most;
  else
     k->right_most->link_one_side = l->left_most;

  if (l->left_most->link_one_side)
     l->left_most->link_other_side = k->right_most;
  else
     l->left_most->link_one_side = k->right_most;

  k->right_most = l->right_most;
  k->right_most->parent= k;


  delete l;

  x->part_child1->node_reset();
  
  if (x->child_count == 1)
    delete_part_node_parent(x);
  else
    x->node_reset();
  
  return true;
}








bool pq_tree::template_Q2(pq_node x)
{                 // template for a Q-node with empties and/or 1 partial child
  

#ifdef _DEBUG_PQ_TREE
cout << "Q2" << endl;
if (pseudo_root->leaf_index > 1)  show("in Q2",x);
//show("in Q2",x);
//show("in Q2",root);
#endif

   if (x->part_child2)  return false;   

//  if (!x->part_child1 || x->part_child2 || too_many_part)     // darf nicht !
//    return false;   
  

  pq_node k;                      // to run through the children
  pq_node l;                      // dito
  pq_node l1,l2;                  // dito
  pq_node m,n;                    // dito, but see text
  pq_node p1=NULL;                // the part_child
  pq_node d1=NULL;                // the side the full's will be turned to ...
  pq_node d2=NULL;                // and the other side
  pq_node aux_leftm=NULL;
  pq_node aux_rightm=NULL;        // dummies for pseudo_root->end_most's
  
  bool    full_found=false;       // turn the full's to the outside
  bool    a,b,c,d;                // for testing of blocked chain
  
  
  if ((p1 = x->part_child1) != 0)
  {
    l1 = l2 = x->part_child1;
  
    m = p1->link_one_side;
    skip_dir(m,l1);
  
    n = p1->link_other_side;
    skip_dir(n,l2);
  
   if (x->full_child_count)
   {
    a = m ? m->status == D_NODE_FULL : 0;
    b = n ? n->status == D_NODE_FULL : 0;   
  
    if (!(a ^ b))  return false;   // xor; (both (full)) or (both (NULL or empty))
  
    if (x->full_child_count == 1)
    {
      if (a) 
      { d1 = p1->link_one_side; 
        d2 = p1->link_other_side;
        k = m;
        l = l1;
      }
      else
      { d2 = p1->link_one_side; 
        d1 = p1->link_other_side;
        k = n;
        l = l2;
      }
    
      go_to_sibling(k, l);

      skip_dir(k, l);
      aux_leftm = l;

// in case of a blocked chain the end_most's of pseudo_root are already used
// and valid
// next reduce round Q2 will match pseudo_root as the father of the blocked chain
    
       full_found = true;
     
      }
    else
      {
         if (a)
            { k = m; l = l1; }
         else
            { k = n; l = l2; }
  
         for(int i = 2; i <= x->full_child_count; i++)
         { go_to_sibling(k, l);
           skip_dir(k,l);
           if ( !k || k->status != D_NODE_FULL )  return false;      

          //there is an empty between the full's
          //or not all full's are at one side
         }
  
         full_found = true;
         go_to_sibling(k, l);  

         skip_dir(k,l);
         aux_leftm = l;
  
         if (a) 
           { d1 = p1->link_one_side; d2 = p1->link_other_side; } 
         else
           { d2 = p1->link_one_side; d1 = p1->link_other_side; }
      
        }
      
    }                    //if (x->full_child_count)
  else
    {                    //... no full's, only a part_child
      
      if (pseudo_root->status == D_NODE_FULL)
      { // ROOT(T,S) is reached
      
        d1 = p1->link_one_side;  
        d2 = p1->link_other_side;
  
        if (d1->type == D_NODE_DIR)
        { l = p1;  
          k = d1;  
          skip_dir(k,l);
          aux_leftm = l;
         }    
        else
          if (p1->right_most->status == D_NODE_FULL) 
             aux_leftm = p1->right_most;
          else  
             aux_leftm = p1->left_most;
       }
      else
      { if (m && n)  return false;        //-part_child is between empties
  
        d1 = NULL;                        //-part_child's full endmost will be
        d2 = m ? p1->link_one_side : p1->link_other_side;
                                   //turned outside and becomes an end_most
       }
   }
  
  } //end of "if (p1 = x->part_child1)"
  else
  { // there's no part_child
  
   if (x->left_most->status == D_NODE_FULL)
     l = x->left_most;
   else
     { l = x->right_most;
       if ( l->status != D_NODE_FULL)  return false;
      }
  
//   if ( pseudo_root->type != D_NODE_QNODE || x == pseudo_root)
//      aux_rightm = aux_leftm = l;
        aux_rightm = aux_leftm = l;
  
   if (x->full_child_count > 1)
   {
     k = l->link_one_side ? l->link_one_side : l->link_other_side;
     skip_dir(k,l);
  
     if (k->status == D_NODE_FULL)
     { for(int i = 2; i <= x->full_child_count; i++)
       { skip_dir(k,l);
         if ( k->status != D_NODE_FULL)  return false;
         go_to_sibling(k, l);
       }
//       if ( pseudo_root->type != D_NODE_QNODE || x == pseudo_root )
//       if ( pseudo_root->status == D_NODE_FULL)
//       { 
         skip_dir(k,l);
         aux_leftm = l;
//        }
      }
     else return false;    // empties are intermingled with full's
   }
  } //else from "if (p1 = x->part_child1)"
                           //no empty's and no part_child between full's
                           //template applicable, now the replacement:
  if (p1)
  {
   if (p1->right_most->status == D_NODE_FULL)  
     { m = p1->right_most; n = p1->left_most; }
   else 
     { m = p1->left_most, n = p1->right_most; }
  
   if (m->link_one_side) 
      m->link_other_side = d1;
   else
      m->link_one_side = d1;
  
   if (d1) 
   {  if (d1->link_one_side == p1) 
         d1->link_one_side = m;
      else
         d1->link_other_side = m;
    }
  
   if (n->link_one_side) 
      n->link_other_side = d2;
   else
      n->link_one_side = d2;
  
   if (d2) 
   {  if (d2->link_one_side == p1) 
         d2->link_one_side = n;
      else
         d2->link_other_side = n;
    }
  
   if (p1 == x->left_most)
   { x->left_most = full_found ? n : m;
     x->left_most->parent = x;
    }
  
   if (p1 == x->right_most) 
   { x->right_most = full_found ? n : m;
     x->right_most->parent = x;
    }
  
   x->part_child1 = NULL;

   //reversed (if necessary) and chained

   x->full_child_count += p1->full_child_count;
   delete p1;
  
   l = d1;  
   k = m;
  
   while (k && k->status == D_NODE_FULL)
   { k->parent = x;
     go_to_sibling(k, l);
     skip_dir(k,l);
   }
  
   aux_rightm = l;
  
  }
   
  if (aux_leftm && aux_rightm)
  { c = aux_leftm->link_one_side && aux_leftm->link_other_side;
    d = aux_rightm->link_one_side && aux_rightm->link_other_side;
    if (c && d)  x->status = D_NODE_DOUBLE_PARTIAL;
   }


  if (pseudo_root->status == D_NODE_FULL || x == pseudo_root){
    pseudo_root->left_most = aux_leftm;
    pseudo_root->right_most = aux_rightm;
  }
  else
  if (x->status == D_NODE_DOUBLE_PARTIAL)  return false;

     
  if (x != pseudo_root)
  { // father pointer of x is valid, that means != NULL
   if (pseudo_root->status == D_NODE_FULL) // ROOT(T,S) reached
       x->node_reset();
   else
     { x->status = D_NODE_PARTIAL;
       if (x->parent->part_child1)
         { if (x->parent->part_child2)
             { too_many_part = true;  
               return false;
              }
           else  
              x->parent->part_child2 = x;
          }
       else  
          x->parent->part_child1 = x;
                  //x becomes one of the x->parent->part_children
      }
   }
  
return true;

}






bool pq_tree::template_Q3(pq_node x)
{                 // template for a Q-node with exactly 2 partial children



#ifdef _DEBUG_PQ_TREE
cout << "Q3";
#endif

  if ( pseudo_root->status != D_NODE_FULL ||
       too_many_part || 
       !x->part_child2 ) return false;
  

  pq_node  k;      //to run through the children
  pq_node  l;      //dito
  pq_node  p1;     //a part_child ...
  pq_node  d1;     //... and its neighbour in direction to the full's
  pq_node  p2;     //dito
  pq_node  d2;     //dito
  pq_node  mm,m;   //to run through the children
  pq_node  nn,n;   //dito
  pq_node  l1,l2;  //dito
  
  unsigned char   cc;     //for a check
  
  bool a,b;
  
  
   l1 = l2 = p1 = x->part_child1;
   m = p1->link_one_side;
   skip_dir(m,l1);
  
   n = p1->link_other_side;
   skip_dir(n,l2);
  
   l1 = l2 = p2 = x->part_child2;
   mm = p2->link_one_side;
   skip_dir(mm,l1);
   nn = p2->link_other_side;
   skip_dir(nn,l2);
   
   if (x->full_child_count)
   { // x has full children
    
    cc = m ? (m->status == D_NODE_PARTIAL ? 1 :
             (m->status == D_NODE_FULL ? 2 : 0)) : 0;
    cc += n ? (n->status == D_NODE_PARTIAL ? 1 :
              (n->status == D_NODE_FULL ? 2 : 0)) : 0;
    if (cc != 2)  return false;         // OH GOTT !! ***

    d1 = (m && m->status == D_NODE_FULL) ?  p1->link_one_side : p1->link_other_side;
  
    cc = mm ? (mm->status == D_NODE_PARTIAL ? 1 : (mm->status == D_NODE_FULL ? 2 : 0)) : 0;
    cc += nn ? (nn->status == D_NODE_PARTIAL ? 1 : (nn->status == D_NODE_FULL ? 2 : 0)) : 0;
    if (cc != 2)  return false;         // ***

    d2 = (mm && mm->status == D_NODE_FULL) ? p2->link_one_side : p2->link_other_side;
    
    /*
    explanation:
    check values for
    NULL empty part full
     0     0     1    2
  
    One sibling is "NULL" or "empty" and the other is "full" is a necessary 
    condition here for a valid Q3 situation (the rest of the test follows).
    */
  
    if (x->full_child_count > 1)
    {
      if (mm && mm->status == D_NODE_FULL)
        { d2 = p2->link_one_side;  k = mm;  l = l1;}
     else
        { d2 = p2->link_other_side;  k = nn;  l = l2;}

     for(int i = 2; i <= x->full_child_count; i++)
     { go_to_sibling(k, l);
       skip_dir(k,l);

       if (!k || k->status != D_NODE_FULL)  return false;  
         // because there is an empty or an part_child between the full's
     }
     
     go_to_sibling(k, l);
     skip_dir(k,l);

     if (!k || k->status != D_NODE_PARTIAL)  return false;  
    }
   }
   else
    { // x has no full child
  
      a = m ? m->status == D_NODE_PARTIAL : 0;
      b = n ? n->status == D_NODE_PARTIAL : 0;
      d1 = a ? p1->link_one_side : p1->link_other_side;
      if (!(a ^ b))  return false;   
  
      a = mm ? mm->status == D_NODE_PARTIAL : 0;
      b = nn ? nn->status == D_NODE_PARTIAL : 0;
      d2 = a ? p2->link_one_side : p2->link_other_side;
      if (!(a ^ b))  return false;   
    }
  
  //no empties and no part_child between full's
  //template is applicable, now the replacement:
  
  pq_node m1,m2;  //the full end_most of a part_child
  pq_node n1,n2;  //the empty end_most of a part_child
  pq_node o;      //a dummy
  
  if (d1 == p2)
  { // the partial children are neighbours
  
   if (p1->right_most->status == D_NODE_FULL)
      { m1 = p1->right_most;
        n1 = p1->left_most;
       }
   else
      { m1 = p1->left_most;
        n1 = p1->right_most;
       }
  
   if (p2->right_most->status == D_NODE_FULL)
      { m2 = p2->right_most;
        n2 = p2->left_most;
       }
   else 
      { m2 = p2->left_most;
        n2 = p2->right_most;
       }
  
   o = (p1->link_one_side == d1) ? p1->link_other_side : p1->link_one_side;
  
   if (m1->link_one_side) m1->link_other_side = m2; else m1->link_one_side = m2;
   if (m2->link_one_side) m2->link_other_side = m1; else m2->link_one_side = m1;
   if (n1->link_one_side) n1->link_other_side = o;  else n1->link_one_side = o;
  
   if (o)
   { if (o->link_one_side == p1)
        o->link_one_side = n1;
     else
        o->link_other_side = n1;
    }
  
   o = (p2->link_one_side == d2) ? p2->link_other_side : p2->link_one_side;
  
   if (n2->link_one_side) 
      n2->link_other_side = o;
   else
      n2->link_one_side = o;
  
   if (o)
   { if (o->link_one_side == p2)
        o->link_one_side = n2;
     else
        o->link_other_side = n2;
    }
  
   d1 = m2;
   d2 = m1;
                          //reversed and chained
  }
  else
  { // full children between the partial
  
   if (p1->right_most->status == D_NODE_FULL)  
     { m1 = p1->right_most; 
       n1 = p1->left_most;
      }
   else 
     { m1 = p1->left_most; 
       n1 = p1->right_most;
      }
  
   o = (p1->link_one_side == d1) ? p1->link_other_side : p1->link_one_side;
  
   if (m1->link_one_side)       m1->link_other_side = d1; else m1->link_one_side = d1; 
   if (d1->link_one_side == p1) d1->link_one_side = m1; else d1->link_other_side = m1;
   if (n1->link_one_side)       n1->link_other_side = o;  else n1->link_one_side = o;
  
   if (o)
   { if (o->link_one_side == p1)  
        o->link_one_side = n1;
     else  
        o->link_other_side = n1;
    }
  
   if (p2->right_most->status == D_NODE_FULL)  
      { m2 = p2->right_most; 
        n2 = p2->left_most; 
       }
   else 
      { m2 = p2->left_most; 
        n2 = p2->right_most; 
       }
  
   o = (p2->link_one_side == d2) ? p2->link_other_side : p2->link_one_side;
  
   if (m2->link_one_side)       m2->link_other_side = d2; else m2->link_one_side = d2;
   if (d2->link_one_side == p2) d2->link_one_side = m2; else d2->link_other_side = m2;
   if (n2->link_one_side)       n2->link_other_side = o;  else n2->link_one_side = o;
  
  
   if (o)
   { if (o->link_one_side == p2)  
        o->link_one_side = n2;
     else  
        o->link_other_side = n2;
    }
  
   // reversed and chained
  }
  
  l = d1;  
  k = m1;
  
  while (k->status == D_NODE_FULL)
  { go_to_sibling(k, l);
    skip_dir(k,l);
  }
  
  pseudo_root->left_most = l;
  
  l = d2;  
  k = m2;
  
  while (k->status == D_NODE_FULL)
  { go_to_sibling(k, l);
    skip_dir(k,l);
  }
  
  pseudo_root->right_most = l;         //end_most of pseudo_root now is valid
  
  if (p1 == x->left_most)
    { x->left_most = n1;
      n1->parent = x;
     }
  else
    if (p2 == x->left_most)
    { x->left_most = n2;
      n2->parent = x;
     }
  
  if (p1 == x->right_most)
    { x->right_most = n1;
      n1->parent = x;
     }
  else
    if (p2 == x->right_most)
    { x->right_most = n2;
      n2->parent = x;
     }
  
  if (x != pseudo_root)  x->node_reset();
  
  pseudo_root->left_most->parent = x;
  
  delete p1;  
  delete p2;
  
  x->part_child1 = x->part_child2 = NULL;
  
  return true;
}









pq_tree::~pq_tree()
{ 
  if (!successful)  del_subtree(root);
  delete   pseudo_root;
  delete[] leaves;
}


bool pq_tree::replace(list<int> & D, list<int>& U, list<int>& I)
{ if ( D.length() > 0 )
  { if ( !reduction(D) ) return false;
    pert_sequence(I);
   }
  update(U);
  return true;
}


bool pq_tree::reduction(list<int>& S)
{ 
 list_item  lit= processed.first();

  if (lit)
  { do  processed.inf(lit)->node_reset();
    while( (lit=processed.succ(lit)) != 0 );

    processed.clear();
  }

  pseudo_root->type = D_NODE_PNODE;
  pseudo_root->node_reset();


  successful = bubble(S) && reduce(S); 

  #ifdef _DEBUG_PQ_TREE
    if (!successful)  show("not successful",root);
  #endif

  return successful;
}



void pq_tree::sequence(list<int>& S, pq_node x, pq_node l)
{
  if (x->type == D_NODE_LEAF)  
  { S.append(x->leaf_index);
    return;
  }

 pq_node k = x->left_most;
 pq_node r = x->right_most;

   do
   { if (k->type == D_NODE_DIR)
     { int i = S.pop() +1;          // insert one more DIR-ptr in the sequence S
                                    // with respect to its direction

       if (k->link_one_side == l) 
         S.push(k->leaf_index);
       else 
         S.push(-k->leaf_index);
       S.push(i);
       processed.push(k);         // we have to delete the DIR-ptr in update()
     }
     else  
       sequence(S,k);

     go_to_sibling(k, l);

   } while (l != r);
}





bool pq_tree::bubble(list<int>& S)
{
  queue<pq_node>  Q;
  pq_node         x, y, z, k, l;
  int              m, blocked_found;

  #ifdef _DEBUG_PQ_TREE
  cout << endl;
  #endif

  root_reached = 0;
  blocked_chain_count = 0;
  blocked_nodes_count = 0;

  pseudo_root->left_most = NULL;
  pseudo_root->right_most= NULL;

  forall(m, S)
  { x = leaves[m];
    x->mark = D_NODE_QUEUED;  Q.append(x); 
  }

  #ifdef _DEBUG_PQ_TREE
  printf(" %d ",this->pseudo_root->leaf_index);
  cons_pq_tree(root,"a","bubble");
  // show("in bubble",root);
  #endif


 while ((Q.size() + blocked_chain_count + root_reached) > 1)
 {
  if (Q.empty())  return false;
 
  x = Q.pop(); 
 
  if ( x->parent_type == D_NODE_PNODE || !x->link_one_side || !x->link_other_side)  
     x->mark = D_NODE_UNBLOCKED;
  else
    { // try to make it valid in constant time 
      x->mark = D_NODE_BLOCKED;
      k = x->link_one_side; l = x;
      skip_dir(k,l);
      if ( k->mark == D_NODE_UNBLOCKED)
        { //the link_one_side-sibling has a valid parent 
          x->mark = D_NODE_UNBLOCKED;
          x->parent = k->parent;
         } 
      else
        { k = x->link_other_side;  l = x;
          skip_dir(k,l);
          if ( k->mark == D_NODE_UNBLOCKED)
          { //the link_other_side-sibling has a valid parent
            x->mark = D_NODE_UNBLOCKED;
            x->parent = k->parent;
           }
         }
      }
 
  if (x->mark == D_NODE_UNBLOCKED)
  { //x has got a valid parent
    y = x->parent;                
    z = x->link_one_side;
    if ( z && (z->mark == D_NODE_BLOCKED || z->type == D_NODE_DIR) )
    { l = x;  
      blocked_found = 0;
      while ( z && (z->mark == D_NODE_BLOCKED || z->type == D_NODE_DIR) )
      { if ( z->type != D_NODE_DIR)
        { blocked_found = 1;
          z->parent = y;
          z->mark = D_NODE_UNBLOCKED;
          y->pert_child_count++;
          blocked_nodes_count--;
         }

        go_to_sibling(z, l);
       }
      if (blocked_found)  blocked_chain_count--;
    }               //to unblock a blocked chain in x->link_one_side direction
 
   z = x->link_other_side;
 
   if ( z && (z->mark == D_NODE_BLOCKED || z->type == D_NODE_DIR) )
   { l = x;  
     blocked_found = 0;
     while ( z && (z->mark == D_NODE_BLOCKED || z->type == D_NODE_DIR) )
     { if ( z->type != D_NODE_DIR)
       { blocked_found = 1;
         z->mark = D_NODE_UNBLOCKED;
         z->parent = y;
         y->pert_child_count++;
         blocked_nodes_count--;
       }
       go_to_sibling(z, l);

     }             //to unblock a blocked chain in x->link_other_side direction
     if (blocked_found)  blocked_chain_count--;
   }
 
   if ( !y )  
      root_reached = 1; 
   else
    { y->pert_child_count++;
      if (y->mark == D_NODE_UNMARKED)
      { Q.append(y);
        y->mark = D_NODE_QUEUED;
       }
     }
 
  } 
  else
  { // x's parent is not valid
    k = x->link_one_side;  
    l = x;  
    skip_dir(k,l); 
    if (k->mark == D_NODE_BLOCKED)  blocked_chain_count--;
    k = x->link_other_side;  
    l = x;  
    skip_dir(k,l);
    if (k->mark == D_NODE_BLOCKED)  blocked_chain_count--;
    blocked_chain_count++;
    blocked_nodes_count++;
   }
 
 } //end of "while (Q->size() + blocked_chain_count + root_reached > 1)"

 if (blocked_chain_count)
 { 
   pseudo_root->pert_child_count = blocked_nodes_count;
   pseudo_root->type = D_NODE_QNODE;
   
   #ifdef _DEBUG_PQ_TREE
//   show("blocked_chain_count am ende von bubble",root);
   #endif

  }

#ifdef _DEBUG_PQ_TREE
//if (pseudo_root->leaf_index > 40)  show("Ende bubble", root); 
#endif

 return true;
}





void pq_tree::bubble_reset(pq_node x)
{
  // in case bubble affects the nodes over the pertinent subtree root
  // we have to reset their pert_child_counts

 pq_node k;

//  while (x && x->pert_child_count)            // alt

  while (x && x->pert_child_count && x != pseudo_root)
  {
    if (x->mark == D_NODE_UNBLOCKED) k= x->parent;
      else k= NULL;
    x->pert_child_count = 0;
    x->mark  = D_NODE_UNMARKED;
    x->status= D_NODE_EMPTY;
    x = k;
  }
}






bool pq_tree::reduce(list<int>& S)
{
  queue<pq_node>  Q;
  pq_node         x, y, k, l;
  
  int              S_size = S.size();
  int              x_type;

//show_pq_tree_test(root);

  while (!S.empty())
  { x = leaves[S.pop()];
    x->pert_leaf_count = 1;
    Q.append(x);
   }

 while (!Q.empty())
 { x = Q.pop();

#ifdef _DEBUG_PQ_TREE
cons_pq_tree(root,"b","reduce");
int     aaa=0;
#endif

 if (x->mark == D_NODE_BLOCKED)         
 { // a blocked chain exists and its members
   // get the "auxiliary" parent pseudo_root

  x->parent = pseudo_root;
  x->mark = D_NODE_UNBLOCKED;

  l = x;
  k = x->link_one_side;
  skip_dir(k, l);

  if (!k || k->mark == D_NODE_UNMARKED)
  { if (pseudo_root->left_most)  
       pseudo_root->right_most = l;
    else  
       pseudo_root->left_most = l;
    }

  l = x;
  k = x->link_other_side;
  skip_dir(k,l);

  if (!k || k->mark == D_NODE_UNMARKED)
  { if (pseudo_root->left_most)  
       pseudo_root->right_most = l;
    else  
       pseudo_root->left_most = l;
   }

  if (Q.empty()) pseudo_root->type = D_NODE_QNODE;

 }

#ifdef _DEBUG_PQ_TREE
//if (!aaa && x->type != D_NODE_LEAF) show("in reduce, der momentane Unterbaum",x);
if (pseudo_root->leaf_index > 40)  show("in reduce, der gesamte Baum", root);
cout << flush;
#endif

 x_type= x->type;               // Type may change in template application

 if (x->pert_leaf_count < S_size)
 {
   if (x != pseudo_root)                // ist immer true hier ? !
   { y = x->parent; 
     y->pert_leaf_count += x->pert_leaf_count;
     if (!(--y->pert_child_count))  Q.append(y);
   }

   if (x_type == D_NODE_LEAF)
     if (!template_PQL1(x, false)) return false;

   if (x_type == D_NODE_PNODE)
     if (!template_PQL1(x, false))
     if (!template_P3(x, false))
     if (!template_P5(x, false)) return false;

   if (x_type == D_NODE_QNODE)
     if (!template_PQL1(x, false))
     if (!template_Q2(x)) return false;
 }
 else
 {                               // x is pruned pert subtree root (PRUNED(T,S))
   pseudo_root->status = D_NODE_FULL;           // ROOT(T,S) reached
   if (x->parent)  bubble_reset(x->parent);

   if (x_type == D_NODE_LEAF)
     if (!template_PQL1(x, true)) return false;

   if (x_type == D_NODE_PNODE)
     if (!template_PQL1(x, true))
     if (!template_P3(x, true))
     if (!template_P5(x, true))
     if (!template_P6(x)) return false;

   if (x_type == D_NODE_QNODE)
     if (!template_PQL1(x, true))
     if (!template_Q2(x))
     if (!template_Q3(x)) return false;
 }

 }       // while Q not empty

#ifdef _DEBUG_PQ_TREE
cons_pq_tree(root,"b","reduce 2");
//show("in reduce, der gesamte Baum nach Reduction", root);
#endif

return true;

}





void pq_tree::pert_sequence(list<int> &S) 
{

#ifdef _DEBUG_PQ_TREE
  if (!pseudo_root->left_most){
   cout << "pseudo_root->left_most gleich NULL in pert_sequence" << endl;
   exit(1);
  }
#endif

  pq_node l = pseudo_root->left_most;
  pq_node k;

  S.clear();
  S.push(0);                // preparation

  if (l == pseudo_root->right_most)
                            // if pseudo_root has only one child life is easy.
  { sequence(S, l);  
    return; 
  }

        // Otherwise we have to find the direction to pseudo_root->right_most.
        // Note that an endmost of pseudo_root not necessary has any NULL-link.

  if (l->link_one_side)
  { k = l->link_one_side;
    skip_dir(k,l);
    l = (k && k->status == D_NODE_FULL)
        ? pseudo_root->left_most->link_other_side
        : pseudo_root->left_most->link_one_side; 
  }
  else  
    l = NULL;

  // The direction is detected, we can scan the sequence S.

  sequence(S, pseudo_root, l);
}




void pq_tree::leaves_double()
{ 
        // if any leaf_index > leaves_size occurs we double the size of array
        // leaves, copy the content of the old array in the lower half of
        // the new and delete the old. Initially leaves_size is 16.
        // Maybe the user had told the total number of leaves while defining
        // his PQ tree.

  if (leaves_size)
  { int  i;

    pq_node* A = new pq_node[2 * leaves_size];
    if (!A) LEDA_EXCEPTION(1,"pq_tree: out of memory");

    for (i=0; i < leaves_size; i++)  A[i] = leaves[i];
    leaves_size *= 2;
    while (i < leaves_size)  A[i++] = NULL;
    delete[]  leaves;
    leaves = A;
  }
  else  leaves_init(32);

 return;
}




void pq_tree::leaves_init(int lsize)
{
  leaves_size = lsize;

  leaves = new pq_node[leaves_size];
  if (!leaves)  LEDA_EXCEPTION(1, "pq_tree: out of memory");
  for(int i = 0; i < leaves_size; i++)  leaves[i] = NULL;
}




void pq_tree::del_pert_subtree()
{
  pq_node k = pseudo_root->left_most;
  pq_node l = NULL;

  while (k && k != pseudo_root->right_most)
  { go_to_sibling(k, l);
    del_subtree(l); // Note that also l is deleted in this function call.
                    // But to progress correctly in the chain we need l's
                    // value (see previous command).
  }
}



        // loescht alle Kinder von x und dann x selber:

void pq_tree::del_subtree(pq_node x)
{
  pq_node k = x->left_most;
  pq_node l = NULL;

  while (k)      
  { //Note that the value of l is essentially for the loop
    //but the pointer l is not valid 
    //(see also the comment in del_pert_subtree() ).

    go_to_sibling(k, l);
    del_subtree(l);     //delete the subtree recursivly
  }

  delete x;

}





bool pq_tree::update(list<int>& S)
{
 int       i=0;
 pq_node  v, w, k= NULL, l;

  if (S.empty())
  {     // delete the pert. subtree contained in processed...

    while (!processed.empty())
    { w = processed.pop();
      if (w->type == D_NODE_LEAF)  leaves[w->leaf_index] = NULL;
      delete w;
    } 

        // if there's anything else in the PQ_tree the reduction has failed.

    for(i=0; i < leaves_size && !leaves[i]; i++);

   return (i == leaves_size);
  }

  if (root && (pseudo_root->left_most == root))
  { // A special case requires special treatment.
    // Note that pseudo_root has only 1 child in this case.

    while (!processed.empty())
    { w = processed.pop();
      if (w->type == D_NODE_LEAF) leaves[w->leaf_index] = NULL;
      delete w;
    } 

        // The pertinent subtree, here that means the whole pq_tree
        // is deleted. The application has failed.
    return false;
  }

  i = 0;
  v = new pq_node_struct;

  if (pseudo_root->left_most &&
      pseudo_root->left_most->parent_type == D_NODE_PNODE)
  { v->parent_type = D_NODE_PNODE; 
    v->parent = pseudo_root->left_most->parent;
  }
  else  
    if (pseudo_root->left_most)  v->parent_type = D_NODE_QNODE;

  if (S.size() == 1)
  {                                              // then create v as a leaf
    v->leaf_index = S.pop();

    while (v->leaf_index >= leaves_size) leaves_double();

    if (leaves[v->leaf_index]) return false;

    leaves[v->leaf_index] = v;
    v->type = D_NODE_LEAF;
  }
  else
  {   //then create v as a P-node with leaves labelled with the elements of S
                  //  v->leaf_index = pseudo_root->leaf_index;
                  //only for testing

      v->type = D_NODE_PNODE;
      v->child_count = S.size();
      v->left_most = l = new pq_node_struct;
      l->link_one_side = NULL;
      l->parent = v;
      l->leaf_index = S.pop();

      while (l->leaf_index >= leaves_size) leaves_double();

      if (leaves[l->leaf_index])  return false;

      leaves[l->leaf_index]=l;
      l->parent_type = D_NODE_PNODE;
      l->type = D_NODE_LEAF;

      while (!S.empty())
      { l->link_other_side = k = new pq_node_struct;
        k->leaf_index = S.pop();

        while (k->leaf_index >= leaves_size)  leaves_double();
 
        if (leaves[k->leaf_index])  return false;
 
        leaves[k->leaf_index] = k;
        k->parent_type = D_NODE_PNODE;
        k->type = D_NODE_LEAF;
        k->parent = v;
        k->link_one_side = l;
        l = k;
      }
      k->link_other_side = NULL;
      v->right_most = k;
  }
     
  if (!root)
  {                       //then the pq_tree is just constructed & still empty
    root = v;
    v->parent_type = D_NODE_QNODE;              // *** warum ?
    return true;
  }

                          //replace the full chain under pseudo_root by v
  k = pseudo_root->left_most;
  l = pseudo_root->right_most;

  if (k == l)            //then pseudo_root has only one child        
    replace_in_siblings(k, v);
  else
  {
     pq_node  k1, l1;

     if (k->link_one_side && k->link_other_side)
     {   // Then k has a full and an empty sibling.
         // The full is contained in the pertinent subtree and to be
         // deleted. The empty becomes a sibling of v.

      if (k->link_one_side->status == D_NODE_EMPTY &&
          k->link_one_side->type != D_NODE_DIR)
            v->link_one_side = k->link_one_side;
      else
         if (k->link_other_side->status == D_NODE_EMPTY &&
             k->link_other_side->type != D_NODE_DIR)  
                v->link_one_side = k->link_other_side;
         else
            if (k->link_one_side->status == D_NODE_FULL)  
               v->link_one_side = k->link_other_side;
            else
               if (k->link_other_side->status == D_NODE_FULL) 
                  v->link_one_side = k->link_one_side;
               else
                { k1 = k->link_one_side;  l1 = k;
                  skip_dir(k1,l1);
                  if (k1->status == D_NODE_EMPTY)  
                     v->link_one_side = k->link_one_side;
                  else  
                     v->link_one_side = k->link_other_side;
                 }

       if (v->link_one_side->link_one_side == k)
           v->link_one_side->link_one_side = v;
       else
           v->link_one_side->link_other_side = v;
      }
    else
    {                            // then k is an endmost of his real father
        v->link_one_side = NULL; 
        if (k->parent->right_most == k) 
           k->parent->right_most = v;
        else
           k->parent->left_most = v;

        v->parent = k->parent;
     }              

        // v is chained instead of k.

    if (l->link_one_side && l->link_other_side)
    {                     // analogous to treatment of k, as above
       if (l->link_one_side->status == D_NODE_EMPTY &&
           l->link_one_side->type != D_NODE_DIR)
             v->link_other_side = l->link_one_side;
       else
          if (l->link_other_side->status == D_NODE_EMPTY &&
              l->link_other_side->type != D_NODE_DIR)
                v->link_other_side = l->link_other_side;
          else
             if (l->link_one_side->status == D_NODE_FULL)
                v->link_other_side = l->link_other_side;
             else
                if (l->link_other_side->status == D_NODE_FULL)
                   v->link_other_side = l->link_one_side;
                else
                 { k1 = l->link_one_side;  l1 = l;
                   skip_dir(k1,l1);
                   if (k1->status == D_NODE_EMPTY)
                      v->link_other_side = l->link_one_side;
                   else
                      v->link_other_side = l->link_other_side;
                  }
       if (v->link_other_side->link_one_side == l) 
           v->link_other_side->link_one_side = v;
       else
           v->link_other_side->link_other_side = v;
      }
   else
     {                   //then l is an endmost of his real father
       v->link_other_side = NULL;
       if (l->parent->right_most == l) 
          l->parent->right_most = v;
       else
          l->parent->left_most = v;

       v->parent = l->parent;
      }              
   // v is chained instead of l.

  }               

        // Now v replaces the chain of full nodes beyond 
        // the real father.

        // v is in the scanning direction of the pert subtree
        // and if necessary we can add a DIR-ptr

  if (pseudo_root->left_most->parent_type == D_NODE_QNODE)
  {
    k = new pq_node_struct;

    k->leaf_index = pseudo_root->leaf_index;
    k->type = D_NODE_DIR;        // insert a DIR-ptr, but not as an end_most
    if (v->link_one_side)
      { k->link_one_side = v->link_one_side;
        v->link_one_side = k;
        k->link_other_side = v;
        k->link_one_side->link_one_side == v ?
         (k->link_one_side->link_one_side = k) :
         (k->link_one_side->link_other_side = k);
       }
    else
      { k->link_other_side = v->link_other_side;
        v->link_other_side = k;
        k->link_one_side = v;
        k->link_other_side->link_one_side == v ?
        (k->link_other_side->link_one_side = k) :
        (k->link_other_side->link_other_side = k);
       }
   }

        // Prepare pseudo_root for the next possible DIR-ptr

   pseudo_root->leaf_index++;
   pseudo_root->mark = D_NODE_UNMARKED;
   pseudo_root->status = D_NODE_EMPTY;

        // Now we delete the pertinent subtree.

   while (!processed.empty())
   { w = processed.pop();
     if (w->type == D_NODE_LEAF) leaves[w->leaf_index] = NULL;
     delete w;
   }

#ifdef _DEBUG_PQ_TREE
  cons_pq_tree(root,"b","update");
#endif

 return true;
} 




void pq_tree::go_to_sibling(pq_node& k, pq_node& last_k)
{ if (k->link_one_side == last_k) 
  { last_k = k;
    k = k->link_other_side;
   }
  else
  { last_k = k;
    k = k->link_one_side;
   }
}

void pq_tree::skip_dir(pq_node& k, pq_node& last_k)
{  // skip direction indicators
  while (k && k->type == D_NODE_DIR) go_to_sibling(k, last_k);
 }

LEDA_END_NAMESPACE

 





