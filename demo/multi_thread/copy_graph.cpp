#include <LEDA/graph/graph.h>
#include <LEDA/system/multi_thread.h>

#include <assert.h>

using namespace leda;
using std::cout;
using std::endl;

static graph G;
static leda_mutex io_mutex;


void copy_graph(GRAPH<node,edge>& H, const graph& G)
{ H.clear();

  node_array<node> v_in_H(G);
  node v;
  forall_nodes(v,G) 
  { node u = H.new_node(v);
    v_in_H[v] = u;
   }

  forall_nodes(v,G)
  { node u = v_in_H[v];
    edge e;
    forall_adj_edges(e,v)
    { node s = source(e);
      node t = target(e);
      if (s == v) H.new_edge(u,v_in_H[t],e);
     }
   }
}


void* thread_func(void* p)
{ 
#if defined(LEDA_MULTI_THREAD)
  LEDA_THREAD_BEGIN;
#endif

  long id = long(p);

  float T = used_time();

  GRAPH<node,edge> H;
  CopyGraph(H,G);

  io_mutex.lock();
  cout << string("thread %2d: |V|= %4d  |E| = %d   %.2f sec", id,
                       H.number_of_nodes(),H.number_of_edges(),used_time(T));
  cout <<endl;
  io_mutex.unlock();

  return NULL;
}


int main()
{
  int n = read_int("n = ");
  int m = read_int("m = ");

  int thr_num = read_int("#threads = ");


  thread_t* thr = new thread_t[thr_num];

  int count = 0;

  random_graph(G,n,m);

  for(;;)
  { 
    float T = used_time();
    cout << ++count << endl;
    for(long i=0; i<thr_num; i++) thr[i] = thread_create(thread_func,(void*)i);
    for(int i=0; i<thr_num; i++) thread_join(thr[i]);
    cout << string("cpu time: %.2f sec", used_time(T)) <<endl;
    cout << endl;
  }

  return 0;
}

