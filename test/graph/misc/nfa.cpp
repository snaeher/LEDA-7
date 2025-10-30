/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  nfa.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/core/set.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/array.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;




// We use sets, stacks, and arrays of nodes 


// alphabeth: `a`,'b', ... , 'z','~'

const char EPSILON = '~';

//------------------------------------------------------------------------------
// NFA's are directed graph with
// node labels of type "int"  (states)  edge labels of type "char" 
//------------------------------------------------------------------------------

typedef GRAPH<int,char> NFA;



//------------------------------------------------------------------------------
// DFA's are directed graph with
// node labels of type set<node>  (sets of nodes of an NFA)
// edge labels of type "char"      
//----------------------------------------------------------------------------*/

// inline int compare(const node& x, const node& y) { return x-y; }


typedef GRAPH<set<node>,char> DFA;



//------------------------------------------------------------------------------
// Epsilon Closure
//------------------------------------------------------------------------------

void EPSILON_CLOSURE(NFA& A, set<node>& T)
{
  // expands node set T by dfs on the epsilon edges 

  stack<node> S;

  node v;
  forall(v,T) S.push(v);

  while (!S.empty())
  { 
    v = S.pop();

    // visit all neighbors u of v not in T and reachable by an epsilon-edge 
    edge e;
    forall_adj_edges(e,v)
    { node u = target(e);
      if ( A[e] == EPSILON && !T.member(u) ) 
       { T.insert(u);
         S.push(u);
        }
     }
   }
 }



//------------------------------------------------------------------------------
// Move
//------------------------------------------------------------------------------

bool MOVE(NFA& A, set<node>& T, char x, set<node>& S)
{
  // result is the set of nodes S to which there 
  // is a transition on input symbol x from a node in T

  S.clear();

  node v;
  forall(v,T)
  { edge e;
    forall_adj_edges(e,v)
      if ( A[e] == x ) S.insert(target(e));
   }

  return !S.empty();
}



//------------------------------------------------------------------------------
// Build a DFA from an NFA
//------------------------------------------------------------------------------

DFA  BUILD_DFA_FROM_NFA(NFA& A, node s0)
{
  // result is a DFA B accepting the same language
  // as NFA A with initial state s0


  DFA B;

  stack<node> S;

  set<node> T;

  // First we create a DFA-node for epsilon-closure(s0)and push it 
  // on the stack S. S contains all nodes of DFA B whose transitions 
  // have not been examined so far.  

  T.insert(s0);

  EPSILON_CLOSURE(A,T);

  node v = B.new_node(T);

  S.push(v);

  while ( ! S.empty() )
  { v = S.pop();

    for(char c = 'a'; c<='z'; c++)  // for each input symbol c do
     {
       if (MOVE(A,B[v],c,T))
       {
         EPSILON_CLOSURE(A,T);
  
         // search for a DFA-node w with B[w] == T 

         bool found = false;       
         node w;
         forall_nodes(w,B)
           if (B[w] == T)
              { found = true;
                break;
               }

         // if no such node exists create it 
  
         if ( !found )                     
          { w = B.new_node(T);
            S.push(w);
           }
  
         B.new_edge(v,w,c);               
        }
      }
   }

 return B;
  
}



  

int main()
{ 

  // Build a NFA A

  NFA A;

  // States = {0,1,...,N-1}

  int N = read_int("number of states N = ");

  cout << "Start state = 0\n";

  array<node> state(0,N-1);

  int i,j;
  char c;

  // create N nodes: state[0], ... , state[N-1]

  for(i=0; i<N; i++) state[i] = A.new_node(i);


  // create edges (transistions)

  cout << "Enter Transitions of NFA (terminate input with   0 0 0)\n";
  

  for(;;)
  { cout << "state1  state2  label : ";
    cin >> i >> j >> c;
    if (i==0 && j==0 && c=='0') break;

    A.new_edge(state[i], state[j], c);
   }

  node u,v;
  edge e;

  // output  NFA A:

  cout << endl;
  cout << "NFA A: \n";

  forall_nodes(v,A)
    { cout << string(" [%d] : ",A[v]);
      forall_adj_edges(e,v) 
        cout << string(" [%d]--%c-->[%d] ",A[v],A[e],A[target(e)]);
      cout << endl;
     }

  DFA B = BUILD_DFA_FROM_NFA(A,state[0]);


  // output  DFA B:

  node_array<int> name(B);

  cout << endl;
  cout << "DFA B:\n";
  i=0;
  forall_nodes(v,B)
    { name[v] = i++;
      cout << string(" [%d] = {",i);
      forall(u,B[v]) cout << " " << A[u];
      cout << " }\n";
     }
  cout << endl;

  forall_nodes(v,B)
    { cout << string(" [%d] : ",name[v]);
      forall_adj_edges(e,v) 
        cout << string(" [%d]--%c-->[%d] ",name[v],B[e],name[target(e)]);
      cout << endl;
     }

  return 0;
}
