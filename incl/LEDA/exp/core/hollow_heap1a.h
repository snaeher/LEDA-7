#include <LEDA/system/basic.h>

using std::cout;
using std::endl;

LEDA_BEGIN_NAMESPACE


template<class P, class I>
class hollow_heap1a { 

class node  {

friend class hollow_heap1a;

   I info;  
   P prio; 
   int rank; // -1 : hollow
   node* next; 
   node* child; 

void add_child(node* p)
{ // insert v into list of children and increase rank
  p->next = child;
  child = p;
  rank++;
}

node* join(node* p)
{ // make p a child of this node or vice versa
  if (prio < p->prio) 
  {  p->next = child;
     child = p;
     p = this;
   }
  else
  { next = p->child;
    p->child = this;
  }

  p->rank++;
  return p;
}
  



bool is_hollow() { return rank < 0; }
bool is_full() { return rank >= 0; }

public:

   node(const I& i, const P& p) 
   {  prio = p;
      info = i;
      next = 0;
      child = 0;
      rank = 0;
    }

  LEDA_MEMORY(node)

 };


static const int MAXRANK = 50;

   int count;         // number of nodes
   int max_rank;
   node* min_ptr;
   node* roots[MAXRANK];


   void insert_node(node* p)
   {
     int i = p->rank;
     while (roots[i])
     { p = p->join(roots[i]);
       roots[i++] = 0;
      }

     roots[i] = p;

     // update minimum - we have to use "<="  
     // the old min_ptr could point to a non-root node here !

     if (min_ptr == 0 || p->prio <= min_ptr->prio) min_ptr = p;

     if (i > max_rank) max_rank = i;
   }


  void check_node(node* p)
  {
    if (p == 0) return;

    int rank = 0;
    for(node* q = p->child; q; q = q->next) 
    { rank++;
      check_node(q);
     }

    if (p->is_full()) 
      assert(p->rank == rank);
    else
      assert(rank <= 2);
  }


  void check()
  {
    int min_i = -1;

    for(int i=0; i<MAXRANK; i++)
    { node* p = roots[i];

      if (p == 0) continue;

      assert(p->next == 0);
      assert(p->rank == i);

      if (min_i == -1 || p->prio < roots[min_i]->prio) min_i = i;

      check_node(p);
    }
  }
    

public:

 typedef node* item;

 hollow_heap1a() : count(0), max_rank(-1), min_ptr(0)
 { for(int i=0; i<MAXRANK; i++) roots[i] = 0; 
  }


 node* insert(const P& prio,const I& inf)
 { node* p = new node(inf,prio);
   if (min_ptr == 0)  min_ptr = p;
   insert_node(p);
   count++;
   return p;
 }


 void del_min()  
 {
   node* h = min_ptr; // head of list of nodes to be removed
   h->next = 0;

   // remove from roots table
   roots[min_ptr->rank] = 0; 

   // undefine min pointer
   min_ptr = 0;


   //int max_i = -1;

   while (h != 0)
   { node* w = h->child;
     node* v = h;
     h = h->next;
     while (w != 0)
     { node* u = w;
       w = w->next;
       if (u->is_hollow())
       { u->next = h;
         h = u;
        }
       else 
       { // u is full
         insert_node(u);
        }
     }
    delete v;
   }

  if (--count == 0) return;


  // find new minimum

/*
  while (roots[max_rank] == 0) max_rank--;
  min_ptr = roots[max_rank];

  for(int i = 0; i<max_rank; i++)
  { node* p = roots[i];
    if (p && p->prio < min_ptr->prio) min_ptr = p;
   }
*/

  int i = 0;
  while (roots[i] == 0) i++;
  min_ptr = roots[i++];

  while(i<=max_rank)
  { node* p = roots[i++];
    if (p && p->prio < min_ptr->prio) min_ptr = p;
   }

 }


void decrease_p(item& it, const P& prio)
{
  node* p = it;

  assert(p->is_full());

  if (roots[p->rank] == p)
  { // p is a root
    p->prio = prio;
    if (prio < min_ptr->prio) min_ptr = p;
    return;
   }

  node* q = new node(p->info,prio);

/*
  int r = p->rank;

  if (r > 2)
  { // move r-2 children from p to q
    node* ptr = p->child;
    ptr = ptr->next;
    q->child = ptr->next;
    ptr->next = 0;
    q->rank = r-2;
   }
*/

  p->rank = -1; // mark p hollow

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

