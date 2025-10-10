
class list { 

  struct list_elem  
  { // a local structure for representing the elements of the list
    int entry;
    list_elem* succ;
    list_elem(const int& x, list_elem* s) : entry(x), succ(s) {}
    friend class list;
   };

  list_elem* hd;  // head of list
  int        sz;  // size of list
  
public:

  void push(int);
  void pop(int&);
  int  head() const;
  int  size() const;

  list();
 ~list();
  list(const list&);
};

