/*******************************************************************************
+
+  LEDA 7.0  
+
+
+  _mc_matching_kececioglu.cpp
+
+
+  Copyright (c) 1995-2023
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/graph/node_slist.h>
#include <LEDA/system/assert.h>
#include <LEDA/graph/mc_matching.h>

#include <LEDA/core/array.h>
#include <LEDA/core/string.h>

LEDA_BEGIN_NAMESPACE

// sn 01/2025
typedef int LABEL;
enum {ODD, EVEN, UNLABELED};

/*
static void shrink_path(node b, node v, node w, 
          node_partition& base, node_array<node>& mate, 
          node_array<node>& pred, node_array<node>& source_bridge, 
          node_array<node>& target_bridge, node_slist& Q)
{ node x = base(v);
  while (x != b)
  { base.union_blocks(x,b);
    x = mate[x];
    base.union_blocks(x,b);
    base.make_rep(b);
    Q.append(x);
    source_bridge[x] = v;  
    target_bridge[x] = w;
    x = base(pred[x]);
  }
}
*/

void find_path(list<node>& P, node x, node y,
                 node_array<LABEL>&  label, node_array<node>& pred,
                 node_array<node>& mate, 
                 node_array<node>& source_bridge,
	       node_array<node>& target_bridge, const node_array<int>& num)
/* traces the even length alternating path from x to y; if non-trivial it starts with
the matching edge incident to x; collects the non-matching edges on this
path as pairs of nodes */
{ if ( x == y ) return;

  if ( label[x] == EVEN ) 
  {
    P.append(mate[x]); P.append(pred[mate[x]]);  //cerr << "\nadded " << num[mate[x]] << " and " << num[pred[mate[x]]];
    find_path(P,pred[mate[x]],y,label,pred,mate,source_bridge,target_bridge,num);   
    return;
  }
  else // x is ODD
    { find_path(P,source_bridge[x],mate[x],label,pred,mate,source_bridge,target_bridge,num);  

      P.append(source_bridge[x]); P.append(target_bridge[x]); //cerr << "\nadded " << num[source_bridge[x]] << " and " << num[target_bridge[x]];
                             
    find_path(P,target_bridge[x],y,label,pred,mate, source_bridge,target_bridge,num);
  
    return;
  }
}



void scan(const graph& G, node v, node v0, stack<node>& SN, stack<edge>& SE, node_partition& base, const node_array<node>& mate){
  edge e; //cerr << "\nscanning node " << num[v];
  SN.push(v);
  SE.push(nil);
  forall_inout_edges(e,v){
    node w = G.opposite(e,v);
    if( base(v) == base(w) ) continue;
    SE.push(e); //cerr << "\npushed edge to " << num[w];
    if( mate[w] == nil && w != v0 ) return;   // alternatively label[w] = UNLABELED
  }
}



list<edge> MAX_CARD_MATCHING_KECECIOGLU(const graph& G, node_array<int>& OSC,
                                                        int heur)
{
    node_array<node> mate(G,nil);
    node_partition base(G);    // now base(v) = v for all nodes v

    node_array<LABEL>  label(G,UNLABELED);     
    node_array<node> parent(G);

    node_array<node> source_bridge(G);
    node_array<node> target_bridge(G);

    node_array<int>  DFSNUM(G);
    int DFSCount = 0;

    if (heur == 1)
    { // greedy heuristic (slow !)
      edge e;
      forall_edges(e,G){
	node v = G.source(e); node w = G.target(e);
        if( v != w && mate[v] == nil && mate[w] == nil){
	  mate[v] = w; 
          mate[w] = v; 
        }
      }
    }

    node v0, v, w; edge e;
    node_array<int> num(G); int count = 1; forall_nodes(v,G) num[v] = count++;


forall_nodes(v0,G)
{ if ( mate[v0] != nil ) continue;

  stack<node> SN; stack<edge> SE; list<node> T;
  //cerr << "\n\n\ngrowing a tree with root " << num[v0];
  label[v0] = EVEN; T.append(v0); DFSNUM[v0] = DFSCount++;
  scan(G, v0, v0, SN, SE, base, mate);

  bool breakthrough = false;

  while (!breakthrough && !SE.empty()){ // grow tree rooted at v0
    edge e = SE.pop();
    if( e == nil) { SN.pop(); continue; }
    
    v = SN.top();
    w = G.opposite(v,e); //cerr << "\nconsidering edge " << num[v] << " " << num[w];
    
    if( base(v) == base(w) || label[base(w)] == ODD ) continue;   // do nothing

    if( label[w] == UNLABELED ){
      label[w] = ODD;            T.append(w);
      parent[w] = v;  DFSNUM[w] = DFSCount++;
      node mw = mate[w];

      if ( mw == nil) { 
        breakthrough = true; 
// cerr << "\naugmenting path with free nodes " << num[w] << " and " << num[v0];
        break; 
      } 

      label[mw] = EVEN;   DFSNUM[mw] = DFSCount++;   T.append(mw);

      //cerr << "\ngrow step or breakthrough with " << num[v] << " " << num[w] << " " << (mate[w] == nil? nil : num[mate[w]]);

      scan(G, mw, v0, SN, SE, base, mate);
    }
    else  // base(w) is EVEN
    { // we have found a blossom; the base is either base[v] or base[w], 
      // whoever has the smaller DFSNUM
      node hv = base(v);
      node hw = base(w);
      if( DFSNUM[hv] < DFSNUM[hw] ){ swap(hv,hw); swap(v,w); }
      //cerr << "\nwalking down from " << num[hv] << " to " << num[hw];
              
      while( hv != hw ){
        node mhv = mate[hv];
	source_bridge[mhv] = v;
	target_bridge[mhv] = w;
        base.union_blocks(hv,hw); base.union_blocks(mhv,hw); base.make_rep(hw);
	scan(G, mhv, v0, SN, SE, base, mate);
	hv = base(parent[mhv]);
      }
    }
  }

  if(breakthrough) {    
    list<node> P; 
    //assert(mate[w] == nil);
    //cerr << "\naugmentation with free nodes " << num[w] << " and " << num[v0];
    //find_path(P,v,hv,label,parent,mate,source_bridge,target_bridge,num);
        
    P.push(v); P.push(w);
    find_path(P,v,v0,label,parent,mate,source_bridge,target_bridge,num);
           
    while(! P.empty())
    { node a = P.pop();
      node b = P.pop();
      mate[a] = b;
      mate[b] = a;
    }

    //forall_nodes(v,G) if (mate[v] != nil) assert(mate[mate[v]] == v);

    forall(v,T) label[v] = UNLABELED;  
    base.split(T);
    //size_of_M++; //cerr << "\nincreased size of M";          
  }

 }

  list<edge> M;
  
   forall_edges(e,G) 
   { node v = source(e);
     node w = target(e);
     if ( v != w  &&  mate[v] == w ) 
     { M.append(e);
       mate[v] = v;
       mate[w] = w;
      }
    }


  forall_nodes(v,G) OSC[v] = -1;

  int number_of_unlabeled = 0;
  node arb_u_node = 0;

  forall_nodes(v,G) 
   if ( label[v] == UNLABELED ) 
   { number_of_unlabeled++;
     arb_u_node = v;
   }

  int L = 0;
  if ( number_of_unlabeled > 0 )
  { OSC[arb_u_node] = 1;
    if (number_of_unlabeled > 2) L = 2;
    forall_nodes(v,G) 
      if ( label[v] == UNLABELED && v != arb_u_node ) OSC[v] = L;
  }

  int K = ( L == 0? 2 : 3);

  forall_nodes(v,G)
   if ( base(v) != v && OSC[base(v)] == -1 ) OSC[base(v)] = K++;

  forall_nodes(v,G)
  { if ( base(v) == v && OSC[v] == -1 )
    { if ( label[v] == EVEN ) OSC[v] = 0;
      if ( label[v] == ODD  ) OSC[v] = 1;
    }
    if ( base(v) != v ) OSC[v] = OSC[base(v)];
  }

 return M;
}


node find_aug_path(const graph& G, node v, node v0, node_partition& base, const node_array<node>& mate,
		   node_array<int>&  even_time, int even_count, node_array<LABEL>& label,
		   node_array<node>& parent, node_array<node>& source_bridge,
		   node_array<node>& target_bridge, list<node>& T, const node_array<int>& num){
  edge e; node w; //cerr << "\ncalling find aug path for " << num[v];
  forall_inout_edges(e,v){
    w = G.opposite(e,v); 
    if( v == w ) continue;
    if( mate[w] == nil && label[w] == UNLABELED ){//cerr << "\nextending path by " << num[w] << ". Completed an augmenting path";
      parent[w] = v;
      T.append(w);
      label[w] = ODD;
      return w;
    }
  }
  // no immediate break-through
  forall_inout_edges(e,v){
    w = G.opposite(e,v);
    if (label[base(w)] == ODD || w == v) continue;
    if (label[base(w)] == UNLABELED){
      label[w] = ODD; parent[w] = v; T.append(w);
      node mw = mate[w];
      label[mw] = EVEN; T.append(mw); even_time[mw] = even_count++;
      //cerr << "\nextended path by " << num[w] << " and " << num[mw];
      node s = find_aug_path(G,mw,v0,base,mate,even_time,even_count,label,parent, source_bridge,target_bridge,T,num);
      if(s != nil)
	return s;
    }
    else{
      node bv = base(v);
      node bw = base(w);
      list<node> tmp;   
      if (even_time[bv] < even_time[bw]){ //blossom_step along forward edge
        // walk down from bw to bv and perform unions
        //cerr << "\nformed a blossom with edge " << num[v] << "  " << num[w] <<". Will walk from " << num[bw]  << " to " << num[bv];
        while(bw != bv){
	  node mate_bw = mate[bw]; //cerr << "\nmate_bw = " << num[mate_bw];
	  base.union_blocks(bw,mate_bw);
	  bw = base(parent[mate_bw]);
	  base.union_blocks(mate_bw,bw); //base.make_rep(bv);
	  tmp.push_front(mate_bw); source_bridge[mate_bw] = w; target_bridge[mate_bw] = v;
          //cerr << "\nnew bw = " << num[bw];
        }
        base.make_rep(bv);
        forall(w,tmp){
	  node s = find_aug_path(G,w,v0,base,mate,even_time,even_count,label,parent, source_bridge,target_bridge,T,num);
          if(s != nil)
	    return s;
	}
      }
    }
  }
  //cerr <<"\nreturning unsuccessfully from call " << num[v];
  return nil;
}
      
    

list<edge> MAX_CARD_MATCHING_KECECIOGLU1(const graph& G, node_array<int>& OSC, 
                                                         int heur)
{ 
   // sn 01/2025
   // Variant of  Keeceglioglu  ?
   // seems to be slower than the original one (see below)
  
    node_array<node> mate(G,nil);
    node_partition base(G);    // now base(v) = v for all nodes v

    node_array<LABEL>  label(G,UNLABELED);     
    node_array<node> parent(G);

    node_array<node> source_bridge(G);
    node_array<node> target_bridge(G);

    node_array<int>  even_time(G);
    int even_count = 0;
  
    if( heur == 1){
      edge e;
      forall_edges(e,G){
	node v = G.source(e); node w = G.target(e);
        if( v != w && mate[v] == nil && mate[w] == nil){
	  mate[v] = w; 
          mate[w] = v; 
        }
      }
    }

    node v0, v, w; edge e; int count = 1;

    node_array<int> num(G); forall_nodes(v,G) num[v] = count++;
    //forall_nodes(v,G) //cerr << "\n" << num[v] << " " << (mate[v] == nil? 0 : num[mate[v]]);

forall_nodes(v0,G)
{ if ( mate[v0] != nil ) continue;

  list<node> T; label[v0] = EVEN; T.append(v0); even_time[v0] = even_count++;
  //cerr << "\n\n\ngrowing a tree with root " << num[v0];
  
  if( (w = find_aug_path(G,v0,v0,base,mate,even_time,even_count,label,parent, source_bridge,target_bridge,T,num)) != nil){
    // have found a augmenting path from v0 to w; augment it.
    list<node> P; //assert(mate[w] == nil);
    //cerr << "\naugmentation with free nodes " << num[w] << " and " << num[v0];
           //find_path(P,v,hv,label,parent,mate,source_bridge,target_bridge);
    node pw = parent[w];  
    P.push(w); P.push(pw); //cerr << "\nwill mate " << num[w] << " and " << num[pw];
    find_path(P,pw,v0,label,parent,mate,source_bridge,target_bridge,num);
           
    while(! P.empty())
    { node a = P.pop();
      node b = P.pop();
      mate[a] = b;
      mate[b] = a;
    }

    forall_nodes(v,G) if (mate[v] != nil) assert(mate[mate[v]] == v);

    forall(v,T) label[v] = UNLABELED;  
    base.split(T);
  }
 }

  list<edge> M;
  
   forall_edges(e,G) 
   { node v = source(e);
     node w = target(e);
     if ( v != w  &&  mate[v] == w ) 
     { M.append(e);
       mate[v] = v;
       mate[w] = w;
      }
    }


  forall_nodes(v,G) OSC[v] = -1;

  int number_of_unlabeled = 0;
  node arb_u_node = 0;

  forall_nodes(v,G) 
   if ( label[v] == UNLABELED ) 
   { number_of_unlabeled++;
     arb_u_node = v;
   }

  int L = 0;
  if ( number_of_unlabeled > 0 )
  { OSC[arb_u_node] = 1;
    if (number_of_unlabeled > 2) L = 2;
    forall_nodes(v,G) 
      if ( label[v] == UNLABELED && v != arb_u_node ) OSC[v] = L;
  }

  int K = ( L == 0? 2 : 3);

  forall_nodes(v,G)
   if ( base(v) != v && OSC[base(v)] == -1 ) OSC[base(v)] = K++;

  forall_nodes(v,G)
  { if ( base(v) == v && OSC[v] == -1 )
    { if ( label[v] == EVEN ) OSC[v] = 0;
      if ( label[v] == ODD  ) OSC[v] = 1;
    }
    if ( base(v) != v ) OSC[v] = OSC[base(v)];
  }

  //forall_nodes(v,G) //cerr << "\nnodes and their OSC-label " << num[v] << " " << OSC[v];

 return M;
}


LEDA_END_NAMESPACE
