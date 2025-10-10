/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _dyntrees.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// -------------------------------------------------------------------- //
// Implementation of class dynamic_trees                                //
//                                                                      //
// See dynamic_trees.h and the additional documentation.                //
// -------------------------------------------------------------------- //

#include <LEDA/core/dynamic_trees.h>
#include <LEDA/core/random_source.h>

LEDA_BEGIN_NAMESPACE

// --- dynamic_trees methods --- //

path dynamic_trees::splice(path p)
// Extend the solid path p at its tail by one edge. Return
// the extended path. Prec.: tail(p) is not a tree root.
{
  vertex v;
  path q,r;
  double x,y;

  v = tail(p)->dparent;
  split(v,q,r,x,y);
  if(q)
  {
    tail(q)->dparent = v;
    tail(q)->dcost = x;
  }
  p = concatenate(p,path_of(v),tail(p)->dcost,tail(p)->y);
  return (r ? concatenate(p,r,y,tail(p)->y) : p);
}

path dynamic_trees::expose(vertex v)
// Convert the path from v to root(v) to a single solid path
// and return it.
{
  path p,q,r;
  double x,y;

  split(v,q,r,x,y);
  if(q)
  {
    tail(q)->dparent = v;
    tail(q)->dcost = x;
  }
  if(!r) p = path_of(v);
  else   p = concatenate(path_of(v),r,y,v->y);

  while(tail(p)->dparent) p = splice(p);

  return p;
}

vertex dynamic_trees::parent(vertex v)
// Return the parent of v in the tree or nil.
{
  return (v == tail(path_of(v)) ? v->dparent : aft(v));
}

vertex dynamic_trees::root(vertex v)
// Return the root of the tree containing v.
{
  return tail(expose(v));
}

double dynamic_trees::cost(vertex v)
// Return the cost of (v,parent(v)).
// Prec.: v is not tree root.
{
  return (v == tail(path_of(v)) ? v->dcost : pcost(v));
}

vertex dynamic_trees::mincost(vertex v)
// Return vertex w closest to root(v) s.t. (w,parent(w)) has minimal cost
// on the path v -> root(v). Prec.: v is not a tree root.
{
  return pmincost(expose(v));
}

void dynamic_trees::update(vertex v, double x)
// Add x to each edge on the path v -> root(v).
{
  pupdate(expose(v),x);
}

void dynamic_trees::link(vertex v, vertex w, double x, void* e_inf)
// Link the tree root v (prec.) to the vertex w in a different
// tree (prec.). The edge (v,w) gets weight x.
{
  concatenate(path_of(v), expose(w), x, e_inf);
}

double dynamic_trees::cut(vertex v)
// Delete the edge (v,parent(v)) and return its weight.
// Prec.: v is not a tree root.
{
  path p,q; double x,y;
  expose(v);
  split(v,p,q,x,y);
  v->dparent = nil;
  v->y = nil;               // invalidate edge information
  return y;
}

void dynamic_trees::evert(vertex v)
// Make v the new root of its tree.
{
  reverse(expose(v)); v->dparent = nil;
  v->y = nil;               // invalidate edge information
}

vertex dynamic_trees::lca(vertex v, vertex w)
// Return the lowest common ancestor of v and w or nil.
// Prec.: v and w are not nil.
{
  expose(v);
  vertex v_path = path_of(v);

  vertex cand;
  for(cand = w; cand && (path_of(cand) != v_path);
      cand = tail(path_of(cand))->dparent);

  return cand;
}

vertex dynamic_trees::make(void* x)
// Create a tree containing a single node v which is returned.
// Store the additional user defined information x at v.
{
  vertex v = new vertex_struct(true,x);
  vertices.append(v);
  return v;
}

void dynamic_trees::clear()
// Delete all vertices and edges.
{
  while(vertices.size())
  {
    vertex v = vertices.pop();
    path p,q;
    double x,y;
    split(v,p,q,x,y);
    delete v;
  }
}

void* dynamic_trees::vertex_inf(vertex v)
// Return the additional user defined information at v.
{
  return v->x;
}

void* dynamic_trees::edge_inf(vertex v)
// Return the additional user defined information at (v,parent(v)).
{
  if(!parent(v)) return nil;
  if(v != tail(path_of(v))) expose(parent(v));
  return v->y;
}



// --- path methods --- //

vertex_struct::vertex_struct(bool leaf, void* initial_x)
// create an isolated vertex
{
  if(leaf)
  {
    child[0] = child[1] = nil;
    handt[0] = handt[1] = this;
    prio = -1;                    // avoid rotating this vertex
    external = true;
    netcost = netmin = MAXDOUBLE;
    y = nil;
  }
  else
  { prio = rand_int();
    external = false;
  }
  par = nil;
  reversed = 0;
  dparent = nil;
  dcost = 0;
  x = initial_x;
}

path path_of(vertex v)
// Return the path containing v.
{
  vertex aux;
  for(aux = v; aux->par; aux = aux->par);
  return aux;
}

vertex head(path p)
// Return the first vertex of p.
{
  return p->handt[p->reversed];
}

vertex tail(path p)
// Return the last vertex of p.
{
  return p->handt[1 - p->reversed];
}

void pupdate(path p, double x)
// Add x to the cost of every edge on p.
{
  p->netmin += x;
}

void reverse(path p)
// Reverse the direction of p.
// Prec.: p represents an entire path (p->par == nil).
{
  p->reversed = 1 - p->reversed;
}

void* vertex_struct::vertex_inf()
// Return the additional user defined information at this node.
{
  return x;
}

vertex bef(vertex v)
// Return the vertex before v on path_of(v) or nil.
{
  // special case: v is an isolated vertex
  if(!v->par) return nil;

  // record the path v -> path_of(v)
  list<vertex> v_path;
  for(vertex vv = v; vv; vv = vv->par) v_path.push(vv);

  // Find the deepest node w on v_path which is a right child
  // and its parent p.
  vertex w = nil;             // a right child
  vertex p = nil;             // its parent
  int p_state = 0;            // reversal state of p
  vertex cand;                // a candidate for a right child
  vertex last;                // its parent
  int reversal_state = 0;     // reversal_state of last

  last = v_path.pop();

  while (v_path.size())
  {
    // compute the reversal state of last
    reversal_state = (reversal_state + last->reversed) % 2;

    // get next candidate
    cand = v_path.pop();

    // check whether cand is a right child
    if(cand == last->child[1 - reversal_state])
    {
      w = cand;
      p = last;
      p_state = reversal_state;
    }

    last = cand;
  }

  if(!w) return nil;            // v is the first vertex on its path

  return (p_state == 0? tail(p->child[p_state]) : head(p->child[p_state]));
}

vertex aft(vertex v)
// Return the vertex after v on path_of(v) or nil.
{
  // special case: v is an isolated vertex
  if(!v->par) return nil;

  // record the path v -> path_of(v)
  list<vertex> v_path;
  for(vertex vv = v; vv; vv = vv->par) v_path.push(vv);

  // Find the deepest node w on v_path which is a left child
  // and its parent p.
  vertex w = nil;             // a left child
  vertex p = nil;             // its parent
  int p_state = 0;            // reversal state of p
  vertex cand;                // a candidate for a left child
  vertex last;                // its parent
  int reversal_state = 0;     // reversal_state of last

  last = v_path.pop();
  while(v_path.size())
  {
    // compute the reversal state of last
    reversal_state = (reversal_state + last->reversed) % 2;

    // get next candidate
    cand = v_path.pop();

    // check whether cand is a left child
    if(cand == last->child[reversal_state])
    {
      w = cand;
      p = last;
      p_state = reversal_state;
    }

    last = cand;
  }

  if(!w) return nil;      // v is the first vertex on its path

  return (p_state == 0? head(p->child[1-p_state]) : tail(p->child[1-p_state]));
}


double pcost(vertex v)
// Return the cost of the edge (v,aft(v)).
// Prec.: v != tail(path_of(v))
{
  // record the path v -> path_of(v)
  list<vertex> v_path;
  for(vertex vv = v; vv; vv = vv->par) v_path.push(vv);

  // Find the deepest node w on v_path which is a left child
  // and its parent p.
  // vertex w = nil;             // a left child
  vertex p = nil;             // its parent
  vertex cand;                // a candidate for a left child
  vertex last;                // its parent
  int reversal_state = 0;     // reversal_state of last
  double grossmin = 0;        // grossmin of last
  double p_grossmin = 0;      // grossmin of p

  last = v_path.pop();
  while(v_path.size())
  {
    // compute the reversal state and grossmin of last
    reversal_state = (reversal_state + last->reversed) % 2;
    grossmin += last->netmin;

    // get next candidate
    cand = v_path.pop();

    // check whether cand is a left child
    if(cand == last->child[reversal_state])
    {
      //w = cand;
      p = last;
      p_grossmin = grossmin;
    }

    last = cand;
  }

  return p->netcost + p_grossmin;
}

vertex pmincost(path p)
// Return the vertex v closest to tail(p) such that the cost
// of (v,aft(v)) is minimal on p.
// Prec.: p contains more than one vertex.
{
  vertex u = p;
  int reversal_state = u->reversed;
  while(
         (u->netcost > 0)
         ||
         (
           !u->child[1-reversal_state]->external
           &&
           (u->child[1-reversal_state]->netmin == 0)
         )
       )
  {
    // check right child
    if(u->child[1-reversal_state]->netmin == 0)
    {
      u = u->child[1-reversal_state];
      reversal_state = (reversal_state + u->reversed) % 2;
    }
    else
    {
      // proceed with left child
      u = u->child[reversal_state];
      reversal_state = (reversal_state + u->reversed) % 2;
    }
  }

  return reversal_state ? head(u->child[reversal_state])
                        : tail(u->child[reversal_state]);
}

path concatenate(path p, path q, double x, void* e_inf)
// Combine p and q by adding the edge (tail(p),head(q)) of cost x.
// Return the combined path.
{
  // create a new vertex representing the new edge, link it to p and q
  vertex new_vertex = new vertex_struct();
  new_vertex->y = e_inf;
  new_vertex->child[0] = p;
  p->par = new_vertex;
  new_vertex->child[1] = q;
  q->par = new_vertex;
  new_vertex->handt[0] = head(p);
  new_vertex->handt[1] = tail(q);

  // adjust netmin and netcost fields
  double min_netmin = leda::min(x,leda::min(p->netmin,q->netmin));
  new_vertex->netmin = min_netmin;
  new_vertex->netcost = x - min_netmin;
  if(!p->external) p->netmin -= min_netmin;
  if(!q->external) q->netmin -= min_netmin;

  // rebalance
  long nv_prio = new_vertex->prio;
  while(    (new_vertex->child[0]->prio > nv_prio)
         || (new_vertex->child[1]->prio > nv_prio) )
    if( new_vertex->child[0]->prio > new_vertex->child[1]->prio )
      rotate(new_vertex->child[0],new_vertex);
    else
      rotate(new_vertex->child[1],new_vertex);

  return path_of(p);
}

void split(vertex v, path& p, path& q, double& x, double&y)
// Split path_of(v) by deleting the edges incident to v.
// Assign the path head(path_of(v)) -> bef(v) to p.
// Assign the path aft(v) -> tail(path_of(v)) tp q.
// Assign the cost of the deleted edge (bef(v),v) to x.
// Assign the cost of the deleted edge (v,aft(v)) to y.
// p or q might be nil, then x or y are undefined.
{
  // Let u be the predecessor of v on path_of(v) and w its
  // successor. Let a and b be the corresponding edges.
  //                   u-(a)-v-(b)-w
  // We have to rotate the node representing a to the top
  // of the balanced binary tree representing path_of(v),
  // and then delete it. Then we do the same for b.
  // The parent of v in the tree is always either a or b.

  p = q = nil;

  while(v->par)
  {
    // found an incident edge
    vertex inc = v->par;

    // rotate it up
    while(inc->par) rotate(inc,inc->par);

    // check whether inc is a or b
    if(   ( inc->reversed && (head(inc->child[1]) == v))
       || (!inc->reversed && (tail(inc->child[0]) == v)) )
    {
      // v is in the left subtree of inc, thus inc == b
      y = inc->netmin + inc->netcost;
      q = inc->child[1-inc->reversed];
      v->y = inc->y;
    }
    else
    {
      // v is in the right subtree of inc, thus inc == a
      x = inc->netmin + inc->netcost;
      p = inc->child[inc->reversed];
      if(inc->reversed) head(p)->y = inc->y;
      else              tail(p)->y = inc->y;
    }

    // cut the tree
    inc->isolate();
    delete inc;
  }
}

void rotate(vertex rot_child, vertex rot_parent)
// Rotate such that rot_child becomes the parent of rot_parent.
// Prec.: rot_child is a child of rot_parent, both are internal.
{
  // --- fix reversal of rot_child ---
  if(rot_child->reversed)
  {
    // exchange the children of rot_child ...
    vertex          aux = rot_child->child[0];
    rot_child->child[0] = rot_child->child[1];
    rot_child->child[1] = aux;
    // ... and reverse them
    rot_child->child[0]->reversed = 1 - rot_child->child[0]->reversed;
    rot_child->child[1]->reversed = 1 - rot_child->child[1]->reversed;
    rot_child->reversed = 0;
  }

  // --- do the actual rotation ---
  // determine the direction dir of the rotation
  int dir = (rot_parent->child[0] == rot_child) ? 1 : 0;

  // subtree which changes sides
  vertex beta = rot_child->child[dir];

  // fix beta
  rot_parent->child[1-dir] = beta;
  beta->par = rot_parent;

  // fix parent field of rot_child
  rot_child->par = rot_parent->par;
  if (rot_child->par)
  { if(rot_child->par->child[0] == rot_parent)
       rot_child->par->child[0] = rot_child;
    else
       rot_child->par->child[1] = rot_child;
   }

  // fix parent field of rot_parent
  rot_child->child[dir] = rot_parent;
  rot_parent->par = rot_child;


  // --- fix additional information ---
  // reversed bits
  if(rot_parent->reversed)
  {
    rot_child->reversed = 1;
    rot_parent->reversed = 0;
  }
  // Now rot_parent->reversed == 0 holds. This is used below.

  // head and tail
  rot_parent->handt[0] = head(rot_parent->child[0]);
  rot_parent->handt[1] = tail(rot_parent->child[1]);
  if(rot_child->reversed)
  {
    rot_child->handt[1] =            // == head(rot_child)
      tail(rot_child->child[1]);
    rot_child->handt[0] =            // == tail(rot_child)
      head(rot_child->child[0]);
  }
  else
  {
    rot_child->handt[0] =            // == head(rot_child)
      head(rot_child->child[0]);
    rot_child->handt[1] =            // == tail(rot_child)
      tail(rot_child->child[1]);
  }

  // netmin and netcost
  vertex alpha = rot_parent->child[dir];
    // child of rot_parent not affected by the rotation itself
  vertex gamma = rot_child->child[1-dir];
    // child of rot_child not affected by the rotation itself

  rot_parent->netcost += rot_parent->netmin;
  rot_child->netmin += rot_parent->netmin;
  rot_child->netcost += rot_child->netmin;

  if(!alpha->external) alpha->netmin += rot_parent->netmin;
  if(!beta->external) beta->netmin += rot_child->netmin;
  if(!gamma->external) gamma->netmin += rot_child->netmin;

  double low_min = leda::min(alpha->netmin,
                            leda::min(rot_parent->netcost,beta->netmin));
  if(!alpha->external) alpha->netmin -= low_min;
  if(!beta->external) beta->netmin -= low_min;
  rot_parent->netmin = low_min;
  rot_parent->netcost -= low_min;

  double high_min = leda::min(rot_parent->netmin,
                             leda::min(rot_child->netcost,gamma->netmin));
  rot_parent->netmin -= high_min;
  if(!gamma->external) gamma->netmin -= high_min;
  rot_child->netmin = high_min;
  rot_child->netcost -= high_min;
}

void vertex_struct::isolate()
// Make this node an isolated node before deletion.
// Prec.: this is a tree root and it is internal.
{
  for(int i=0; i<=1; i++)
  {
    vertex v = this->child[i];
    if(!v->external)
    {
      v->reversed = (v->reversed + this->reversed) % 2;
      v->netmin += this->netmin;
    }
    v->par = nil;
    this->child[i] = nil;
  }
}

/*
void print_path(path p, int reversal_state)
{ cout << " ";
  vertex v = p;
  if ( v->external ) cout << (int) v->x;
  else 
  { reversal_state = (reversal_state + p->reversed )%2;
    //cout << "first tree";
    print_path(p->child[reversal_state],reversal_state);
    //cout << (int) v-> y;  prints edge inf
    //cout << " second tree";
    print_path(p->child[1 - reversal_state],reversal_state);
  }
}
*/
    
LEDA_END_NAMESPACE
