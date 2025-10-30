/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  help.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>
#include "local.h"

LEDA_BEGIN_NAMESPACE

void GraphWin::help_news()
{
 panel P;
 P.text_item(string("\\bf New in Version %f",version()));
 P.text_item("");
 P.text_item("New Node Shapes: rhombus_node, roundrect_node, ovalrect_node");
 P.text_item("");
 P.text_item("Improved Grid Mode");
 P.text_item("");
 P.text_item("Shrink/Expand (layout menu: tools)");
 P.text_item("");
 P.text_item("Export (file menu): LaTeX, Windows Metafiles, Clipboard");
 P.text_item("");
 P.text_item("Saving/Restoring Default Settings");
 P.text_item("");
 P.fbutton("ok");
 open_panel(P);
}


/*
void GraphWin::help_about()
{

panel P;

P.text_item(string("\\bf GraphWin (Version %f)",version()));
P.text_item("");
P.text_item("");
P.text_item("\
a LEDA data type for constructing, manipulating, and visualizing \
graphs and for animating and debugging graph algorithms. \
Please see the LEDA User Manual for a detailed description.");
P.text_item("");
P.text_item("");
P.text_item(string("The design and implementation of GraphWin was \
supported in part by DFG-Grant Na303/1-2, Effiziente Algorithmen \
f%cr diskrete Probleme und ihre Anwendungen. ",uml_u));
P.text_item("");
P.fbutton("ok");

open_panel(P);
}
*/


void GraphWin::help_about()
{
  //panel P;
  window P(2*win_p->width()/3,100);
  
  P.text_item(string("\\bf GraphWin (Version %.2f)",version()));
  P.text_item("");
  P.text_item("GraphWin is a LEDA data type for constructing, displaying and");
  P.text_item("manipulating graphs and for animating and visualizing graph");
  P.text_item("algorithms. A detailed description can be found in the LEDA");
  P.text_item("Book and User Manual.");
  P.text_item("");
  
  P.fbutton("ok");
  
  //open_panel(P);
  
  P.display(*win_p,window::center,window::center);
  P.read_mouse();
}


void GraphWin::help_mouse_left()
{

//panel P(500,-1);
panel P(300,-1);

P.text_item("\\bf\\blue Left Mouse Button  (Create/Move/Resize)");
P.text_item("");
P.text_item("\
 A \\bf single click on the background \\rm creates a new node.");
 P.text_item("");
 P.text_item("\
 A \\bf single click on a node \\rm selects this node as the source of a new\
 edge. The next click defines the target of the edge which is either an\
 existing node or a new node (if clicked on the window background). Before\
 defining the target node, bends may be introduced using the middle button.\
 The creation of the new edge can be canceled by clicking the right button.");  
P.text_item("");

P.text_item("\
 Clicking on a node or edge and\\bf dragging\\rm the pointer while holding\
 down the button moves the node or inserts a bend into the edge, respectively.\
 If click and drag is done on an empty position the window background\
 will be moved.");
P.text_item("");
/*
P.text_item("\
 \\bf Resize a node \\rm by holding down the SHIFT key, clicking on the\
 border line of the node, and dragging the mouse pointer.");
*/
P.text_item("\
 \\bf Resize a node \\rm by clicking on the border line of the node and\
 dragging the mouse pointer.");
P.text_item("");

P.text_item("\
 A \\bf double click \\rm on a node or edge opens a setup panel for\
 changing the attributes of the corresponding object.");
P.text_item("");

P.text_item("\
  \\bf\\blue Some of the above operations are also available in the node\
  and edge context menus which can be opened with the right mouse button.");
P.text_item("");

P.fbutton("ok");

open_panel(P);

}



void GraphWin::help_mouse_middle()
{

panel P;
//panel P(300,-1);


P.text_item("\\bf\\blue Middle Mouse Button   (Select) ~~~~~~~~~~~~~~~");
P.text_item("");
P.text_item("\
 The middle button is used for selecting or deselecting nodes and edges.\
 A \\bf single click \\rm includes (removes) the object under the mouse cursor\
 into (from) the current selection. If a SHIFT key is pressed simultaneously\
 the connected component of the corresponding object is selected or\
 unselected. By clicking on the window background, holding down the button,\
 and dragging the mouse a rectangular area is defined. After releasing the\
 button all objects contained in this area are selected or unselected.");
P.text_item("");
P.text_item("\
 On two-button systems you have to hold down the ALT key and click\
 the left button.");
P.text_item("");

P.fbutton("ok");

open_panel(P);
}


void GraphWin::help_mouse_right()
{

//panel P;
panel P(300,-1);

P.text_item("\\bf\\blue Right Mouse Button  (Context Menu)");
P.text_item("");
P.text_item("\
 The right button clicked on a node or edge opens a context menu that can be\
 used for deleting the corresponding object or for changing its attributes.");

P.fbutton("ok");

open_panel(P);
}

LEDA_END_NAMESPACE

