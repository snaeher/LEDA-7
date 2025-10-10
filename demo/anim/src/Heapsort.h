#ifndef HEAPSORT_INT
#define HEAPSORT_INT

#include <LEDA/system/event.h>


class alg_heapsort {
  
  public :

  EVENT1<alg_heapsort&>             start_event;
  EVENT2<alg_heapsort&, int>         _1_event;
  EVENT2<alg_heapsort&, int>         _2_event;
  EVENT3<alg_heapsort&, int, int>    _3_event;
  EVENT2<alg_heapsort&, int>         _4_event;
  EVENT3<alg_heapsort&, int, int>    _5_event;
  EVENT2<alg_heapsort&, int>         _6_event;
  EVENT3<alg_heapsort&, int, int>    _7_event;
  EVENT3<alg_heapsort&, int, int>    _8_event;
  EVENT3<alg_heapsort&, int, int>    _9_event;
  EVENT3<alg_heapsort&, int, int>    _10_event;
  EVENT2<alg_heapsort&, int>         _11_event;
  EVENT2<alg_heapsort&, int>         _12_event;
  EVENT2<alg_heapsort&, int>         _13_event;
  EVENT1<alg_heapsort&>              finish_event;
};


class Heapsort : public alg_heapsort {
  
  private:
  
  void swap(int A_int[], int i, int j);
  void heapify(int A_int[], int k, int N);
  void _heapsort(int A_int[], int N);
  
  public:
  
  void operator () (int A_int[], int N)
  {  
    start_event(*this);
    _heapsort(A_int, N);
    finish_event(*this);
  }  
};

extern Heapsort HEAPSORT; 


#endif
