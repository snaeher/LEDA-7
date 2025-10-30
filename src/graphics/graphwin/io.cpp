/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  io.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/gml_gwgraph.h>
#include <LEDA/system/file.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/graph/dimacs.h>

/*
#include <iomanip.h>
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "local.h"
#include "graphwin_undo.h"

#include "ps.h" 
#include "svg.h" 

LEDA_BEGIN_NAMESPACE


//----------------------------------------------------------------------------
// postscript output
//----------------------------------------------------------------------------

static bool ps_draw_grid = false;
static double n_yfac = 0.5, n_xfac = 0.5;
static double e_yfac = 0.5, e_xfac = 0.5;
static bool adj_node_label_font = false;
static bool adj_edge_label_font = false;

static string translate_font(string font_name, gw_font_type ft)
{
  switch (ft) {

    case user_font:
    case roman_font: 
                     if (font_name == "Times") font_name += "-Roman";
                     break;

    case bold_font: 
                     font_name += "-Bold";
                     break;

    case italic_font: 
                     if (font_name == "Times")
                        font_name += "-Italic";
                     else
                        font_name += "-Oblique";
                     break;

    case fixed_font: 
                     font_name = "Courier";
                     break;
  }

  return font_name;
}


void GraphWin::ps_draw_obj_label(PostScript& ps, node v,edge e,string label)
{
  int clr;
  int label_clr;
  gw_font_type fn;
  double fn_size;

  if (v)
    { clr = get_color(v);
      label_clr = get_label_color(v);
      fn = node_label_font_type;
      fn_size = node_label_font_size;
     }
  else
    { clr = get_color(e);
      label_clr = get_label_color(e);
      fn = edge_label_font_type;
      fn_size = edge_label_font_size;
     }


  if (label_clr == gw_auto_color) label_clr = text_color(clr);

  if (label[0] == '$') fn = italic_font;

  list<string> L;
  double x,y,x1,y1;
  double yshift;

  label = label.replace_all("\n","\\n");

  split_label(v,e,label,L,x,y,x1,y1,yshift);

  if (v && get_label_pos(v) == central_pos && L.length() == 1)
  { ps.set_font(translate_font(ps_fontname,fn),fn_size);
    set_font(fn,fn_size,node_label_user_font);
    point p = get_position(v);
    ps.draw_ctext(p.xcoord(),p.ycoord(),L.head(),label_clr);
    return;
   }

  y -= 0.85*fn_size;

  double x0 = x;

  string s;
  while (!L.empty())
  { string s = L.pop();

    char c = s[0];

    if (c == '\n')
    { x = x0;
      y -= fn_size;
      continue;
     }


    if (c == '_' || c == '^')
     { ps.set_font(translate_font(ps_fontname,fn),0.6*fn_size);
       set_font(fn,0.6*fn_size,node_label_user_font);
       s = s.del(0);
       if (c == '_')
          { ps.draw_text(x,y-0.4*yshift,s,label_clr);
            if (!L.empty() && L.head()[0] != '^')
               x += 0.9*win_p->text_width(s);
           }
       else
         { ps.draw_text(x,y+0.7*yshift,s,label_clr);
           if (!L.empty() && L.head()[0] != '_')
               x += 0.9*win_p->text_width(s);
          }
      }
    else
     { ps.set_font(translate_font(ps_fontname,fn),fn_size);
       set_font(fn,fn_size,node_label_user_font);
       ps.draw_text(x,y,s,label_clr);
       x += 0.9*win_p->text_width(s);
      }
   }
}


void GraphWin::extract_labels(ostream& o, string psname, string pspath)
{

  // psname = basename(psname);

  int n = psname.length();
  while (--n >= 0) {
    if (psname[n] == '\\' || psname[n] == '/') break;
  }

  psname = psname(n+1,psname.length());


  graph&  G = get_graph();
  window& W = get_window();

  double xmin, ymin, xmax, ymax;
  get_bounding_box(xmin,ymin,xmax,ymax);

  double w = (xmax-xmin)*ps_factor;
  double h = (ymax-ymin)*ps_factor;

  o << "\\setlength{\\unitlength}{1pt}" << endl;

  if (adj_node_label_font || adj_edge_label_font )
  {
    o << "%\n"; 
    o << "\\begingroup\\makeatletter\\ifx\\LEDASetFont\\undefined%\n";
    o << "\\gdef\\LEDASetFont#1#2#3#4#5{%\n";
    o << "\\reset@font\\fontsize{#1}{#2pt}%\n"; 
    o << "\\fontfamily{#3}\\fontseries{#4}\\fontshape{#5}%\n";   
    o << "\\selectfont}%\n";
    o << "\\fi\\endgroup%\n";
    o << "%\n";
  }
  o << string("\\begin{picture}(%.2f,%.2f)(0,0)\n",w,h,xmin,ymin);
       
  int fsz;      
  gw_font_type ft;
  
  get_node_label_font(ft,fsz);  
  double n_font_size = ps_factor * fsz;
  
  get_edge_label_font(ft,fsz);
  double e_font_size = ps_factor * fsz;
   
  string fontfamily = "\\rmdefault";
  string fontseries = "\\mddefault";
  string fontshape  = "\\updefault";    
  string z_abst = "\\baselineskip";
   
  string psname0 = psname.del(".ps");  

  if (pspath != "")
    o << "\\put(0.0,0.0){\\includegraphics{" << pspath  << "/" << psname0 << "}}\n";
  else
    o << "\\put(0.0,0.0){\\includegraphics{" << psname0 << "}}\n";       

  node v;
  forall_nodes(v,G)
  { string label = get_label(v);
    gw_label_type label_t = get_label_type(v);
    gw_position label_pos = get_label_pos(v);
    
    if (label_t & index_label)
    { string s = get_index_label(v);      
      if (label_t != index_label && label_pos != central_pos && label[0] != '@')
      { point pos = get_position(v);
        double x = (pos.xcoord() - xmin)*ps_factor;
        double y = (pos.ycoord() - ymin)*ps_factor;        
        o << string("\\put(%.2f,%.2f){\\makebox(0,0){%s}}",x,y,~s) << endl;
       }
      if (label_t == index_label || label[0] == '@') label = s;
     }

    int ppos = label.index("@");
    if (ppos > -1) label = label(0,ppos-1);

    if (label == "") continue;

    string tmp = label;
    tmp = tmp.del_all("$");  
    tmp = tmp.del_all("\\");
    tmp = tmp.del_all("_"); 

    double tw = W.text_width(tmp);
    double th = W.text_height(tmp);
    point pos = compute_label_pos(v,tw,th);
    double x = (pos.xcoord() - xmin)*ps_factor;
    double y = (pos.ycoord() - ymin)*ps_factor;
    tw *= ps_factor;
    th *= ps_factor;
    
    // modified by O.Z.
    //o << string("\\put(%.2f,%.2f){\\makebox(0,0){%s}}",x+tw/2,y-th/2,~label);
   
    color c = get_label_color(v);
    int r, g, b;
    c.get_rgb(r,g,b);
                
    if (adj_node_label_font)
    { o << string("\\put(%.2f,%.2f){\\makebox(0,0){", x + tw * n_xfac,y - th * n_yfac);    
      o <<   string("\\smash{");
      o <<     string("\\LEDASetFont{%.0f}{%s pt}{%s}{%s}{%s}", n_font_size, ~z_abst,
                                                              ~fontfamily, ~fontseries, ~fontshape);
      o <<     string("{\\color[rgb]{%.2f,%.2f,%.2f}", float(r)/255.0f, float(g)/255.0f, float(b)/255.0f);
      o <<       string("%s",~label); 
      o <<     string("}"); // \color
      o <<   string("}");   // \smash
      o << string("}}");    // \makebox + \put
      o << endl;
    }
    else
    { o << string("\\put(%.2f,%.2f){\\makebox(0,0){",x+tw/2,y-th/2);
      o << string("\\color[rgb]{%.2f,%.2f,%.2f}", float(r)/255.0f, float(g)/255.0f, float(b)/255.0f);
      o << string("%s}}",~label);
      o << endl;
    }
  }

  edge e;
  forall_edges(e,G)
  { string  label = get_label(e);
    if (label == "") continue;
    
    string tmp = label;
    tmp = tmp.del_all("$");  
    tmp = tmp.del_all("\\");
    tmp = tmp.del_all("_");
    
    double tw = W.text_width(tmp);
    double th = W.text_height(tmp);
    point pos = compute_label_pos(e,tw,th);
    double x = (pos.xcoord() - xmin)*ps_factor;
    double y = (pos.ycoord() - ymin)*ps_factor;
    tw *= ps_factor;
    th *= ps_factor;
    
    // modfied by O.Z.
    // o << string("\\put(%.2f,%.2f){\\makebox(0,0){%s}}", x+tw/2,y-th/2,~label);
  
    color c = get_label_color(e);
    int r, g, b;
    c.get_rgb(r,g,b);
    
    if (adj_edge_label_font)
    { o << string("\\put(%.2f,%.2f){\\makebox(0,0){", x+ tw * e_xfac,y - th * e_yfac);    
      o <<   string("\\smash{");
      o <<     string("\\LEDASetFont{%.0f}{%s pt}{%s}{%s}{%s}", e_font_size, ~z_abst,
                                                              ~fontfamily, ~fontseries, ~fontshape);
      o <<     string("{\\color[rgb]{%.2f,%.2f,%.2f}", float(r)/255.0f, float(g)/255.0f, float(b)/255.0f);
      o <<       string("%s",~label); 
      o <<     string("}"); // \color
      o <<   string("}");   // \smash
      o << string("}}");    // \makebox + \put
      o << endl;  
    }
    else
    { o << string("\\put(%.2f,%.2f){\\makebox(0,0){",x+tw/2,y-th/2);
      o << string("\\color[rgb]{%.2f,%.2f,%.2f}", float(r)/255.0f, float(g)/255.0f, float(b)/255.0f);
      o << string("%s}}",~label);
      o << endl;
    }
  }

  o << "\\end{picture}" << endl;
  o << endl;
}



void GraphWin::print_ps(ostream& o, bool tex)
{
  PostScript ps;

  graph&  G = get_graph();
  window& W = get_window();

  double factor = 1;

  double bbx0, bby0, bbx1, bby1;
  get_bounding_box(bbx0,bby0,bbx1,bby1);


  double xorig;
  double yorig;

  if (ps_factor > 0)
   { factor = ps_factor;
     xorig = -bbx0*factor;
     yorig = -bby0*factor;
    }
  else
   { // special values for ps_factor

     //  0 : zoom to window 
     // -1 : zoom to graph 
     // -2 : 1:1

     double ps_xmin   = ps.get_xmin();
     double ps_ymin   = ps.get_ymin();
     double ps_width  = ps.get_xmax() - ps.get_xmin();
     double ps_height = ps.get_ymax() - ps.get_ymin();

     double x0 = get_xmin();
     double y0 = get_ymin();
     double x1 = get_xmax();
     double y1 = get_ymax();

     if (!G.empty() && ps_factor == -1) // zoom to graph
     { x0 = bbx0;
       y0 = bby0;
       x1 = bbx1;
       y1 = bby1;
      }

     if (ps_factor == -2) // 1:1
     { x0 = bbx0;
       y0 = bby0;
       x1 = bbx0 + W.pix_to_real((int)ps_width); 
       y1 = bby0 + W.pix_to_real((int)ps_height); 
      }

     double gw_dx = x1-x0;
     double gw_dy = y1-y0;
 
     double fx = ps_width/gw_dx;
     double fy = ps_height/gw_dy;
 
     if (fx < fy)
       factor = fx;
     else
       factor = fy;
 
     ps_width  = factor*gw_dx;
     ps_height = factor*gw_dy;
 
     xorig = -x0*factor + ps_xmin;
     yorig = -y0*factor + ps_ymin;
  }


  ps.set_origin(xorig,yorig);
  ps.set_scale(factor);
 
   
/* changed by O.Z.: no additional space */

  ps.set_bounding_box(factor*bbx0+xorig, factor*bby0+yorig,
                      factor*bbx1+xorig, factor*bby1+yorig);

/*
  ps.set_bounding_box(factor*bbx0+xorig-20, factor*bby0+yorig-20,
                      factor*bbx1+xorig+20, factor*bby1+yorig+20);
*/
  
  
  if (ps_draw_grid)
    ps.draw_grid(get_xmin(),get_ymin(),get_xmax(),get_ymax(),get_grid_dist());

  string msg = get_message();
  ps.set_font("Times-Roman",16);
  ps.draw_text(bbx0,bby1,msg,black);

  shape x;
  forall(x,shape_list)
  { ps.set_line_width(x.lwidth);
    ps.set_line_style(x.lstyle);

    if (x.poly.empty())
    { // circle
      point p = x.circ.center();
      double r = x.circ.radius();
      ps.draw_filled_ellipse(p.xcoord(),p.ycoord(),r,r,x.fcol);
      ps.draw_ellipse(p.xcoord(),p.ycoord(),r,r,x.bcol);
      ps.draw_ctext(p.xcoord(),p.ycoord(),x.label,x.tcol);
      continue;
     }

    // polygon

    ps.draw_filled_polygon(x.poly.vertices(),x.fcol);
    ps.draw_polygon(x.poly.vertices(),x.bcol);
    rectangle bb = BoundingBox(x.poly);
    double xc = bb.xmin() + 4/factor;
    double yc = bb.ymax() - win_p->text_height(x.label);
    ps.draw_text(xc,yc,x.label,x.tcol);
   }


  double r2min = MAXINT;

  node v;
  forall_nodes(v,G) 
  { //if (get_label_pos(v) != central_pos) continue;
    if (get_label_type(v) == no_label) continue;
    double r2 = get_radius2(v);
    if (r2min > r2) r2min = r2;
   }

  if (r2min == MAXINT) r2min = 12/factor;

  string font_name = ps_fontname;
  gw_font_type ft;
  int fsz;
  get_edge_label_font(ft,fsz);
  ps.set_font(translate_font(font_name,ft),edge_label_font_size);


  gw_edge_style style = solid_edge;

  ps.set_line_style((line_style) style);

  edge e;
  forall_edges(e,G) 
  { 
    if (get_width(e) == 0) continue;

    double w = get_thickness(e);
    ps.set_line_width(w);

    if (style != get_style(e)) 
    { style=get_style(e);
      ps.set_line_style((line_style) style);
     }

    list<point> P = get_edge_points(e);

    normalize_polygon(P,W.pix_to_real(1));

    if (P.length() < 2) continue;

    color c=get_color(e);
    gw_edge_shape sh=get_shape(e);

    int arr = 0;
    if (get_direction(e) == redirected_edge)  arr = 1;
    if (get_direction(e) == directed_edge)    arr = 2;
    if (get_direction(e) == bidirected_edge)  arr = 3;
    if (get_direction(e) == mid_directed_edge) arr = 4;
    if (get_direction(e) == mid_redirected_edge) arr = 8;
    if (get_direction(e) == mid_bidirected_edge) arr = 12;

    if (P.length() == 2) sh = poly_edge;

    switch (sh) {

      case poly_edge:   
                      if (edge_border && (line_style)style == solid && c!=black)
                          ps.draw_filled_arrow(P,arr,c);
                      else
                          ps.draw_polygon_edge(P,arr,c);
                      break;

      case circle_edge: { point p = P.head();
                          point q = P[P.succ(P.first())];
                          point r = P.tail();
                          ps.draw_circle_edge(p,q,r,arr,c);
                          break;
                         }

      case bezier_edge: ps.draw_bezier_edge(P,arr,c);
                        break;

      case spline_edge: ps.draw_spline_edge(P,arr,c);
                        break;
     }

   if (tex) continue;

   string label_str = get_label(e);
   if (label_str == "") continue;
   ps_draw_obj_label(ps,nil,e,label_str);

  }

  
  ps.set_line_style((line_style)solid_edge);

  get_node_label_font(ft,fsz);
  ps.set_font(translate_font(font_name,ft),node_label_font_size);

  forall_nodes(v,G) 
  { point  p  = get_position(v);
    double x  = p.xcoord();
    double y  = p.ycoord();
    double r1 = get_radius1(v);
    double r2 = get_radius2(v);
    color  c  = get_color(v);
    color  bc = get_border_color(v);

    ps.set_line_style(solid);

    double bw  = get_border_thickness(v);
    double bw2 = get_border_thickness(v)/2 - 1.0/factor;
    if (bw2 < 0) bw2 = 0;

    ps.set_line_width(bw);

    string label = get_label(v);

    int hatch = 0;

    if (label == "@hatched1") hatch = 1;
    if (label == "@hatched2") hatch = 2;

    int ppos = label.index("@");

    string xpmf;
    if (ppos > -1)
    { if (hatch == 0)
      { string name = label(ppos+1,label.length()-1);
        xpmf = xpm_path(name);
        if (xpmf == "") cerr << "xpm2ps: " + name + " not found." << endl; 
       }
      label = label(0,ppos-1);
     }


    char* pmap = get_pixmap(v);

    double pw=0,ph=0;

    if (pmap)
    { pw = W.pix_to_real(W.get_pixrect_width(pmap));
      ph = W.pix_to_real(W.get_pixrect_height(pmap));
     }


    double rndness = 0;

    switch(get_shape(v)) {

      case circle_node:
        r2=r1;
      case ellipse_node:
       { if (hatch)
            ps.draw_hatched_ellipse(x,y,r1,r2,hatch,c);
         else
            ps.draw_filled_ellipse(x,y,r1,r2,c);

         if (pmap)
         {  ps.clip_ellipse(x,y,r1,r2);
            if (xpmf != "") 
               ps.include_xpm(xpmf,x,y,pw,ph);
            else
               ps.put_pixrect(pmap,x,y,pw,ph);
            ps.clip_reset();
          }

         if (bw > 0) ps.draw_ellipse(x,y,r1-bw2,r2-bw2,bc);
         break;
       }
        
      case square_node:
        r2=r1;
      case roundrect_node: 
      case ovalrect_node: 
      case rectangle_node:
       { 
         if (get_shape(v) == roundrect_node) rndness = 0.5;
         if (get_shape(v) == ovalrect_node)  rndness = 1.0;

         if (hatch)
            ps.draw_hatched_rectangle(x-r1,y-r2,x+r1,y+r2,hatch,c);
         else
            if (rndness == 0)
              ps.draw_filled_rectangle(x-r1,y-r2,x+r1,y+r2,c);
            else
              ps.draw_filled_roundrect(x-r1,y-r2,x+r1,y+r2,rndness,c);

         if (pmap)
         {  ps.clip_rectangle(x-r1,y-r2,x+r1,y+r2);
            if (xpmf != "") 
               ps.include_xpm(xpmf,x,y,pw,ph);
            else
               ps.put_pixrect(pmap,x,y,pw,ph);
            ps.clip_reset();
          }

         if (bw <= 0)  break;

         if (rndness == 0)
           ps.draw_rectangle(x-r1+bw2,y-r2+bw2,x+r1-bw2,y+r2-bw2,bc);
         else
           ps.draw_roundrect(x-r1+bw2,y-r2+bw2,x+r1-bw2,y+r2-bw2,rndness,bc);
         break;
       }

      case rhombus_node : {
        list<point> L;
        L.append(point(x,y-r2));
        L.append(point(x+r1,y));
        L.append(point(x,y+r2));
        L.append(point(x-r1,y));

        if (hatch)
           ps.draw_hatched_polygon(L,hatch,c);
        else
           ps.draw_filled_polygon(L,c);

        if (pmap)
        { ps.clip_polygon(L);
          if (xpmf != "") 
             ps.include_xpm(xpmf,x,y,pw,ph);
          else
              ps.put_pixrect(pmap,x,y,pw,ph);
           ps.clip_reset();
         }

        if (bw > 0) ps.draw_polygon(L,bc);
        break;
      }

     default: // unhandled shapes
              break;
    }


    if (tex) continue;

    int  label_t = get_label_type(v);

    if (label_t == no_label) continue;

    //string      label =     get_label(v);

    gw_position label_pos = get_label_pos(v);
    color  lc = get_label_color(v);

    if (lc == -1)
    { lc = black;
      if (c == black ||
          c == red   ||
          c == blue  ||
          c == violet||
          c == brown ||
          c == pink  ||
          c == green2||
          c == blue2 ||
          c == grey3 ) lc = white;
     }

    if (label_t & index_label)
    { string s = get_index_label(v);
      if (label_t != index_label && label_pos != central_pos && pmap == 0)
      { set_label_pos(v,central_pos);
        ps_draw_obj_label(ps,v,nil,s);
        set_label_pos(v,label_pos);
       }
      if (label_t == index_label || pmap) label = s;
     }

    if (label == "") continue;

    ps_draw_obj_label(ps,v,nil,label);
  }
 
  ps.print(o);
}


void GraphWin::print_svg(ostream& out)
{
  GraphWin& gw = *this;

  graph&  G = gw.get_graph();
  window& W = gw.get_window();
  
  edge    e;
  node    v; 

  /*
    For alternatives change the following code region. 
    Take care to have a variable svg of type 
    ScalableVectorGraphics. 
   */

  ScalableVectorGraphics svg; 

  if (graph_name != DefGraphName) svg.set_title(graph_name);

  // part I (see description)
  
  // window coordinates

  if( ! svg_whole_graph )
  {
    svg.set_bounding_box(gw.get_xmin(), gw.get_ymin(), 
                          gw.get_xmax(), gw.get_ymax(), W.scale() ); 
   }
  else
  { double bbx0, bby0, bbx1, bby1;
    gw.get_bounding_box(bbx0,bby0,bbx1,bby1);

    double margin = (bbx1-bbx0)/15;

    bbx0 -= margin;
    bbx1 += margin;
    bby0 -= margin;
    bby1 += margin;

    // last parameter is scaling factor (need to be adjusted)

    svg.set_bounding_box(bbx0,bby0,bbx1,bby1, W.width()/(bbx1-bbx0)); 
  }

  // set node and edge label fonts

  gw_font_type node_label_font; 
  int node_label_font_sz; 
  gw.get_node_label_font(node_label_font, node_label_font_sz); 
  svg.set_node_label_font(node_label_font, node_label_font_sz);

  gw_font_type edge_label_font; 
  int edge_label_font_sz; 
  gw.get_edge_label_font(edge_label_font, edge_label_font_sz); 
  svg.set_edge_label_font(edge_label_font,edge_label_font_sz); 

  svg.set_transparency(1-svg_edge_transparency);


  // draw shapes

  shape x;
  forall(x,shape_list)
  { 
    svg.set_shape_attr(x.fcol,x.bcol,x.lwidth); // x.lstyle ?

    if (x.poly.empty())
    { // circle
      point p = x.circ.center();
      double r = x.circ.radius();
      svg.draw_circle(p,r);
      svg.draw_ctext(p,x.label);
      continue;
     }

    // polygon
    svg.draw_filled_polygon(x.poly.vertices());
    svg.draw_polygon(x.poly.vertices());

    rectangle bb = BoundingBox(x.poly);
    svg.draw_ctext(bb.center(),x.label);

   }


  
  // part II (see description) 

  int index = 0; 
  forall_edges( e , G ) 
    { 
      if( gw.get_width(e) == 0 ) 
	{ continue; }
      
      list<point> P = gw.get_edge_points(e);
      
      normalize_polygon(P, W.pix_to_real(1));
      
      if( P.length() < 2 ) 
	{ continue; }
      
      int style = 0; 
      switch( gw.get_style(e) )
	{
	case dashed_edge: 
	  style = 1; 
	  break; 
	case dotted_edge: 
	  style = 2; 
	  break; 
	case dashed_dotted_edge: 
	  style = 3; 
	  break; 
	default: 
	  style = 0; 
	}

      svg.set_line_attr( gw.get_thickness(e) , 
			style , 
			gw.get_color(e) ); 
      
      int arr = 0;
      switch( gw.get_direction(e) )
	{
	case redirected_edge: 
	  arr = 1; 
	  break; 
	case directed_edge: 
	  arr = 2; 
	  break;
	case bidirected_edge: 
	  arr = 3; 
	  break;
	case mid_directed_edge: 
	  arr = 4; 
	  break; 
	case mid_redirected_edge: 
	  arr = 8; 
	  break; 
	case mid_bidirected_edge: 
	  arr = 12; 
	  break; 
	default: 
	  arr = 0; 
	}

      gw_edge_shape sh   = gw.get_shape(e);
      bool draw_border   = gw.get_edge_border(); // s.n.
      gw_position   lpos = gw.get_label_pos(e); 
      if (P.length() == 2) 
	{ sh = poly_edge; }
      
      svg.set_label_attr( gw.get_label_color(e) , gw.get_bg_color() ); 
      svg.set_shape_attr(gw.get_color(e),black,gw.get_thickness(e)/10);
      svg.draw_edge(P , arr , sh, draw_border, lpos , 
		    gw.get_label(e), index++); 
    }
  

  // part III (see description)

  set_font(node_label_font_type,node_label_font_size,node_label_user_font);
  
  index = 0; 
  forall_nodes( v , G ) 
  { 
    color  c       = gw.get_color(v); 
    color  lc      = gw.get_label_color(v); 
    gw_position lp = gw.get_label_pos(v); 
    point  p       = gw.get_position(v);
    double bw2     = gw.get_border_thickness(v)/2 - 1.0; 
    if( bw2 < 0 ) 
      { bw2 = 0; }
    
    svg.set_shape_attr( c , 
		       gw.get_border_color(v) , 
		       gw.get_border_thickness(v) ); 

    string label = gw.get_label(v);

    // clear label if it does not fit into node

    double tw = gw.get_window().text_width(label);
    double th = gw.get_window().text_height(label);
    if (th > 1.9*gw.get_radius2(v) || tw > 1.9*gw.get_radius1(v)) label = "";

    svg.set_label_attr( lc , c ); 

    svg.draw_node( gw.get_shape(v) , 
		   p.xcoord() , p.ycoord() , 
		   gw.get_radius1(v) , 
		   gw.get_radius2(v) ,
		   bw2 , lp , 
		   gw.get_label_type(v) , 
		   label, 
		   index++ ); 

  } // end for all v in G



  // part IV (see description)

  svg.print(out); 
}



static bool confirm_overwrite(string fname, window* win_p)
{ 
#if defined (__win32__)
  fname = fname.replace_all("\\", "/");
#endif
  panel P;
  P.buttons_per_line(2);
  P.text_item("");
  P.text_item("\\bf File exists"); 
  P.text_item("");
  P.text_item(string("\\tt\\blue %s",~fname)); 
  P.text_item("");
  P.button("overwrite",0);
  P.button("cancel",1);
  return (P.open(*win_p) == 0);
}


static bool confirm_loading(window* win_p)
{ panel P;
  P.buttons_per_line(2);
  P.text_item("");
  P.text_item("\\bf\\blue Overwrite existing graph ?"); 
  P.fbutton("overwrite",0);
  P.button("cancel",1);
  return (P.open(*win_p) == 0);
}


static int load_or_join(window* win_p)
{ panel P;
  P.text_item("");
  P.text_item("\\bf\\blue Overwrite or extend existing graph ?"); 
  P.fbutton("overwrite",1);
  P.button("extend",2);
  P.button("cancel",0);
  return P.open(*win_p);
}


bool GraphWin::save_wmf(string fname, bool ask_override)
{ window& W = get_window();

  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;

  int curs = W.set_cursor(XC_watch);

  double  d = W.pix_to_real(16);

  double x0,y0,x1,y1;
  get_bounding_box(x0,y0,x1,y1);

  W.open_metafile(fname);
  W.draw_box(x0-d,y0-d,x1+d,y1+d,white);
  W.draw_grid(x0-d,y0-d,x1+d,y1+d);
  draw_shapes();
  draw_graph();
  W.close_metafile();
  W.set_cursor(curs);
  return true;
}



bool GraphWin::save_ps(string fname, bool ask_override) 
{
  window& W = get_window();

  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;

  ofstream o(fname,ios::out);
  
  if (!o)
  { string path = get_directory() + "/" + fname;
    LEDA_EXCEPTION(0, string("save ps: cannot write to ") + path);
    return false;
  }

  int curs = W.set_cursor(XC_watch);
  print_ps(o);
  W.set_cursor(curs);
  if (o.fail()) return false;
  o.close();
  return true;
}


bool GraphWin::save_svg(string fname, bool ask_override) 
{
  window& W = get_window();

  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;

/*

  // svg options panel

  int svg_width = 50;
  bool svg_background = true;
  bool svg_title = true;

  panel pan("SVG Settings");
  pan.text_item("\\bf\\blue SVG Settings");
  pan.text_item("");
  pan.int_item("Image Size",svg_width,1,100);        
  pan.bool_item("Show Box",  svg_background);
  pan.bool_item("Show Title", svg_title);
  pan.text_item("");
  pan.button("continue");
  pan.open(*win_p);
*/

  ofstream o(fname,ios::out);

  if (!o)
  { string path = get_directory() + "/" + fname;
    LEDA_EXCEPTION(0, string("save svg: cannot write to ") + path);
    return false;
  }
  int curs = W.set_cursor(XC_watch);
  print_svg(o);
  W.set_cursor(curs);
  if (o.fail()) return false;
  o.close();
  return true;
}



bool GraphWin::save_latex(string fname, string pspath, double xmin, double ymin,
                                                       double xmax, double ymax,
                                                       double width, 
                                                       bool ask_override) 
{

/*
  int p = fname.last_index("/");
*/

  int p = fname.length()-1;
  while (p > 0 && fname[p] != '/') p--;

  pspath = fname.head(p);

//cout << "pspath = " << pspath << endl;


  window& W = get_window();

  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;

  ofstream texout(fname,ios::out);
  if (!texout) return false;


  if (xmin == xmax) 
    get_bounding_box(xmin,ymin,xmax,ymax);

  double fsave = ps_factor;

  if (width > 0 && xmax > xmin)
     ps_factor = (width*PSPixelPerCM)/(xmax-xmin);
  else
     ps_factor = 1;

  fname = fname.del(".tex") + ".ps";

  extract_labels(texout,fname,pspath);
  if (texout.fail()) return false;
  texout.close();

  ofstream psout(fname,ios::out);
  if (!psout) return false;
  int curs = W.set_cursor(XC_watch);
  print_ps(psout,true);
  W.set_cursor(curs);
  if (psout.fail()) return false;
  psout.close();

  ps_factor = fsave;

  return true;
}



//----------------------------------------------------------------------------
// GML output
//----------------------------------------------------------------------------


static gw_position flip_pos(gw_position pos)
{ // change gw_position according to a flip of the y-axis
 
  gw_position newp = pos;
  switch(pos)
  {
    case central_pos:   break;
    case north_pos:     { newp = south_pos; break; }
    case south_pos:     { newp = north_pos; break; }
    case west_pos:      { newp = east_pos; break; }
    case east_pos:      { newp = west_pos; break; }
    case northwest_pos: { newp = southeast_pos; break; }
    case northeast_pos: { newp = southwest_pos; break; }
    case southwest_pos: { newp = northeast_pos; break; }
    case southeast_pos: { newp = northwest_pos; break; }

  }
  return newp;
}


void GraphWin::flip_y()
{
  graph& G = get_graph();

  node v = G.first_node();

  if (v == nil) return;

  double xmin,xmax,ymin,ymax;
  get_bounding_box(xmin,ymin,xmax,ymax);

  double ymid = (ymin+ymax)/2;

  forall_nodes(v,G)
  { point p = n_info[v].pos;
    n_info[v].pos = point(p.xcoord(), 2*ymid - p.ycoord());
    n_info[v].label_pos = flip_pos(n_info[v].label_pos);
  }

  edge e;
  forall_edges(e,G)
  { list<point>& poly = get_poly(e);
    list_item it;
    forall_items(it,poly)
    { point p = poly[it];
      poly[it] = point(p.xcoord(), 2*ymid - p.ycoord());
     }

    point p = e_info[e].s_anchor;
    e_info[e].s_anchor = point(p.xcoord(),-p.ycoord());

    point q = e_info[e].t_anchor;
    e_info[e].t_anchor = point(q.xcoord(),-q.ycoord());

    e_info[e].label_pos = flip_pos(e_info[e].label_pos);
  }
}


bool GraphWin::save_gml(ostream& out)
{ bool save_flush = set_flush(false);
  save_node_attributes();
  save_edge_attributes();
  flip_y();
  bool b = write_gml(*this,out);
  restore_node_attributes();
  restore_edge_attributes();
  set_flush(save_flush);
  return b;
}


bool GraphWin::save_gml(string fname, bool ask_override)
{ 
  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;
  ofstream out(fname);
  return save_gml(out);
}


//----------------------------------------------------------------------------

static ostream& operator << (ostream& o, const node_info& inf) {
  o << inf.pos.xcoord()   << ' ';
  o << inf.pos.ycoord()   << ' ';
  o << int(inf.shape)     << ' ';
//o << int(inf.border_clr)<< ' ';
  o << inf.border_clr     << ' '; // r g b
  o << inf.border_w       << ' '; 
  o << inf.r1             << ' ';
  o << inf.r2             << ' ';
//o << int(inf.clr)       << ' ';
  o << inf.clr            << ' '; // r g b
  o << int(inf.label_t)   << ' ';
//o << int(inf.label_clr) << ' ';
  o << inf.label_clr      << ' '; // r g b
  o << int(inf.label_pos) << ' ';
  o << inf.label.replace_all("\n","\\n");
  return o;
}

//----------------------------------------------------------------------------

// static int translate_pos[] = {4,0,1,2,5,8,7,6,3};

static void skip_comments(istream& i)
{ if (i)
  { char c; i >> c;
    while (c == '#')
    { read_line(i);
      if (!i) break;
      i >> c;
     }
    if (c != '#') i.putback(c);
  }
}

inline void read_color(istream& i, color& clr, double version)
{ 
  if (version < 1.4)
  { int index;
    i >> index;
    if (index == 0 || index == 1)
    { // swap black und white
      index = 1 - index;
     }
    if (index > 16) index = -1;
    clr = color::get(index);
    return;
   }

  i >> clr;
}

static istream& read_node_info(istream& i,node_info& inf,double version) 
{
  double x,y;
  float r1,r2,border_w; 
  int    label_t,shape,label_pos;
  color  border_clr,clr,label_clr;

  skip_comments(i);

  i >> x;
  i >> y;
  i >> shape;
  read_color(i,border_clr,version);
  i >> border_w;
  i >> r1;
  i >> r2;
  read_color(i,clr,version);
  i >> label_t;
  read_color(i,label_clr,version);
  i >> label_pos;

  //skip blanks
  char c;
  do i.get(c); while (c == ' ');
  i.putback(c);

/*
  // this is done by string::read anyway

  // fix for dos line delimiter '\r\n'
  string s = read_line(i);
  if (s.ends_with("\r")) 
    inf.label = s.head(-1);
  else
    inf.label = s;
*/

  inf.label.read_line(i);


  inf.pos       = point(x,y);
  inf.clr       = clr;
  inf.r1        = r1;
  inf.r2        = r2;
  inf.shape     = (gw_node_shape)shape;
  inf.border_clr= border_clr;
  inf.border_w  = border_w;
  inf.label_t   = (gw_label_type)label_t;
  inf.label_pos = (gw_position)label_pos;
  inf.label_clr = label_clr;
  inf.selected  = 0;
  inf.pixmap    = 0;

  inf.label_curs = -1;

  return i;
}

//----------------------------------------------------------------------------

static void print_polygon(ostream& o, const list<point>& P) 
{ point p;
  o << P.size() << ' ';
  forall(p,P) o << p << ' ';
}

//----------------------------------------------------------------------------

static void read_polygon(istream& i, list<point>& P) 
{ point p;
  int   n = 0;
  P.clear();
  i >> n;
  while (n--) 
  { i >> p;
    P.append(p);
   } 
}

//----------------------------------------------------------------------------

static ostream& operator << (ostream& o, const edge_info& inf) 
{
  o << inf.width          << ' ';
//o << int(inf.clr)       << ' ';
  o << inf.clr            << ' '; // rgb
  o << int(inf.shape)     << ' ';
  o << int(inf.style)     << ' ';
  o << int(inf.dir)       << ' ';
  o << int(inf.label_t)   << ' ';
//o << int(inf.label_clr) << ' ';
  o << inf.label_clr      << ' '; // rgb
  o << int(inf.label_pos) << ' ';
  o << inf.s_anchor       << ' ';
  o << inf.t_anchor       << ' ';
  print_polygon(o,inf.p);
  o << inf.label.replace_all("\n","\\n");
  return o;
}

//----------------------------------------------------------------------------  
                                                                              
static istream& read_edge_info(istream& i,edge_info& inf, double version) 
{   
  float width;
  int    label_t,style,dir,shape,label_pos;
  color  clr, label_clr;

  skip_comments(i);

  i >> width;
  read_color(i,clr,version);
  i >> shape;
  i >> style;
  i >> dir;
  i >> label_t;
  read_color(i,label_clr,version);
  i >> label_pos;
  i >> inf.s_anchor;
  i >> inf.t_anchor;

  read_polygon(i,inf.p);

  //skip blanks 
  char c;
  do i.get(c); while (c == ' ');
  i.putback(c);

/*
  // this is done by string::read anyway

  // fix for dos line delimiter '\r\n'
  string s = read_line(i);
  if (s.ends_with("\r")) 
    inf.label = s.head(-1);
  else
    inf.label = s;
*/

  inf.label.read_line(i);


  inf.width     = width;
  inf.clr       = clr;
  inf.shape     = (gw_edge_shape)shape;
  inf.style     = (gw_edge_style)style;
  inf.dir       = (gw_edge_dir)dir;
  inf.label_t   = (gw_label_type)label_t;
  inf.label_clr = label_clr;
  inf.label_pos = (gw_position)label_pos;
  inf.selected  = 0;
  inf.slider_f  = 0;

/*
  if (inf.slider_f) { 
    for(int i=0; i<MAX_SLIDERS; i++) inf.slider_f[i] = 0;
  }
*/

  inf.label_curs = -1;

  return i;
}                                                                              
                                                                              

//----------------------------------------------------------------------------

inline void comment(ostream& out, const char* s)
{ out << string("# %s",s) << endl; }


bool GraphWin::save_layout(ostream& out) 
{
  out.precision(7);

  comment(out,"version string");
  out << string("GraphWin %f",FileFormatVersion) << endl;

  comment(out,"scaling  wxmin  wymin  wxmax  wymax");
  out << win_p->scale() << " ";
  out << get_xmin() << " " << get_ymin() << " ";
  out << get_xmax() << " " << get_ymax() << endl; 

  comment(out,"node label font and size");
  out << (int)node_label_font_type << " " << node_label_font_size << endl;

  comment(out,"edge label font and size");
  out << (int)edge_label_font_type << " " << edge_label_font_size << endl;

  comment(out,"node index format");
  out << node_index_format << endl;

  comment(out,"edge index format");
  out << edge_index_format << endl;

  comment(out,"multi-edge distance");
  out << edge_distance << endl;

  comment(out,"");
  comment(out,"node infos");
  comment(out,"x y shape bclr(r,g,b) bwidth r1 r2 clr(r,g,b) ltype lclr(r,g,b) lpos lstr");

  node v;
  forall_nodes(v,*gr_p) out << n_info[v] << endl;

  comment(out,"");
  comment(out,"edge infos");
  comment(out,"width clr(r,g,b) shape style dir ltype lclr(r,g,b) lpos sanch tanch poly lstr");

  forall_nodes(v,*gr_p) 
  { edge e;
    forall_adj_edges(e,v)
    { if (source(e) != v) continue; // undirected graph
      float& w = e_info[e].width;
      if (w == int(w)) w += float(0.001);
      out << e_info[e] << endl;
     }
   }
  
  out << endl;

  return true;
}


bool GraphWin::save_layout(string fname, bool ask_override) 
{
  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;
  ofstream out(fname);
  return save_layout(out);
 }



bool GraphWin::save_gw(ostream& out) 
{ if (out.fail()) return false;
  gr_p->write(out);
  save_layout(out);
  return out.good() != 0;
}


bool GraphWin::save_gw(string fname, bool ask_overwrite) 
{
  if (is_file(fname) && ask_overwrite 
                     && !confirm_overwrite(fname,win_p)) return true;
  ofstream out(fname);
  return save_gw(out);
}

//----------------------------------------------------------------------------


void GraphWin::load_finish(double wx0, double wy0, double wx1, double /*wy1*/)
{
  graph& G = get_graph();
  window& W = get_window();

  bool old_flush = set_flush(false);

  double R = 0;
  double BW = 0;
  double LW = 0;
  
  int n = 0;
  int m = 0;

  int pm_count = 0;
  int pos_count = 0;

  node v;
  forall_nodes(v,G)
  { if (get_color(v)==invisible && get_border_color(v)==invisible) continue;
    n++;
    R  += get_radius2(v); 
    BW += get_border_thickness(v);
    string label = get_label(v);
    if (label.index('@') > -1) pm_count++;

    if (get_position(v) != point(0,0)) pos_count++;
   }


  edge e;
  forall_edges(e,G)
  { if (get_color(e) == invisible) continue;
    m++;
    LW += get_thickness(e);
   }

  double f = R/(n*get_node_radius2());

  set_node_radius1(f*get_node_radius1(),false);
  set_node_radius2(f*get_node_radius2(),false);
  set_node_border_thickness(BW/n,false);

  if (LW > 0) set_edge_thickness(LW/m,false);

  if (pos_count == 0) {
    // no layout
    // message("\\bf Computing initial layout ...");
    gw_spring_embed(*this,true);
    gw_spring_embed(*this,false);
  }


  if (zoom_objects && pm_count > 0) 
  { message("\\bf\\blue Pixmap nodes:\\black ~~Turning zoom objects off.");
    zoom_objects=false;
   }

  if (W.is_open()) 
  { if (wx0 == wx1)
    { // no bounding box suplied
      zoom_graph();
    }
    else
    { if (show_status)
      { double f = (wx1-wx0)/(get_xmax()-get_xmin());
        wy0 -= f*win_p->pix_to_real(status_win_height);
       }
      window_init(wx0,wx1,wy0);
    }
   }

  leda_wait(1.5);
  message("");

  set_flush(old_flush);
}


static string read_index_format(istream& in)
{ string s;
  in >> s;
  switch (s[0]) {
    case '0': s = "%d"; break;
    case '1': s = "%D"; break;
    case '2': s = "%c"; break;
    case '3': s = "%C"; break;
  }
  return s;
}


int GraphWin::read_layout(istream& in, const list<node>& V,
                          double wx0, double wy0, double wx1, double wy1)
{
  double version = 0;

  double old_scaling;

  skip_comments(in);

  string str;
  in >> str;

  if (str != "GraphWin")  return 5;  // no layout information

  skip_comments(in);

  in >> version;

  if (version <= 1.31)  // version < 1.32
  { LEDA_EXCEPTION(1, string("read gw: version %.4f not supported",version));
    return 5;
   }

  if (version > FileFormatVersion) 
  { LEDA_EXCEPTION(1,string("gw-format version %.2f newer than expected",version));
    return 5;
   }


  skip_comments(in);
  in >> old_scaling;


  //skip blanks and \r carriage return (MS-Windows)
  char c;
  do in.get(c); while (c == ' ' || c == '\r');
  in.putback(c);

  double xx0 = 0, xx1 = 0, yy0 = 0, yy1 = 0;

  if (c == '-' || isdigit(c)) // window coordinates
     in >> xx0 >> yy0 >> xx1 >> yy1;

  if (wx0 == wx1)
  { if (wx0 == 0)
    { wx0 = xx0;
      wx1 = xx1;
      wy0 = yy0;
      wy1 = yy1;
     }
    else
    { double dx0 = wy0 - get_xmin();
      double dx1 = get_xmax() - wy0;
      double dy0 = wy1 - get_ymin();
      double dy1 = get_ymax() - wy1;
      double x = (xx0 + xx1)/2;
      double y = (yy0 + yy1)/2;
      double f = 10*(xx1 - xx0)/(get_xmax() - get_xmin());
      if (f == 0) f = 10;
      wx0 = x - f*dx0;
      wy0 = y - f*dy0;
      wx1 = x + f*dx1;
      wy1 = y + f*dy1;
     }
   }


  bool old_format = (version < FileFormatVersion);

  if (old_format && get_window().is_open())
  { message(string("\\bf Reading GW-Format Version %.2f.",version));
    leda_wait(1.0);
    message("");
   }


  float f = float(old_scaling/win_p->scale());

  if (get_zoom_objects() || wx0 < wx1) f = 1;


  { int i; 
    double d;

    skip_comments(in);
    in >> i >> d;
    node_label_font_type = gw_font_type(i);
    node_label_font_size = f*d;

    skip_comments(in);
    in >> i >> d;
    edge_label_font_type = gw_font_type(i);
    edge_label_font_size = f*d;

    skip_comments(in);
    node_index_format = read_index_format(in);

    skip_comments(in);
    edge_index_format = read_index_format(in);

    skip_comments(in);
    in >> d;
    edge_distance = f*d;
   }


  node v;
  forall(v,V) 
  { node_info ni;
    if (read_node_info(in,ni,version))
    { ni.r1 *= f;
      ni.r2 *= f;
      ni.border_w *= f;
      n_info[v] = ni;
      n_info[v].init = true;
     }
    else break;
   }

  if (in.fail()) 
  { // illegal node infos
    //if (old_flush)
    { embed_edges();
      redraw();
     }
    return 3;
  }


  forall(v,V) 
  { edge e;
    forall_adj_edges(e,v)
    { if (source(e) != v) continue; // undirected graph
      edge_info ei;
      if (read_edge_info(in,ei,version))
      { if (gr_p->is_undirected()) ei.dir = undirected_edge;
        if (ei.width != int(ei.width)) 
          ei.width *= f;
        else
          ei.width *= n_info[source(e)].border_w;
        e_info[e] = ei;
        e_info[e].init = true;
       }
      else break;
     }
   }
  
  if (in.fail()) 
  { //  illegal edge infos
    //if (old_flush) 
    { embed_edges();
      redraw();
     }
    return 4;
  }

  load_finish(wx0,wy0,wx1,wy1);

  if (old_format && get_window().is_open()) message("");

  return 0;
}



int GraphWin::load_layout(istream& in)
{

  double version = 1.2;
  double old_scaling;

  double wx0=0, wy0=0, wx1=0, wy1=0;

  bool fl = set_flush(false);

  skip_comments(in);

  string str;
  in >> str;

  if (str == "GraphWin")
  { skip_comments(in);
    in >> version;
    skip_comments(in);
    in >> old_scaling;
   }
  else
     old_scaling = atof(str);

  char c;
  do in.get(c); while (c == ' ' || c == '#');
  in.putback(c);

  if (c == '-' || isdigit(c)) // window coordinates
     in >> wx0 >> wy0 >> wx1 >> wy1;


  float f = float(old_scaling/win_p->scale());

  if (get_zoom_objects() || wx0 < wx1) f = 1;

  { int i; 
    double d;

    skip_comments(in);
    in >> i >> d;
    node_label_font_type = gw_font_type(i);
    node_label_font_size = f*d;

    skip_comments(in);
    in >> i >> d;
    edge_label_font_type = gw_font_type(i);
    edge_label_font_size = f*d;

    skip_comments(in);
    node_index_format = read_index_format(in);

    skip_comments(in);
    edge_index_format = read_index_format(in);

    skip_comments(in);
    in >> d;
    edge_distance = f*d;
   }


  node_array<double> xpos(*gr_p);
  node_array<double> ypos(*gr_p);
  node_array<double> xrad(*gr_p);
  node_array<double> yrad(*gr_p);

  node v;
  forall_nodes(v,*gr_p) 
  { node_info ni;
    read_node_info(in,ni,version);
    ni.r1 *= f;
    ni.r2 *= f;
    ni.border_w *= f;
    xpos[v] = ni.pos.xcoord();
    ypos[v] = ni.pos.ycoord();
    xrad[v] = ni.r1;
    yrad[v] = ni.r2;
    set_info(v,ni,N_ALL & ~N_SIZE);
   }

  if (in.fail()) 
  { set_flush(fl);
    return 3;
   }

  edge_array<list<double> > xbends(*gr_p);
  edge_array<list<double> > ybends(*gr_p);

  edge_array<double> xsanch(*gr_p);
  edge_array<double> ysanch(*gr_p);
  edge_array<double> xtanch(*gr_p);
  edge_array<double> ytanch(*gr_p);


  bool adjust_node_size = false;

  forall_nodes(v,*gr_p) 
  { edge e;
    forall_adj_edges(e,v)
    { if (source(e) != v) continue; // undirected graph
      edge_info ei;
      read_edge_info(in,ei,version);
      if (ei.width != int(ei.width)) 
        ei.width *= f;
      else
        ei.width *= n_info[source(e)].border_w;

      if (ei.s_anchor != point(0,0) || ei.t_anchor != point(0,0))
         adjust_node_size = true;

      ei.p.pop();
      ei.p.Pop();
      point q;
      forall(q,ei.p)
      { xbends[e].append(q.xcoord());
        ybends[e].append(q.ycoord());
       }

      xsanch[e] = ei.s_anchor.xcoord();
      ysanch[e] = ei.s_anchor.ycoord();
      xtanch[e] = ei.t_anchor.xcoord();
      ytanch[e] = ei.t_anchor.ycoord();

      set_info(e,ei,E_ALL);
     }
   }

  if (in.fail()) 
  { set_flush(fl);
    return 4;
   }


  if (adjust_node_size)
  { double dx=0, dy=0;
    double f;
    fill_win_params(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,f,f);
    transform_layout(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,f,f);
    set_flush(false);
    node v;
    forall_nodes(v,*gr_p)
       set_border_thickness(v,f*get_border_thickness(v));
   }
  else
  { double dx=0, dy=0;
    double fx=1, fy=1;
    fill_win_params(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
    transform_layout(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
  }

  set_flush(fl);
  set_layout(xpos,ypos,xrad,yrad,xbends,ybends,xsanch,ysanch,xtanch,ytanch);

  return 0;
}


int GraphWin::load_gml_layout(istream& in)
{

  graph& G = get_graph();
  // save positions and bends

  int n = G.number_of_nodes();
  int m = G.number_of_edges();
  int i;

  point*        old_pos   = new point[n];
  list<point>*  old_bends = new list<point>[m];

  i = 0;
  node v;
  forall_nodes(v,G)  old_pos[i++] = get_position(v);

  i = 0;
  edge e;
  forall_edges(e,G)  old_bends[i++] = get_bends(e);

  bool fl = set_flush(false);


  if (gw_undo) gw_undo->start_complex_command();

  graph save_g;
  save_g.join(*gr_p);

  bool gml_flip = false;
  gml_gwgraph parser(this,in,gml_flip);
  flip_y();

  if (gw_undo) gw_undo->finish_complex_command(false);


  if(parser.errors())
  { if (gw_undo) {
      gw_undo->undo();
      gw_undo->redo_clear();
    }
    else { 
      gr_p->clear();
      gr_p->join(save_g);
    }
    redraw();
    set_flush(fl);
    message("");
    return 1;
  }


  node_array<double> xpos(*gr_p);
  node_array<double> ypos(*gr_p);
  edge_array<list<double> > xbends(*gr_p);
  edge_array<list<double> > ybends(*gr_p);

  i = 0;
  forall_nodes(v,G) 
  { point p = get_position(v);
    xpos[v] = p.xcoord();
    ypos[v] = p.ycoord();
    if (i < n) set_position(v,old_pos[i++]);
   }

  i = 0;
  forall_edges(e,G) 
  { list<point> L = get_bends(e);
    point p;
    forall(p,L)
    { xbends[e].append(p.xcoord());
      ybends[e].append(p.ycoord());
     }
    if (i < m) set_bends(e,old_bends[i++]);
   }

  redraw();

  delete[] old_pos;
  delete[] old_bends;

/*
  double xmin,ymin,xmax,ymax,r1,r2;
  get_extreme_coords(xmin,ymin,xmax,ymax,r1,r2);

  double d = win_p->pix_to_real(40);

  double wx0 = get_xmin() + r1 + d;
  double wy1 = get_ymin() + r1 + d;
  double wx0 = get_xmax() - r2 + d;
  double wy1 = get_ymax() - r2 + d;

  double dx,dy,fx,fy;
  fill_win_params(wx0,wy0,wx1,wy1,xmin,ymin,xmax,ymax,dx,dy,fx,fy);

  adjust_coords_to_win(xpos,ypos,xbends,ybends,wx0,wy0,wx1,wy1);

  forall_nodes(v,G)
  { set_radius1(v,fx*get_radius1(v));
    set_radius2(v,fy*get_radius2(v));
   }
*/

  set_flush(fl);
  adjust_coords_to_win(xpos,ypos,xbends,ybends);
  set_layout(xpos,ypos,xbends,ybends,false);
  //zoom_graph();
  if (!fl) redraw();

  return 0;
}



int GraphWin::read(istream& in, bool clear)
{
  if (!check_init_graph_handler()) return 0;

  bool old_flush = set_flush(false);

  //if (gw_undo) gw_undo->start_complex_command();

  if (gw_undo)
  { delete gw_undo;
    gw_undo = 0;
   }

  graph save_g;
  if (gr_p->is_undirected()) save_g.make_undirected();

  list<node> V0;
  map<node,node_info> ninf_save;
  node v;

  list<edge> E0;
  map<edge,edge_info> einf_save;
  edge e;
  double old_x0,old_y0,old_x1,old_y1;

  if (!clear)
  { forall_nodes(v,*gr_p) ninf_save[v] = n_info[v];
    forall_edges(e,*gr_p) einf_save[e] = e_info[e];
    V0 = gr_p->all_nodes();
    E0 = gr_p->all_edges();
    get_bounding_box(old_x0,old_y0,old_x1,old_y1);
  }

  save_g.join(*gr_p);

  int err = gr_p->read(in);

  // if (gw_undo) gw_undo->finish_complex_command(false);

  if (err == 3)  // no LEDA-graph; restore saved graph
  { if (gw_undo) {
      gw_undo->undo();
      gw_undo->redo_clear();
    }
    else {
      gr_p->clear();
      gr_p->join(save_g);
    }
    redraw();
    set_flush(old_flush);
    return 2;
  }

  edges_embedded=false;

  list<node> V1 = gr_p->all_nodes();
  list<edge> E1 = gr_p->all_edges();

  if (clear) 
  { set_default_win_coords();
    init_graph();
    //zoom(0);
  }
  else
    { gr_p->join(save_g);
      init_graph();
      forall(v,V0) n_info[v] = ninf_save[v];
      forall(e,E0) e_info[e] = einf_save[e];
     }


  int result;

  result = read_layout(in,V1);

/*
  if (result == 5) // no layout
  { //gw_random_embed(*this);
    if (get_window().is_open()) center_graph();
  }
*/


  if (result == 5) // no layout
  { set_flush(old_flush);
    call_init_graph_handler();
    return result;
  }


  if (!clear)
  { double x0,y0,x1,y1;
    get_bounding_box(V1,E1,x0,y0,x1,y1);
    double dx,dy;
    if (old_x1 - old_x0 > old_y1 - old_y0)
     { dx = ((old_x1 - old_x0) - (x1 - x0))/2;
       dy = (old_y1 - y0) + get_window().pix_to_real(50);
      }
    else
     { dx = (old_x1 - x0) + get_window().pix_to_real(50);
       dy = ((old_y1 - old_y0) - (y1 - y0))/2;
      }
    bool save_flush = set_flush(true);
    set_select(V1,true);
    move_nodes_with_edges(V1,vector(dx,dy));
    //zoom_graph();
    set_select(V1,false);
    set_flush(save_flush);
   }

  set_flush(old_flush);

  zoom_graph();

  call_init_graph_handler();

  return result;
}


int GraphWin::read_dimacs(istream& in)
{
  if (!check_init_graph_handler()) return 0;

  bool old_flush = set_flush(false);

  // read dimacs graph
  graph& G = get_graph();
  node_array<string> n_label(G);
  edge_array<string> e_label(G);
  list<node> L;
  string problem = Read_Dimacs_Graph(in,G,L,n_label,e_label);

  // reset default attributes
  init_default();
  set_node_label_type(gw_label_type(user_label | index_label));
  set_node_label_pos(north_pos);

  init_graph();

  load_finish(0,0,0,0);

  if (problem == "sp")
      message("\\bf Found Dimacs Shortest Paths Problem");
  if (problem == "max")
      message("\\bf Found Dimacs Max Flow Problem");
  if (problem == "min")
      message("\\bf Found Dimacs Min Cost Flow Problem");

  node v;
  forall(v,L) set_user_label(v,n_label[v]);

  edge e;
  forall_edges(e,G) set_label(e,e_label[e]);

/*
  G.set_node_position(L.head(),nil);
  G.set_node_position(L.tail(),G.last_node());
*/

  redraw();
  set_flush(old_flush);

  call_init_graph_handler();
  return 0;
}


int GraphWin::read_dimacs(string fname, bool ask_override)
{ ifstream in(fname);
  if(in.fail()) return 1;             

  if (ask_override && !gr_p->empty() && !confirm_loading(win_p)) return 0;

  //message("\\bf Reading Dimacs File\\rm\\blue ~~" + fname);
  message("\\bf " + fname);
  int x = read_dimacs(in);

  leda_wait(1);
  message("");
  return x;
}


bool GraphWin::save_dimacs(ostream& out)
{ if (out.fail()) return false;
  graph& G = get_graph();
  node s = G.first_node();
  node t = G.last_node();
  edge_array<int> cap(G);
  edge e;
  forall_edges(e,G) cap[e] = atoi(get_user_label(e));
  Write_Dimacs_MF(out,G,s,t,cap);
  return true;
}


bool GraphWin::save_dimacs(string fname, bool ask_override) 
{
  if (is_file(fname) && ask_override 
                     && !confirm_overwrite(fname,win_p)) return true;
  ofstream out(fname);
  return save_dimacs(out);
}




int GraphWin::drop_file(string fname, double x, double y)
{

  ifstream in(fname);

  bool old_flush = set_flush(false);

  graph save_g;
  if (gr_p->is_undirected()) save_g.make_undirected();

  if (gw_undo) gw_undo->start_complex_command();
  save_g.join(*gr_p);
  int err = gr_p->read(in);
  if (gw_undo) gw_undo->finish_complex_command(false);

  if (err == 3) 
  { // no LEDA-graph; restore saved graph
    if (gw_undo) {
      gw_undo->undo();
      gw_undo->redo_clear();
    }
    else {
      gr_p->clear();
      gr_p->join(save_g);
    }
    redraw();
    set_flush(old_flush);
    return 2;
  }

  edges_embedded=false;
  
  init_graph();

  int result = read_layout(in,gr_p->all_nodes(),-1,x,-1,y);

  set_flush(true);

  zoom_graph();

  set_flush(old_flush);

  return result;
}


/*
int GraphWin::read(istream& in, bool clr)
{
  graph G;

  if (gr_p->is_undirected()) G.make_undirected();

  if (G.read(in) == 3) 
  { redraw();
    return 2;
  }

  list<node> V = G.all_nodes();

  bool old_flush=set_flush(false);

  if (gw_undo) gw_undo->start_complex_command();
  if (clr) gr_p->clear();
  gr_p->join(G);
  if (gw_undo) gw_undo->finish_complex_command(false);

  edges_embedded=false;
  
  int result = read_layout(in,V);
  init_matrix();

  set_flush(old_flush);
  return result;
}
*/


int GraphWin::read(string fname, bool clr, bool ask_override)
{ ifstream in(fname);

  if (in.fail())  // cannot open file
  { if (get_flush()) 
    { embed_edges();
      redraw();
     }
    message("");
    return 1;             
  }

  if (ask_override && !gr_p->empty()) 
  { int a = load_or_join(win_p); 
    if (a == 0) return 0;
    clr = (a == 1);
   }


  //message("\\bf Reading GW-File\\rm\\blue ~~"+fname);
  message("\\bf " + fname);

  int s = read(in,clr);

  leda_wait(1);
  message("");

  return s;
 }


//----------------------------------------------------------------------------
// GML input
//----------------------------------------------------------------------------

int GraphWin::read_gml(istream& in)
{
  if (!check_init_graph_handler()) return 0;

  int result = 0;

  bool gml_flip = false;

  if (gw_undo) delete gw_undo;

  gml_gwgraph parser(this,in,gml_flip);

  gw_undo = new graphwin_undo(*this);

  if (parser.errors())
  { clear_graph();
    result = 2;
   }

  flip_y();

  if (result == 0) load_finish(0,0,0,0);

  call_init_graph_handler();

  return result;
}



int GraphWin::read_gml_string(string in_str)
{
  if (!check_init_graph_handler()) return 0;

  int result = 0;

  if (gw_undo) delete gw_undo;

  gml_gwgraph parser(this);
  parser.parse_string(in_str);

  gw_undo = new graphwin_undo(*this);

  if (parser.errors())
  { clear_graph();
    result = 2;
   }

  flip_y();

  if (result == 0) load_finish(0,0,0,0);

  call_init_graph_handler();

  return result;
}


int GraphWin::read_gml(string fname, bool ask_override)
{
  ifstream in(fname);

  // check whether file exists and can be read
  if(in.fail())  
  { bool f = set_flush(false);
    if (f) 
    { embed_edges();
      redraw();
    }
    set_flush(f);
    return 1;             
  }

  if (ask_override && !gr_p->empty() && !confirm_loading(win_p)) return 0;

  //message("\\bf Reading GML-File\\rm\\blue ~~" + fname);
  message("\\bf " + fname);

  int s = read_gml(in);

  leda_wait(1);
  message("");

  return s;
}



//----------------------------------------------------------------------------

enum { LOAD, UPLOAD, SAVE, SAVE_MF,
       LOAD_GW, SAVE_GW, LOAD_GML, SAVE_GML, LOAD_DIMACS_MF, SAVE_DIMACS_MF,
       SAVE_PS, SAVE_SVG, SAVE_TEX, SAVE_SCREEN, FILE_ALL, FILE_CANCEL };


static GraphWin* gwp;


static string base_name(string fname, string suffix)
{ string s = fname.del(suffix);
  int len = s.length();
  int i = len-1;
  while (i >= 0 && s[i] != '/' && s[i] != '\\') i--;
  return s(i+1,len-1);
}

static void no_layout_diag(GraphWin& gw, string fname) 
{ int direction = 0;
  panel P("GraphWin");
  P.text_item("");
  P.text_item("\\bf No layout information in\\n");
  P.text_item("\\tt\\blue " + fname);
  P.text_item("");
  P.text_item("Please choose an initial drawing");
  P.text_item("");
  P.choice_item("Edges", direction,"undirected","directed");

  P.buttons_per_line(4);

  P.fbutton("spring",0);
  P.button("random",1);
  P.button("circle",2);
  P.button("center",3);

  int but = gw.open_panel(P);

  if (direction == 0)
    gw.set_edge_direction(undirected_edge);
  else
    gw.set_edge_direction(directed_edge);

  switch (but) {
  case 0: gw_spring_embed(gw,true);
          break;
  case 1: gw_random_embed(gw);
          break;
  case 2: gw_circle_embed(gw);
          break;
  case 3: gw.center_graph();
          break;
  }
}


void GraphWin::load_gw_graph(string filename)
{
  string dir = get_directory();

  switch (read(filename,true,true)) {
   case 1 : acknowledge("No such file",            filename);
            break;
   case 2 : acknowledge("Error in GW format",      filename);
            break;
   case 3 : acknowledge("Invalid node attributes", filename);
            break;
   case 4 : acknowledge("Invalid edge attributes", filename);
            break;
   case 5 : //acknowledge("No Layout Information", filename);
            no_layout_diag(*this, filename);
            break;
   default: undo_clear();
            break;
  }
  set_graphname(base_name(filename,".gw"));
}
  

void GraphWin::load_gml_graph(string filename)
{
  string dir = get_directory();

  switch (read_gml(filename,true)) {
   case 1 : acknowledge("No such file", filename);
            break;
   case 2 : acknowledge("GML parse error", filename);
            break;
   default: undo_clear();
            break;
  }
  set_graphname(base_name(filename,".gml"));
}


void GraphWin::load_dimacs_graph(string filename)
{ 
  string dir = get_directory();

  switch (read_dimacs(filename,true)) {
   case 1 : acknowledge("No such file", filename);
            break;
   default: undo_clear();
            no_layout_diag(*this, filename);
            break;
  }
  set_graphname(base_name(filename,".dimacs"));
}


void GraphWin::save_gw_graph(string filename)
{ if (!save_gw(filename,true)) 
    acknowledge("Cannot write", filename);
  else
    undo_clear();
  set_graphname(base_name(filename,".gw"));
}
      
void GraphWin::save_gml_graph(string filename)
{ if (!save_gml(filename,true)) 
    acknowledge("Cannot write", filename);
  else
    undo_clear();
  set_graphname(base_name(filename,".gml"));
}

void GraphWin::save_dimacs_graph(string filename)
{ if (!gwp->save_dimacs(filename,true))
    acknowledge("Cannot write", filename);
  else
    undo_clear();
  set_graphname(base_name(filename,".dimacs"));
}

void GraphWin::save_graph(string fname)
{ if (fname.ends_with(".gw")) save_gw_graph(fname);
  else
  if (fname.ends_with(".gml")) save_gml_graph(fname);
  else
  if (fname.ends_with(".dimacs")) save_dimacs_graph(fname);
  else
    acknowledge("save_graph: unknown file type", fname);
}



void GraphWin::load_graph(string fname)
{ if (fname.ends_with(".gw")) load_gw_graph(fname);
  else
  if (fname.ends_with(".gml")) load_gml_graph(fname);
  else
  if (fname.ends_with(".dimacs")) load_dimacs_graph(fname);
  else
    acknowledge("load_graph: unknown file type", fname);
}

static void save_graph_func(string fname) { gwp->save_graph(fname); }
static void load_graph_func(string fname) { gwp->load_graph(fname); }

  


void save_metafile(string fname)
{ gwp->save_wmf(fname,true); }


static void save_ps_file(string filename)
{ gwp->save_ps(filename,true);
//gwp->set_ps_filename(filename);
}

static void save_svg_file(string filename)
{ gwp->save_svg(filename,true); }
      

//static const char* slider_format = "! Size  %4.1f cm x %4.1f cm";
static const char* slider_format = "!%4.1f cm x %4.1f cm";
static double ps_max_w;
static double ps_max_h;
static double ps_cur_w;
static double ps_cur_h;
static point ps_cap_pos;

//static string ps_path = "."; 
static string ps_path = ""; 
 

static void save_latex_file(string fname)
{ gwp->save_latex(fname,ps_path,0,0,0,0,ps_cur_w,true);
  //gwp->set_ps_filename(fname);
}


static bool s_full_color = true;

static void save_screenshot(string filename) // postscript
{ gwp->redraw();
  gwp->get_window().screenshot(filename,s_full_color); 
 }



static void slider_func(int p)
{ window* wp = window::get_call_window();
  panel_item it = window::get_call_item();
  ps_cur_w = ps_max_w*p/100;
  ps_cur_h = ps_max_h*p/100;
  string label(slider_format,ps_cur_w,ps_cur_h);
  wp->set_item_label(it,label);
  gwp->message(string("width = %.1f cm  height = %.1f cm",ps_cur_w,ps_cur_h));
}


void GraphWin::file_handler(int what)
{
  window& W=get_window();

  gwp = this; 

  string fname = get_graphname();
  string dname = get_dirname();

  if (what==SAVE_GW || what==SAVE_GML || what==SAVE_TEX || what==SAVE_PS ||
      what == SAVE_SVG || what==SAVE_MF)
  { if (get_graph().empty())
    { message("\\bf Empty Graph.");
      leda_wait(1.5);
      message("");
      return;
     }
   }


  switch (what) {

  case SAVE:     break;

  case LOAD:     fname = "";
                 break;

  case SAVE_GW:
  case LOAD_GW:  fname += ".gw";
                 break;

  case SAVE_GML:
  case LOAD_GML: fname += ".gml";
                 break;

  case SAVE_PS:  fname += ".ps";
                 break;

  case SAVE_SVG: fname += ".svg";
                 break;

  case SAVE_MF:  fname += ".wmf";
                 break;

  case SAVE_TEX: fname += ".tex";
                 break;

  case SAVE_SCREEN:
#if defined(__win32__)
     fname = "gw_screenshot.wmf";
#else
     fname = "gw_screenshot.ps";
#endif
                 break;

  case LOAD_DIMACS_MF:
  case SAVE_DIMACS_MF: fname += ".dimacs";
                       break;

  }

//cout << "what = " << what << "  fname = " << fname << endl;


  if (window::display_type() == "xx" && what != LOAD)
  {
    if (what == UPLOAD) {
      message("\\bf\\blue Upload in progress  ...");
      window::upload(dname);
      return;
    }

    if (! is_directory("tmp"))
    { string path = get_directory() + "/tmp";
      LEDA_EXCEPTION(0, string("create tmp: ") + path);
      create_directory("tmp");
    }


    string tmp = "tmp/" + fname;

    switch (what) {
     case SAVE_GW:  save_gw(tmp,false);
                    break;
     case SAVE_GML: save_gml(tmp,false);
                    break;
     case SAVE_SVG: save_svg(tmp,false);
                    break;
     case SAVE_PS:  save_ps(tmp,false);
                    break;
    }

    window::download(tmp); 
    return;
  }


  file_panel P(fname,dname);

  switch(what) {

/*
  case LOAD_GW:  P.set_load_handler(load_gw_graph);
                 P.set_load_string("Load Graph from GW File");
                 break;

  case SAVE_GW:  P.set_save_handler(save_gw_graph);
                 P.set_save_string("Save Graph to GW File");
                 break;

  case LOAD_GML: P.set_load_handler(load_gml_graph);
                 P.set_load_string("Load Graph from GML File");
                 break;

  case SAVE_GML: P.set_save_handler(save_gml_graph);
                 P.set_save_string("Save Graph to GML File");
                 break;

  case LOAD_DIMACS_MF: 
                 P.set_load_handler(load_dimacs_graph);
                 P.set_load_string("Load DIMACS Maxflow Problem.");
                 break;

  case SAVE_DIMACS_MF: 
                 P.set_save_handler(save_dimacs_graph);
                 P.set_save_string("Save DIMACS Maxflow Problem.");
                 break;
*/


  case LOAD:  
              P.set_load_handler(load_graph_func);
              P.set_load_string("Load Graph");
              break;

  case SAVE:  P.set_save_handler(save_graph_func);
              P.set_save_string("Save Graph");
              break;


  case SAVE_MF:  P.set_save_handler(save_metafile);
                 P.set_save_string("Write Windows Metafile");
                 break;

  case SAVE_PS:  P.set_save_handler(save_ps_file);
                 P.set_save_string("Write Postscript File");
                 break;

  case SAVE_SVG: P.set_save_handler(save_svg_file);
                 P.set_save_string("Write SVG File");
                 break;

  case SAVE_TEX: P.set_save_handler(save_latex_file);
                 P.set_save_string("Write LaTeX/PS Files");
                 break;

  case SAVE_SCREEN: 
                 P.set_save_handler(save_screenshot);
#if defined(__win32__)
                 P.set_save_string("Write Metafile Screenshot");
#else
                 P.set_save_string("Write Postscript Screenshot");
#endif
                 break;
  }


  //string fontname = get_ps_fontname();

  int p = 100;

  //int dtype = 0;

  if ( what == SAVE_PS)
    { P.set_pattern("PS Files","*.ps");
      window& pan = P.get_window();
      P.init_panel();
      P.set_mswin(false);
      //pan.text_item("");
      //pan.text_item("\\bf\\blue Options");
      //pan.text_item("");
      //pan.string_item("Font", fontname,get_ps_fontlist(),4);
      pan.text_item("");
      panel_item it = pan.bool_item(" draw grid",ps_draw_grid);
      if (get_grid_dist() == 0) pan.disable_item(it);
     }
  else
   if ( what == SAVE_SVG)
      P.set_pattern("SVG Files","*.svg");
    else
     if (what == SAVE_TEX) 
      { double x0,y0,x1,y1;
        get_bounding_box(x0,y0,x1,y1);

        ps_cur_w = (x1 - x0)/PSPixelPerCM;
        ps_cur_h = (y1 - y0)/PSPixelPerCM;

        double d = W.pix_to_real(20);

        if ( (x1-x0+2*d) > (get_xmax() - get_xmin()) ||
             (y1-y0+2*d) > (get_ymax() - get_ymin()) )
           zoom_graph();
        else 
        //  if (x0-d < get_xmin() || x1+d > get_xmax() ||
        //      y0-d < get_ymin() || y1+d > get_ymax() )
           center_graph();

        ps_max_w = ps_max_h = 20.0;

        if (ps_cur_w > 0) ps_max_h = ps_max_w * ps_cur_h/ps_cur_w;

        p = int(100*ps_cur_w/ps_max_w);

 
        W.start_buffering();
        W.draw_box(x0,y0,x1,y1,grey1);
        draw_shapes();
        draw_graph();
        W.flush_buffer(x0,y0,x1,y1);
        W.stop_buffering();

        string str("w = %.2f cm   h = %.2f cm",ps_max_w*p/100,ps_max_h*p/100);

        W.set_font("T18");
        double th = W.text_height(str);
        ps_cap_pos = point((x0+x1)/2,y0-0.75*th);

        text_mode tm = W.set_text_mode(opaque);
        W.draw_ctext(ps_cap_pos,str,black);
        W.set_text_mode(tm);

        P.set_pattern("LaTeX Files","*.tex");

      // panel for latex/ps options 

        ps_path = dname;

        panel pan("PS/LaTex Settings");

        pan.text_item("");

        pan.string_item("ps path",ps_path);

        pan.text_item("Image Size");
        pan.int_item(string(slider_format,ps_max_w*p/100,ps_max_h*p/100),
                     p,1,100,slider_func);        
                     
        //n_xfac = node_font_size < 30 ? 0.05/5.0 * node_font_size + 0.48 : 0.8;    
        pan.text_item("");
        pan.bool_item(" Adjust node label size",adj_node_label_font);
        pan.double_item(" x - offset factor",n_xfac);
        pan.double_item(" y - offset factor",n_yfac);                     

        pan.text_item("");
        pan.bool_item(" Adjust edge label size",adj_edge_label_font);
        pan.double_item(" x - offset factor",e_xfac);                     
        pan.double_item(" y - offset factor",e_yfac);                     
        
        if (get_grid_dist() != 0) pan.bool_item("draw grid",ps_draw_grid);

       pan.button("continue");
       //pan.open(*win_p);

       int wx =  W.xpos();
       int wy =  W.ypos();
       pan.open(-wx,wy);
       }
     else
       if (what == SAVE_SCREEN || what == SAVE_MF)
          { 
#if defined(__win32__)
            P.set_pattern("Meta Files","*.wmf");
#else
            P.set_pattern("PS Files","*.ps");
#endif

            window& pan = P.get_window();
            P.init_panel();
            P.set_mswin(false);
            pan.text_item("");
            pan.bool_item(" full color",s_full_color);
           }
        else
           if (what == SAVE)
           { //P.set_pattern("GW Files",  "*.gw");
             P.add_pattern("GW Files",  "*.gw");
             P.add_pattern("GML Files", "*.gml");
             P.add_pattern("DIMACS",    "*.dimacs");
            }
           else
           if (what == LOAD)
           { //P.set_pattern("All Files", "*.*");
             P.add_pattern("All Files", "*.*");
             P.add_pattern("GW Files",  "*.gw");
             P.add_pattern("GML Files", "*.gml");
             P.add_pattern("DIMACS",    "*.dimacs");
            }
           else
           if (what == SAVE_GML || what == LOAD_GML)
               P.set_pattern("GML Files","*.gml");
           else
             if (what == SAVE_GW  || what == LOAD_GW)
               P.set_pattern("GW Files","*.gw");
             else
               if (what == SAVE_DIMACS_MF  || what == LOAD_DIMACS_MF)
               { P.set_pattern("Dimacs Files","*.dimacs");
/*
                 window& pan = P.get_window();
                 P.init_panel();
                 P.set_mswin(false);
                 panel_item it = pan.choice_item("type",dtype,"max-flow",
                                                              "mc-flow",
                                                              "matching");
                 pan.disable_item(it);
*/
                }


  if (what != SAVE_SCREEN) W.disable_panel();

  if (what == SAVE_TEX)
   { P.set_frame_label("Export LaTeX/PS");
     double dx = (get_xmax() - get_xmin() - ps_cur_w*PSPixelPerCM)/2;
     double dy = (get_ymax() - get_ymin() - ps_cur_h*PSPixelPerCM)/2;
     int wx =  W.xpos() + W.real_to_pix(0.75*dx);
     int wy =  W.ypos() + W.real_to_pix(0.75*dy);
     //P.x_open();
     //P.open();
     P.open(-wx,wy);
    }
  else
  { P.open();
    //P.x_open();
   }

  W.enable_panel();

  redraw();

  set_dirname(dname);

// set_flush(old_flush);
}

void gw_upload_handler(GraphWin& gw)     { gw.file_handler(UPLOAD);  }

void gw_load_handler(GraphWin& gw)       { gw.file_handler(LOAD);  }
void gw_save_handler(GraphWin& gw)       { gw.file_handler(SAVE);  }
void gw_save_gw_handler(GraphWin& gw)    { gw.file_handler(SAVE_GW);  }
void gw_save_gml_handler(GraphWin& gw)   { gw.file_handler(SAVE_GML);  }

void gw_save_ps_handler(GraphWin& gw)    { gw.file_handler(SAVE_PS);  }
void gw_save_svg_handler(GraphWin& gw)   { gw.zoom_graph(); gw.file_handler(SAVE_SVG);  }
void gw_save_tex_handler(GraphWin& gw)   { gw.file_handler(SAVE_TEX);  }
void gw_screenshot_handler(GraphWin& gw) { gw.file_handler(SAVE_SCREEN); }
void gw_file_handler(GraphWin& gw)       { gw.file_handler(FILE_ALL); }
void gw_metafile_handler(GraphWin& gw)   { gw.file_handler(SAVE_MF);  }



void gw_clipboard_handler(GraphWin& gw)
{ window& W = gw.get_window();
  graph&  G = gw.get_graph();

  if (G.empty())
  { gw.message("\\bf\\red Empty Graph.");
    leda_wait(1);
    gw.message("");
    return;
   }

  int format = 0;

  panel P;
  P.text_item("");
  P.text_item("\\bf\\blue Copy Graph to Clipboard");
  P.text_item("");
  P.choice_item("",format,"windows metafile","windows bitmap");

  P.button("copy",  0);
  P.button("cancel",1);

  if (gw.open_panel(P) == 1) return;

  gw.redraw();

  switch (format) {

  case 0: { gwp = &gw;
            save_metafile("");
            W.metafile_to_clipboard();
            break;
           }

  case 1: { double x0,y0,x1,y1;
            gw.get_bounding_box(x0,y0,x1,y1);
            W.pixrect_to_clipboard(W.get_pixrect(x0,y0,x1,y1));
            break;
           }
  }

}



void gw_print_ps_handler(GraphWin& gw)
{ 
  string fname = tmp_file_name();

  string lpr_cmd = gw.get_print_cmd();
  string del_cmd = "rm -f";

  fname += ".ps";

  list<string> commands;
  commands.append("lpr -h");
  commands.append("hp3pr");
  commands.append("lp -o nobanner");
  commands.append("gv");
  commands.append("ghostview");
  commands.append("xterm -e less");

  string fontname = gw.get_ps_fontname();

  // ps_factor:    0     -1        -2
  //             window  graph  no zooming (1:1)


  int zoom_mode = int(-gw.ps_factor);

  panel pr_panel;
  pr_panel.text_item("");
  pr_panel.text_item("\\bf\\blue Print PostScript");
  pr_panel.text_item("");
  pr_panel.text_item(fname);
  pr_panel.text_item("");
  pr_panel.string_item("cmd",lpr_cmd,commands,commands.length());
  //pr_panel.string_item("font", fontname,gw.get_ps_fontlist(),4);
  pr_panel.choice_item("zoom",zoom_mode,"window","graph","1:1");

  panel_item it = pr_panel.bool_item("draw grid",ps_draw_grid);
  if (gw.get_grid_dist() == 0) pr_panel.disable_item(it);

  pr_panel.fbutton("print",0);
  pr_panel.button("cancel",1);

  if (gw.open_panel(pr_panel) == 0)
  { gw.set_ps_fontname(fontname);
    gw.ps_factor = -zoom_mode;
    gw.save_ps(fname);
    gw.redraw();
    if (system(lpr_cmd + " " + fname) & 0xFF) {
		cerr << "gw_print_ps_handler: running '" << lpr_cmd << " " << fname << "' failed" << endl;
	} else if (system(del_cmd + " " + fname) & 0xFF) {
		cerr << "gw_print_ps_handler: running '" << del_cmd << " " << fname << "' failed" << endl;
	}
   }

  gw.set_print_cmd(lpr_cmd);
}


// defaults

bool GraphWin::save_defaults(string fname)
{ ofstream out(fname);
  if (out.fail()) return false;
  out << n_model << endl;
  out << e_model << endl;

  // global options

 out << "zoom_objects  " << zoom_objects       << endl;
 out << "zoom_factor   " << zoom_factor        << endl;
 out << "adjust_size   " << adjust_to_label    << endl;
 out << "create_target " << auto_create_target << endl;
 out << "edge_border   " << edge_border        << endl;
 out << "show_status   " << show_status        << endl;
 out << "grid_style    " << (int)g_style       << endl;
 out << "flush         " << flush              << endl;

 out << "window_xpos   " << win_p->xpos()    << endl;
 out << "window_ypos   " << win_p->ypos()    << endl;
 out << "window_width  " << win_p->width()   << endl;
 out << "window_height " << win_p->height()  << endl;

  return true;
 }

bool GraphWin::read_defaults(string fname)
{ ifstream in(fname);

  if (in.good()) 
  { node_info ni;
    read_node_info(in,ni,FileFormatVersion);
    //set_node_info(ni,N_ALL & ~N_WIDTH & ~N_HEIGHT);
    set_node_info(ni,N_ALL);
   }

  if (in.good())
  { edge_info ei;
    read_edge_info(in,ei,FileFormatVersion);
    set_edge_info(ei,E_ALL);
   }

  if(in.fail()) 
  { init_default();
    return false;
   }

  string key;
  double val;
  while (in >> key >> val)
  { if (key == "zoom_objects")  zoom_objects       = (val != 0);
    if (key == "zoom_factor")   zoom_factor        =  val;
    if (key == "adjust_size")   adjust_to_label    = (val != 0);
    if (key == "create_target") auto_create_target = (val != 0);
    if (key == "edge_border")   edge_border        = (val != 0);
    if (key == "show_status")   show_status        = (val != 0);
    if (key == "grid_style")    set_grid_style(grid_style(int(val)));
    if (key == "flush")         flush              = (val != 0);
   }
  return in.good() != 0;
 }



bool GraphWin::save_defaults()
{ char* home = getenv("HOME");
  string fname;
  if (home) fname = string("%s/",home);
  return save_defaults(fname + ".graphwinrc");
}

bool GraphWin::read_defaults()
{ char* home = getenv("HOME");
  string fname;
  if (home) fname = string("%s/",home);
  return read_defaults(fname + ".graphwinrc");
}

LEDA_END_NAMESPACE
