/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  draw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>
#include <ctype.h>

#if defined(_MSC_VER)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#endif

LEDA_BEGIN_NAMESPACE

/*
static unsigned char stip_bits[] = {
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa,
0x55, 0x55, 0xaa, 0xaa };
*/


static unsigned char hatch_bits1[] = {
0x88, 0x88, 0x44, 0x44,
0x22, 0x22, 0x11, 0x11,
0x88, 0x88, 0x44, 0x44,
0x22, 0x22, 0x11, 0x11,
0x88, 0x88, 0x44, 0x44,
0x22, 0x22, 0x11, 0x11,
0x88, 0x88, 0x44, 0x44,
0x22, 0x22, 0x11, 0x11 };

static unsigned char hatch_bits2[] = {
0x11, 0x11, 0x22, 0x22,
0x44, 0x44, 0x88, 0x88,
0x11, 0x11, 0x22, 0x22,
0x44, 0x44, 0x88, 0x88,
0x11, 0x11, 0x22, 0x22,
0x44, 0x44, 0x88, 0x88,
0x11, 0x11, 0x22, 0x22,
0x44, 0x44, 0x88, 0x88 };

static unsigned char hatch_bits3[] = {
0x80, 0x80, 0x41, 0x41,
0x22, 0x22, 0x14, 0x14,
0x08, 0x08, 0x14, 0x14,
0x22, 0x22, 0x41, 0x41,
0x80, 0x80, 0x41, 0x41,
0x22, 0x22, 0x14, 0x14,
0x08, 0x08, 0x14, 0x14,
0x22, 0x22, 0x41, 0x41 };


/*
inline color light_color(color col)
{ int r,g,b;
  col.get_rgb(r,g,b);
  int x = 256-(r+g+b)/3;
  color c(r+x,g+x,b+x);
  if (!c.is_good()) c = grey1;
  return c;
}

inline color dark_color(color col)
{ int r,g,b;
  col.get_rgb(r,g,b);
  color c(r/2,g/2,b/2);
  if (!c.is_good()) c = grey3;
  return c;
}
*/


//----------------------------------------------------------------------------

void GraphWin::draw_node(node v)
{
  draw_node(v,0,0);
 }

void GraphWin::redraw_node(node v)
{ window& W = get_window();
  W.start_buffering();
  draw_node(v);
  flush_node(v);
  W.stop_buffering();
}

void GraphWin::clear_node_box(node v)
{ node_info& v_inf = n_info[v];
  double sel_w = v_inf.border_w + win_p->pix_to_real(2);
  double r1 = v_inf.r1 + sel_w;
  double r2 = v_inf.r2 + sel_w;
  double x  = v_inf.pos.xcoord();
  double y  = v_inf.pos.ycoord();
  double x0 = x - r1;
  double y0 = y - r2;
  double x1 = x + r1;
  double y1 = y + r2;
  win_p->clear(x0,y0,x1,y1);
}


void GraphWin::split_label(node v,edge e,string label, list<string>& L,
                           double& x0, double& y0, double& x1, double& y1,
                           double& yshift)
{
  int ind;
  gw_font_type fn;
  double fn_size;
  string fn_user;

  if (v)
   { ind =  gr_p->index(v);
     fn = node_label_font_type;
     fn_size = node_label_font_size;
     fn_user = node_label_user_font;
    }
  else
   { ind = gr_p->index(e);
     fn  =  edge_label_font_type;
     fn_size = edge_label_font_size;
     fn_user = edge_label_user_font;
    }

  set_font(fn,fn_size,fn_user);

  label = label.replace_all("%d",string("%d",ind));
  label = label.replace_all("%D",string("%d",ind+1));
  label = label.replace_all("%c",string("%c",ind+'a'));
  label = label.replace_all("%C",string("%c",ind+'A'));


  if (label[0] == '$') 
  { fn = italic_font;
    label = label.del(0);
    int l = label.length();
    if (label[l-1] == '$') label = label.del(l-1);
   }

  double th0 = win_p->text_height(label);

  // split label

  L.clear();
  while (label != "")
  { 
    if (label[0] == '\n')
    { L.append("\n");
      int k = 1;
      while (label[k] == ' ') k++;
      label = label.del(0,k-1);
      continue;
     }

    if (label[0] == '\\' && label.length() > 1 && label[1] == 'n')
    { L.append("\n");
      int k = 2;
      while (label[k] == ' ') k++;
      label = label.del(0,k-1);
      continue;
     }

    if (label[0] == '_' || label[0] == '^')
    { int j = 2;
      if (label[1] == '{')
      { label = label.del(1);
        int k = label.index("}");
        if (k >= 0) 
        { label = label.del(k);
          j = k;
         }
       }
      L.append(label(0,j-1));
      label = label.del(0,j-1);
      continue;
     }

    int i  = label.index("_",1);
    int i2 = label.index("^",1);
    int i3 = label.index("\n",1);
    int i4 = label.index("\\",1);

    if (i  == -1) i  = label.length();
    if (i2 == -1) i2 = label.length();
    if (i3 == -1) i3 = label.length();
    if (i4 == -1 || label[i4+1] != 'n') i4 = label.length();

    if (i2 < i) i = i2;
    if (i3 < i) i = i3;
    if (i4 < i) i = i4;

    L.append(label(0,i-1));
    label = label.del(0,i-1);
  }

  // compute width and height of text box

  yshift = 0.6 * win_p->text_height("H");

  double  d = fn_size/16;

  double tw0  = 0;

  double th = th0;
  double tw = tw0;

  list_item it;
  forall_items(it,L)
  { string s = L[it];
    list_item it1 = L.succ(it);
    char c = s[0];

    if (c == '\n')
    { if (tw0 > tw) tw = tw0;
      tw0 = 0;
      th0 = th + fn_size;
      th = th0;
      continue;
     }

    if (c == '_' || c == '^')
     { set_font(fn,fn_size/2,fn_user);
       s = s.del(0);
       if (c == '_')
          { if (it1== nil || L[it1][0] != '^')
               tw0 += win_p->text_width(s)+d;
            th = th0 + fn_size/6;
           }
       else
          { if (it1== nil || L[it1][0] != '_')
               tw0 += win_p->text_width(s)+d;
            th = th0 + fn_size/6;
           }
      }
    else
     { set_font(fn,fn_size,fn_user);
       tw0 += win_p->text_width(s);
      }
   }

   if (tw0 > tw) tw = tw0;

  point pos;
  if (v)
    pos = compute_label_pos(v,tw,th);
  else
    pos = compute_label_pos(e,tw,th);

  x0 = pos.xcoord();
  y0 = pos.ycoord();
  x1 = x0 + tw;
  y1 = y0 + th;
}



void GraphWin::draw_obj_label(node v,edge e,string label,color label_clr,
                                                         double dx,double dy)
{
  int clr;
  //int label_clr;
  gw_font_type fn;
  double fn_size;
  string fn_user;

  if (v)
    { clr = get_color(v);
/*
      label_clr = get_label_color(v);
      if (label_clr == gw_auto_color) 
      { if (get_label_pos(v) == central_pos)
           label_clr = text_color(clr);
        else
           label_clr = black;
       }
*/
      fn = node_label_font_type;
      fn_size = node_label_font_size;
      fn_user = node_label_user_font;
     }
  else
    { clr = get_color(e);
/*
      label_clr = get_label_color(e);
      if (label_clr == gw_auto_color)  label_clr = black;
*/
      fn = edge_label_font_type;
      fn_size = edge_label_font_size;
      fn_user = edge_label_user_font;
     }

  bool simple_label = true;

  for(int i=0; i<label.length(); i++)
  { char c = label[i];
    if ( c == '$' || c == '%' /* || c == '\n' */ || c == '\\' )
    { simple_label = false;
      break;
     }
  }

  if (simple_label) 
  { set_font(fn,fn_size,fn_user);
    double tw = win_p->text_width(label);
    double th = win_p->text_height(label);
    point pos;
    if (v)
       pos = compute_label_pos(v,tw,th);
    else
       pos = compute_label_pos(e,tw,th);
    if (dx != 0 || dy != 0) pos = pos.translate(dx,dy);

    win_p->draw_text(pos,label,label_clr);

    if (e && labelBoxEnabled)
      win_p->draw_rectangle(pos,pos.translate(tw,-th),blue);

    return;
   }


  if (label[0] == '$') fn = italic_font;

  list<string> L;
  double x,y,x1,y1;
  double yshift;
  split_label(v,e,label,L,x,y,x1,y1,yshift);

  double  d = fn_size/16;

  x += dx;
  y += dy;

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
     { set_font(fn,0.6*fn_size,fn_user);
       s = s.del(0);
       if (c == '_')
          { win_p->draw_text(x+d,y-yshift,s,label_clr);
            if (!L.empty() && L.head()[0] != '^')
               x += win_p->text_width(s)+2*d;
           }
       else
         { win_p->draw_text(x+d,y,s,label_clr);
           if (!L.empty() && L.head()[0] != '_')
               x += win_p->text_width(s)+2*d;
          }
      }
    else
     { set_font(fn,fn_size,fn_user);
       win_p->draw_text(x,y,s,label_clr);
       x += win_p->text_width(s);
      }
   }
}



void GraphWin::draw_node(node v, double dx, double dy) 
{

  if (!win_init_done) return;

  node_info &v_inf=n_info[v];

  char*  hatch_bits = 0;
   
  string lab = get_label(v);

  gw_node_shape shape = get_shape(v);

  int ppos = lab.index("@");

  if (ppos > -1)
  { 
    lab = lab(ppos+1,lab.length()-1);

    if (lab == "hatched1") hatch_bits = (char*)hatch_bits1;
    else
    if (lab == "hatched2") hatch_bits = (char*)hatch_bits2;
    else
    if (lab == "hatched3") hatch_bits = (char*)hatch_bits3;
    else
    if (lab.index(".xpm") > -1)
    { string xpm_file = xpm_path(lab);
      if (xpm_file != "" && is_file(xpm_file) && v_inf.pixmap == 0)
      { char* pmap = win_p->create_pixrect(xpm_file);
        if (pmap)
        { v_inf.pixmap = pmap;
          int w = win_p->get_pixrect_width(pmap);
          int h = win_p->get_pixrect_height(pmap);
          if (shape == circle_node) { 
             if (w < h) h = w; else w = h;
           }
          set_width(v,w);
          set_height(v,h);
         }
        else set_label(v,lab.replace("@",""));
       }
     }
   }



  double r1 = v_inf.r1;
  double r2 = v_inf.r2;
  double x  = v_inf.pos.xcoord()+dx;
  double y  = v_inf.pos.ycoord()+dy;


  int save_lw  = win_p->set_line_width(1);

  int clr      = v_inf.clr;
  int line_clr = v_inf.border_clr;
  char* pmap   = v_inf.pixmap;

//cout << string("clr = 0x%08x",clr) << endl;


  if (v_inf.selected && clr == invisible)  clr = white;

  int sel_w = 2;

  if (shape == square_node || shape == circle_node) r2 = r1;

/*
  if (shape == circle_node || shape == ellipse_node || shape == rhombus_node)
    sel_w = 1;
*/

  double pix = win_p->pix_to_real(1);

  if (r1 == 0) 
    r1 = win_p->pix_to_real(1);
  else
    r1 = win_p->pix_to_real(win_p->real_to_pix(r1));

  if (r2 == 0) 
    r2  = win_p->pix_to_real(1);
  else
    r2  = win_p->pix_to_real(win_p->real_to_pix(r2));

  //x  = win_p->xreal(win_p->xpix(x));
  //y  = win_p->yreal(win_p->ypix(y));

  double x0 = x - r1;
  double y0 = y - r2;
  double x1 = x + r1;
  double y1 = y + r2;

  double lw = v_inf.border_w;


  if (v_inf.selected)
  { double sx0 = x0 - sel_w*pix - lw;
    double sy0 = y0 - sel_w*pix - lw;
    double sx1 = x1 + sel_w*pix + lw;
    double sy1 = y1 + sel_w*pix + lw;
    win_p->draw_box(sx0,sy0,sx1,sy1,(clr != red) ? red : yellow);
    win_p->draw_rectangle(sx0,sy0,sx1,sy1,black);
   }


   int label_clr = v_inf.label_clr;

   if (label_clr == gw_auto_color) 
   { if (v_inf.label_pos == central_pos)
        label_clr = text_color(clr);
     else
        label_clr = black;
    }


   if (v == current_node || v == start_node || v == found_node)
   { 
     //clr = label_clr;  
     //win_p->set_stipple((char*)stip_bits,clr);

     if (v == found_frame) {
        line_clr = red;
        //lw = 3*lw;
     }

     color tclr = text_color(clr);
/*
     if (clr == invisible)
        clr = color(0.6f,tclr,white);
     else
     if (tclr == white)
       clr = color(0.25f,tclr,clr);
     else
       clr = color(0.40f,tclr,clr);
*/

/*
     float f = 0.7f;
     if (v == current_node || v == start_node) f = 0.4f; // darker
*/

     float f = 0.4f;

     if (tclr == white) f = 1-f;
     clr = color(f,tclr,clr);
     label_clr = text_color(clr);
    }
   else 
   { if (hatch_bits) 
     { win_p->set_stipple((char*)hatch_bits,clr); 
       clr = label_clr;  
      }
    }


/*
  double lw2 = win_p->pix_to_real((win_p->real_to_pix(lw)-1)/2);
  if (lw2 < 0) lw2 = 0;
*/

  double lw2 = lw/2;

  win_p->set_line_width(win_p->real_to_pix(lw));


  double rndness = 0;

  switch (shape) {

    case ellipse_node : 
    case circle_node  : { 

            win_p->draw_filled_ellipse(x,y,r1-lw2+pix,r2-lw2+pix,clr);

            win_p->set_stipple(0);

            if (pmap) // pixmap
            { win_p->clip_mask_window(0);
              win_p->clip_mask_ellipse(x,y,r1,r2,1);
              win_p->center_pixrect(x+pix,y,pmap);
              win_p->clip_mask_window(1);
             }

            if (lw > 0) {
              //win_p->draw_ellipse(x,y,r1+pix,r2+pix,line_clr);
              win_p->draw_ellipse(x,y,r1-lw2+pix,r2-lw2+pix,line_clr);
            }

            if (v_inf.label_pos == central_pos)
              win_p->set_clip_rectangle(x-r1+lw2,y-r2+lw2,x+r1-lw2,y+r2-lw2);
	    break;
       }

    case rhombus_node : {
            list<point> L;
            L.append(point(x,y0));
            L.append(point(x1,y));
            L.append(point(x,y1));
            L.append(point(x0,y));

            win_p->draw_filled_polygon(L,clr);
            win_p->set_stipple(0);

            if (pmap) // pixmap
            { win_p->clip_mask_window(0);
              win_p->clip_mask_polygon(L,1);
              win_p->center_pixrect(x+pix,y,pmap);
              win_p->clip_mask_window(1);
             }

            if (lw > 0) win_p->draw_polygon(L,line_clr);

            if (v_inf.label_pos == central_pos)
               win_p->set_clip_rectangle(x0,y0,x1,y1);
            break;
           }

    case triang0_node : 
    case triang1_node : 
    case triang2_node : 
    case triang3_node : {

            point a = v_inf.pos.translate(0,r2);
            //point b = v_inf.pos.translate(-sqrt(3.0)*r1/2, -0.5*r1);
            //point c = v_inf.pos.translate( sqrt(3.0)*r1/2, -0.5*r1);
            point b = v_inf.pos.translate(-r1, -sqrt(r2*r2-r1*r1));
            point c = b.translate(2*r1,0);

            double phi = 0;

            switch (shape) {
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
            L.append(a.rotate(v_inf.pos,phi));
            L.append(b.rotate(v_inf.pos,phi));
            L.append(c.rotate(v_inf.pos,phi));

            win_p->draw_filled_polygon(L,clr);
            win_p->set_stipple(0);

            if (pmap) // pixmap
            { win_p->clip_mask_window(0);
              win_p->clip_mask_polygon(L,1);
              win_p->center_pixrect(x+pix,y,pmap);
              win_p->clip_mask_window(1);
             }

            if (lw > 0) win_p->draw_polygon(L,line_clr);

            if (v_inf.label_pos == central_pos)
               win_p->clip_mask_polygon(L,1);
            break;
      }



    case roundrect_node : 
    case ovalrect_node : 
    case square_node : 
    case rectangle_node : {

            if (shape == roundrect_node) rndness = 0.5;
            if (shape == ovalrect_node)  rndness = 1.0;

            if (rndness == 0)
              { win_p->draw_box(x0,y0,x1,y1,clr);
                //win_p->draw_rectangle(x0,y0,x1,y1,clr);
               }
            else
               win_p->draw_roundbox(x0,y0,x1,y1,rndness,clr);

            win_p->set_stipple(0);


            if (pmap) // pixmap
            { win_p->set_clip_rectangle(x0,y0,x1,y1);
              win_p->center_pixrect(x+pix,y,pmap);
              win_p->reset_clipping();
             }

            if (lw <= 0) break;

            if (rndness == 0)
               win_p->draw_rectangle(x0+lw2,y0+lw2,x1-lw2,y1-lw2,line_clr);
            else
               win_p->draw_roundrect(x0+lw2,y0+lw2,x1-lw2,y1-lw2,rndness,
                                                                 line_clr);
   
            if (v_inf.label_pos == central_pos)
               win_p->set_clip_rectangle(x0+lw2,y0+lw2,x1-lw2,y1-lw2);
	    break;
      }

  }

  win_p->set_line_width(save_lw);

  int fn_pix  = win_p->real_to_pix(node_label_font_size); 

  if (hide_labels || fn_pix <= 4)
  { win_p->reset_clipping();
    return;
   }

  gw_position label_pos = v_inf.label_pos;
  int         label_t   = v_inf.label_t;
 

  string label = get_label(v);

  if (ppos > -1) label = label(0,ppos-1);

/*
  dx += pix;
*/

  double max_lw = 2.1*get_radius1(v);
  double max_lh = 2.1*get_radius2(v);

  if (label_t & index_label)
  { string s = get_index_label(v);
    if (node_label_font_size < max_lh && win_p->text_width(s) < max_lw &&
        label_t != index_label && label_pos != central_pos && pmap == 0)
    { v_inf.label_pos = central_pos;
      win_p->set_clip_rectangle(x0,y0,x1,y1);
      draw_obj_label(v,nil,s,label_clr,dx,dy);
      v_inf.label_pos = label_pos;
      win_p->reset_clipping();
     }
    if (label_t == index_label || pmap) label = s;
   }

  if ((label == "" && v_inf.label_curs < 0) ||
      (label_pos==central_pos && (clr != invisible || line_clr != invisible) &&
      (node_label_font_size > max_lh || win_p->text_width(label) > max_lw)))
  { win_p->reset_clipping();
    return;
   }

  if (v_inf.label_curs == -1)
     draw_obj_label(v,nil,label,label_clr,dx,dy);
  else
   { set_font(node_label_font_type,node_label_font_size,node_label_user_font);
     double tw = win_p->text_width(label);
     double th = win_p->text_height(label);
     if (tw == 0)
     { tw = win_p->text_width("| |");
       th = win_p->text_height("| |");
      }
     point pos = compute_label_pos(v,tw,th);
     x = pos.xcoord() + dx;
     y = pos.ycoord() + dy;
     win_p->draw_text_with_cursor(x,y,label,v_inf.label_curs,label_clr);
    }

  win_p->reset_clipping();
  return;
}


//----------------------------------------------------------------------------

void GraphWin::draw_edge(edge e) {
  draw_edge(e,0,0);
}

void GraphWin::redraw_edge(edge e)
{ window& W = get_window();
  W.start_buffering();
  draw_edge(e);
  flush_edge(e);
  W.stop_buffering();
}




void GraphWin::draw_edge(edge e, double dx, double dy) 
{

  if (!win_init_done) return;

  edge_info& e_inf = e_info[e];

  color c = e_inf.clr;

  color label_clr = e_inf.label_clr;

  double e_length = 0; 

double pix = win_p->pix_to_real(1);

  list<point> P;
  point q;
  forall(q,e_inf.p) 
  {
    //point p = q.translate(dx,dy);
    point p = q.translate(dx-pix,dy-pix); // WHY ?

    if (!P.empty()) e_length += p.distance(P.back());
    P.append(p);
   }


/*
  int arr = 0;
  if (e_inf.dir == redirected_edge)          arr = 1;
  else if (e_inf.dir == directed_edge)       arr = 2;
  else if (e_inf.dir == bidirected_edge)     arr = 3;
  else if (e_inf.dir == mid_directed_edge)   arr = 4;
  else if (e_inf.dir == mid_redirected_edge) arr = 8;
  else if (e_inf.dir == mid_bidirected_edge) arr = 12;
*/

  int arr = e_inf.dir;

  if (win_p->real_to_pix(e_length) < 10) arr = 0;
  if (win_p->real_to_pix(e_length) < 3) return;


  //window& W = get_window();
  if (P.length() > 2) normalize_polygon(P,win_p->pix_to_real(1));

  line_style old_style = win_p->get_line_style();
  int        old_width = win_p->get_line_width();

  int e_width = get_width(e);

  double e_thick = get_thickness(e);

  if (e_inf.selected)
  { win_p->set_line_style(solid);
    win_p->set_line_width(e_width+6);
    if (e_inf.shape == circle_edge && P.length() > 2)
     { list_item it1 = P.first();
       list_item it2 = P.succ(it1);
       list_item it3 = P.last();
       win_p->draw_arc(P[it1],P[it2],P[it3],grey1);
      }
    else
       win_p->draw_polyline(P,grey1);
   }

  if (e_inf.width == 0) 
  { win_p->set_line_style(old_style);
    win_p->set_line_width(old_width);
    return;
   }

  win_p->set_line_style((line_style)e_inf.style);
  win_p->set_line_width(e_width);


  if (e_inf.shape == circle_edge && P.size() > 2)
  { list_item it1 = P.first();
    list_item it2 = P.succ(it1);
    list_item it3 = P.last();
    win_p->draw_arc(P[it1],P[it2],P[it3],arr,e_thick,c);
    win_p->set_line_style(old_style);
    win_p->set_line_width(old_width);
   }
  else
  if ((e_inf.shape==spline_edge || e_inf.shape==bezier_edge) && P.size() > 2)
  { 
    if (e_inf.shape == spline_edge) 
        win_p->draw_spline(P,32,arr,e_thick,edge_border,c);

    if (e_inf.shape == bezier_edge) 
        win_p->draw_bezier(P,64,arr,e_thick,edge_border,c);

    win_p->set_line_style(old_style);
    win_p->set_line_width(old_width);
   }
  else
   { int js = win_p->set_join_style(0);
     //double d = 0.9*e_thick;
     double d = e_thick;
     win_p->draw_filled_arrow(P,arr,d,edge_border,c);
     win_p->set_join_style(js);
    }

  
  if (e == current_edge || e_inf.selected)  // show bends
  { list_item it;
    forall_items(it,P)
    { if (it == P.first() || it == P.last()) continue;
      double x = P[it].xcoord();
      double y = P[it].ycoord();
      double d = win_p->pix_to_real(4 + e_width/2);
      win_p->set_line_style(solid);
      win_p->set_line_width(1);
      //win_p->draw_disc(x,y,d,white);
      win_p->draw_disc(x,y,d,ivory);
      win_p->draw_circle(x,y,d,c);
     }
   }
  
  // slider

  for(int i=0; i<MAX_SLIDERS; i++)
  { 
    if (edge_slider_handler[i]==NULL && 
        start_edge_slider_handler[i]==NULL && 
        end_edge_slider_handler[i]==NULL) continue;

    if (e_inf.slider_f == NULL) continue;

    point p1 = P.head(); 
    point p2 = P.tail(); 

    vector vec = p2 - p1;
    double f = e_inf.slider_f[i];
    point q = p1.translate(f*vec);

    win_p->set_line_width(3);

    if (e_inf.shape == circle_edge && P.length() > 2)
    { point p3 = P[P.succ(P.first())];
      circle C(p1,p3,p2);
      point cent = C.center();
      if (orientation(p1,p2,p3) < 0)
        { double phi = f*cent.angle(p1,p2);
          q = p1.rotate(cent,phi);
         }
      else
        { double phi = (1-f)*cent.angle(p2,p1);
          q = p2.rotate(cent,phi);
         }
     }

    win_p->set_line_style(solid);

/*
    double dmin = win_p->pix_to_real(6);
    double d = e_thick/1.5 + win_p->pix_to_real(4);
*/

    double dmin = win_p->pix_to_real(8);
    double d = e_thick/1.2 + win_p->pix_to_real(4);

    if (d < dmin) d = dmin;

    double x  = q.xcoord();
    double y  = q.ycoord();
    color slc = slider_color[i];

    win_p->set_line_width(1);
    win_p->draw_box(x-d,y-d,x+d,y+d,slc);
    win_p->draw_pix(x,y,text_color(slc));
    win_p->draw_rectangle(x-d,y-d,x+d,y+d,black);

    win_p->set_line_style((line_style)e_inf.style);
    win_p->set_line_width(e_width);
  }

  win_p->set_line_style(old_style);
  win_p->set_line_width(old_width);


  // label

  string label = get_label(e);

  if (label == "" && e_inf.label_curs < 0) return; 

  if (e_inf.label_curs == -1)
     draw_obj_label(nil,e,label,label_clr,dx,dy);
  else
   { set_font(edge_label_font_type,edge_label_font_size,edge_label_user_font);
     double tw = win_p->text_width(label);
     double th = win_p->text_height(label);
     point pos = compute_label_pos(e,tw,th);
     double x = pos.xcoord() + dx;
     double y = pos.ycoord() + dy;
     win_p->draw_text_with_cursor(x,y,label,e_inf.label_curs,e_inf.label_clr);
   }

}

//----------------------------------------------------------------------------

void GraphWin::draw_node_with_edges(node v) { 
  edge e;
  forall_adj_edges(e,v) draw_edge(e);
  forall_in_edges(e,v)  draw_edge(e);
  draw_node(v);
}

//----------------------------------------------------------------------------


void GraphWin::draw_edges(node v, node w) {
  edge e;
  forall_inc_edges(e,v,w) draw_edge(e);
  forall_inc_edges(e,w,v) draw_edge(e);
}

//----------------------------------------------------------------------------

void GraphWin::draw_graph(node v, edge e,
                          double x0, double y0, double x1, double y1, 
                          double dx, double dy)
{ 
  // draw subgraph in (x0,y0,x1,y1) without v and e translated by (dx,dy)

  if (!edges_embedded) embed_edges();

  list<node> V = get_nodes_in_area(x0,y0,x1,y1);
  list<edge> E = get_edges_in_area(x0,y0,x1,y1);

  edge x;
  forall(x,E) 
    if (x != e && source(x) != v && target(x) != v) draw_edge(x,dx,dy);

  node u;
  forall(u,V) 
    if (u != v) draw_node(u,dx,dy);
}

void GraphWin::draw_graph(double x0, double y0, double x1, double y1,
                                                double dx, double dy) 
{ draw_graph(nil,nil,x0,y0,x1,y1,dx,dy); }

void GraphWin::draw_graph(node v,double dx, double dy) 
{ 
/*
  double x0 = get_xmin() - dx;
  double y0 = get_ymin() - dy;
  double x1 = get_xmax() - dx;
  double y1 = get_ymax() - dy;
  draw_graph(v,nil,x0,y0,x1,y1,dx,dy);
*/

  if (!edges_embedded) embed_edges();

  edge e;
  forall_edges(e,*gr_p) 
    if (source(e) != v && target(e) != v) draw_edge(e,dx,dy);

  node u;
  forall_nodes(u,*gr_p) 
    if (u != v) draw_node(u,dx,dy);
}

void GraphWin::draw_graph(edge e,double dx, double dy) 
{ 
/*
  double x0 = get_xmin() - dx;
  double y0 = get_ymin() - dy;
  double x1 = get_xmax() - dx;
  double y1 = get_ymax() - dy;
  draw_graph(nil,e,x0,y0,x1,y1,dx,dy);
*/

  if (!edges_embedded) embed_edges();

  edge x;
  forall_edges(x,*gr_p) 
    if (x != e) draw_edge(x,dx,dy);

  node u;
  forall_nodes(u,*gr_p) draw_node(u,dx,dy);
}


void GraphWin::draw_graph(double dx, double dy) 
{ draw_graph(node(nil),dx,dy); }
           

void GraphWin::draw_graph(const list<node>& L,double dx, double dy) 
{
  if (L.size() == 1)
  { draw_graph(L.head());
    return;
   }

  const graph& G=get_graph();

  double x0 = get_xmin() - dx;
  double y0 = get_ymin() - dy;
  double x1 = get_xmax() - dx;
  double y1 = get_ymax() - dy;

  list<node> V = get_nodes_in_area(x0,y0,x1,y1);
  list<edge> E = get_edges_in_area(x0,y0,x1,y1);

  node_array<bool> draw(G,true);

  node v;
  forall(v,L) draw[v] = false;

  edge e;
  forall(e,E) 
    if (draw[target(e)] && draw[source(e)]) draw_edge(e);

  forall(v,V) 
    if (draw[v]) draw_node(v);
}



void GraphWin::draw_graph(const list<edge>& L, double dx, double dy)
{
  if (L.size() == 1)
  { draw_graph(L.head());
    return;
   }

  const graph& G=get_graph();

  double x0 = get_xmin() - dx;
  double y0 = get_ymin() - dy;
  double x1 = get_xmax() - dx;
  double y1 = get_ymax() - dy;

  list<node> V = get_nodes_in_area(x0,y0,x1,y1);
  list<edge> E = get_edges_in_area(x0,y0,x1,y1);

  edge_array<bool> draw(G,true);
  edge e;
  forall(e,L) draw[e] = false;

  forall(e,E) if (draw[e]) draw_edge(e);

  node v;
  forall(v,V) draw_node(v);
}


//----------------------------------------------------------------------------

void GraphWin::s_redraw(window *w_p, double x0, double y0, double x1, double y1)
{ 

  window&    W = *w_p;
  GraphWin& gw = *(W.get_graphwin());

  if (gw.win_width != W.width() || gw.y_max != W.ymax()) // window resized
  { double f = double(gw.win_width)/W.width();
    gw.win_width = W.width();
    gw.window_init(W.xmin(),W.xmax(),W.ymin(),f);
    return;
   }

  W.start_buffering();
  W.flush_buffer(x0,y0,x1,y1);
  W.stop_buffering();
}

//----------------------------------------------------------------------------


void GraphWin::draw_message()
{ if (message_str == "") return;
  win_p->set_text_font();
  double th = win_p->text_height("H");
  double x0 = get_xmin() + 0.5*th;
  double x1 = get_xmax() - 0.5*th;
  double y  = get_ymax() - 0.3*th;
  win_p->text_box(x0,x1,y,message_str);
}


void GraphWin::redraw() 
{ 
  if (!win_p->is_open()) return;

  if (!n_anim_list.empty()) 
    { win_p->del_pixrect(n_anim_buf);
      n_animation_start(n_anim_list);
      win_p->start_buffering();
      node v;
      forall(v,n_anim_list)
      { embed_node_with_edges(v);
        draw_node_with_edges(v);
       }
     }
  else
     if (!e_anim_list.empty()) 
     { win_p->del_pixrect(e_anim_buf);
       e_animation_start(e_anim_list);
       win_p->start_buffering();
       edge e;
       forall(e,e_anim_list) 
       { node v = source(e);
         node w = target(e);
         embed_edges(v,w);
         draw_edges(v,w);
        }
      }
    else
       { win_p->start_buffering();
         win_p->clear();
         draw_graph(get_xmin(),get_ymin(),get_xmax(),get_ymax());
       }

  draw_message();

  win_p->flush_buffer();
  win_p->stop_buffering();

/*
cout << "read mouse 1" << endl;
win_p->read_mouse();
*/

}


void GraphWin::redraw(double x0, double y0, double x1, double y1) 
{ if (!win_p->is_open()) return;
  win_p->start_buffering();
  win_p->clear();
  draw_graph(x0,y0,x1,y1);
  win_p->flush_buffer(x0,y0,x1,y1);
  win_p->stop_buffering();
}



//----------------------------------------------------------------------------
// animation
//----------------------------------------------------------------------------



void GraphWin::n_animation_start(const list<node>& L, bool nodes_only)
{ n_anim_list = L;
  n_anim_flush = set_flush(false);
  window& W = get_window();
  graph& G = get_graph();
  W.start_buffering();
  if (nodes_only)
    { node v;
      forall(v,L)
      { current_node = v;
        draw_node(v);
        current_node = nil;
       }
     }
  else
    { if (L.length() == 1 && bg_pixmap == 0 && bg_redraw_func == 0) // seems not work with bg-pixmaps 
       { node u = L.head();
         compute_bounding_box(L.head(),anim_x0,anim_y0,anim_x1,anim_y1);
         W.clear(anim_x0,anim_y0,anim_x1,anim_y1);
         list<edge> E = get_edges_in_area(anim_x0,anim_y0,anim_x1,anim_y1);
         W.set_clip_rectangle(anim_x0,anim_y0,anim_x1,anim_y1);

         edge e;
         forall(e,E) { 
            if (source(e) != u && target(e) != u) draw_edge(e);
         }

         if (bg_pixmap == 0) W.reset_clipping();

         node v;
         forall_nodes(v,G) {
           if (v!=u) draw_node(v);
         }
/*
         list<node> V = get_nodes_in_area(anim_x0,anim_y0,anim_x1,anim_y1);
         forall(v,V) if (v != u) draw_node(v);
*/
        }
      else
      { W.clear();
        draw_graph(L); // draw graph except of L
       }
      draw_message();
     }
  n_anim_buf = W.get_window_pixrect();
  W.stop_buffering();
}


void GraphWin::n_animation_start(node v, bool nodes_only)
{ window& W = get_window();
  list<node> L = (list<node>)v;
  n_animation_start(L,nodes_only);
  W.get_mouse(); // allow interaction
 }


void GraphWin::n_animation_end()
{ window& W = get_window();

  if (bg_redraw_func) {
     redraw();
  }

  node v;
  forall(v,n_anim_list) embed_node_with_edges(v);
  n_anim_list.clear();
  W.del_pixrect(n_anim_buf);
  set_flush(n_anim_flush);
  W.get_mouse(); // allow interaction
}


void GraphWin::n_animation_step(bool nodes_only)
{ window& W = get_window();
  W.start_buffering();
  W.set_pixrect(n_anim_buf);
  node v;
  if (nodes_only)
    forall(v,n_anim_list) draw_node(v);
  else
    forall(v,n_anim_list)
    { embed_node_with_edges(v);
      draw_node_with_edges(v);
     }
  if (n_anim_list.length() == 1 && bg_redraw_func == 0)
   { adjust_bounding_box(n_anim_list.head(),anim_x0,anim_y0, anim_x1,anim_y1);
     W.flush_buffer(anim_x0,anim_y0,anim_x1,anim_y1);
    }
  else
     W.flush_buffer();
  W.stop_buffering();
  W.get_mouse(); // allow interaction
}


void GraphWin::e_animation_start(const list<edge>& L)
{ e_anim_list = L;
  e_anim_flush = set_flush(false);
  window& W = get_window();
  W.start_buffering();
  W.clear();
  draw_graph(L);
  draw_message();
  e_anim_buf = W.get_window_pixrect();
  W.stop_buffering();
  if (L.length() == 1) 
    compute_bounding_box(L.head(),anim_x0,anim_y0,anim_x1,anim_y1);
  W.get_mouse(); // allow interaction
}

void GraphWin::e_animation_start(edge e)
{ window& W = get_window();
  list<edge> L = (list<edge>)e;
  e_animation_start(L);
  W.get_mouse(); // allow interaction
 }


void GraphWin::e_animation_end()
{ window& W = get_window();
  e_anim_list.clear();
  W.del_pixrect(e_anim_buf);
  set_flush(e_anim_flush);
  W.get_mouse(); // allow interaction
}


void GraphWin::e_animation_step()
{ window& W = get_window();
  W.start_buffering();
  W.set_pixrect(e_anim_buf);
  edge e;
  forall(e,e_anim_list) 
  { node v = source(e);
    node w = target(e);
    embed_edges(v,w);
    draw_edges(v,w);
   }
  if (e_anim_list.length() == 1)
   { adjust_bounding_box(e_anim_list.head(),anim_x0,anim_y0, anim_x1,anim_y1);
     W.flush_buffer(anim_x0,anim_y0,anim_x1,anim_y1);
    }
  else
    W.flush_buffer();
  W.stop_buffering();
  W.get_mouse(); // allow interaction
}


void GraphWin::redraw(node v)
{ // redraw graph (only node v has changed)
  window& W = get_window();
  W.start_buffering();
  W.clear();
  draw_graph();
  flush_node(v);
  W.stop_buffering();
}

void GraphWin::redraw(edge e)
{ // redraw graph (only edge e has changed)
  window& W = get_window();
  W.start_buffering();
  W.clear();
  draw_graph();
  flush_edge(e);
  W.stop_buffering();
}

LEDA_END_NAMESPACE
  
