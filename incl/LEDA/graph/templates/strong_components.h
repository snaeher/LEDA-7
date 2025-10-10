/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  strong_components.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/b_stack.h>
#include <LEDA/core/tuple.h>
#include <assert.h>




LEDA_BEGIN_NAMESPACE


template <class graph_t>
class  STRONG_COMPONENTS_CM {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

int   count1;
int   count2;
float cputime;


template<class compnum_array>
void dfs(const graph_t& G, node v, compnum_array& compnum,
                                   node_array<int,graph_t>& dfsnum,
                                   b_stack<node>& unfinished,
                                   node_array<bool,graph_t>& in_unfinished,
                                   b_stack<node>& roots)
{
  node w;

  dfsnum[v] = ++count1;
  in_unfinished[v] = true;
  unfinished.push(v);
  roots.push(v);

  edge e;
  forall_out_edges(e,v)
    { w = G.target(e);
      if (dfsnum[w]==-1) 
       dfs(G,w,compnum,dfsnum,unfinished,in_unfinished,roots);
      else 
       if (in_unfinished[w])
        while (dfsnum[roots.top()]>dfsnum[w])  roots.pop();
     }

  if (v==roots.top())
  { node u;
    do { u=unfinished.pop();
         in_unfinished[u] = false;
         compnum[u] = count2;
       } while (u != v);
     count2++;
     roots.pop(); 
    }
}


public:

template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();

  b_stack<node> roots(n);
  b_stack<node> unfinished(n);
  node_array<bool,graph_t> in_unfinished(G,false);
  node_array<int,graph_t> dfsnum(G,-1);

  count1 = 0; 
  count2 = 0;

  node v;
  forall_nodes(v,G) 
     if (dfsnum[v] == -1) 
        dfs(G,v,compnum,dfsnum,unfinished,in_unfinished,roots);

  cputime = used_time(t);
  return count2;
}

template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};




template <class graph_t>
class  STRONG_COMPONENTS_CM_NEW {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

int count1;
int count2;

float cputime;



template<class compnum_array>
void dfs(const graph_t& G, node v, compnum_array& compnum,
                                   b_stack<node>& unfinished,
                                   b_stack<int>& roots)
{ 
  int dfsnum = --count1;

  compnum[v] = dfsnum;
  unfinished.push(v);
  roots.push(dfsnum);

  edge e;
  forall_out_edges(e,v)
  { node w =G.target(e);
    int d = compnum[w];
    if (d == -1) 
      dfs(G,w,compnum,unfinished,roots);
    else 
      if (d < -1)
         while (roots.top() < d)  roots.pop();
   }

  if (roots.top() == dfsnum) 
  { node u;
    do { u = unfinished.pop(); // u is a node of the scc with root v 
         compnum[u] = count2;
        } while (v != u);
    roots.pop(); 
    count2++; 
   }
}


public:

template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();

  b_stack<int>  roots(n);
  b_stack<node> unfinished(n);

  count1 = -1; 
  count2 = 0;

  node v;
  forall_nodes(v,G) compnum[v] = -1; 

  forall_nodes(v,G) 
    if (compnum[v] == -1) 
       dfs(G,v,compnum,unfinished,roots);

  cputime = used_time(t);
  return count2;
}

template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};


template <class graph_t>
class  STRONG_COMPONENTS_CM_NEW1 {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

int count1;
int count2;

float cputime;



template<class compnum_array>
void dfs(const graph_t& G, node v, compnum_array& compnum,
                                   b_stack<node>& edgeStack,
                                   b_stack<node>& unfinished,
                                   b_stack<int>&  roots)
{ 
  int dfsnum = --count1;

  compnum[v] = dfsnum;
  roots.push(dfsnum);
  unfinished.push(v);

  int sz = edgeStack.size();  // remember current size of edgeStack 

  edge e;
  forall_rev_out_edges(e,v) edgeStack.push(G.target(e));
  
  while (edgeStack.size() > sz)
  { node w = edgeStack.pop();
    int  d = compnum[w];

    if (d >= 0) continue;

    if (d == -1) 
      dfs(G,w,compnum,edgeStack,unfinished,roots);
    else 
      while (roots.top() < d) roots.pop();
   }


  if (roots.top() == dfsnum) 
  { node u;
    do { u = unfinished.pop(); // u is a node of the scc with root v 
         compnum[u] = count2;
        } while (v != u);
    roots.pop(); 
    count2++; 
   }
}


public:

template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<node> edgeStack(m+1);
  b_stack<int>  roots(n);
  b_stack<node> unfinished(n);

  count1 = -1; 
  count2 = 0;

  node v;
  forall_nodes(v,G) compnum[v] = -1; 

  forall_nodes(v,G) 
    if (compnum[v] == -1) 
       dfs(G,v,compnum,edgeStack,unfinished,roots);

  cputime = used_time(t);
  return count2;
}

template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};



//-------------------------------------------------------------------------


template<class node_t>
inline void* StopOutEdge(node_t v) { return v->stop_out_edge(); }

inline void* StopOutEdge(node) { return NULL; }



template <class graph_t>
class  STRONG_COMPONENTS_CM_NR {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;


float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{ 
  float t = used_time();

  typedef two_tuple<node,edge> stack_el;

  int n = G.number_of_nodes();

  b_stack<stack_el> S(n+1);
  b_stack<int>      roots(n);
  b_stack<node>     unfinished(n);

  stack_el sentinel(0,0);

  int count     = -1;   // dfs numbers:  -2, -3, ... 
  int scc_count =  0;   // scc numbers:   0,  1, ...

  node v;
  forall_nodes(v,G) compnum[v] = -1;  // mark unvisited

  node u;
  forall_nodes(u,G)
  { 
    if (compnum[u] != -1) continue;

    unfinished.push(u);
    roots.push(--count);
    compnum[u] = count;

    node v = u;
    edge e = G.first_out_edge(u);


    S.push(sentinel);

    while (v != NULL)
    { 
      if (e == StopOutEdge(v))     // all edges scanned: finish v
      { if (compnum[v] == roots.top()) 
        { node u;
          do { u = unfinished.pop(); // u is a node of the scc with root v 
               compnum[u] = scc_count;
             } while (v != u);
          roots.pop(); 
          scc_count++;
         }
   
        stack_el p = S.pop();
        v = p.first();
        e = p.second();
        continue;
       }

      node w = G.target(e);
      int d = compnum[w];

      if (d == -1)  // tree edge (v,w)
        { compnum[w] = --count;
          roots.push(count);
          unfinished.push(w);
          S.push(stack_el(v,G.next_out_edge(e)));
          v = w;
          e = G.first_out_edge(w);
         }
       else
        { if (d < -1) while (roots.top() < d)  roots.pop();
          e = G.next_out_edge(e);
         }
    } 
  }

  cputime = used_time(t);
  return scc_count;
}



template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};



//----------------------------------------------------------------------------


#define    FLAG(x) node((unsigned long)x | 0x80000000)
#define  UNFLAG(x) node((unsigned long)x & 0x7fffffff)
#define FLAGGED(x) ((unsigned long)x & 0x80000000)



template<class graph_t>
class  STRONG_COMPONENTS_CM_NR1 {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<int>  dfsnumStack(n);
  b_stack<node> edgeStack(n+m);

  b_stack<node> unfinished(n);
  b_stack<int>  roots(n);

  int count1 = -1;  // dfsnumbers  -2, -3, ...
  int count2 =  0;  // scc numbers  0, 1, 2, ... 

  node v;
  forall_nodes(v,G) compnum[v] = -1; // unvisited

  forall_nodes(v,G)
  {
    if (compnum[v] != -1) continue;

    int dfsnum = --count1;
    compnum[v] = dfsnum;

    roots.push(dfsnum);
    unfinished.push(v);
    edgeStack.push(FLAG(v));

    edge e;
    forall_rev_out_edges(e,v) edgeStack.push(G.target(e));

    for (;;)
    { node w = edgeStack.pop();

      if (FLAGGED(w))
      {
        if (roots.top() == dfsnum)
        { w = UNFLAG(w);
          node u;
          do { u = unfinished.pop();
               compnum[u] = count2;
             } while (w != u);
          roots.pop();
          count2++;
        }

        if (edgeStack.empty()) break;

        dfsnum = dfsnumStack.pop();
        continue;
      }


      int d = compnum[w];

      if (d == -1)
      { dfsnumStack.push(dfsnum);
        dfsnum = --count1;          
        compnum[w] = dfsnum;
        roots.push(dfsnum);
        unfinished.push(w);
        edgeStack.push(FLAG(w));
        edge e;
        forall_rev_out_edges(e,w) edgeStack.push(G.target(e));
        continue;
       }

      if (d < -1) while (roots.top() < d) roots.pop();
    }
  }


  cputime = used_time(t);

  return count2;
}


template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};



template<class graph_t>
class  STRONG_COMPONENTS_CM_NR2 {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<int>  dfsnumStack(n);
  b_stack<node> edgeStack(n+m);

  b_stack<node> unfinished(n);
  b_stack<int>  roots(n);

  int count1 = -1;  // dfsnumbers  -2, -3, ...
  int count2 =  0;  // scc numbers  0, 1, 2, ... 

  node v;
  forall_nodes(v,G) compnum[v] = -1; // unvisited

  forall_nodes(v,G)
  {
    if (compnum[v] != -1) continue;

    int dfsnum = --count1;
    compnum[v] = dfsnum;
    roots.push(dfsnum);
    unfinished.push(v);

    edgeStack.push(NULL);
    edge e;
    forall_rev_out_edges(e,v) edgeStack.push(G.target(e));

    for(;;)
    { 
      node w = edgeStack.pop();

      if (w == NULL)
      {
        if (roots.top() == dfsnum)
        { int d = 0;
          while (d != dfsnum)
          { node u = unfinished.pop();
            d = compnum[u];
            compnum[u] = count2;
           }
          roots.pop();
          count2++;
        }

        if (edgeStack.empty()) break;

        dfsnum = dfsnumStack.pop();
        continue;
      }

      int d = compnum[w];

      if (d == -1)
      { dfsnumStack.push(dfsnum);
        dfsnum = --count1;
        compnum[w] = dfsnum;
        roots.push(dfsnum);
        unfinished.push(w);
        edgeStack.push(NULL);
        edge e;
        forall_rev_out_edges(e,w) edgeStack.push(G.target(e));
        continue;
       }

      if (d < -1) while (roots.top() < d) roots.pop();
    }
  }

  cputime = used_time(t);

  return count2;
}


template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};


template<class graph_t>
class  STRONG_COMPONENTS_CM_NR3 {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<node> nodeStack(n);
  b_stack<node> edgeStack(n+m);

  b_stack<node> unfinished(n);
  b_stack<int>  roots(n);


  int count1 = -1;  // dfsnumbers  -2, -3, ...
  int count2 =  0;  // scc numbers  0, 1, 2, ... 

  node u;
  forall_nodes(u,G) compnum[u] = -1; // unvisited

  forall_nodes(u,G)
  {
    if (compnum[u] != -1) continue;

    compnum[u] = --count1;
    roots.push(count1);
    unfinished.push(u);
    edgeStack.push(u);
    edge e;
    forall_rev_out_edges(e,u) 
    { node w = G.target(e);
      if (w != u) edgeStack.push(w);
     }

    node v = u;
    int  dfsnum = count1;

    for(;;)
    { 
      node w = edgeStack.pop();

      if (w == v)
      { if (roots.top() == dfsnum)
        { node u;
          do { u = unfinished.pop();
               compnum[u] = count2;
             } while (w != u);
          roots.pop();
          count2++;
        }

        if (nodeStack.empty()) break;

        v = nodeStack.pop();
        dfsnum = compnum[v];
        continue;
      }

      int d = compnum[w];

      if (d == -1)
      { compnum[w] = --count1;
        nodeStack.push(v);
        v = w;
        dfsnum = count1;
        roots.push(count1);
        unfinished.push(w);
        edgeStack.push(w);
        edge e;
        forall_rev_out_edges(e,w)
        { node u = G.target(e);
          if (u != w) edgeStack.push(u);
         }
        continue;
       }

      if (d < -1) while (roots.top() < d) roots.pop();
    }
  }

  cputime = used_time(t);

  return count2;
}


template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};





//---------------------------------------------------------------------------


template <class graph_t>
class  STRONG_COMPONENTS_LPT {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

int count;
int scc_count;

float cputime;

template <class compnum_array>
void dfs(const graph_t& G, node v, compnum_array& compnum,
                                   node_array<bool,graph_t>& incurrent,
                                   node_array<int,graph_t>& dfsnum,
                                   node_array<int,graph_t>& lowpnt,
                                   b_stack<node>& current)
{
  count++;
  dfsnum[v] = count;
  lowpnt[v] = count;

  incurrent[v] = true;
  current.push(v);

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    if (dfsnum[w] == 0)
    { dfs(G,w,compnum,incurrent,dfsnum,lowpnt,current);
      if (lowpnt[w] < lowpnt[v]) lowpnt[v] = lowpnt[w];
     }
    else
      if (incurrent[w] && dfsnum[w] < lowpnt[v]) lowpnt[v] = dfsnum[w];
  }

  if (lowpnt[v] == dfsnum[v])
  { node u;
    do { u = current.pop();
         incurrent[u] = false;
         compnum[u] = scc_count;
     } while (u != v);
    scc_count++;
   }
}


public:

template <class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();

  node_array<bool,graph_t> incurrent(G);
  b_stack<node>          current(n);
  node_array<int,graph_t>  dfsnum(G);
  node_array<int,graph_t>  lowpnt(G);

  scc_count = 0;
  count = 0;

  node v;
  forall_nodes(v,G)   
  { dfsnum[v] = 0;
    incurrent[v] = false;
   }

  forall_nodes(v,G)   
    if (dfsnum[v] == 0)
       dfs(G,v,compnum,incurrent,dfsnum,lowpnt,current);

 cputime = used_time(t);
 return scc_count;
}

template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};



//---------------------------------------------------------------------------


template <class graph_t>
class  STRONG_COMPONENTS_LPT_NEW {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

int count;
int scc_count;

float cputime;


/*
   As in the implementation of the Cheriyan/Mehlhorn algorithm (suggested 
   by Peter Sanders) we use only one node_array "compnum" for both dfs 
   and component numbering. For every node v there are 3 cases: 

   unvisited:   compnum[v] == 0
   completed:   compnum[v]  > 0  is the scc-number of v 
   unfinished:  compnum[v]  < 0  is the dfs-number of v
   
   For the computation of lowpoints it is important that dfs-numbers
   are generated increasingly. Therefore we start dfs numbering at -n,
   i.e. the range of dfs numbers is [-n ... -1].

   The most important improvement compared wit the text book version
   of the algorithm is the implicit representation of lowpoints:
   We do not store low points in a node_array ! In the recursive dfs
   function it suffices to store the lowpoint of the current node v in 
   a local variable and to make dfs return this value.

   See the code below for more details.
*/




template <class compnum_array>
int dfs0(const graph_t& G, node v, compnum_array& compnum,
                                   b_stack<node>& unfinished)
{ 
  // returns lowpoint of v 

  int dfsnum = count++;
  int lowpnt = dfsnum;
  compnum[v] = dfsnum; 

  unfinished.push(v);

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    int d = compnum[w];
    if (d == 0)
      { // first visit of w
        int lp = dfs0(G,w,compnum,unfinished,count,scc_count);
        if (lp < lowpnt) lowpnt = lp;
       }
    else
      { // w is unfinished if d < 0 (then d == dfsnum(w)) 
        if (d < lowpnt) lowpnt = d;
       }
  }

  if (dfsnum == lowpnt) // v is root of completed scc
  { scc_count++;
    node w; 
    do { w = unfinished.pop();
         compnum[w] = scc_count;
       } while (w != v);
   }

  return lowpnt;
}


template <class compnum_array>
int dfs(const graph_t& G, node v, compnum_array& compnum,
                                  b_stack<node>& unfinished)
{ int dfsnum = count++;
  int lowpnt = dfsnum;
  compnum[v] = dfsnum; 
  unfinished.push(v);

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    int d = compnum[w];
    if (d == -1) d = dfs(G,w,compnum,unfinished);
    if (d < lowpnt) lowpnt = d;
   }

  if (dfsnum == lowpnt)
  { node u; 
    do { u = unfinished.pop();
         compnum[u] = scc_count;
       } while (u != v);
    scc_count++;
   }

  return lowpnt;
}


public:

template <class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{
  float t = used_time();

  int n = G.number_of_nodes();

  b_stack<node> unfinished(n);

  scc_count = 0;
  count = -(n+1);

  node v;
  forall_nodes(v,G) compnum[v] = -1;

  forall_nodes(v,G)   
  { if (compnum[v] != -1) continue;
    dfs(G,v,compnum,unfinished);
   }

 cputime = used_time(t);
 return scc_count;
}

template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};



//---------------------------------------------------------------------------


template <class graph_t>
class  STRONG_COMPONENTS_LPT_NR {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;


float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{ 
  float t = used_time();

  typedef four_tuple<node,edge,int,int> stack_el;

  stack_el sentinel(0,0,0,0);

  int n = G.number_of_nodes();

  b_stack<stack_el> S(n);
  b_stack<node>     unfinished(n);

  int count     = -(n+1);   // dfs numbers:  -(n+1), -n, ... 
  int scc_count =  0;       // scc numbers:   0,  1, ...

  node v;
  forall_nodes(v,G) compnum[v] = -1;  // mark unvisited

  node u;
  forall_nodes(u,G)
  { 
    if (compnum[u] != -1) continue;

    node v = u;
    edge e = G.first_out_edge(u);
    int dfsnum = count++;
    int lowpnt = dfsnum;

    compnum[v] = dfsnum;
    unfinished.push(v);

    S.push(sentinel);

    while (v != NULL)
    { 
      if (e == StopOutEdge(v))     // all edges scanned: finish v
      { if (dfsnum == lowpnt) 
        { node u;
          do { u = unfinished.pop(); // u is a node of the scc with root v 
               compnum[u] = scc_count;
             } while (v != u);
          scc_count++;
         }
   
        stack_el p = S.pop();
        v      = p.first();
        e      = p.second();
        dfsnum = p.third();
        int lp = p.fourth();
        if (lp < lowpnt) lowpnt = lp;
        continue;
       }

      node w = G.target(e);
      int  d = compnum[w];

      if (d == -1)  // tree edge (v,w)
        { S.push(stack_el(v,G.next_out_edge(e),dfsnum,lowpnt));
          v = w;
          e = G.first_out_edge(w);
          dfsnum = count++;
          lowpnt = dfsnum;
          compnum[w] = dfsnum;
          unfinished.push(w);
         }
       else
        { if (d < lowpnt)  lowpnt = d;
          e = G.next_out_edge(e);
         }
    } 
  }

  cputime = used_time(t);
  return scc_count;
}



template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};


template <class graph_t>
class  STRONG_COMPONENTS_LPT_NR1 {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;


float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{ 
  float t = used_time();

  typedef two_tuple<int,int> stack_el;


  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<stack_el> S(n);
  b_stack<node>     edgeStack(n+m);
  b_stack<node>     unfinished(n);

  int count     = -(n+1);   // dfs numbers:  -(n+1), -n, ... 
  int scc_count =  0;       // scc numbers:   0,  1, ...

  node v;
  forall_nodes(v,G) compnum[v] = -1;  // mark unvisited

  node u;
  forall_nodes(u,G)
  { 
    if (compnum[u] != -1) continue;

    node v = u;
    int dfsnum = count++;
    int lowpnt = dfsnum;

    compnum[v] = dfsnum;
    unfinished.push(v);

    edgeStack.push(NULL);
    edge e;
    forall_rev_out_edges(e,v)
    { node u = G.target(e);
      if (u != v) edgeStack.push(u);
     }

    for(;;)
    { 
      node w = edgeStack.pop();

      if (w == NULL)     // all edges scanned: finish v
      { if (dfsnum == lowpnt) 
        { int d = 0;
          while (d != dfsnum)
          { node u = unfinished.pop(); 
            d = compnum[u];
            compnum[u] = scc_count;
          } 
          scc_count++;
         }
   
        if (S.empty()) break;

        stack_el p = S.pop();
        dfsnum = p.first();
        int lp = p.second();
        if (lp < lowpnt) lowpnt = lp;
        continue;
       }

      int d = compnum[w];

      if (d == -1)  // tree edge (v,w)
        { S.push(stack_el(dfsnum,lowpnt));
          v = w;
          edgeStack.push(NULL);
          edge e;
          forall_rev_out_edges(e,v)
          { node u = G.target(e);
             if (u != v) edgeStack.push(u);
           }
          dfsnum = count++;
          lowpnt = dfsnum;
          compnum[w] = dfsnum;
          unfinished.push(w);
         }
       else
         if (d < lowpnt)  lowpnt = d;
    } 
  }

  cputime = used_time(t);
  return scc_count;
}


template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};



template <class graph_t>
class  STRONG_COMPONENTS_LPT_NR2 {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;


float cputime;

public:


template<class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{ 
  float t = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<int>  dfsnumStack(n);
  b_stack<int>  lowpntStack(n);
  b_stack<node> edgeStack(n+m);

  b_stack<node> unfinished(n);

  int count = -(n+1);   // dfs numbers:  -(n+1), -n, ... 
  int scc_count =  0;   // scc numbers:   0,  1, ...

  node v;
  forall_nodes(v,G) compnum[v] = -1;  // mark unvisited

  forall_nodes(v,G)
  { 
    if (compnum[v] != -1) continue;

    int dfsnum = count++;
    int lowpnt = dfsnum;

    compnum[v] = dfsnum;
    unfinished.push(v);

    edgeStack.push(NULL);
    edge e;
    forall_rev_out_edges(e,v) edgeStack.push(G.target(e));

    for(;;)
    { 
      node w = edgeStack.pop();

      if (w == NULL)   // all edges scanned: finish current node
      { if (dfsnum == lowpnt) 
        { int d = 0;
          while (d != dfsnum)
          { node u = unfinished.pop(); 
            d = compnum[u];
            compnum[u] = scc_count;
          } 
          scc_count++;
         }
   
        if (dfsnumStack.empty()) break;

        dfsnum = dfsnumStack.pop();
        int lp = lowpntStack.pop();
        if (lp < lowpnt) lowpnt = lp; 

        continue;
       }

      int d = compnum[w];

      if (d == -1)  // tree edge (v,w)
      { dfsnumStack.push(dfsnum);
        lowpntStack.push(lowpnt);
        edgeStack.push(NULL);
        edge e;
        forall_rev_out_edges(e,w) edgeStack.push(G.target(e));
        dfsnum = count++;
        lowpnt = dfsnum;
        compnum[w] = dfsnum;
        unfinished.push(w);
        continue;
       }

      if (d < lowpnt)  lowpnt = d;
    } 
  }

  cputime = used_time(t);
  return scc_count;
}


template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};





//---------------------------------------------------------------------------



template <class graph_t>
class STRONG_COMPONENTS_REV {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

float cputime;

template <class compnum_array>
void dfs_comp(const graph_t& G, node v, compnum_array& compnum, 
                                        b_stack<node>& S)
{ compnum[v] = 0;
  edge e;
  forall_out_edges(e,v) 
  { node w = G.target(e);
    if (compnum[w] == -1) dfs_comp(G,w,compnum,S);
   }
  S.push(v);
} 

template <class compnum_array>
void dfs(const node_array<node*,graph_t>& A, node v, 
                                             compnum_array& compnum, int k)
{ 
  int indeg = -compnum[v]; 
  node* p = A[v];

  compnum[v] = k;

  for(int i=0; i<indeg; i++)
  { node w = p[i];
    if (compnum[w] <= 0) dfs(A,w,compnum,k);
   }

/*
  node* q = p+indeg;
  while (p != q)
  { node w = *p++;
    if (compnum[w] <= 0) dfs(A,w,compnum,k);
   }
*/
 }


public:


template <class compnum_array>
int run(const graph_t& G, compnum_array& compnum)
{ 
  float t = used_time();

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  b_stack<node> S(n);

  node v;
  forall_nodes(v,G) compnum[v] = -1;

  forall_nodes(v,G)
    if (compnum[v] == -1) dfs_comp(G,v,compnum,S);

  assert(S.size() == n);

  node* Q = new node[m];
  node_array<node*,graph_t> A(G);

  edge e;
  forall_edges(e,G) compnum[G.target(e)]--;

  node* s = Q;
  forall_nodes(v,G)
  { s -= compnum[v];
    A[v] = s;
   }

  assert(s == Q+m);

  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v) *--A[G.target(e)] = v;
   }


  int count = 0;
  while (!S.empty())
  { node u = S.pop();
    if (compnum[u] <= 0) dfs(A,u,compnum,++count);
   }

  delete[] Q;

  forall_nodes(v,G) compnum[v] = count-compnum[v];

  cputime = used_time(t);
  return count;
}

template <class compnum_array>
int operator()(const graph_t& G, compnum_array& compnum)
{ return run(G,compnum); }

float cpu_time() const { return cputime; }

};






template <class graph_t> class scc_test {

typedef typename graph_t::node node;
typedef typename graph_t::edge edge;

public:

template <class compnum_array>
void run(const graph_t& G, compnum_array& compnum)

{
   STRONG_COMPONENTS_CM<graph_t>      scc0;
   STRONG_COMPONENTS_CM_NEW<graph_t>  scc1;
   STRONG_COMPONENTS_CM_NEW1<graph_t>  scc2;
   STRONG_COMPONENTS_CM_NR<graph_t>    scc3;
   STRONG_COMPONENTS_CM_NR2<graph_t>  scc4;

   STRONG_COMPONENTS_LPT<graph_t>     scc5;
   STRONG_COMPONENTS_LPT_NEW<graph_t> scc6;
   STRONG_COMPONENTS_LPT_NR<graph_t>  scc7;
   STRONG_COMPONENTS_LPT_NR1<graph_t> scc8;
   STRONG_COMPONENTS_LPT_NR2<graph_t> scc9;

/*
   STRONG_COMPONENTS_REV<graph_t>     scc9;
*/

   node_array<int,graph_t> compnum0(G);
   scc0(G,compnum0);

   node v;

   scc0(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc0.cpu_time()) << std::flush;

   scc1(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc1.cpu_time()) << std::flush;

   scc2.run(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc2.cpu_time()) << std::flush;

   scc3(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc3.cpu_time()) << std::flush;

   scc4(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc4.cpu_time()) << std::flush;

   std::cout << "     " << std::flush;

   scc5(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc5.cpu_time()) << std::flush;

   scc6(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc6.cpu_time()) << std::flush;

   scc7.run(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc7.cpu_time()) << std::flush;

   scc8.run(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f ", scc8.cpu_time()) << std::flush;

   scc9(G,compnum);
   forall_nodes(v,G) assert(compnum0[v] == compnum[v]);
   std::cout << string("%5.2f", scc9.cpu_time()) << std::flush;

   std::cout << std::endl;
}

};


LEDA_END_NAMESPACE

