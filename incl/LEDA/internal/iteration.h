/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  iteration.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_ITERATION_H
#define LEDA_ITERATION_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721158
#include <LEDA/internal/PREAMBLE.h>
#endif

LEDA_BEGIN_NAMESPACE






template<class T>
inline void LOOP_ASSIGN(T& x, void* p) { x = (T)p; }


//------------------------------------------------------------------------------
//   forall_items
//------------------------------------------------------------------------------

// robust iteration (current item may be deleted)

#define forall_items(it,D)\
for(auto leda_loop_next=(D).first_item();\
    it=leda_loop_next,leda_loop_next=(D).next_item(it),it;)


/*
// non robust iteration
#define forall_items(it,D) for(it=(D).first_item(); it; it=(D).next_item(it))
*/


//------------------------------------------------------------------------------
//   forall_rev_items
//------------------------------------------------------------------------------

#define forall_rev_items(it,D)\
for(auto leda_loop_next=(D).last_item();\
    it=leda_loop_next,leda_loop_next=(D).pred_item(it),it;)


//------------------------------------------------------------------------------
//   forall(x,...)
//------------------------------------------------------------------------------

#define forall(x,D)\
for(auto leda_loop_next=(D).first_item(), leda_loop_it=leda_loop_next;\
 leda_loop_it=leda_loop_next, x = leda_loop_it ? (D).inf(leda_loop_it) : x,\
 leda_loop_next=(D).next_item(leda_loop_it), leda_loop_it;)

//------------------------------------------------------------------------------
//   forall_rev(x,...)
//------------------------------------------------------------------------------

#define forall_rev(x,D)\
for(auto leda_loop_next=(D).last_item(), leda_loop_it=leda_loop_next;\
 leda_loop_it=leda_loop_next, x = leda_loop_it ? (D).inf(leda_loop_it) : x,\
 leda_loop_next=(D).pred_item(leda_loop_it), leda_loop_it;)

//------------------------------------------------------------------------------
//   forall_defined(x,...)
//------------------------------------------------------------------------------

#define forall_defined(x,D)\
for(auto leda_loop_next=(D).first_item(), leda_loop_it=leda_loop_next;\
 leda_loop_it=leda_loop_next, x = leda_loop_it ? (D).key(leda_loop_it) : x,\
 leda_loop_next=(D).next_item(leda_loop_it), leda_loop_it;)




//------------------------------------------------------------------------------
// Macros for generating STL iterators
//------------------------------------------------------------------------------

#if __GNUC__ < 7
typedef unsigned long ptrdiff_t;
#endif


template<class T1, class T2>
class stl_access_pair { 
public:
 const T1& first; 
 const T2& second;
 stl_access_pair(const T1& x, const T2& y) : first(x), second(y) {}
}; 


#define DEFINE_STL_ITERATORS(DTYPE,ITYPE,VTYPE)\
\
class iterator {\
public:\
  DTYPE* L;\
  ITYPE it;\
 iterator(DTYPE* x, ITYPE y) : L(x), it(y)  {}\
 iterator() : L(0),it(0)   {}\
 iterator& operator++()    { it = L->stl_next_item(it); return *this; }\
 iterator& operator--()    { it = L->stl_pred_item(it); return *this; }\
 iterator  operator++(int) { iterator tmp = *this; operator++(); return tmp; }\
 iterator  operator--(int) { iterator tmp = *this; operator--(); return tmp; }\
 VTYPE& operator*() const  { return L->stl_item_access(it); }\
 VTYPE* operator->() const { return &(L->stl_item_access(it)); }\
 bool operator==(const iterator& i) { return it == i.it; }\
 bool operator!=(const iterator& i) { return it != i.it; }\
/*\
 friend ostream& operator<<(ostream& o, const iterator& i)\
 { return o << i.it; }\
*/\
 operator ITYPE() const { return it; }\
\
 typedef VTYPE  value_type;\
 typedef VTYPE* pointer;\
 typedef VTYPE& reference;\
 typedef std::bidirectional_iterator_tag iterator_category;\
 typedef ptrdiff_t difference_type;\
\
/*friend VTYPE* value_type(const iterator&) { return (VTYPE*)0; }*/\
};\
\
iterator  begin() { return iterator(this,first_item());}\
iterator  end()   { return iterator(this,this->stl_next_item(last_item()));  }\
\
class const_iterator {\
  const DTYPE* L;\
  ITYPE it;\
 public:\
  const_iterator(const DTYPE* x, ITYPE y) : L(x), it(y)  {}\
 const_iterator() : L(0),it(0)   {}\
 const_iterator& operator++()    { it = L->stl_next_item(it); return *this; }\
 const_iterator& operator--()    { it = L->stl_pred_item(it); return *this; }\
 const_iterator  operator++(int) { const_iterator tmp = *this; operator++(); return tmp; }\
 const_iterator  operator--(int) { const_iterator tmp = *this; operator--(); return tmp; }\
 const VTYPE& operator*() const  { return L->stl_item_access(it); }\
 const VTYPE* operator->() const { return &(L->stl_item_access(it)); }\
 bool operator==(const const_iterator& i) { return it == i.it; }\
 bool operator!=(const const_iterator& i) { return it != i.it; }\
/*\
 friend ostream& operator<<(ostream& o, const const_iterator& i)\
 { return o << i.it; }\
*/\
 operator ITYPE() const { return it; }\
\
 typedef VTYPE  value_type;\
 typedef const VTYPE* pointer;\
 typedef const VTYPE& reference;\
 typedef std::bidirectional_iterator_tag iterator_category;\
 typedef ptrdiff_t difference_type;\
\
/*friend VTYPE* value_type(const const_iterator&) { return (VTYPE*)0; }*/\
};\
\
const_iterator begin() const { return const_iterator(this,first_item()); }\
const_iterator end()   const { return const_iterator(this,this->stl_next_item(last_item()));   }


#if LEDA_ROOT_INCL_ID == 721158
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
