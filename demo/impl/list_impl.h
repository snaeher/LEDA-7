
class list_impl {

  struct list_impl_elem
  { void* entry;   
    list_impl_elem* succ;
    list_impl_elem(void* x,list_impl_elem* s):entry(x),succ(s) {}
    friend class list_impl;
   };

  list_impl_elem* hd;
  int sz;

 protected: 

  list_impl();
 ~list_impl(); 

  void* head() const;
  void* pop();
  void  push(void* x);
  void  clear();
  int   size() const;
};

