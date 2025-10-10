#include <LEDA/geo/geo_alg.h>

#include <LEDA/system/multi_thread.h>
#include <LEDA/system/file.h>

#include <LEDA/core/dictionary.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/impl/ab_tree.h>


using namespace leda;
using std::cout;
using std::endl;

static leda_mutex io_mutex; // used for io
static int seg_num = 0; // number of segments
static int thr_num = 0; // number of threads


static list<rat_segment> seglist;


void* thread_func(void* p)
{ 
  LEDA_THREAD_BEGIN;

  long id = long(p);

  float T = used_time();

  GRAPH<rat_point,rat_segment> G;
  SWEEP_SEGMENTS(seglist,G,true);

  io_mutex.lock();
  cout << string("thread %2d  (0x%08x): |V|= %4d  |E| = %d   %.2f sec", id,
         get_thread_id(),G.number_of_nodes(),G.number_of_edges(),used_time(T));
  cout <<endl;
  io_mutex.unlock();

  //print_statistics();

  return NULL;
}


int main(int argc, char** argv)
{

/*
  int argc;
  char** argv;
  get_command_line(argc,argv);

  for(int i=0; i<argc;i++) cout << argv[i] << endl;
*/

  cout << endl;

  seg_num = read_int("#segments = ");
  thr_num = read_int("#threads = ");

  rand_int.set_seed(961960*seg_num);

  thread_t* thr = new thread_t[thr_num];

  int count = 0;


  for(int i=0;i<4;i++)
  { 
    seglist.clear(); 
  
    rand_int.set_seed(961960*seg_num);
  
    for (int i = 0; i < seg_num; i++)
    { leda::rat_point p, q;
      leda::random_point_in_unit_square(p);
      leda::random_point_in_unit_square(q);
      if (p != q) seglist.push_back(leda::rat_segment(p, q));
    }
  
    float T1 = used_time();
    float T2 = real_time();

    cout << endl;
    cout << ++count << endl;
    for(long i=0; i<thr_num; i++) thr[i] = thread_create(thread_func,(void*)i);
    for(int i=0; i<thr_num; i++) thread_join(thr[i]);
  

    float t1 = used_time(T1);
    float t2 = real_time(T2);

    cout << endl;
    cout << string("cpu time: %.2f sec  real_time: %.2f  speed up: %.1f", 
                                       t1, t2, t1/t2) << endl;
    cout << endl;

  }

  print_statistics();

  return 0;
}

