/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_delau_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/tuple.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/file.h> 
#include <LEDA/system/stream.h>
#include <LEDA/core/map.h>
#include <LEDA/core/d_array.h>
#include <LEDA/system/assert.h>

using namespace leda;

#include "d3_complex.h"


using std::cout;
using std::endl;


string UGLYGLOB_READ;
string UGLYGLOB_WRITE;

void read_from_file(string fn)
{
  UGLYGLOB_READ=fn;
}

void write_to_file(string fn)
{
  UGLYGLOB_WRITE=fn;
}

void dummy(string fn){ UGLYGLOB_READ="";UGLYGLOB_WRITE="";}

void file_open(list<d3_rat_point>& L)
{  
  string s1,s2;
  s1="dat.pts"; s2=".";
  file_panel P(s1,s2);
  P.set_load_handler(read_from_file);
  P.set_cancel_handler(dummy);
  P.set_pattern("*.pts");
  P.open(); 

  if (UGLYGLOB_READ != ""){
   cout << UGLYGLOB_READ << "\n";
   if (is_file(UGLYGLOB_READ)){
     file_istream I(UGLYGLOB_READ);
     int z,anz;
     d3_rat_point pu;

     I >> anz; 
     cout << "loading " << anz << " points!\n";

     for (z=0;z<anz;z++) {
       I >> pu;
       L.append(pu);
     }
     
   }
  }
}

void file_save(list<d3_rat_point>& L)
{
  string s1,s2;
  s1="dat.pts"; s2=".";
  file_panel P(s1,s2);
  P.set_save_handler(write_to_file);
  P.set_cancel_handler(dummy);
  P.set_pattern("*.pts");
  P.open();  

  if (UGLYGLOB_WRITE != ""){
   cout << UGLYGLOB_WRITE << "\n";
   if (! is_file(UGLYGLOB_WRITE)){
     file_ostream O(UGLYGLOB_WRITE);
     int anz;
     d3_rat_point pu;

     anz=L.size();
     cout << "saving " << anz << " points!\n";

     O << anz; 
     forall (pu,L) O << pu;
   }
  }
}


static window W(550,650,"3D-Delaunay-Demo");
static window W2(650,650,"cell view");

static int maxcoord = 1000;

static list<d3_rat_point> ptlist;

void redraw()
{ d3_rat_point p; 
  forall(p,ptlist)
     W.draw_point(p.xcoordD(),p.ycoordD(),blue);
}

void check_sph(list<d3_rat_point>& L)
{
 if (L.size() < 4) return;
 list_item li=L.first();
 d3_rat_point ap,bp,cp,dp,ep;
 ap= L.inf(li);
 li=L.succ(li); bp= L.inf(li);
 li=L.succ(li); cp= L.inf(li);
 li=L.succ(li); dp= L.inf(li);

 forall(ep,L) { if (on_sphere(ap,bp,cp,dp,ep)) cout << "y"; else cout << "n"; }
 cout << "\n";
}

void generate_input(int n, int input, int maxc, list<d3_rat_point>& L)
{ L.clear();
  W.clear();
  W.message("Generating Points");

  switch (input) {
   case 0: random_points_in_cube(n,3*maxc/4,L); break;
   case 1: random_points_in_ball(n,maxc,L); break;
   case 2: random_points_on_paraboloid(n,maxc,L); break;
   case 3: //random_points_in_square(n,maxc,L); break;
          { lattice_points(n,3*maxc/4,L); 
             list_item it;
             forall_items(it,L) 
             { d3_rat_point p = L[it];
               L[it] = d3_rat_point(p.X(),p.Y(),0,p.W()); 
              }
             break;
           }
   case 4: lattice_points(n,3*maxc/4,L); break;
   case 5: random_d3_rat_points_on_sphere(n,maxc,L);  break;
   case 6: random_points_on_segment(n,maxc,L); break;
  }
  W.del_messages();
  ptlist=L; 
  redraw();
}


bool is_artificial(const d3_rat_point& p)
{ double x = p.xcoordD();
  double y = p.ycoordD();
  double z = p.zcoordD();
  return (fabs(x) > maxcoord || fabs(y) > maxcoord || fabs(z) > maxcoord);
}


int main(){

 int n = 32;
 int t = 0;
 int input = 0;
 int ausgabe = 0;
 int algo = 0;

 list<string> choices;
 choices.append("cube"); 
 choices.append("ball"); 
 choices.append("para"); 
 choices.append("d2mesh");
 choices.append("d3mesh"); 
 choices.append("sphere"); 
 choices.append("line");

 list<string> zahl;
 zahl.append("0"); 
 zahl.append("100"); 
 zahl.append("200"); 
 zahl.append("300");
 zahl.append("400"); 
 zahl.append("500"); 
 zahl.append("600"); 

 list<string> ou;
 ou.append("DT"); 
 //ou.append("Hull"); 
 //ou.append("DT-cell"); 
 ou.append("Voro1");
 ou.append("Voro2");
 //ou.append("VD1"); 
 //ou.append("VD2");

 list<string> algart;
 algart.append("D3-Del"); algart.append("D3-Tr");

 W.choice_item("input",input,choices);
 W.choice_item("points",t,zahl);

 W.int_item("    +",n,1,100);

 W.choice_item("output",ausgabe,ou);
 //W.choice_item("algor.",algo,algart);


 W.button("gen",  1); 
 W.button("run",  2); 
 W.button("setup",3);
 W.button("load", 4); 
 W.button("save", 5);
 W.button("exit", 0);

 W.set_redraw(redraw);
 W.init(-maxcoord,maxcoord,-3*maxcoord/4);
 W.display();
 W.set_node_width(2);

 W2.init(-1.2*maxcoord,1.2*maxcoord,-1.2*maxcoord);
 W2.button("next",1);
 W2.button("close",0);

 GRAPH<d3_rat_point,int> H; // output graph
 GRAPH<d3_rat_point,int> outgr; // output graph for other window (W2)
 d3_simpl_complex C;

 int anim_speed = 12;

 d3_window anim(W,H);
 d3_window anim2(W2,outgr);

 anim.set_elim(false); anim2.set_elim(false);
 anim.set_solid(false); anim2.set_solid(false);
 anim.set_speed(anim_speed); anim2.set_speed(anim_speed);
 anim.set_node_width(2); anim2.set_node_width(2);

 list<d3_rat_point> L;

 int but = W.read_mouse();
 int but2;
 color pnt_col;

 int big = 4*maxcoord;

 //helper points...

 map<d3_rat_point,bool> is_help(false);

 d3_rat_point phelp1(-big,-big,-big,1);
 d3_rat_point phelp2( big,-big,-big,1);
 d3_rat_point phelp3(-big,-big, big,1);
 d3_rat_point phelp4( big,-big, big,1);
 d3_rat_point phelp5(-big, big,-big,1);
 d3_rat_point phelp6( big, big,-big,1);
 d3_rat_point phelp7(-big, big, big,1);
 d3_rat_point phelp8( big, big, big,1);

 is_help[phelp1] = true;
 is_help[phelp2] = true;
 is_help[phelp3] = true;
 is_help[phelp4] = true;
 is_help[phelp5] = true;
 is_help[phelp6] = true;
 is_help[phelp7] = true;
 is_help[phelp8] = true;


 while (but != 0)
 {
   switch (but) {
  
   case 1: { int m = (1 << t)/2 * 100;
             L.clear();
             C.clear();
             generate_input(m+n,input,int(W.xmax()/2),L);
             H.clear();
             break;
           }

   case 2: { H.clear();
             float t1 = used_time();

             C.reset_flip_counters();
             C.clear();

             if (algo==0) {
               float Tim=used_time();
  
               if ( ausgabe == 1 || ausgabe == 2)
               { L.append(phelp1); 
                 L.append(phelp2); 
                 L.append(phelp3); 
                 L.append(phelp4);
                 L.append(phelp5); 
                 L.append(phelp6); 
                 L.append(phelp7); 
                 L.append(phelp8);
               }

               D3_DELAU(L,C); 
               cout << "----- Time:" << used_time(Tim) << " -----\n";
             }
             else D3_TRIANG(L,C);

             cout << "Side of sphere:" << d3_rat_point::sos_count << "\n";

             assert(C.check_vert_sim_links());
             assert(C.check(1,cout) == 0);
             assert(C.delaunay_check(1));

             C.init_dia(true); //set diagrammedge informations
             C.set_dia_nondia();

             if (ausgabe==0) { C.compute_graph(H); pnt_col=black; }   // DT

             if (ausgabe==100) 
             { C.compute_surface_graph(H); pnt_col=black; }  // Hull
 
	     if (ausgabe==101) 
             { // DT+Simplices
                C.compute_graph(H);
                W2.init(-1.2*maxcoord,1.2*maxcoord,-1.2*maxcoord);
                W2.display();
                sc_simplex sakt;
                list<sc_simplex> allsim=C.all_simplices();             
 
                forall(sakt,allsim){
                   C.compute_graph(sakt,outgr);
                   W2.clear();
                   node_array<rat_vector> pos2(outgr);
                   node v;
                   forall_nodes(v,outgr) pos2[v] =  outgr[v].to_vector();
                   anim2.init(pos2); 
                   anim2.draw();
                   
                   do {
                    but2=anim2.move();
                   }
                   while (but2 !=1 && but2 !=0);
                   
                   if (but2==0) break;              
                }

                W2.close();    
                pnt_col=black;
             }

             if (ausgabe==1 || ausgabe == 2)
             { // DT+ closed Voronoi cells

                C.compute_graph(H);


                C.make_vlists();
                list<sc_vertex> vall = C.all_vertices();

                W2.init(-1.2*maxcoord,1.2*maxcoord,-1.2*maxcoord);
                W2.display();

                outgr.clear();

                map<d3_rat_point,node> V(nil);

                d3_rat_point p;
                forall(p,L) 
                  if (!is_help[p]) V[p] = outgr.new_node(p);


                list<list<edge>*> cell_list;
                list<node>        cur_list;

                sc_vertex vakt;
                forall(vakt,vall)
                {
                 list<d3_rat_point>* aktli=(list<d3_rat_point>*)(vakt->helper);
                 if (aktli ==0 || aktli->empty()) continue;
 
                 GRAPH<d3_rat_point,int> hull;
                 CONVEX_HULL((*aktli),hull);

                 cur_list.append(V[C.pos(vakt)]);

                 list<edge>* cell = new list<edge>;
                 *cell = hull.all_edges();
                 cell_list.append(cell);

                 outgr.join(hull);
                }

                node_array<rat_vector> pos2(outgr);

                node v;
                forall_nodes(v,outgr) pos2[v] = outgr[v].to_vector();

                color nhc = (ausgabe == 1) ? blue : black;

                anim2.init(pos2);
                anim2.set_node_hcolor(nhc);

                if (ausgabe == 2) anim2.set_solid(true);

                forall_nodes(v,outgr)  anim2.set_color(v,grey2);

                edge e;
                forall_edges(e,outgr) 
                { d3_rat_point p = outgr[source(e)];
                  d3_rat_point q = outgr[target(e)];
                  if (ausgabe == 2 || (is_artificial(p) && is_artificial(q)))
                    anim2.set_color(e,invisible);
                  else
                    anim2.set_color(e,grey1);
                 }
                 

                edge_array<int> e_ord(outgr,0);
                int count = 0;

                list<edge>* cell;
                forall(cell,cell_list)
                {
                  count++;

                  edge e;
                  forall(e,*cell) 
                  { e_ord[e] = count;
                    anim2.set_color(e,nhc);
                   }

                  if (ausgabe == 1) outgr.sort_edges(e_ord);

                  node cur_v = cur_list.pop();

                  if (cur_v) anim2.set_color(cur_v,nhc);

                  do but2 = anim2.move(); while (but2 !=1 && but2 !=0);

                  if (cur_v) anim2.set_color(cur_v,grey2);

                  forall(e,*cell)
                  { d3_rat_point p = outgr[source(e)];
                    d3_rat_point q = outgr[source(e)];
                    if (ausgabe == 2 || (is_artificial(p) && is_artificial(q)))
                      anim2.set_color(e,invisible);
                    else
                      anim2.set_color(e,grey1);
                   }
		 
                  if (but2==0) break;
		}

                W2.close();
                C.kill_vlists();
                pnt_col=black;
             }

             if (ausgabe==4 || ausgabe==5) {

              pnt_col=grey1;

              if (ausgabe==4) 
                 C.compute_voronoi(H,true); 
              else 
                 C.compute_voronoi(H,false);

              cout << "nodes:" << H.number_of_nodes() << "\n";
              pnt_col=grey1;

              W2.init(-1.2*maxcoord,1.2*maxcoord,-1.2*maxcoord);
              W2.display();
              node v,nd;
              
              forall_nodes(nd,H){
                to_vdnb(H,nd,outgr);
                W2.clear();
                node_array<rat_vector> pos2(outgr);
                forall_nodes(v,outgr) pos2[v] =  outgr[v].to_vector();

                anim2.init(pos2); 

                anim2.draw();

                do { but2=anim2.move(); }
                while (but2 !=1 && but2 !=0);
                   
                if (but2==0) break;  
              }

              W2.close();
             }


             float t2 = used_time();
             W.clear();

             sc_vertex vakt;
             node nh = 0;
             list<d3_rat_point> * aktli;
             
             if (ausgabe==4 || ausgabe==5) C.make_vlists();

             list<sc_vertex> vall=C.all_vertices();

             forall(vakt,vall){

              if (ausgabe < 4)
                aktli = 0;
              else
                aktli = (list<d3_rat_point> *)((*vakt).helper);

              if (aktli==NULL || ! (*aktli).empty()){
               node_array<rat_vector> pos(H);
               node v;
               edge e;

               if (ausgabe==4 || ausgabe==5) { 
                 nh = H.new_node(C.pos(vakt)); 
                 pos[nh]= H[nh].to_vector(); 
                }

               forall_nodes(v,H) pos[v] =  H[v].to_vector();

               anim.init(pos); 

               forall_edges(e,H)
               { d3_rat_point p = H[source(e)];
                 d3_rat_point q = H[target(e)];
                 if (is_artificial(p) || is_artificial(q)) 
                    anim.set_color(e,invisible);
                }

               anim.draw();

               if (ausgabe!=4 && ausgabe!=5) break;
              
               forall_edges(e,H) {
                   if((*aktli).search(H[H.source(e)]) && 
                      (*aktli).search(H[H.target(e)])) anim.set_color(e,blue);
                   else  anim.set_color(e,pnt_col);
               }



               do {
                 but2=anim.move();
               }
               while (but2 !=1 && but2 !=0);
		
               H.del_node(nh);

               if (but2==0) break;
              }
	     }
  

             if (ausgabe==4 || ausgabe==5) C.kill_vlists();

             W.message(string("%5.2f sec",t2-t1));
             break;
            }

   case 3: { panel P;
             P.int_item("max xcoord",maxcoord,1,1000);
             P.int_item("anim speed",anim_speed,1,32);
             P.open(W);
             W.init(-maxcoord,maxcoord,-3*maxcoord/4);
             anim.set_speed(anim_speed);
             break;
           }

   case 4: { 
             L.clear();
             file_open(L);
             break;
           }

   case 5: {
             file_save(L); 
             break;
           }

   }
   but = anim.move();
 }


 return 0;
}
