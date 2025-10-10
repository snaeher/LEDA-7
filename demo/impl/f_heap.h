
#include <LEDA/basic.h>

class f_heap_node;

typedef f_heap_node* f_heap_item;

class f_heap_node {

 friend class f_heap;

 f_heap_item left;     // left and right siblings (circular list)
 f_heap_item right;
 f_heap_item parent;   // parent node
 f_heap_item child;    // a child 
 f_heap_item next;     // list of all items
 f_heap_item pred;

 int  rank;            // number of children 
 bool marked;          // mark bit

 GenPtr key;           // key
 GenPtr inf;           // information 


f_heap_node(GenPtr k, GenPtr info, f_heap_item n)
{ 
  // the third argument n is always the first item in the list 
  // of all items of a Fibonacci heap. The new item is added 
  // at the front of the list 

  key = k;
  inf = info;
  rank = 0;
  marked = false;
  parent = nil;
  child = nil;
  next = n;
  if (n) n->pred = this;
}

 LEDA_MEMORY(f_heap_node)

};


class f_heap  {

  int number_of_nodes;         
  int power;
  int logp;

  f_heap_item minptr;       
  f_heap_item node_list;

  
  virtual int    cmp(GenPtr,GenPtr)     const = 0; 
  virtual void   clear_key(GenPtr&)     const = 0; 
  virtual void   clear_inf(GenPtr&)     const = 0; 
  virtual GenPtr copy_key(GenPtr&)      const = 0; 
  virtual GenPtr copy_inf(GenPtr&)      const = 0;
  virtual int    key_type_id()          const = 0;

  
  f_heap_item link(f_heap_item, f_heap_item);


  f_heap_item cut(f_heap_item);


public:

typedef f_heap_item item; 

protected:  

 // constructors, destructor, assignment
 f_heap();
 f_heap(const f_heap&);
 f_heap& operator=(const f_heap&);
 virtual ~f_heap();

 // priority queue operations
 f_heap_item insert(GenPtr, GenPtr);
 f_heap_item find_min()  const;
 
 void   del_min(); 
 void   decrease_key(f_heap_item,GenPtr);
 void   change_inf(f_heap_item,GenPtr);
 void   del_item(f_heap_item);
 void   clear();
 
 GenPtr key(f_heap_item) const;
 GenPtr inf(f_heap_item) const;
 int    size() const; 
 bool   empty() const;

 // iteration
 f_heap_item first_item() const;
 f_heap_item next_item(f_heap_item) const;
};

