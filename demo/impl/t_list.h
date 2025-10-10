
template <class E>
class t_list { 

  struct list_elem 
  { E entry;
    list_elem* succ;
    list_elem(const E& x, list_elem* s) : entry(x), succ(s) {}
   };

  list_elem* hd;  // head of list
  int        sz;  // size of list
  
public:


  void push(const E&);
  void pop(E&);
  const E& head() const;
  int  size() const;

  t_list();
  t_list(const t_list<E>&);
 ~t_list();

};

