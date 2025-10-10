/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ps_file.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/ps_file.h>
#include <LEDA/system/file.h>
#include <LEDA/geo/float_geo_alg.h>

#include <stdlib.h>
#include <time.h>

/*
#include <sys/param.h>
*/

#if !defined(EXIT_FAILURE)
#define EXIT_FAILURE 1
#endif


LEDA_BEGIN_NAMESPACE

ps_file& ps_file::operator<<(const char* str) {
  file << str;
  return *this;
}

ps_file& ps_file::operator<<(double d) {
  file << string("%f ",d);
  return *this;
}

ps_file& ps_file::operator<<(int i) {
  file << string("%d ",i);
  return *this;
}


// Static members of class ps_file:
const string ps_file::VERSION        ="PS-Adobe-2.0";
const short  ps_file::X_SHIFT        =60;
const short  ps_file::Y_SHIFT        =100;
#ifndef PI
const double ps_file::PI             =3.14159265358979323846;
#endif
const double ps_file::SIZE           =100.0;
const string ps_file::DEFAULT_FONT   ="Helvetica";
const double ps_file::DEFAULT_FACTOR =5.0/3.57;

const int ps_file::FG_COLOR = -1;

// Auxiliary functions

void ps_file::scale_xcoord(double& x) {
  x=(x-X0)*scaling; }

void ps_file::scale_xcoord(double& x1,double& x2) {
  scale_xcoord(x1);scale_xcoord(x2); }

void ps_file::scale_ycoord(double& y) {
  y=(y-Y0)*scaling; }

void ps_file::scale_ycoord(double& y1,double& y2) {
  scale_ycoord(y1);scale_ycoord(y2); }

void ps_file::scale_radius(double& r) {
  r*=scaling; }

list_item ps_file::grayvalue_item(color c) {
  list_item gvi;
  forall_items(gvi,grayvalue_list)
    if (grayvalue_list.inf(gvi).c==c) return gvi;
  return NULL; }

inline int ps_file::round(double d) {
  return int(d+0.5); }

inline bool ps_file::exor(bool a, bool b) {
  return ((a && !b) || (b && !a));
}

inline double ps_file::dist(double x1, double y1, double x2, double y2) {
  return hypot(fabs(x1-x2),fabs(y1-y2)); }

inline double ps_file::dist(point p1, point p2) {
  return dist(p1.xcoord(),p1.ycoord(),p2.xcoord(),p2.ycoord()); }

// Constructors, destructors, initializers
ps_file::ps_file(double w, double h, string name):
  filename(name), file(name), width(w), height(h) {
  if (!file) {
    cerr << "file name error: can't write to " << name << endl;
    cerr << "exiting\n";
    exit(EXIT_FAILURE); }
  initialize(); }

ps_file::ps_file(string name):
  filename(name), file(name), width(PS_WIDTH), height(PS_HEIGHT) {
  initialize(); }

void ps_file::initialize() {
  bbx=width/PS_PIXEL;bby=height/PS_PIXEL;
  must_scale=true;
  page=1;
  set_defaults();
  
  // needed for color initialization ...
  color dummy_cl(0,0,0);
/*
  dummy_cl.set_red(0);
*/

  time_t t;
  time(&t);

  file << "%!" << VERSION << endl;
  file << "%%Creator: LEDA-PS-Converter\n";
  file << "%%Title: " << filename << endl;
  file << "%%CreationDate: " << ctime(&t);
  file << "%%Pages: (atend)\n";
  file << "%%DocumentFonts: " << textfont << endl;
  file << "%%BoundingBox: " << X_SHIFT            << " " << Y_SHIFT << " "
                            << X_SHIFT+round(bbx) << " " << Y_SHIFT+round(bby) << endl;
  file << "%%EndComments\n\n";

  file << "% Abbreviations\n";
  file << "/BB {\n";
  file << "  newpath\n";
  file << "  " << X_SHIFT     << " " << Y_SHIFT     << " moveto\n";
  file << "  " << X_SHIFT+bbx << " " << Y_SHIFT     << " lineto\n";
  file << "  " << X_SHIFT+bbx << " " << Y_SHIFT+bby << " lineto\n";
  file << "  " << X_SHIFT     << " " << Y_SHIFT+bby << " lineto\n";
  file << "  closepath } def\n";
  file << "/dp {\n";
  file << "  newpath\n";
  file << "  moveto\n";
  file << "  closepath\n";
  file << "  1 setlinecap\n";
  file << "  stroke\n";
  file << "  0 setlinecap } def\n";
  file << "/ds {\n";
  file << "  newpath\n";
  file << "  moveto\n";
  file << "  lineto\n";
  file << "  stroke } def\n";
  file << "/dc {\n";
  file << "  newpath\n";
  file << "  0 360 arc\n";
  file << "  stroke } def\n";
  file << "/dd {\n";
  file << "  newpath\n";
  file << "  0 360 arc\n";
  file << "  fill } def\n";
  file << "/cg   { currentgray } def\n";
  file << "/cgsg { currentgray setgray } def\n";
  file << "/chp  { charpath } def\n";
  file << "/cp   { closepath } def\n";
  file << "/cpfi { closepath fill } def\n";
  file << "/cpst { closepath stroke } def\n";
  file << "/ff   { findfont } def\n";
  file << "/gr   { grestore } def\n";
  file << "/gs   { gsave } def\n";
  file << "/lt   { lineto } def\n";
  file << "/mt   { moveto } def\n";
  file << "/np   { newpath } def\n";
  file << "/sc   { scale } def\n";
  file << "/scf  { scalefont } def\n";
  file << "/sd   { setdash } def\n";
  file << "/sf   { setfont } def\n";
  file << "/sg   { setgray } def\n";
  file << "/slc  { setlinecap } def\n";
  file << "/slw  { setlinewidth } def\n";
  file << "/sp   { showpage } def\n";
  file << "/spds { stringwidth pop 2 div sub } def\n";
  file << "/srgb { setrgbcolor } def\n";
  file << "/st   { stroke } def\n";
  file << "/sw   { stringwidth } def\n";
  file << "/tl   { translate } def\n\n";

  // from graphwin ...
  
  file << "/LEDA_dict 100 dict def" << endl;
  file << "LEDA_dict begin" << endl;

  file << "%%" << endl;
  file << "%% FONTS" << endl;
  file << "/font_dict 2 dict def" << endl;
  file << "/set_font {" << endl;
  file << "  font_dict" << endl;
  file << "  begin" << endl;
  file << "    /font_size exch def /font_name exch def" << endl;
  file << "    font_name cvn findfont font_size scalefont setfont" << endl;
  file << "  end" << endl;
  file << "} def" << endl;

  file << "%%" << endl;
  file << "%% POLYGONS" << endl;
  file << "/polygon_dict 1 dict def" << endl;
  file << "/poly_line {" << endl;
  file << "  polygon_dict" << endl;
  file << "  begin" << endl;
  file << "    /points exch def" << endl;
  file << "    points 2 mul dup 2 sub roll" << endl;
  file << "    moveto" << endl;
  file << "    points 1 sub" << endl;
  file << "    dup { dup 2 mul 1 add -2 roll lineto 1 sub} repeat" << endl;
  file << "    pop" << endl;
  file << "  end" << endl;
  file << "} def" << endl;
  file << "/polygon { newpath poly_line closepath } def" << endl;


  file << "%%" << endl;
  file << "%% Round Rectangles" << endl;
  file << "/rrect {" << endl;
  file << "  5 dict" << endl;
  file << "  begin" << endl;
  file << "   newpath" << endl;
  file << "   /r exch def /y1 exch def /x1 exch def /y0 exch def /x0 exch def" << endl;
  file << "   x0 r add y0 moveto" << endl;
  file << "   x1 y0 x1 y1 r arct" << endl;
  file << "   x1 y1 x0 y1 r arct" << endl;
  file << "   x0 y1 x0 y0 r arct" << endl;
  file << "   x0 y0 x1 y0 r arct" << endl;
  file << "   closepath" << endl;
  file << "  end" << endl;
  file << "} def" << endl;


  file << "%%" << endl;
  file << "%% ELLIPSES" << endl;
  file << "/ellipse_dict 4 dict def" << endl;
  file << "/ellipse {" << endl;
  file << "  ellipse_dict" << endl;
  file << "  begin" << endl;
  file << "    /yrad exch def /xrad exch def /ypos exch def /xpos exch def" << endl;
  file << "    matrix currentmatrix" << endl;
  file << "    xpos ypos translate" << endl;
  file << "    xrad yrad scale" << endl;
  file << "    0 0 1 0 360 arc" << endl;
  file << "    setmatrix" << endl;
  file << "  end" << endl;
  file << "} def" << endl;

  file << "%%" << endl;
  file << "%% hatching" << endl;
  file << "/hatch1_dict 4 dict def" << endl;
  file << "/hatch1_rect {" << endl;
  file << " hatch1_dict" << endl;
  file << " begin" << endl;
  file << "  0.25 units setlinewidth" << endl;
  file << "  /ymax exch def /xmax exch def /ymin exch def /xmin exch def" << endl;
  file << "  xmin ymax ymin sub sub 3 units xmax " << endl;
  file << "  { ymin moveto ymax ymin sub dup rlineto stroke } for" << endl;
  file << " end" << endl;
  file << "} def" << endl;
  file << "%" << endl;
  file << "/hatch2_dict 4 dict def" << endl;
  file << "/hatch2_rect {" << endl;
  file << " hatch2_dict" << endl;
  file << " begin" << endl;
  file << "  0.25 units setlinewidth" << endl;
  file << "  /ymax exch def /xmax exch def /ymin exch def /xmin exch def" << endl;
  file << "  xmax ymax ymin sub add -3 units xmin " << endl;
  file << "  { ymin moveto ymin ymax sub ymax ymin sub rlineto stroke } for" << endl;
  file << " end"  << endl;
  file << "} def" << endl;

  file << "%%" << endl;
  file << "%% TEXT" << endl;
  file << "/draw_text {" << endl;
  file << "  3 dict begin" << endl;
  file << "    /label exch def /ypos exch def /xpos exch def" << endl;
  file << "    xpos ypos moveto" << endl;
  file << "    label show" << endl;
  file << "  end" << endl;
  file << "} def" << endl;

  file << "%%" << endl;
  file << "/draw_ctext {" << endl;
  file << "  8 dict begin" << endl;
  file << "   /sz exch def /label exch def /ypos exch def /xpos exch def" << endl;
  file << "   xpos ypos moveto" << endl;
  file << "   label stringwidth pop -2 div sz -0.32 mul rmoveto" << endl;

  file << "   label show" << endl;
  file << "  end" << endl;
  file << "} def" << endl;

  file << "%%" << endl;
  file << "%% PIXMAPS" << endl;
  file << "/draw_pixmap {" << endl;
  file << " 10 dict begin" << endl;
  file << "   /ly exch def /lx exch def /h exch def /w exch def /y exch def /x exch def" << endl;
  file << "   /pix w 3 mul string def" << endl;
  file << "   x y translate" << endl;
  file << "   lx ly scale" << endl;
  file << "   w h 8 [w 0 0  0 h sub  0 h]" << endl;
  file << "   {currentfile pix readhexstring pop}" << endl;
  file << "   false 3 colorimage" << endl;
  file << " end" << endl;
  file << "} def" << endl;

  file << "%%" << endl;
  file << "%% LINESTYLES" << endl;
  file << "/units  { f_scale div } def" << endl;
  file << "/solid  { [ ] } def" << endl;
  file << "/dashed { [ 3 units 2 units] } def" << endl;
  file << "/dotted { [ 0.5 units 2 units ] } def" << endl;
  file << "/dashed_dotted { [ 3 units 2 units 0.5 units 2 units ] } def" << endl;
  file << "/set_line_style { 0 setdash } def" << endl;

  file << "%%" << endl;
  file << "%% INIT" << endl;
  /*
  file << "1 1 scale" << endl;
  file << "1 setlinewidth" << endl;
  file << "(Helvetica) 12.0 set_font" << endl;
  file << "%%EndProlog" << endl;
   */

  file << "%%" << endl;
  file << "%%BeginProgram" << endl;
  // end from graphwin
  
  new_font();
  file << "BB clip\n\n";
  file << "%%EndProlog\n\n";
  file << "%%Page: 1 1\n";
  file << "gs\n";
  //if (X_SHIFT || Y_SHIFT) 
  file << X_SHIFT << " " << Y_SHIFT << " tl\n";
  init(0.0,SIZE,0.0); }


void ps_file::set_defaults() {
  draw_bb=true;
  outputmode=colored_mode;
  fg_col=black;
  fill_color=invisible;
  linewidth=1;
  unitlinewidth=PS_PIXEL;
  linestyle=solid;
  pt_style=cross_point;
  nodewidth=20;
  crosswidth=0.2;
  arrowlength=0.5;
  arrowangle=PI/10;
  textfont=DEFAULT_FONT;
  fontsize=0.4;
  textmode=transparent; }

// When a new page is started, don't take defaults, but the old settings
// on the last page:
void ps_file::set_old_values() {
  if (fg_col!=black || outputmode==gray_mode) change_color(fg_col);
  if (unitlinewidth/PS_PIXEL*linewidth!=1.0) file << unitlinewidth/PS_PIXEL*linewidth << " slw\n";
  if (linestyle!=solid) {
    int on,off;
    int lw=round(unitlinewidth/PS_PIXEL*linewidth);
    switch(linestyle) {
      case solid : break;
      case dashed: on =leda_max(lw*2,12);
		   off=leda_max(lw,6);
		   file << "[" << on << " " << off << "] 0";
		   break;
      case dotted: on =leda_max(lw,1);
		   off=leda_max(on,6);
		   file << "[" << on << " " << off << "] 0"; 
                   break;
      default:     break;
    }

    file << " sd\n"; }
  /* Reinstall old textmode? */
  new_font(); }

void ps_file::init(double x0, double x1, double y0) {
  X0=x0;X1=x1;Y0=y0;
  Y1=Y0+height*(X1-X0)/width;
  scaling=bbx/(X1-X0);  /*  =bby/(Y1-Y0) */ }

void ps_file::newpage() {
  finish_old_page();
  page++;
  file << "%%Page: " << page << " " << page << "\n";
  file << "gs\n";
  //if (X_SHIFT || Y_SHIFT) 
  file << X_SHIFT << " " << Y_SHIFT << " tl\n";
  set_old_values(); }

void ps_file::finish_old_page() {
  if (draw_bb) file << "gr gs BB st\n";
  file << "sp gr\n\n"; }

void ps_file::newfile(double w, double h, string name) {
  if (page!=0) {       // User didn't close the old file
    cout << "closing current file: ";
    close(); }
  filename=name;
  file.open(name);
  width=w, height=h;
  if (!file) {
    cerr << "file name error: can't write to " << name << endl;
    cerr << "exiting\n";
    exit(EXIT_FAILURE); }
  initialize(); }

void ps_file::newfile(string name) {
  if (page!=0) {       // User didn't close the old file
    cout << "closing current file: ";
    close(); }
  filename=name;
  file.open(name);
  width=PS_WIDTH, height=PS_HEIGHT;
  if (!file) {
    cerr << "file name error: can't write to " << name << endl;
    cerr << "exiting\n";
    exit(EXIT_FAILURE); }
  initialize(); }

void ps_file::close() {
  finish_old_page();
  file << "%%Trailer\n";
  file << "%%Pages: " << page << endl;
  file.close();
  page=0;   // To recognize whether close has been called or not
  cout << "file " << filename << " has been created in ";
  cout.flush();
  if (system("pwd") & 0xFF)
	  cout << " the current directory" << endl;
}

ps_file::~ps_file() {
  if (page!=0) {       // User didn't close the last file
    cout << "closing current file: ";
    close(); }}


// Setting parameters

bool ps_file::set_draw_bb(bool flag) {
  bool temp=draw_bb;
  draw_bb=flag;
  return temp; }


output_mode ps_file::set_output_mode(output_mode new_mode) {
  output_mode temp=outputmode;
  if (exor(new_mode==colored_mode,outputmode==colored_mode)) {
  //if (exor(new_mode,outputmode)) {
    outputmode=new_mode;
    change_color(fg_col); }
  return temp; }

double ps_file::set_gray_value(color c,double d) {
  if ((int)c<0) {
    cerr << "file " << filename << ", page " << page
         << ": Error in ps_file::set_gray_value(color,double):\n"
         << "color index must be non-negative\n";
    cerr << "exiting\n";
    exit(EXIT_FAILURE); }
  if (d<0 || d>1) {
    cerr << "file " << filename << ", page " << page
         << ": Error in ps_file::set_gray_value(color,double):\n"
         << "grayvalue must be in 0..1\n";
    cerr << "exiting\n";
    exit(EXIT_FAILURE); }
  double temp;
  list_item gvi=grayvalue_item(c);
  if (gvi) {
    temp=grayvalue_list.inf(gvi).grayvalue;
    grayvalue_list[gvi].grayvalue=d; }
  else {
    temp=-1;
    grayvalue_list.append(ps_file_pair(c,d)); }
  if (c==fg_col && outputmode==gray_mode) change_color(fg_col);
  return temp; }

color ps_file::set_color(color c) {
  color temp=fg_col;
  if (c!=fg_col) {
    fg_col=c;
    change_color(c); }
  return temp; }
  
color ps_file::set_fill_color(color c) {
  color temp=fill_color;
  fill_color=c;
  return temp; 
}

// This function is used internally to temporarily change the
// foreground color if a color is specified in drawing operations:
void ps_file::change_color(color c) {
  if (outputmode==colored_mode) change_rgb(c);
  else {
    list_item gvi=grayvalue_item(c);
    if (gvi) file << grayvalue_list.inf(gvi).grayvalue << " sg\n";
    else {
      change_rgb(c);
      file << "cgsg\n"; }}}

// PostScript uses a [0,1] range for rgb-values instead of [0,255]:
void ps_file::change_rgb(color c) {
  int r,g,b;
  c.get_rgb(r,g,b);
  file << double(r)/255.0 << " "
       << double(g)/255.0 << " "
       << double(b)/255.0 << " srgb\n"; }

double ps_file::set_line_width(double pix) {
  double temp=linewidth;
  if (pix!=linewidth) {
    linewidth=pix;
    file << unitlinewidth/PS_PIXEL*linewidth << " slw\n";
    change_line_style(linestyle); }
  return temp; }

// cm specifications are converted into pixels:
double ps_file::set_unit_line_width(double cm) {
  double temp=unitlinewidth;
  if (cm!=unitlinewidth) {
    unitlinewidth=cm;
    file << unitlinewidth/PS_PIXEL*linewidth << " slw\n";
    change_line_style(linestyle); }
  return temp; }
  
point_style ps_file::set_point_style(point_style p)
{
  point_style temp=pt_style;
  pt_style=p;
  return temp;
}

line_style ps_file::set_line_style(line_style s) {
  line_style temp=linestyle;
  if (s!=linestyle) change_line_style(s);
  return temp; }

void ps_file::change_line_style(line_style s) {
  linestyle=s;
  int on,off;
  int lw=round(unitlinewidth/PS_PIXEL*linewidth);
  switch(s) {
    case solid : file << "[ ]   0";
		 break;
    case dashed: on =leda_max(lw*2,12);
		 off=leda_max(lw,6);
		 file << "[" << on << " " << off << "] 0";
		 break;
    case dotted: on =leda_max(lw,1);
		 off=leda_max(on,6);
		 file << "[" << on << " " << off << "] 0"; 
                 break;
    default:     break;
   }
  file << " sd\n"; }

double ps_file::set_node_width(double pix) {
  double temp=nodewidth;
  nodewidth=pix;
  return temp; }

double ps_file::set_cross_width(double cm) {
  double temp=crosswidth;
  crosswidth=cm;
  return temp; }

double ps_file::set_arrow_length(double cm) {
  double temp=arrowlength;
  arrowlength=cm;
  return temp; }

double ps_file::set_arrow_angle(double angle) {
  double temp=arrowangle;
  arrowangle=angle;
  return temp; }

string ps_file::set_text_font(string s) {
  string temp=textfont;
  textfont=s;
  new_font();
  return temp; }

double ps_file::set_font_size(double fs) {
  double temp=fontsize;
  fontsize=fs;
  new_font();
  return temp; }

// Whenever a new font is set or the size of the current font is
// changed, the PostScript 'setfont' operation must be called. The
// size of a font is computed according to the size of the letter
// 'M'. This size is different for different PostScript fonts, even
// for the same fontsize. The 'fontfactor' tries to compensate these
// differences:
void ps_file::new_font() {
  double fontfactor;
  if (textfont(0,4)=="Arial") fontfactor=5.0/3.3; // ??????
  else
  if (textfont(0,4)=="Times") fontfactor=5.0/3.3;
  else
    if (textfont(0,8)=="Helvetica") fontfactor=5.0/3.57;
    else
      if (textfont(0,6)=="Courier") fontfactor=5.0/2.91;
      else
	if (textfont(0,5)=="Symbol") fontfactor=5.0/3.3;
	else {
	  cerr << "file " << filename << ", page " << page
	       << ": Warning: " << textfont << ": unknown font; using "
	       << DEFAULT_FONT  << " instead\n";
	  file << "% Warning: " << textfont << ": unknown font; using "
	       << DEFAULT_FONT  << " instead\n";
	  textfont=DEFAULT_FONT;
	  fontfactor=DEFAULT_FACTOR; }
  file << "/" << textfont << " ff " << fontsize/PS_PIXEL*fontfactor << " scf sf\n"; }

text_mode ps_file::set_text_mode(text_mode) {
  text_mode temp=textmode;
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::set_text_mode not implemented\n";
  file << "% Warning: ps_file::set_text_mode not implemented\n";
  return temp; }


// Reading parameters

bool        ps_file::get_draw_bb    () {return draw_bb; }
output_mode ps_file::get_output_mode() {return outputmode; }
double      ps_file::get_gray_value (color c) {
  if ((int)c<0) {
    cerr << "file " << filename << ", page " << page
         << ": Error in ps_file::get_gray_value(color):\n"
         << "color index must be non-negative\n";
    cerr << "exiting\n";
    exit(EXIT_FAILURE); }
  list_item gvi=grayvalue_item(c);
  if (gvi) return grayvalue_list.inf(gvi).grayvalue;
  else return -1; }
color         ps_file::get_color           () {return fg_col;        }
color         ps_file::get_fill_color      () {return fill_color;    }
double      ps_file::get_line_width      () {return linewidth;     }
double      ps_file::get_unit_line_width () {return unitlinewidth; }
line_style    ps_file::get_line_style      () {return linestyle;     }
point_style   ps_file::get_point_style     () {return pt_style;      }
double      ps_file::get_node_width      () {return nodewidth;     }
double      ps_file::get_cross_width     () {return crosswidth;    }
double      ps_file::get_arrow_length    () {return arrowlength;   }
double      ps_file::get_arrow_angle     () {return arrowangle;    }
string        ps_file::get_text_font       () {return textfont;      }
double      ps_file::get_font_size       () {return fontsize;      }
text_mode     ps_file::get_text_mode       () {return textmode;      }

// Drawing operations
// Drawing points
void ps_file::draw_point(double x, double y, color c) {
  if (c == invisible) return;
  scale_xcoord(x);scale_ycoord(y);
  must_scale=false;
  
  double ws = set_line_width(1);
  line_style ls = set_line_style(solid);

  switch (pt_style) {

   case pixel_point: {
                        draw_pixel(x,y,c);
                        break;
		      }

   case cross_point: {
                       double x0=x-crosswidth/PS_PIXEL/2.0;double y0=x0+y-x;
                       double x1=x+crosswidth/PS_PIXEL/2.0;double y1=x1+y-x;
                       draw_segment(x0,y0,x1,y1,c);
                       y0=-x0+y+x;y1=-x1+y+x;
                       draw_segment(x0,y0,x1,y1,c);
                       break;
		      }

   case plus_point:  {
                       double x0=x-crosswidth/PS_PIXEL/2.0;double y0=x0+y-x;
                       double x1=x+crosswidth/PS_PIXEL/2.0;double y1=x1+y-x;
                       draw_segment(x,y0,x,y1,c);
                       draw_segment(x0,y,x1,y,c);   
                       break;
		      }

   case circle_point: {
                        draw_disc(x,y,3,get_fill_color());
                        draw_circle(x,y,3);
                        break;
		       }

   case rect_point: {
                       draw_box(x-2,y-2,x+2,y+2,get_fill_color());
                       draw_rectangle(x-2,y-2,x+2,y+2);
                       break;
		      }

   case disc_point: {
                      draw_disc(x,y,3);
                      break;
		     }

   case box_point:  {
                      draw_box(x-2,y-2,x+2,y+2);
                      break;
		     }

  }
  
  set_line_width(ws);
  set_line_style(ls);
  must_scale=true; 
}  
  
void ps_file::draw_point(point p, color c) {draw_point(p.xcoord(),p.ycoord(),c); }

void ps_file::draw_pixel(double x, double y, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << x << " " << y << " dp\n";
  if (c!=fg_col) change_color(fg_col); 
}

void ps_file::draw_pixel(point p, color c) {
  draw_pixel(p.xcoord(),p.ycoord(),c); 
}

// Drawing line segments
void ps_file::draw_segment(double x1, double y1, double x2, double y2, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) {scale_xcoord(x1,x2);scale_ycoord(y1,y2); }
  file << x1 << " " << y1 << " " << x2 << " " << y2 << " ds\n";
  if (c!=fg_col) change_color(fg_col); }
void ps_file::draw_segment(point p, point q, color c) {
  draw_segment(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),c); }
void ps_file::draw_segment(segment s, color c) {
  draw_segment(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),c); }

// Drawing lines
void ps_file::draw_line(double x1, double y1, double x2, double y2, color c) {
  if (c == invisible) return;
  if (x1==x2) draw_segment(x1,Y0,x2,Y1,c);
  else {
    double delta=(y2-y1)/(x2-x1);
    draw_segment(X0,delta*(X0-x1)+y1,X1,delta*(X1-x1)+y1,c); }}
void ps_file::draw_line(point p, point q, color c) {
  draw_line(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),c); }
void ps_file::draw_line(segment s, color c) {
  draw_line(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),c); }
void ps_file::draw_line(line l, color c) {
  if (l.is_horizontal()) draw_hline(l.y_proj(X0),c);
  else
    if (l.is_vertical()) draw_vline(l.x_proj(Y0),c);
    else draw_line(X0,l.y_proj(X0),X1,l.y_proj(X1),c); }
void ps_file::draw_hline(double y, color c) {draw_line(X0,y,X1,y,c); }
void ps_file::draw_vline(double x, color c) {draw_line(x,Y0,x,Y1,c); }

void ps_file::draw_arc(double, double, double, double, double, color) {
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::draw_arc not implemented\n";
  file << "% Warning: ps_file::draw_arc not implemented\n"; }
void ps_file::draw_arc(point p, point q, double r, color c) {
  draw_arc(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),r,c); }
void ps_file::draw_arc(segment s, double r, color c) {
  draw_arc(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),r,c); }

// Drawing arrows
void ps_file::draw_arrow_head(point p, double dir, color c) {
  if (c == invisible) return;
  segment s1(p,PI+dir+arrowangle,arrowlength/PS_PIXEL/scaling);
  segment s2(p,PI+dir-arrowangle,arrowlength/PS_PIXEL/scaling);
  draw_segment(s1,c);
  draw_segment(s2,c); }
void ps_file::draw_arrow(double x1, double y1, double x2, double y2, color c) {
  draw_arrow(segment(x1,y1,x2,y2),c); }
void ps_file::draw_arrow(point p, point q, color c) {
  draw_arrow(segment(p,q),c); }
void ps_file::draw_arrow(segment s, color c) {
  draw_segment(s,c);
  draw_arrow_head(s.target(),s.direction(),c); }
  
void ps_file::draw_arc_arrow(double, double, double, double, double, color) {
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::draw_arc_arrow not implemented\n";
  file << "% Warning: ps_file::draw_arc_arrow not implemented\n"; }
void ps_file::draw_arc_arrow(point p, point q, double r, color c) {
  draw_arc_arrow(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),r,c); }
void ps_file::draw_arc_arrow(segment s, double r, color c) {
  draw_arc_arrow(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),r,c); }

// Drawing circles
void ps_file::draw_circle(double x, double y, double r, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y);scale_radius(r); }
  file << x << " " << y << " " << r << " dc\n";
  if (c!=fg_col) change_color(fg_col); }
void ps_file::draw_circle(point p, double r, color c) {
  draw_circle(p.xcoord(),p.ycoord(),r,c); }
void ps_file::draw_circle(circle C, color c) {
  draw_circle(C.center().xcoord(),C.center().ycoord(),C.radius(),c); }
void ps_file::draw_ellipse(double x, double y, double r1, double r2, color c) {
  if (c == invisible) return;
  file << "gs\n";
  file << r1/r2 << " 1 sc\n";
  scale_xcoord(x);x*=r2/r1;scale_ycoord(y);scale_radius(r2);
  must_scale=false;
  draw_circle(x,y,r2,c);
  must_scale=true;
  file << "gr\n"; }
void ps_file::draw_ellipse(point p, double r1, double r2, color c) {
  draw_ellipse(p.xcoord(),p.ycoord(),r1,r2,c); }

// Drawing discs
void ps_file::draw_disc(double x, double y, double r, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y);scale_radius(r); }
  file << x << " " << y << " " << r << " dd\n";
  if (c!=fg_col) change_color(fg_col); }
void ps_file::draw_disc(point p, double r, color c) {
  draw_disc(p.xcoord(),p.ycoord(),r,c); }
void ps_file::draw_disc(circle C, color c) {
  draw_disc(C.center().xcoord(),C.center().ycoord(),C.radius(),c); }
  
void ps_file::draw_filled_ellipse(double x, double y, double r1, double r2, color c) {
  if (c == invisible) return;
  file << "gs\n";
  file << r1/r2 << " 1 sc\n";
  scale_xcoord(x);x*=r2/r1;scale_ycoord(y);scale_radius(r2);
  must_scale=false;
  draw_disc(x,y,r2,c);
  must_scale=true;
  file << "gr\n"; }
void ps_file::draw_filled_ellipse(point p, double r1, double r2, color c) {
  draw_filled_ellipse(p.xcoord(),p.ycoord(),r1,r2,c); }

// Drawing polygons
void ps_file::draw_polygon(const list<point>& lp, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  point p;
  double x=lp.head().xcoord(),y=lp.head().ycoord();
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << "np\n";
  file << x << " " << y << " mt\n";
  for(int i=1;i<lp.length();i++) {
    p=lp.contents(lp.get_item(i));
    x=p.xcoord();y=p.ycoord();
    if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
    file << x << " " << y << " lt\n"; }
  file << "cpst\n";
  if (c!=fg_col) change_color(fg_col); }

void ps_file::draw_polygon(polygon P, color c) {
  draw_polygon(P.vertices(),c); }

void ps_file::draw_filled_polygon(const list<point>& lp, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  point p;
  double x=lp.head().xcoord(),y=lp.head().ycoord();
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << "np\n";
  file << x << " " << y << " mt\n";
  for(int i=1;i<lp.length();i++) {
    p=lp.contents(lp.get_item(i));
    x=p.xcoord();y=p.ycoord();
    if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
    file << x << " " << y << " lt\n"; }
  file << "cpfi\n";
  if (c!=fg_col) change_color(fg_col); }
  
void ps_file::draw_filled_polygon(polygon P, color c) {
  draw_filled_polygon(P.vertices(),c); }
  
// new  
void ps_file::draw_filled_polygon(gen_polygon GP, color c)
{
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;

  if (GP.empty()) return;
  
  

  if (GP.full()) 
  { draw_box(xmin(), ymin(), xmax(), ymax(), c); 
    return; 
   }

  //get the convex components ...
  list<polygon> LP = CONVEX_COMPONENTS(GP);
  polygon iter;
  forall(iter, LP) draw_filled_polygon(iter, c);
  
}  
  
  
  
void ps_file::draw_rectangle(double x0, double y0, double x1, double y1, color c) {
  if (c == invisible) return;
  list<point> lp;
  lp.append(point(x0,y0));lp.append(point(x1,y0));
  lp.append(point(x1,y1));lp.append(point(x0,y1));
  draw_polygon(lp,c); }
void ps_file::draw_rectangle(point p, point q, color c) {
  draw_rectangle(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),c); }
void ps_file::draw_rectangle(rectangle r, color c) {
  draw_rectangle(r.lower_left(),r.upper_right(),c); }

// new from graphwin ...
void ps_file::draw_roundrect(double x0, double y0, double x1, double y1, 
                                                      double rndness, color c) 
{ if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) { scale_xcoord(x0); scale_ycoord(y0);
                     scale_xcoord(x1); scale_ycoord(y1);
		   }
  double w = x1-x0;
  double h = y1-y0;
  double r = (w > h) ? (h/2) : (w/2);
  *this << x0  << y0 << x1  << y1 << (rndness*r) << " rrect stroke\n";
  if (c!=fg_col) change_color(fg_col);
}

void ps_file::draw_filled_roundrect(double x0,double y0,double x1,double y1, 
                                                 double rndness, color c)
{ if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) { scale_xcoord(x0); scale_ycoord(y0);
                     scale_xcoord(x1); scale_ycoord(y1);
		   }     
  double w = x1-x0;
  double h = y1-y0;
  double r = (w > h) ? (h/2) : (w/2);
  *this << x0  << y0 << x1  << y1 << (rndness*r) << " rrect fill\n";
  if (c!=fg_col) change_color(fg_col);
}



  
void ps_file::draw_box(double x0, double y0, double x1, double y1, color c) {
  if (c == invisible) return;
  list<point> lp;
  lp.append(point(x0,y0));lp.append(point(x1,y0));
  lp.append(point(x1,y1));lp.append(point(x0,y1));
  draw_filled_polygon(lp,c); }
void ps_file::draw_box(point p, point q, color c) {
  draw_box(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),c); }
void ps_file::draw_box(rectangle r,color c) {
  draw_box(r.lower_left(),r.upper_right(),c);
}

void ps_file::draw_triangle(point A, point B, point C, color c) {
  if (c == invisible) return;
  list<point> lp;
  lp.append(A);lp.append(B);lp.append(C);
  draw_polygon(lp,c); }
void ps_file::draw_filled_triangle(point A, point B, point C, color c) {
  if (c == invisible) return;
  list<point> lp;
  lp.append(A);lp.append(B);lp.append(C);
  draw_filled_polygon(lp,c); }

// Drawing functions
void ps_file::plot_xy(double, double, win_draw_func, color) {
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::plot_xy not implemented\n";
  file << "% Warning: ps_file::plot_xy not implemented\n"; }
void ps_file::plot_yx(double, double, win_draw_func, color) {
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::plot_yx not implemented\n";
  file << "% Warning: ps_file::plot_yx not implemented\n"; }

// Drawing text
void ps_file::draw_text(double x, double y, string s, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << x << " " << y-fontsize/PS_PIXEL << " mt\n";
  file << "(" << s << ") show\n";
  if (c!=fg_col) change_color(fg_col); }
void ps_file::draw_text(point p, string s, color c) {
  draw_text(p.xcoord(),p.ycoord(),s,c); }

void ps_file::draw_ctext(double x, double y, string s, color c) {
  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << x << " (" << s << ") spds\n";
  file << y-fontsize/PS_PIXEL/2.0 << " mt\n";
  file << "(" << s << ") show\n";
  if (c!=fg_col) change_color(fg_col); }

void ps_file::draw_ctext(point p, string s, color c) {
  draw_ctext(p.xcoord(),p.ycoord(),s,c); }

// Drawing nodes
void ps_file::draw_node(double x0, double y0, color c) {
  draw_circle(x0,y0,nodewidth/2.0/scaling,c); }
void ps_file::draw_node(point p, color c) {
  draw_node(p.xcoord(),p.ycoord(),c); }
void ps_file::draw_filled_node(double x0, double y0, color c) {
  draw_disc(x0,y0,nodewidth/2.0/scaling,c); }
void ps_file::draw_filled_node(point p, color c) {
  draw_filled_node(p.xcoord(),p.ycoord(),c); }
void ps_file::draw_text_node(double x, double y, string s, color c) {
  if (c == invisible) return;
  if (c==white) draw_node(x,y,black);else draw_filled_node(x,y,c);
  if (c==FG_COLOR) c=fg_col;
  color c1;
  if (c==black) c1=white;else c1=black;
  if (c1!=fg_col) change_color(c1);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << "np\n";
  file << x << " (" << s << ") spds\n";
  file << y-fontsize/PS_PIXEL/2.0 << " mt\n";
  file << "(" << s << ") false chp\n";
  file << "fill\n";
  if (c1!=fg_col) change_color(fg_col); }
void ps_file::draw_text_node(point p, string s, color c) {
  draw_text_node(p.xcoord(),p.ycoord(),s,c); }
void ps_file::draw_int_node(double x, double y, int i, color c) {
  if (c == invisible) return;
  draw_filled_node(x,y,c);
  if (c==FG_COLOR) c=fg_col;
  color c1;
  if (c==black) c1=white;else c1=black;
  if (c1!=fg_col) change_color(c1);
  if (must_scale) {scale_xcoord(x);scale_ycoord(y); }
  file << "np\n";
  file << x << " (" << i << ") spds\n";
  file << y-fontsize/PS_PIXEL/2.0 << " mt\n";
  file << "(" << i << ") false chp\n";
  file << "fill\n";
  if (c1!=fg_col) change_color(fg_col); }
void ps_file::draw_int_node(point p, int i, color c) {
  draw_int_node(p.xcoord(),p.ycoord(),i,c); }

// Drawing edges
void ps_file::draw_edge(double x1, double y1, double x2, double y2, color c) {
  if (c == invisible) return;
  scale_xcoord(x1,x2);scale_ycoord(y1,y2);
  must_scale=false;
  line l(segment(x1,y1,x2,y2));
  double d=dist(x1,y1,x2,y2),dx=fabs(x2-x1),r=nodewidth/2.0;
  if (d<nodewidth) draw_segment(x1,y1,x2,y2,c);
  else
    if (dx==0) draw_segment(x1,leda_min(y1,y2)+r,x1,leda_max(y1,y2)-r,c);
    else {
      double delta_x=r/d*dx;
      if (x1<x2) draw_segment(x1+delta_x,l.y_proj(x1+delta_x),x2-delta_x,l.y_proj(x2-delta_x),c);
      else       draw_segment(x1-delta_x,l.y_proj(x1-delta_x),x2+delta_x,l.y_proj(x2+delta_x),c); }
  must_scale=true; }
void ps_file::draw_edge(point p, point q, color c) {
  draw_edge(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),c); }
void ps_file::draw_edge(segment s, color c) {
  draw_edge(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),c); }
void ps_file::draw_edge_arrow(double x1, double y1, double x2, double y2, color c) {
  if (c == invisible) return;
  scale_xcoord(x1,x2);scale_ycoord(y1,y2);
  must_scale=false;
  double temp=scaling;scaling=1;
  line l(segment(x1,y1,x2,y2));
  double d=dist(x1,y1,x2,y2),dx=fabs(x2-x1),r=nodewidth/2.0;
  if (d<nodewidth) draw_arrow(x1,y1,x2,y2,c);
  else
    if (dx==0) draw_arrow(x1,leda_min(y1,y2)+r,x2,leda_max(y1,y2)-r,c);
    else {
      double delta_x=r/d*dx;
      if (x1<x2) draw_arrow(x1+delta_x,l.y_proj(x1+delta_x),x2-delta_x,l.y_proj(x2-delta_x),c);
      else	 draw_arrow(x1-delta_x,l.y_proj(x1-delta_x),x2+delta_x,l.y_proj(x2+delta_x),c); }
  must_scale=true;
  scaling=temp; }
void ps_file::draw_edge_arrow(point p, point q, color c) {
  draw_edge_arrow(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),c); }
void ps_file::draw_edge_arrow(segment s, color c) {
  draw_edge_arrow(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),c); }
  
void ps_file::draw_arc_edge(double, double, double, double, double, color) {
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::draw_arc_edge not implemented\n";
  file << "% Warning: ps_file::draw_arc_edge not implemented\n"; }
void ps_file::draw_arc_edge(point p, point q, double r, color c) {
  draw_arc_edge(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),r,c); }
void ps_file::draw_arc_edge(segment s, double r, color c) {
  draw_arc_edge(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),r,c); }
  
void ps_file::draw_arc_edge_arrow(double, double, double, double, double, color)
{
  cerr << "file " << filename << ", page " << page
       << ": Warning: ps_file::draw_arc_edge_arrow not implemented\n";
  file << "% Warning: ps_file::draw_arc_edge_arrow not implemented\n"; }
void ps_file::draw_arc_edge_arrow(point p, point q, double r, color c) {
  draw_arc_edge_arrow(p.xcoord (),p.ycoord (),q.xcoord (),q.ycoord (),r,c); }
void ps_file::draw_arc_edge_arrow(segment s, double r, color c) {
  draw_arc_edge_arrow(s.xcoord1(),s.ycoord1(),s.xcoord2(),s.ycoord2(),r,c); }

// new from graphwin

point ps_file::draw_arrow_head_poly(const point& q, double a, color c)
{ 
  double d = get_line_width() + 1.2/1;

  double X[4];
  double Y[4];
  point m = window::arrow_head(q,a,d,X,Y);

  for(int i=0; i<4; i++) {
    if (must_scale) { scale_xcoord(X[i]); scale_ycoord(Y[i]); }
    *this << X[i] << Y[i]; 
  }
  *this << 4 << "polygon fill\n";

  return m;
}

void ps_file::draw_polygon_edge(const list<point>& P, int arrow, color c) 
{

  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);  

  list<point> Q = P;

  if (arrow == 4)
  { int m = Q.length()/2;
    list_item it = Q.first();
    while (--m) it = Q.succ(it);
    list<point> lp1,lp2;
    Q.split(it,lp1,lp2,leda::behind);
    point p = lp1.tail();
    point q = lp2.head();
    point s = midpoint(lp1.tail(),lp2.head());
    lp1.append(s);
    lp2.push(s);
    draw_polygon_edge(lp1,2,c);
    draw_polygon_edge(lp2,1,c);
    return;
  }


  if (arrow & 1)
  { list_item it1 = Q.first();
    list_item it2 = Q.succ(it1);
    segment s(Q[it2],Q[it1]);
    Q[it1] = draw_arrow_head_poly(s.target(),s.angle(),c);
   }

  if (arrow & 2)
  { list_item it1 = Q.last();
    list_item it2 = Q.pred(it1);
    segment s(Q[it2],Q[it1]);
    Q[it1] = draw_arrow_head_poly(s.target(),s.angle(),c);
   }

  point p;
  double xc,yc;
  
  forall(p,Q) {
   xc = p.xcoord(); yc = p.ycoord();
   if (must_scale) { scale_xcoord(xc); scale_ycoord(yc); }
   *this << xc << yc;
  }
  *this << Q.size() << "poly_line stroke\n";
  if (c!=fg_col) change_color(fg_col);
}


// ---------------------------------------------------------------------------

void ps_file::draw_bezier_edge(const list<point>& P, int arrow, color c) {

  if (c == invisible) return;
  if (c==FG_COLOR) c=fg_col;
  if (c!=fg_col) change_color(c);  

  int m = 64;

  double* X = new double[m];
  double* Y = new double[m];
  window::compute_bezier(P,m,X,Y);

  if (arrow & 1) 
  { segment s(X[1],Y[1],X[0],Y[0]);
    point q = draw_arrow_head_poly(s.target(),s.angle(),c);
    X[0] = q.xcoord();
    Y[0] = q.ycoord();
   }
    
  if (arrow & 2) 
  { segment s(X[m-2],Y[m-2],X[m-1],Y[m-1]);
    point q = draw_arrow_head_poly(s.target(),s.angle(),c);
    X[m-1] = q.xcoord();
    Y[m-1] = q.ycoord();
   }

  for(int i=0; i<m; i++) {
    if (must_scale) { scale_xcoord(X[i]); scale_ycoord(Y[i]); }
    *this << X[i] << Y[i];
  }
  
  *this << m << "poly_line stroke\n";

  delete[] X;
  delete[] Y;
  
 if (c!=fg_col) change_color(fg_col);  
}


void ps_file::draw_spline_edge(const list<point>& L, int arrow, color c)
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



void ps_file::draw_circle_edge(point p, point q, point r, int arrow, color cl)
{ 
  if (cl == invisible) return;
  if (cl==FG_COLOR) cl=fg_col;
  if (cl!=fg_col) change_color(cl);  

  circle C(p,q,r);

  int orient = orientation(p,q,r);

  if (orient == 0 || C.radius() > 100*p.distance(r)) 
  { list<point> L;
    L.append(p);
    L.append(q);
    L.append(r);
    draw_polygon_edge(L,arrow,cl);
    return;
   }

  point  cen = C.center();
  double rad = C.radius();

  if (arrow == 4)
  { double phi = cen.angle(p,r);
    if (orient < 0) phi -= 2*LEDA_PI;
    point a = p.rotate(cen,0.25*phi);
    point b = p.rotate(cen,0.50*phi);
    point c = p.rotate(cen,0.75*phi);
    draw_circle_edge(p,a,b,2,cl);
    draw_circle_edge(r,c,b,2,cl);
    return;
   }



  if (arrow & 1)
  { segment s(p,cen);
    double d = LEDA_PI/2;
    if (orient < 0) d = -d;
    p = draw_arrow_head_poly(s.source(),s.angle()+d,cl);
  }

  if (arrow & 2)
  { segment s(r,cen);
    double d = LEDA_PI/2;
    if (orient > 0) d = -d;
    r = draw_arrow_head_poly(s.source(),s.angle()+d,cl);
   }

  point  ref   = cen.translate(1,0);
  double start = 360*cen.angle(ref,p)/(2*LEDA_PI);
  double stop  = 360*cen.angle(ref,r)/(2*LEDA_PI);

  double xc1,yc1,xc2,yc2;
  xc1 = p.xcoord(); yc1= p.ycoord();
  xc2 = cen.xcoord(); yc2= cen.ycoord();
  
  if (must_scale){
    scale_xcoord(xc1); scale_ycoord(yc1);
    scale_xcoord(xc2); scale_ycoord(yc2);
    scale_radius(rad);
  }

  *this << xc1 << yc1 << "moveto\n";
  *this << xc2 << yc2 << rad << start << stop;

  if (orient > 0) 
    *this << "arc stroke\n";
  else
    *this << "arcn stroke\n";
    
  if (cl!=fg_col) change_color(fg_col);  
}


// end new from graphwin




void ps_file::draw_ray(double x1, double y1, double x2, double y2, color col)
{
  if (col == invisible) return;
  double dx = x2 - x1;
  double dy = y2 - y1;

  if (dx == 0 && dy == 0)
  { draw_pixel(x1,y1,col); 
    return;
  }

  double x,y;

  if (fabs(dy) < fabs(dx))
  { x = (x1 < x2) ? xmax() : xmin();
    y = y1 + (x-x1)*dy/dx;
  }
  else
  { y = (y1 < y2) ? ymax() : ymin();
    x = x1 + (y-y1)*dx/dy;
  }

  ps_file::draw_segment(x1,y1,x,y,col);
}

void ps_file::draw_ray(const point& o, const point& t, color c)
{ ps_file::draw_ray(o.xcoord(),o.ycoord(),t.xcoord(),t.ycoord(),c); }

void ps_file::draw_ray(const segment& s, color c)
{ ps_file::draw_ray(s.source(),s.target(),c); }

void ps_file::draw_ray(const ray& r, color c)
{ ps_file::draw_ray(r.point1(),r.point2(),c); }



// clipping and xpm support ...

void ps_file::clip_reset()
{ *this << "initclip\n"; }

void ps_file::clip_ellipse(double x, double y, double r1, double r2)
{ 
   if (must_scale) {
     scale_xcoord(x); scale_ycoord(y); 
     scale_radius(r1); scale_radius(r2);
   }
   *this << x << y << r1 << r2 << "ellipse clip newpath\n"; 
}

void ps_file::clip_rectangle(double x0, double y0, double x1, double y1)
{ 
  if (must_scale) {
    scale_xcoord(x0); scale_xcoord(x1);
    scale_ycoord(y0); scale_ycoord(y1);
  }

  double w = x1-x0;
  double h = y1-y0;
  *this << x0  << y0 << w << h << "rectclip\n";
}

void ps_file::clip_polygon(const list<point>& P)
{ point p;
  double  xc,yc;
  
  forall(p,P) {
    xc = p.xcoord(); yc = p.ycoord();
    if (must_scale) { scale_xcoord(xc); scale_ycoord(yc); }
    
    *this << xc << yc; 
  }
  
  
  *this << P.size() << "polygon clip newpath\n";
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
  { while (in.get() != '"') {}
    int index = get_index(in,cpp);
    while (in.get() != '#');
    string rgb_str;
    rgb_str.read(in,'"');
    table[index] = rgb_str;
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



void ps_file::include_xpm(string fname, double x, double y, double w1, 
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

  //cerr << "xpm2ps:" << fname << endl;

  if (cpp > 2) 
  { cerr << "xpm2ps: chars_per_pixel > 2" << endl;
    return;
   }

  double xs = x-w1/2 , ys = y-h1/2;
  
  if (must_scale) { 
    scale_xcoord(xs); scale_ycoord(ys); 
    scale_xcoord(w1); scale_ycoord(h1);
  }

  file << endl;
  file << "gsave" << endl;
  file << string("%f %f %d %d %f %f draw_pixmap",xs,ys,w,h,w1,h1);
  file << endl;

  xpm_to_ps(istr,w,h,ncol,cpp,file);

  file << "grestore" << endl;
}



void ps_file::put_pixrect(char* pmap, double x, double y, double w1, 
                                                             double h1)
{ 
  window W;

  int w = W.get_pixrect_width(pmap);
  int h = W.get_pixrect_height(pmap);
  
  double xs = x-w1/2 ,ys = y-h1/2;
  
  if (must_scale) { 
    scale_xcoord(xs); scale_ycoord(ys); 
    scale_xcoord(w1); scale_ycoord(h1);
  }  

  file << endl;
  file << "gsave" << endl;
  file << string("%f %f %d %d %f %f draw_pixmap",xs,ys,w,h,w1,h1);
  file << endl;

  string psname("/tmp/gw_%x.ps",pmap);

  if (!is_file(psname))
  { ofstream out(psname);
    W.pixrect_to_ps(pmap,out,true);
   }

  ifstream in(psname);

  while (in) file << read_line(in) << endl;
    
  file << "grestore" << endl;
}


LEDA_END_NAMESPACE
