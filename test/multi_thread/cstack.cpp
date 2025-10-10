#include <LEDA/core/list.h>
#include <LEDA/internal/system.h>

using namespace leda;

using std::cout;
using std::cin;
using std::endl;


#if defined(_MSC_VER)
inline void * Compare_And_Swap(void **ptr, void *old_val, void *new_val) {
	return InterlockedCompareExchangePointer(ptr, new_val, old_val);
}

inline long Compare_And_Swap(long *ptr, long old_val, long new_val) {

// #if defined(__win32__)
  return InterlockedCompareExchange((volatile long *)ptr, new_val, old_val);
/*

#else
// win64
	return InterlockedCompareExchange64((volatile long *)ptr, new_val, old_val);
#endif
*/
}


#else
// linux

inline long Compare_And_Swap(long *ptr, long old_val, long new_val) {
	return __sync_val_compare_and_swap((volatile long *)ptr, old_val, new_val);
}


inline long Compare_And_Swap(unsigned long *ptr, unsigned long old_val, unsigned long new_val) {
	return __sync_val_compare_and_swap((volatile unsigned long *)ptr, old_val, new_val);
}


inline int Compare_And_Swap(int *ptr, int old_val, int new_val) {
	return __sync_val_compare_and_swap((volatile int *)ptr, old_val, new_val);
}

inline void * Compare_And_Swap(void **ptr, void *old_val, void *new_val) {
	return (void *)Compare_And_Swap((long *)ptr, (long)old_val, (long)new_val);
}

template <typename T>
inline T * Compare_And_Swap(T **ptr, const T *old_val, const T *new_val) {
	return (T *)Compare_And_Swap((long *)ptr, (long)old_val, (long)new_val);
}

template <typename T>
inline bool Compare_And_Swap_b(T *ptr, const T &old_val, const T &new_val) {
	return (T)Compare_And_Swap(ptr, old_val, new_val) == old_val;
}

#endif



template <class T>
class cstack {

  struct node {
     T    inf;
     node* next;
     node(const T& x) : inf(x)  {}
  };

   node* head;

public:

   cstack() : head(0) {}

   void push(const T& x)
   { node* new_head = new node(x);
     for(;;)
     { node* old_head = head;
       new_head->next = old_head;
       if (Compare_And_Swap(&head, old_head, new_head) == old_head) break;
      }
    }

    bool pop(T& result)
    { node* old_head;

      for(;;)
      { old_head = head;
        if (old_head == 0) break;
        node* new_head = old_head->next;
        if (Compare_And_Swap(&head,old_head,new_head) == old_head) break;
       }

      if (old_head == 0) return false;

      result = old_head->inf;
      delete old_head;
      return true;
     }

       

};


int main()
{

   cstack<int> S;

   S.push(17);
   S.push(7);
   S.push(3);

   int x;
   if (S.pop(x)) cout << x <<endl;
   if (S.pop(x)) cout << x <<endl;
   if (S.pop(x)) cout << x <<endl;
}
