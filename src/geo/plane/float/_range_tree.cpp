/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _range_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/range_tree.h>

LEDA_BEGIN_NAMESPACE 

// Let's start with some simple member functions of class rt_elem

// the 2-dimensional constructor
//
rt_elem::rt_elem(GenPtr k0, GenPtr k1, GenPtr i) {
  rt_keys=new GenPtr[2];
  rt_keys[0]=k0; rt_keys[1]=k1;
  rt_inf=i;
}

// the 3-dimensional constructor
//
rt_elem::rt_elem(GenPtr k0, GenPtr k1, GenPtr k2, GenPtr i) {
  rt_keys=new GenPtr[3];
  rt_keys[0]=k0; rt_keys[1]=k1; rt_keys[2]=k2;
  rt_inf=i;
}

// the d-dimensional constructor
//
rt_elem::rt_elem(int dim, GenPtr* k, GenPtr i) {
  rt_keys=new GenPtr[dim];
  for( int d=0; d<dim; d++ )
    rt_keys[d]=k[d];
  rt_inf=i;
}



// And here are the member functions of class range_tree

// insert new_elem into the primary tree and update its secondary 
// structures if lev<dim-1
//
void range_tree::rt_insert( rt_item new_elem )
{

#ifdef DEBUG
  cout << endl;
  for( int auxl=0; auxl<lev; auxl++ ) cout << "\t";
  cout << "insert " << (int) new_elem->inf() << ":   "; cout.flush();
#endif

  if( lev<dim-1 ) {			// take care of secondary structures

    base_tree_item p, new_leaf;
    range_tree* sec;

    // insert the new element into the primary tree 
    //
    aux.clear();
    new_leaf = base_tree::insert( new_elem, new_range_tree(dim,lev+1), 
  				            new_range_tree(dim,lev+1) );
  
    // now we insert the new element into all secondary structures of 
    // nodes on the path from the root to the new leaf with a non-empty
    // secondary structure (remember that these nodes are not in aux)
    //
    p = get_root();
    while( p ) {
      sec = (range_tree*) inf(p);
      if( sec->size() )
        sec->rt_insert(new_elem);
      if( cmp(new_elem,key(p))<=0 )
        p = l_child(p);
      else
        p = r_child(p);
    }

    // and into the (empty) secondary structure of the new leaf
    //
    ((range_tree*) inf(new_leaf))->rt_insert(new_elem);

    // for all nodes appended to aux by the function propagate_change(), 
    // we rebuild the secondary structure from scratch
    //
    rt_item* elem_array = new rt_item[size()];		// array of elements
    int elem_no;					// number of elements

    forall( p, aux ) {
      elem_no = elements_in_subtree(elem_array, p);
      ((range_tree*) inf(p))->build_tree(elem_array, 0, elem_no-1);
    }
  
    delete[] elem_array;
  }
  else
    // for lev==dim-1 the range tree is just an ordinary search tree
    //
    base_tree::insert( new_elem, 0, 0 );
}





// recursively build a range tree for the elements 
// elem_array[lidx],...,elem_array[ridx]
// if p==0 we build the tree on this level and the secondary structure of the
// root, otherwise we just build the secondary structure of p
//
void range_tree::build_tree( rt_item* elem_array, int lidx, int ridx, 
			     base_tree_item p )
{

#ifdef DEBUG
  cout << endl;
  for( int gg=0; gg<lev; gg++ )
    cout << "\t";
  cout << "build_tree " << p << ":   ";
  cout.flush();
#endif

  int i;

  // the last level of the range tree is just a binary search tree
  //
  if( lev==dim-1 ) {
    for( i=lidx; i<=ridx; i++ ) {

#ifdef DEBUG
      cout << endl;
      for( int gg=0; gg<lev; gg++ )
        cout << "\t";
      cout << "insert " << (int) elem_array[i]->inf() << ":   ";
      cout.flush();
#endif

      base_tree::insert( elem_array[i], 0, 0 );
    }
    return;
  }

  // we are entering this level for the first time, therefor we have
  // to insert all elements and sort them according to the new level
  //
  if( !p ) {
    for( i=lidx; i<=ridx; i++ ) {

#ifdef DEBUG
      cout << endl;
      for( int gg=0; gg<lev; gg++ )
        cout << "\t";
      cout << "insert " << (int) elem_array[i]->inf() << ":   ";
      cout.flush();
#endif

      base_tree::insert( elem_array[i], new_range_tree(dim,lev+1), 
                                        new_range_tree(dim,lev+1) );
    }
    p = get_root();
    elem_array = new rt_item[ridx-lidx+1];
    lidx = 0;
    ridx = elements_in_subtree(elem_array,p)-1;	/* get sorted array */
  }

  // build the secondary structure
  //
  ((range_tree*) inf(p))->build_tree(elem_array, lidx, ridx);

  // if p is an inner node, we recursively build the secondary structures
  // of its children
  //
  if (is_inner(p)) {
    int l=lidx, r=ridx, med=(l+r)/2, c=cmp(elem_array[med],key(p));

    // "split" the array at key(p)
    //
    while( c ) {
      if( c>0 )
	r = med-1;
      else
	l = med+1;
      med = (l+r)/2;
      c = cmp(elem_array[med], key(p));
    }

    if (r_child(p))
      build_tree(elem_array, med + 1, ridx, r_child(p));
    if (l_child(p))
      build_tree(elem_array, lidx, med, l_child(p));
  }

  // free the memory of the array elem_array, if we are done
  //
  if( p==get_root() )
    delete[] elem_array;
}



// compute a sorted array (according to the actual level) of all 
// elements in the subtree rooted at subroot and return their number
//
int range_tree::elements_in_subtree( rt_item* elem_array, 
				     base_tree_item subroot )
{
  int elem_no=0;
  base_tree_item p=subroot, q=subroot;

  while( is_inner(p) )			// find leftmost leaf
    p = l_child(p);
  while( is_inner(q) )			// find rightmost leaf
    q = r_child(q);

  while( p!=q ) {			// collect all elements inbetween
    elem_array[elem_no++] = rt_item(key(p));
    p = succ(p);
  }
  elem_array[elem_no++] = rt_item(key(q));

  return elem_no;			// return the number of elements
}




// return a list of all elements in the tree whose key is between
// left and right on each level
//
void range_tree::rt_query( rt_item& Left, rt_item& Right, 
			   list<rt_item>& res ) const
{
  if( size()>0 ) {			// avoid special case
    base_tree_item p, q;

    if( lev<dim-1 ) {			// we have to perform recursive quieries

      // find the last node common to both search paths
      //
      p = get_root();
      while( is_inner(p) ) {
	if( cmp(Right,key(p))<=0 )
	  p = l_child(p);
	else if( cmp(Left,key(p))>0 )
	  p = r_child(p);
	else
	  break;
      }

      if( is_inner(p) ) {

        // traverse the left subpath
	//
	q = l_child(p);
	while( is_inner(q) ) {
	  if( cmp(Left,key(q))<=0 ) {
	    if( r_child(q) )
	      // recursively query all nodes right to the subpath
	      //
	      ((range_tree*) inf(r_child(q)))->rt_query(Left,Right,res);
	    q = l_child(q);
	  }
	  else
	    q = r_child(q);
	}
	if( cmp(Left,key(q))<=0 && cmp(Right,key(q))>=0 )
	  ((range_tree*) inf(q))->rt_query(Left,Right,res);

        // traverse the right subpath
	//
	q = r_child(p);
	while( is_inner(q) ) {
	  if( cmp(Right,key(q))>0 ) {
	    if( l_child(q) )
	      // recursively query all nodes left to the subpath
	      //
	      ((range_tree*) inf(l_child(q)))->rt_query(Left,Right,res);
	    q = r_child(q);
	  }
	  else
	    q = l_child(q);
	}
	if( cmp(Left,key(q))<=0 && cmp(Right,key(q))>=0 )
	  ((range_tree*) inf(q))->rt_query(Left,Right,res);
      }
      else {
        // we only have to look at one leaf
	//
	if( cmp(Left,key(p))<=0 && cmp(Right,key(p))>=0 )
	  ((range_tree*) inf(p))->rt_query(Left,Right,res);
      }
    }
    else {
      // append all elements between left and right on level dim-1
      // to the res
      //
      p = locate_succ(Left);
      while( p && cmp(key(p),Right)<=0 ) {
        res.append( rt_item(key(p)) );
        p = succ(p);
      }
    }
  }
}



// delete elem in the primary tree and update its secondary 
// structures if lev<dim-1
//
void range_tree::rt_del( rt_item elem )
{
  if( lev<dim-1 ) {			// take care of secondary structures
    base_tree_item p;

    // delete elem in all secondary structures on the search path to elem
    //
    p = get_root();
    while( is_inner(p) ) {
      ((range_tree*) inf(p))->rt_del(elem);
      if( cmp(elem,key(p))<=0 )
        p = l_child(p);
      else
        p = r_child(p);
    };

    // and in the primary tree
    //
    aux.clear();
    base_tree::del_item(p);

    // for all nodes appended to aux by the function propagate_change(), 
    // we rebuild the secondary structure from scratch
    //
    rt_item* elem_array = new rt_item[size()];		// array of elements
    int elem_no;					// number of elements

    forall( p, aux ) {
      elem_no = elements_in_subtree(elem_array, p);
      ((range_tree*) inf(p))->build_tree(elem_array, 0, elem_no-1);
    }
  
    delete[] elem_array;
  }
  else
    // for lev==dim-1 the range tree is just an ordinary search tree
    //
    base_tree::del(elem);
}



// return a list of all elements in the tree
//
list<rt_item> range_tree::all_items() const
{
  list<rt_item> res;

  if( !empty() ) {
    base_tree_item p=base_tree::min(),
                   q=base_tree::max();
    res.append( (rt_item) key(p) );
    while( p!=q ) {
      p = cyclic_succ(p);
      res.append( (rt_item) key(p) );
    }
  }
  return res;
}



// compute minimum element on a given level
//
rt_item range_tree::rt_min( int d ) const
{
  if( empty() ) return 0;
  if( lev<d )					// proceed to next level
    return ((range_tree*) inf(get_root()))->rt_min(d);
  else 
    return( (rt_item) key(base_tree::min()) );
}



// compute maximum element on a given level
//
rt_item range_tree::rt_max( int d )  const {
  if( empty() ) return 0;
  if( lev<d )					// proceed to next level
    return ((range_tree*) inf(get_root()))->rt_max(d);
  else 
    return( (rt_item) key(base_tree::max()) );
}

LEDA_END_NAMESPACE 
