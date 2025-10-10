#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/geo/real_rectangle.h>
#include <LEDA/geo/rat_gen_polygon.h>
#include <LEDA/graphics/real_window.h>
#include <LEDA/system/stream.h>
#include <LEDA/system/file.h>

#include <stdlib.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cerr;
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::endl;
#endif


static window W(750,800,"Polygon With Circular Edges Buffer Demo");

static r_circle_gen_polygon poly;

static void message(string s) { W.set_status_string(s); }



static r_circle_polygon mouse_polygon(window& W)
{
  r_circle_polygon Q;
  bool ok = false;
  bool go_on = false;
  do {
   if (!(W >> Q)) break;

   if (Q.is_weakly_simple())
      ok = true;
   else 
    { panel msg("Error");
      msg.text_item("");
      msg.text_item("Polygon is \\red not simple\\black .");
      msg.text_item("");
      msg.button("again",0);
      msg.button("cancel",1);
      go_on = msg.open(W) != 1;
      W.clear();
    }
  } while(!ok && go_on);

  return ok ? Q : r_circle_polygon();
}


static void redraw(window* wp, double x0, double y0, double x1, double y1)
{ x1++; y1++;
  list<r_circle_segment> clip_P_segs;
  rat_point c1(point(x0,y0));
  rat_point c2(point(x1,y0));
  rat_point c3(point(x1,y1));
  rat_point c4(point(x0,y1));
  clip_P_segs.append(rat_segment(c1,c2));
  clip_P_segs.append(rat_segment(c2,c3));
  clip_P_segs.append(rat_segment(c3,c4));
  clip_P_segs.append(rat_segment(c4,c1));
  r_circle_gen_polygon clip_P(clip_P_segs);
  r_circle_gen_polygon Q = poly.intersection(clip_P);
  draw_filled(*wp,Q,ivory);
  draw(*wp,Q,black);
}



#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  double step = 0.05;

double xmin = -10;
double xmax = 10;
double ymin = -10;
double ymax = 10;

  if (argc > 1)
  { file_istream in(argv[1]);
    if (!in) 
    { error_handler(1,string("Cannot open file: %s.",argv[1]));
      return 1;
     }
    in >> poly;

list<r_circle_point> L = poly.vertices();

xmin = MAXINT;
xmax = -MAXINT;
ymin = MAXINT;
ymax = -MAXINT;

r_circle_point p;

forall(p,L)
{ point q = p.approximate_by_rat_point().to_float();

cout << string("%.2f %.2f",q.xcoord(),q.ycoord()) << endl;

  if (q.xcoord() > xmax)  xmax = q.xcoord();
  if (q.xcoord() < xmin)  xmin = q.xcoord();
  if (q.ycoord() > ymax)  ymax = q.ycoord();
  if (q.ycoord() < ymin)  ymin = q.ycoord();
}
cout << string("%.2f %.2f %.2f %.2f",xmin,ymin,xmax,ymax) <<endl;


    //poly = poly.translate(-xmin,-ymin);

/*
xmin = -50;
xmax = 250;
ymin = -50;
*/

xmin -= 50;
xmax += 150;
ymin -= 150;

    if (argc > 2) step = atoi(argv[2]);
   }


  W.set_show_coordinates(true);
  W.set_redraw(redraw);
  W.set_node_width(2);  


  W.init(xmin,xmax,ymin);
  W.display(window::center,window::center);
  W.open_status_window();



  bool insert_holes = (argc == 2 && string(argv[1]) == "-x");

  if (argc == 1 || insert_holes)
  { message("Please draw an r_circle_polygon.");
    poly = mouse_polygon(W); 
/*
    polygon P;
    W >> P;
    rat_polygon Q(P);
    poly = rat_gen_polygon(Q);
*/
   }


  if (insert_holes)
  { for(int x = -2000; x <= 2000; x += 1000) 
     for(int y = -2000; y <= 2000; y += 1000) 
      { rat_circle circ(rat_point(x,y,1), rat_point(x+300,y,1));
        poly = poly.diff(r_circle_polygon(circ));
       }
   }

  file_ostream out("poly.out");
  out << poly << endl;

  W.clear();
  draw_filled(W,poly,ivory);
  draw(W,poly,black);

  W.read_mouse();

  W.start_buffering();

  array<r_circle_gen_polygon> P(32);
  P[0] = poly;

  for(int i = 1; i<=100; i++)
  {
  //double delta = step;
    double delta = 1.0 + i*step;
    message(string("i = %d   delta = %.2f", i, delta));
  
    r_circle_gen_polygon Q = poly.buffer(delta);
    double area = Q.approximate_area();
    int n = Q.vertices().length();
    int p = Q.polygons().length();
  
    message(string("i = %d   delta = %.2f  vertices: %d   parts:%d   area: %.2f",
                    i, delta, n, p, area));
  
    P[i] = Q;
  
  
    W.clear();
    draw_filled(W,Q,ivory);
    draw(W,Q,black); 
    W.flush_buffer();

/*
    int d = 15;
  
    if (delta > 0)
    { int c = 120 + d*i;
      for(int j=i; j>=0; j--)
      { draw_filled(W,P[j],color(c,c,c));
        draw(W,P[j],black); 
        c -= d;
       }
     }
    else
    { int c = 120;
      for(int j=0; j<=i; j++)
      { draw_filled(W,P[j],color(c,c,c));
        draw(W,P[j],black); 
        c += d;
       }
     }
W.flush_buffer();

*/

  
W.message("Continue: left button        Quit: right button"); 
W.flush_buffer();
  
if (W.read_mouse() == MOUSE_BUTTON(3)) break;
  
message("");
W.del_messages();
W.flush_buffer();


  if (Q.empty()) break;
  
   //P = Q;
  }
  
  W.stop_buffering();

  W.message("Click a button to exit");
  W.read_mouse();

  return 0;
}

