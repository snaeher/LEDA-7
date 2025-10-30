/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _b_heap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/b_heap.h>


LEDA_BEGIN_NAMESPACE

b_heap::b_heap(int a, int b) : _size(0), T(a,b)
{ if (b<a) LEDA_EXCEPTION(1,"constructor: illegal size\n");
  low = a;
  high = b;
  min = high+1;
  max = low-1;
  int i;
  for(i=a;i<=b;i++) T[i] = new list<b_heap_item>;
}

b_heap::b_heap(const b_heap& h) : min(h.min),max(h.max),low(h.low),high(h.high),_size(h._size), T(h.low,h.high)
{
	for(int i=low;i<=high;i++) 
	{
		T[i] = new list<b_heap_item>;
	 list<b_heap_node*> *L = h.T[i];
		list_item it = L->first();
		while(it != nil)
		{
			b_heap_node* it1 = L->contents(it);
			b_heap_node* n = new b_heap_node(it1->key,it1->info);
			h.copy_inf(n->info);
			n->loc=T[i]->append(n);
			it = L->succ(it);
		}
	}

}

b_heap& b_heap::operator=(const b_heap& h)
{
	if(this != &h)
	{
		clear();

		_size = h._size;
		low = h.low; high = h.high;
		min = h.min; max = h.max;

		T.resize(low,high);

		for(int i=low;i<=high;i++) 
		{
			T[i] = new list<b_heap_item>;
			list<b_heap_item> *L = h.T[i];
			list_item it = L->first();
			while(it != nil)
			{
			b_heap_node* it1 = L->contents(it);
			b_heap_node* n = new b_heap_node(it1->key,it1->info);
			h.copy_inf(n->info);
			n->loc=T[i]->append(n);
			it = L->succ(it);
			}
		}
	}
	return *this;
}


	


void b_heap::clear()
{ int i;
  for (i=low;i<=high;i++) 
	{

		list<b_heap_item> *L = T[i];
		list_item it = L->first();
		while(it != nil)
		{
			clear_inf(L->operator[](it)->info);
			it = L->succ(it);
		}

		T[i]->clear();
	}
  min = high+1;
  max = low-1;
  _size = 0;
} 

void b_heap::insert(b_heap_item it)
{ int k = it->key;
  if (k < low || k > high) 
     LEDA_EXCEPTION(1,string("insert: illegal key %d\n",k));

  if (k < min) min = k;
  if (k > max) max = k;

  it->loc = T[k]->append(it);
  ++_size;
}

b_heap_item b_heap::find_min()
{ if (min>high) return 0;
  return T[min]->head();
}

b_heap_item b_heap::find_max()
{ if (max<low) return 0;
  return T[max]->head();
}

void b_heap::del_min()
{ if (min>high) 
       LEDA_EXCEPTION(1,"b_heap del_min: heap is empty");
  b_heap_item p = T[min]->pop();
	
  clear_inf(p->info);
  delete p;
  --_size;
  while ((min <= high) && (T[min]->empty())) min++;
  if (min>high) max = low-1;
}

void b_heap::del_max()
{ if (max<0) LEDA_EXCEPTION(1,"b_heap del_max: heap is empty");
  b_heap_item p = T[max]->pop();

  clear_inf(p->info);
  delete p;
  --_size;
  while ((max>=low) && (T[max]->empty())) max--;
  if (max<low) min = high+1;
}

void b_heap::decrease_key(b_heap_item it, int k)
{ if (it==0) LEDA_EXCEPTION(1,"decrease_key: item = nil\n");
  if (k<low || k>high) LEDA_EXCEPTION(1,"decrease_key: illegal key\n");
  if (it->loc==0) LEDA_EXCEPTION(1,"decrease_key: item not found\n");
  T[it->key]->del(it->loc);
  it->key = k;
  it->loc = T[k]->append(it); 
  while ((max >= low) && (T[max]->empty())) max--;
  if (k<min) min = k;
//  return it;
}

void b_heap::increase_key(b_heap_item it, int k)
{ if (it==0) LEDA_EXCEPTION(1,"increase_key: item = nil\n");
  if (k<low || k>high) LEDA_EXCEPTION(1,"increase_key:illegal key\n");
  if (it->loc==0) LEDA_EXCEPTION(1,"increase_key: item not found\n");
  T[it->key]->del(it->loc);
  while ((min <= high) && (T[min]->empty())) min++;
  it->key = k;
  it->loc = T[k]->append(it); 
  if (k>max) max = k;
 // return it;
}


void b_heap::delete_item(b_heap_item it) 
{ if (it==0) LEDA_EXCEPTION(1,"delete_item: item = nil\n");
  if (it->loc==0) LEDA_EXCEPTION(1,"delete_item: item not found\n");
  T[it->key]->del(it->loc); 
  while ((min <= high) && (T[min]->empty())) min++;
  while ((max >= low) && (T[max]->empty())) max--;
	clear_inf(it->info);
  delete it;
  --_size;
}

void b_heap::print()
{ for (int i=low;i<=high;i++) 
  { cout << i << ": ";
		
		list<b_heap_item> *L = T[i];
		list_item it = L->first();
		while(it!=nil)
		{
			print_inf(L->operator[](it)->info);
			cout << " ";
			it = L->succ(it);
		}
    //T[i]->print();
    cout << "\n";
   }
 }

LEDA_END_NAMESPACE
