
#include "list_impl.h"

template<class T> 
class list : private list_impl {

public:

  void push(const T& x) { 
     list_impl::push(new T(x)); 
  }

  const T&  head() const { 
     return *(T*)list_impl::head(); 
  }

  void pop(T& x) { 
    T* p = (T*)list_impl::pop();
    x = *p;
    delete p;
  }

  int size() const { 
    return list_impl::size(); 
  }

  void clear() { 
     while (size() > 0) 
        delete (T*)list_impl::pop(); 
    list_impl::clear(); 
  }

  list() : list_impl() {}

 ~list() { 
    clear();
  }

};

