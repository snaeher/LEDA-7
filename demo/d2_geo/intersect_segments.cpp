/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  intersect_segments.c
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

//#include <LEDA/geo/float_kernel_types.h> // careful: rounding errors!
#include <LEDA/geo/rat_kernel_types.h>
//#include <LEDA/geo/real_kernel_types.h>

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

using namespace leda;


enum { file_load, file_save, file_all, file_exit };

static GRAPH<POINT,SEGMENT>* gp;
static string alg = "sweep";
static float run_t = 0;

static list<SEGMENT> seglist;
static window* win_ptr = 0;

static string  dname = ".";
static string  fname = "segments.dat";
static string  filter = "";


void report_intersection(const SEGMENT& s1, const SEGMENT& s2)
{ POINT p;
  if (s1.intersection_of_lines(s2,p)) gp->new_node(p);
 }


void draw_seglist(window& W, const list<SEGMENT>& seglist, bool status)
{ int j = 0;
  SEGMENT s;
  forall(s,seglist) 
  { color c = color::get(1 + (j++ % 15));
    W.draw_segment(s.to_segment(),c);
    if (status) W.set_status_string(string(" segments: %4d",j));
   }
}


void redraw(window* wp)
{ window& W = *wp;
  GRAPH<POINT,SEGMENT>& G = *gp;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  string msg(" %s |V| = %d |E| = %d  t = %6.2f sec", alg,n,m,run_t);
  W.set_status_string(msg);

  W.start_buffering();
  W.clear();
  draw_seglist(W,seglist,false);

  node v;
  forall_nodes(v,G) 
     W.draw_filled_node(G[v].to_point(),red);

  W.flush_buffer();
  W.stop_buffering();

}



void run_alg(window& W, string alg, const list<SEGMENT>& seglist, 
                        GRAPH<POINT,SEGMENT>& G)
{
  G.clear();
  G.make_directed();

  W.start_buffering();
  W.clear();
  draw_seglist(W,seglist,false);
  W.flush_buffer();
  W.stop_buffering();


  float T = used_time();

  if (alg == "sweep")    
    SWEEP_SEGMENTS(seglist,G,false);

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
    SEGMENT s;
    forall(s,seglist)
    { G.new_node(s.source());
      G.new_node(s.target());
      }
   }

  if (alg == "trivial")
  { TRIVIAL_SEGMENTS(seglist,report_intersection);
    SEGMENT s;
    forall(s,seglist)
    { G.new_node(s.source());
      G.new_node(s.target());
     }
   }

  run_t = used_time(T);

  redraw(&W);
}
  

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




void set_grid(int sz)
{ int grid_dist = int((win_ptr->xmax() - win_ptr->xmin())/(2*sz+2));
  win_ptr->set_grid_mode(grid_dist);
}



int main()
{ 
/*
  cout << "using " << leda_tname((POINT::coord_type*)0) << "-kernel" << endl;
  rat_point::use_filter = 0;
  rat_segment::use_filter = 0;
*/

  int N = 50;
  int grid_size = 3;

  int h = 8*window::screen_dpi();
  int w = int(0.8 * h);


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

  alg = "sweep";

  W.int_item("segments", N,0,1000);
  W.int_item("grid size",grid_size,1,32,set_grid);
  W.string_item("algorithm",alg,alg_names,10);

  W.button("mouse", 1);
  W.button("rand",  2);
  W.button("file",  99,file_menu);
  W.button("run",   3);
  W.button("graph", 4);
  W.button("quit",  0);

  //int d = 2;
  int d = int(0.5 + window::screen_dpi()/40.0);
  W.set_node_width(d);

  W.set_grid_style(line_grid);
  W.set_redraw(redraw);

  //W.set_clear_on_resize(false);

  W.init(-1000,1000,-1000);
  W.display(window::center,window::center);

  W.open_status_window();

  GRAPH<POINT,SEGMENT> G;
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
             seglist.append(SEGMENT(x1,y1,x2,y2));
            }
     
           break;
          }
     
   
       case 2: // random
        { seglist.clear();
          W.clear();

          int gmin = -grid_size;
          int gmax =  grid_size;

          double d = W.get_grid_dist();

          for(int i = 1; i <= N; i++)
          { double x0 = rand_int(gmin,gmax)*d;
            double y0 = rand_int(gmin,gmax)*d;
            double x1 = rand_int(gmin,gmax)*d;
            double y1 = rand_int(gmin,gmax)*d;
            SEGMENT s = SEGMENT(int(x0),int(y0),int(x1),int(y1));
            seglist.append(s);
            W.draw_segment(s.to_segment(),blue);
            W.set_status_string(string(" segments: %4d",i));
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

/*
    if (but == 1 || but == 2 || but == 99)
    { G.clear();
      W.clear();
      draw_seglist(W,seglist);
    }
*/

  }

  rat_point::print_statistics();
  return 0;

}

