
#define PRIO_IMPL f_heap

typedef PRIO_IMPL::item pq_item;

template<class P, class I> 
class p_queue: private PRIO_IMPL
{
  int    key_type_id()           const { return LEDA_TYPE_ID(P); }

  int    cmp(GenPtr x, GenPtr y) const 
         { return LEDA_COMPARE(P,x,y); }

  void   clear_key(GenPtr& x)    const { LEDA_CLEAR(P,x); }

  void   clear_inf(GenPtr& x)    const { LEDA_CLEAR(I,x); }

  GenPtr copy_key(GenPtr& x)     const { return LEDA_COPY(P,x); }

  GenPtr copy_inf(GenPtr& x)     const { return LEDA_COPY(I,x); }

public:
  p_queue()  {}
  p_queue(const p_queue<P,I>& Q):PRIO_IMPL(Q) {}
 ~p_queue()  { PRIO_IMPL::clear(); }
 
  p_queue<P,I>& operator=(const p_queue<P,I>& Q) 
  { PRIO_IMPL::operator=(Q); return *this; }
  
  P       prio(pq_item it) const 
          { return LEDA_CONST_ACCESS(P,PRIO_IMPL::key(it)); }
  I       inf(pq_item it)  const 
          { return LEDA_CONST_ACCESS(I,PRIO_IMPL::inf(it)); }
  pq_item find_min()       const { return PRIO_IMPL::find_min();}
  void    del_min()              { PRIO_IMPL::del_min(); }
  void    del_item(pq_item it)   { PRIO_IMPL::del_item(it); }
  
  pq_item insert(const P& x, const I& i) 
  { return PRIO_IMPL::insert(leda_cast(x),leda_cast(i)); }
  
  void    change_inf(pq_item it, const I& i) 
  { PRIO_IMPL::change_inf(it,leda_cast(i)); }
  
  void decrease_p(pq_item it, const P& x)
  { PRIO_IMPL::decrease_key(it,leda_cast(x)); }
  
  int  size()  const { return PRIO_IMPL::size(); }
  bool empty() const { return (size()==0) ? true : false; }
  void clear()       { PRIO_IMPL::clear(); }
  
  pq_item first_item()          const 
          { return PRIO_IMPL::first_item(); }
  pq_item next_item(pq_item it) const 
          { return PRIO_IMPL::next_item(it); }
  
};

