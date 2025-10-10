/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_plane_triang.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



// - TRIANGULATE_POINTS on a  d3-plane (points sorted)
//   look at the d2 LEDA-sources

#include <LEDA/graph/graph.h>


//using namespace leda;

#include "d3_complex.h"

#define left_bend(p,e)   (orientation(p,G[source(e)],G[target(e)]) > 0)


using std::cout;
using std::endl;


LEDA_BEGIN_NAMESPACE

void DELAUNAY_FLIPPING(d3_simpl_complex& Comp, list<Simpl_Triang>& S)
{
  Simpl_Triang T,Trl,Th;
  int tf;
  list<sc_simplex> LS1,LS2;
  sc_simplex si;
  int flipcount=0;  

  while ( !S.empty() )
  { T=S.pop();

    if (T.S1==NULL || T.S2==NULL) continue;

    if (Comp.flippable(T.S1,T.S2,Th)) 
      tf=Comp.flip(T.S1,T.S2,Th); 
    else tf=0;

    if (tf>0)
    { 
      flipcount++;

      LS1.clear(); LS2.clear();

      if (Comp.link_acd(T.S1) != T.S2) LS1.append(Comp.link_acd(T.S1)); 
      if (Comp.link_abd(T.S1) != T.S2) LS1.append(Comp.link_abd(T.S1)); 
      if (Comp.link_bcd(T.S1) != T.S2) LS1.append(Comp.link_bcd(T.S1));

      if (Comp.link_acd(T.S2) != T.S1) LS2.append(Comp.link_acd(T.S2));
      if (Comp.link_abd(T.S2) != T.S1) LS2.append(Comp.link_abd(T.S2)); 
      if (Comp.link_bcd(T.S2) != T.S1) LS2.append(Comp.link_bcd(T.S2));
     
      forall(si,LS1){ Trl.S1=si; Trl.S2=T.S1; S.push(Trl); }
      forall(si,LS2){ Trl.S1=si; Trl.S2=T.S2; S.push(Trl); }
    }
  }

  cout << flipcount << " Flips!\n";
}

void DELAUNAY_FLIPPING(d3_simpl_complex& Comp)
{
  list<Simpl_Triang> S;
  list<sc_simplex> all= Comp.all_simplices();
  Simpl_Triang Trl;

  if (all.empty()) return;

  sc_simplex akt;
  sc_simplex sother;
  forall (akt,all) Comp.set_Marker(akt,-1);
  
  queue<sc_simplex> Q;
  akt= all.head(); Comp.set_Marker(akt,-98);
  Q.append(akt);

  //visited:Marker==-98; linked:Marker==-99

  while (! Q.empty()){
   akt=Q.pop(); Comp.set_Marker(akt,-99);
   sother= Comp.link_abd(akt);
   if (sother!=NULL && Comp.get_Marker(sother) !=-99){ 
    Trl.S1=akt; Trl.S2=sother; S.append(Trl);
    if (Comp.get_Marker(sother)==-1) { Q.append(sother); Comp.set_Marker(sother,-98); }
   }
   sother= Comp.link_acd(akt);
   if (sother!=NULL && Comp.get_Marker(sother) !=-99){
    Trl.S1=akt; Trl.S2=sother; S.append(Trl);
    if (Comp.get_Marker(sother)==-1) { Q.append(sother); Comp.set_Marker(sother,-98); }   
   }
   sother= Comp.link_bcd(akt);  
   if (sother!=NULL && Comp.get_Marker(sother) !=-99){
    Trl.S1=akt; Trl.S2=sother; S.append(Trl);
    if (Comp.get_Marker(sother)==-1) { Q.append(sother); Comp.set_Marker(sother,-98); }
   }
  }

  //cout << "SIMPL_TRIANGLES:" << S.size() << "\n";
  if (S.size()>0) DELAUNAY_FLIPPING(Comp,S);
}

edge TRIANGULATE_POINTS(const list<rpoint_zg>& L0, GRAPH<rpoint_zg,sc_simplex>& G, d3_simpl_complex& C)
// input list has to be sorted
{ 
  G.clear();
  if (L0.empty()) return nil;

  list<rpoint_zg> L = L0;

  rpoint_zg first,sec,act;

  rpoint_zg last_p = L.pop();            
  node  last_v = G.new_node(last_p);
  first=last_p;

  while (!L.empty() && last_p == L.head()) L.pop();
  edge x,y;
  list<edge> vorn;

  if (!L.empty())
  { last_p = L.pop();
    sec= last_p;
    node v = G.new_node(last_p);
    x = G.new_edge(last_v,v,0); G[x]=NULL; vorn.append(x);
    y = G.new_edge(v,last_v,0); G[y]=NULL; 
    G.set_reversal(x,y);
    last_v = v;
  }
   
  rpoint_zg p;
  sc_simplex sakt,slast=NULL;
/*
  edge first1=x,first2=y; 
  bool fflag=rue;
*/
 int zlr=0;
  if ((!L.empty()) && collinear(first,sec,L.head())) { 
       act=L.pop();
       zlr++;
       node v= G.new_node(act);
       x= G.new_edge(last_v,v,0); G[x]=NULL; vorn.append(x);
       y= G.new_edge(v,last_v,0); G[x]=NULL;
       G.set_reversal(x,y);
       last_v=v;
  }
  //cout << zlr << "+2 collineare points!\n";

  forall(p,L) 
  { if (p == last_p) continue; 

    edge e =  G.last_adj_edge(last_v);

    last_v = G.new_node(p);
    last_p = p;
    slast = NULL;

    // walk up to upper tangent
    do e = G.pred_face_edge(e); while (left_bend(p,e));

    // walk down to lower tangent and triangulate
    do { edge succ_e = G.succ_face_edge(e);
         x = G.new_edge(succ_e,last_v,0,0);
         y = G.new_edge(last_v,source(succ_e),0);
         G.set_reversal(x,y);

         if (left_bend(p,succ_e)) {
            sakt=C.add_simplex((G[source(succ_e)]).v,(G[target(succ_e)]).v,(G[last_v]).v);
            G[x]=sakt;
            G[y]=sakt;
            if (slast!=NULL){
              C.set_link(slast,sakt,1); C.set_link(sakt,slast,2);
            }
            if (G[succ_e]!=NULL){
             C.set_link(sakt,G[succ_e],(G[source(succ_e)]).v,(G[target(succ_e)]).v,(G[target(succ_e)]).v);
            }
            else { G[succ_e]=sakt; G[G.reversal(succ_e)]=sakt; }

            slast=sakt;
         }
         else { G[x]=slast; G[y]=slast; }

         e = succ_e;
       } while (left_bend(p,e));
   }

  edge hull_edge = G.last_edge();

  cout << "Nodes:" << G.number_of_nodes() << " Edges:" << G.number_of_edges() << "\n";
  return hull_edge;
}

edge TRIANGULATE_POINTS(list<sc_vertex>& L0, GRAPH<rpoint_zg,sc_simplex>& G,d3_simpl_complex& C,int wert)
{
 list<rpoint_zg> L1;
 sc_vertex v;
 rpoint_zg akt;
 rat_point rp;

 forall(v,L0){
   switch(wert){
   case 0:
     rp=(C.pos(v)).project_xy();
     break;
   case 1:
     rp=(C.pos(v)).project_xz();
     break;
   case 2:
     rp=(C.pos(v)).project_yz();
     break;
   }
   akt.p=rp; 
   akt.v=v;
   L1.append(akt);
 }

 return TRIANGULATE_POINTS(L1,G,C);
}

edge build_simp_comp(list<d3_rat_point>& L, d3_simpl_complex& C,int wert)
{
 d3_rat_point p;
 list<sc_vertex> LV;
 sc_vertex vz;

 forall(p,L){
  vz= C.add_vertex(p);
  LV.append(vz);
 }

 GRAPH<rpoint_zg,sc_simplex> G;
 edge he=TRIANGULATE_POINTS(LV,G,C,wert);
 return he;
}

LEDA_END_NAMESPACE
