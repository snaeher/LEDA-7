/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_delau_alg.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// Delaunay-Triangulierung in 3d unter Verwendung der 
// 3d-Simplexkomplexklasse d3_simpl_complex

#include <LEDA/system/basic.h>

//using namespace leda;

#include "d3_complex.h"


using std::cout;
using std::endl;


LEDA_BEGIN_NAMESPACE

void calculate_triang(list<d3_rat_point>& L,d3_simpl_complex& Comp,int wert)
{ build_simp_comp(L,Comp,wert);
  DELAUNAY_FLIPPING(Comp);
  return;                                                            
}

sc_simplex spec_case(list<d3_rat_point>& L,d3_simpl_complex& Comp,
                     int fall,int wert)
{ 
 sc_simplex last=NULL;
 d3_rat_point p1,p2;
 list_item it;
 sc_vertex v1,v2;
 sc_simplex s1,s2;
 s1=NULL;s2=NULL;

 if (fall==1){
  p1=L.contents(L.first());
  v1=Comp.add_vertex(p1);
  forall_items(it,L){ 
    if (it != L.first()){
     p2=L.contents(it); v2=Comp.add_vertex(p2);
     s2=Comp.add_simplex(v1,v2,v2);
     if (s1!=NULL){
       Comp.set_link(s1,s2,1); Comp.set_link(s2,s1,3);
     }
     s1=s2;
     v1=v2;
    }
  }
  last=s2;
 }
 else { // coplanar case
  calculate_triang(L,Comp,wert); // compute triangulation 
 }
 
 return last;
}


int get_pro_plane(d3_rat_point A,d3_rat_point B,d3_rat_point C)
{
 // get projection plane
 
 rat_point ap=A.project_xy(); 
 rat_point bp=B.project_xy(); 
 rat_point cp=C.project_xy();
 if (! collinear(ap,bp,cp)) return 0;

 ap=A.project_xz(); 
 bp=B.project_xz(); 
 cp=C.project_xz();
 if (! collinear(ap,bp,cp)) return 1;

 return 2;
}

sc_simplex comp_initial_tr(list<d3_rat_point>& L, d3_simpl_complex& Comp,
                           d3_rat_point& lastp, list<sc_simplex>& trhull)
{
 // compute start-DT, put it into  C
 
 sc_simplex last;
 d3_rat_point A,B,C,D;
 sc_vertex  v1,v2,v3,v4;

 if (L.size() < 4){
  int sz=L.size();
  switch (sz){
  case 1:
   A=L.pop();
   v1=Comp.add_vertex(A);
   break;
  case 2:
   // degen. simplex
   A=L.pop(); B=L.pop();
   v1=Comp.add_vertex(A); v2=Comp.add_vertex(B);
   last=Comp.add_simplex(v1,v2,v2);
   break;
  case 3:
   A=L.pop(); B=L.pop(); C=L.pop();
   v1=Comp.add_vertex(A); v2=Comp.add_vertex(B); v3=Comp.add_vertex(C);
   last=Comp.add_simplex(v1,v2,v3);
   break;
  }
  return NULL;
 }

 // normal case
 list<d3_rat_point> cpl;
 
 A=L.pop(); B=L.pop(); C=L.pop();
 cpl.append(A); cpl.append(B);

 while ( collinear(A,B,C) && !L.empty() ){
  cpl.append(C);
  C=L.pop(); 
 }
 cpl.append(C);
 if (L.empty() && collinear(A,B,C)) { last=spec_case(cpl,Comp,1,0); return last; } // all points collinear
 if (L.empty()) {
   int wert=get_pro_plane(A,B,C);
   last=spec_case(cpl,Comp,2,wert); return last; 
 }  

 D=L.pop();
 while ( coplanar(A,B,C,D) && !L.empty() ){
   cpl.append(D);
   D=L.pop();
 }
 if (L.empty() && coplanar(A,B,C,D)) cpl.append(D);
 lastp=D;

 if (L.empty() && cpl.size()>3) { 
   int wert=get_pro_plane(A,B,C);
   last=spec_case(cpl,Comp,2,wert); return last; 
 } // all points coplanar 

 if (cpl.size() == 3){ //first 4 points not coplanar

   v1=Comp.add_vertex(A); v2=Comp.add_vertex(B); v3=Comp.add_vertex(C); v4=Comp.add_vertex(D);
    
   sc_simplex s1,s2,s3,s4,s5;
   s1=Comp.add_simplex(v1,v2,v3,v4);
   s2=Comp.add_simplex(v1,v2,v3); s3=Comp.add_simplex(v1,v2,v4);
   s4=Comp.add_simplex(v2,v3,v4); s5=Comp.add_simplex(v1,v3,v4);

   // neighboursettings
   // S1
   Comp.set_link(s1,v4,s2,NULL); Comp.set_link(s1,v3,s3,NULL);
   Comp.set_link(s1,v1,s4,NULL); Comp.set_link(s1,v2,s5,NULL);
   // others
   Comp.set_link(s2,v3,s3,v4); Comp.set_link(s2,v1,s4,v4);
   Comp.set_link(s2,v2,s5,v4); Comp.set_link(s3,v1,s4,v3);
   Comp.set_link(s3,v2,s5,v3); Comp.set_link(s4,v2,s5,v1);
   
   trhull.append(s2); trhull.append(s3); trhull.append(s5); trhull.append(s4); // hullsimplices
   return s4;
 }
 else {
  int wert=get_pro_plane(A,B,C);
  last=spec_case(cpl,Comp,2,wert);

  sc_vertex DV=Comp.add_vertex(D);
  sc_simplex sakt=0;
  sc_simplex sneu=0;
  list<sc_simplex> akt=Comp.Sil;
  list<sc_simplex> NL; 

  forall(sakt,akt){
    sneu=Comp.add_simplex(Comp.vertex_a(sakt),Comp.vertex_b(sakt),Comp.vertex_c(sakt));
    NL.append(sneu);
    Comp.set_link(sakt,sneu,4);
    Comp.set_link(sneu,sakt,4);
    Comp.set_hull_info(sakt,false);
    Comp.set_vertex(sakt,DV,4);
  }

  sc_simplex geg, pt;  
  list<sc_simplex> NHTR;

  forall(sakt,NL){ 
    geg= Comp.link_abc(sakt);
    pt= Comp.link_abd(geg);
    if (pt !=NULL) Comp.set_link(sakt,Comp.link_abc(pt),3);
    else {
     sneu=Comp.add_simplex(Comp.vertex_a(sakt),Comp.vertex_b(sakt),DV);
     trhull.append(sneu);
     Comp.set_link(sakt,sneu,3); Comp.set_link(sneu,sakt,3);
     Comp.set_link(sneu,geg,4); Comp.set_link(geg,sneu,3);
  
     Comp.set_vert_sim_link(Comp.vertex_a(sneu),NULL);
     Comp.set_vert_sim_link(Comp.vertex_b(sneu),NULL);
     NHTR.append(sneu);
    }
    pt=Comp.link_acd(geg);
    if (pt !=NULL) Comp.set_link(sakt,Comp.link_abc(pt),2); 
    else {
     sneu=Comp.add_simplex(Comp.vertex_a(sakt),Comp.vertex_c(sakt),DV);
     trhull.append(sneu);
     Comp.set_link(sakt,sneu,2); Comp.set_link(sneu,sakt,3);     
  
     Comp.set_link(sneu,geg,4); Comp.set_link(geg,sneu,2);
     Comp.set_vert_sim_link(Comp.vertex_a(sneu),NULL);
     Comp.set_vert_sim_link(Comp.vertex_b(sneu),NULL);
 
     NHTR.append(sneu); 
    }
    pt= Comp.link_bcd(geg);
    if (pt !=NULL) Comp.set_link(sakt,Comp.link_abc(pt),1); 
    else {
     sneu=Comp.add_simplex(Comp.vertex_b(sakt),Comp.vertex_c(sakt),DV);
     trhull.append(sneu);
     Comp.set_link(sakt,sneu,1); Comp.set_link(sneu,sakt,3);     

     Comp.set_link(sneu,geg,4); Comp.set_link(geg,sneu,1);
     Comp.set_vert_sim_link(Comp.vertex_a(sneu),NULL);
     Comp.set_vert_sim_link(Comp.vertex_b(sneu),NULL);
 
     NHTR.append(sneu); 
    } 
  }
  last=sneu; //a new hulltr.

  // setting links...
  sc_simplex sa,sb;

  forall(sakt,NHTR){
    sa= Comp.get_simplex(Comp.vertex_a(sakt)); sb=Comp.get_simplex(Comp.vertex_b(sakt));
    if (sa !=NULL){ 
     Comp.set_link(sakt,sa,2);
     if (Comp.vertex_a(sakt)==Comp.vertex_b(sa)) Comp.set_link(sa,sakt,1); else Comp.set_link(sa,sakt,2);
    }
    else Comp.set_vert_sim_link(Comp.vertex_a(sakt),sakt);
   
    if (sb !=NULL){
     Comp.set_link(sakt,sb,1);
     if (Comp.vertex_b(sakt)==Comp.vertex_a(sb)) Comp.set_link(sb,sakt,2); else Comp.set_link(sb,sakt,1);
    } 
    else  Comp.set_vert_sim_link(Comp.vertex_b(sakt),sakt);
 
  }  

 }
 
 return last;
}


int orientation(d3_simpl_complex& Co, sc_vertex a, sc_vertex b,
                                      sc_vertex c, d3_rat_point d)
{
 return orientation (Co.pos(a),Co.pos(b),Co.pos(c),d);
}

int test_vis(d3_simpl_complex& C,sc_simplex vis, sc_simplex other, 
             sc_vertex  c1, sc_vertex  c2, d3_rat_point p,d3_rat_point pv)
{
 if (other==NULL) return -1;

 d3_rat_point cp1,cp2,cp3;
 cp1= C.pos(c1); cp2=C.pos(c2);

 cp3= C.pos(C.find_other(other,c1,c2,c2));
 int o1,o2;
 o1=orientation(pv,cp1,cp2,p);
 o2=orientation(cp1,cp3,cp2,p);
 
 if (o1==o2) return 1; else return -1; 

}


sc_simplex really_vis(d3_simpl_complex& Cm,sc_simplex cand,d3_rat_point p,
                      list<sc_simplex>& trhull)
{
 sc_simplex last=NULL;
 sc_simplex pt;
 d3_rat_point a,b,c,other1;

 do {

  a=Cm.pos(Cm.vertex_a(cand)); b=Cm.pos(Cm.vertex_b(cand)); c=Cm.pos(Cm.vertex_c(cand));
  pt=Cm.link_abd(cand);
  other1= Cm.pos(Cm.find_other(pt,Cm.vertex_a(cand),Cm.vertex_b(cand),Cm.vertex_b(cand)));

  int o1,o2;
  o1=orientation(a,b,c,p); o2=orientation(a,b,c,other1);

  if (o2==0){
   pt= Cm.link_acd(cand);
   other1= Cm.pos(Cm.find_other(pt,Cm.vertex_a(cand),Cm.vertex_c(cand),Cm.vertex_c(cand)));
   o2=orientation(a,b,c,other1);

   if (o2==0){
    pt=Cm.link_bcd(cand);
    other1= Cm.pos(Cm.find_other(pt,Cm.vertex_b(cand),Cm.vertex_c(cand),Cm.vertex_c(cand)));    
    o2=orientation(a,b,c,other1);

    if (o2==0){
     pt=Cm.link_abc(cand);
     other1= Cm.pos(Cm.find_other(pt,Cm.vertex_a(cand),Cm.vertex_b(cand),Cm.vertex_c(cand)));  
     o2=orientation(a,b,c,other1);      
    }
   }
  }
 
  if (o1==0 || o1==o2 || o2==0) { // problem...
   // not visible...
   if (! trhull.empty()) cand=trhull.pop();
   else  last=cand; 

  }
  else {
   last=cand;
  }

 } while (last != cand);

 return last; 
}


void find_visible_triangles(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,sc_simplex n,
                            d3_rat_point p,int counter,list<sc_simplex>& trhull)
// find the from p visible hulltrs., put them into Trl
{
 Trl.clear(); Border.clear();
 n=really_vis(C,n,p,trhull);
 
 Trl.append(n);
 C.set_Marker(n,counter); //mark first visible simplex ...

 // start searching ...
 queue<sc_simplex> Q;
 sc_simplex sakt;
 Simpl_Triang T;
 Q.append(n);
 sc_simplex s1,s2,s3;
  
 while (! Q.empty()){
   sakt=Q.pop();

   s1= C.link_abd(sakt); s2=C.link_bcd(sakt); s3=C.link_acd(sakt);

   if (s1!=NULL && C.get_Marker(s1) != counter){
    if (test_vis(C,sakt,s1,C.vertex_a(sakt),C.vertex_b(sakt),p, C.pos(C.vertex_c(sakt)) ) >0) { 
      Trl.append(s1); Q.append(s1); 
      C.set_Marker(s1,counter);
    }
    else { //bordertriangle...
      T.S1=s1; T.S2=sakt; T.A=C.vertex_a(sakt); T.B=C.vertex_b(sakt); 
      T.C=C.vertex_c(sakt); 
      Border.append(T);
    }
   }
   if (s2!=NULL && C.get_Marker(s2) != counter){
    if (test_vis(C,sakt,s2,C.vertex_b(sakt),C.vertex_c(sakt),p, C.pos(C.vertex_a(sakt)) ) >0) {
      Trl.append(s2); Q.append(s2);
      C.set_Marker(s2,counter);
    }
    else { //bordertriangle...
      T.S1=s2; T.S2=sakt; T.A=C.vertex_b(sakt); T.B=C.vertex_c(sakt); 
      T.C=C.vertex_a(sakt); 
      Border.append(T);
    }
   }
   if (s3!=NULL && C.get_Marker(s3) != counter){
    if (test_vis(C,sakt,s3, C.vertex_a(sakt),C.vertex_c(sakt),p, C.pos(C.vertex_b(sakt)) ) >0) {
      Trl.append(s3); Q.append(s3);
      C.set_Marker(s3,counter);
    }
    else { //bordertriangle...
      T.S1=s3; T.S2=sakt; T.A=C.vertex_a(sakt); T.B=C.vertex_c(sakt); 
      T.C=C.vertex_b(sakt); 
      Border.append(T);
    }
   }
 }

}

void check_htr(list<sc_simplex>& trhull)
{
  /*
 sc_simplex sakt;
 forall(sakt,trhull){
  assert(sakt->HULL_TR);
  assert(sakt->abc->HULL_TR);
  assert(sakt->abd->HULL_TR);
  assert(sakt->acd->HULL_TR);
  assert(sakt->bcd->HULL_TR);
 }
 */
}


sc_simplex triangulate_new(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,sc_vertex PV,
                         queue<Simpl_Triang>& TQ,list<sc_simplex>& trhull)
{

 sc_simplex last;
 sc_simplex akt;
 sc_simplex akt_abc;
 d3_rat_point p;

 p=C.pos(PV);

 // change triangles to real simplices
 forall(akt,Trl){
    C.set_hull_info(akt,false);
    C.set_vertex(akt,PV,4);
    akt_abc=C.link_abc(akt); 
    if (akt_abc!=NULL) TQ.append(Simpl_Triang(akt,akt_abc,C.vertex_a(akt),C.vertex_b(akt),C.vertex_c(akt)));
 }

 Simpl_Triang T;
 sc_simplex newsim;
 sc_simplex sa,sb; 

 
 forall(T,Border){ C.set_vert_sim_link(T.A,NULL); C.set_vert_sim_link(T.B,NULL); } 

 forall(T,Border){
  sa=C.get_simplex(T.A); sb=C.get_simplex(T.B);  
  newsim= C.add_simplex(T.A,T.B,PV); trhull.append(newsim);
  if (sa !=NULL){ //link
    C.set_link(newsim,sa,2);
    if (C.vertex_a(newsim) == C.vertex_b(sa)) C.set_link(sa,newsim,1); else C.set_link(sa,newsim,2);
  }
  if (sb !=NULL){ //link
    C.set_link(newsim,sb,1);
    if (C.vertex_b(newsim) == C.vertex_a(sb)) C.set_link(sb,newsim,2); else C.set_link(sb,newsim,1);
  }

  C.set_vert_sim_link(T.A,newsim); C.set_vert_sim_link(T.B,newsim);

  C.set_link(newsim,T.S1,3); 
  C.update_link(T.S1,T.S2,newsim);
  C.update_link(T.S2,T.S1,newsim);
  C.set_link(newsim,T.S2,4);
 }


 last= C.get_hulltriangle();
 return last;
} 

void delaunay_flip(d3_simpl_complex& C,queue<Simpl_Triang>& TQ,sc_vertex PV)
// change complex, so that all simplices in the complex get local-delauany property
{
 Simpl_Triang Trl;
 sc_simplex S1,S2;
 //int wert;

 while (! TQ.empty()){ 
  Trl= TQ.pop();
  // still there ?
  S1=Trl.S1; S2=Trl.S2;
  if (S1!=NULL && S2!=NULL && C.complex_of(S1)==&C && C.complex_of(S2)==&C){

    if (C.flippable(S1,S2,Trl)){ // common triangle flippable ?
         /*wert= */ C.flip(S1,S2,TQ,Trl,PV);
    }

  }
 }

}


sc_simplex update_tr(d3_simpl_complex& C,d3_rat_point p,int counter,sc_simplex n,list<sc_simplex>& trhull, bool dtr)
{
 sc_simplex last;
 sc_vertex PV;
 list<sc_simplex> Trl;
 list<Simpl_Triang> Border;
 queue<Simpl_Triang> TQ;

 find_visible_triangles(C,Trl,Border,n,p,counter,trhull); 

 TQ.clear(); 
 trhull.clear();

 PV=C.add_vertex(p); 
 last=triangulate_new(C,Trl,Border,PV,TQ,trhull);
 
 if (dtr) delaunay_flip(C,TQ,PV);                      

 if  (! C.get_hull_info(last)) cout << "error! last no hull_tr!\n";

 return last;
}

void compute_tr(list<d3_rat_point>& L, d3_simpl_complex& C,bool flipflag)
// flipflag true... Delaunay Triangulation, false..  Triangulation
{
 
 int counter=0;
 sc_simplex last;
 d3_rat_point lastp;
 list<sc_simplex> trhull; 

 last=comp_initial_tr(L,C,lastp,trhull); //compute start DT

 while (!L.empty()){
    counter++;
    d3_rat_point p = L.pop();
    last=update_tr(C,p,counter,last,trhull,flipflag);
 }
 
}

void D3_TRIANG(const list<d3_rat_point>& L0, d3_simpl_complex& C)
{
  if (L0.empty()) return;
  list<d3_rat_point> L = L0;

  L.sort(); L.unique();
  compute_tr(L,C,false);  
}

void D3_TRIANG(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& G)
{
  G.clear();
  d3_simpl_complex C;
  D3_TRIANG(L0,C);
  C.compute_graph(G);
}

void D3_DELAU(const list<d3_rat_point>& L0, d3_simpl_complex& C)
{
  if (L0.empty()) return;
  list<d3_rat_point> L = L0;

  L.sort();
  L.unique();
  compute_tr(L,C,true);  
}

void D3_DELAU(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& G)
{
  G.clear();
  d3_simpl_complex C;
  D3_DELAU(L0,C);
  C.compute_graph(G);
}

void D3_VORONOI(const list<d3_rat_point>& L0, GRAPH<d3_rat_sphere,int>& G)
{
  G.clear();
  GRAPH<d3_rat_point,int> H;
  d3_simpl_complex C;

  D3_DELAU(L0,C);
  C.compute_voronoi(H,true);
}


void to_vdnb(GRAPH<d3_rat_point,int>& H,node nd,GRAPH<d3_rat_point,int>& outgr)
// H[nd] and H[nd] neighbours to outgr ...
{
 outgr.clear();
 node md=outgr.new_node(H[nd]);
 node v,w;
 edge e1,e2;

 forall_adj_nodes(v,nd){
   w=outgr.new_node(H[v]);
   e1=outgr.new_edge(md,w); e2=outgr.new_edge(w,md);
   outgr.set_reversal(e1,e2);
 } 
}



void to_vorocell(d3_simpl_complex& C,sc_vertex vert,GRAPH<d3_rat_point,int>& outgr)
{ }

LEDA_END_NAMESPACE
