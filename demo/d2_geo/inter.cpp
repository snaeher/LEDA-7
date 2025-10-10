/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  inter.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/stream.h>

using namespace leda;


enum { file_load, file_save, file_all, file_exit };

static GRAPH<rat_point,rat_segment>* gp;

void report_intersection(const rat_segment& s1, const rat_segment& s2)
{ rat_point p;
  if (s1.intersection_of_lines(s2,p)) gp->new_node(p);
 }


void run_alg(window& W, string alg, const list<rat_segment>& seglist, 
                        GRAPH<rat_point,rat_segment>& G)
{
    W.clear();
    int j = 0;
    rat_segment s;
    forall(s,seglist) 
    { color c = color(1 + (j++ % 15));
      W.draw_segment(s.to_segment(),c);
     }

    G.clear();
    G.make_directed();

    W.message(alg);

    float T = used_time();
  
    if (alg == "sweep")    
      SWEEP_SEGMENTS(seglist,G);
  
    if (alg == "sweep (planar map)")
      SWEEP_SEGMENTS(seglist,G,true);

    if (alg == "mulmuley") 
      MULMULEY_SEGMENTS(seglist,G);

    if (alg == "mulmuley (undirected)") 
    { G.make_undirected();
      MULMULEY_SEGMENTS(seglist,G);
     }

    if (alg == "mulmuley (planar map)") 
      MULMULEY_SEGMENTS(seglist,G,true);

    if (alg == "balaban")
    { BALABAN_SEGMENTS(seglist,report_intersection);
      rat_segment s;
      forall(s,seglist)
      { G.new_node(s.source());
        G.new_node(s.target());
        }
     }

    if (alg == "trivial")
    { TRIVIAL_SEGMENTS(seglist,report_intersection);
      rat_segment s;
      forall(s,seglist)
      { G.new_node(s.source());
        G.new_node(s.target());
       }
     }

    float t = used_time(T);
    int n = G.number_of_nodes();
    int m = G.number_of_edges();

    W.del_messages();
    W.message(string("%s |V| = %d |E| = %d  t = %6.2f sec", alg,n,m,t));
  
    node v;
    forall_nodes(v,G) 
       W.draw_filled_node(G[v].to_point(),red);
}
  

static list<rat_segment> seglist;
static window* win_ptr;

static string  dname = ".";
static string  fname = "segments.dat";
static string  filter = "";

static void read_file(string fn)
{ file_istream in(fn);
  seglist.read(in);
  win_ptr->set_grid_mode(0);
 }

static void write_file(string fn)
{ file_ostream out(fn);
  seglist.print(out);
  win_ptr->set_grid_mode(0);
 }


static void file_handler(int what)
{
  file_panel FP(*win_ptr,fname,dname);

  switch (what) {
  case file_load: FP.set_load_handler(read_file);
                  break;
  case file_save: FP.set_save_handler(write_file);
                  break;
  case file_all:  FP.set_load_handler(read_file);
                  FP.set_save_handler(write_file);
                  break;
  }
  if (filter != "") FP.set_pattern(filter);
  FP.open();
}




void redraw(window* wp)
{ window& W = *wp;
  GRAPH<rat_point,rat_segment>& G = *gp;
  W.start_buffering();
  W.clear();
  int j = 0;
  rat_segment s;
  forall(s,seglist) 
  { color c = color(1 + (j++ % 15));
    W.draw_segment(s.to_segment(),c);
   }
  node v;
  forall_nodes(v,G) 
     W.draw_filled_node(G[v].to_point(),red);
  W.flush_buffer();
  W.stop_buffering();
}


void set_grid(int sz)
{ int grid_dist = int((win_ptr->xmax() - win_ptr->xmin())/(2*sz+2));
  win_ptr->set_grid_mode(grid_dist);
}



int main()
{ 
  int N = 50;
  int grid_size = 3;
  string alg = "sweep";

  int h = int(0.65 * window::screen_height());
  int w = int(0.82 * h);

  menu file_menu;
  file_menu.button("Load File",file_load,file_handler);
  file_menu.button("Save File",file_save,file_handler);
  //file_menu.button("Exit",file_exit);

  window W(w,h,"Segment Intersection Algorithms");
  win_ptr = &W;

  list<string> alg_names;
  alg_names.append("sweep");
  alg_names.append("sweep (planar map)");
  alg_names.append("mulmuley");
  alg_names.append("mulmuley (undirected)");
  alg_names.append("mulmuley (planar map)");
  alg_names.append("balaban");
  alg_names.append("trivial");

  W.int_item("segments", N,0,1000);
  W.int_item("grid size",grid_size,1,32,set_grid);
  W.string_item("algorithm",alg,alg_names,10);

  W.button("mouse", 1);
  W.button("rand",  2);
  W.button("file",  99,file_menu);
  W.button("run",   3);
  W.button("graph", 4);
  W.button("quit",  0);

  W.set_node_width(2);
  W.set_grid_style(line_grid);
  W.set_redraw(redraw);

  W.init(-1000,1000,-1000);
  W.display(window::center,window::center);

  GRAPH<rat_point,rat_segment> G;
  gp = &G;

  set_grid(grid_size);

  for(;;)
  {
    int but = W.read_mouse();

    if (but == 0) break;
  
    switch (but) {

       case 1: // mouse
         { seglist.clear();
           W.clear();
     
           segment s;
           while (W >> s)
           { W << s;
             int x1 = int(s.xcoord1());
             int y1 = int(s.ycoord1());
             int x2 = int(s.xcoord2());
             int y2 = int(s.ycoord2());
             seglist.append(rat_segment(x1,y1,1,x2,y2,1));
            }
     
           break;
          }
     
   
       case 2: // random
        { seglist.clear();

          int gmin = -grid_size;
          int gmax =  grid_size;

          double d = W.get_grid_dist();

          for(int i = 0; i < N; i++)
          { double x0 = rand_int(gmin,gmax)*d;
            double y0 = rand_int(gmin,gmax)*d;
            double x1 = rand_int(gmin,gmax)*d;
            double y1 = rand_int(gmin,gmax)*d;
            seglist.append(rat_segment(int(x0),int(y0),1,int(x1),int(y1),1));
            W.del_messages();
            W.message(string("%4d",i));
           }
           break;
        }


    case 3: { // run algorithm
              run_alg(W,alg,seglist,G);
              break;
             }

    case 4: { // start GraphWin
              char* buf = W.get_window_pixrect();
              GraphWin gw(G,W);
              gw.set_node_label_type(no_label);
              gw.set_node_shape(circle_node);
              gw.set_node_width(8);
              gw.set_directed(true);
              node v;
              forall_nodes(v,G) gw.set_position(v,G[v].to_point());
              W.disable_panel();
              gw.display();
              gw.edit();
              W.enable_panel();
              W.put_pixrect(buf);
              W.del_pixrect(buf);
              break;
             }

    }

    if (but == 1 || but == 2 || but == 99)
    { G.clear();
      W.clear();
      int j = 0;
      rat_segment s;
      forall(s,seglist) 
      { color c = color(1 + (j++ % 15));
        W.draw_segment(s.to_segment(),c);
      }
    }

}


 rat_point::print_statistics();

 return 0;

}

