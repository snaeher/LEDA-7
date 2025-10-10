#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_list.h>
#include <LEDA/core/b_queue.h>
#include <LEDA/core/slist.h>

#include <assert.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;

static int del_count = 0;

static int bfs(const graph& G,const node_list& free_in_A, 
                              const node_array<bool>& free,
                              node_array<int>& dist,
                              node_array<edge>& pred,
                              int start_level)
{
  // bfs starting at free nodes in A with initial dist values = start_level
  // returns the target level (dist value of first level with a free node)
  // (-1 if no free node in B can be reached)
  // Precondition: forall(v,G) dist[v] < start_level 

  int n = G.number_of_nodes();
  b_queue<node> Q(n);

  node v;
  forall(v,free_in_A)
  { Q.append(v);
    dist[v] = start_level;
   }

  int target_level = -1; // undefined

  while (!Q.empty())
  { node v = Q.pop();
    int dv = dist[v];

    if (dv == target_level) break;

    edge e;
    forall_out_edges(e,v)
    { node w = G.target(e);

      if (dist[w] >= start_level ) continue; // reached before

      dist[w] = dv+1; 
      pred[w] = nil;

      if (target_level != -1) continue;

      if (free[w]) 
        target_level = dist[w];
      else
        Q.append(w);

    }
  }

  return target_level;
}



static edge find_augmenting_path(const graph& G, edge x, 
                                                 const node_array<bool>& free,
                                                 const node_array<int>& dist,
                                                 node_array<edge>& pred)
{ 
  // use dfs to compute a path starting with the unmatched edge x to a free 
  // node (on the target level) using "useful" edges only, i.e. edges (v,w) 
  // with dist[w] = dist[v]+1

  node v = G.target(x);
  pred[v] = x;

  if (free[v]) return x;

  int d = dist[v] + 1;

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    if (dist[w] != d || pred[w] != nil) continue;
    edge y = find_augmenting_path(G,e,free,dist,pred);
    if (y) return y;
  }

  return nil;
}



static node find_alternating_path(const graph& G, node v, 
                                  const node_array<int>& dist,
                                  node_array<int>& label,
                                  int target_level, 
                                  int N)
{
  // use dfs to check if some node on the target level is reachable from v
  // using all edges (not only useful edges) 
  // returns such a node or nil (if it doesnt exist)

  // a node v is visited iff label[v] == N;
  label[v] = N;

  if (dist[v] == target_level) return v;

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    if (label[w] == N) continue;
    node u = find_alternating_path(G,w,dist,label,target_level,N);
    if (u) return u;
  }

  return nil;
}




list<edge> MAX_CARD_BIPARTITE_MATCHING_HK_NEW(graph& G, const list<node>& A, 
                                                        const list<node>& B,
                                                        node_array<bool>& NC,
                                                        bool use_heuristic)
{
  //check that all edges are directed from A to B
  node v;  
  forall(v,B) assert(G.outdeg(v) == 0); 

  node_array<bool> free(G,true);

  if (use_heuristic) 
  { int count = 0;
    edge e;
    forall_edges(e,G)
    { node v = G.source(e);
      node w = G.target(e);
      if (free[v] && free[w])
      { free[v] = false;
        free[w] = false;
        G.rev_edge(e);
        count++;
       }
     }
  } 

  node_list free_in_A;
  forall(v,A) {
     if (free[v]) free_in_A.append(v);
  }

  node_array<int>  dist(G,-1);
  node_array<edge> pred(G,nil);


  int start_level = 0;
  int target_level = -1;

  node_array<int> label(G,0);
  int N = 0;

  del_count = 0;

  while ((target_level = bfs(G,free_in_A,free,dist,pred,start_level)) != -1) 
  { 
    slist<edge> L;

    forall(v,free_in_A)
    { 
/*
      if (find_alternating_path(G,v,dist,label,target_level,++N) == nil) {
        free_in_A.del(v);
        del_count++;
        continue;
      }
*/

      edge x = nil;

      int d = dist[v]+1;

      edge e;
      forall_out_edges(e,v)
      { node w = G.target(e);
        if (dist[w] != d || pred[w] != nil) continue;
        x = find_augmenting_path(G,e,free,dist,pred);
        if (x) break;
      }

      if (x) { 
        L.append(x);
        continue;
      }

      if (find_alternating_path(G,v,dist,label,target_level,++N) == nil) {
        free_in_A.del(v);
        del_count++;
      }

    }

    while (!L.empty())
    { edge e = L.pop(); 
      free[G.target(e)] = false;
      node u;
      while (e)
      { G.rev_edge(e);
        u = G.target(e);
        e = pred[u];
      }
      free[u] = false;
      free_in_A.del(u);
    } 

    // prepare for next phase

    start_level = target_level+1;

    if (start_level > (MAXINT - 2*G.number_of_nodes()))
    { // possible overflow: reset dist values and start_level
      dist.init(G,-1);
      start_level = 0;
     }
  }
    
  list<edge> result;

  forall(v,B) {
    edge e;
    forall_out_edges(e,v) result.append(e);
  }


  // compute vertex cover NC

  forall_nodes(v,G) NC[v] = false;
  node_array<bool> reachable(G,false);

  forall(v,A) {
    if (free[v]) DFS(G,v,reachable);
  }

  edge e;
  forall(e,result)
  { node v = G.source(e);
    node w = G.target(e);
    if (reachable[v])
      NC[v] = true;
    else
      NC[w] = true;
   }


  // restore original edge orientation
  forall(e,result) G.rev_edge(e);

  return result;
}




int main()
{
  graph G;
  list<node> A;
  list<node> B;
  

  // test with random bipartite graphs of growing density

  int a = 100000;
  int b = 100000;

  //bool heuristic = true;
  bool heuristic = false;


  for(int i=1; i<=10; i++)
  {
    //float density = float(i*i*i)/10000; 

    int k = i+4;
    //double density = double(k*k)/1000; 
    double density = double(k*k)/10000; 

    int m = int(0.5 + density*a*b);

    cout << endl;
    cout << string("%2d: |A| = %d  |B| = %d  |E| = %d   density: %.2f %%",
                                                 i,a,b,m,100*density) << endl;
  
    random_bigraph(G,a,b,m,A,B);
    
    node_array<bool> NC(G,false);

    float T = used_time();

    cout << "MAX_CARD_BIPARTITE_MATCHING          " << flush;
    list<edge> M0 = MAX_CARD_BIPARTITE_MATCHING(G,A,B,NC);
    cout << string("%5.2f sec    |M| = %d\n",used_time(T), M0.length());

    cout << "MAX_CARD_BIPARTITE_MATCHING_ABMP     " << flush;
    M0 = MAX_CARD_BIPARTITE_MATCHING_ABMP(G,A,B,NC,heuristic);
    cout << string("%5.2f sec    |M| = %d\n",used_time(T), M0.length());
    
    cout << "MAX_CARD_BIPARTITE_MATCHING_HK       " << flush;
    list<edge> M = MAX_CARD_BIPARTITE_MATCHING_HK(G,A,B,NC,heuristic);
    cout << string("%5.2f sec    |M| = %d\n",used_time(T), M.length());

    cout << "MAX_CARD_BIPARTITE_MATCHING_HK1      " << flush;
    list<edge> M1 = MAX_CARD_BIPARTITE_MATCHING_HK1(G,A,B,NC,heuristic);
    cout << string("%5.2f sec    |M| = %d\n",used_time(T), M1.length());
    assert(M1.length() == M.length());

    cout << "MAX_CARD_BIPARTITE_MATCHING_HK2      " << flush;
    list<edge> M2 = MAX_CARD_BIPARTITE_MATCHING_HK2(G,A,B,NC,heuristic);
    cout << string("%5.2f sec    |M| = %d\n",used_time(T), M2.length());
    assert(M2.length() == M.length());

    cout << "MAX_CARD_BIPARTITE_MATCHING_HK_NEW   " << flush;
    list<edge> M3 = MAX_CARD_BIPARTITE_MATCHING_HK_NEW(G,A,B,NC,heuristic);
    cout << string("%5.2f sec    |M| = %d\n",used_time(T), M3.length());

    cout << "deleted: " << del_count << endl;

    assert(M.length() == M0.length());

    cout << endl;
  }
  
  return 0;
}
