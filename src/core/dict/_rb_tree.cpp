/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _rb_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/rb_tree.h>


//----------------------------------------------------------------------------
//  rb_tree:
//
//  rebalancing of red black trees
//
//  S. N"aher (1993)
//----------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

void rb_tree::insert_rebal(rb_tree_item v)
{
  /* preconditions:  v is new node created by insertion
                     v != root
    
                        u
                        |
                        v
                       / \ 
                      x   y
  */

  rb_tree_item u = v->parent;

  get_root()->set_bal(black);

  while (u->get_bal() == red)
  {
    int dir = (v == u->child[left]) ? left : right;

    rb_tree_item p = u->parent; 
    int dir1 = (u == p->child[left]) ? left : right;

    rb_tree_item w = p->child[1-dir1];

    if ( w->get_bal() == red )    // p has two red children (u and w)
       { u->set_bal(black);
         w->set_bal(black);
         if (p == get_root())  
         { p->set_bal(black); 
           break; 
          }
         p->set_bal(red);
         v = p;
         u = p->parent;
        }
    else 
       if (dir1 == dir)      // rebalancing by one rotation
         { rotation(p,u,dir1);
           p->set_bal(red);
           u->set_bal(black);
           break;
          }       
       else
        { double_rotation(p,u,v,dir1);
          p->set_bal(red);
          v->set_bal(black);
          break;
         }
   }

}



void rb_tree::del_rebal(rb_tree_item w, rb_tree_item p)
{
  // precondition:    p is removed inner node
  //                  w is remaining child of p (already linked to parent u)
  //                  w != root



  if (p->get_bal()==red) return;  // case 1 : nothing to do

  if (w->get_bal()==red)          // case 2
  { w->set_bal(black); 
    return; 
   } 


  get_root()->set_bal(black);

  rb_tree_item u = w->parent;
  int dir = (w == u->child[left]) ? left : right;

  while (true)
  {
    rb_tree_item w = u->child[1-dir];
  
    /* situation: black height of subtree rooted at black node 
       v = u->child[dir] is by one too small, w = sibling of v
      
       => increase black height of v or "move" v towards the root
      
                                |                         |
                                u                         u
                 dir=left:     / \        dir=right:     / \ 
                              /   \                     /   \ 
                             v     w                   w     v
                                  / \                 / \ 
                                 /   \               /   \ 
                                y     x             x     y
   
    */
  
    if ( w->get_bal()==black )                    // case 2: v and w are black
      { rb_tree_item y = w->child[1-dir];
        if ( y->get_bal()==red )                  // case 2.b 
           { rotation(u,w,1-dir);
             w->set_bal(u->get_bal());
             u->set_bal(black);
             y->set_bal(black);
             break;
            }
        else   
           if ( (y=w->child[dir])->get_bal()==red ) // case 2.c 
              { double_rotation(u,w,y,1-dir);
                y->set_bal(u->get_bal());
                u->set_bal(black);
                break;
              }
           else 
              if ( u->get_bal()==red )     // case 2.a2
                 { w->set_bal(red);
                   u->set_bal(black);
                   break; 
                  }
              else                        // case 2.a1
                 { rotation(u,w,1-dir);
                   u->set_bal(red);
                   if ( w == get_root() )
                      break;
                   else // the only non-terminating case
                      { u = w->parent;
                        dir = (w == u->child[left]) ? left : right;
                       }
                  }
      }     
    else                                  // case 3: v ist black, w ist red
      { rb_tree_item x = w->child[dir];
        rb_tree_item y;
        if ( x->child[1-dir]->get_bal()==red )          // case 3.b
          { double_rotation(u,w,x,1-dir);
            w->child[dir]->set_bal(black);
            break;
           }
        else 
           if ( (y = x->child[dir])->get_bal()==red )   // case 3.c
             { rotation(x,y,dir);
               w->child[dir] = y; 
               double_rotation(u,w,y,1-dir);
               y->set_bal(black);
               break;
              }
           else                                     // case 3.a 
             { rotation(u,w,1-dir);
               w->set_bal(black);
               x->set_bal(red);
               break;
              }
       }
  
   } /* end of loop */

}

LEDA_END_NAMESPACE
