/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  delau_speed.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//#define USE_GNUPLOT

#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/core/random_source.h>
#include <math.h>
#include <stdlib.h>


#if defined USE_GNUPLOT
#include <LEP/GraphLEP/gnuplot.h>
#endif

using namespace leda;

using std::ifstream;
using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;


// FABS
#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ == 1
#define FABS(x) x = fp::abs(x)
#else
// causes internal compiler error in g++-3.0.1 (s.n. August 2001)
#define FABS(x) fp::clear_sign_bit(x)
#endif



void usage()
{
  cerr << "usage:\n";
  cerr << "\t d2_delau_static_filter_speed <min> <max> <generation> <steps> <range>\n\n";
  cerr << "min       : start point list size\n";
  cerr << "max       : end point list size\n";
  cerr << "generation: 1 - in square, 2 - in disc, 3 - near circle, 4 - lattice\n";
  cerr << "steps     : augmentation\n";
  cerr << "range     : generator(range)\n";
  exit(1);
}


void compute_bounds(const list<rat_point>& L, double& xmax, 
                                              double& ymax, 
                                              double& wmax)
{
  if (L.empty()) return;
  
  rat_point p = L.head();

  xmax = fabs(p.XD()); 
  ymax = fabs(p.YD()); 
  wmax = fabs(p.WD());
  
  forall(p,L)
  { double x = fabs(p.XD()); 
    double y = fabs(p.YD()); 
    double w = fabs(p.WD());
    if (x > xmax) xmax=x;
    if (y > ymax) ymax=y;
    if (w > wmax) wmax=w;            
  }
}



// get system name ...

inline string get_system()
{
  system("hostname > HOST_NAME");

  string name; 
  
  ifstream f("HOST_NAME");
  f >> name;
  f.close();
  
#if !defined(__win32__)
  system("rm -f HOST_NAME");
#else
  system("del HOST_NAME");
#endif  

  return name;
}

void generate_points(list<rat_point>& L, int number, int gen, int maxc)
{
  L.clear();
  
  if (number <= 0) return;
  
  switch (gen) {
    case 1:  random_points_in_square(number,maxc,L); break;
    case 2:  random_points_in_disc(number,maxc,L); break;
    case 3:  random_points_near_circle(number,maxc,L); break;
    case 4:  lattice_points(number,maxc,L); break;
    default: random_points_in_square(number,maxc,L); break;
  }

/*  
  // now change the points in the list
  random_source S(1,100);
  
  list_item it;
  forall_items(it, L){
     unsigned int wact;
     S >> wact;
     integer Iact(wact);
     
     rat_point& ra = L[it];
     L[it] = rat_point(ra.X(),ra.Y(), Iact);
  }
*/    
}


int main(int argv,char** argc)
{
  cout << "Comparing Delaunay triangulation algorithms of LEDA with and without static filter!\n";
 
  if (argv < 5) usage();
  
  int min   = atoi(argc[1]);
  int max   = atoi(argc[2]);
  int gen   = atoi(argc[3]);
  int steps = atoi(argc[4]);
  int range = atoi(argc[5]);
  
  int size  = max / steps;

  cout << "start size : " << min << endl;
  cout << "stop size  : " << max << endl;
 
  cout << "generation : ";
  
  string generator;
  
  switch(gen){ 
    case 1:  { generator="in square"; cout << "in square" << endl; break;   }
    case 2:  { generator="in disc"; cout << "in disc" << endl; break; }
    case 3:  { generator="near circle"; cout << "near circle" << endl; break; }
    case 4:  { generator="lattice"; cout << "lattice" << endl; break; }
    default: { cout << "in square" << endl; break;  }
  } 

  double* t1 = new double [size+1];
  double* t2 = new double [size+1];
  double* t3 = new double [size+1];
  double* t4 = new double [size+1];  
  double* ls = new double [size+1];

  int i;
  for (i = 0; i < size; i ++)
    t1[i] = t2[i] = t3[i] = t4[i] = ls[i] = 0; 
  
 list<rat_point> L;
 GRAPH<rat_point,int> H;
 
 i = min;
 for (int j = 0; i <= max; i += steps, j++)
 {
    L.clear();
    ls[j] = i;     
   
    cout << "random points (" << i << ")  ";
    cout.flush();
    
    float T0 = used_time();
    generate_points(L,i,gen,range); 
    
    cout << string("%2.2f",used_time(T0)) << " sec";
    cout << "\t real input size    " << L.size() << endl << endl; 
    
    double xmax, ymax, wmax;
    compute_bounds(L, xmax, ymax, wmax);   

    cout << "x/y/wmax:" << xmax << "/" << ymax << "/" << wmax << "\n";   
    
    rat_point::use_static_filter = false;
    rat_point::set_static_soc_error_bound(MAXDOUBLE);
    rat_point::set_static_orientation_error_bound(MAXDOUBLE);
    
    float tm = used_time();
    DELAUNAY_FLIP(L,H);
    float tm2 = used_time(tm);
    t1[j] = tm2;
    cout << "DELAUNAY_FLIP without static filter took " << tm2 << " seconds!\n";
    //cout << "#nodes:" << H.number_of_nodes() << "\n";
   
    H.clear();
   
    tm = used_time();
    DELAUNAY_DWYER(L,H);
    tm2 = used_time(tm);
    t2[j] = tm2;
    cout << "DELAUNAY_DWYER without static filter took " << tm2 << " seconds!\n";
    //cout << "#nodes:" << H.number_of_nodes() << "\n";

    H.clear();
    
    rat_point::use_static_filter = true;
    rat_point::set_static_soc_error_bound(xmax, ymax, wmax);
    rat_point::set_static_orientation_error_bound(xmax, ymax, wmax);
    
    tm = used_time();
    DELAUNAY_FLIP(L,H);
    tm2 = used_time(tm);
    t3[j] = tm2;
    cout << "DELAUNAY_FLIP with static filter took " << tm2 << " seconds!\n";
    //cout << "#nodes:" << H.number_of_nodes() << "\n";
   
    H.clear();
   
    tm = used_time();
    DELAUNAY_DWYER(L,H);
    tm2 = used_time(tm);
    t4[j] = tm2;
    cout << "DELAUNAY_DWYER with static filter took " << tm2 << " seconds!\n";
    //cout << "#nodes:" << H.number_of_nodes() << "\n";

    H.clear();    
 }
 
#if defined USE_GNUPLOT 
 cout << "calling gnuplot...\n"; cout.flush();
 
 // gnuplot output
 
 string sys = get_system();
 
 string name = "2d Delaunay triangulation on " + sys + " ,generator:" + generator;

 gnuplot_ctrl GP(name);

 GP.set_xlabel("#points");
 GP.set_ylabel("t in [s]");
 GP.set_style(gp_style::linespoints);
  
 GP.set_timestamp();
 
  double* t = t1;
  if (t2[size-1] > t[size-1]) t = t2;
  if (t3[size-1] > t[size-1]) t = t3;
  if (t4[size-1] > t[size-1]) t = t4; 
 
  GP.set_yrange("*",string("%f",t[size-1] + (t[size-1] - t[0])/3.5));
    
  GP.plot_xy(ls,t1,size,"DELAUNAY_FLIP  (no static filter)");
  //GP.plot_xy(ls,t2,size,"DELAUNAY_DWYER (no static filter)");
  GP.plot_xy(ls,t3,size,"DELAUNAY_FLIP  (static filter)");
  //GP.plot_xy(ls,t4,size,"DELAUNAY_DWYER (static filter)");  

  GP.set_terminal(gp_terminal::postscript);
  
  sys = sys.replace_all(":","_");

  name = "results/d2_delau_" + sys + ".ps"; 
  GP.set_output(name);
  GP.replot();

  cout << "Press <ENTER> to continue" << endl;   
  while (cin.get() != '\n');
  
  GP.close();
#endif

  delete[] t1;
  delete[] t2;

  delete[] ls; 

 return 0;
}





