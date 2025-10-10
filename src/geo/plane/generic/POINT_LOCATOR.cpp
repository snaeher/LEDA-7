/*******************************************************************************

+
+  LEDA 7.2  
+
+
+  POINT_LOCATOR.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE

void POINT_LOCATOR::construct_matrix()
{ 
  double cells = G.number_of_nodes()/50.0;

  if (cells < 64) { M = 0; return; }


  int high = int(std::sqrt(cells) - 1);

  M = new array2<node>(0,high,0,high);
  M->init(0);

  xmin = MAXINT;
  ymin = MAXINT;
  xmax = -MAXINT;
  ymax = -MAXINT;

  node v;
  forall_nodes(v,G)
  { point p = pos[v].to_float();
    if (p.xcoord() > xmax) xmax = p.xcoord();
    if (p.ycoord() > ymax) ymax = p.ycoord();
    if (p.xcoord() < xmin) xmin = p.xcoord();
    if (p.ycoord() < ymin) ymin = p.ycoord();
  }

 fx = high/(xmax - xmin);
 fy = high/(ymax - ymin);

 forall_nodes(v,G)
 { point p = pos[v].to_float();
   int x = int(fx * (p.xcoord() - xmin));
   int y = int(fy * (p.ycoord() - ymin));
   (*M)(x,y) = v;
 }

 // fill empty cells

 for(int i = M->low1(); i <= M->high1(); i++)
 { int j = M->low2();
   while (j <= M->high2() && (*M)(i,j) == 0) j++;
   int k = M->high2();
   while (k >= M->low2() && (*M)(i,k) == 0)  k--;

   if (j <= k)
   { node v = (*M)(i,j);
     while (j >= M->low2()) (*M)(i,j--) = v;
     node u = (*M)(i,k);
     while (k <= M->high2()) (*M)(i,k++) = u;
    }
  }

 for(int i = M->low2(); i <= M->high2(); i++)
 { int j = M->low1();
   while (j <= M->high1() && (*M)(i,j) == 0) j++;
   int k = M->high1();
   while (k >= M->low1() && (*M)(i,k) == 0)  k--;

   if (j <= k)
   { node v = (*M)(i,j);
     while (j >= M->low1()) (*M)(i,j--) = v;
     node u = (*M)(i,k);
     while (k <= M->high1()) (*M)(i,k++) = u;
    }
  }


}



edge POINT_LOCATOR::locate(POINT q) const
{ 
  edge start = 0;

  if (M)
  { point p = q.to_float();
    int x = int(fx*(p.xcoord()-xmin));
    int y = int(fy*(p.ycoord()-ymin));
  
    if (x < M->low1())  x = M->low1();
    if (x > M->high1()) x = M->high1();
    if (y < M->low2())  y = M->low2();
    if (y > M->high2()) y = M->high2();
 
    node v = (*M)(x,y);
  
    start =  v ? G.first_adj_edge(v) : 0;
   }

  return LOCATE_IN_TRIANGULATION(G,pos,q,start);
}


bool POINT_LOCATOR::check_locate(POINT p, edge e) const
{ return CHECK_LOCATE_IN_TRIANGULATION(G,pos,p,e); }


bool POINT_LOCATOR::is_hull_edge(edge e) const
{ if (e == 0) return false;
  edge x = G.face_cycle_succ(e);
  POINT a = pos[source(e)];
  POINT b = pos[target(e)];
  POINT c = pos[target(x)]; 
  return orientation(a,b,c) <= 0;
}




int POINT_LOCATOR::number_of_empty_cells() const
{ int count = 0;
  if (M)
  { for(int i=M->low1(); i <= M->high1(); i++)
      for(int j=M->low2(); j <= M->high2(); j++)
        if ((*M)(i,j) == 0) count++;
   }
  return count;
 }

int POINT_LOCATOR::number_of_cells() const 
{ if (M == 0) return 0;
  return (M->high1() - M->low1() + 1) * (M->high2() - M->low1() + 1); 
 }
  
bool POINT_LOCATOR::intersection(const LINE& l, list<edge>& cuts) const
{

	edge e1;
	list<edge> investigate;
	POINT p1 = l.point1();
	edge_array<bool> visited(G, false);

	e1 = locate(p1);

	// in case it is a hull edge
	// if there is an intersection:
	// assign the reverse edge of the hull edge to e1
	// append e1 to investigate

	if (is_hull_edge(e1))
	{
		edge start = e1;
		

		do
		{
			POINT p = pos[G.source(e1)];
			POINT q = pos[G.target(e1)];
			SEGMENT s(p, q);
			if (l.intersection(s))
			{
				e1 = G.reversal(e1);
				investigate.append(e1);
				visited[e1] = true;
				cuts.append(e1);
				break;
			}
			e1 = G.face_cycle_succ(e1);
		} while(start != e1);

	}
	else
	{
		investigate.append(e1);
		cuts.append(e1);
		visited[e1] = true;
	}


	// After check, e1 is some edge of an inner face, that is cut by line l
	// investigate never contains a hull edge


	while (!investigate.empty())
	{
		e1 = investigate.pop();

		// compute other edges of face
		edge e2 = G.face_cycle_succ(e1);
		edge e3 = G.face_cycle_succ(e2);
		
		// compute points
		POINT a = pos[G.source(e1)];
		POINT b = pos[G.target(e1)];
		POINT c = pos[G.target(e2)];
		
		// compute segments with edge length
		SEGMENT s1(a,b);
		SEGMENT s2(b,c);
		SEGMENT s3(c,a);

		// compute intersecting segments
		// stop when hull is reached
		// if not, continue
		if (l.intersection(s1))
		{

			//cuts.append(e1);

			edge rev = G.reversal(e1);

			if (!is_hull_edge(rev) && !(visited[rev]))
			{
				investigate.append(rev);
				visited[rev] = true;
				cuts.append(rev);
			}
		}
		if (l.intersection(s2))
		{
			if (!visited[e2])
			{
				cuts.append(e2);
				visited[e2] = true;
			}

			edge rev = G.reversal(e2);

			if (!is_hull_edge(rev) && !(visited[rev]))
			{
				investigate.append(rev);
				visited[rev] = true;
				cuts.append(rev);
			}
		}
		if (l.intersection(s3))
		{

			if (!visited[e3])
			{
				cuts.append(e3);
				visited[e3] = true;
			}

			edge rev = G.reversal(e3);

			if (!is_hull_edge(rev) && !(visited[rev]))
			{
				investigate.append(rev);
				visited[rev] = true;
				cuts.append(rev);
			}
		}
	}
	// here, all intersections have been found and cuts contains the list of intersecting edges, where all inner reversals are contained
	// hull edges should not be contained
	if (cuts.empty())
		return false;
	return true;
}


bool POINT_LOCATOR::intersection(const RAY& l, list<edge>& cuts) const
{

	edge e1;
	list<edge> investigate;
	POINT sp;
	POINT p1 = l.point1();
	edge_array<bool> visited(G, false);

	e1 = locate(p1);

	// in case it is a hull edge
	// if there is an intersection:
	// assign the reverse edge of the hull edge to e1
	// append e1 to investigate

	if (is_hull_edge(e1))
	{
		edge start = e1;
		

		do
		{
			POINT p = pos[G.source(e1)];
			POINT q = pos[G.target(e1)];
			SEGMENT s(p, q);
			if (l.intersection(s, sp))
			{
				e1 = G.reversal(e1);
				investigate.append(e1);
				cuts.append(e1);
				visited[e1] = true;
				break;
			}
			e1 = G.face_cycle_succ(e1);
		} while(start != e1);

	}
	else
	{
		investigate.append(e1);
		cuts.append(e1);
		visited[e1] = true;
	}


	// After check, e1 is some edge of an inner face, that is cut by line l
	// investigate never contains a hull edge


	while (!investigate.empty())
	{
		e1 = investigate.pop();

		// compute other edges of face
		edge e2 = G.face_cycle_succ(e1);
		edge e3 = G.face_cycle_succ(e2);
		
		// compute points
		POINT a = pos[G.source(e1)];
		POINT b = pos[G.target(e1)];
		POINT c = pos[G.target(e2)];
		
		// compute segments with edge length
		SEGMENT s1(a,b);
		SEGMENT s2(b,c);
		SEGMENT s3(c,a);

		// compute intersecting segments
		// stop when hull is reached
		// if not, continue
		if (l.intersection(s1, sp))
		{

			//cuts.append(e1);

			edge rev = G.reversal(e1);

			if (!is_hull_edge(rev) && !(visited[rev]))
			{
				investigate.append(rev);
				visited[rev] = true;
				cuts.append(rev);
			}
		}
		if (l.intersection(s2, sp))
		{
			if (!visited[e2])
			{
				cuts.append(e2);
				visited[e2] = true;
			}

			edge rev = G.reversal(e2);

			if (!is_hull_edge(rev) && !(visited[rev]))
			{
				investigate.append(rev);
				visited[rev] = true;
				cuts.append(rev);
			}
		}
		if (l.intersection(s3, sp))
		{

			if (!visited[e3])
			{
				cuts.append(e3);
				visited[e3] = true;
			}

			edge rev = G.reversal(e3);

			if (!is_hull_edge(rev) && !(visited[rev]))
			{
				investigate.append(rev);
				visited[rev] = true;
				cuts.append(rev);
			}
		}
	}
	// here, all intersections have been found and cuts contains the list of intersecting edges, where all inner reversals are contained
	// hull edges should not be contained
	if (cuts.empty())
		return false;
	return true;
}

LEDA_END_NAMESPACE 
 
