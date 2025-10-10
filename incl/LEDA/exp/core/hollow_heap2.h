#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE


template<class P, class I>
class hollow_heap2 { 

class hollow_heap2_node  {

friend class hollow_heap2;

   I info;  
   P prio; 
   int rank; // -1 : hollow
   hollow_heap2_node* next; 
   hollow_heap2_node* child; 
   hollow_heap2_node* xparent; 


void add_child(hollow_heap2_node* p)
{ // insert v into list of children and increase rank
  //assert(rank == p->rank);
  //assert(prio <= p->prio);
  p->next = child;
  child = p;
  rank++;
}


bool is_hollow() { return rank < 0; }

public:

   hollow_heap2_node(const I& i, const P& p) 
   {  prio = p;
      info = i;
      next = 0;
      child = 0;
      rank = 0;
      xparent = 0;
    }

  LEDA_MEMORY(hollow_heap2_node)

 };

static const int MAXRANK = 50;

   int count;         // number of nodes
   int max_rank;
   hollow_heap2_node* min_ptr;
   hollow_heap2_node* roots[MAXRANK];


   void insert_node(hollow_heap2_node* p)
   {
     // Be careful to keep the minimum node (if min_ptr != 0)
     // a root (min_ptr = roots[min_ptr->rank]). In order to
     // achieve this the following precondition must be true.

     // Precondition:
     // If p->prio is a minimal priority in the heap
     // then min_ptr must point to it and not to any other
     // node with minimal priority (unless min_ptr == 0).

     int i = p->rank;

     p->next = 0; // necessary ?

     while (roots[i])
     { hollow_heap2_node* q = roots[i];
       if (q == min_ptr || q->prio < p->prio) swap(p,q);
       p->add_child(q);
       roots[i++] = 0;
      }

     roots[i] = p;

     // min_ptr is either undefined or points to a root node
     assert(min_ptr == 0 || roots[min_ptr->rank] == min_ptr);

     if (i > max_rank) max_rank = i;
   }




  void check_node(hollow_heap2_node* p)
  {
    if (p == 0) return;

    int rank = 0;
    for(hollow_heap2_node* q = p->child; q; q = q->next) 
    { rank++;
      check_node(q);
     }

    if (p->rank >= 0) 
      assert(p->rank == rank);
    else
      assert(rank <= 2);
  }


  void check()
  {
    int min_i = -1;

    for(int i=0; i<MAXRANK; i++)
    { hollow_heap2_node* p = roots[i];

      if (p == 0) continue;

      assert(p->next == 0);

      if (min_i == -1 || p->prio < roots[min_i]->prio) min_i = i;

      check_node(p);
    }

  }
    

    

public:

 typedef hollow_heap2_node* item;

 hollow_heap2() : count(0), max_rank(-1), min_ptr(0)
 { for(int i=0; i<MAXRANK; i++) roots[i] = 0; }


 hollow_heap2_node* insert(const P& prio, const I& inf)
 { hollow_heap2_node* p = new hollow_heap2_node(inf,prio);
   count++;

   // if prio is smaller or EQUAL to the current minimum
   // set min_ptr to p (see precondition of insert_node)
   if (min_ptr ==  0 || prio <= min_ptr->prio) min_ptr = p;

   insert_node(p);
   return p;
 }




 void del_min()  
 { 
   hollow_heap2_node* h = min_ptr; // head of list of nodes to be removed

   // remove minimum from roots table
   roots[min_ptr->rank] = 0; 

   // undefine min pointer (see precondition of insert_node)
   min_ptr = 0;


   while (h != 0)
   { hollow_heap2_node* w = h->child;
     hollow_heap2_node* v = h;
     h = h->next;
     while (w != 0)
     { hollow_heap2_node* u = w;
       w = w->next;
       if (u->is_hollow())
       { if (u->xparent == 0)
         { u->next = h;
           h = u;
          }
         else
         { if (u->xparent == v) 
             w = 0;
           else
             u->next = 0;
           u->xparent = 0;
          }
        }
       else // full
         insert_node(u);
     }
    delete v;
   }

  count--;

  // find new minimum

  for(int j = 0; j <= max_rank; j++)
   { hollow_heap2_node* p = roots[j];
     if (p == 0) continue;
     if (min_ptr == 0 || p->prio < min_ptr->prio) min_ptr = p;
    }

 }


void decrease_p(item& it, const P& prio)
{
  hollow_heap2_node* p = it;

  assert(p->rank >= 0); // full

  if (roots[p->rank] == p) 
  { // root
    p->prio = prio;
    if (prio < min_ptr->prio) min_ptr = p;
    return;
   }

  hollow_heap2_node* q = new hollow_heap2_node(p->info,prio);


  int r = p->rank;

  p->xparent = q;
  q->child = p;

  p->rank = -1;
  q->rank = max(0,r-2);

  if (prio <= min_ptr->prio) min_ptr = q; // see precondition of insert_node
  insert_node(q);

  it = q;
}
  


 item find_min() const { return min_ptr; }

 const I& inf(item it) const { return it->info; }
 const P& prio(item it) const { return it->prio; }

 int  size()  const { return count; }
 int  empty() const { return count == 0; }

};

LEDA_END_NAMESPACE

