/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gml_gwgraph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//-------------------------------------------------------------------
// class gml_gwgraph, parser for graph in GML format with additional
//                    attributes like positions, colors, bends, ...  
//                    + a procedure for writing GW graph in GML     
//                                                                   
// by David Alberts and S. Naeher (1997/98)                                    
//
// last modifiations:
//
// March 1998:  node border width
// June  1998:  edge directions
//
//------------------------------------------------------------------- 

#include <LEDA/graphics/gml_gwgraph.h>
#include <LEDA/core/string.h>
#include <LEDA/system/stream.h>
#include <string.h>
#include <stdlib.h>


//------------------------------------------------------------------- //
// parser                                                             //
//------------------------------------------------------------------- //

LEDA_BEGIN_NAMESPACE

gml_gwgraph::gml_gwgraph(GraphWin* GW)
: gml_graph(GW->get_graph()), gw(GW), the_mirror(false) {}

gml_gwgraph::gml_gwgraph(GraphWin* GW, char* filename, bool mirror)
: gml_graph(GW->get_graph()), gw(GW), the_mirror(mirror) { 
  parse(filename); 
}

gml_gwgraph::gml_gwgraph(GraphWin* GW, istream& ins, bool mirror)
: gml_graph(GW->get_graph()), gw(GW), the_mirror(mirror) { 
  parse(ins); 
}



void gml_gwgraph::get_node_shape(const gml_object* gobj, node v)
{
  char* shape = gobj->get_string();

  if( !strcmp(shape,"rectangle") || !strcmp(shape,"text") )
    gw->set_shape(v,rectangle_node);
  else
  {
    gw->set_shape(v,ellipse_node);   // default
  }
}

void gml_gwgraph::get_node_ltype(const gml_object* gobj, node v)
{
  char* ltype = gobj->get_string();

  if(strcmp(ltype,"no_label") == 0)
  { gw->set_label_type(v,no_label);
    return;
   }

  if(strcmp(ltype,"index_label") == 0)
  { gw->set_label_type(v,index_label);
    return;
   }

  if(strcmp(ltype,"text") == 0) 
  { gw->set_label_type(v,user_label);
    return;
   }

  if(strcmp(ltype,"data_label") == 0)
  { gw->set_label_type(v,data_label);
    return;
   }

  if(strcmp(ltype,"indextext_label") == 0)
  { gw->set_label_type(v,gw_label_type(index_label | user_label));
    return;
   }

  if(strcmp(ltype,"indexdata_label") == 0)
  { gw->set_label_type(v,gw_label_type(index_label | data_label));
    return;
   }
}

void gml_gwgraph::get_edge_ltype(const gml_object* gobj, edge e)
{
  char* ltype = gobj->get_string();

  if(strcmp(ltype,"no_label") == 0)
  { gw->set_label_type(e,no_label);
    return;
   }

  if(strcmp(ltype,"index_label") == 0)
  { gw->set_label_type(e,index_label);
    return;
   }

  if(strcmp(ltype,"text_label") == 0) 
  { gw->set_label_type(e,user_label);
    return;
   }

  if(strcmp(ltype,"data_label") == 0)
  { gw->set_label_type(e,data_label);
    return;
   }

  if(strcmp(ltype,"indextext_label") == 0)
  { gw->set_label_type(e,gw_label_type(index_label | user_label));
    return;
   }

  if(strcmp(ltype,"indexdata_label") == 0)
  { gw->set_label_type(e,gw_label_type(index_label | data_label));
    return;
   }
}


color gml_gwgraph::get_color(const gml_object* gobj)
{
  char* rgb =  gobj->get_string();

  if (strlen(rgb) == 0) return invisible;

  long col = strtol(rgb+1, 0, 16);   // convert the string
  // skip leading # ^^^^^

  int r = (col & 0xff0000) >> 16;
  int g = (col & 0x00ff00) >> 8;
  int b = (col & 0x0000ff);

  return color(r,g,b);
}


void gml_gwgraph::get_edge_style(const gml_object* gobj, edge e)
{
  char* style = gobj->get_string();

  if(!strcmp(style,"dashed_edge"))
    gw->set_style(e,dashed_edge);
  else
  {
    if(!strcmp(style,"dotted_edge"))
      gw->set_style(e,dotted_edge);
  }
}

void gml_gwgraph::get_edge_shape(const gml_object* gobj, edge e)
{
  char* shape = gobj->get_string();

  if(strcmp(shape,"poly_edge"))  // if not a poly_edge
  {
    if(!strcmp(shape,"circle_edge"))
    {
      gw->set_shape(e,circle_edge);
      return;
    }
    if(!strcmp(shape,"bezier_edge"))
    {
      gw->set_shape(e,bezier_edge);
      return;
    }
    if(!strcmp(shape,"spline_edge"))
    {
      gw->set_shape(e,spline_edge);
      return;
    }
  }
}

void gml_gwgraph::get_node_lpos(const gml_object* gobj, node v)
{
  char* l = gobj->get_string();
  string lpos(l);
  if(the_pos.defined(lpos))
  {
    switch(the_pos[lpos])
    {
      case central_pos:   { gw->set_label_pos(v,central_pos);   break; }
      case northwest_pos: { gw->set_label_pos(v,northwest_pos); break; }
      case north_pos:     { gw->set_label_pos(v,north_pos);     break; }
      case northeast_pos: { gw->set_label_pos(v,northeast_pos); break; }
      case south_pos:     { gw->set_label_pos(v,south_pos);     break; }
      case southeast_pos: { gw->set_label_pos(v,southeast_pos); break; }
      case southwest_pos: { gw->set_label_pos(v,southwest_pos); break; }
      case east_pos:      { gw->set_label_pos(v,east_pos);      break; }
      case west_pos:      { gw->set_label_pos(v,west_pos);      break; }
      default: break;
    }
  }
}


void gml_gwgraph::get_edge_lpos(const gml_object* gobj, edge e)
{
  char* l = gobj->get_string();
  string lpos(l);

  if(the_pos.defined(lpos))
  {
    switch(the_pos[lpos])
    {
      case central_pos:   { gw->set_label_pos(e,central_pos);   break; }
      case northwest_pos: { gw->set_label_pos(e,northwest_pos); break; }
      case north_pos:     { gw->set_label_pos(e,north_pos);     break; }
      case northeast_pos: { gw->set_label_pos(e,northeast_pos); break; }
      case south_pos:     { gw->set_label_pos(e,south_pos);     break; }
      case southeast_pos: { gw->set_label_pos(e,southeast_pos); break; }
      case southwest_pos: { gw->set_label_pos(e,southwest_pos); break; }
      case east_pos:      { gw->set_label_pos(e,east_pos);      break; }
      case west_pos:      { gw->set_label_pos(e,west_pos);      break; }
      default: break;
    }
  }
}


void gml_gwgraph::get_edge_direction(const gml_object* gobj, edge e)
{
  char* arrow_str = gobj->get_string();

  if (strcmp(arrow_str,"none") == 0)
    gw->set_direction(e,undirected_edge);
  else
    if (strcmp(arrow_str,"first") == 0)
      gw->set_direction(e,redirected_edge);
    else
      if (strcmp(arrow_str,"last") == 0)
        gw->set_direction(e,directed_edge);
      else 
        if (strcmp(arrow_str,"both") == 0)
           gw->set_direction(e,bidirected_edge);
    
}




static gw_position mirror_pos(gw_position pos)
// change gw_position according to a flip of the y-axis (Rotation ?)
{
  gw_position newp = pos;
  switch(pos)
  {
    case north_pos:     { newp = south_pos; break; }
    case northeast_pos: { newp = southwest_pos; break; }
    case northwest_pos: { newp = southeast_pos; break; }
    case south_pos:     { newp = north_pos; break; }
    case southwest_pos: { newp = northeast_pos; break; }
    case southeast_pos: { newp = northwest_pos; break; }
    default: break;
  }
  return newp;
}

void gml_gwgraph::mirror()
{
  node v = the_graph->first_node();

  if (v == nil) return;

  double y_min = gw->get_position(v).ycoord();
  double y_max = gw->get_position(v).ycoord();

  forall_nodes(v,*the_graph)
  { double y = gw->get_position(v).ycoord();
    if (y > y_max) y_max = y;
    if (y < y_min) y_min = y;
  }

  double y_mid = (y_min+y_max)/2;

  point p;
  forall_nodes(v,*the_graph)
  {
    // y coordinate
    p = gw->get_position(v);
    p = point(p.xcoord(), 2*y_mid - p.ycoord());
    gw->set_position(v,p);

    // label position
    if(gw->get_label_type(v) != no_label)
      gw->set_label_pos(v,mirror_pos(gw->get_label_pos(v)));
  }

  edge e;
  forall_edges(e,*the_graph)
  {
    // bends
    bends.clear();
    bends = gw->get_bends(e);
    if(bends.size())
    {
      list<point> new_bends;
      point bend;
      forall(bend,bends)
      {
        p = point(bend.xcoord(), 2*y_mid - bend.ycoord());
        new_bends.append(p);
      }
      gw->set_bends(e,new_bends);
    }

    // source and target anchors
    p = gw->get_source_anchor(e);
    if(p.ycoord() != 0.0)
    { p = point(p.xcoord(), (-1) * p.ycoord());
      gw->set_source_anchor(e,p);
     }
    p = gw->get_target_anchor(e);
    if(p.ycoord() != 0.0)
    {
      p = point(p.xcoord(), (-1) * p.ycoord());
      gw->set_target_anchor(e,p);
     }

    // label position
    if(gw->get_label_type(e) != no_label)
      gw->set_label_pos(e,mirror_pos(gw->get_label_pos(e)));
  }
}


void gml_gwgraph::init_rules()
{
  if(rules_set) return;

  reset_path();

  // node label
  append("graph");
    append("node");
      append("label");
        add_rule(node_label,gml_string);
      goback();

      // node x coordinate
      append("graphics");
        append("x");
          add_rule(node_x,gml_double);
        goback();

        // node y coordinate
        append("y");
          add_rule(node_y,gml_double);
        goback();

        // node width
        append("w");
          add_rule(node_w,gml_double);
        goback();

        // node height
        append("h");
          add_rule(node_h,gml_double);
        goback();

        // node border width
        append("width");
          add_rule(node_width,gml_double);
        goback();

        // node shape
        append("type");
          add_rule(node_shape,gml_string);
        goback();

        // node color
        append("fill");
          add_rule(node_color,gml_string);
        goback();

        // node outline color
        append("outline");
          add_rule(node_outline,gml_string);
        goback();
      goback();

      // node label color
      append("LabelGraphics");
        append("fill");
          add_rule(node_lcolor,gml_string);
        goback();

        // node label position
        append("anchor");
          add_rule(node_lpos,gml_string);
        goback();

        // node label type
        append("type");
          add_rule(node_ltype,gml_string);
        goback(); // end gw_type
      goback(); // end LabelGraphics
    goback(); // end node

    // edge label
    append("edge");
      append("label");
        add_rule(edge_label,gml_string);
      goback();

      // edge style
      append("graphics");

        // edge direction (arrow position)
        append("arrow");
          add_rule(edge_direction,gml_string);
        goback();

        append("style");
          add_rule(edge_style,gml_string);
        goback();

        // edge color
        append("fill");
          add_rule(edge_color,gml_string);
        goback();

        // edge width
        append("width");
          add_rule(edge_width,gml_double);
        goback();

        // use smooth curve for edge with bends
        append("smooth");
          add_rule(edge_smooth,gml_int);
        goback();

        // type of curve to use (GraphWin specific, default: poly_edge)
        append("shape");
          add_rule(edge_shape,gml_string);
        goback();

        // start of new bend
        append("Line");
          append("point");
            add_rule(edge_bend,gml_list);

            // x coordinate of a bend
            append("x");
              add_rule(edge_bendx,gml_double);
            goback();

            // y coordinate of a bend
            append("y");
              add_rule(edge_bendy,gml_double);
            goback(); // end y
          goback(); // end point
        goback(); // end Line
      goback(); // end graphics

      // edge anchors
      append("edgeAnchor");
        add_rule(edge_anchors,gml_list);

        // x coordinate of source anchor
        append("xSource");
          add_rule(edge_sancx,gml_double);
        goback();

        // y coordinate of source anchor
        append("ySource");
          add_rule(edge_sancy,gml_double);
        goback();

        // x coordinate of target anchor
        append("xTarget");
          add_rule(edge_tancx,gml_double);
        goback();

        // y coordinate of target anchor
        append("yTarget");
          add_rule(edge_tancy,gml_double);
        goback();
      goback();

      // edge label color
      append("LabelGraphics");
        append("fill");
          add_rule(edge_lcolor,gml_string);
        goback();

        // edge label type
        append("type");
          add_rule(edge_ltype,gml_string);
        goback();

        // edge label position
        append("anchor");
          add_rule(edge_lpos,gml_string);


  reset_path();

  // set up translation table for positions
  the_pos["c"]  = central_pos;
  the_pos["nw"] = northwest_pos;
  the_pos["n"]  = north_pos;
  the_pos["ne"] = northeast_pos;
  the_pos["e"]  = east_pos;
  the_pos["se"] = southeast_pos;
  the_pos["s"]  = south_pos;
  the_pos["sw"] = southwest_pos;
  the_pos["w"]  = west_pos;

  gml_graph::init_rules();
}



bool gml_gwgraph::interpret(gml_rule r, const gml_object* gobj)
{
  bool ok = true;

  switch(r)
  {
    // --- redefined rules --- //
    case new_graph:
    {
      ok = graph_intro(gobj);
      break;
    }
    case directed:
    // GraphWin currently does not work well with undirected graph,
    // so we only suppress drawing arrows and do not turn a graph
    // into an undirected one, if "directed 1" appears in the GML file.
    {
      bool is_directed = (gobj->get_int() != 0);
      gw->set_directed(is_directed);
      break;
    }
    case new_node:
    { node_count++;
      switch (node_count) {
        case 1: current_node = dummy1;
                break;
        case 2: current_node = dummy2;
                break;
        default:
                current_node = gw->new_node(point(0,0));
                break;
       }

      has_id = false;
      gml_node_rule f;
      forall(f,new_node_rules) ok = ok && (*f)(gobj,the_graph,current_node);
      break;
    }
    case new_edge:
    {
      edge e = gw->new_edge(dummy1,dummy2);
      (*(edge_s))[e] = -1;
      (*(edge_t))[e] = -1;
      current_edge = e;
      gml_edge_rule f;
      forall(f,new_edge_rules) ok = ok && (*f)(gobj,the_graph,current_edge);
      break;
    }

    // --- new rules --- //
    case node_x:
    {
      double x = gobj->get_double();
      double y = (gw->get_position(current_node)).ycoord();
      gw->set_position(current_node,point(x,y));
      break;
    }
    case node_y:
    {
      double x = (gw->get_position(current_node)).xcoord();
      double y = gobj->get_double();
      gw->set_position(current_node,point(x,y));
      break;
    }
    case node_shape:
    {
      get_node_shape(gobj,current_node);
      break;
    }
    case node_outline:
    {
      gw->set_border_color(current_node,get_color(gobj));
      break;
    }
    case node_w:
    {
      gw->set_radius1(current_node,gobj->get_double()/2);
      break;
    }
    case node_h:
    {
      gw->set_radius2(current_node,gobj->get_double()/2);
      break;
    }
    case node_width:
    {
      gw->set_border_thickness(current_node,gobj->get_double()/2);
      break;
    }
    case node_color:
    {
      gw->set_color(current_node,get_color(gobj));
      break;
    }
    case node_ltype:
    {
      get_node_ltype(gobj,current_node);
      break;
    }
    case node_lcolor:
    {
      gw->set_label_color(current_node,get_color(gobj));
      break;
    }
    case node_lpos:
    {
      get_node_lpos(gobj,current_node);
      break;
    }
    case node_label:
    {
      gw->set_label(current_node,gobj->get_string());
      break;
    }

    case edge_width:
    { double thick = gobj->get_double();
      int w = int(thick);
      if (w == thick) // integer indicates width in pixels
        gw->set_width(current_edge,w);
      else
        gw->set_thickness(current_edge,thick);
      break;
    }

    case edge_color:
    {
      gw->set_color(current_edge,get_color(gobj));
      break;
    }

    case edge_shape:
    {
      get_edge_shape(gobj,current_edge);
      break;
    }
    case edge_smooth:
    {
      if(gw->get_shape(current_edge) == poly_edge)
        gw->set_shape(current_edge,bezier_edge);
      break;
    }
    case edge_style:
    {
      get_edge_style(gobj,current_edge);
      break;
    }
    case edge_ltype:
    {
      get_edge_ltype(gobj,current_edge);
      break;
    }
    case edge_lcolor:
    {
      gw->set_label_color(current_edge,get_color(gobj));
      break;
    }
    case edge_lpos:
    {
      get_edge_lpos(gobj,current_edge);
      break;
    }
    case edge_direction:
    {
      get_edge_direction(gobj,current_edge);
      break;
    }
    case edge_anchors:
    {
      current_sanc = point(0,0);
      current_tanc = point(0,0);
      break;
    }
    case edge_sancx:
    {
      double x = gobj->get_double();
      double y = current_sanc.ycoord();
      current_sanc = point(x,y);
      break;
    }
    case edge_sancy:
    {
      double x = current_sanc.xcoord();
      double y = gobj->get_double();
      current_sanc = point(x,y);
      break;
    }
    case edge_tancx:
    {
      double x = gobj->get_double();
      double y = current_tanc.ycoord();
      current_tanc = point(x,y);
      break;
    }
    case edge_tancy:
    {
      double x = current_tanc.xcoord();
      double y = gobj->get_double();
      current_tanc = point(x,y);
      break;
    }
    case edge_bend:
    {
      current_bend = point(MAXINT,MAXINT);
      break;
    }
    case edge_bendx:
    {
      double x = gobj->get_double();
      double y = current_bend.ycoord();
      current_bend = point(x,y);
      break;
    }
    case edge_bendy:
    {
      double x = current_bend.xcoord();
      double y = gobj->get_double();
      current_bend = point(x,y);
      break;
    }
    case edge_label:
    {
      gw->set_label(current_edge,gobj->get_string());
      break;
    }

    // --- inherited rules (see .../src/graph/_g_gmlio.c) --- //
    default:
    {
      ok = gml_graph::interpret(r,gobj);
      break;
    }      
  }

  return ok;
}

bool gml_gwgraph::list_end(gml_rule r, const gml_object* gobj)
{
  bool ok = true;

  switch(r)
  {
    case new_graph:
    {
      ok = graph_end(gobj);

      if(ok)
      { if (the_mirror) mirror();
/*
        // this resets all edge attributes (e.g. bends and anchors)
        //gw->update_graph();
*/
        gw->init_matrix();
        gw->embed_edges();
       }

      gw->set_flush(old_flush);

      break;
    }

    case new_edge:
    {
      if (!bends.empty())
      { if (bends.size() >= 2)
        { bends.pop_front();
          bends.pop_back();
         }

        gw->set_bends(current_edge,bends);
        bends.clear();
      }

/*
      if(bends.size() >= 2)
      { point p = bends.pop();
        point q = bends.Pop();
        point r;
        point s;

        node v = (*(node_by_id))[(*edge_s)[current_edge]];
        node w = (*(node_by_id))[(*edge_t)[current_edge]];
        point pv = gw->get_position(v);
        point pw = gw->get_position(w);

        if (!bends.empty())
        { r = bends.head();
          s = bends.tail();
         }
        else
        { r = pw;
          s = pv;
         }

        if (p != pv && fabs(pv.angle(p,r)) > 0.01)
        { double vdx = (p.xcoord() - pv.xcoord())/gw->get_radius1(v);
          double vdy = (p.ycoord() - pv.ycoord())/gw->get_radius2(v);
          gw->set_source_anchor(current_edge,point(vdx,vdy));
         }

        if (q != pw && fabs(pw.angle(q,s)) > 0.01)
        { double wdx = (q.xcoord() - pw.xcoord())/gw->get_radius1(w);
          double wdy = (q.ycoord() - pw.ycoord())/gw->get_radius2(w);
          gw->set_target_anchor(current_edge,point(wdx,wdy));
         }

        gw->set_bends(current_edge,bends);
      }
      bends.clear();
*/

      ok = edge_end(gobj);
      break;
    }

    case edge_anchors:
    {
      gw->set_source_anchor(current_edge,current_sanc);
      gw->set_target_anchor(current_edge,current_tanc);
      break;
    }

    case edge_bend:
    {
      if( (current_bend.xcoord() != MAXINT)
          &&
          (current_bend.ycoord() != MAXINT) ) bends.append(current_bend);
      else
      {
        print_error(*gobj,"ignoring incomplete bend");
      }
      break;
    }

    default:
    {
      ok = gml_graph::list_end(r,gobj);
      break;
    }
  }

  return ok;
}


bool gml_gwgraph::graph_intro(const gml_object* gobj)
{
  old_flush = gw->set_flush(false);

  gw->get_graph().clear();
  gw->update_graph();

  // default window coordinats
  gw->set_default_win_coords();
  gw->zoom(0);

  // default look
  gw->init_default();

  // default is undirected (will be changed if "directed 1" attribute found)
  gw->set_directed(false);

  node_by_id = new map<int,node>;
  edge_s = new map<edge,int>;
  edge_t = new map<edge,int>;

  the_graph->clear();
  node_count = 0;

  dummy1 = gw->new_node(point(0,0));
  dummy2 = gw->new_node(point(0,0));

  right_node_type = true;
  right_edge_type = true;

  // call new graph rules
  bool ok = true;
  gml_graph_rule f;
  forall(f,new_graph_rules) ok = ok && (*f)(gobj,the_graph);


  return ok;
}


//------------------------------------------------------------------- //
// writing GW graph in GML                                           //
//------------------------------------------------------------------- //

static void print_double(ostream& out, const char* prefix, double d)
// N.B. we want a decimal point in any case in order to
//      mark this as a double
{
  if(d >= 100000.0)
  {
    // use scientific format
    out << string("%s%.1e\n",prefix,d);
  }
  else
  {
    // assemble prefix and value in s
    string s("%s%g",prefix,d);

    // look for decimal point in s
    bool no_point = true;
    for(int i=0; i<s.length(); i++) if(s[i] == '.') no_point = false;

    // if there is no point, we append one
    if(no_point) s = s + string(".0");

    // print s and closing newline to out
    out << s << "\n";
  }
}

static void print_color(ostream& out, const char* prefix, const color& c)
{
  out << prefix;

  if (c == invisible)
    out << "\"\"" << endl;
  else
  { int r,g,b;
    c.get_rgb(r,g,b);
    out << string("\"#%06X\"", (r<<16) + (g<<8) + b) << endl;
   }
}

inline int text_color(int col)
{ 
  if (col == black ||
      col == red   ||
      col == blue  ||
      col == violet||
      col == brown ||
      col == pink  ||
      col == green2||
      col == blue2 ||
      col == grey3 )
     return white;
  else
     return black;
}


static void print_position(ostream& out, int pos)
{ switch(pos)
  { case central_pos:   { out << "c"; break; }
    case north_pos:     { out << "n"; break; }
    case south_pos:     { out << "s"; break; }
    case east_pos:      { out << "e"; break; }
    case west_pos:      { out << "w"; break; }
    case northwest_pos: { out << "nw"; break; }
    case northeast_pos: { out << "ne"; break; }
    case southeast_pos: { out << "se"; break; }
    case southwest_pos: { out << "sw"; break; }
    default:            { out << "unknown"; break; }
  }
}



bool write_gml(GraphWin& GW, ostream& out,
               void (*node_cb)(ostream&,const graph*, const node),
               void (*edge_cb)(ostream&,const graph*, const edge))
// writes the graph in GW in GML format to the ostream out. The callback
// functions node_cb and edge_cb are called once while writing each
// node or edge respectively. They can be used to write additional
// fields to the GML output. The user is in charge of keeping the
// syntactical correctness of the GML output when using the callbacks.
{

  const graph& G = GW.get_graph();

  out << "Creator "  << '"' << "LEDA GraphWin" << '"' << endl;
  out << "Version "  << GraphWin::version() << endl;
  out << "graph [\n" << endl;
  out << "  label "  << '"' << '"' << endl;
  out << "  directed " << int(GW.get_directed()) << "\n\n";

  bool write_node_parameters = (string(G.node_type()) != "void");
  bool write_edge_parameters = (string(G.edge_type()) != "void");

  node v;
  forall_nodes(v,G)
  {
    out << "  node [\n";
    out << "    id " << G.index(v) << "\n";
    if(write_node_parameters)
    {
      out << "    parameter " << '"' << G.get_node_entry_string(v);
      out << '"' << "\n";
    }
    if(node_cb) (*node_cb)(out,&G,v);

/*
    if((GW.get_label_type(v) == user_label) && GW.get_label(v).length())
      out << "    label " << '"' << GW.get_label(v) << '"' << "\n";
*/

    string s = GW.get_user_label(v);

    if(s.length())
      out << "    label " << '"' << s.replace_all("\\n","\n") << '"' << "\n";

    gw_position lp = GW.get_label_pos(v);

  //if(lp != central_pos)
    { out << "      labelAnchor " << '"';
      print_position(out,lp);
      out << '"' << "\n";
    }

    out << "    graphics [\n";
    point pos = GW.get_position(v);
    print_double(out,"      x ",pos.xcoord());
    print_double(out,"      y ",pos.ycoord());
    print_double(out,"      w ",2*GW.get_radius1(v));
    print_double(out,"      h ",2*GW.get_radius2(v));
 
    out << "      type " << '"';
    if (GW.get_shape(v) == rectangle_node || GW.get_shape(v) == square_node) 
      out << "rectangle";
    else 
      out << "oval";
    out << '"' << endl;

    print_double(out,"      width ",   2*GW.get_border_thickness(v));
    print_color(out, "      fill ",    GW.get_color(v));
    print_color(out, "      outline ", GW.get_border_color(v));

    out << "    ]\n";

    gw_label_type lt = GW.get_label_type(v);
    if(lt != no_label)
    {
      string s;
      if (lt & index_label) s = "index";

      switch (lt & ~index_label)
      { case user_label:  { s = /* s + */ "text"; break; }
        case data_label:  { s = /* s + */ "data"; break; }
        case no_label:    break;
        default:          { s = "unknown"; break; }
      }

      if (s != "text") s = s + "_label";

      out << "    LabelGraphics [\n";
      out << "      type " << '"' << s << '"' << "\n";

      color lcol = GW.get_label_color(v);
      if (lcol == gw_auto_color) lcol = text_color(lcol);
      print_color(out,"      fill ",(int)lcol);

      gw_position lp = GW.get_label_pos(v);

      //if(lp != central_pos)
      { out << "      anchor " << '"';
        print_position(out,lp);
        out << '"' << "\n";
       }
      out << "    ]\n";
    }
    out << "  ]\n";
  }
  out << "\n\n";

  edge e;
  forall_edges(e,G)
  {
    out << "  edge [\n";
    out << "    source " << G.index(source(e)) << "\n";
    out << "    target " << G.index(target(e)) << "\n";
    if(write_edge_parameters)
    { out << "    parameter " << '"' << G.get_edge_entry_string(e);
      out << '"' << "\n";
    }
    if(edge_cb) (*edge_cb)(out,&G,e);

/*
    if((GW.get_label_type(e) == user_label) && GW.get_label(e).length())
      out << "    label " << '"' << GW.get_label(e) << '"' << "\n";
*/
    if(GW.get_user_label(e).length())
      out << "    label " << '"' << GW.get_user_label(e) << '"' << "\n";

    int    width = GW.get_width(e);
    double thick = GW.get_thickness(e);

    if (thick == int(thick)) thick += 0.001; //integers used for width in pixel

    list<point> bends = GW.get_bends(e);

    bool has_bends = (bends.size() > 0);
    bool custom_color = (GW.get_color(e) != black);
    bool custom_style = (GW.get_style(e) != solid_edge);

    //if( (width != 1) || has_bends || custom_color || custom_style)
    {
      out << "    graphics [\n";

      // next line is for Graphlet compatibility
      out << "      type " << '"' << "line" << '"' << "\n";

      // arrow type
      out << "      arrow " << '"';
      switch (GW.get_direction(e)) {
        case undirected_edge: out << "none"; break;
        case directed_edge:   out << "last"; break;
        case redirected_edge:  out << "first"; break;
        case bidirected_edge: out << "both"; break;
        default:              out << "unknown"; break;
      }
      out << '"' << "\n";

      if(width != 1) print_double(out,"      width ",thick);
      if(custom_color) print_color(out,"      fill ",GW.get_color(e));
      if(custom_style)
      {
        out << "      style " << '"';
        gw_edge_style st = GW.get_style(e);
        switch(st)
        {
          case dashed_edge: { out << "dashed_edge"; break; }
          case dotted_edge: { out << "dotted_edge"; break; }
          default:          { out << "unknown"; break; }
        }
        out << '"' << "\n";
      }
      if(has_bends)
      {
        gw_edge_shape shape = GW.get_shape(e);
        if(shape != poly_edge)
        {
          out << "      smooth 1\n";
          out << "      shape " << '"';
          switch(shape)
          {
            case bezier_edge: { out << "bezier_edge"; break; }
            case spline_edge: { out << "spline_edge"; break; }
            case circle_edge: { out << "circle_edge"; break; }
            default:          { out << "unknown"; break; }
          }
          out << '"' << "\n";
        }
        out << "      Line [\n";
        bends.push(GW.get_position(source(e)));
        bends.append(GW.get_position(target(e)));
        point bend;
        forall(bend,bends)
        {
          out << "        point [\n";
          print_double(out,"          x ",bend.xcoord());
          print_double(out,"          y ",bend.ycoord());
          out << "        ]\n";
        }
        out << "      ]\n";
      }
      out << "    ]\n";
    }

    gw_label_type lt = GW.get_label_type(e);
    if(lt != no_label)
    {
      if((lt != user_label) || GW.get_label(e).length())
      {
        out << "    LabelGraphics [\n";
        out << "      type " << '"';
        switch(lt)
        {
          case user_label:  { out << "text"; break; }
          case index_label: { out << "index_label"; break; }
          case data_label:  { out << "data_label"; break; }
          default:          { out << "unknown"; break; }
        }
        out << '"' << "\n";
        print_color(out,"      fill ",GW.get_label_color(e));
        gw_position lp = GW.get_label_pos(e);
        out << "      anchor " << '"';
        switch(lp)
        { case central_pos:   { out << "c"; break; }
          case north_pos:     { out << "n"; break; }
          case south_pos:     { out << "s"; break; }
          case east_pos:      { out << "e"; break; }
          case west_pos:      { out << "w"; break; }
          case northwest_pos: { out << "nw"; break; }
          case northeast_pos: { out << "ne"; break; }
          case southeast_pos: { out << "se"; break; }
          case southwest_pos: { out << "sw"; break; }
          default:            { out << "unknown"; break; }
        }
        out << '"' << "\n";
        out << "    ]\n";
      }
    }

    if(   (GW.get_source_anchor(e) != point(0.0,0.0))
       || (GW.get_target_anchor(e) != point(0.0,0.0)) )
    {
      point sanc = GW.get_source_anchor(e);
      point tanc = GW.get_target_anchor(e);
      double sx = sanc.xcoord();
      double sy = sanc.ycoord();
      double tx = tanc.xcoord();
      double ty = tanc.ycoord();

      out << "    edgeAnchor [\n";
      if(sx != 0.0) print_double(out,"      xSource ",sx);
      if(sy != 0.0) print_double(out,"      ySource ",sy);
      if(tx != 0.0) print_double(out,"      xTarget ",tx);
      if(ty != 0.0) print_double(out,"      yTarget ",ty);
      out << "    ]\n";
    }

    out << "  ]\n";
  }
  out << "\n]\n";

  return !out.fail();
}

bool write_gml(GraphWin& GW, ostream& out,
               void (*node_cb)(ostream&,const GraphWin&, const node),
               void (*edge_cb)(ostream&,const GraphWin&, const edge))
// writes the graph in GW in GML format to the ostream out. The callback
// functions node_cb and edge_cb are called once while writing each
// node or edge respectively. They can be used to write additional
// fields to the GML output. The user is in charge of keeping the
// syntactical correctness of the GML output when using the callbacks.
{

  const graph& G = GW.get_graph();

  out << "Creator "  << '"' << "LEDA GraphWin" << '"' << endl;
  out << "Version "  << GraphWin::version() << endl;
  out << "graph [\n" << endl;
  out << "  label "  << '"' << '"' << endl;
  out << "  directed " << int(GW.get_directed()) << "\n\n";

  bool write_node_parameters = (string(G.node_type()) != "void");
  bool write_edge_parameters = (string(G.edge_type()) != "void");

  node v;
  forall_nodes(v,G)
  {
    out << "  node [\n";
    out << "    id " << G.index(v) << "\n";
    if(write_node_parameters)
    {
      out << "    parameter " << '"' << G.get_node_entry_string(v);
      out << '"' << "\n";
    }
    if(node_cb) (*node_cb)(out,GW,v);

/*
    if((GW.get_label_type(v) == user_label) && GW.get_label(v).length())
      out << "    label " << '"' << GW.get_label(v) << '"' << "\n";
*/

    string s = GW.get_user_label(v);

    if(s.length())
      out << "    label " << '"' << s.replace_all("\\n","\n") << '"' << "\n";

    gw_position lp = GW.get_label_pos(v);

  //if(lp != central_pos)
    { out << "      labelAnchor " << '"';
      print_position(out,lp);
      out << '"' << "\n";
    }

    out << "    graphics [\n";
    point pos = GW.get_position(v);
    print_double(out,"      x ",pos.xcoord());
    print_double(out,"      y ",pos.ycoord());
    print_double(out,"      w ",2*GW.get_radius1(v));
    print_double(out,"      h ",2*GW.get_radius2(v));
 
    out << "      type " << '"';
    if (GW.get_shape(v) == rectangle_node || GW.get_shape(v) == square_node) 
      out << "rectangle";
    else 
      out << "oval";
    out << '"' << endl;

    print_double(out,"      width ",   2*GW.get_border_thickness(v));
    print_color(out, "      fill ",    GW.get_color(v));
    print_color(out, "      outline ", GW.get_border_color(v));

    out << "    ]\n";

    gw_label_type lt = GW.get_label_type(v);
    if(lt != no_label)
    {
      string s;
      if (lt & index_label) s = "index";

      switch (lt & ~index_label)
      { case user_label:  { s = /* s + */ "text"; break; }
        case data_label:  { s = /* s + */ "data"; break; }
        case no_label:    break;
        default:          { s = "unknown"; break; }
      }

      if (s != "text") s = s + "_label";

      out << "    LabelGraphics [\n";
      out << "      type " << '"' << s << '"' << "\n";

      color lcol = GW.get_label_color(v);
      if (lcol == gw_auto_color) lcol = text_color(lcol);
      print_color(out,"      fill ",(int)lcol);

      gw_position lp = GW.get_label_pos(v);

      //if(lp != central_pos)
      { out << "      anchor " << '"';
        print_position(out,lp);
        out << '"' << "\n";
       }
      out << "    ]\n";
    }
    out << "  ]\n";
  }
  out << "\n\n";

  edge e;
  forall_edges(e,G)
  {
    out << "  edge [\n";
    out << "    source " << G.index(source(e)) << "\n";
    out << "    target " << G.index(target(e)) << "\n";
    if(write_edge_parameters)
    { out << "    parameter " << '"' << G.get_edge_entry_string(e);
      out << '"' << "\n";
    }
    if(edge_cb) (*edge_cb)(out,GW,e);

/*
    if((GW.get_label_type(e) == user_label) && GW.get_label(e).length())
      out << "    label " << '"' << GW.get_label(e) << '"' << "\n";
*/
    if(GW.get_user_label(e).length())
      out << "    label " << '"' << GW.get_user_label(e) << '"' << "\n";

    int    width = GW.get_width(e);
    double thick = GW.get_thickness(e);

    if (thick == int(thick)) thick += 0.001; //integers used for width in pixel

    list<point> bends = GW.get_bends(e);

    bool has_bends = (bends.size() > 0);
    bool custom_color = (GW.get_color(e) != black);
    bool custom_style = (GW.get_style(e) != solid_edge);

    //if( (width != 1) || has_bends || custom_color || custom_style)
    {
      out << "    graphics [\n";

      // next line is for Graphlet compatibility
      out << "      type " << '"' << "line" << '"' << "\n";

      // arrow type
      out << "      arrow " << '"';
      switch (GW.get_direction(e)) {
        case undirected_edge: out << "none"; break;
        case directed_edge:   out << "last"; break;
        case redirected_edge:  out << "first"; break;
        case bidirected_edge: out << "both"; break;
        default:              out << "unknown"; break;
      }
      out << '"' << "\n";

      if(width != 1) print_double(out,"      width ",thick);
      if(custom_color) print_color(out,"      fill ",GW.get_color(e));
      if(custom_style)
      {
        out << "      style " << '"';
        gw_edge_style st = GW.get_style(e);
        switch(st)
        {
          case dashed_edge: { out << "dashed_edge"; break; }
          case dotted_edge: { out << "dotted_edge"; break; }
          default:          { out << "unknown"; break; }
        }
        out << '"' << "\n";
      }
      if(has_bends)
      {
        gw_edge_shape shape = GW.get_shape(e);
        if(shape != poly_edge)
        {
          out << "      smooth 1\n";
          out << "      shape " << '"';
          switch(shape)
          {
            case bezier_edge: { out << "bezier_edge"; break; }
            case spline_edge: { out << "spline_edge"; break; }
            case circle_edge: { out << "circle_edge"; break; }
            default:          { out << "unknown"; break; }
          }
          out << '"' << "\n";
        }
        out << "      Line [\n";
        bends.push(GW.get_position(source(e)));
        bends.append(GW.get_position(target(e)));
        point bend;
        forall(bend,bends)
        {
          out << "        point [\n";
          print_double(out,"          x ",bend.xcoord());
          print_double(out,"          y ",bend.ycoord());
          out << "        ]\n";
        }
        out << "      ]\n";
      }
      out << "    ]\n";
    }

    gw_label_type lt = GW.get_label_type(e);
    if(lt != no_label)
    {
      if((lt != user_label) || GW.get_label(e).length())
      {
        out << "    LabelGraphics [\n";
        out << "      type " << '"';
        switch(lt)
        {
          case user_label:  { out << "text"; break; }
          case index_label: { out << "index_label"; break; }
          case data_label:  { out << "data_label"; break; }
          default:          { out << "unknown"; break; }
        }
        out << '"' << "\n";
        print_color(out,"      fill ",GW.get_label_color(e));
        gw_position lp = GW.get_label_pos(e);
        out << "      anchor " << '"';
        switch(lp)
        { case central_pos:   { out << "c"; break; }
          case north_pos:     { out << "n"; break; }
          case south_pos:     { out << "s"; break; }
          case east_pos:      { out << "e"; break; }
          case west_pos:      { out << "w"; break; }
          case northwest_pos: { out << "nw"; break; }
          case northeast_pos: { out << "ne"; break; }
          case southeast_pos: { out << "se"; break; }
          case southwest_pos: { out << "sw"; break; }
          default:            { out << "unknown"; break; }
        }
        out << '"' << "\n";
        out << "    ]\n";
      }
    }

    if(   (GW.get_source_anchor(e) != point(0.0,0.0))
       || (GW.get_target_anchor(e) != point(0.0,0.0)) )
    {
      point sanc = GW.get_source_anchor(e);
      point tanc = GW.get_target_anchor(e);
      double sx = sanc.xcoord();
      double sy = sanc.ycoord();
      double tx = tanc.xcoord();
      double ty = tanc.ycoord();

      out << "    edgeAnchor [\n";
      if(sx != 0.0) print_double(out,"      xSource ",sx);
      if(sy != 0.0) print_double(out,"      ySource ",sy);
      if(tx != 0.0) print_double(out,"      xTarget ",tx);
      if(ty != 0.0) print_double(out,"      yTarget ",ty);
      out << "    ]\n";
    }

    out << "  ]\n";
  }
  out << "\n]\n";

  return !out.fail();
}

LEDA_END_NAMESPACE
