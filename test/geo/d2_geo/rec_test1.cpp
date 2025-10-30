/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  rec_test1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rectangle.h>
#include <LEDA/geo/rectangle.h>

using namespace leda;

using std::cout;
using std::cin;
using std::endl;



void print_info(rectangle& R)
{
 cout << "rectangle:" <<  R << "\n";
 cout << "----------\n";

 cout << "ymin:" << R.ymin() << "\n";
 cout << "ymax:" << R.ymax() << "\n";
 cout << "xmin:" << R.xmin() << "\n";
 cout << "xmax:" << R.xmax() << "\n";
 cout << "Hoehe:" << R.height() << "\n";
 cout << "Breite:" << R.width() << "\n";

 cout << "obererer linker Punkt:" << R.upper_left() << "\n";
 cout << "obererer rechter Punkt:" << R.upper_right() << "\n";
 cout << "unterer linker Punkt:" << R.lower_left() << "\n";
 cout << "unterer rechter Punkt:" << R.lower_right() << "\n";

 list<point> L= R.vertices();
 cout << "vertices:" << L << "\n";

 cout << "Punkt/Segment:" << R.is_point() << "/" << R.is_segment() << "\n";
 cout << "Degenerate   :" << R.is_degenerate() <<"\n";

 cout << "point eingeben:";
 point rp;
 cin >> rp;
 cout << "inside/outside/contains:" << R.inside(rp) << " " << R.outside(rp) << " " << R.contains(rp) << "\n";

 cout << "Flaeche:" << R.area() << "\n";

 rectangle r2;
 r2= R.rotate90();
 cout << "rotated rectangle:" << r2 << "\n";
 r2= R.reflect(point(0,0));
 cout << "reflected rectangle:" << r2 << "\n";

 cout << "\n";
}

void print_info(rat_rectangle& R)
{
 cout << "rectangle:" <<  R << "\n";
 cout << "----------\n";

 cout << "ymin:" << R.ymin() << "\n";
 cout << "ymax:" << R.ymax() << "\n";
 cout << "xmin:" << R.xmin() << "\n";
 cout << "xmax:" << R.xmax() << "\n";
 cout << "Hoehe:" << R.height() << "\n";
 cout << "Breite:" << R.width() << "\n";

 cout << "obererer linker Punkt:" << R.upper_left() << "\n";
 cout << "obererer rechter Punkt:" << R.upper_right() << "\n";
 cout << "unterer linker Punkt:" << R.lower_left() << "\n";
 cout << "unterer rechter Punkt:" << R.lower_right() << "\n";

 list<rat_point> L= R.vertices();
 cout << "vertices:" << L << "\n";

 cout << "Punkt/Segment:" << R.is_point() << "/" << R.is_segment() << "\n";
 cout << "Degenerate   :" << R.is_degenerate() <<"\n";


 cout << "rat_point eingeben:";
 rat_point rp;
 cin >> rp;
 cout << "inside/outside/contains:" << R.inside(rp) << " " << R.outside(rp) << " " << R.contains(rp) << "\n";

 cout << "Flaeche:" << R.area() << "\n";

 rat_rectangle r2;
 r2= R.rotate90();
 cout << "rotated rat_rectangle:" << r2 << "\n";
 r2= R.reflect(rat_point(0,0,1));
 cout << "reflected rat_rectangle:" << r2 << "\n";

 cout << "\n";
}

main()
{
 point p1(12,33);
 rectangle R(p1,10,25);

 print_info(R);

 rat_point prat(10,10);
 rat_rectangle RR(prat,200,100);

 print_info(RR); 
}
