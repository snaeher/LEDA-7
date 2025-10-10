#include <LEDA/geo/geo_alg.h>
#include <LEDA/system/multi_thread.h>


using namespace leda;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

using std::ifstream;

static leda_lock io_mutex; // used for io
static int seg_num = 0; // number of segments
static int thr_num = 0; // number of threads

static list<rat_segment> seglist;

/*
static bool add_manager = true;
*/


void* thread_func(void* p)
{ 
  unsigned id = get_thread_id();

/*
  if (add_manager) std_memory.add_manager(id);
*/

  float T = used_time();
  float t = elapsed_time();

  GRAPH<rat_point,rat_segment> G;
  SWEEP_SEGMENTS(seglist,G,true);

  io_mutex.lock();
  cout << string("%2d (%2d): |V|= %4d  |E| = %d  used: %.2f real: %.2f", 
                       long(p), id, G.number_of_nodes(),G.number_of_edges(),
                       used_time(T),elapsed_time(t));
  cout <<endl;
  io_mutex.unlock();

  return NULL;
}



int main(int argc, char** argv)
{
  if (argc < 2)
  { cerr << "usage: sw file" << endl;
    return 1;
   }


  ifstream in(argv[1]);
  seglist.read(in);

  thr_num = 2;

  seg_num = seglist.length();

  //rand_int.set_seed(17*seg_num);

  cout << endl;
  cout << "threads:  " << thr_num << endl;
  cout << "segments: " << seg_num << endl;
  cout << endl;

  float T = used_time();

  thread_t* thr = new thread_t[thr_num];

  for(int j=0; j < 1; j++)
  { for(long i=0; i<thr_num; i++) 
     { thr[i] = thread_create(thread_func,(void*)i);
       //if (add_manager) std_memory.add_manager(thr[i]);
      }
    for(int i=0; i<thr_num; i++) thread_join(thr[i]);
   }

/*
  std_memory.print_statistics();
  std_memory.clear();
  std_memory.print_statistics();
*/

  cout << endl;
  cout << string("total cpu time: %.2f sec", used_time(T)) << endl;
  cout << endl;

  return 0;
}
