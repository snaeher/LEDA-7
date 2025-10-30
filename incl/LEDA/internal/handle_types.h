/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  handle_types.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_HANDLE_H
#define LEDA_HANDLE_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 722155
#include <LEDA/internal/PREAMBLE.h>
#endif


//------------------------------------------------------------------------------
// handle_base/rep: base classes for handle types string, point, segment,...
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

class __exportC handle_rep  {

private:


 //int count;
 atomic_counter count;

public:

 handle_rep() : count(1) {}

 unsigned long ref_num() const { return count; }
 unsigned long ref_plus()  { return ++count; } 
 unsigned long ref_minus() { return --count; } 

 LEDA_MEMORY(handle_rep)

};


class HandleBase {
protected:
void* PTR;

#if (defined(_MSC_VER) && defined(_M_X64))
friend unsigned long long ID_Number(const HandleBase& x)
{ //return *((unsigned long long*)&x.PTR); 
	union { void* ptr; unsigned long long id; } u = { x.PTR };
	return u.id;
}
#else
friend unsigned long ID_Number(const HandleBase& x) 
{ //return *((unsigned long*)&x.PTR); 
  union { void* ptr; unsigned long id; } u = {x.PTR};
  return u.id;
}
#endif

friend bool identical(const HandleBase& x, const HandleBase& y) 
{ return x.PTR == y.PTR; }
};



template <class rep_type> 
class handle_base : public HandleBase {

protected:

rep_type* ptr() const { return (rep_type*)PTR; }

public:

//handle_base() { PTR = nullptr; }
handle_base() { PTR = 0; }

 handle_base(const handle_base& x) 
 { PTR = x.PTR;  
   ptr()->ref_plus();
 }

 ~handle_base() { if (PTR && ptr()->ref_minus() == 0) delete ptr(); }

 handle_base& operator=(const handle_base& x)
 { x.ptr()->ref_plus();
   if (ptr()->ref_minus() == 0) delete ptr();
   PTR = x.PTR;
   return *this;
  }
 
 unsigned long refs() const { return ptr()->ref_num(); }
 
 LEDA_MEMORY(handle_base<T>)
};

#define HANDLE_BASE(T) handle_base<T>

LEDA_END_NAMESPACE


#if LEDA_ROOT_INCL_ID == 722155
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


#endif
 
