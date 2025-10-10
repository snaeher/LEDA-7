#ifndef _GW_LOCAL_H_
#define _GW_LOCAL_H_

#define GraphWinVersion   2.1
#define FileFormatVersion 1.4

#define DefGraphName            "graph"

// window

#define DefWindowLabel          "GraphWin"

#define DefWinSizeX            window::default_width()
#define DefWinSizeY            window::default_height()


// psa4: width = 510 = 18 cm * 28.35 pix/cm

#define DefXMin                 -10
#define DefXMax                 500
#define DefYMin                 -20

#define DefBgPixrect            0
#define DefBgColor              white

#define DefGridDist             0
#define DefGridStyle            line_grid
#define DefGridAxis             true

#define DefShowStatus           true
#define DefShowMask             (STAT_NODES|STAT_EDGES|STAT_COORD|STAT_UNDO)

#define DefButtonsPerLine       9
#define DefExtraButtonsPerLine  3


// psa4: width = 510 = 18 cm * 28.35 pix/cm

#define DefXMin                 -10
#define DefXMax                 500
#define DefYMin                 -20

#define DefBgPixrect            0
#define DefBgColor              white

#define DefGridDist             0
#define DefGridStyle            line_grid
#define DefGridAxis             true

#define DefShowStatus           true
#define DefShowMask             (STAT_NODES|STAT_EDGES|STAT_COORD|STAT_UNDO)

#define DefButtonsPerLine       9
#define DefExtraButtonsPerLine  3


// default node attributes

#define DefNodeShape            circle_node //ellipse_node

#define DefNodeRadius1          14.0      
#define DefNodeRadius2          14.0 


#define DefNodeColor            ivory
#define DefNodeBorderColor      black
#define DefNodeBorderWidth      0.8 
#define DefNodePixmap           NULL

#define DefNodeLabelFontSize    14.0
#define DefNodeLabelType        index_label
#define DefNodeLabelClr         gw_auto_color
#define DefNodeLabelPos         central_pos
#define DefNodeLabel            ""


// default edge attributes

#define DefEdgeColor            black
#define DefEdgeWidth            0.8
#define DefEdgeAnchor           point(0,0)
#define DefEdgeShape            poly_edge
#define DefEdgeStyle            solid_edge
#define DefEdgeDir              directed_edge

#define DefEdgeLabelFontSize    12.0
#define DefEdgeLabelType        user_label
#define DefEdgeLabelClr         black
#define DefEdgeLabelPos         east_pos
#define DefEdgeLabel            ""


// global options

#define DefNodeIndexFormat      "%d" 
#define DefEdgeIndexFormat      "%d"
#define DefEdgeBorder           false
#define DefNodeLabelFontType    roman_font
#define DefEdgeLabelFontType    roman_font
#define DefNodeEmbed            random_embed
#define DefMultiEdgeDist        5
#define DefAutoCreateTarget     true


// animation and zooming

#define DefFlush                true
#define DefMaxMoveItems         200

/*
#define DefAnimationSteps       100     
*/

#define DefAnimationSteps \
((window::display_type() == "xx") ? 30 : \
((window::display_type() == "mswin") ? 150 : 500))

#define DefAnimationMode        move_all_nodes

#define DefZoomFactor           1.25
#define DefZoomObjects          true
#define DefZoomLabels           true
#define DefZoomHideLabels       false

#define DefNodeAdjLabel         true
#define DefAutoEdge             true


// AGD

//#define DefAgdHost              ""
#define DefAgdHost              "localhost"
#define DefAgdPort              9660
#define DefAgdInputFormat       0
#define DefAgdOutputFormat      0
#define DefAgdOptionDiag        false


// misc
#define DefGenNodes             25
#define DefGenEdges             50      
#define DefOrthoMode            false
#define DefCallsEnabled         true
#define DefD3Look               false
#define DefPSFontName           "Times"
#define DefPrintCmd             "lpr -h"

#endif

