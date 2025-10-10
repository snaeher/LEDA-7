/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  checked_p_queue_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_CHECKED_P_QUEUE_H
#define LEDA_CHECKED_P_QUEUE_H

#include <LEDA/core/p_queue.h>
#include <LEDA/core/list.h>
#include <LEDA/core/partition.h>
#include <LEDA/core/tuple.h> 
#include <assert.h>

using namespace leda;


template <class P, class I, class impl> 
class checked_p_queue : public p_queue<P,I,impl>
{
  typedef typename p_queue<P,I,impl>::item pq_item;
  typedef  four_tuple<P,I,pq_item,partition_item> check_object;
  list<check_object> L;
  typedef list_item L_item;

  typedef two_tuple<P,L_item> step_object;

  list<step_object> S;
  typedef list_item S_item;

  Partition<S_item> Part;

  int phase_length, op_count;

  p_queue<P,L_item,impl>* PQ;

  
  P& prio(L_item l_it)                       { return L[l_it].first(); }
  const P&  prio(L_item l_it) const          { return L[l_it].first(); }
  I& inf(L_item l_it)                        { return L[l_it].second(); }
  const I&  inf(L_item l_it) const           { return L[l_it].second(); }
  pq_item& pq_it(L_item l_it)                { return L[l_it].third(); }
  pq_item  pq_it(L_item l_it) const          { return L[l_it].third(); }
  partition_item& part_it(L_item l_it)       { return L[l_it].fourth(); }
  partition_item  part_it(L_item l_it) const { return L[l_it].fourth(); }

  P& prio_of_S_item(S_item s_it)             { return S[s_it].first(); }
  P  prio_of_S_item(S_item s_it) const       { return S[s_it].first(); }
  L_item& L_it(S_item s_it)                  { return S[s_it].second(); }
  L_item  L_it(S_item s_it)  const           { return S[s_it].second(); }


  partition_item canonical_part_it(L_item l_it) const
  { return ((Partition<S_item>*) &Part)->find(part_it(l_it)); }

  S_item canonical_inf(L_item l_it) const
  { return ((Partition<S_item>*) &Part)->inf(canonical_part_it(l_it)); } 

  bool is_unrestricted(L_item l_it) const
  { return canonical_inf(l_it) == nil; }

  bool is_restricted(L_item l_it) const 
  { return ! is_unrestricted(l_it); }

  L_item  last_item_in_step(L_item l_it) const
  { if ( is_restricted(l_it) )
        return L_it(canonical_inf(l_it));
    return L.last();
  }

  bool is_last_item_in_step(L_item l_it) const
  { return ( last_item_in_step(l_it) == l_it) ; }

  bool is_only_item_in_step(L_item l_it) const
  { return (is_last_item_in_step(l_it) &&
     ( L.pred(l_it) == nil || is_last_item_in_step(L.pred(l_it))));
  }


  void validate_data_structure() const
  { 
  #ifdef VALIDATE_DATA_STRUCTURE
    assert( PQ->size() == L.size() );
   
    L_item l_it;
    forall_items(l_it,L)
      { assert( pq_it(l_it) != nil ) ;
        assert( PQ->inf(pq_it(l_it)) == l_it );
      }

    l_it = L.first();
    S_item s_it = S.first();
    while (s_it)
      { assert(canonical_inf(l_it) == s_it);
        while (l_it != L_it(s_it) )
          { l_it = L.succ(l_it);
            assert(l_it != nil); 
            assert(canonical_inf(l_it) == s_it);
          }
        s_it = S.succ(s_it);
        l_it = L.succ(l_it);
      }
    while (l_it)
      { assert(canonical_inf(l_it) == nil);
        l_it = L.succ(l_it);
      }
  #endif
  }


  void check_lower_bound(L_item l_it) const
  { assert(is_unrestricted(l_it) ||  
           compare(prio_of_S_item(canonical_inf(l_it)), prio(l_it)) <= 0 );
  }


  void periodic_check()
  { if ( ++op_count == phase_length )
    { L_item l_it;
      forall_items(l_it,L) check_lower_bound(l_it);
      phase_length = 2*phase_length;
      op_count = 0;
    }
  }


  void update_lower_bounds(P p)
  { if ( L.empty() ||
         ( !S.empty() && compare(prio_of_S_item(S.last()),p) >= 0
           && L_it(S.last()) == L.last()))  return;
    S_item s_it;
    while ( !S.empty()  && 
            compare(prio_of_S_item(s_it = S.last()),p) <= 0 )
      { L_item l_it = L_it(s_it);
        if ( L.succ(l_it) )
          Part.union_blocks(part_it(l_it),part_it(L.succ(l_it)));
        S.pop_back();
      }
    Part.change_inf(canonical_part_it(L.last()),
          S.append(step_object(p,L.last())));
  }
       

  /* the default copy constructor and assignment operator work 
     incorrectly, we make them unaccessible by 
     declaring them private                                    */
  checked_p_queue(const checked_p_queue<P,I,impl>& Q);
  checked_p_queue<P,I,impl>& operator=(const checked_p_queue<P,I,impl>& Q);

public:

  checked_p_queue(p_queue<P,L_item,impl>& PQ_ext) // constructor
  { PQ = &PQ_ext;
    assert(PQ->empty());
    phase_length = 4; op_count = 0;
  }

  
  pq_item insert(const P& p, const I& i) 
  { pq_item p_it = PQ->insert(p,(L_item) 0);
    L_item last_l_it = L.last(); // last item in old list

    partition_item pa_it = Part.make_block((S_item) 0);
    list_item l_it = L.append(check_object(p,i,p_it,pa_it));
    PQ->change_inf(p_it,l_it);
    if (last_l_it && is_unrestricted(last_l_it) )
        Part.union_blocks(part_it(l_it),part_it(last_l_it));

    periodic_check();
    validate_data_structure();

    return (pq_item) l_it;
  }


  pq_item find_min() const 
  { L_item l_it = PQ->inf(PQ->find_min());

    check_lower_bound(l_it); 
    ((checked_p_queue<P,I,impl>*)this)->update_lower_bounds(prio(l_it));

    ((checked_p_queue<P,I,impl>*)this)->periodic_check();
    validate_data_structure();

    return (pq_item) l_it;
  }


  void del_item(pq_item p_it) 
  { L_item l_it = (L_item) p_it;

    check_lower_bound(l_it);

    if ( is_restricted(l_it) )
    { if ( is_only_item_in_step(l_it) ) 
        S.del_item(canonical_inf(l_it));
      else if (is_last_item_in_step(l_it) )
              L_it(canonical_inf(l_it)) = L.pred(l_it);
    }

    PQ->del_item(pq_it(l_it));
    L.del_item(l_it);

    periodic_check();
    validate_data_structure();
  }


  P del_min() 
  { L_item l_it = PQ->inf(PQ->find_min());

    P p = prio(l_it);
    del_item((pq_item)l_it);
    update_lower_bounds(p);

    periodic_check();
    validate_data_structure();

    return p;
  }


  const P& prio(pq_item it) const    
  { ((checked_p_queue<P,I,impl>*)this) -> periodic_check(); 
    return prio((L_item) it); 
  }

  const I& inf(pq_item it) const  
  { ((checked_p_queue<P,I,impl>*)this) -> periodic_check();
    return inf((L_item) it);  
  }

  void change_inf(pq_item it, const I& i)
  { periodic_check();
    inf((L_item) it) = i ; 
  }

  int  size() const 
  { ((checked_p_queue<P,I,impl>*)this) -> periodic_check();
    return L.size(); 
  }

  bool empty() const 
  { ((checked_p_queue<P,I,impl>*)this) -> periodic_check();
    return L.empty(); 
  }


  void decrease_p(pq_item p_it, const P& p)
  { L_item l_it = (L_item) p_it;
    check_lower_bound(l_it);
    assert( compare(p,prio(l_it)) <= 0 );
    prio(l_it) = p;
      
    PQ->decrease_p(pq_it(l_it),p);

    if ( is_restricted(l_it) )
    { if ( is_only_item_in_step(l_it) ) S.del_item(canonical_inf(l_it));
      else if (is_last_item_in_step(l_it) )
              L_it(canonical_inf(l_it)) = L.pred(l_it);
    
      list<check_object> L1, L_it;
      L.split(l_it,L,L1,leda::before);
      L1.split(l_it,L_it,L1,leda::after);
      L.conc(L1);
      list_item last_it = L.last();
      L.conc(L_it);

      part_it(l_it) = Part.make_block((S_item) 0);

      if (last_it && is_unrestricted(last_it) )
        Part.union_blocks(part_it(l_it),part_it(last_it));
    }
    periodic_check();
    validate_data_structure();
  }


  void clear() 
  { L_item l_it;
    forall_items(l_it,L) check_lower_bound(l_it);
    PQ->clear(); L.clear(); S.clear(); Part.clear(); 
  }

  ~checked_p_queue() { clear(); }


};

#endif

#include <LEDA/core/p_queue.h>
#include <LEDA/core/impl/bin_heap.h>
#include <LEDA/core/random.h>
#include <LEDA/core/array.h>
#include <LEDA/core/IO_interface.h>


int main(){


IO_interface I("Checked Priority Queues");

I.write_demo("This demo compares the speed of checked priority queues \
with the speed of unchecked priority queues. It generates n random \
doubles and sorts them twice, once using an unchecked queue and once using \
a checked queue.");

int n = I.read_int("n = ",100000);


array<double> A(n);

random_source S;

for (int i = 0; i < n; i++) S >> A[i];

float T = used_time();
{ p_queue<double,int,bin_heap> PQ(n);
  for (int i = 0; i < n; i++) PQ.insert(A[i],0);
  while ( !PQ.empty() ) PQ.del_min();
}
float T1 = used_time(T);

{ p_queue<double,list_item,bin_heap> PQ(n);
  checked_p_queue<double,int,bin_heap> CPQ(PQ);
  for (int i = 0; i < n; i++) CPQ.insert(A[i],0);
  while ( !CPQ.empty() ) CPQ.del_min();
}
float T2 = used_time(T);

I.write_demo("Time for unchecked queue = ",T1);
I.write_demo("Time for checked queue = ",T2);





return 0;
}

