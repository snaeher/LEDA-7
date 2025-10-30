/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  node_partition.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_NODE_PARTITION_H
#define LEDA_NODE_PARTITION_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 722236
#include <LEDA/internal/PREAMBLE.h>
#endif



//------------------------------------------------------------------------------
// node partitions 
//------------------------------------------------------------------------------

#include <LEDA/graph/graph.h>

/*{xxxManpage {node_partition_T} {graph} {Node Partitions}}*/

/*{\Manpage {node_partition} {} {Node Partitions}}*/

#include <LEDA/core/partition.h>

LEDA_BEGIN_NAMESPACE

template <class graph_t=graph, 
          class ItemArray=node_array<partition_item,graph_t> >


class __exportC node_partition_T : protected Partition<typename graph_t::node>
{
/*{\Mdefinition
An instance $P$ of the data type $node\_partition$ is a partition of the nodes
of a graph $G$.}*/

typedef typename graph_t::node node;

const graph_t& gr;

ItemArray ITEM;


public:

void init(const graph_t& G)
{ node v;
  forall_nodes(v,G) ITEM[v] = Partition<node>::make_block(v);
}

/*{\Mcreation P }*/

 node_partition_T(const graph_t& G) : gr(G),ITEM(G,nil) { init(G); }

#if 0
 node_partition(const graph& G);
#endif
/*{\Mcreate creates a |\Mname| |\Mvar| containing for every node $v$ in $G$ a 
            block $\{v\}$.}*/

~node_partition_T()               {}   

/*{\Moperations 1.2 5.0}*/

int  same_block(node v, node w)   
{ return Partition<node>::same_block(ITEM[v],ITEM[w]); }
/*{\Mopl      returns positive integer if $v$ and $w$ belong to the
              same block of |\Mvar|, 0 otherwise.}*/


void union_blocks(node v, node w) 
{ Partition<node>::union_blocks(ITEM[v],ITEM[w]); } 
/*{\Mopl      unites the blocks of |\Mvar| containing nodes
	      $v$ and $w$.}*/

void reset(node v) { Partition<node>::reset(ITEM[v]); }

void split(const list<node>& L)
{ node v;
  forall(v,L) Partition<node>::reset(ITEM[v]);
  forall(v,L) make_rep(v);
}
/*{\Mop       makes all nodes in $L$ to singleton blocks.\\
              \precond     $L$ is a union of blocks. }*/


node find(node v) 
{ return node(Partition<node>::inf(Partition<node>::find(ITEM[v]))); }
/*{\Mop       returns a canonical representative node of 
	      the block that contains node $v$.}*/


//void make_rep(node v) { Partition<node>::change_inf(ITEM[find(v)],v); }

void make_rep(node v) { 
partition_item it = Partition<node>::find(ITEM[v]); 
Partition<node>::change_inf(it,v); }
/*{\Mop       makes v the canonical representative of the block 
              containing $v$.}*/

int size(node v) 
{ return Partition<node>::size(ITEM[v]); }
/*{\Mop       returns the size of  
	      the block that contains node $v$.}*/

int number_of_blocks() 
{ return Partition<node>::number_of_blocks(); }
/*{\Mop       returns the number of blocks of |\Mvar|. }*/



node operator()(node v) { return find(v); }
/*{\Mfunop    returns |\Mvar|.find($v$). }*/

};

typedef node_partition_T<graph> node_partition;

/*{\Mimplementation
A node partition for a graph $G$ is implemented by a combination of a 
partition $P$ and a node array of $partition\_item$ associating with 
each node in $G$ a partition item in $P$. Initialization takes linear time,
union\_blocks takes time $O(1)$ (worst-case), and same\_block and find take 
time $O(\alpha(n))$ (amortized).  The cost of a split is proportional to the 
cost of the blocks dismantled. The space requirement is $O(n)$, where $n$ 
is the number of nodes of $G$. }*/


#if LEDA_ROOT_INCL_ID == 722236
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
