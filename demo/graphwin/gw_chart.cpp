/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_chart.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::ifstream;
using std::cerr;
using std::endl;
#endif


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  if (argc < 2)
  { cerr << string("usage: ") + argv[0] + " file" << endl; 
    return 1;
   }

  GraphWin gw(720,700);

  gw.set_edge_direction(undirected_edge);
  gw.set_node_shape(rectangle_node);
  gw.set_node_color(grey2);
  gw.set_node_label_type(user_label);
  gw.set_node_label_font(roman_font,9);
  gw.set_grid_dist(10);
  gw.set_zoom_objects(false);


  string title;
  int N,i;
  color  clr[16];
  string label[16];
  string unit[16];
  double mul[16];
  double add[16];
  double x,xscale,xmin,xmax,xstep;
  double y,yscale,ymin,ymax,ystep;
  string xlabel,ylabel;

  ifstream in(argv[1]);
  title = read_line(in);
  in >> N;
  for(i=0; i<N; i++)
  { in >> clr[i];
    in >> mul[i] >> add[i] >> unit[i];
    label[i] = read_line(in);
   }

  in >> xscale >> xmin >> xmax >> xstep;
  xlabel = read_line(in);

  in >> yscale >> ymin >> ymax >> ystep;
  ylabel = read_line(in);


  gw.win_init(-30,int(xmax/xscale)+80,-(60+20*i));
  gw.set_node_width(4);
  gw.set_node_height(4);
  gw.display(window::center,window::center);

  gw.message(title); 

  gw.set_node_label_pos(south_pos);

  node u = gw.new_node(point(20,0));
  gw.set_color(u,invisible);
  gw.set_border_color(u,invisible);
  gw.set_label(u,xlabel);

  for(i=0; i<N; i++)
  { if (mul[i] == 0) continue;
    u = gw.new_node(point(20,-20*(i+1)));
    gw.set_color(u,invisible);
    gw.set_border_color(u,invisible);
    gw.set_label(u,unit[i]);
  }

  for(x=xmin; x <= xmax; x+=xstep)
  { double xpos = x/xscale;
    node v = gw.new_node(point(xpos,0));
    gw.set_label(v,string("%d",int(x)));
    for(i=0; i<N; i++)
    { if (mul[i] == 0) continue;
      double n = mul[i]*x + add[i];
      v = gw.new_node(point(xpos,-20*(i+1)));
      gw.set_color(v,invisible);
      gw.set_border_color(v,invisible);
      gw.set_label_color(v,clr[i]);
      gw.set_label(v,string("%d",int(n)));
     }
   }

  gw.set_node_label_pos(east_pos,false);

  u = gw.new_node(point(xmax/xscale + 20,ymax/yscale+20));
  gw.set_color(u,invisible);
  gw.set_border_color(u,invisible);
  gw.set_label(u," "+ylabel);
  
  for(y=ymin; y <= ymax; y+=ystep)
  { node v = gw.new_node(point(xmax/xscale + 20,y/yscale));
    gw.set_label(v,string("%4d",int(y)));
   }

  gw.set_node_shape(circle_node,false);
  gw.set_node_color(ivory,false);


  node last_v[16];

  for(i=0;i<N;i++) 
  { last_v[i] = 0;
    gw.set_node_border_color(clr[i],false);
    gw.set_edge_color(clr[i],false);
    node u = gw.new_node(point(50,330-30*i));
    gw.set_label_pos(u,east_pos);
    gw.set_label(u," "+label[i]);
   }

  while (in >> x)
  { for(i=0;i<N;i++)  
    { in >> y;
      gw.set_node_border_color(clr[i],false);
      gw.set_edge_color(clr[i],false);
      node v = gw.new_node(point(x/xscale,y/yscale));
      if (last_v[i]) gw.new_edge(last_v[i],v);
      last_v[i] = v;
     }
   }

  gw.zoom_graph();
  gw.edit();

  return 0;
}



