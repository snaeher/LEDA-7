#ifndef QUICKSORT_INT
#define QUICKSORT_INT

#include <LEDA/system/event.h>


class alg_qsort {
  
  public :

  EVENT1<alg_qsort&>             start_event;
  EVENT1<alg_qsort&>             finish_event;
  EVENT3<alg_qsort&, int, int>   _1_event;
  EVENT3<alg_qsort&, int, int>   _2_event;
  EVENT2<alg_qsort&, int>        _3_event;
  EVENT2<alg_qsort&, int>        _4_event;
  EVENT2<alg_qsort&, int>        _5_event;
  EVENT1<alg_qsort&>             _6_event;
  EVENT3<alg_qsort&, int, int>   _7_event;
  EVENT3<alg_qsort&, int, int>   _8_event;
  EVENT3<alg_qsort&, int, int>   _9_event;
  EVENT3<alg_qsort&, int, int>   _10_event;
  EVENT3<alg_qsort&, int, int>   _11_event;
  EVENT3<alg_qsort&, int, int>   _12_event;
  EVENT3<alg_qsort&, int, int>   _13_event;
};

  class Qsort : public alg_qsort {
  
  void swap (int A_int[], int i, int j); 
  void _qsort(int A_int[], int l, int r);

  public :
  
  void operator () (int A_int[], int l, int r)
  { 
    start_event(*this);
    _qsort(A_int, l, r);
    finish_event(*this);
  }  
};

extern Qsort QUICKSORT; 


#endif
