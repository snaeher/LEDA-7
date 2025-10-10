/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _f_heap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/impl/f_heap.h>

#include <math.h>

//-----------------------------------------------------------------------------
// class f_heap_node
// 
// S. Naeher (1993)
//-----------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

f_heap_node* f_heap::first_item() const { return node_list; }

f_heap_node* f_heap::next_item(f_heap_node* p) const { return p ? p->next : 0; }



void f_heap_node::link(f_heap_node* child)
// Vereinigung der beiden Heap geordneten Baeume h1 und h2
// child wird neues Kind des Knotens
// Vorbedingung: rank == child->rank && key <= child->key
{ 
   //child aus seiner Liste loeschen
   child->left->right = child->right;
   child->right->left = child->left;

   // child in zirkulaere Liste der Kinder von vater aufnehmen
   if ( children == 0 )
     { children = child;
       // zirkulaere Liste aufbauen
       child->left = child;
       child->right = child;
     }
   else
     // in bestehende Liste aufnehmen
     { child->left = children;
       child->right = children->right;
       children->right->left = child;
       children->right = child;
     }

   child->parent = this;
   rank++;
   child->mark = 0;

}

void f_heap_node::cut(f_heap_node* m_ptr)
// loescht die Beziehung des Knotens zu seinem Elterknoten
// und fuegt ihn in Wurzel-Liste (nach m_ptr) ein
// Precondition: parent != 0 
{ 
    if ( parent->rank == 1 )
         parent->children = 0;
     else  // mehrere Kinder
        { if ( parent->children == this ) parent->children = right;
          // x aus zirkulaerer Liste loeschen
          left->right = right;
          right->left = left;
         } 
     parent->rank--;
     parent=0;

    // in zirkulaere Liste der Wurzeln aufnehmen

    left = m_ptr;
    right = m_ptr->right;
    m_ptr->right->left = this;
    m_ptr->right = this;

}


//-----------------------------------------------------------------------------
// class f_heap
//-----------------------------------------------------------------------------

f_heap::f_heap(int) 
{ node_count = 0;
  minptr = 0;
  node_list = 0;
 }  


f_heap::f_heap(const f_heap& H)
{ node_count = 0;
  minptr = 0;
  node_list = 0;

  f_heap_node* min_p=0;

  for(f_heap_node* p = H.first_item(); p; p = H.next_item(p))
   { f_heap_node* q = insert(p->key,p->inf);  
     // base class insert: calls default virtuals => we must call virtuals of H
     // and compute the minimum
     H.copy_key(q->key);    
     H.copy_inf(q->inf);
     if (min_p ==0) min_p = q;
     else if ( H.cmp(q->key,min_p->key) < 0 ) min_p = q;
    }
   minptr = min_p;
}

f_heap& f_heap::operator=(const f_heap& H)
{ if (this != &H)
  { clear();
    for (f_heap_node* p = H.first_item(); p; p = H.next_item(p)) 
     insert(p->key,p->inf);  // calls correct virtual functions 
   }
  return *this;
 }

  

int f_heap::max_rank() const
{ // max_rank <= 1.4404 * log_2(node_count)
  int x = 0;
  int n = node_count;
  while (n) { x++;
              n >>= 1;
             }
  return int(1.5*x);
}

void f_heap::change_inf(f_heap_node* p, GenPtr i)
{ clear_inf(p->inf);
  copy_inf(i);
  p->inf = i;
}
  

f_heap_node *f_heap::insert(GenPtr k, GenPtr info)
{   // Kreieren eines neuen heap ordered trees und Einfuegen 

    copy_key(k);
    copy_inf(info);

    f_heap_node *neu = new_f_heap_node(k,info);

    if ( minptr == 0 )
      { minptr = neu;
        // zirkulaere Liste aufbauen
        neu->right = neu;
        neu->left = neu;
      }
    else  
      // in zirkulaere Liste aufnehmen und minptr ueberpruefen
      { neu->left = minptr;
        neu->right = minptr->right;
        minptr->right->left = neu;
        minptr->right = neu;
        if ( cmp(k,minptr->key) < 0 ) minptr = neu;
       }

    node_count++;

    return ( neu );

 } 


void f_heap::decrease_key(f_heap_node *start, GenPtr newkey)
{ f_heap_node* vater = start->parent;
  f_heap_node* x = start;

  int d = cmp(newkey,x->key);
  int dm =cmp(newkey,minptr->key);

  if ( d==0  && dm != 0 ) return; 

  if ( d > 0 )
  { cout << "   new = "; print_key(newkey);
    cout << "   old = "; print_key(x->key);
    cout << "   min = "; print_key(minptr->key);
    cout <<  "\n";
    LEDA_EXCEPTION(1,"f_heap: key too large in decrease_key.");
   }

  copy_key(newkey);
  clear_key(x->key);
  x->key = newkey; 

  if ( vater && cmp(newkey,vater->key) <= 0 )
  { // streiche Kante ( x, Vater(x) )
    x->cut(minptr);

    // errege(Vater(x))
    x = vater;
    while ( x->mark && x->parent )
    { vater = x->parent;
      x->cut(minptr);
      x = vater;
     }
    x->mark = 1;
   }

  if ( dm <= 0 ) minptr = start;   // "=" --> del_item

}


f_heap_node* f_heap::merge_heaps(f_heap_node** rank_array)
{ switch ( key_type_id() ) {
  case INT_TYPE_ID: return int_merge_heaps(rank_array);
  default:          return gen_merge_heaps(rank_array);
  }
}


f_heap_node* f_heap::int_merge_heaps(f_heap_node** rank_array)
{
  f_heap_node* lauf = minptr->right;
  f_heap_node* min  = lauf;

  while (lauf != minptr)   // former minimum is stopper
  { f_heap_node* r1 = lauf;
    f_heap_node* r2;
    int rank = r1->rank;
    lauf = lauf->right;
    while ((r2=rank_array[rank]) != 0)
    { rank_array[rank] = 0;
      if (LEDA_ACCESS(int,r1->key) <= LEDA_ACCESS(int,r2->key))
         r1->link(r2);
      else 
       { r2->link(r1);
         r1 = r2; 
        }
      rank++;
     }
    rank_array[rank] = r1;
    if (LEDA_ACCESS(int,r1->key) <= LEDA_ACCESS(int,min->key)) min = r1;
  }
  return min;
}



f_heap_node* f_heap::gen_merge_heaps(f_heap_node** rank_array)
{
  f_heap_node* lauf = minptr->right;
  f_heap_node* min  = lauf;

  while (lauf != minptr)   // former minimum is stopper
  { f_heap_node* r1 = lauf;
    f_heap_node* r2;
    int rank = r1->rank;
    lauf = lauf->right;
    while ((r2=rank_array[rank]) != 0)
    { rank_array[rank] = 0;
      if (cmp(r1->key,r2->key) <= 0) 
         r1->link(r2);
      else 
       { r2->link(r1);
         r1 = r2; 
        }
      rank++;
     }
    rank_array[rank] = r1;
    if ( cmp(r1->key,min->key) <= 0 ) min = r1;  // new minimum
  }
  return min;
}



void f_heap::del_min()
{ 
  // remove node with minimal key

  f_heap_node* res = minptr;

  // empty ?
  if ( minptr == 0 ) return;

  node_count--;

  // last node ?
  if (node_count==0 )
  { minptr = 0;
    clear_key(res->key);
    clear_inf(res->inf);
    delete res;
    node_list = 0;
    return;
  }

  f_heap_node* r1 = minptr->right;
  f_heap_node* r2 = minptr->children;

  if ( r2 )
   {  // delete parent pointers for ring2
     while ( r2->parent )
      { r2->parent = 0;
        r2 = r2->right;
       }    

     // merging (min remains unchanged) 
     r2->left->right = r1;
     r1->left->right = r2;
     f_heap_node* help = r1->left;
     r1->left = r2->left;
     r2->left = help;
    }

  f_heap_node*  rank_array[64];
  f_heap_node** p = rank_array;
  f_heap_node** q = rank_array+max_rank()+1;

  while (p < q) *p++ = 0;

  f_heap_node* new_min = merge_heaps(rank_array);


  // remove old minimum
  minptr->left->right = minptr->right;
  minptr->right->left = minptr->left;

  // assign new minimum
  minptr = new_min;

  clear_key(res->key);
  clear_inf(res->inf);

  r1 = res->pred;
  r2 = res->next;

  if (r2) r2->pred = r1;

  if (r1) 
     r1->next  = r2;
  else
     node_list = r2;

  delete res;

}


void f_heap::clear()
{ f_heap_node* tail = node_list;

  if (tail==0) return;

  for(;;)
  { clear_key(tail->key);
    clear_inf(tail->inf);
    if (tail->next) 
       tail = tail->next;
    else 
       break;
   }

   std_memory.deallocate_list(node_list,tail,sizeof(f_heap_node));

   node_count = 0;
   minptr     = 0;
   node_list  = 0;
}

LEDA_END_NAMESPACE


