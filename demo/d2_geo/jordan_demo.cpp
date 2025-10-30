/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  jordan_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/plane.h>
#include <LEDA/graphics/window.h>

#include <LEDA/core/array.h>
#include <LEDA/core/list.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/impl/skiplist.h>

#include <LEDA/system/stream.h>


using namespace leda;


bool Jordan_sort(const list<double>&, list<double>&, 
                                      window* Window = 0); 



const double infty = MAXDOUBLE;

double x1; 
list_item x1_item = nil; 

enum SIDE {upper,lower};
SIDE side; 

enum {Left,Right}; 



class intersection; 
typedef intersection* Intersection;

class bracket;
typedef bracket*      Bracket;

list<Intersection> L;


class intersection{
  public:
  double x;
  Bracket containing_bracket_in[2];

  intersection(double xcoord)
  { x = xcoord; 
    containing_bracket_in[upper] = nil; 
    containing_bracket_in[lower] = nil; 
  }
};


int cmp_brackets(const Bracket&,const Bracket&); 
typedef sortseq<Bracket,int,skiplist> children_seq; 

typedef sortseq<Bracket,int,skiplist>::item sq_item; 

class bracket{
  public:
  double left_x;
  list_item endpt[2];
  children_seq children;
  sq_item pos_among_sibs;

  bracket(list_item a, list_item b, SIDE side) : children(cmp_brackets)
  { if (L[a]->x > L[b]->x) leda_swap(a,b);

    left_x = L[a]->x;
 
    endpt[Left] = a; 
    L[a]->containing_bracket_in[side] = this;

    endpt[Right] = b; 
    L[b]->containing_bracket_in[side] = this;
  }

  bracket(double x): children(cmp_brackets) { left_x = x; }

  bool contains(double x)
  { return ( L[endpt[Left]]->x < x && x < L[endpt[Right]]->x ); }
};

int cmp_brackets(const Bracket & b1,const Bracket & b2)
{ return compare(b1->left_x,b2->left_x); }


inline int compare(const Bracket & b1,const Bracket & b2)
{ return cmp_brackets(b1,b2); }

 

list_item L_insert(double x, list_item it, int dir)
{ if ((side == lower && 
     (dir == leda::before && L.pred(it) == x1_item && x < x1)) ||
     (dir == leda::after &&  L.succ(it) == x1_item && x > x1) )
     it = x1_item;
  return L.insert(new intersection(x),it,dir); 
}
 


bool Jordan_sort(const list<double>& In, list<double>& Out,
                                        window* Window)
{ if ( In.length() <= 1 ) {  Out = In; return true; }

  
  x1 = In.head(); 

  L.clear(); 

  list_item minus_infty_item = L.append(new intersection(-infty));  
  list_item xi_item = x1_item = L.append(new intersection(x1)); 
  list_item plus_infty_item = L.append(new intersection(infty)); 

  bracket upper_root(minus_infty_item,plus_infty_item,upper); 
  bracket lower_root(minus_infty_item,plus_infty_item,lower); 
; 

  /* we now process x_2 up to x_n */

  list_item it = In.succ(In.first()); // the second item
  side = upper; 
  while (it)
  { 
    double x = In[it]; 
    double xi = L[xi_item]->x; 
    if (x == xi || x == x1) return false; 
    list_item l_item = L.pred(xi_item); 
    if (l_item == x1_item && side == lower) l_item = L.pred(l_item); 
    list_item r_item = L.succ(xi_item); 
    if (r_item == x1_item && side == lower) r_item = L.succ(r_item); 

    Intersection l = L[l_item]; 
    Intersection r = L[r_item]; 

    Bracket lB = l->containing_bracket_in[side]; 
    Bracket rB = r->containing_bracket_in[side]; 

    list_item    x_item; 

    if (Window != nil)
    { double r = (xi - x)/2; if (r < 0) r = -r; 
     if ( side == upper)
       Window->draw_arc(point(xi,50),point((xi+x)/2,50+r), point(x,50),red); 
     else
       Window->draw_arc(point(xi,50),point((xi+x)/2,50-r),point(x,50),black); 
       
    }

    int dir = ( x > xi ? leda::after : leda::before);

    if (lB == rB)  
      { 
        if (!(lB->contains(x))) return false;

        x_item = L_insert(x,xi_item,dir);
        Bracket new_bracket = new bracket(x_item,xi_item,side); 
        new_bracket->pos_among_sibs = lB->children.insert(new_bracket,0); 
 }
    else 
      { 
        children_seq S(cmp_brackets); // just for the type

        if ( l->x < x && x < r->x )
        { x_item = L_insert(x,xi_item,dir);  
          Bracket new_bracket = new bracket(xi_item,x_item,side);
          new_bracket->pos_among_sibs =
              ( lB->contains(x) ?
                S.insert_at(rB->pos_among_sibs,new_bracket,0,leda::before) :
                S.insert_at(lB->pos_among_sibs,new_bracket,0,leda::after) );       
        }
        else
         if ( dir == leda::after )
         { 
           Bracket query_bracket = new bracket(x); 
           sq_item x_pos = S.finger_locate_pred(rB->pos_among_sibs, 
                                                            query_bracket); 

           Bracket tB = S.key(x_pos);

           list_item next = L.succ(tB->endpt[Right]);
           if ( next == x1_item && side == lower ) next = L.succ(next);

           if ( x <= L[tB->endpt[Right]]->x || x >= L[next]->x )
           return false; 

           x_item = L_insert(x,tB->endpt[Right],leda::after); 

           Bracket new_bracket = new bracket(xi_item,x_item,side); 
           new_bracket->pos_among_sibs =
             S.insert_at(rB->pos_among_sibs,new_bracket,0,leda::before); 

           S.delete_subsequence(rB->pos_among_sibs, x_pos,
                                                     new_bracket->children); 
 }
         else 
         { 
           Bracket query_bracket = new bracket(x); 
           sq_item x_pos = S.finger_locate_succ(lB->pos_among_sibs, 
                                                             query_bracket); 

           Bracket tB = S.key(x_pos); 

           list_item next = L.pred(tB->endpt[Left]);
           if ( next == x1_item && side == lower ) next = L.pred(next);

           if ( x >= L[tB->endpt[Left]]->x || x <= L[next]->x )
             return false;

           x_item = L_insert(x,tB->endpt[Left],leda::before);
            
           Bracket new_bracket = new bracket(x_item,xi_item,side); 
           new_bracket->pos_among_sibs =
             S.insert_at(tB->pos_among_sibs,new_bracket,0,leda::before); 

           S.delete_subsequence(x_pos,lB->pos_among_sibs,
                                                     new_bracket->children); 
 }
 }

    xi_item = x_item; 
; 
    it = In.succ(it); 
    side = ((side == upper)? lower : upper); // change sides
  }

  
  Out.clear(); 
  L.pop(); L.Pop(); 
  forall_items(it,L) Out.append(L[it]->x); 
; 

  return true; 

}



#include <LEDA/core/array2.h>
#include <LEDA/graph/graph.h>
#include <LEDA/numbers/integer.h>
#include <LEDA/core/list.h>
#include <LEDA/core/stack.h>

static int pow_of_two[31];
static bool first = true;

integer rand_integer(integer a, integer b)
{ // generates a random integer in [a..b]
  if (first) 
  { first = false;
    pow_of_two[0] = 1;
    for (int i = 1; i <= 30; i++) pow_of_two[i] = 2*pow_of_two[i-1];
  }

  integer c = b - a;
  // generate r in [0,c]
  integer r;
  int L = c.length();
  do 
  { r = 0;
    while ( L >= 30)
    { r = r*pow_of_two[30] + rand_int(0,pow_of_two[30]-1);
      L = L - 30;
    }
    r = r*pow_of_two[L] + rand_int(0,pow_of_two[L]-1);
  }
  while (r > c);
  return a + r;
}

enum TYPE {j_open, j_closed};

list<int> gen_nested_brackets(int n)
{ list<int> result;
  array2<integer> s(n+1,n+1);
  int i,j;
  for (j = 0; j <= n; j++) s(0,j) = 1;

  for (i = 1; i <= n; i++)
    for (j = 0; j <= n - i; j++)
      { integer d = 0;
        if (j != 0) d = s(i,j-1);
        s(i,j) = s(i-1,j+1) + d;
       }

  int a = 0; int b = 0;
  for (i = 0; i < 2*n; i++)
  { int c = a - b;
    if ( c == 0 ) { result.append(j_open); a++; }
    else 
      if ( a == n ) {result.append(j_closed); b++; }
      else
      { integer x = rand_integer(1,s(n-a,c));
        if ( x <= s(n-a-1,c+1) )
          { result.append(j_open); a++; }
        else
          { result.append(j_closed); b++; }
      }
  }
  return result;
}


list<int> simple_gen_nested_brackets(int n)
{ list<int> result;
  
  int a = 0; int b = 0;
  for (int i = 0; i < 2*n; i++)
  { int c = a - b;
    if ( c == 0 ) { result.append(j_open); a++; }
    else 
      if ( a == n ) {result.append(j_closed); b++; }
      else
      { // still n - a open and n - b closed
        integer x = rand_int(1,n - a + n - b);
        if ( x <= n - a )
          { result.append(j_open); a++; }
        else
          { result.append(j_closed); b++; }
      }
  }
  return result;
}

#include <LEDA/graph/node_partition.h>

list<int> Jordan_gen(int n)
{ array<node> V(2*n);

  GRAPH<int,edge> G;
  int i;

  for (i = 0; i < 2*n; i++) V[i] = G.new_node(i);

  list<int> A;

  if (n < 500)
    A = gen_nested_brackets(n);
  else
    A = simple_gen_nested_brackets(n); 
  
  node_partition P(G);
  stack<int> S;
  for (i = 0; i < 2*n; i++)
  { if ( A.pop() == j_open ) S.push(i);
    else 
    { int j = S.pop();
      edge e = G.new_edge(V[i],V[j]);
      G[e] = G.new_edge(V[j],V[i],e);
      P.union_blocks(V[i],V[j]);
    }
  }


  for (i = 0; i < 2*n; i++)
  if ( S.empty() || P.same_block(V[i],V[S.top()]) ) S.push(i); 
  else
  { int j = S.pop();
    edge e = G.new_edge(V[i],V[j]);
    G[e] = G.new_edge(V[j],V[i],e);
    P.union_blocks(V[i],V[j]);
  }
    
  G.make_map();

  node end = V[S.pop()];
  list<int> result; 
  node v = V[S.pop()];
  edge e = G.first_adj_edge(v);

  result.append(G[v]);
  node w;
  do
  { w = G.target(e);
    result.append(G[w]);
    e = G.face_cycle_succ(e);
  } while ( w != end );

  return result;
}


int main()
{ window W("Jordan Sorting"); W.display();

  while (true)
  { W.clear();

    list<double> In,Out; 

    list<point> L; 
    point p; 
    W.message("This demo illustrates Jordan sorting.");
    W.message("The input is an open or closed polygonal curve C.");
    W.message("Let L be the list of intersections of C with the x-axis ");
W.message("in the order in which the intersections appear on C. ");
W.message("Jordan sort takes L and generates the list of intersections ");
W.message("in their order on the x-axis. ");
W.message("It does so in linear time (although with a fairly large constant).");
    W.message("");
    W.message("The input is either read from a file, drawn \
manually, or program generated.");
    W.message("Try manual input first (select edit)");
    string* S; 
    S  = new string[4]; 
    S[0] = "file"; 
    S[1] = "edit";
    S[2] = "generator"; 
    S[3] = "quit";

    int c = W.read_panel("what kind of input?",4,S);
    if ( c == 3) break;
    W.del_message();
    if ( c <= 1 )
    { if ( c == 0)
      { file_istream I(W.read_string("file name")); 
        while (I >> p)   L.append(p); 
        I.close(); 
      }
      else
      {

      W.message("Please input a closed simple polygon in \
clockwise orientation.");
W.message("Every click of the left mouse button generates a vertex, ");
W.message("a click of the middle button terminates the input."); 
      W.message(""); 
      W.message("The program then intersects the polygon with the x - axis and"); 
      W.message("sorts the intersections into left to right order. It runs in"); 
      W.message("linear time. If the input polygon is not simple then it may"); 
      W.message("return that the input is not simple"); 

      W.message("");
      W.message("Press any button to continue the demo.");
 
      W.read_mouse();
      W.del_message(); 

      line x_axis(point(0,50),0); // horizontal line through point (0,50)
      W.draw_line(x_axis,red); 

      polygon P1; 
      W >> P1; 
      L = P1.vertices(); 

      if (W.confirm("want to safe?"))
      { 
        file_ostream O(W.read_string("file name")); 
        L.print(O); 
        O.close(); 
      }
    } // end else if c == 0
    /* L is the list of vertices of a simple polygon */
    line x_axis(point(0,50),0); 
    W.draw_line(x_axis,red); 

    polygon P(L); 

    L.clear(); 
    L = P.intersection(x_axis); 
    int i = 0; 
    forall(p,L)
    { In.append(p.xcoord()); 
      W.draw_point(p,yellow); 
      W.draw_ctext(p.xcoord(),52,string("%d",i++),blue); 
    }
    W.message("The display shows the input to procedure Jordan_sort:"); 
    W.message("a list of real numbers: the i - th number in the list"); 
    W.message("is labelled i."); 
    W.message(" "); 
    W.message("The program will sort these numbers and in the process of sorting"); 
    W.message("reconstruct the Jordan curve and the two nested sets of parenthesis"); 
    W.message("above and below the x - axis"); 
    W.message("");
    W.message("Press any button to continue the demo.");
 
    W.read_mouse();
    W.del_message();
 

    if ( Jordan_sort(In,Out,&W) )
    { double x; 
      int i =0; 
      forall(x,Out) W.draw_ctext(x,48,string("%d",i++),blue); 
    }
    else  W.draw_ctext(50,80,"polygon is not simple"); 
    } // end if c <= 1
    else
    { // generate input 
      int n = W.read_int("number of brackets = ");
      list<int> L = Jordan_gen(n);
      list<double> In, Out;
      int i;
      forall(i,L) In.append(10 + (((double) i) * 40)/n) ;  
      Jordan_sort(In,Out,&W);
    }

    W.message("");
    W.message("Press any button to continue the demo.");
 
    W.read_mouse();

    //int n = read_int("type a nonzero number to continue ");
    //if ( n == 0 ) break;

  }
  return 0;
}

