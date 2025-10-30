/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _p_heap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/impl/p_heap.h>

LEDA_BEGIN_NAMESPACE


static p_heap const *class_ptr;

// ============== comparison_link Macros (s.n.) ================================

#define comparison_link(with_el,new_el)                                  \
  if (cmp(with_el->key,new_el->key)<0)                                   \
    { with_el->r_child=new_el->r_child;                                  \
      if (with_el->r_child!=nil)                                         \
              with_el->r_child->parent=with_el;                          \
      new_el->r_child=with_el->l_child;                                  \
      if (new_el->r_child!=nil)                                          \
              new_el->r_child->parent=new_el;                            \
      new_el->parent=with_el;                                            \
      with_el->l_child=new_el; }                                         \
  else                                                                   \
    { with_el->r_child=new_el->l_child;                                  \
      if (with_el->r_child!=nil)                                         \
              with_el->r_child->parent=with_el;                          \
      new_el->parent=with_el->parent;                                    \
      if (new_el->parent!=nil)                                           \
              new_el->parent->r_child=new_el;                            \
      new_el->l_child=with_el;                                           \
      with_el->parent=new_el;                                            \
      with_el = new_el;                                                  }

#define int_comparison_link(with_el,new_el)                              \
  if (with_el->key < new_el->key)                                        \
    { with_el->r_child=new_el->r_child;                                  \
      if (with_el->r_child!=nil)                                         \
              with_el->r_child->parent=with_el;                          \
      new_el->r_child=with_el->l_child;                                  \
      if (new_el->r_child!=nil)                                          \
              new_el->r_child->parent=new_el;                            \
      new_el->parent=with_el;                                            \
      with_el->l_child=new_el; }                                         \
  else                                                                   \
    { with_el->r_child=new_el->l_child;                                  \
      if (with_el->r_child!=nil)                                         \
              with_el->r_child->parent=with_el;                          \
      new_el->parent=with_el->parent;                                    \
      if (new_el->parent!=nil)                                           \
              new_el->parent->r_child=new_el;                            \
      new_el->l_child=with_el;                                           \
      with_el->parent=new_el;                                            \
      with_el = new_el;                                                  }



//====== construct (p_heap&) ===========================================

p_heap::p_heap(const p_heap& with)
{
    item_count =0;
    if((this!=&with)&&(with.item_count>0)){
        class_ptr=&with;
        copy_sub_tree(head,with.head);
        class_ptr=this;
    }
}

//====== operator = =====================================================

p_heap& p_heap::operator=(const p_heap& with)
{
      if(this!=&with){
         if((with.item_count>0)&&(item_count>0))
                clear();
        class_ptr=&with;
        copy_sub_tree(head,with.head);
        class_ptr=this;
                
        }
        return(*this);
}

//=========== copy_sub_tree =============================================

void  p_heap::copy_sub_tree(ph_item* whereto,ph_item* from) 
{
   if (item_count==0)   // target tree is empty 
   {
        
         head =new ph_item(from->key,from->inf);
         class_ptr->copy_key(head->key);
         class_ptr->copy_inf(head->inf);
         item_count++;
        
         do_copy(head,from->l_child,true);
   }
 
   else
        
                if ((cmp(whereto->key,from->key)<=0)  // precondition:
                        &&(whereto->l_child==nil))    // subelement <= parent
                
                        do_copy(whereto,from,true);
                        // true: that is left child from whereto        
}

 

//====== do_copy ======================================================

void p_heap::do_copy(ph_item* father,ph_item* from,bool direction)
{
// direction : false=right true=left

        
        ph_item* hilf=new_ph_item(from->key,from->inf);
        
        hilf->parent=father;
        if (direction)
                father->l_child=hilf;
        else
                father->r_child=hilf;

        if (from->l_child!=nil)
                do_copy(hilf,from->l_child,true);
                
        if (from->r_child!=nil)
                do_copy(hilf,from->r_child,false);
}

//===== new_ph_item =====================================================

ph_item* p_heap::new_ph_item(GenPtr key,GenPtr inf)
{
        ph_item* help=new ph_item(key,inf);

        copy_key(help->key);
        copy_inf(help->inf);
        help->parent=nil;
        item_count++;

        return help;
}

        
                
// ========== clear ====================================================

void p_heap::clear()
{
  if (item_count>0)
        clear_sub_tree(head);
        
}

// ======= clear_sub_tree ===============================================

void p_heap::clear_sub_tree(ph_item* sub)
{
        
        if (sub->l_child!=nil)
                clear_sub_tree(sub->l_child);
        if (sub->r_child!=nil)
                clear_sub_tree(sub->r_child);
        if (sub->parent!=nil)
        {       if(sub->parent->l_child==sub)
                   sub->parent->l_child=nil;
                else
                   sub->parent->r_child=nil;
         }

        clear_key(sub->key);
        clear_inf(sub->inf);
        delete sub;
        item_count--;
}
                


//======= insert =======================================================

ph_item* p_heap::insert(GenPtr key,GenPtr inf)
{       
        ph_item* help;

        help = new ph_item(key,inf);
        copy_key(help->key);
        copy_inf(help->inf);

        if (item_count==0)      // very first element
          { item_count++;
            head=help;
            return help;
           }
        else                    // just another element
          { item_count++;
            comparison_link(head,help);
            return help;
           }

        
}


// ====== decrease_key ==================================================

void p_heap::decrease_key(ph_item* which,GenPtr key)
{
   register ph_item* help2=nil;
   register ph_item* which_parent = which->parent;

   if (key_type_id() == INT_TYPE_ID)
     if (key <= which->key)  // smaller or equal to the old element
      { 
        which->key=key;
   
        if (which!=head)         // which is not already minimum
        { if (which->r_child!=nil)
          { help2=which->r_child;
            help2->parent=which_parent;
            which->r_child=nil;
           }
   
          if (which_parent->l_child==which)
             which_parent->l_child=help2;
          else                    
             which_parent->r_child=help2;
   
          which->parent=nil;
          int_comparison_link(head,which);
         }
      }
     else /* error */;
   else
     if (cmp(key,which->key)<=0)  // smaller or equal to the old element
      { 
        clear_key(which->key);
        which->key=key;
        copy_key(which->key);
   
        if (which!=head)         // which is not already minimum
        { if (which->r_child!=nil)
          { help2=which->r_child;
            help2->parent=which_parent;
            which->r_child=nil;
           }
   
          if (which_parent->l_child==which)
             which_parent->l_child=help2;
          else                    
             which_parent->r_child=help2;
   
          which->parent=nil;
          comparison_link(head,which);
         }
      }
     else /* error */;
}                       
                

//========= delete_min_multipass ()  (multipass algorithm) =============

void p_heap::delete_min_multipass()
{
 if (item_count==1)     // only one element in structure
   {
        clear_key(head->key);
        clear_inf(head->inf);
        delete head;
        item_count=0;
   }
   else
   {
        head=head->l_child;
        clear_key(head->parent->key);
        clear_inf(head->parent->inf);
        delete head->parent;    // delete min
        head->parent=nil;
        item_count--;
        
      if (head->r_child!=nil)   // there are two ore more consecutive elements
        head=multipass(head);
    
  }// end else

}

//======== delete_min_twopass, (twopass algorithm) ============================

void p_heap::delete_min_twopass()
{
        
   if (item_count==1)   // only one element in structure
   {
        clear_key(head->key);
        clear_inf(head->inf);
        delete head;
        item_count=0;
   }
   else
   {
        head=head->l_child;
        clear_key(head->parent->key);
        clear_inf(head->parent->inf);
        delete head->parent;    // delete min
        head->parent=nil;
        item_count--;
        
      if (head->r_child!=nil)   // there are two ore more consecutive elements
      
        head=twopass(head);
        

      } // end else
}



// ============== twopass ================================================              
        
ph_item*  p_heap::twopass(ph_item* h)
{
 //pass 1 : left to right comparison link (successive pairs of root nodes)

  register ph_item* help1,*help2;

  help1=h;
  help2=h->r_child;
  
  if (key_type_id() == INT_TYPE_ID)
        while (help2!=nil)               // there are 2 ore more elements left
        { h=help1->r_child->r_child;   // use of h as a helper
          int_comparison_link(help1,help2);
                
          if (h!=nil)       // first case comp _link
             if (h->r_child!=nil)
               { // second case
                 // now we have to more nodes to test
                 help2=h->r_child;
                 help1=h;
                }
             else
               help2=nil;
           else
             { h=help1;     // last element in list
               help2=nil;
              }
          }
   else
        while (help2!=nil)
        { h=help1->r_child->r_child;
          comparison_link(help1,help2);
                
          if (h!=nil)
             if (h->r_child!=nil)
               { help2=h->r_child;
                 help1=h;
                }
             else
               help2=nil;
           else
             { h=help1;
               help2=nil;
              }
         }

  //pass 2 : right to left comparison link (allways the two rightmost nodes)

        help1=h->parent;
        help2=h;

   if (key_type_id() == INT_TYPE_ID)
        while (help1!=nil)
        { int_comparison_link(help1,help2);
          h=help1;
          help2=help1;
          help1=help1->parent;
         }
    else
        while (help1!=nil)
        { comparison_link(help1,help2);
          h=help1;
          help2=help1;
          help1=help1->parent;
         }
        
 // h points now again to the very first element

 return h;

}

// ================ multipass ==========================================

ph_item* p_heap::multipass(ph_item* h)
{
 // now pass 1 (multi times) : left to right comparison link 
 //                            (successive pairs of root nodes)
       ph_item* save=h;      
       ph_item* help1,*help2;

       while(h->r_child!=nil)
       { save=h;
         help1=h;
         help2=h->r_child;
        
         while (help2!=nil)      // there are 2 ore more elements left
         { save=help1->r_child->r_child; // use of save as a helper
           comparison_link(help1,help2);
                
           if (save!=nil)       // first case comp _link
             if (save->r_child!=nil)
                { // second case
                  // now we have to more nodes to test
                  help2=save->r_child;
                  help1=save;
                 }
              else
                 help2=nil;
           else
              { save=help1;     // last element in list
                help2=nil;
               }
         } // end while (help2!=nil)


        if (h->parent!=nil)  // may be first element is child (comp link)
                h=h->parent;

    } // end while (repeat pass 1)

  return h;
}

LEDA_END_NAMESPACE
