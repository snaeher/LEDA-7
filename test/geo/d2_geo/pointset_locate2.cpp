/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  pointset_locate2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/point_set.h>
#include <LEDA/geo/point_set.h>

// point generation ...
#include <LEDA/geo/random_point.h>
#include <LEDA/geo/random_rat_point.h>

#include<LEDA/core/h_array.h>
#include<LEDA/core/random_source.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


typedef rat_point      POINT;
typedef rat_segment    SEGMENT;
typedef rat_point_set  POINT_SET;

/*
typedef point          POINT;
typedef segment        SEGMENT;
typedef point_set      POINT_SET;
*/

// get enumber edges from S and insert them into LE
void get_edges(const POINT_SET& S,list<edge>& LE,int enumber)
{
    LE.clear();
    const list<node>& LN = S.all_nodes();
    int zl,i,act,cnt=0;
    node v;
    edge e;
    if (LN.size() < enumber) zl = LN.size(); else zl = enumber;
    
    random_source R(0,LN.size()-1);
    h_array<int,int> H;
    
    for(i=0;i<zl;i++){
      act = R();   
      if (H.defined(act)) zl++;
      else H[act]=1;
    }     
    
    forall(v,LN){
      if (H.defined(cnt)){
        e = S.first_adj_edge(v);
	if (e!=NULL) LE.append(e);
      }
      cnt++;
    }
}


void sample(const POINT_SET& S, POINT_SET& Plittle, int numb, map<node,node>& MPP)
{
  map<POINT, node> M_to_source;
  
  list<POINT> LP;
  node v;
  int cnt = 0;
  
  // get a sample ...
  forall_nodes(v,S){
   if (cnt==0) {
     POINT pact = S.pos(v);
     LP.append(pact);
     M_to_source[pact] = v;
   }
   cnt++;
   if (cnt==20) cnt=0;
  }

  Plittle.init(LP);
  
  // fill MPP
  
  forall_nodes(v, Plittle){  
    node vbig    = M_to_source[Plittle[v]];
    MPP[v] = vbig;
  }

}


int main(void){
 // generate a "big" point set ...
 list<POINT> LP;
 
 cout << "Generating point set with 100000 points ...\n"; cout.flush();
 
 random_points_in_square(100000, 1000, LP);

 POINT_SET S(LP);
 
 // generate 10000 points for locate operation ...
 list<POINT> LOC;
 
 random_points_in_square(10000, 1100, LOC);
 
 // locate ...
 POINT piter;
 list<edge> res1, res2, res3;
 
 cout << "conventional locate:\n" << flush;
 
 float tm = used_time();
 
 forall(piter,LOC){
   edge e = S.locate(piter);
   res1.append(e);
 }
 
 cout << "Time for conventional locate:" << used_time(tm) << "\n";
 
 cout << "new locate (with start edges):\n" << flush;
 
 float tm2 = used_time();
 
 int enumber = 30;
 list<edge> LE;
 get_edges(S,LE,enumber);
 
 tm = used_time();
 
 forall(piter,LOC){
   
   edge e = S.locate(piter, LE);
   
   // perform a check ...
   //S.check_locate(e,piter);
   
   res2.append(e);
 }
 
 cout << "Time for new locate with start edge :" << used_time(tm) << "\n"; 
 cout << "Time for new locate with start edge + preprocessing:" << used_time(tm2) << "\n"; 
 
 POINT_SET Plittle;
 map<node,node> MPP; // Plittle -> S
 
 //cout << "before sample !\n" << flush;
 sample(S, Plittle, 15, MPP);
 //cout << "after sample !\n" << flush;
  
 tm = used_time();
 
 list<edge> LE2;
 get_edges(Plittle,LE2,10); 
 
 forall(piter,LOC){
   edge ehelp = Plittle.locate(piter, LE2);
   node v = MPP[source(ehelp)];
   edge e = S.locate(piter, S.first_adj_edge(v));
   res3.append(e);
   
   //S.check_locate(e,piter);
 }
 
 cout << "Time for hierarchical locate :" << used_time(tm) << "\n";  
 
 return 0;
}
