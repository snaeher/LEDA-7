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
class cqueue {

  struct node {
     T    inf;
     node* next;
     node(const T& x) : inf(x),next(0)  {}
  };

   node* head;
   node* tail;

public:

   cqueue(const T& x) { head = tail = new node(x); }

   void append(const T& x)
   { node* p = new node(x);
     node* old_tail;

     for(;;)
     { old_tail = tail;
       node* next = old_tail->next;
       if (old_tail == tail)
       {  if (next == 0)
           { if (Compare_And_Swap(&old_tail->next,next,p) == next)
                break;
            }
          else
           Compare_And_Swap(&tail,old_tail,next) ;
        }
      }

     Compare_And_Swap(&tail,old_tail,p) ;
    }



    bool pop(T& result)
    { node* old_head;

      for(;;)
      { old_head = head;
        node* old_tail = tail;
        node* next = old_head->next;

        if (old_head == head)
        {  if (old_head == old_tail)
           {  if (next == 0) return false;
              Compare_And_Swap(&tail,old_tail,next);
            }
           else
           { result = next->inf;
             if (Compare_And_Swap(&head,old_head,next) == old_head) break;
            }
          }
       }

      delete old_head;
      return true;
     }
};


int main()
{

   cqueue<int> S(-1);

   S.append(17);
   S.append(7);
   S.append(3);

   int x;
   if (S.pop(x)) cout << x <<endl;
   if (S.pop(x)) cout << x <<endl;
   if (S.pop(x)) cout << x <<endl;
}
