/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  ps.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/core/array.h>
#include <LEDA/system/file.h>
#include <time.h>

#include "ps.h"

LEDA_BEGIN_NAMESPACE

PostScript& PostScript::operator<<(const char* str) {
  ostr << str;
  return *this;
}

PostScript& PostScript::operator<<(color c) {
  int r,g,b;
  c.get_rgb(r,g,b);
  ostr << string("%.3f %.3f %.3f ", r/255.0, g/255.0, b/255.0);
  return *this;
}

PostScript& PostScript::operator<<(double d) {
  ostr << string("%f ",d);
  return *this;
}

PostScript& PostScript::operator<<(int i) {
  ostr << string("%d ",i);
  return *this;
}

// ---------------------------------------------------------------------------

void PostScript::header(ostream& o) {
  time_t t;
  time(&t);
  o << "%!PS-Adobe-2.0" << endl;
  o << "%%Creator: LEDA GraphWin" << endl;
  o << "%%CreationDate: " << ctime(&t);
  o << "%%Pages:  1" << endl;
  o << "%%BoundingBox: " << " " << x_min << " " << y_min 
                         << " " << x_max << " " << y_max << endl; 
  o << "%%EndComments" << endl;
}

// ---------------------------------------------------------------------------

void PostScript::footer(ostream& o) {
  o << endl << "%%EndProgram" << endl << endl;
  o << "showpage" << endl;
  o << "end %% LEDA_dict" << endl;
  o << "%%EOF" << endl;
}

// ---------------------------------------------------------------------------

void PostScript::prolog(ostream& o) {

  o << "/LEDA_dict 100 dict def" << endl;
  o << "LEDA_dict begin" << endl;

  o << "%%" << endl;
  o << "%% COLORS" << endl;

  o << "%%" << endl;
  o << "%% FONTS" << endl;
  o << "/font_dict 2 dict def" << endl;
  o << "/set_font {" << endl;
  o << "  font_dict" << endl;
  o << "  begin" << endl;
  o << "    /font_size exch def /font_name exch def" << endl;
//o << "    font_name cvn font_size selectfont" << endl;
  o << "    font_name cvn findfont font_size scalefont setfont" << endl;
  o << "  end" << endl;
  o << "} def" << endl;

  o << "%%" << endl;
  o << "%% POLYGONS" << endl;
  o << "/polygon_dict 1 dict def" << endl;
  o << "/poly_line {" << endl;
  o << "  polygon_dict" << endl;
  o << "  begin" << endl;
  o << "    /points exch def" << endl;
  o << "    points 2 mul dup 2 sub roll" << endl;
  o << "    moveto" << endl;
  o << "    points 1 sub" << endl;
  o << "    dup { dup 2 mul 1 add -2 roll lineto 1 sub} repeat" << endl;
  o << "    pop" << endl;
  o << "  end" << endl;
  o << "} def" << endl;
  o << "/polygon { newpath poly_line closepath } def" << endl;


  o << "%%" << endl;
  o << "%% Round Rectangles" << endl;
  o << "/rrect {" << endl;
  o << "  5 dict" << endl;
  o << "  begin" << endl;
  o << "   newpath" << endl;
  o << "   /r exch def /y1 exch def /x1 exch def /y0 exch def /x0 exch def" << endl;
  o << "   x0 r add y0 moveto" << endl;
  o << "   x1 y0 x1 y1 r arct" << endl;
  o << "   x1 y1 x0 y1 r arct" << endl;
  o << "   x0 y1 x0 y0 r arct" << endl;
  o << "   x0 y0 x1 y0 r arct" << endl;
  o << "   closepath" << endl;
  o << "  end" << endl;
  o << "} def" << endl;


  o << "%%" << endl;
  o << "%% ELLIPSES" << endl;
  o << "/ellipse_dict 4 dict def" << endl;
  o << "/ellipse {" << endl;
  o << "  ellipse_dict" << endl;
  o << "  begin" << endl;
  o << "    newpath" << endl;
  o << "    /yrad exch def /xrad exch def /ypos exch def /xpos exch def" << endl;
  o << "    matrix currentmatrix" << endl;
  o << "    xpos ypos translate" << endl;
  o << "    xrad yrad scale" << endl;
  o << "    0 0 1 0 360 arc" << endl;
  o << "    setmatrix" << endl;
  o << "    closepath" << endl;
  o << "  end" << endl;
  o << "} def" << endl;

  o << "%%" << endl;
  o << "%% hatching" << endl;
  o << "/hatch1_dict 4 dict def" << endl;
  o << "/hatch1_rect {" << endl;
  o << " hatch1_dict" << endl;
  o << " begin" << endl;
  o << "  0.25 units setlinewidth" << endl;
  o << "  /ymax exch def /xmax exch def /ymin exch def /xmin exch def" << endl;
  o << "  xmin ymax ymin sub sub 3 units xmax " << endl;
  o << "  { ymin moveto ymax ymin sub dup rlineto stroke } for" << endl;
  o << " end" << endl;
  o << "} def" << endl;
  o << "%" << endl;
  o << "/hatch2_dict 4 dict def" << endl;
  o << "/hatch2_rect {" << endl;
  o << " hatch2_dict" << endl;
  o << " begin" << endl;
  o << "  0.25 units setlinewidth" << endl;
  o << "  /ymax exch def /xmax exch def /ymin exch def /xmin exch def" << endl;
  o << "  xmax ymax ymin sub add -3 units xmin " << endl;
  o << "  { ymin moveto ymin ymax sub ymax ymin sub rlineto stroke } for" << endl;
  o << " end"  << endl;
  o << "} def" << endl;

  o << "%%" << endl;
  o << "%% TEXT" << endl;
  o << "/draw_text {" << endl;
  o << "  3 dict begin" << endl;
  o << "    /label exch def /ypos exch def /xpos exch def" << endl;
  o << "    xpos ypos moveto" << endl;
  o << "    label show" << endl;
  o << "  end" << endl;
  o << "} def" << endl;

  o << "%%" << endl;
  o << "/draw_ctext {" << endl;
  o << "  8 dict begin" << endl;
  o << "   /sz exch def /label exch def /ypos exch def /xpos exch def" << endl;
  o << "   xpos ypos moveto" << endl;
  o << "   label stringwidth pop -2 div sz -0.32 mul rmoveto" << endl;
/*
  o << "   label false charpath pathbbox newpath" << endl;
  o << "   /y1 exch def /x1 exch def /y0 exch def /x0 exch def" << endl;
  o << "   xpos ypos moveto" << endl;
  o << "   x0 x1 sub 2 div y0 y1 sub 2 div rmoveto" << endl;
*/
  o << "   label show" << endl;
  o << "  end" << endl;
  o << "} def" << endl;

  o << "%%" << endl;
  o << "%% PIXMAPS" << endl;
  o << "/draw_pixmap {" << endl;
  o << " 10 dict begin" << endl;
  o << "   /ly exch def /lx exch def /h exch def /w exch def /y exch def /x exch def" << endl;
  o << "   /pix w 3 mul string def" << endl;
  o << "   x y translate" << endl;
  o << "   lx ly scale" << endl;
  o << "   w h 8 [w 0 0  0 h sub  0 h]" << endl;
  o << "   {currentfile pix readhexstring pop}" << endl;
  o << "   false 3 colorimage" << endl;
  o << " end" << endl;
  o << "} def" << endl;



  o << "%%" << endl;
  o << "%% LINESTYLES" << endl;
  o << "/units  { f_scale div } def" << endl;
  o << "/solid  { [ ] } def" << endl;
  o << "/dashed { [ 3 units 2 units] } def" << endl;
  o << "/dotted { [ 0.5 units 2 units ] } def" << endl;
  o << "/dashed_dotted { [ 3 units 2 units 0.5 units 2 units ] } def" << endl;
  o << "/set_line_style { 0 setdash } def" << endl;

  o << "%%" << endl;
  o << "%% INIT" << endl;
  o << "1 1 scale" << endl;
  o << "1 setlinewidth" << endl;
  o << "(Helvetica) 12.0 set_font" << endl;
  o << "%%EndProlog" << endl;

  o << "%%" << endl;
  o << "%%BeginProgram" << endl;
}

// ---------------------------------------------------------------------------

PostScript::PostScript() {

  lstyle_name[solid]  = "solid";
  lstyle_name[dashed] = "dashed";
  lstyle_name[dotted] = "dotted";
  lstyle_name[dashed_dotted] = "dashed_dotted";

  col    = black;
  lstyle = solid;
  lwidth = 1;
  scale  = 1;

  // w = 18 cm
  x_min = 1.5 * PSPixelPerCM;
  x_max = 19.5 * PSPixelPerCM;

  // h = 26 cm
  y_min = 2 * PSPixelPerCM;
  y_max = 28 * PSPixelPerCM;
}


// ---------------------------------------------------------------------------

void PostScript::set_origin(double x, double y) {
  *this << x << y << "translate\n";
}

void PostScript::set_scale(double f) {
  scale = f;
  *this << "/f_scale " << f << " def\n";
  *this << f << f << "scale\n";
}

// ---------------------------------------------------------------------------

void PostScript::set_color(color c) 
{ if (col != c) *this << c << "setrgbcolor\n"; 
  col = c; 
 }

void PostScript::set_line_width(double w) 
{ if (lwidth == w) return;
  lwidth = w; 
  *this << w << "setlinewidth\n"; 
 }

void PostScript::set_line_style(line_style ls) {
  if (lstyle == ls) return;
  lstyle = ls;
  *this << lstyle_name[ls] << " set_line_style\n";
}

// ---------------------------------------------------------------------------

double PostScript::get_xmax() { return x_max; }
double PostScript::get_ymax() { return y_max; }
double PostScript::get_xmin() { return x_min; }
double PostScript::get_ymin() { return y_min; }

// ---------------------------------------------------------------------------

void PostScript::set_bounding_box(double x0, double y0, double x1, double y1)
{ x_min = x0;
  y_min = y0;
  x_max = x1;
  y_max = y1;
}


// ---------------------------------------------------------------------------

void PostScript::set_font(const char* name, double size) {
  font_size = size;
  *this << "(" << name << ") " << size << "set_font\n";
}   

// ---------------------------------------------------------------------------

void PostScript::draw_text(double x, double y, string t, color c) 
{ if (c == invisible) return;
  set_color(c);
  t = t.replace_all("(","\\(");
  t = t.replace_all(")","\\)");
  *this << x << y << "(" << t << ") " << "draw_text\n";
}
void PostScript::draw_ctext(double x, double y,string t, color c)
{ if (c == invisible) return;
  set_color(c);
  t = t.replace_all("(","\\(");
  t = t.replace_all(")","\\)");
  *this << x << y << "(" << t << ") " << font_size << "draw_ctext\n";
}

// ---------------------------------------------------------------------------


point PostScript::draw_arrow_head(const point& q, double a, color c)
{ 
  //double d = lwidth+1.75/scale;

  double d = lwidth + 1.2/scale;

  double X[4];
  double Y[4];
  point m = window::arrow_head(q,a,d,X,Y);

  if (c != invisible)
  { set_color(c);
    for(int i=0; i<4; i++) *this << X[i] << Y[i]; 
    *this << 4 << "polygon fill\n";
   }

  return m;
}


void PostScript::draw_filled_arrow(const list<point>& L, int arrow, color c)
{
  if (c == invisible) return;

  if (arrow >= 4)
  { int n = L.length();
    int m = n/2;
    list<point> lp = L;
    list_item it = lp.first();
    while (--m) it = lp.succ(it);
    list<point> lp1,lp2;
    lp.split(it,lp1,lp2,leda::behind);
    point p = lp1.tail();
    point q = lp2.head();
    point s = midpoint(p,q);
    lp1.append(s);
    lp2.push(s);

    if (arrow == 4)
    { lp2.push(p);
      draw_filled_arrow(lp2,0,c);
      draw_filled_arrow(lp1,2,c);
     }

    if (arrow == 8)
    { lp1.append(q);
      draw_filled_arrow(lp1,0,c);
      draw_filled_arrow(lp2,1,c);
     }
   
    if (arrow == 12 )
    { draw_filled_arrow(lp1,2,c);
      draw_filled_arrow(lp2,1,c);
     }

    return;
  }


  int sz = 16*L.length();

  double lw = lwidth;

  double d = lwidth + 1.2/scale;

  double* X = new double[sz];
  double* Y = new double[sz];
  int n = window::arrow(L,d,arrow,X,Y);

  set_color(c);
  int i;
  for(i=0; i<n; i++) *this << X[i] << Y[i]; 
  *this << n << "polygon fill\n";

  set_line_width(lw/10);
  set_color(black);
  for(i=0; i<n; i++) *this << X[i] << Y[i]; 
  *this << n << "polygon stroke\n";

  set_line_width(lw);

  delete[] X;
  delete[] Y;
}


// ---------------------------------------------------------------------------

void PostScript::draw_ellipse(double x, double y, double r1, double r2, color c)
{ if (c == invisible) return;
  set_color(c);
  *this << x << y << r1 << r2 << "ellipse stroke\n";
 }


void PostScript::draw_filled_ellipse(double x, double y, double r1, double r2, 
                                                                    color c)
{ if (c == invisible) return;
  set_color(c);
  *this << x << y << r1 << r2 << "ellipse fill\n";
}

void PostScript::draw_hatched_ellipse(double x,double y,double r1,double r2,
                                                                   int h,
                                                                   color c)
{ if (c == invisible) return;
  set_color(c);
  *this << "gsave\n";

  *this << x << y << r1 << r2 << "ellipse clip newpath\n";
  *this << (x-r1) << (y-r2) << (x+r1) << (y+r2); 
  if (h == 1)
    *this << "hatch1_rect\n";
  else
    *this << "hatch2_rect\n";

  *this << "grestore\n";
}



// ---------------------------------------------------------------------------

void PostScript::draw_circle(const point& p, double r, color c)
{ if (c == invisible) return;
  set_color(c);
  *this << p.xcoord() << p.ycoord() << r << 0 << 360 << "arc stroke\n";
 }

void PostScript::draw_filled_circle(const point& p, double r, color c)
{ if (c == invisible) return;
  set_color(c);
  *this << p.xcoord() << p.ycoord() << r << 0 << 360 << "arc fill\n";
 }



void PostScript::draw_polygon(const list<point>& P, color c)
{ if (c == invisible) return;
  set_color(c);
  point p;
  forall(p,P) *this << p.xcoord() << p.ycoord(); 
  *this << P.size() << "polygon stroke\n";
 }


void PostScript::draw_filled_polygon(const list<point>& P, color c)
{ if (c == invisible) return;
  set_color(c);
  point p;
  forall(p,P) *this << p.xcoord() << p.ycoord(); 
  *this << P.size() << "polygon fill\n";
 }

void PostScript::draw_hatched_polygon(const list<point>& P, int h, color c)
{ if (c == invisible) return;
  set_color(c);

  *this << "gsave\n";

  point p = P.head();
  double xmin = p.xcoord();
  double xmax = p.xcoord();
  double ymin = p.ycoord();
  double ymax = p.ycoord();
  forall(p,P) 
  { if (xmin > p.xcoord()) xmin = p.xcoord(); 
    if (ymin > p.ycoord()) ymin = p.ycoord(); 
    if (xmax < p.xcoord()) xmax = p.xcoord(); 
    if (ymax < p.ycoord()) ymax = p.ycoord(); 
    *this << p.xcoord() << p.ycoord(); 
   }
  *this << P.size() << "polygon clip newpath\n";
  *this << xmin << ymin << xmax << ymax;
  if (h == 1)
    *this << "hatch1_rect\n";
  else
    *this << "hatch2_rect\n";

  *this << "grestore\n";
 }


// ---------------------------------------------------------------------------

void PostScript::draw_rectangle(double x0, double y0, double x1, double y1, 
                                                                 color c) 
{ if (c == invisible) return;
  set_color(c);
  *this << x0  << y0;
  *this << x1  << y0;
  *this << x1  << y1;
  *this << x0  << y1;
  *this << 4 << "polygon stroke\n";
}

void PostScript::draw_filled_rectangle(double x0, double y0, double x1, 
                                                             double y1, color c)
{ if (c == invisible) return;
  set_color(c);
  *this << x0  << y0;
  *this << x1  << y0;
  *this << x1  << y1;
  *this << x0  << y1;
  *this << 4 << "polygon fill\n";
}


void PostScript::draw_roundrect(double x0, double y0, double x1, double y1, 
                                                      double rndness, color c) 
{ if (c == invisible) return;
  set_color(c);
  double w = x1-x0;
  double h = y1-y0;
  double r = (w > h) ? (h/2) : (w/2);
  *this << x0  << y0 << x1  << y1 << (rndness*r) << " rrect stroke\n";
}

void PostScript::draw_filled_roundrect(double x0,double y0,double x1,double y1, 
                                                 double rndness, color c)
{ if (c == invisible) return;
  set_color(c);
  double w = x1-x0;
  double h = y1-y0;
  double r = (w > h) ? (h/2) : (w/2);
  *this << x0  << y0 << x1  << y1 << (rndness*r) << " rrect fill\n";
}


void PostScript::draw_hatched_rectangle(double x0, double y0, double x1, 
                                                              double y1,
                                                              int h,
                                                              color c)
{ if (c == invisible) return;
  set_color(c);
  *this << "gsave\n";

  *this << x0  << y0;
  *this << x1  << y0;
  *this << x1  << y1;
  *this << x0  << y1;
  *this << 4 << "polygon clip newpath\n";

  *this << x0  << y0 << x1  << y1;
  if (h == 1)
    *this << "hatch1_rect\n";
  else
    *this << "hatch2_rect\n";

  *this << "grestore\n";
}



// ---------------------------------------------------------------------------

void PostScript::draw_polygon_edge(const list<point>& P, int arrow, color c) 
{
  if (c == invisible) return;

  set_color(c);


  if (arrow == 4 || arrow == 8)
  { int m = P.length()/2;
    list_item it = P.first();
    while (--m) it = P.succ(it);
    point a = P[it];
    it = P.succ(it);
    point b = P[it];
    point s = midpoint(a,b);
    segment seg;
    if (arrow & 4) seg = segment(a,s);
    if (arrow & 8) seg = segment(b,s);
    draw_polygon_edge(P,0,c);
    draw_arrow_head(s,seg.angle(),c);
    return;
   }

  list<point> Q = P;

  if ((arrow & 4) || (arrow & 8))
  { int m = Q.length()/2;
    list_item it = Q.first();
    while (--m) it = Q.succ(it);
    list<point> lp1,lp2;
    Q.split(it,lp1,lp2,leda::behind);
    point p = lp1.tail();
    point q = lp2.head();
    point s = midpoint(lp1.tail(),lp2.head());
    lp1.reverse_items();
    lp1.push(s);
    lp2.push(s);
    int arr1 = (arrow & 4) ? 1 : 0;
    int arr2 = (arrow & 8) ? 1 : 0;
    draw_polygon_edge(lp1,arr1,c);
    draw_polygon_edge(lp2,arr2,c);
    return;
  }

  segment s1;
  segment s2;

  if (arrow & 1)
  { list_item it1 = Q.first();
    list_item it2 = Q.succ(it1);
    s1 = segment(Q[it2],Q[it1]);
    Q[it1] = draw_arrow_head(s1.target(),s1.angle(),invisible);
   }

  if (arrow & 2)
  { list_item it1 = Q.last();
    list_item it2 = Q.pred(it1);
    s2 = segment(Q[it2],Q[it1]);
    Q[it1] = draw_arrow_head(s2.target(),s2.angle(),invisible);
   }

  point p;
  forall(p,Q) *this << p.xcoord() << p.ycoord();
  *this << Q.size() << "poly_line stroke\n";

  if (arrow & 1) draw_arrow_head(s1.target(),s1.angle(),c);
  if (arrow & 2) draw_arrow_head(s2.target(),s2.angle(),c);

}


// ---------------------------------------------------------------------------

void PostScript::draw_bezier_edge(const list<point>& P, int arrow, color c) {

  if (c == invisible) return;

  set_color(c);

  int m = 64;

  double* X = new double[m];
  double* Y = new double[m];
  window::compute_bezier(P,m,X,Y);

  if (arrow & 1) 
  { segment s(X[1],Y[1],X[0],Y[0]);
    point q = draw_arrow_head(s.target(),s.angle(),c);
    X[0] = q.xcoord();
    Y[0] = q.ycoord();
   }
    
  if (arrow & 2) 
  { segment s(X[m-2],Y[m-2],X[m-1],Y[m-1]);
    point q = draw_arrow_head(s.target(),s.angle(),c);
    X[m-1] = q.xcoord();
    Y[m-1] = q.ycoord();
   }

  for(int i=0; i<m; i++) *this << X[i] << Y[i];
  *this << m << "poly_line stroke\n";

  delete[] X;
  delete[] Y;
}


void PostScript::draw_spline_edge(const list<point>& L, int arrow, color c)
{
    int size = L.size(); 

    array<point>  pol(size+2);
    array<double> seglength(size+2);
    array<vector> tangent(size+2);

    if (L.head() == L.tail()) 
    { // closed spline
      pol[0]      = L[L.pred(L.last())];
      pol[size+1] = L[L.succ(L.first())];
     }
    else
    { // open spline
      pol[0]      = L.head();
      pol[size+1] = L.tail();
     }

    int i=1;

    point pt;
    forall(pt,L) pol[i++] = pt;

    for (i=1; i <= size; i++) tangent[i] = pol[i+1] - pol[i-1];
    for (i=0; i <= size; i++) seglength[i] = pol[i].distance(pol[i+1]);

    for (i=1; i<size; i++)
    { double q  = 3*(seglength[i-1] + seglength[i]);
      double h1 = seglength[i-1]/q;
      double h2 = seglength[i]/q;

      list<point> A;
      A.append(pol[i]);
      A.append(pol[i] + h1*tangent[i]);
      A.append(pol[i+1] - h2*tangent[i+1]);
      A.append(pol[i+1]);

      if (i == 1 && (arrow & 1))
        draw_bezier_edge(A,1,c);
      else 
       if (i == size-1 && (arrow & 2))
         draw_bezier_edge(A,2,c);
       else
         draw_bezier_edge(A,0,c);
    } 
}



// ---------------------------------------------------------------------------



void PostScript::draw_circle_edge(point p, point q, point r, int arrow, color c)
{ 
  if (c == invisible) return;

  circle C(p,q,r);

  int orient = orientation(p,q,r);

  if (orient == 0 || C.radius() > 100*p.distance(r)) 
  { list<point> L;
    L.append(p);
    L.append(q);
    L.append(r);
    draw_polygon_edge(L,arrow,col);
    return;
   }

  set_color(c);

  point  cen = C.center();
  double rad = C.radius();

  if (arrow == 4)
  { double phi = cen.angle(p,r);
    if (orient < 0) phi -= 2*LEDA_PI;
    point a = p.rotate(cen,0.25*phi);
    point b = p.rotate(cen,0.50*phi);
    point c = p.rotate(cen,0.75*phi);
    draw_circle_edge(p,a,b,2,col);
    draw_circle_edge(r,c,b,2,col);
    return;
   }



  if (arrow & 1)
  { segment s(p,cen);
    double d = LEDA_PI/2;
    if (orient < 0) d = -d;
    p = draw_arrow_head(s.source(),s.angle()+d,col);
  }

  if (arrow & 2)
  { segment s(r,cen);
    double d = LEDA_PI/2;
    if (orient > 0) d = -d;
    r = draw_arrow_head(s.source(),s.angle()+d,col);
   }

  point  ref   = cen.translate(1,0);
  double start = 360*cen.angle(ref,p)/(2*LEDA_PI);
  double stop  = 360*cen.angle(ref,r)/(2*LEDA_PI);


  *this << p.xcoord() << p.ycoord() << "moveto\n";
  *this << cen.xcoord() << cen.ycoord() << rad << start << stop;

  if (orient > 0) 
    *this << "arc stroke\n";
  else
    *this << "arcn stroke\n";
}


// ----------------------------  Operators  ----------------------------------

bool PostScript::print(ostream& o)
{ ostr << ends;
  //char *ps_str = ostr.str(); 
  string ps_str = ostr.str(); 
  header(o);
  prolog(o);
  o << ps_str;
  footer(o);
  //delete[] ps_str;
  return !o.fail();
 } 


bool PostScript::print(string fname) 
{ ofstream o(fname);
  return print(o);
 }
  



// convert xpm into ps pixmap

static int get_index(istream& in, int sz)
{ int index = in.get();
  if (sz == 2) index = 256*index + in.get();
  return index;
 }
  

static void xpm_to_ps(istream& in, int width, int height, int ncol, int cpp, 
                                                               ostream& out)
{ string table[1<<16];

  read_line(in);

  for(int n = 0; n < ncol; n++)
  { 
    while (in.get() != '"');
    int index = get_index(in,cpp);

    //while (in.get() != '#');
    char c;
    do c = (char)in.get();
    while (c != '#' && c != 'N');

    if (c == 'N')  // None
       table[index] = "FFFFFF";
    else 
       table[index].read(in,'"');

    read_line(in);
   }


  for(int i=0; i<height; i++)
  { while (in.get() != '"') {}
    for(int j=0; j<width; j++)
    { if (j%12 == 0)  out << endl;
      out << table[get_index(in,cpp)];
     }
    read_line(in);
  }

  out << endl;
  out << endl;
}



void PostScript::include_xpm(string fname, double x, double y, double w1, 
                                                               double h1)
{ 
  ifstream istr(fname);

  int w, h, ncol, cpp;

  while (istr && istr.get() != '"');

  istr >> w >> h >> ncol >> cpp;

  if (istr.fail())
  { cerr << "xpm2ps: cannot open " + fname  << endl;
    return;
   }

  cerr << "xpm2ps: " << fname << endl;

  if (cpp > 2) 
  { cerr << "xpm2ps: chars_per_pixel > 2" << endl;
    return;
   }


  ostr << endl;
  ostr << "gsave" << endl;
  ostr << string("%f %f %d %d %f %f draw_pixmap",x-w1/2,y-h1/2,w,h,w1,h1);
  ostr << endl;

  xpm_to_ps(istr,w,h,ncol,cpp,ostr);

  ostr << "grestore" << endl;
}



void PostScript::put_pixrect(char* pmap, double x, double y, double w1, 
                                                             double h1)
{ 
  window W;

  int w = W.get_pixrect_width(pmap);
  int h = W.get_pixrect_height(pmap);

  ostr << endl;
  ostr << "gsave" << endl;
  ostr << string("%f %f %d %d %f %f draw_pixmap",x-w1/2,y-h1/2,w,h,w1,h1);
  ostr << endl;

  string psname("/tmp/gw_%x.ps",pmap);

  if (!is_file(psname))
  { ofstream out(psname);
    W.pixrect_to_ps(pmap,out,true);
   }

  ifstream in(psname);

  while (in) ostr << read_line(in) << endl;
    
  ostr << "grestore" << endl;
}

// clipping

void PostScript::clip_reset()
{ *this << "initclip\n"; }

void PostScript::clip_ellipse(double x, double y, double r1, double r2)
{ *this << x << y << r1 << r2 << "ellipse clip newpath\n"; }

void PostScript::clip_rectangle(double x0, double y0, double x1, double y1)
{ double w = x1-x0;
  double h = y1-y0;
  *this << x0  << y0 << w << h << "rectclip\n";
}

void PostScript::clip_polygon(const list<point>& P)
{ point p;
  forall(p,P) *this << p.xcoord() << p.ycoord(); 
  *this << P.size() << "polygon clip newpath\n";
 }




void PostScript::draw_line(double x0, double y0,double x1, double y1)
{ *this << x0 << y0 << x1 << y1 << "2 poly_line stroke\n"; }

void PostScript::draw_grid(double xmin, double ymin, double xmax, double ymax,
                                                                  double d)
{ 
  if (d == 0) return;

  if (d < 0) d = -d;

  double x0 = 0;
  while (x0 > xmin) x0 -= d;
  while (x0 < xmin) x0 += d;

  double y0 = 0;
  while (y0 > ymin) y0 -= d;
  while (y0 < ymin) y0 += d;

  int r,g,b;
  color c0 = grey1;
  c0.get_rgb(r,g,b);

  color c1(r+24,g+24,b+24);
  color c2(r-16,g-16,b-16);

  set_color(c1);
  for(double i = x0; i < xmax; i += d) draw_line(i,ymin,i,ymax);
  for(double j = y0; j < ymax; j += d) draw_line(xmin,j,xmax,j);

  set_color(c2);
  draw_line(0,ymin,0,ymax); 
  draw_line(xmin,0,xmax,0); 
}

LEDA_END_NAMESPACE
