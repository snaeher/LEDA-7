/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  svg.c
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

#include "svg.h"


LEDA_BEGIN_NAMESPACE

// ----------------------------  Operators << --------------------------------

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(const char* str) 
{
  ostr << str;
  return *this;
}

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(color c) 
{
  if( c == invisible ) 
    { *this << string("\"none\""); }
  else
    {
      int r,g,b;
      c.get_rgb(r,g,b);
      *this << string("\"rgb(%d,%d,%d)\"", r, g, b);
    }
  return *this;
}

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(ScalableVectorGraphics& (*f)(ScalableVectorGraphics&))
{
  return f(*this); 
}


ScalableVectorGraphics& ScalableVectorGraphics::operator<<(double d) 
{
  *this << string("%f",d);
  return *this;
}

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(int i) 
{
  *this << string("%d",i);
  return *this;
}

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(ostream& (*f)(ostream&))
{
  ostr << f; 
  return *this; 
}

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(point p)
{
  *this << p.xcoord() 
	<< ( comma ? "," : " " )
	<< p.ycoord(); 
  return *this; 
}

ScalableVectorGraphics& ScalableVectorGraphics::operator<<(const list<point>& P)
{
  point p; 
  bool first = true;
  forall( p , P ) 
    { 
      if( !first )
	{ *this << " "; }
      else
	{ first = false; }
      *this << p ; 
    } 
  return *this; 
}



// ---------------------------------------------------------------------------

void ScalableVectorGraphics::header(ostream& o) {
  time_t t;

  time(&t);

/*
  o << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
    << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20001102//EN\"" << endl
    << "    \"http://www.w3.org/TR/2000/CR-SVG-20001102/DTD/" 
    << "svg-20001102.dtd\">" 
    << endl << endl; 
*/

  /* write comment */ 
  o << "<!--" <<  endl;
  o << "     SVG" << endl;
  o << "     Creator: LEDA " <<  GraphWin::version_str() << endl;
  o << "     CreationDate: " << ctime(&t) << endl;
  o << " -->" << endl;
  o << endl;
  o<< endl;


  int svg_h = 600; 
  int svg_w = int(svg_h*(width/height));

  /* write header */ 
  o << "<svg xmlns=\"http://www.w3.org/2000/svg\""   << endl;
  o << "     style=\"margin:20px;\""                 << endl;
  o << "     preserveAspectRatio=\"none\""           << endl;
  o << string("     width=\"%dpx\" height=\"%dpx\"",svg_w,svg_h) << endl;
  o << string("     viewBox=\"%.3f %.3f %.3f %.3f\">",
                                      x_min,y_min,width,height) << endl;
  o << endl;

//string text_font_family = "Droid Sans";
  string text_font_family = "Arial";

//string fixed_font_family = "Lucida Console";
  string fixed_font_family = "Consolas";

  double xpix = width/svg_w;
  double ypix = width/svg_h;

  o << "  <style type=\"text/css\">" << endl;

  o << "    .Node { }" << endl; 
  o << "    .Edge { opacity:" << edge_transparency << " }" << endl;
  o << "    .Text { white-space:pre;font-family:Arial;font-size:"   
                                                << 13*ypix << "px; }" << endl;
  o << "    .Title { font-family:Arial;font-weight:bold;font-size:" 
                                                << 15*ypix << "px; }" << endl;


  o << "    .NodeLabel { "; 
  switch( node_label_font )
  {
    case roman_font:  
      o << "font-family:" << text_font_family << ";";
      break;
    case bold_font:   
      o << "font-family:" << text_font_family  << ";font-weight:bold;";
      break; 
    case italic_font: 
      o << "font-family:"<< text_font_family << ";font-style:italic;";
      break;
    case fixed_font:  
      o << "font-family:" << fixed_font_family << ";";
      break;
    default: 
      cerr << "Warning: unknown font used, using default font instead" << endl;
  }

//o << "font-size:" << node_font_size << "pt;}" << endl;
  o << "font-size:" << node_font_size << "px;}" << endl;


  o << "    .EdgeLabel { "; 
  switch( edge_label_font )
  {
    case roman_font:  
      o << "font-family:" << text_font_family << ";";
      break;
    case bold_font:   
      o << "font-family:" << text_font_family << ";font-weight:bold;";
      break; 
    case italic_font: 
      o << "font-family:" << text_font_family << ";font-style:italic;";
      break;
    case fixed_font:  
      o << "font-family:" << fixed_font_family << ";";
      break;
    default: 
      cerr << "Warning: unknown font used, using default font instead" << endl;
  }

//o << "font-size:" << edge_font_size  << "pt;}" << endl;
  o << "font-size:" << edge_font_size  << "px;}" << endl;

  o << "  </style>" << endl;
  o << endl;

  o << "<g transform=\"scale(1 -1) translate(0 " << -(y_min+y_max) << ")\">";
  o << endl;
  o << endl; 

  // background rectangle

  o << "<!-- background box -->" << endl;
  o << endl; 
  o << string("<rect x=\"%.3f\" y=\"%.3f\"",x_min,y_min) << endl;
  o << string("      width=\"%.2f\" height=\"%.2f\"",width,height) << endl;
  o << string("      fill=\"rgb(245,245,245)\" stroke=\"rgb(0,0,0)\"") << endl;
  o << string("      stroke-width=\"%.3fpx\" />",ypix)<< endl;
  o << endl; 


  o << "<text class=\"Title\"" << endl; 
  o << "      x=\"" << 5*xpix << "\" y=\"" << 17*ypix << "\""         << endl;
  o << "      fill=\"rgb(0,0,128)\" text-anchor=\"start\""            << endl;
  o << string("      transform=\"scale(1 -1) translate(%.3f %.3f)\">",
                                                        x_min,-y_max) << endl;
  o << "      " << title << endl;
  o << "</text>" << endl;
  o << endl; 

  o << "<text class=\"Text\"" << endl; 
  o << "      x=\"" << -5*xpix << "\" y=\"" << -7*ypix << "\""        << endl;
  o << "      fill=\"rgb(64,64,64)\" text-anchor=\"end\""             << endl;
  o << string("      transform=\"scale(1 -1) translate(%.3f %.3f)\">",
                                                        x_max,-y_min) << endl;
  o << "      LEDA  " << GraphWin::version_str() << endl;
  o << "</text>" << endl;
  o << endl; 

}

// ---------------------------------------------------------------------------

void ScalableVectorGraphics::footer(ostream& o) 
{
  o << "  </g>" << endl;
  o << endl;
  o << "</svg>" << endl;
}

// ---------------------------------------------------------------------------

void ScalableVectorGraphics::prolog(ostream& o) 
{ o << "  <!-- begin of prolog -->" << endl; 
  userProlog(o); 
  o << "  <!-- end of prolog -->" << endl; 
  o << endl;
}


// ----------------------------  Operators  ----------------------------------

bool ScalableVectorGraphics::print(ostream& o)
{ 
  ostr << ends;
  string ps_str = ostr.str(); 
  header(o);
  prolog(o);
  o << ps_str;
  footer(o);
  return !o.fail();
} 


bool ScalableVectorGraphics::print(string fname) 
{ 
  ofstream o(fname);
  bool ret = print(o); 
  // o.close(); 
  return ret; 
}
  





// ---------------------------------------------------------------------------

void ScalableVectorGraphics::set_bounding_box(double x0, double y0, 
					   double x1, double y1, 
					   double wscale)
{
  x_min  = x0; 
  y_min  = y0; 
  x_max  = x1; 
  y_max  = y1; 
  width  = x1 - x0;
  height = y1 - y0;

  scale  = wscale; 
}


void ScalableVectorGraphics::set_node_label_font(gw_font_type fn, int sz)
{ node_label_font = fn; 
  //node_font_size = sz/3.5;
  node_font_size = (0.9*sz)/scale;
}

void ScalableVectorGraphics::set_edge_label_font(gw_font_type fn, int sz)
{ edge_label_font = fn; 
  //edge_font_size  = sz/3.5;
  edge_font_size = (0.9*sz)/scale;
}


// ---------------------------------------------------------------------------

// 1 fill , 2 border , 3 complete

void ScalableVectorGraphics::insertShapeAtr(int space, int mode)
{
  if( ( mode & 1 ) && ( mode & 2 ) )
    { 
      // complete 
      if( shape_border_width == 0 )
	{ insertFill(space); }
      else
	{ insertBoth(space); } 
    }
  else
    {
      if( mode & 1 ) 
	{
	  // only fill
	  insertFill(space); 
	}
      else
	{
	  // only border
	  insertBorder(space); 
	}
    }
}

void ScalableVectorGraphics::writeSpaces(int space)
{
  for( int index = 0 ; index < space ; index++ )
    { *this << " "; }
}

void ScalableVectorGraphics::insertFill(int space)
{
  writeSpaces(space); 
  
  *this << "fill=" << shape_color << " stroke=\"none\" "; 
}

void ScalableVectorGraphics::insertBorder(int space)
{
  writeSpaces(space); 

  *this << "fill=\"none\" "; 
  if( shape_border_width == 0 )
    { *this << "stroke=\"none\" " ; }
  else
    { 
      *this << "stroke=" << shape_border_color << " "
	    << "stroke-width=\"" << shape_border_width << "\" "; 
    }
}

void ScalableVectorGraphics::insertBoth(int space)
{
  writeSpaces(space); 

  *this << "fill=" << shape_color << " "; 
  if( shape_border_width == 0 )
    { *this << "stroke=\"none\" " ; }
  else
    { 
      *this << "stroke=" << shape_border_color << " "
	    << "stroke-width=\"" << shape_border_width << "\" "; 
    }
}

void ScalableVectorGraphics::insertLineAtr()
  { insertLineAtr(0); }

void ScalableVectorGraphics::insertLineAtr(int space)
{
  if( ( line_color == invisible ) ||
      ( line_thickness == 0.0 ) )
    { return; }
  
  writeSpaces(space); 

  if( edge_transparency <= 0.0 ) 
    {
      *this << "fill=\"none\" "
	    << "stroke=\"none\""; 
    }
  else
    {
      *this << "fill=\"none\" "
	    << "stroke=" << line_color << " "
	    << "stroke-width=\"" << line_thickness << "\" ";
      switch( line_style )
	{
	case 1: 
	  // dashed_edge
	  *this << "stroke-dasharray=\"5 5\""; 
	  break; 
	case 2: 
	  // dashed_edge
	  *this << "stroke-dasharray=\"1 3\""; 
	  break; 
	case 3: 
	  // dashed_edge
	  *this << "stroke-dasharray=\"5 5 1 5\""; 
	  break; 
	}
    }
}

color ScalableVectorGraphics::labelColor()
{
  if( label_color == gw_auto_color )
    {
      if( label_pos != central_pos )
	{ return black; }
      else
	{
	  switch( label_bg_color )
	    {
	    case black: 
	    case red: 
	    case blue: 
	    case violet: 
	    case brown: 
	    case pink:
	    case green2: 
	    case blue2:
	    case grey3:
	      return white; 
	    default: 
	      return black; 
	    }
	}
    }
  else
    { return label_color; }
}


// ---------------------------------------------------------------------------

void ScalableVectorGraphics::draw_ctext(point pos, string txt)
{
  *this << "<text class=\"NodeLabel\"" 
	<< " x=\"0\" y=\"" << 0.4*node_font_size << "\" " << endl
        << "          fill=" << labelColor() ;

  *this << "          text-anchor=\"middle\" " << endl; 

  *this << "          transform=\"scale(1 -1) translate(" 
	<< pos.xcoord() << " " << -pos.ycoord() << ")\">" << endl;

  *this << "    " << txt << endl;
  *this << "</text>" << endl; 
}



point ScalableVectorGraphics::draw_arrow_head(const point& q, double a)
{ 
//double d = line_thickness + 3.0 / scale; 
  double d = line_thickness + 2.75/ scale; 
  
  double X[4];
  double Y[4];
  point m = window::arrow_head(q,a,d,X,Y);

  *this << "    <polygon fill=";

  if (edge_transparency <= 0.0)
     *this << "\"none\"" << endl;
  else 
     *this << line_color << endl;

  *this << "             points=\""; 
  for( int index = 0 ; index < 4 ; index++ )
    { 
      if( index != 0 )
	{ *this << " "; }
      *this << X[index] << "," << Y[index]; 
    }
  *this << "\" />"
        << endl; 
  
  return m; 
}


// ---------------------------------------------------------------------------

void ScalableVectorGraphics::draw_bounded_ellipse(double x, double y, 
					       double r1, double r2)
{
  if( ( shape_color == invisible ) && 
      ( ( shape_border_width <= 0.0 ) ||
	( shape_border_color == invisible ) ) )
    { return; }
  
  *this << "    <ellipse cx=\"" << x << "\" cy=\"" << y << "\""
	<< endl
	<< "             rx=\"" << r1 << "\" ry=\"" << r2 << "\" "
	<< endl;
  insertShapeAtr(4+9, 3); 
  *this << " />"
        << endl;  
}

void ScalableVectorGraphics::draw_ellipse(double x, double y, 
				       double r1, double r2)
{ 
  if( ( shape_border_color == invisible ) || 
      ( shape_border_width <= 0.0 ) )
    { return; }

  color old_sc = shape_color; 
  shape_color = invisible; 
  draw_bounded_ellipse(x, y, r1, r2); 
  shape_color = old_sc; 
}


void ScalableVectorGraphics::draw_filled_ellipse(double x, double y, 
					      double r1, double r2)
{ 
  if( shape_color == invisible ) 
    { return; }

  color old_sbc = shape_border_color; 
  shape_border_color = invisible; 
  draw_bounded_ellipse(x, y, r1, r2); 
  shape_border_color = old_sbc; 
}

// ---------------------------------------------------------------------------


void ScalableVectorGraphics::draw_bounded_polygon(const list<point>& P)
{ 
  if( ( shape_color == invisible ) && 
      ( ( shape_border_color == invisible ) || 
	( shape_border_width <= 0.0 ) ) )
    { return; }
  
  *this << "    <polygon "; 
  insertShapeAtr(0, 3);
  *this << endl 
	<< "             points=\"" 
	<< commaInPoint
	<< P 
	<< "\" />"
	<< endl; 
}

void ScalableVectorGraphics::draw_polygon(const list<point>& P)
{ 
  if( ( shape_border_color == invisible ) ||
      ( shape_border_width <= 0.0 ) )
    { return; }

  color old_sc = shape_color; 
  shape_color = invisible; 
  draw_bounded_polygon(P); 
  shape_color = old_sc; 
}


void ScalableVectorGraphics::draw_filled_polygon(const list<point>& P)
{
  if( shape_color == invisible )
    { return; }

  color old_sbc = shape_border_color; 
  shape_border_color = invisible; 
  draw_bounded_polygon(P); 
  shape_border_color = old_sbc; 
}

// ---------------------------------------------------------------------------

void ScalableVectorGraphics::draw_bounded_rectangle(double x0, double y0, 
						 double x1, double y1, 
						 double rd)
{
  int mode = 3; 

  if( shape_color == invisible )
    { mode--; }
  if( ( shape_border_width <= 0.0 ) ||
      ( shape_border_color == invisible ) )
    { mode -= 2; }

  if( mode == 0 ) 
    { return; }
  
  double width  = x1 - x0; 
  double height = y1 - y0; 
  bool   useRX  = width < height; 
  
  *this << "    <rect x=\"" << x0 << "\" y=\"" << y0 << "\" "; 
  if( rd != 0 )
    { 
      *this << ( useRX ? "rx" : "ry") 
	    << "=\"" 
	    << rd * ( useRX ? width : height ) 
	    << "\""; 
    }
  *this << endl
        << "          width=\"" << width
	<< "\" height=\"" << height << "\" "
	<< endl;
  insertShapeAtr(4+6, 3); 
  *this << " />"
	<< endl;
}


void ScalableVectorGraphics::draw_rectangle(double x0, double y0, 
					 double x1, double y1, 
					 double rd) 
{ 
  color old_sc = shape_color; 
  shape_color = invisible; 
  draw_bounded_rectangle(x0, y0, x1, y1, rd); 
  shape_color = old_sc; 
}

void ScalableVectorGraphics::draw_filled_rectangle(double x0, double y0, 
						double x1, double y1, 
						double rd) 
{ 
  color old_sbc = shape_border_color; 
  shape_border_color = invisible; 
  draw_bounded_rectangle(x0, y0, x1, y1, rd); 
  shape_border_color = old_sbc; 
}

// -------------------------  general draw methods  --------------------------

void ScalableVectorGraphics::draw_node(gw_node_shape shape, 
				    double x, double y, 
				    double r1, double r2, 
				    double bw2, 
				    gw_position lpos, 
				    gw_label_type ltype, 
				    string label, 
				    int index)
{
  node_italic = false; 
  /* just in case ... */
  // *this << "    <g class=\"Node\">"
  //       << endl; 
  switch( shape )
    {
    case circle_node:
      r2=r1;
    case ellipse_node:
      draw_bounded_ellipse( x , y , r1-bw2 , r2-bw2 ); 
      break; 
    case square_node:
      r2=r1;
    case roundrect_node: 
    case ovalrect_node: 
    case rectangle_node:
      {
	double rndness = 0; 
	if( shape == roundrect_node ) 
	  { rndness = 0.25; }
	if( shape == ovalrect_node )  
	  { rndness = 0.5; }
	draw_bounded_rectangle( x-r1+bw2 , y-r2+bw2 , 
				x+r1-bw2 , y+r2-bw2 , 
				rndness ); 
	break;
      }
    case rhombus_node : 
      {
	list<point> L;
	L.append(point(x,y-r2));
	L.append(point(x+r1,y));
	L.append(point(x,y+r2));
	L.append(point(x-r1,y));
	  
	draw_bounded_polygon(L);
	break;
      }
    case triang0_node: 
    case triang1_node: 
    case triang2_node: 
    case triang3_node: 
      {
	point pos(x, y); 
	point a = pos.translate(0,r1);
	point b = pos.translate(-sqrt(3.0)*r1/2, -0.5*r1);
	point c = pos.translate( sqrt(3.0)*r1/2, -0.5*r1);
	
	double phi = 0;
	
	switch( shape ) 
	  {
	  case triang1_node : phi = LEDA_PI/2;
	    break;
	  case triang2_node : phi = 2*LEDA_PI/2;
	    break;
	  case triang3_node : phi = 3*LEDA_PI/2;
	    break;
	  default:            phi = 0;
	    break;
	  }
	
	list<point> L;
	L.append(a.rotate(pos,phi));
	L.append(b.rotate(pos,phi));
	L.append(c.rotate(pos,phi));
	
	draw_bounded_polygon(L); 
      }
    default: // unhandled shapes
      break;
    }

  // draw label

  list<string> labels; 
  bool start, middle, end; 
  double yshift;
  
  label_pos     = lpos; 
  point initpos = init_node_label( point(x,y) , 
				   r1 , r2 , 
				   lpos , shape , 
				   label , index , 
				   labels , yshift , 
				   start , middle , end); 

  draw_text_node( initpos , r1, r2,
		  labels , yshift , 
		  start , middle , end ); 

  if( ( ltype & index_label ) && 
      ( label_pos != central_pos ) && 
      ( ltype != index_label ) ) 
    {	    
      gw_position old_pos = label_pos; 
      label_pos = central_pos; 
      draw_text_node( point(x, y) , string("%d", index) , 
		      false , true , false ); 
      label_pos = old_pos; 
    }
  *this << endl; 
  /* see top */
  // *this << "    </g>" 
  //       << endl << endl; 
}

void ScalableVectorGraphics::draw_edge(const list<point>& P, 
				    int arrow, 
				    gw_edge_shape shape, 
                                    bool draw_border,
				    gw_position lpos, 
				    string label, 
				    int index)
{
  orient = false; 
  edge_italic = false;
  *this << "    <g class=\"Edge\">"
        << endl; 
  switch( shape )
    {
    case poly_edge:   
      if (draw_border)
         draw_filled_arrow(P , arrow);
      else
         draw_polygon_edge(P , arrow);
      break;
    case circle_edge: 
      draw_circle_edge( P.head() , 
			P[P.succ(P.first())] , 
			P.tail() , arrow ); 
      break; 
    case bezier_edge: 
      draw_bezier_edge(P, arrow); 
      break; 
    case spline_edge:
      draw_spline_edge(P, arrow); 
      break; 
    }
  *this << "    </g>"
        << endl; 

  label_pos     = lpos;
  
  list<string> labels; 
  init_edge_label( label , index , labels ); 

  double yshift = 1.25 * edge_font_size;

  list_item it0=P.first();
  list_item it1=P.succ(it0);
  list_item it2=P.last();
  
  point a = P[it0];
  point b = P[it1];
  point c = P[it2];
  
  if( ( P.length() >= 3 ) && 
      ( shape == circle_edge ) && 
      ( orientation(a,b,c) != 0 ) )
  { 
    circle C(a,b,c);
    point cen = C.center();
    if ( C.orientation() > 0 )
      { 
	a = cen.rotate(b,LEDA_PI/2);
	b = cen.rotate(b,-LEDA_PI/2);
      }
    else
      { 
	a = cen.rotate(b,-LEDA_PI/2);
	b = cen.rotate(b,LEDA_PI/2);
      }
  }

  draw_text_edge( a , b , labels , yshift ); 

  *this << endl; 
}

// --------------------  special draw methods for edges  ---------------------

void ScalableVectorGraphics::draw_filled_arrow(const list<point>& L, 
					       int arrow)
{
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
      draw_filled_arrow(lp2,0);
      draw_filled_arrow(lp1,2);
     }

    if (arrow == 8)
    { lp1.append(q);
      draw_filled_arrow(lp1,0);
      draw_filled_arrow(lp2,1);
     }
   
    if (arrow == 12 )
    { draw_filled_arrow(lp1,2);
      draw_filled_arrow(lp2,1);
     }

    return;
  }


  int sz = 16*L.length();

//double d = line_thickness + 3.0/scale;
  double d = line_thickness + 2.75/scale;

  double* X = new double[sz];
  double* Y = new double[sz];
  int n = window::arrow(L,d,arrow,X,Y);

  *this << "    <polygon "; 
  insertShapeAtr(0, 3);
  *this << endl 
	<< "             points=\"" 
	<< commaInPoint;

  for( int i = 0 ; i < n ; i++ )
  { if( i != 0 ) *this << " "; 
    *this << X[i] << "," << Y[i]; 
   }
	*this << "\" />" << endl; 

  delete[] X;
  delete[] Y;
}


void ScalableVectorGraphics::draw_polygon_edge(const list<point>& P, 
					    int arrow)
{
  if( ( line_color == invisible ) || 
      ( line_thickness == 0.0 ) )
    { return; }

  list<point> Q = P;

  if( ( arrow & 4 ) || ( arrow & 8 ) )
    { 
      int m = Q.length() / 2;
      list<point> lp1,lp2;
      list_item it = Q.first();
      
      while (--m) 
	{ it = Q.succ(it); }
      
      Q.split(it, lp1, lp2, leda::behind);
      
      point p = lp1.tail();
      point q = lp2.head();
      point s = midpoint(lp1.tail(), lp2.head());

      lp1.append(s);
      lp2.push(s);

      draw_polygon_edge(lp1, (arrow & 4 ? 2 : 0)); 
      draw_polygon_edge(lp2, (arrow & 8 ? 1 : 0)); 

      return;
    }
  
  
  if (arrow & 1)
    { 
      list_item it1 = Q.first();
      list_item it2 = Q.succ(it1);
      segment s(Q[it2],Q[it1]);
      Q[it1] = draw_arrow_head( s.target() , s.angle() );
    }
  
  if (arrow & 2)
    { 
      list_item it1 = Q.last();
      list_item it2 = Q.pred(it1);
      segment s(Q[it2],Q[it1]);
      Q[it1] = draw_arrow_head( s.target() , s.angle() );
    }
  
  *this << "    <polyline "; 
  insertLineAtr(); 
  *this << endl 
	<< "              points=\"" 
	<< commaInPoint
	<< Q 
	<< "\" />" << endl; 
}


void ScalableVectorGraphics::draw_bezier_edge(const list<point>& P, int arrow)
{
  if( line_color == invisible) 
    { return; }
  
  int m = 64;
  double* X = new double[m];
  double* Y = new double[m];
  window::compute_bezier(P,m,X,Y);

  if( arrow & 1 ) 
  { 
    segment s(X[1],Y[1],X[0],Y[0]);
    point q = draw_arrow_head(s.target(),s.angle());
    X[0] = q.xcoord(); 
    Y[0] = q.ycoord();
   }
    
  if( arrow & 2 ) 
  { 
    segment s(X[m-2],Y[m-2],X[m-1],Y[m-1]);
    point q = draw_arrow_head(s.target(),s.angle());
    X[m-1] = q.xcoord();
    Y[m-1] = q.ycoord();
  }

  
  // Waring: bezier curves are drawn as polygon lines!
  *this << "    <polyline "; 
  insertLineAtr(); 
  *this << endl 
	<< "              points=\"" ; 

  // Koordinaten der Punkte durch Kommata trennen
  for(int i=0; i<m; i++) 
    { 
      if( i != 0 )
	{ *this << " "; }
      *this << X[i] << "," << Y[i]; 
    }

  *this << "\" />" << endl; 

  delete[] X;
  delete[] Y;
}

void ScalableVectorGraphics::draw_spline_edge(const list<point>& L, int arrow)
{
  int size = L.size(); 
  
  array<point>  pol(size+2);
  array<double> seglength(size+2);
  array<vector> tangent(size+2);
  
  if (L.head() == L.tail()) 
    { 
      // closed spline
      pol[0]      = L[L.pred(L.last())];
      pol[size+1] = L[L.succ(L.first())];
    }
  else
    { 
      // open spline
      pol[0]      = L.head();
      pol[size+1] = L.tail();
    }
  
  int i=1;
  
  point p;
  forall(p,L) pol[i++] = p;
  
  for (i=1; i <= size; i++) tangent[i] = pol[i+1] - pol[i-1];
  for (i=0; i <= size; i++) seglength[i] = pol[i].distance(pol[i+1]);
  
  for (i=1; i<size; i++)
    { 
      double q  = 3*(seglength[i-1] + seglength[i]);
      double h1 = seglength[i-1]/q;
      double h2 = seglength[i]/q;
      
      list<point> A;
      A.append(pol[i]);
      A.append(pol[i] + h1*tangent[i]);
      A.append(pol[i+1] - h2*tangent[i+1]);
      A.append(pol[i+1]);

      if( i == 1 && (arrow & 1) )
        { draw_bezier_edge(A,1); }
      else 
	{
	  if( i == size-1 && (arrow & 2) )
	    { draw_bezier_edge(A,2); }
	  else
	    { draw_bezier_edge(A,0); }
	}
    } 
}

// ---------------------------------------------------------------------------



void ScalableVectorGraphics::draw_circle_edge(point p, point q, point r, 
					   int arrow)
{ 
  if( line_color == invisible ) 
    { return; }

  circle C(p,q,r);
  int ori = orientation(p,q,r);

  if( ori == 0 || 
      C.radius() > 100*p.distance(r) ) 
    { 
      list<point> L;
      L.append(p);
      L.append(q);
      L.append(r);
      draw_polygon_edge(L , arrow);
      return;
    }
  
  point  cen = C.center();
  double rad = C.radius();
  double phi = cen.angle(p,r);
  
  if( ( arrow & 4 ) || ( arrow & 8 ) )
    { 
      if( ori < 0 ) 
	{ phi -= 2*LEDA_PI; }
      point a = p.rotate(cen,0.25*phi);
      point b = p.rotate(cen,0.50*phi);
      point c = p.rotate(cen,0.75*phi);
      draw_circle_edge(p , a , b , (arrow & 4 ? 2 : 0));
      draw_circle_edge(r , c , b , (arrow & 8 ? 2 : 0));
      return;
    }
  
  
  
  if( arrow & 1 )
    { 
      segment s(p,cen);
      double d = LEDA_PI/2;
      if (ori < 0) d = -d;
      p = draw_arrow_head( s.source() , s.angle()+d );
    }
  
  if( arrow & 2 )
    { 
      segment s(r,cen);
      double d = LEDA_PI/2;
      if (ori > 0) d = -d;
      r = draw_arrow_head( s.source() , s.angle()+d );
    }
  
  if( ori < 0 ) 
    { phi -= 2*LEDA_PI; }

  bool large_flag = ( phi > LEDA_PI ) || ( phi < -LEDA_PI ); 
  bool sweep_flag = ( ori > 0 ); 

  *this << "    <path d=\"M"
	<< noCommaInPoint
	<< p 
        << " A" << rad << " " << rad
	<< " 0 " 
	<< ( large_flag ? "1" : "0" ) << "," 
        << ( sweep_flag ? "1" : "0" ) << " "
	<< noCommaInPoint
	<< r 
        << "\" " << endl; 
  insertLineAtr(11); 
  *this << " />"
        << endl << endl; 
}

// ---------------------------- Background  ----------------------------------

void ScalableVectorGraphics::draw_circle(point center , double r)
{ draw_ellipse(center.xcoord(), center.ycoord(), r, r); }

void ScalableVectorGraphics::draw_filled_circle(point center , double r)
{ draw_filled_ellipse(center.xcoord(), center.ycoord(), r, r); }

void ScalableVectorGraphics::background_circles(point center , 
					     const list<double>& r, 
					     color border, 
					     double width)
{
  set_shape_attr( invisible , border , width ); 
 
  double d; 
  forall( d , r )
    { draw_circle(center, d); }
}

// --------------------------------------------------------------------------


// ---------------------------  drawing text  --------------------------------

point ScalableVectorGraphics::calculate_pos_node(point p, 
					      double r1, double r2, 
					      gw_position label_p, 
					      gw_node_shape shape)
{
  if( label_p == central_pos )
    { return p; }
  
  double d  = 0;
  double x  = p.xcoord();
  double y  = p.ycoord();


  if( shape == square_node || 
      shape == rectangle_node || 
      label_p == north_pos || 
      label_p == south_pos ||
      label_p == west_pos  || 
      label_p == east_pos ) { 
     //d = pix_to_real(2); 
     d = node_font_size;
    }
  else { 
     //d = pix_to_real(-3); 
     d = node_font_size/2;
  }
  

  switch( label_p )
    {
    case northeast_pos: 
    case southeast_pos: 
    case east_pos: 
      x += r1 + d; 
      break; 
    case northwest_pos: 
    case southwest_pos: 
    case west_pos: 
      x -= r1 + d; 
      break; 
    default: 
      break; 
    }

  switch( label_p )
    {
    case north_pos: 
    case northeast_pos: 
    case northwest_pos: 
      y += r2 + d; 
      break; 
    case south_pos: 
    case southwest_pos: 
    case southeast_pos: 
      y -= r2 + d; 
      break; 
    default: 
      break; 
    }
  
  return point(x,y);
}

point ScalableVectorGraphics::init_node_label( point pos , 
					    double r1 , double r2 , 
					    gw_position label_p , 
					    gw_node_shape shape , 
					    string label , int index , 
					    list<string>& L,
					    double& yshift , 
					    bool& start, bool& middle, 
					    bool& end )
{
  label = label.replace_all("%d",string("%d",index));
  label = label.replace_all("%D",string("%d",index+1));
  label = label.replace_all("%c",string("%c",index+'a'));
  label = label.replace_all("%C",string("%c",index+'A'));
  
  label = label.replace_all("\n","\\n");
  
  if (label[0] == '$') 
    { 
      /* stetzen von italic font */ 
      node_italic = true; 
      label = label.del(0);
      int l = label.length();
      if (label[l-1] == '$') 
	{ label = label.del(l-1); }
    }
  
  // split label
  L.clear();
  while( label != "" )
    { 
      if( label[0] == '\n' )
	{ 
	  L.append("\n");
	  int k = 1;
	  while( label[k] == ' ' ) 
	    { k++; }
	  label = label.del(0,k-1);
	  continue;
	}
      
      if( label[0] == '\\' && 
	  label.length() > 1 && 
	  label[1] == 'n' )
	{ 
	  L.append("\n");
	  int k = 2;
	  while (label[k] == ' ') k++;
	  label = label.del(0,k-1);
	  continue;
	}
      
      /* 
      if( label[0] == '_' || 
	  label[0] == '^' )
	{ 
	  int j = 2;
	  if( label[1] == '{' )
	    { 
	      label = label.del(1);
	      int k = label.index("}");
	      if( k >= 0 ) 
		{ 
		  label = label.del(k);
		  j = k;
		}
	    }
	  L.append(label(0,j-1));
	  label = label.del(0,j-1);
	  continue;
	}
      */
      
      int i  = label.index("\n",1);
      int i2 = label.index("\\",1);
      // int i3 = label.index("_",1);
      // int i4 = label.index("^",1);
      
      if( i  == -1 ) 
	{ i  = label.length(); }
      if( i2 == -1 || 
	  label[i2+1] != 'n' )
	{ i2 = label.length(); }
      
      //        if( i3 == -1 ) 
      //  	{ i3 = label.length(); }
      //        if( i4 == -1 )
      //  	{ i4 = label.length(); }
      
      if (i2 < i) i = i2;
      //        if( i3 < i) i = i3;
      //        if( i4 < i) i = i4;
      
      L.append(label(0,i-1));
      label = label.del(0,i-1);
    }
  
  // end split label
  
  //yshift = node_font_size; 
  yshift = 1.25 * node_font_size; 


  switch( label_p )
    {
    case central_pos: 
    case north_pos: 
    case south_pos: 
      start  = false; 
      middle = true; 
      end    = false; 
      break;
    case northwest_pos: 
    case west_pos: 
    case southwest_pos:
      start  = false; 
      middle = false; 
      end    = true; 
      break; 
    case northeast_pos: 
    case east_pos: 
    case southeast_pos: 
      start  = true;  
      middle = false; 
      end    = false; 
      break; 
    }


  point newpos = calculate_pos_node(pos, 
				    r1, r2, 
				    label_p, shape); 
  return newpos; 
}

void ScalableVectorGraphics::init_edge_label( string label , int index , 
					    list<string>& L )
{
  label = label.replace_all("%d",string("%d",index));
  label = label.replace_all("%D",string("%d",index+1));
  label = label.replace_all("%c",string("%c",index+'a'));
  label = label.replace_all("%C",string("%c",index+'A'));
  
  
  if (label[0] == '$') 
    { 
      /* stetzen von italic font */ 
      edge_italic = true; 
      label = label.del(0);
      int l = label.length();
      if (label[l-1] == '$') 
	{ label = label.del(l-1); }
    }
  
  // split label
  L.clear();
  while( label != "" )
    { 
      if( label[0] == '\n' )
	{ 
	  L.append("\n");
	  int k = 1;
	  while( label[k] == ' ' ) 
	    { k++; }
	  label = label.del(0,k-1);
	  continue;
	}
      
      if( label[0] == '\\' && 
	  label.length() > 1 && 
	  label[1] == 'n' )
	{ 
	  L.append("\n");
	  int k = 2;
	  while (label[k] == ' ') k++;
	  label = label.del(0,k-1);
	  continue;
	}
      
      /* 
      if( label[0] == '_' || 
	  label[0] == '^' )
	{ 
	  int j = 2;
	  if( label[1] == '{' )
	    { 
	      label = label.del(1);
	      int k = label.index("}");
	      if( k >= 0 ) 
		{ 
		  label = label.del(k);
		  j = k;
		}
	    }
	  L.append(label(0,j-1));
	  label = label.del(0,j-1);
	  continue;
	}
      */
      
      int i  = label.index("\n",1);
      int i2 = label.index("\\",1);
      // int i3 = label.index("_",1);
      // int i4 = label.index("^",1);
      
      if( i  == -1 ) 
	{ i  = label.length(); }
      if( i2 == -1 || 
	  label[i2+1] != 'n' )
	{ i2 = label.length(); }
      
      //        if( i3 == -1 ) 
      //  	{ i3 = label.length(); }
      //        if( i4 == -1 )
      //  	{ i4 = label.length(); }
      
      if (i2 < i) i = i2;
      //        if( i3 < i) i = i3;
      //        if( i4 < i) i = i4;
      
      L.append(label(0,i-1));
      label = label.del(0,i-1);
    }
}




void ScalableVectorGraphics::draw_text_node(point pos, double r1, double r2,
					 list<string> labels, 
					 double yshift, 
					 bool start, bool middle, bool end)
{

  if( ( labelColor() == invisible ) ||
      ( labels.empty() ) )
    { return; }
  
  if( labels.length() == 1 )
    { 
      draw_text_node( pos , labels.front() , start , middle , end ); 
      return; 
    }

  *this << "    <g class=\"NodeLabel\"" 
	<< " x=\"0\" "
        << endl
        << "       fill=" << labelColor() ;

  if( node_italic )
    { *this << " font-style=\"italic\" "; }
  *this << endl; 


  start  = true;
  middle = false;
  end    = false;  // s.n.  


  if (start)
      *this << "          text-anchor=\"start\" " << endl; 

  if (middle)
      *this << "          text-anchor=\"middle\" " << endl; 

  if (end)
      *this << "          text-anchor=\"end\" " << endl; 


  *this << "       transform=\"scale(1 -1) translate(" 
	//<< pos.xcoord() << " " 
	<< (pos.xcoord()-0.925*r1) << " "  // s.n.
        << -pos.ycoord() << ")\">"
	<< endl; 
  

  double y = 1.5*yshift - r2; // s.n.
  //double y = yshift - r2;


  string s; 
  forall( s , labels )
    {
      if( s == "\n" )
	{ y += yshift;  }
      else
	{
	  *this << "      <text y=\"" << y << "\">"
	        << endl
	        << "        " << s
	        << endl
	        << "      </text>"
	        << endl; 
	}
    }
  *this << "    </g>"
        << endl; 
}


void ScalableVectorGraphics::draw_text_node(point pos, 
					 string label, 
					 bool start, bool middle, 
					 bool end)
{
  if (labelColor() == invisible || label == "") return; 

  *this << "    <text class=\"NodeLabel\"" 
	<< " x=\"0\" y=\"" << 0.4*node_font_size << "\" " << endl;
  *this << "          fill=" << labelColor() ;
  if( node_italic ) { *this << " font-style=\"italic\" "; }
  *this << endl; 


  if (start)
     *this << "          text-anchor=\"start\" " << endl; 
  else
  if (middle)
     *this << "          text-anchor=\"middle\" " << endl; 
  else
  if (end)
     *this << "          text-anchor=\"end\" " << endl; 

  *this << "          transform=\"scale(1 -1) translate(" 
	<< pos.xcoord() << " " << -pos.ycoord() << ")\">"
        << endl
        << "    " << label 
        << endl
        << "    </text>"
        << endl; 
}




void ScalableVectorGraphics::draw_text_edge(point start ,  point end , 
					 list<string> labels, 
					 double yshift)
{
  if( ( labelColor() == invisible ) ||
      ( labels.empty() ) )
    { return; }

  if( labels.length() == 1 )
    { 
      draw_text_edge( start , end , labels.front() ); 
      return; 
    }

  double dx = - start.xcoord() + end.xcoord(); 
  double dy = - start.ycoord() + end.ycoord(); 

  bool   east      = (label_pos == east_pos); 
  bool   west      = (label_pos == west_pos); 
  double sgn       = -0.5; 
  bool   firstX    = true;
  string adjust    = "middle"; 

  if( label_pos != central_pos )
    {
      bool adjustPos = fabs(dx) >= fabs(dy); 
      bool posX = ( dx >= 0 );  
      bool posY = ( dy < 0 );  

      firstX = adjustPos; 
      
      if( posY )
	{
	  if( east )
	    { adjust = "end"; }
	  if( west )
	    { adjust = "start"; }
	}
      else
	{
	  if( east )
	    { adjust = "start"; }
	  if( west )
	    { adjust = "end"; }
	}

      if( dy == 0 ) 
	{ adjust = "middle"; }

      if( (  posX  &  posY  &  adjustPos & east ) ||
	  (  posX  &  posY  & !adjustPos & west ) ||
	  ( !posX  &  posY               & west ) ||
	  ( !posX  & !posY  &  adjustPos & west ) ||
	  ( !posX  & !posY  & !adjustPos & east ) || 
	  (  posX  & !posY               & east ) )
	{ sgn = 1; }
    }
  
  point pos(midpoint( start , end )); 
  
  *this << "    <g class=\"EdgeLabel\"" 
	<< " " << ( firstX ? "x" : "y" ) << "=\"0\" "
        << endl
        << "       fill=" << labelColor() ;
  if( edge_italic )
    { *this << " font-style=\"italic\" "; }
  *this << endl
        << "          text-anchor=\"" << adjust << "\" "
        << endl
        << "       transform=\"scale(1 -1) translate(" 
	<< pos.xcoord() << " " << -pos.ycoord() << ")\">"
        << endl; 
  
  string s; 
  double y = sgn * edge_font_size; 
  forall( s , labels )
    {
      if( s == "\n" )
	{ y+= yshift; }
      else
	{
	  *this << "      <text y=\"" << y << "\">"
	        << endl
	        << "        " << s
	        << endl
	        << "      </text>"
	        << endl; 
	}
    }
  *this << "    </g>"
	<< endl << endl; 
}


void ScalableVectorGraphics::draw_text_edge(point start ,
					 point end , 
					 string label )
{
  if( ( labelColor() == invisible ) ||
      ( label == "" ) )
    { return; }

  double dx = - start.xcoord() + end.xcoord(); 
  double dy = - start.ycoord() + end.ycoord(); 

  bool   east      = (label_pos == east_pos); 
  bool   west      = (label_pos == west_pos); 
  double sgn       = -0.5; 
  bool   firstX    = true;
  string adjust    = "middle"; 

  if( label_pos != central_pos )
    {
      bool adjustPos = fabs(dx) >= fabs(dy); 
      bool posX = ( dx >= 0 );  
      bool posY = ( dy < 0 );  

      firstX = adjustPos; 
      
      if( posY )
	{
	  if( east )
	    { adjust = "end"; }
	  if( west )
	    { adjust = "start"; }
	}
      else
	{
	  if( east )
	    { adjust = "start"; }
	  if( west )
	    { adjust = "end"; }
	}

      if( dy == 0 ) 
	{ adjust = "middle"; }

      if( (  posX  &  posY  &  adjustPos & east ) ||
	  (  posX  &  posY  & !adjustPos & west ) ||
	  ( !posX  &  posY               & west ) ||
	  ( !posX  & !posY  &  adjustPos & west ) ||
	  ( !posX  & !posY  & !adjustPos & east ) || 
	  (  posX  & !posY               & east ) )
	{ sgn = 1; }
    }
  
  point pos(midpoint( start , end )); 
  
  *this << "    <text class=\"EdgeLabel\" " 
	<< ( firstX ? "x=\"0\" y=\"" : "y=\"0\" x=\"" )
	<< sgn * edge_font_size << "\" "
        << endl
        << "          fill=" << labelColor() ;
  if( edge_italic )
    { *this << " font-style=\"italic\" "; }
  *this << endl
        << "          text-anchor=\"" << adjust << "\" "
        << endl
        << "          transform=\"scale(1 -1) translate(" 
	<< pos.xcoord() << " " << -pos.ycoord() << ")\">"
        << endl
        << "    " << label 
        << endl
        << "    </text>"
        << endl << endl; 
}

LEDA_END_NAMESPACE
