#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE


template<class I, class P>
class f3_heap { 

class f3_heap_node  {

friend class f3_heap;

   I info;  
   P prio; 
   f3_heap_node* next; 
   f3_heap_node* child; 

void add_child(f3_heap_node* v)
{ // insert v into list of children
  // precondition: rank == v->rank && prio <= child->prio
   v->next = child;
   child = v;
}

public:

   f3_heap_node(const I& i, const P& p) 
   {  prio = p;
      info = i;
      next = 0;
      child = 0;
    }

   f3_heap_node() {};

  //LEDA_MEMORY(f3_heap_node)

 };


   int count;         // number of nodes
   int copy_count;    // number of copies
   int min_index;
   int num_nodes;
   f3_heap_node* roots[32];
   f3_heap_node* free;
   f3_heap_node* node_buffer;


   int insert_node(f3_heap_node* p, int r)
   { f3_heap_node** ptr = roots + r; 
     while (*ptr) 
     { f3_heap_node* q = *ptr;
       if (q->prio < p->prio) swap(p,q);
       p->add_child(q);
       *ptr++ = 0;
      }
     *ptr = p;
     return ptr-roots;
   }


  void check()
  {
    int n = 0;

    int min_i = -1;

    for(int i=0; i<32; i++)
    { f3_heap_node* p = roots[i];
      if (p == 0) continue;
      n += (1<<i);

      if (min_i == -1 || p->prio < roots[min_i]->prio) min_i = i;

      int rank = 0;
      for(f3_heap_node* q = p->child; q; q = q->next) rank++;

      assert(rank == i);
    }

    assert(n == count);

    if (count == 0)
      assert(min_index == -1);
    else
      assert(roots[min_index]->prio <= roots[min_i]->prio);
  }
    

    

public:

 typedef f3_heap_node* item;

 f3_heap_node* new_node(const I& inf, const P& prio)
 { f3_heap_node* p = free;
   free = free->next;
   p->info = inf;
   p->prio = prio;
   return p;
  }


 f3_heap(int n)
 { node_buffer = new f3_heap_node[n]; 
   num_nodes = n;
   clear();
  }

 ~f3_heap() { delete[] node_buffer; }


 void clear()
 { count = 0;
   copy_count = 0;
   min_index = -1;
   for(int i=0; i<32; i++) roots[i] = 0; 
   // init memory
   free = node_buffer;
   f3_heap_node* last = free + num_nodes - 1;
   for(f3_heap_node* p = free; p < last; p++) p->next = p+1;
   last->next = 0;
  }

    

 void decrease_p(const I& inf, const P& prio)
 { copy_count++;
   insert(inf,prio);
  }

 item insert(const I& inf, const P& prio)
 { 
   f3_heap_node* p = new_node(inf,prio);

   int i = insert_node(p,0);

   if (++count == 1)
     min_index = 0;
   else
   { f3_heap_node* q = roots[min_index];
     if (q == 0 || p->prio <= q->prio) min_index =i;
    }

   return p;
 }


 void del_min(bool genuine)  
 { 
   f3_heap_node* min_ptr = roots[min_index];

   roots[min_index] = 0;

   f3_heap_node* q = min_ptr->child;

   for(int i = min_index-1; i >= 0; i--)
   { f3_heap_node* p = q;
     q = q->next;
     insert_node(p,i);
    }

   // free min_ptr
   min_ptr->next = free;
   free = min_ptr;

   count--;
   if (!genuine) copy_count--;

   min_index = -1;

   if (count == 0) return;

   // find new minimum

   int j = 0;
   while ((count & (1<<j)) == 0) j++;

   min_index = j;
   P min_p = roots[j]->prio;

   while ((1 << ++j) <= count)
   { if ((count & (1<<j)) == 0) continue;
     P x = roots[j]->prio;
     if (x < min_p)
     { min_index = j;
       min_p = x;
      }
    }

    assert(min_index >= 0);
 }


 item find_min() const { return (min_index < 0) ? 0 : roots[min_index]; }

 const I& inf(item it) const { return it->info; }
 const P& prio(item it) const { return it->prio; }

 float ratio()  const { return float(copy_count)/(count-copy_count); }
 int  size()  const { return count-copy_count; }
 int  empty() const { return size() == 0; }

 int  free_nodes() const { return num_nodes - count; }

};

LEDA_END_NAMESPACE

