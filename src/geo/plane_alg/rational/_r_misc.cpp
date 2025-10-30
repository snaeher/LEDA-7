/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _r_misc.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/geo/geo_alg.h>

#include "TypesR.h"
#include "misc.cpp"

LEDA_BEGIN_NAMESPACE

void Minimal_Area_Bounding_Box(const list<rat_point>& L, rat_point& llc, rat_point& lrc, rat_point& urc, rat_point& ulc)
{
	rat_polygon p;

	//construct the convex hull; 
	p = CONVEX_HULL_POLY(L);

	list<rat_point> plist = p.vertices();

	// need calipers, to each line keep book of the incident vertex

	rat_line l1, l2, l3, l4;
	rat_point p1, p2, p3, p4, v;
	list_item it, it1, it2, it3, it4;

	v = plist.head();
	p1 = p2 = p3 = p4= v;
	it1 = it2 = it3 = it4 = plist.first();

	rat_vector v1(rational(0), rational(1));
	rat_vector v2(rational(1), rational(0));



	// we construct a starting polygon through the four extreme vertices

	forall_items(it, plist)
	{
		if (plist[it].xcoord() < p1.xcoord())
		{ p1 = plist[it]; it1 = it;};
		
		if (plist[it].ycoord() < p2.ycoord())
		{ p2 = plist[it]; it2 = it;};
		
		if (plist[it].xcoord() > p3.xcoord())
		{ p3 = plist[it]; it3 = it;};
		
		if (plist[it].ycoord() > p4.ycoord())
		{ p4 = plist[it]; it4 = it;};
	}
	

	l1 = rat_line(p1, -v1);
	l2 = rat_line(p2, v2);
	l3 = rat_line(p3, v1);
	l4 = rat_line(p4, -v2);

	list<rat_point> rectnodes;

	l1.intersection(l2, llc);
	l3.intersection(l4, urc);
	lrc = rat_point(urc.xcoord(), llc.ycoord());
	ulc = rat_point(llc.xcoord(), urc.ycoord());

	rectnodes.append(llc);
	rectnodes.append(lrc);
	rectnodes.append(urc);
	rectnodes.append(ulc);


	rat_polygon minpol(rectnodes, rat_polygon::NO_CHECK);

	// keep book of the polygons area

	rational area = minpol.area();

	// each segment has to be considered only once; this is one too many
	int iter = plist.size();

	rat_vector vec1, vec2, vec3, vec4, vec;
	double actangle, angle;
	int index;
	rat_line l1succ, l2succ, l3succ, l4succ;
	rat_point llc1, lrc1, urc1, ulc1;
	rational actarea;
	rat_polygon pol, actpol;


	// rotate the calipers:
	// compute for each line the new line that is adjacent to the next segment

	vec1 = p1 - plist[ plist.cyclic_succ(it1) ];
	l1succ = rat_line(p1, vec1);

	vec2 = p2 - plist[ plist.cyclic_succ(it2) ];
	l2succ = rat_line(p2, vec2);

	vec3 = p3 - plist[ plist.cyclic_succ(it3) ];
	l3succ = rat_line(p3, vec3);

	vec4 = p4 - plist[ plist.cyclic_succ(it4) ];
	l4succ = rat_line(p4, vec4);


	while (iter--)
	{

		// the smallest angle between line and its predecessor wins

		actangle = l1.to_float().angle(l1succ.to_float());
		if (actangle > 6.2831852)
			actangle = 0;
		angle = actangle;
		vec = vec1;
		index = 1;

		actangle = l2.to_float().angle(l2succ.to_float());
		if (actangle > 6.2831852)
			actangle = 0;
		if (angle > actangle)
		{
			angle = actangle;
			vec = vec2;
			index = 2;
		}

		actangle = l3.to_float().angle(l3succ.to_float());
		if (actangle > 6.2831852)
			actangle = 0;
		if (angle > actangle)
		{
			angle = actangle;
			vec = vec3;
			index = 3;
		}
		actangle = l4.to_float().angle(l4succ.to_float());
		if (actangle > 6.2831852)
			actangle = 0;
		if (angle > actangle)
		{
			angle = actangle;
			vec = vec4;
			index = 4;
		}

		// rotate the lines

		switch(index) 
		{
			case 1: 
				it1 = plist.cyclic_succ(it1);
				p1 = plist[ it1 ];		
				l1 = l1succ;
				l2 = rat_line(p2, vec.rotate90());
				l3 = rat_line(p3, -vec);
				l4 = rat_line(p4, vec.rotate90(3));
				break;
			case 2:
				it2 = plist.cyclic_succ(it2);
				p2 = plist[ it2 ];
				l1 = rat_line(p1, vec.rotate90(3));
				l2 = l2succ;
				l3 = rat_line(p3, vec.rotate90());
				l4 = rat_line(p4, -vec);
				break;
			case 3:
				it3 = plist.cyclic_succ(it3);
				p3 = plist[ it3 ];
				l1 = rat_line(p1, -vec);
				l2 = rat_line(p2, vec.rotate90(3));
				l3 = l3succ;
				l4 = rat_line(p4, vec.rotate90());
				break;
			case 4:
				it4 = plist.cyclic_succ(it4);
				p4 = plist[ it4 ];
				l1 = rat_line(p1, vec.rotate90());
				l2 = rat_line(p2, -vec);
				l3 = rat_line(p3, vec.rotate90(3));
				l4 = l4succ;
		}

		// compute the new rectangle
		l1.intersection(l2, llc1);
		l2.intersection(l3, lrc1);
		l3.intersection(l4, urc1);
		l4.intersection(l1, ulc1);

		rectnodes.clear();

		rectnodes.append(llc1);
		rectnodes.append(lrc1);
		rectnodes.append(urc1);
		rectnodes.append(ulc1);

		actpol = rat_polygon(rectnodes, rat_polygon::NO_CHECK);

		// compare the area
		actarea = actpol.area();

		if (area > actarea)
		{
			pol = actpol;
			area = actarea;
			llc = llc1;
			lrc = lrc1;
			urc = urc1;
			ulc = ulc1;
		}


		// we have to recompute the successor lines
		if (iter > 1)
		{
			vec1 = p1 - plist[ plist.cyclic_succ(it1) ];
			l1succ = rat_line(p1, vec1);

			vec2 = p2 - plist[ plist.cyclic_succ(it2) ];
			l2succ = rat_line(p2, vec2);

			vec3 = p3 - plist[ plist.cyclic_succ(it3) ];
			l3succ = rat_line(p3, vec3);

			vec4 = p4 - plist[ plist.cyclic_succ(it4) ];
			l4succ = rat_line(p4, vec4);
		}
	}
}

LEDA_END_NAMESPACE

