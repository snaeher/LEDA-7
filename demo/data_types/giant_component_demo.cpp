/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  giant_component_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:20 $


#include <LEDA/core/partition.h>
#include <LEDA/core/IO_interface.h>
#include <LEDA/core/array.h>
#include <LEDA/core/sortseq.h>

using namespace leda;



int main(){

  
  IO_interface I("The Giant Component");

  I.write_demo("This demo illustrates the phenomenon of the giant component \
  in a random graph.");

  I.write_demo("We perform the following chance experiment. We start with a \
  partition of n items arranged into n singleton blocks and then repeat the \
  following step until the largest block has reached size n/2. \
  We choose two items \
  at random and merge the blocks containing them (this has no effect if the \
  two items belong already to the same block).");

  I.write_demo("During the experiment we keep track of the \
  size of the two largest blocks.");
  int n = I.read_int("n = ",1000000);


  partition P;
  array<partition_item> Item(n);
  sortseq<int,int> freq;

  for (int i = 0; i < n; i++) Item[i] = P.make_block();

  int iteration = 0; int step = 1; int max_size = 1;
  freq.insert(1,n);

  while ( max_size < n/2 )
  { int v = rand_int(0,n-1);
    int w = rand_int(0,n-1);
    iteration++;
    if ( P.same_block(Item[v],Item[w]) ) continue;

    seq_item it = freq.lookup(P.size(Item[v]));
    freq[it]--;
    if ( freq[it] == 0 ) freq.del_item(it);

    it = freq.lookup(P.size(Item[w]));
    freq[it]--;
    if ( freq[it] == 0 ) freq.del_item(it);

    P.union_blocks(Item[v],Item[w]);

    int size = P.size(Item[v]);
    
    it = freq.lookup(size);
    if (it) freq[it]++; else freq.insert(size,1);

    it = freq.max();
    max_size = freq.key(it);
    int second_size = freq.key(freq.pred(it));
    
    while (max_size >= step*n/100 )
    { 
      I.write_demo("The maximal block size jumped above 0." + 
          string("%d",step) + "n after ",iteration," iterations.");
      I.write_demo(1,"The maximal size of a block is ",max_size);
      I.write_demo(1,"and the second largest size of a block is ",second_size,".");
      if ( step%5 == 0 ) I.stop();

      step++;
    }
  }
  return 0;
}
    

