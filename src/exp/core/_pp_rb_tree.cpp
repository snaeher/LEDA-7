/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _pp_rb_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/pp_rb_tree.h>

//----------------------------------------------------------------------------
//  pp_rb_tree:
//
//  rebalancing of red black trees
//
//  S. Naeher (1993)
//----------------------------------------------------------------------------


LEDA_BEGIN_NAMESPACE

void pp_rb_tree::insert_rebal(pp_rb_tree_item v)
{
  // preconditions:  v is new node created by insertion
  //                 v != root
  //
  /*                    u    */
  /*                    |    */
  /*                    v    */
  /*                   / \   */
  /*                  x   y  */
  

  pp_rb_tree_item u = v->parent;

  root()->set_bal(Black);

  while (u->get_bal() == Red)
  {
    int dir = (v == u->left) ? left : right;

    pp_rb_tree_item p = u->parent; 
    int dir1 = (u == p->left) ? left : right;

    pp_rb_tree_item w = p->child(1-dir1);

    if ( w->get_bal() == Red )    // p has two red children (u and w)
       { u->set_bal(Black);
         w->set_bal(Black);
         if (p == root())  
         { p->set_bal(Black); 
           break; 
          }
         p->set_bal(Red);
         v = p;
         u = p->parent;
        }
    else 
       if (dir1 == dir)      // rebalancing by one rotation
         { rotation(p,u,dir1);
           p->set_bal(Red);
           u->set_bal(Black);
           break;
          }       
       else
        { double_rotation(p,u,v,dir1);
          p->set_bal(Red);
          v->set_bal(Black);
          break;
         }
   }
}

void pp_rb_tree::del_rebal(pp_rb_tree_item w, pp_rb_tree_item p)
{
  // precondition:    p is removed inner node
  //                  w is remaining child of p (already linked to parent u)
  //                  w != root

  if (p->get_bal()==Red) return;  // case 1 : nothing to do

  if (w->get_bal()==Red)          // case 2
  { w->set_bal(Black); 
    return; 
   } 

  root()->set_bal(Black);

  pp_rb_tree_item u = w->parent;
  int dir = (w == u->left) ? left : right;

  while (true)
  {
    pp_rb_tree_item w = u->child(1-dir);
  
    // situation: black height of subtree rooted at black node 
    // v = u->child(dir) is by one too small, w = sibling of v
    //
    // => increase black height of v or "move" v towards the root
    
    /*                          |                         |         */
    /*                          u                         u         */
    /*           dir=left:     / \        dir=right:     / \        */
    /*                        /   \                     /   \       */
    /*                       v     w                   w     v      */
    /*                            / \                 / \           */
    /*                           /   \               /   \          */
    /*                          y     x             x     y         */
  
    if ( w->get_bal()==Black )                    // case 2: v and w are black
      { pp_rb_tree_item y = w->child(1-dir);
        if ( y->get_bal()==Red )                  // case 2.b 
           { rotation(u,w,1-dir);
             w->set_bal(u->get_bal());
             u->set_bal(Black);
             y->set_bal(Black);
             break;
            }
        else   
           if ( (y=w->child(dir))->get_bal()==Red ) // case 2.c 
              { double_rotation(u,w,y,1-dir);
                y->set_bal(u->get_bal());
                u->set_bal(Black);
                break;
              }
           else 
              if ( u->get_bal()==Red )     // case 2.a2
                 { w->set_bal(Red);
                   u->set_bal(Black);
                   break; 
                  }
              else                        // case 2.a1
	        { // u, v, w, x and y are black
		  w->set_bal(Red);
		  // now black height of subtree rooted at u is by 1 too small
		  if (u == root()) break;
		  else {
		    pp_btree_node* new_u = u->parent;
		    dir = (u == new_u->left) ? left : right;
		    u = new_u;
		  }
		}
/*               { rotation(u,w,1-dir);
                   u->set_bal(Red);
                   if ( w == root() )
                      break;
                   else // the only non-terminating case
                      { u = w->parent;
                        dir = (w == u->left) ? left : right;
                       }
                  }*/
      }     
    else                                  // case 3: v ist black, w ist red
      { pp_rb_tree_item x = w->child(dir);
        pp_rb_tree_item y;
        if ( x->child(1-dir)->get_bal()==Red )          // case 3.b
          { double_rotation(u,w,x,1-dir);
            w->child(dir)->set_bal(Black);
            break;
           }
        else 
           if ( (y = x->child(dir))->get_bal()==Red )   // case 3.c
             { rotation(x,y,dir);
               w->child(dir) = y; 
               double_rotation(u,w,y,1-dir);
               y->set_bal(Black);
               break;
              }
           else                                     // case 3.a 
             { rotation(u,w,1-dir);
               w->set_bal(Black);
               x->set_bal(Red);
               break;
              }
       }
  
   } /* end of loop */
}

LEDA_END_NAMESPACE
