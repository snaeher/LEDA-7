/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _skiplist.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/impl/skiplist.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

atomic_counter skiplist_node::id_count(0);


// #define CHECK_INVARIANTS

inline int NODE_SIZE(int l)
{ 
  int l1 = 0;
  if ( l > 0 )  // compute smallest power of two >= l
  { l1 = 1; 
    while (l1 < l) l1 <<= 1; 
  }
  return int(sizeof(skiplist_node))+
              (l1)*int(sizeof(skiplist_node*));
}
  
 
#define NEW_NODE(v,l) \
v = (sl_item)std_memory.allocate_bytes(NODE_SIZE(l));\
v->height = l;\

#define FREE_NODE(v) \
std_memory.deallocate_bytes(v,NODE_SIZE(v->height))


inline int HEADER_SIZE()
{ int l1 = 1; 
  while (l1 < MaxHeight) l1 <<= 1; 
  return int(sizeof(header_node))+
            (l1)*int(sizeof(skiplist_node*));
}
  
 
#define NEW_HEADER(v) \
/*v = (large_item)std_memory.allocate_bytes(HEADER_SIZE());*/\
v = (large_item) new char[HEADER_SIZE()];\
v->height = MaxHeight;

#define FREE_HEADER(v) \
/* std_memory.deallocate_bytes(v,HEADER_SIZE()) */ \
delete[] (char*)v;


skiplist::skiplist(float p) 
{ prob = p;
  //fill_random_source();
  randomsLeft = 0;
#ifdef SMEM   
  header = new header_node;
  header->forward = new sl_item[MaxHeight+1];
  header->height = MaxHeight;
  STOP = new skiplist_node;
  STOP->height = -1;
#else
  NEW_HEADER(header);
  NEW_NODE(STOP,-1);
#endif
  header->true_height = 0;
  header->myseq = this;  
  STOP->backward= (sl_item) header;
  STOP->pred= (sl_item) header;
  header->forward[0] = STOP;
  owner = 0;
} 
 
skiplist::skiplist(const skiplist& L) 
{ prob = L.prob;
  //fill_random_source();
  randomsLeft = 0;
#ifdef SMEM   
  header = new header_node;
  header->forward = new sl_item[MaxHeight+1];
  header->height = MaxHeight;
  STOP = new skiplist_node;
  STOP->height = -1;
#else
  NEW_HEADER(header);
  NEW_NODE(STOP,-1);
#endif
  header->true_height = 0;
  header->myseq = this;
  STOP->backward= (sl_item) header;
  STOP->pred= (sl_item) header;

  header->forward[0] = STOP;
 
  sl_item p = L.STOP->pred;
  while (p!= L.header) 
  { insert_at_item(header,p->key,p->inf);
    L.copy_key(p->key);    
    L.copy_inf(p->inf);
    p = p->pred;
  }
  owner = 0;
} 

 
skiplist& skiplist::operator=(const skiplist& L) 
{ clear();
  sl_item p = L.STOP->pred;
  while (p!= L.header) 
  { insert_at_item(header,p->key,p->inf,leda::behind);
    p = p->pred;
   }
  return *this;
 } 
 
void skiplist::clear() 
{ sl_item p,q;
  p = header->forward[0];
  while(p!=STOP)
  { q = p->forward[0];
    clear_key(p->key);
    clear_inf(p->inf);
#ifdef SMEM
    delete[] p->forward;
    delete p;
#else
    FREE_NODE(p);
#endif
    p = q; 
   }
 header->true_height = 0;
 header->forward[0] = STOP;
 STOP->pred= (sl_item) header;
}
 
 
 
skiplist::~skiplist() 
{ clear();
#ifdef SMEM
  delete[] header->forward;
  delete header;
  delete STOP;
#else
  FREE_HEADER(header);
  FREE_NODE(STOP);
#endif
}

sl_item skiplist::search(sl_item v, int h, GenPtr key, int& l) const
{ 
  if (key_def_order()) {
    switch (key_type_id()) {
      case INT_TYPE_ID:    return int_search(v,h,key,l);
      case DOUBLE_TYPE_ID: return double_search(v,h,key,l);
    }
  }

  return gen_search(v,h,key,l);
}

sl_item skiplist::gen_search(sl_item v, int h, GenPtr key, int& l) const
{ sl_item p = v;
  sl_item q = p->forward[h];
  l = 0;

#ifdef CHECK_INVARIANTS
  assert(p->height == MaxHeight || cmp(key,p->key) > 0);
  assert(q->height < 0 || cmp(key,q->key) <= 0);
#endif
 
  if (q->height >= 0 && cmp(key,q->key) == 0)  return q;

  int k = h - 1;
  int c = -1;
  
  while (k >=0)
  { /* p->key < key < p->forward[k+1]->key and c = -1 */
    q = p->forward[k];
    while (k == q->height && (c = cmp(key,q->key)) > 0)
    { p = q;
      q = p->forward[k];
    }
    if (c == 0) break;
    k--;
  }
  l = k;
   

#ifdef CHECK_INVARIANTS
  p = q->pred;
  assert(p->height == MaxHeight || cmp(key,p->key) > 0);
  assert(q->height <  0 || cmp(key, q->key) <= 0);
  assert(l >= 0 && cmp(key,q->key) == 0 ||
  ( l < 0 && (q->height < 0 || cmp(key,q->key) < 0)));
#endif

  return q;
}


sl_item skiplist::int_search(sl_item v, 
                           int h, GenPtr key, int& l) const
{ sl_item p = v;
  sl_item q = p->forward[h];
  l = 0; 
  int ki = LEDA_ACCESS(int,key);         
  int k = h - 1;
  STOP->key = key;
  
  while (k >= 0)
  { /* p->key < key <= p->forward[k+1]->key */
    q = p->forward[k];
    while ( ki > LEDA_ACCESS(int,q->key) )
    { p = q;
      q = p->forward[k];
    }
    if ( ki == LEDA_ACCESS(int,q->key) && q != STOP ) break;
    k--;
  }
  l = k;
   

#ifdef CHECK_INVARIANTS
  p = q->pred;
  assert(p->height==MaxHeight || ki>LEDA_ACCESS(int,p->key));
  assert(q->height <  0 || ki <= LEDA_ACCESS(int,q->key));
  assert(l >= 0 && ki == LEDA_ACCESS(int,q->key) ||
  ( l < 0 && (q->height<0 || ki<LEDA_ACCESS(int,q->key))));
#endif

  return q;
}

sl_item skiplist::double_search(sl_item v, 
                           int h, GenPtr key, int& l) const
{ sl_item p = v;
  sl_item q = p->forward[h];
  l = 0;
  double ki = LEDA_ACCESS(double,key);
  int k = h - 1;
  STOP->key = key;
  
  while (k >= 0)
  { /* p->key < key <= p->forward[k+1]->key */
    q = p->forward[k];
    while ( ki > LEDA_ACCESS(double,q->key) )
    { p = q;
      q = p->forward[k];
    }
    if (ki==LEDA_ACCESS(double,q->key) && q!=STOP) break;
    k--;
  }
  l = k;
   

#ifdef CHECK_INVARIANTS
p = q->pred;
assert(p->height==MaxHeight||ki>LEDA_ACCESS(double,p->key));
assert(q->height <  0 || ki <= LEDA_ACCESS(double,q->key));
assert(l >= 0 && ki == LEDA_ACCESS(double,q->key) ||
   ( l<0 && (q->height<0||ki<LEDA_ACCESS(double,q->key))));
#endif

  return q;
}


sl_item skiplist::locate_succ(GenPtr key) const
{ int l;
  sl_item q = search(header,header->true_height,key,l);  
  return (q == STOP) ? 0 : q;
}

sl_item skiplist::locate(GenPtr key) const 
{ return locate_succ(key); }


sl_item skiplist::locate(GenPtr key, bool& equal) const 
{ int k;
  sl_item q = search(header,header->true_height,key,k);  
  if (q == STOP) q = 0;
  equal = (q && k >= 0);
  return q;
 }



sl_item skiplist::locate_pred(GenPtr key) const
{ int l;
  sl_item q = search(header,header->true_height,key,l);
  if (l < 0) q = q->pred;
  return (q == header) ? 0 : q;
}

 
sl_item skiplist::lookup(GenPtr key) const
{ int k;
  sl_item q = search(header,header->true_height,key,k);
  return (k < 0) ? 0 : q;
}



sl_item skiplist::finger_search_from_front( GenPtr key, int& l) const
{
  if (key_def_order()) {
    switch (key_type_id()) {
      case INT_TYPE_ID:    return int_finger_search_from_front(key,l);
      case DOUBLE_TYPE_ID: return double_finger_search_from_front(key,l);
    }
  }

  return gen_finger_search_from_front(key,l);
}

sl_item skiplist::gen_finger_search_from_front(GenPtr key,
                                              int& l) const
{ int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;

  while ( k < th )
  { if ( cmp(key,header->forward[k]->key) <= 0 ) break;
    k++;
  }
  return search(header,k,key,l);
}


sl_item skiplist::int_finger_search_from_front(GenPtr key, 
                                              int& l) const
{ int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  int ki = LEDA_ACCESS(int,key);

  while ( k < th )
  { if ( ki <= LEDA_ACCESS(int,
                          header->forward[k]->key) ) break;
    k++;
  }
  return search(header,k,key,l);
}

sl_item skiplist::double_finger_search_from_front(GenPtr key, 
                                              int& l) const
{ int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  double ki = LEDA_ACCESS(double,key);

  while ( k < th )
  { if ( ki <= LEDA_ACCESS(double,
                          header->forward[k]->key) ) break;
    k++;
  }
  return search(header,k,key,l);
}


sl_item skiplist::finger_search_from_rear(GenPtr key, int& l)const
{ 
  if (key_def_order()) {
    switch (key_type_id()) {
      case INT_TYPE_ID:    return int_finger_search_from_rear(key,l);
      case DOUBLE_TYPE_ID: return double_finger_search_from_rear(key,l);
    }
  }

  return gen_finger_search_from_rear(key,l);
}

sl_item skiplist::gen_finger_search_from_rear(GenPtr key,
                                              int& l) const
{ sl_item q = STOP->pred; 
  int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;

  while ( k < th )
  { if ( cmp(key, q->key) > 0 ) break;
    k++;
    while (k > q->height)  q = q->backward;
  }
  return search(q,k,key,l);
}


sl_item skiplist::int_finger_search_from_rear(GenPtr key, 
                                              int& l) const
{ sl_item q = STOP->pred; 
  int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  int ki = LEDA_ACCESS(int,key);

  while ( k < th )
  { if ( ki > LEDA_ACCESS(int,q->key) ) break;
    k++;
    while (k > q->height)  q = q->backward;
  }
  return search(q,k,key,l);
}

sl_item skiplist::double_finger_search_from_rear(GenPtr key, 
                                              int& l) const
{ sl_item q = STOP->pred; 
  int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  double ki = LEDA_ACCESS(double,key);

  while ( k < th )
  { if ( ki > LEDA_ACCESS(double,q->key) ) break;
    k++;
    while (k > q->height)  q = q->backward;
  }
  return search(q,k,key,l);
}


sl_item skiplist::finger_search(GenPtr key, int& l)const
{ 
  if (key_def_order()) {
    switch (key_type_id()) {
     case INT_TYPE_ID:    return int_finger_search(key,l);
     case DOUBLE_TYPE_ID: return double_finger_search(key,l);
    }
  }
  return gen_finger_search(key,l);
}

sl_item skiplist::gen_finger_search(GenPtr key,
                                              int& l) const
{ sl_item q = STOP->pred; 
  int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  int c1 = 0;

  while ( k < th )
  { c1 = cmp(key,header->forward[k]->key);
    int c2 = cmp(key, q->key);
    if ( c1 <= 0 || c2 > 0 ) break;

    k++;

    while (k > q->height)  q = q->backward;
  }
  if (c1 <= 0) 
    //return search(header,k,key,l);
    return search(header,k+1,key,l); // k+1 (s.n.)
  else
    return search(q,k,key,l);
}

sl_item skiplist::int_finger_search(GenPtr key, 
                                              int& l) const
{ sl_item q = STOP->pred; 
  int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  int ki = LEDA_ACCESS(int,key);

  while ( k < th )
  { if ( ki <= LEDA_ACCESS(int,
                          header->forward[k]->key) ) break;
    if ( ki > LEDA_ACCESS(int,q->key) ) break;

    k++;

    while (k > q->height)  q = q->backward;
  }

  if ( ki <= LEDA_ACCESS(int,header->forward[k]->key) )
    //return search(header,k,key,l);
    return search(header,k+1,key,l); // k+1 (s.n.)
  else 
    return search(q,k,key,l);
}

sl_item skiplist::double_finger_search(GenPtr key, 
                                             int& l) const
{ sl_item q = STOP->pred; 
  int th = header->true_height;
  if (th == -1) return STOP;
  l = 0; 
  int k = 0;
  double ki = LEDA_ACCESS(double,key);

  while ( k < th )
  { if ( ki <= LEDA_ACCESS(double,
                         header->forward[k]->key) ) break;
    if ( ki > LEDA_ACCESS(double,q->key) ) break;

    k++;

    while (k > q->height)  q = q->backward;
  }
  if ( ki <= LEDA_ACCESS(double,header->forward[k]->key) )
    //return search(header,k,key,l);
    return search(header,k+1,key,l); // k+1 (s.n.)
  else 
    return search(q,k,key,l);
}


sl_item skiplist::finger_search(sl_item v, GenPtr key, int& l) const
{ 
  if (key_def_order()) {
    switch (key_type_id()) {
      case INT_TYPE_ID:    return int_finger_search(v,key,l);
      case DOUBLE_TYPE_ID: return double_finger_search(v,key,l);
    }
  }

  return gen_finger_search(v,key,l);
}


sl_item skiplist::gen_finger_search(sl_item v, 
                                  GenPtr key, int& l) const
{ l = 0;
  sl_item p = v;

  if ( p->height < 0 ) p = p->backward;
  // if p was a STOP node then it is a header now
  if ( p->height == MaxHeight ) 
    return ((large_item) p)->myseq->finger_search(key,l);

  int dir = cmp(key, v->key);
  if  ( dir == 0 ) return v;

  int k = 0;
  int c ;

  if (dir > 0)
  { while ( p->height < MaxHeight && 
            p->forward[k]->height  >= 0 && 
            (c = cmp(key,p->forward[k]->key )) >= 0 )
    { if ( c == 0 ) return p->forward[k];
      k++;
      while ( k > p->height ) p = p->backward;
    }
    if ( p->height == MaxHeight ) 
      return ((large_item)p)->myseq->finger_search(key,l);
  }
  else 
  { while ( p->height < MaxHeight && 
            p->forward[k]->height >= 0 && 
            (c = cmp(key, p->key)) <= 0 )
    { if ( c == 0 )  return p;
      k = p->height;
      p = p->backward;
    }
    if (p->forward[k]->height  < 0 ) 
    { p = p->forward[k]->backward;
      return ((large_item)p)->myseq->finger_search(key,l);
    }
  }

#ifdef CHECK_INVARIANTS
assert(p->height == MaxHeight || cmp(key, p->key) > 0);
assert(p->forward[k]->height < 0 || 
             cmp(key, p->forward[k]->key) < 0);
#endif

  return search(p,k,key,l);
}

sl_item skiplist::int_finger_search(sl_item v, 
GenPtr key, int& l) const
{ l = 0;
  sl_item p = v;
  int ki = LEDA_ACCESS(int,key);

  if ( p->height < 0 ) p = p->backward;
  if ( p->height == MaxHeight ) 
    return ((large_item) p)->myseq->finger_search(key,l);

  if  ( ki == LEDA_ACCESS(int,v->key) ) return v;

  int k = 0;
        
  if ( ki >  LEDA_ACCESS(int,v->key) )
  { while ( p->height < MaxHeight && 
            p->forward[k]->height >= 0 && 
            ki >= LEDA_ACCESS(int,p->forward[k]->key))
    { if ( ki == LEDA_ACCESS(int,p->forward[k]->key)) 
                                   return p->forward[k];
      k++;
      while (k > p->height) p = p->backward;
    }
    if (p->height == MaxHeight) 
      return ((large_item)p)->myseq->finger_search(key,l);
  }
  else 
  { while ( p->height < MaxHeight &&
            p->forward[k]->height >= 0 && 
            ( ki <= LEDA_ACCESS(int,p->key)) )
    { if ( ki == LEDA_ACCESS(int,p->key) )  return p;
      k = p->height;
      p = p->backward;
    }
    if (p->forward[k]->height  < 0 ) 
    { p = p->forward[k]->backward;
      return ((large_item)p)->myseq->finger_search(key,l);
    }
  }

#ifdef CHECK_INVARIANTS
assert(p->height == MaxHeight || ki > LEDA_ACCESS(int,p->key));
assert(p->forward[k]->height < 0 || 
                     ki < LEDA_ACCESS(int,p->forward[k]->key));
#endif

  return search(p,k,key,l);
}


sl_item skiplist::double_finger_search(sl_item v, 
GenPtr key, int& l) const
{ l = 0;
  sl_item p = v;
  double ki = LEDA_ACCESS(double,key);

  if ( p->height < 0 ) p = p->backward;
  if ( p->height == MaxHeight ) 
    return ((large_item) p)->myseq->finger_search(key,l);

  if  ( ki == LEDA_ACCESS(double,v->key) ) return v;

  int k = 0;
        
  if ( ki >  LEDA_ACCESS(double,v->key) )
  { while ( p->height < MaxHeight && 
            p->forward[k]->height >= 0 && 
            ki >= LEDA_ACCESS(double,p->forward[k]->key)) 
    { if ( ki == LEDA_ACCESS(double,p->forward[k]->key)) 
        return p->forward[k];
      k++;
      while (k > p->height) p = p->backward;
    }
    if (p->height == MaxHeight) 
      return  ((large_item) p)->myseq->finger_search(key,l);
  }
  else 
  { while ( p->height < MaxHeight && 
            p->forward[k]->height >= 0 && 
            ki <= LEDA_ACCESS(double,p->key) )
    { if ( ki == LEDA_ACCESS(double,p->key) )  return p;
      k = p->height;
      p = p->backward;
    }
    if (p->forward[k]->height  < 0 ) 
    { p = p->forward[k]->backward;
      return ((large_item) p)->myseq->finger_search(key,l);
    }
  }

#ifdef CHECK_INVARIANTS
assert(p->height == MaxHeight || 
                          ki > LEDA_ACCESS(double,p->key));
assert(p->forward[k]->height < 0 || 
              ki < LEDA_ACCESS(double,p->forward[k]->key));
#endif

  return search(p,k,key,l);
}


sl_item skiplist::finger_locate_succ(sl_item v, 
                                         GenPtr key) const
{ int l;
  sl_item q = finger_search(v,key,l);  
  return (q->height < 0) ? 0 : q;
}

sl_item skiplist::finger_locate_succ(GenPtr key) const
{ int l;
  sl_item q = finger_search(key,l);  
  return (q==STOP) ? 0 : q;
}


sl_item skiplist::finger_locate_pred(sl_item v, 
                                         GenPtr key) const
{ int l;
  sl_item q = finger_search(v,key,l);
  if (l < 0) q = q->pred;
  return (q->height == MaxHeight) ? 0 : q;
}

 
sl_item skiplist::finger_locate_pred(GenPtr key) const
{ int l;
  sl_item q = finger_search(key,l);
  if (l < 0) q = q->pred;
  return (q==header) ? 0 : q;
}

 
sl_item skiplist::finger_lookup(sl_item v, 
                                         GenPtr key) const
{ int l;
  sl_item q = finger_search(v,key,l);
  return (l<0) ? 0 : q;
}

sl_item skiplist::finger_lookup(GenPtr key) const
{ int l;
  sl_item q = finger_search(key,l);
  return (l<0) ? 0 : q;
}

sl_item skiplist::finger_locate_succ_from_front(GenPtr key) const
{ int l;
  sl_item q = finger_search_from_front(key,l);  
  return (q==STOP) ? 0 : q;
}

sl_item skiplist::finger_locate_pred_from_front(GenPtr key) const
{ int l;
  sl_item q = finger_search_from_front(key,l);
  if (l < 0) q = q->pred;
  return (q==header) ? 0 : q;
}

sl_item skiplist::finger_lookup_from_front(GenPtr key) const
{ int l;
  sl_item q = finger_search_from_front(key,l);
  return (l<0) ? 0 : q;
}

sl_item skiplist::finger_locate_succ_from_rear(GenPtr key) const
{ int l;
  sl_item q = finger_search_from_rear(key,l);  
  return (q==STOP) ? 0 : q;
}

sl_item skiplist::finger_locate_pred_from_rear(GenPtr key) const
{ int l;
  sl_item q = finger_search_from_rear(key,l);
  if (l < 0) q = q->pred;
  return (q==header) ? 0 : q;
}

sl_item skiplist::finger_lookup_from_rear(GenPtr key) const
{ int l;
  sl_item q = finger_search_from_rear(key,l);
  return (l<0) ? 0 : q;
}


void skiplist::insert_item_at_item(sl_item q, 
                                   sl_item p, int dir)
{ int k = q->height;
  if (dir == leda::before) p = p->pred;

  /* insert item |q| immediately after item |p| */

  sl_item x;
  q->pred = p;
  p->forward[0]->pred = q;
  for (k = 0; k <= q->height; k++ )
  { while (k > p->height) p = p->backward;
    x = p->forward[k];
    if (p->height == MaxHeight && x->height < 0 )
    {/* we have reached header and STOP and need to 
           increase true_height */
     ((large_item) p)->true_height = k + 1;
     p->forward[k+1] = x;
     
    }
    q->forward[k] = x;
    p->forward[k] = q;
    if ( x->height == k ) x->backward = q;
   }
   q->backward = p;
}


sl_item skiplist::insert_at_item(sl_item p, 
                                     GenPtr key, GenPtr inf)
{ sl_item q;
  if (p->height < 0) p = p->pred;
  else
  { if ( p->height < MaxHeight )
    { int c = cmp(key,p->key);

      if (c == 0)
      { clear_inf(p->inf);
        copy_inf(inf);
        p->inf = inf;
        return p;
      }

      if ( c<0 ) p = p->pred;
    }
  }
 
  int k = randomLevel();
  if ( k >= MaxHeight ) k = MaxHeight - 1;
 
#ifdef SMEM
  q = new skiplist_node;
  q->forward = new sl_item[k+1];
  q->height = k;
#else
  NEW_NODE(q,k);
#endif
  copy_key(key);
  copy_inf(inf);
  q->key = key;
  q->inf = inf;
  q->id = skiplist_node::id_count++;
  insert_item_at_item(q,p,leda::behind);
  return q;
}
 
int skiplist::randomLevel()
{ int height = 0;
  int b = 0;

  if ( prob == 0.25 )  
  { while ( b == 0 )
    { if (randomsLeft < 2) fill_random_source();
      b = randomBits&3;    // read next two random bits
      randomBits >>= 2;
      randomsLeft -= 2;
      if ( b == 0 ) height++; // increase height with prob 0.25
     }
  }
  else            // user defined prob.
  { double p;
    //rand_int >> p;
    ran >> p;
    while ( p < prob ) 
    { height++;
      ran >> p;
    }
  }
  return height;
}


sl_item skiplist::insert_at_item(sl_item p, 
GenPtr key, GenPtr inf, int dir)
{ sl_item q;
  int k = randomLevel();
#ifdef SMEM
  q = new skiplist_node;
  q->forward = new sl_item[k+1];
  q->height = k;
#else
  NEW_NODE(q,k);
#endif
  copy_key(key);
  copy_inf(inf);
  q->key = key;
  q->inf = inf;
  q->id = skiplist_node::id_count++;
  insert_item_at_item(q,p,dir);
  return q;
}
 

void skiplist::remove_item(sl_item q)
{ 
  if (q->height == MaxHeight || q->height < 0)
    LEDA_EXCEPTION(1,"cannot remove improper item");

  sl_item p = q->backward;
  sl_item x = 0;

  for(int k = q->height; k >= 0; k--)
  { while ( p->forward[k] != q ) p = p->forward[k];
    x = q->forward[k];
    p->forward[k] = x;
    if ( x->height == k ) x->backward = p;
   }
  x->pred = p;
}
 
 
void skiplist::del_item(sl_item q)
{ 
  if (q->height == MaxHeight || q->height < 0)
    LEDA_EXCEPTION(1,"cannot delete improper item");
  remove_item(q);
  clear_key(q->key);
  clear_inf(q->inf);
  sl_item p = q->forward[q->height];
#ifdef SMEM
  delete[] q->forward;
  delete q;
#else
  FREE_NODE(q);
#endif
  if ( p->height < 0 )
  { large_item r = (large_item) p->backward;     
    int& h = r->true_height;
    while( h > 0 && r->forward[h - 1] == p) h--;
  }
}

sl_item skiplist::insert(GenPtr key, GenPtr inf)
{ int k;
  sl_item p = search(header,header->true_height,key,k);
  if ( k >= 0 )
  { clear_inf(p->inf);
    copy_inf(inf);
    p->inf  = inf;
    return p;
  }
  p = insert_at_item(p,key,inf,leda::before);
  return p;
}

void skiplist::del(GenPtr key)
{ int k;
  sl_item q = search(header,header->true_height,key,k);
  if ( k>=0 ) del_item(q);
}


void skiplist::reverse_items(sl_item p, sl_item q)
{ sl_item r;
  while ( p != q )
  { r = p;
    p = p->forward[0];
    remove_item(r);
    insert_item_at_item(r,q,leda::behind);
  }
}

void skiplist::conc(skiplist& S1, int dir)
{ if (header->true_height < S1.header->true_height)
  { leda_swap(header->myseq,S1.header->myseq);
    leda_swap(header,S1.header);
    leda_swap(STOP,S1.STOP);
 
    dir = ((dir == leda::behind) ? leda::before : leda::behind);
  }

  if (S1.STOP->pred == S1.header)  return;

  /* S1 is non-empty and since height >= S1.height this is
     also non-empty */

  if (dir == leda::behind)
  { sl_item p = STOP->pred;
    sl_item q = S1.STOP->pred;
 
    assert(cmp(p->key, S1.header->forward[0]->key) < 0);
   
    STOP->pred = q;
    S1.header->forward[0]->pred = p;

    for (int k = 0; k < S1.header->true_height; k++)
    { /* p and q are the rightmost items of height at 
         least k in this and S1, respectively */
      sl_item r = S1.header->forward[k];
      p->forward[k] = r; 
      if ( r->height == k ) r->backward = p;
         q->forward[k] = STOP;
      while (p->height == k) p = p->backward;
      while (q->height == k) q = q->backward;
    }
  }
  else
  { sl_item q = S1.STOP->pred;

    assert(cmp(q->key, header->forward[0]->key) < 0);

    S1.header->forward[0]->pred= (sl_item) header;
    header->forward[0]->pred = q;

    for (int k = 0; k < S1.header->true_height; k++)
    { // q is the rightmost item of height at least k in S1  
      sl_item r = header->forward[k];
      q->forward[k] = r;
      if (r->height == k) r->backward = q;
      r = S1.header->forward[k];
      header->forward[k] = r; 
      if (r->height == k) r->backward= (sl_item) header;
      while (q->height == k) q = q->backward;
    }
  }

  S1.header->true_height = 0;
  S1.STOP->pred = (sl_item) S1.header;
  S1.header->forward[0] = S1.STOP;

#ifdef CHECK_INVARIANTS
  this->check_data_structure("this in conc");
  check_data_structure(S1,"S1 in conc");
#endif

}


void skiplist::split_at_item(sl_item p,skiplist& S1,
                                     skiplist& S2,int dir)
{ if (dir == leda::before) p = p->pred;

  sl_item p1 = p;
  sl_item p2 = p->forward[0];
  int max_lev = -1;

  while ( p1->height < MaxHeight && p2->height >= 0 )
  { /* p1 and p2 are proper towers of height 
       larger than max_lev    */
    max_lev++;
    while (p1->height == max_lev) p1 = p1->backward;
    while (p2->height == max_lev) p2 = p2->forward[max_lev];
  }

  /* we have seen proper towers of height max_lev on both 
     sides of the split and either p1 or p2 is a sentinel */

  large_item pheader;

  if (p1->height == MaxHeight) 
    pheader = (large_item) p1;
  else 
    pheader = (large_item) p2->backward;
 
  skiplist* Pp = pheader->myseq;

  if (Pp != &S1)  S1.clear(); 
  if (Pp != &S2)  S2.clear();

  if (p1->height == MaxHeight)
  { /* we reuse pheader and pSTOP for S2 */

    if (Pp != &S2)
    { leda_swap(Pp->header->myseq, S2.header->myseq);
      leda_swap(Pp->header,S2.header);
      leda_swap(Pp->STOP,S2.STOP);
 
    }
    S1.header->true_height = 1+max_lev; 

    p1 = p;

    for (int k =0; k <= max_lev; k++)
    { // p1 is the rightmost item in S1 of height at least k
 
      sl_item q = S2.header->forward[k];
      S1.header->forward[k] = q;
      if (q->height == k) q->backward = (sl_item) S1.header;
      S2.header->forward[k] = p1->forward[k];
      if (p1->forward[k]->height == k)
         p1->forward[k]->backward = (sl_item) S2.header;
      p1->forward[k] = S1.STOP;
      while (k == p1->height) p1 = p1->backward;
    }

    S1.header->forward[max_lev + 1] = S1.STOP;
    /* the next line sets the predecessor of S1.STOP 
       correctly if S1 is non-empty; if it is empty 
       the last line corrects the mistake */
    S1.STOP->pred = p;
    S2.header->forward[0]->pred = (sl_item) S2.header;
    S1.header->forward[0]->pred = (sl_item) S1.header;
  }
  else
  { /* we want to reuse pheader and pSTOP for S1 */
    if (Pp != &S1)
    { leda_swap(Pp->header->myseq,S1.header->myseq);
      leda_swap(Pp->header,S1.header);
      leda_swap(Pp->STOP,S1.STOP);
    }
    S2.header->true_height = 1 + max_lev;

    p1 = p;
    p2 = S1.STOP->pred;

    for (int k =0; k <= max_lev; k++)
    { /* p1 and p2 are the rightmost items in S1 and S2 
         of height at least k, respectively */ 

      sl_item q = p1->forward[k];
      S2.header->forward[k] = q;
      if (q->height == k) q->backward = (sl_item) S2.header;
      p1->forward[k] = S1.STOP;
      p2->forward[k] = S2.STOP;
      while (k == p1->height) p1 = p1->backward;
      while (k == p2->height) p2 = p2->backward;
    }

    S2.header->forward[max_lev + 1] = S2.STOP;
    /* the next line sets the predecessor of S2.STOP 
       correctly if S2 is non-empty; if it is empty then 
       the next line corrects the mistake */
    S2.STOP->pred = S1.STOP->pred;
    S2.header->forward[0]->pred = (sl_item) S2.header;

    S1.STOP->pred = p;
    S1.header->forward[0]->pred = (sl_item) S1.header;
  }

  if (Pp != &S1 && Pp != &S2)
  { /* P is empty if distinct from S1 and S2 */
    Pp->header->forward[0] = Pp->STOP;
    Pp->STOP->pred = Pp->STOP->backward = 
                                 (sl_item) Pp->header;
    Pp->header->true_height = 0;
  }


#ifdef CHECK_INVARIANTS
  this->check_data_structure("this in split");
  Pp->check_data_structure("P in split");
  check_data_structure(S1,"S1 in split");
  check_data_structure(S2,"S2 in split");
#endif
}


void skiplist::merge(skiplist& S1)
{ sl_item p= (sl_item) header;
  sl_item q = S1.header;
  while ( p->height  >= 0 && q->height >= 0 )
  { p = p->forward[0];
    q = q->forward[0];
  }

  if (q->height >= 0)  
  { /* swap if this is shorter than S1 */
    leda_swap(header->myseq,S1.header->myseq);
    leda_swap(header,S1.header);
    leda_swap(STOP,S1.STOP);
  }

  /* now S1 is at most as long as this */
  sl_item finger= (sl_item) header;
  p = S1.header->forward[0];

  while (p->height >= 0)
  { sl_item q = p->forward[0];
    int l;
    finger = finger_search(finger,p->key,l);
    if (l >= 0) LEDA_EXCEPTION(1,"equal keys in merge");
    insert_item_at_item(p,finger,leda::before);
    finger = p; // put finger at newly inserted item
    p = q;
  }


  S1.header->true_height = 0;
  S1.STOP->pred = (sl_item) S1.header;
  S1.header->forward[0] = S1.STOP;
#ifdef CHECK_INVARIANTS
  check_data_structure("this in merge");
  S1.check_data_structure("S1 in merge");
#endif
}


void skiplist::delete_subsequence(sl_item a,
  sl_item b,skiplist& S1)
{ S1.clear();
  sl_item p1 = a->pred;
  sl_item p2 = b;
  sl_item p3 = b->forward[0];
  int k = -1;

  while ( p1->height < MaxHeight && p3->height >= 0 && 
          p2->height < MaxHeight &&
                            cmp(p2->key,a->key) >= 0 )
  { k++;
    while ( p1->height == k)  p1 = p1->backward;
    while ( p2->height == k)  p2 = p2->backward;
    while ( p3->height == k)  p3 = p3->forward[k];
  }

  if (p1->height == MaxHeight || p3->height  < 0)
  { if (p1->height < MaxHeight) p1 = p3->backward;
    skiplist* Pp = ((large_item) p1)->myseq;
    skiplist S2,S3;
 
    split_at_item(b,S2,S3,leda::behind);
    split_at_item(a,*Pp,S1,leda::before);  

    Pp->conc(S3,leda::behind);

    return;
  }

  // the middle list is the lowest and we have to do some work

  p1 = a->pred;
  p2 = b;

  /* correct predecessor pointers */

  a->pred = (sl_item) S1.header;
  S1.STOP->pred = b;
  b->forward[0]->pred = p1;

  /* height of S1 */

  S1.header->true_height = 1 + k;
  S1.header->forward[1+k] = S1.STOP;

  for (int i = 0; i <= k; i++)
  { /* p1 and p2 are the rightmost items of height at least 
       i in the first and second part, respectively */

    sl_item q = p1->forward[i];
    S1.header->forward[i] = q;
    if (q->height == i) q->backward = S1.header;
    q = p2->forward[i];
    p1->forward[i] = q;
    if (q->height == i) q->backward = p1;
    p2->forward[i] = S1.STOP;

    while (i == p1->height)  p1 = p1->backward;
    while (i == p2->height)  p2 = p2->backward;
  }
}


void skiplist::validate_data_structure()
{ assert(header == header->myseq->header); 
  assert (header->height == MaxHeight);
  assert(STOP->height == -1);

  int max_proper_height = -1;
  sl_item p = (sl_item) header;

  while (p != STOP)
  { assert(p->height >= 0);
    if (p != header && p->height > max_proper_height) 
                             max_proper_height = p->height;
    p = p->forward[0];
  }
  assert(header->true_height == max_proper_height + 1);

  p = (sl_item) header;
  while (p != STOP)
  { sl_item q = p->forward[0];
    assert(p == q->pred);                   //condition three

    if (p != header && q != STOP)           //check order
                            assert(cmp(p->key,q->key) < 0);
  
    for(int h=0; h<=leda_min(p->height,header->true_height);h++)
    { sl_item r = p->forward[0];          
      while (r->height < h && r != STOP) r = r->forward[0];
      assert ( r == p->forward[h]);         //condition one
                               
      if ( h == r->height ) assert(r->backward == p);
    }                                       //condition two
    p = q;
  }
                   
  assert(STOP->backward == (sl_item) header);
}


int skiplist::size() const
{ int count = 0;
  sl_item p;
  p = (sl_item) header;
  while ((p = p->forward[0]) != STOP)  count++ ;
  return count;
}

void skiplist::print(const skiplist & S1,ostream& out,
string s, char space) const
{ 
  sl_item p= ((sl_item) S1.header)->forward[0];
  out << s;
  while (p != S1.STOP)
  {  out << string(space);
     print_key(out, p->key);
     p = p->forward[0];
  }
  out.flush();
}


void skiplist::check_data_structure(const skiplist& S1, string s)
{ string t = "";

  if (S1.header != S1.header->myseq->header) 
    {t =t + "error in myseq";}

  int h = -1;
  sl_item p= (sl_item) S1.header;
  while (p != S1.STOP)
  { sl_item q = p->forward[0];
    if (p != q->pred)  t =t + "error in underlying list";
          
    if (p != S1.header && p->height > h) h = p->height;
    if (p != S1.header && q != S1.STOP && 
                         cmp(p->key,q->key) >= 0)
            { t = t + "error in order";
      print(S1,cout,"wrong order",' '); 
      cout << endl;
                 }
    if (p != S1.STOP)
    { for (int i = 0; i <= leda_min(p->height,
                           S1.header->true_height); i++)
      { sl_item r = p->forward[0];
        while (r->height < i && r != S1.STOP) 
                                      r = r->forward[0];
        if ( r != p->forward[i])  
                             t = t + "error in forward";
        if ( i == r->height && r->backward != p)
                            t = t + "error in backward";
      }
    }
    p = q;
  }
  if (S1.header->true_height < h +1)  
                            t = t + "height too small";
                   
  if (S1.header->true_height > h +1)  
                             t = t + "height too large";
                    
  if (S1.STOP->backward != (sl_item) S1.header)  
                               t = t + "error in stop";
                      
  string t1 = "";
  if (t != t1) 
     { cout << s << "   " << t; cout << endl; cout.flush();}
}


skiplist* skiplist::my_collection(sl_item p)
{
  if (p == nil) return nil;

  while ( p->height < MaxHeight ) p = p->backward;

  // p is the header and hence a large item
  return ((large_item)p)->myseq;
}

LEDA_END_NAMESPACE
