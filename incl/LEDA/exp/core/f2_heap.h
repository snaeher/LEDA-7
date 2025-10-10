#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE


template<class I, class P>
class f2_heap { 

class f2_heap_node  {

friend class f2_heap;

   I info;  
   P prio; 
   f2_heap_node* next; 
   f2_heap_node* child; 

void add_child(f2_heap_node* v)
{ // insert v into list of children
  // precondition: rank == v->rank && prio <= child->prio
   v->next = child;
   child = v;
}

public:

   f2_heap_node(const I& i, const P& p) 
   {  prio = p;
      info = i;
      next = 0;
      child = 0;
    }

  LEDA_MEMORY(f2_heap_node)

 };


   int count;         // number of nodes
   int min_index;
   f2_heap_node* roots[32];
   f2_heap_node* buffer; // list of additional nodes


   void insert_node(f2_heap_node* p, int r)
   { f2_heap_node** ptr = roots + r;
     for(;;)
     { f2_heap_node* q = *ptr;
       if (q == 0) break;
       if (q->prio < p->prio) swap(p,q);
       p->add_child(q);
       *ptr++ = 0;
      }
     *ptr = p;
   }


  void check()
  {
    int n = 0;

    int min_i = -1;

    for(int i=0; i<32; i++)
    { f2_heap_node* p = roots[i];
      if (p == 0) continue;
      n += (1<<i);

      if (min_i == -1 || p->prio < roots[min_i]->prio) min_i = i;

      int rank = 0;
      for(f2_heap_node* q = p->child; q; q = q->next) rank++;

      assert(rank == i);
    }

    for(f2_heap_node* q = buffer; q; q = q->next) n++;

    assert(n == count);

    if (count == 0)
      assert(min_index == -1);
    else
      assert(roots[min_index]->prio <= roots[min_i]->prio);
  }
    

    

public:

 typedef f2_heap_node* item;

 f2_heap() : count(0),min_index(-1),buffer(0) 
 { for(int i=0; i<32; i++) roots[i] = 0; }


 item insert(const I& inf, const P& prio)
 { f2_heap_node* p = new f2_heap_node(inf,prio);

   if (min_index < 0) 
   { // first node
     assert(count == 0);
     min_index = 0;
     roots[0] = p;
    }
   else
   if (p->prio < roots[min_index]->prio) 
   { // new minimum: swap p and roots[0]
     min_index = 0;
     f2_heap_node* q = roots[0];
     roots[0] = p;
     if (q)
     { q->next = buffer;
       buffer = q;
      }
    }
   else
   { p->next = buffer;
     buffer = p;
    }

   count++;
   return p;
 }


 void del_min()  
 { 
   f2_heap_node* min_ptr = roots[min_index];
   roots[min_index] = 0;

   while (buffer) 
   { f2_heap_node* p = buffer;
     buffer = p->next;
     insert_node(p,0);
    }

   f2_heap_node* q = min_ptr->child;

   for(int i = min_index-1; i >= 0; i--)
   { f2_heap_node* p = q;
     q = q->next;
     insert_node(p,i);
    }

   delete min_ptr;

   count--;

   // find new minimum

   min_index = -1;

   for(int x = count, j=0; x > 0; x/=2, j++)
   { f2_heap_node* p = roots[j];
     if (p == 0) continue;
     if (min_index == -1 || p->prio < roots[min_index]->prio) min_index = j;
    }

 }


 item find_min() const { return (min_index < 0) ? 0 : roots[min_index]; }

 const I& inf(item it) const { return it->info; }
 const P& prio(item it) const { return it->prio; }

 int  size()  const { return count; }
 int  empty() const { return count == 0; }

};

LEDA_END_NAMESPACE

