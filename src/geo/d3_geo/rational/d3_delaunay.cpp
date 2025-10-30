/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_delaunay.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_delaunay.h>


// d3_complex.h
// Header-file for Simplexkomplex, Vertex, Simplex, Simpl_Triang

#include <LEDA/graph/graph.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/queue.h>
#include <LEDA/core/array.h>
#include <LEDA/numbers/rat_vector.h>
#include <LEDA/geo/d3_rat_plane.h>
#include <LEDA/geo/d3_rat_sphere.h>
#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/geo/d3_hull.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE


class d3_simpl_complex;
class Simplex; 
class D3_vertex;

typedef D3_vertex* sc_vertex;
typedef Simplex* sc_simplex;


class D3_vertex{
  // friend classes
 friend class d3_simpl_complex;
 friend class Simplex;
 friend class Simpl_Triang;

 // friend functions
 friend int compare (const D3_vertex& T1, const D3_vertex& T2);
 friend ostream& operator<<(ostream& s, const D3_vertex& T);
 friend istream& operator>>(istream& s, D3_vertex& T);

public:
 D3_vertex(const D3_vertex& S);
 void* helper;

private:
 d3_rat_point stor;

 d3_simpl_complex* com_of; 
 list_item it;             //position in vertex-list
 sc_simplex my;            //a simplex, where the vertex belongs to...

 D3_vertex(d3_rat_point w,d3_simpl_complex* c);

 D3_vertex(); 

};

inline int compare(const D3_vertex& T1, const D3_vertex& T2)
{ return compare(T1.stor,T2.stor); }

inline ostream& operator<<(ostream& s, const D3_vertex& T){ return s << T.stor;}
inline istream& operator>>(istream& s, D3_vertex& T){ return s;}

class Simplex { 
 // friend classes
 friend class d3_simpl_complex;
 friend class D3_vertex;
 friend class Simpl_Triang;

 // friend functions
 friend int compare (const Simplex&, const Simplex&);
 friend ostream& operator<<(ostream&, const Simplex&);
 friend istream& operator>>(istream&, Simplex&);

public:
 void* helper;

private:
 bool HULL_TR;       
 sc_vertex a,b,c; 
 sc_vertex d;      
 sc_simplex abc;
 bool abcdia;
 sc_simplex abd;       // 4 links...
 bool abddia;
 sc_simplex acd;
 bool acddia;
 sc_simplex bcd;       
 bool bcddia;

 string name; 

 int Marker;
 d3_simpl_complex* com_of; //complex of the simplex  
 list_item it;             //position in the simplex-list

 list<sc_simplex> all_neighbours() const;

 Simplex(sc_vertex Ap,sc_vertex Bp,sc_vertex Cp);

 Simplex(sc_vertex Ap,sc_vertex Bp,sc_vertex Cp,sc_vertex Dp);

 Simplex(const Simplex& S);

 void init(sc_simplex s1,sc_simplex s2,sc_simplex s3,sc_simplex s4);

 void init(sc_simplex s1,sc_simplex s2,sc_simplex s3);

 void update(sc_simplex old, sc_simplex neu);

 void change(sc_vertex k1,sc_vertex k2,sc_vertex k3,sc_vertex k4);

 void change(sc_vertex k1,sc_vertex k2,sc_vertex k3);

 void clear_links();

 sc_vertex find_other(sc_vertex p1,sc_vertex p2,sc_vertex p3);

 int index(sc_vertex p);

 sc_vertex vertex(int i);

 void set_link(sc_simplex S2,int i);

 bool in_simplex(d3_rat_point p);

 bool insphere(d3_rat_point p);

 int control_vert(sc_vertex v1,sc_vertex v2,sc_vertex v3);

 int control_vert(sc_vertex v1,sc_vertex v2);

 int link_new(sc_simplex ot);
  
 d3_rat_point circle_midpoint();

 d3_rat_point sphere_midpoint();

 rational vol();
};

inline int compare(const Simplex& T1, const Simplex& T2)
{ if (T1.a==T2.a && T1.b==T2.b && T1.c==T2.c && T1.d==T2.d) return 1;
  return 0;
}

inline ostream& operator<<(ostream& s, const Simplex& T)
{
 if (T.HULL_TR) {
   s << "Hulltriangle, ";
   s << T.name << *(T.a) << " " << *(T.b) << " " << *(T.c) << "\n";
 }
 else  s << T.name << *(T.a) << " " << *(T.b) << " " << *(T.c) << " " << *(T.d) << "\n";

 list<sc_simplex> an;
 an=T.all_neighbours();
 s << an.size() << " neighboursettings!\n";

 s << "abc:";
 if (T.abc == NULL) s << "-"; else s << (*(T.abc)).name;
 s << " abd:";
 if (T.abd == NULL) s << "-"; else s << (*(T.abd)).name;
 s << " acd:";
 if (T.acd == NULL) s << "-"; else s << (*(T.acd)).name;
 s << " bcd:";
 if (T.bcd == NULL) s << "-"; else s << (*(T.bcd)).name; 

 s << "\n";

 s << "Marker:" << T.Marker << "\n";
 return s;
}

inline istream& operator>>(istream& s, Simplex& T)
{ s >> T.Marker;  return s; }

inline istream& operator>>(istream& s, Simplex*&)
{ return s; }



class Simpl_Triang{

public:
 sc_simplex S1; // 2 simplices who share the Simpl_Triang
 sc_simplex S2;

 sc_vertex A,B,C;

 Simpl_Triang();

 Simpl_Triang(sc_simplex sim1,sc_simplex sim2,sc_vertex Ap,sc_vertex Bp,sc_vertex Cp);

 Simpl_Triang(const Simpl_Triang& S);
 
};

inline int compare(const Simpl_Triang& T1, const Simpl_Triang& T2)
{
 if ( ( (T1.S1) == (T2.S1) && (T1.S2) == (T2.S2)) ||
      ( (T1.S2) == (T2.S1) && (T1.S1) == (T2.S2)) )
 return 0;
 else return 1;
}

inline ostream& operator<<(ostream& s, const Simpl_Triang& T){ 
 if (T.A !=NULL) s << *(T.A); else s << "NULL ";
 if (T.B !=NULL) s << *(T.B); else s << "NULL ";
 if (T.C !=NULL) s << *(T.C); else s << "NULL ";
 return s;
}

inline istream& operator>>(istream& s, Simpl_Triang& T){ return s;}



/*{\Manpage {d3_simpl_complex} {} {Simplexcomplex in 3d space} {C}}*/

class d3_simpl_complex{

/*{\Mdefinition
A |\Mname| is a container storing vertices (|sc_vertex|) and simplices (|sc_simplex|). The simplices can be
linked together, so that they form neighbourhood-relations.\\
Every vertex stores a |d3_rat_point|. Every simplex consists of 4 (or 3) vertices and has the ability to store
4 links to neighbour simplices. There simplices consisting of 3 vertices are called hulltriangles. Two  simplices can
be neighbours, if they have 3 vertices together, or if they have 2 vertices together and both are hulltriangles.
}*/

 private:
  //flipcounter
  int flk30,flk31,flk32,flk33,flk34,flk35; 
  int flk1,flk10,flk11,flk12; 

  list<sc_simplex> Sil;
  list<sc_vertex> Vert;
  sc_simplex hull_last;  // last inserted hull triangle

 public:
  void reset_flip_counters();

  void output_flip_counters();

  friend void DELAUNAY_FLIPPING(d3_simpl_complex&);

  friend sc_simplex comp_initial_tr(list<d3_rat_point>&,d3_simpl_complex&,d3_rat_point&,list<sc_simplex>&);
  friend void find_visible_triangles(d3_simpl_complex&,list<sc_simplex>&,list<Simpl_Triang>&,sc_simplex,d3_rat_point,int,list<sc_simplex>&);
  friend sc_simplex triangulate_new(d3_simpl_complex&,list<sc_simplex>&,list<Simpl_Triang>&,d3_rat_point,stack<Simpl_Triang>&,list<sc_simplex>&);

/*{\Mcreation}*/

  d3_simpl_complex();
/*{\Mcreate creates a Simplexcomplex in 3d space.}*/

  ~d3_simpl_complex();

/*{\Moperations 2.5 4}*/

  sc_simplex get_hulltriangle()
/*{\Mop   returns a hulltriangle (if there is one in the complex; otherwise NULL).}*/
  { return hull_last; }

  int number_of_vertices()
/*{\Mop   returns the number of vertices in |C|.}*/  
  { return Vert.length(); }
  
  int number_of_simplices()
/*{\Mop   returns the number of simplices in |C|.}*/
  { return Sil.length(); }

  list<d3_rat_point> points();
/*{\Mop   returns the points in the complex.}*/

  const list<sc_simplex>& all_simplices()
/*{\Mop   returns the list of all simplices of |C|.}*/
  { return Sil; }

  list<sc_vertex> all_vertices()
/*{\Mop   returns the list of all vertices of |C|.}*/
  { return Vert; }

  d3_rat_point pos(sc_vertex v)
/*{\Mop   returns the position of vertex |v|.}*/
  { return v->stor; }
  
  void set_vertex(sc_simplex sim,sc_vertex v,int i)
  {
   if (i==1) (*sim).a=v;
   if (i==2) (*sim).b=v;
   if (i==3) (*sim).c=v;
   if (i==4) (*sim).d=v;
  }

  bool get_hull_info(sc_simplex sim) { return sim->HULL_TR; }
  void set_hull_info(sc_simplex sim,bool val) { (*sim).HULL_TR=val; }
  void set_vert_sim_link(sc_vertex v,sc_simplex s) { (*v).my=s; }
 
  sc_vertex vertex_a(sc_simplex sim)
/*{\Mop   returns the |sc_vertex a| of |sc_simplex sim|.}*/
  { return sim->a; }

  sc_vertex vertex_b(sc_simplex sim)
/*{\Mop   returns the |sc_vertex b| of |sc_simplex sim|.}*/
  { return sim->b; }

  sc_vertex vertex_c(sc_simplex sim)
/*{\Mop   returns the |sc_vertex c| of |sc_simplex sim|.}*/
  { return sim->c; }

  sc_vertex vertex_d(sc_simplex sim)
/*{\Mop   returns the |sc_vertex d| of |sc_simplex sim|.}*/
  { return sim->d; }

  sc_simplex link_abc(sc_simplex sim)
/*{\Mop  returns the first linked |sc_simplex| of |sim|.}*/
  { return sim->abc; }

  sc_simplex link_abd(sc_simplex sim)
/*{\Mop  returns the second linked |sc_simplex| of |sim|.}*/
  { return sim->abd; }

  sc_simplex link_acd(sc_simplex sim)
/*{\Mop  returns the third linked |sc_simplex| of |sim|.}*/
  { return sim->acd; }

  sc_simplex link_bcd(sc_simplex sim)
/*{\Mop  returns the fourth linked |sc_simplex| of |sim|.}*/
  { return sim->bcd; }

  void set_link(sc_simplex sim,sc_simplex other,int i) 
/*{\Mop  sets |i-th| link |(1<=i<=4)| from |sim| to |other|.}*/
  { (*sim).set_link(other,i); }

  void update_link(sc_simplex sim,sc_simplex old,sc_simplex newsim)
/*{\Mop  trys to update to link from |sim| to |old|.}*/
  { (*sim).update(old,newsim); }

  d3_simpl_complex* complex_of(sc_vertex v)
/*{\Mop  returns a pointer to the simplexcomplex that |v| belongs to.}*/
  { return (*v).com_of; }

  d3_simpl_complex* complex_of(sc_simplex s)
/*{\Mop  returns a pointer to the simplexcomplex that |s| belongs to.}*/
  { return (*s).com_of; }

  list<sc_simplex> all_neighbours(sc_simplex sim) const;
/*{\Mop  returns all neighboursimplices from |sim|.}*/

  sc_vertex find_other(sc_simplex sim,sc_vertex p1,sc_vertex p2,sc_vertex p3)
/*{\Mop  returns a vertex not element |{p1,p2,p3}| from |sim|.}*/
  { return (*sim).find_other(p1,p2,p3); }

  int get_Marker(sc_simplex& sim) { return sim->Marker; }

  void set_Marker(sc_simplex& sim,int mk) { (*sim).Marker=mk; } 

  void clear();
/*{\Mop   clears |C| (deletes all simplices and vertices).}*/

  void reset_markers();

  sc_simplex get_simplex(sc_vertex v);
/*{\Mop   returns a simplex, that contains |v|.}*/

  sc_simplex opposite_simplex(sc_simplex S,int i);
/*{\Mop   returns a simplex, that is opposite to the i-th vertex of simplex |S|.}*/

  sc_simplex opposite_simplex(sc_simplex S,sc_vertex v);
/*{\Mop   returns a simplex, that is opposite to vertex v of |S|.}*/

  sc_vertex opposite_vertex(sc_simplex S,int i);
/*{\Mop   returns a vertex, that is opposite to the i-th vertex of simplex |S|.}*/

  sc_vertex opposite_vertex(sc_simplex S,sc_vertex v);
/*{\Mop   returns a vertex opposite to vertex v of |S|.}*/

  sc_vertex add_vertex(d3_rat_point p);
/*{\Mop   adds a vertex, that stores p, to |C|.}*/

  sc_simplex add_simplex(sc_vertex a,sc_vertex b,sc_vertex c);
/*{\Mop   adds a degenerated simplex (hulltriangle) to |C| and returns it.}*/

  sc_simplex add_simplex(sc_vertex a,sc_vertex b,sc_vertex c,sc_vertex d);
/*{\Mop   adds a nondegenerated simplex to |C| and returns it.}*/

  sc_simplex insert_vertex(sc_simplex S,sc_vertex v);
/*{\Mop   inserts |v| into |S| and triangulates new (precondition: |v| inside |S|).}*/

  sc_simplex triang_simplex(sc_simplex S,sc_vertex v);
  sc_simplex triang_simplex(sc_simplex S,d3_rat_point p);
  sc_simplex triang_simplex(sc_simplex S,sc_vertex v,int st);
  // den Simplex S mit Hilfe des darinliegenden Punktes p (bzw. vertex) aufteilen
  // (neu triangulieren)

  sc_simplex search_simplex(d3_rat_point p); 
/*{\Mop   searches the simplex in |C| with |p| inside and returns it.}*/

  sc_simplex search_simplex(sc_vertex v);
/*{\Mop   searches the simplex in |C| with |v| inside and returns it.}*/

  sc_simplex wtsearch_simplex(d3_rat_point p);
  // search using walkthrought method

  sc_simplex wtsearch_simplex(sc_vertex v);
  // search using walkthrough method

  int remove_simplex(sc_simplex s);
/*{\Mop   removes |s| from |C|; if the function is successful, 1 is returned.}*/

  bool flippable(sc_simplex S1, sc_vertex v);
/*{\Mop   returns true, if the side of |S1|, that doesnt contain |v|, is flippable, false otherwise.}*/

  bool flippable(sc_simplex S1, sc_simplex S2,Simpl_Triang& Trl);
/*{\Mop   returns true, if the side between |S1| and |S2| is flippable, false otherwise.}*/

  void init_dia(bool w);
  // initialices all diagram/nondiagramm-informations.

  int  set_dia_nondia();
  // sets diagram/nondiagramm-informations

  bool local_delaunay(sc_simplex S1);
/*{\Mop   tests,if |S1| is local delaunay.}*/

  bool global_delaunay(sc_simplex S1);
/*{\Mop   tests.if |S1| is global delaunay.}*/

  int flip(sc_simplex S1,sc_simplex S2,Simpl_Triang& Trl);
/*{\Mop   trys to flip the side between |S1| and |S2|; if successful, a value \>0 is returned.}*/
  // S1 und S2 muessen im Komplex sein und Triangle gemeinsam haben...

  int flip(sc_simplex S1,sc_simplex S2,queue<Simpl_Triang>& TQ,Simpl_Triang& T,sc_vertex PV);
  // wie voriger flip, aber die Grenzdreiecke der geflippten beiden Simplices werden in der
  // TL-Liste zurueckgegeben

  void input_complex(); // Eingabe des Simplexcomplex fuer debugging-Zwecke

  int get_link_index(sc_simplex S1,sc_simplex S2);
/*{\Mop   returns the number of the link from |S1|, that points to |S2| (|<=0| - no link).}*/
   
  sc_simplex set_link(sc_simplex S1,int i,sc_simplex S2,int j);
/*{\Mop   sets the neighbourhood relation between |S1| and |S2|. (|i| and |j| are the numbers 
          of the noncommon vertices); if not successful (|S1| and |S2| no neighbours or not in the same
          complex), NULL is returned.
}*/

  sc_simplex set_link(sc_simplex S1,sc_simplex S2, sc_vertex v1, sc_vertex v2, sc_vertex v3);
/*{\Mop   sets the neighbourhood relation between |S1| and |S2|. (|v1,v2,v3| are the 3 
          common vertices).}*/

  sc_simplex set_link(sc_simplex S1,sc_vertex i,sc_simplex S2,sc_vertex j);
/*{\Mop   sets the neighbourhood relation between |S1| and |S2|. (|i| and |j| are the 2 noncommon 
          vertices)}*/   

  int check(int output,ostream& ost);
/*{\Mop   checks the complex, results go to |ost|. |output==0|: no output, |output==1|: reports errors and
          number of simplices/vertices, |output==2|: like 1 and output of all vertices/simplices.
          returned value==0 - no errors, returned value != 0 - errors.
}*/

  bool check_vert_sim_links();
/*{\Mop   checks, if all vertices in the complex have a valid linksetting to a simplex.}*/

  bool delaunay_check(int w);
/*{\Mop   checks all simplices in |C| for delaunay property; if all simplices have this property, the
          function returns true, otherwise false. |w==1| : checks local delaunay property, |w==2| : checks
          global delaunay property.
}*/ 

  bool all_delaunay_check(); // global delaunay check
  int check_delaunay(); // local delaunay check- gibt die Zahl der simplices zurueck, die nicht local del.

  bool compute_voronoi(GRAPH<d3_rat_sphere,int>& Gout);
/*{\Mop   uses the complex to compute a GRAPH |Gout| describing the voronoi-diagramm of the vertices of the complex.
          If all simplices are local delaunay, the function computes |Gout| and returns true; otherwise the function
          returns false.
}*/

  bool compute_voronoi(GRAPH<d3_rat_point,int>& Gout,bool hflag);
/*{\Mop   uses the complex to compute a GRAPH |Gout| describing the voronoi-diagramm of the vertices of the complex.
          If all simplices are local delaunay, the function computes |Gout| and returns true; otherwise the function
          returns false.\\
          |hflag==true| : edges to hulltriangles and between hulltriangles inserted in |Gout|\\
          |hflag==false|: no edges to hulltriangles and between hulltriangles inserted in |Gout|\\
}*/

  bool get_incident_simplices(sc_vertex v,list<sc_simplex>& ret);
/*{\Mop   finds |v's| incident simplices and returns them in |ret|. If |v| is not in |C|, false is returned.}*/

  bool get_incident_points(sc_vertex v,list<d3_rat_point>& dl);

  void kill_vlists(); // deletes the lists with voronoi-nodes associated with the vertices.

  bool make_vlists(); // computes the lists with voronoi-nodes associated with the vertices.}*/

  void compute_graph(sc_simplex sakt,GRAPH<d3_rat_point,int>& Gout);
/*{\Mop   computes a GRAPH |Gout|  containing |sakt| and its neighbours.}*/

  void compute_surface_graph(GRAPH<d3_rat_point,int>& Gout);
/*{\Mop   computes a GRAPH |Gout| only containing the hulltriangles of the complex.}*/

  void compute_graph(GRAPH<d3_rat_point,int>& Gout);
/*{\Mop   computes from |C| the GRAPH |Gout|.}*/

/*
  void compute_graph(GRAPH<d3_rat_point,int>& Gout,set<sc_vertex>& DS,list<edge>& erg);
*/
/*{\Mop   computes from |C| the GRAPH |Gout|; the edges between members of |DS| are returned
          in |erg|.}*/

  int common_triangle(sc_simplex S1, sc_simplex S2, Simpl_Triang& T);
/*{\Mop   finds the common |Simpl_Triang| between |S1| and |S2|, returns it in |T|.}*/

  int configuration(sc_simplex S1, sc_simplex S2, Simpl_Triang& T);
/*{\Mop   returns the configuration (see [  ]) of the simplices |S1| and |S2| (they must be neighbours;
          otherwise the function returns 0); in T the common triangle is returned.
}*/
  void link_save(sc_simplex SI, sc_simplex pt1, sc_simplex pt2, list<sc_simplex>& L);
  void link_korr(sc_simplex SI, list<sc_simplex>& L,list<Simpl_Triang>& TRL);
  void link_korr(sc_simplex SI, list<sc_simplex>& L,queue<Simpl_Triang>& TQ,sc_vertex PV);

  void flip_konf3(sc_simplex S1, sc_simplex S2,d3_simpl_complex* Cl,int konf,Simpl_Triang& T,queue<Simpl_Triang>& TQ,sc_vertex PV);
  // flipping in configuration 3 

  void flip_konf1(sc_simplex S1, sc_simplex S2,d3_simpl_complex* Cl,int konf,Simpl_Triang& T,queue<Simpl_Triang>& TQ,sc_vertex PV);
  // flipping in configuration 1

  int d2_test_convex(sc_vertex a,sc_vertex b,sc_vertex c,sc_vertex d);
  // returns 1, if the 4 points stored bye 4 D3-vertices, that lie in a plane (!),
  // form a convex quadrangle.
};



bool in_sim(const d3_rat_point& p1,const d3_rat_point& p2,const d3_rat_point& p3,const d3_rat_point& cand);

bool test_inside(d3_rat_point a,d3_rat_point b,d3_rat_point c,d3_rat_point other);
// returns true, if the 3 d3_rat_point's a,b,c (they have to lie in one plane)
// form a circle containing other.




 int join_or_link(GRAPH<d3_rat_sphere,int>& Gout,node n1,node n2);
 int join(GRAPH<d3_rat_point,int>& Gout,node n1,node n2); 


class rpoint_zg { 
public:
 rat_point p; 
 sc_vertex v;
};

inline int orientation(rpoint_zg a,rpoint_zg b,rpoint_zg c)
{  return orientation(a.p,b.p,c.p); }

inline int left_turn(rpoint_zg a,rpoint_zg b,rpoint_zg c)
{  return left_turn(a.p,b.p,c.p); }

inline int right_turn(rpoint_zg a,rpoint_zg b,rpoint_zg c)
{  return right_turn(a.p,b.p,c.p); }

inline bool collinear(rpoint_zg a,rpoint_zg b,rpoint_zg c)
{  return collinear(a.p,b.p,c.p); }

inline int side_of_circle(rpoint_zg a,rpoint_zg b,rpoint_zg c,rpoint_zg d)
// hier den Test mit den d3-Vertices und nicht der Projektion (??)
{  return side_of_circle(a.p,b.p,c.p,d.p); }

inline int compare(const rpoint_zg& T1, const rpoint_zg& T2)
{  return compare(T1.p,T2.p); }

inline ostream& operator<<(ostream& s, const rpoint_zg& T){ return s << T.p;}
inline istream& operator>>(istream& s, rpoint_zg& T){ return s;}
inline int operator==(const rpoint_zg& a,const rpoint_zg& b){
  return (a.p==b.p);
}

/*{\Mtext
{\bf Non-Member Functions}
\smallskip
}*/



void DELAUNAY_FLIPPING(d3_simpl_complex& Comp, list<Simpl_Triang>& S);

void DELAUNAY_FLIPPING(d3_simpl_complex& Comp);

edge TRIANGULATE_POINTS(const list<rpoint_zg>& L0, GRAPH<rpoint_zg,sc_simplex>& G, d3_simpl_complex& C);
edge TRIANGULATE_POINTS(list<sc_vertex>& L0, GRAPH<rpoint_zg,sc_simplex>& G,d3_simpl_complex& C,int wert);
edge build_simp_comp(list<d3_rat_point>& L, d3_simpl_complex& C,int wert);


void calculate_triang(list<d3_rat_point>& L,d3_simpl_complex& Comp,int wert);

sc_simplex spec_case(list<d3_rat_point>& L,d3_simpl_complex& Comp,int fall,int wert);

int get_pro_plane(d3_rat_point A,d3_rat_point B,d3_rat_point C);

sc_simplex comp_initial_tr(list<d3_rat_point>& L,d3_simpl_complex& Comp,d3_rat_point& lastp,list<sc_simplex>& trhull);

int orientation(sc_vertex a,sc_vertex b,sc_vertex c,d3_rat_point d);

int test_vis(sc_simplex vis, sc_simplex other, sc_vertex c1, sc_vertex c2, d3_rat_point p,d3_rat_point pv);

sc_simplex really_vis(d3_simpl_complex& C,sc_simplex cand,d3_rat_point p,list<sc_simplex>& trhull);

void find_visible_triangles(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,sc_simplex n,d3_rat_point p,int counter,list<sc_simplex>& trhull);

void check_htr(list<sc_simplex>& trhull);

sc_simplex triangulate_new(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,d3_rat_point p,stack<Simpl_Triang>& TS,list<sc_simplex>& trhull);

void delaunay_flip(d3_simpl_complex& C,stack<Simpl_Triang>& TS);

sc_simplex update_tr(d3_simpl_complex& C,d3_rat_point p,int counter,sc_simplex n,list<sc_simplex>& trhull, bool dtr);

void compute_tr(list<d3_rat_point>& L, d3_simpl_complex& C,bool flipflag);



void D3_TRIANG(const list<d3_rat_point>& L0, d3_simpl_complex& C);

void D3_DELAU(const list<d3_rat_point>& L0, d3_simpl_complex& C);


void to_vdnb(GRAPH<d3_rat_point,int>& H,node nd,GRAPH<d3_rat_point,int>& outgr);

void to_vorocell(d3_simpl_complex& C,sc_vertex vert,GRAPH<d3_rat_point,int>& outgr);








rational r_det(rational a11,rational a12,rational a13,
               rational a21,rational a22,rational a23,
               rational a31,rational a32,rational a33)
{
 return a11*a22*a33+a12*a23*a31+a13*a21*a32 -
        a13*a22*a31-a11*a23*a32-a12*a21*a33;
}

d3_rat_point intersect(rat_vector n1,rat_vector n2,rat_vector n3,
                                     rational D1,rational D2,rational D3)
{
 rational A1=n1.xcoord(); 
 rational B1=n1.ycoord(); 
 rational C1=n1.zcoord();
 rational A2=n2.xcoord(); 
 rational B2=n2.ycoord(); 
 rational C2=n2.zcoord();
 rational A3=n3.xcoord(); 
 rational B3=n3.ycoord(); 
 rational C3=n3.zcoord();

 rational det= r_det(A1,B1,C1,A2,B2,C2,A3,B3,C3);
 rational xdet=r_det(D1,B1,C1,D2,B2,C2,D3,B3,C3);
 rational ydet=r_det(A1,D1,C1,A2,D2,C2,A3,D3,C3);
 rational zdet=r_det(A1,B1,D1,A2,B2,D2,A3,B3,D3);

 if (det==0) LEDA_EXCEPTION(1,"intersec: determinant==0 .");

 rational xw=-xdet/det; 
 rational yw=-ydet/det; 
 rational zw=-zdet/det;

 return d3_rat_point(xw.normalize(),yw.normalize(),zw.normalize());
}
 

 D3_vertex::D3_vertex(d3_rat_point w,d3_simpl_complex* c)
 {
  stor=w;
  com_of=c;
  my=NULL;
  helper=NULL;
 }

 D3_vertex::D3_vertex(){
  my=NULL; helper=NULL;
 }

 D3_vertex::D3_vertex(const D3_vertex& S)
 {
  stor=S.stor;
  com_of=S.com_of;
  it=S.it;
  my=S.my;
  helper=S.helper;
 } 


 list<sc_simplex> Simplex::all_neighbours() const 
 {
  list<sc_simplex> all;
  if (abc != NULL) all.append(abc);
  if (abd != NULL) all.append(abd);
  if (acd != NULL) all.append(acd);
  if (bcd != NULL) all.append(bcd);
  return all;
 }

 list<sc_simplex> d3_simpl_complex::all_neighbours(sc_simplex sim) const
 {
  return sim->all_neighbours();
 }
 
 Simplex::Simplex(sc_vertex Ap,sc_vertex Bp,sc_vertex Cp)
 {
  HULL_TR=true; 
  a=Ap; 
  b=Bp; 
  c=Cp; 
  d=NULL;  
  Marker=-1; 
  helper=NULL;
  abc=NULL; 
  abd=NULL; 
  acd=NULL; 
  bcd=NULL;
  name="";
  Ap->my=this; 
  Bp->my=this; 
  Cp->my=this;
  abcdia=false; 
  abddia=false; 
  acddia=false; 
  bcddia=false;
 }

 Simplex::Simplex(sc_vertex Ap,sc_vertex Bp,sc_vertex Cp,sc_vertex Dp)
 {
  HULL_TR=false; 
  a=Ap; 
  b=Bp; 
  c=Cp; 
  d=Dp; 
  Marker=-1; 
  helper=NULL;
  abc=NULL; 
  abd=NULL; 
  acd=NULL; 
  bcd=NULL;  
  name="";
  Ap->my=this; 
  Bp->my=this; 
  Cp->my=this; 
  Dp->my=this;
  abcdia=false; 
  abddia=false; 
  acddia=false; 
  bcddia=false;
 }

 Simplex::Simplex(const Simplex& S)
 {
  HULL_TR=S.HULL_TR;
  a=S.a;
  b=S.b;
  c=S.c;
  d=S.d;
  abd=S.abd;
  bcd=S.bcd;
  acd=S.acd;
  abc=S.abc;
  name=S.name;
  helper=S.helper;
  Marker=S.Marker;
  abcdia=S.abcdia;
  abddia=S.abddia; 
  acddia=S.acddia; 
  bcddia=S.bcddia;
 }

 void Simplex::init(sc_simplex s1,sc_simplex s2,sc_simplex s3,sc_simplex s4)
 {
  abd=s1; 
  bcd=s2; 
  acd=s3; 
  abc=s4;
 }

 void Simplex::init(sc_simplex s1,sc_simplex s2,sc_simplex s3)
 {
  abd=s1; 
  bcd=s2; 
  acd=s3;
  helper=NULL;
 }

 void Simplex::update(sc_simplex old, sc_simplex neu)
 {
  if (old==abd) { abd=neu; return; }
  if (old==bcd) { bcd=neu; return; }
  if (old==acd) { acd=neu; return; }
  if (old==abc) { abc=neu; return; }  
 }

 void Simplex::change(sc_vertex k1,sc_vertex k2,sc_vertex k3,sc_vertex k4)
 {
  a=k1; 
  b=k2; 
  c=k3; 
  d=k4;
  a->my=this; 
  b->my=this; 
  c->my=this; 
  d->my=this;
 }

 void Simplex::change(sc_vertex k1,sc_vertex k2,sc_vertex k3)
 {
  a=k1; 
  b=k2; 
  c=k3;
  a->my=this; 
  b->my=this; 
  c->my=this;
 }

 void Simplex::clear_links()
 {
  abc=NULL; 
  abd=NULL; 
  acd=NULL; 
  bcd=NULL;
 }

 sc_vertex Simplex::find_other(sc_vertex p1,sc_vertex p2,sc_vertex p3)
 {
  if (p1!=a && p2!=a && p3!=a) return a;
  if (p1!=b && p2!=b && p3!=b) return b;
  if (p1!=c && p2!=c && p3!=c) return c;
  if (! HULL_TR && p1!=d && p2!=d && p3!=d) return d;
  return NULL;
 }

 int Simplex::index(sc_vertex p)
 {
  if (p==a) return 1;
  if (p==b) return 2;
  if (p==c) return 3;
  if (p==d) return 4;
  return 0;
 }

 sc_vertex Simplex::vertex(int i)
 {
   switch(i) {
   case 1: return a;
   case 2: return b;
   case 3: return c;
   case 4: return d;
   }
   return NULL;
 }

 void Simplex::set_link(sc_simplex S2,int i)
 // change link to a neighbour...
 {
   switch(i){
   case 1: /*a*/ bcd=S2; break;   
   case 2: /*b*/ acd=S2; break;
   case 3: /*c*/ abd=S2; break;
   case 4: /*d*/ abc=S2; break;
   }
 }

 bool Simplex::in_simplex(d3_rat_point p)
 {
   if (HULL_TR) 
    return in_sim(a->stor,b->stor,c->stor,p);
   else
    return contained_in_simplex(a->stor,b->stor,c->stor,d->stor,p);
 }

 bool Simplex::insphere(d3_rat_point p)
 {
   if (HULL_TR) return false;
   else return inside_sphere(a->stor,b->stor,c->stor,d->stor,p);
 }

 int Simplex::control_vert(sc_vertex v1,sc_vertex v2,sc_vertex v3)
 {
   //  3 vertices element { a,b,c,d } ?

  int i1=index(v1); 
  int i2=index(v2); 
  int i3=index(v3);

  if (i1==0 || i2==0 || i3==0) return 0;
  // zurueckgeben, ob abc,abd,acd,bcd
  if (i1!=4 && i2!=4 && i3!=4) return 1; //abc
  if (i1!=3 && i2!=3 && i3!=3) return 2; //abd 
  if (i1!=2 && i2!=2 && i3!=2) return 3; //acd
  return 4; //bcd
 }

 int Simplex::control_vert(sc_vertex v1,sc_vertex v2)
 {
  int i1=index(v1); 
  int i2=index(v2); 
  if (i1==0 || i2==0) return 0;
  if (i1!=3 && i2!=3) return 2; //ab
  if (i1!=2 && i2!=2) return 3; //ac
  if (i1!=1 && i1!=1) return 4; //bc 
  return 0;
 }

 int Simplex::link_new(sc_simplex ot)
 {
  if (ot==NULL) return 0;

  sc_vertex v1=NULL; 
  sc_vertex v2=NULL; 
  sc_vertex v3=NULL;

  int w=0;
  
  if (! HULL_TR) {
   if (ot->abc==NULL){ 
     v1=ot->a; 
     v2=ot->b; 
     v3=ot->c;
     w=control_vert(v1,v2,v3); 
     if (w !=0) ot->abc=this;
   }
   if (ot->abd==NULL && w==0){ 
     v1=ot->a; 
     v2=ot->b; 
     v3=ot->d;
     w=control_vert(v1,v2,v3); 
     if (w !=0) ot->abd=this; 
   }
   if (ot->acd==NULL && w==0){ 
     v1=ot->a; 
     v2=ot->c; 
     v3=ot->d;
     w=control_vert(v1,v2,v3); 
     if (w !=0) ot->acd=this;
   }
   if (ot->bcd==NULL && w==0){ 
     v1=ot->b; 
     v2=ot->c; 
     v3=ot->d;
     w=control_vert(v1,v2,v3); 
     if (w !=0) ot->bcd=this;
   } 
  }
  else { //abc-Verweis braucht nicht beachtet werden
   if (ot->abd==NULL && w==0){ 
     v1=ot->a; 
     v2=ot->b; 
     w=control_vert(v1,v2); 
     if (w !=0) ot->abd=this; 
   }
   if (ot->acd==NULL && w==0){ 
     v1=ot->a; 
     v2=ot->c; 
     w=control_vert(v1,v2); 
     if (w !=0) ot->acd=this; 
   }
   if (ot->bcd==NULL && w==0){ 
     v1=ot->b; 
     v2=ot->c;
     w=control_vert(v1,v2); 
     if (w !=0) ot->bcd=this; 
   } 
  }

  switch(w){
  case 1: abc=ot; break;
  case 2: abd=ot; break;
  case 3: acd=ot; break;
  case 4: bcd=ot; break;
  }
  
  if (w>0 && w<5) return w; else return 0;
 }
 
 d3_rat_point Simplex::circle_midpoint()
 // for hulltriangles
 {
  d3_rat_point pneu;
  d3_rat_point m1,m2;
  d3_rat_plane pl1(a->stor,b->stor,c->stor);
  d3_rat_point h=c->stor + pl1.normal();
  d3_rat_plane plh(a->stor,b->stor,h); 
  d3_rat_plane plh2(b->stor,c->stor,h);
  
  m1=midpoint(a->stor,b->stor);
  m2=midpoint(b->stor,c->stor);
 
  rat_vector r1=pl1.normal();
  rat_vector r2=plh.normal();
  rat_vector r3=plh2.normal();

  d3_rat_plane pl2(m1,m1+r1,m1+r2),pl3(m2,m2+r1,m2+r3);

  rat_vector n1,n2,n3;
  n1=pl1.normal(); n2=pl2.normal(); n3=pl3.normal();

  rational D1,D2,D3;
  D1=-(n1*m1.to_vector()); D2=-(n2*m1.to_vector()); D3=-(n3*m2.to_vector());

  //jetzt Schnittpunkt der 3 Ebenen ermitteln 
  pneu=intersect(n1,n2,n3,D1,D2,D3);
  return pneu;   
 }

 d3_rat_point Simplex::sphere_midpoint()
 {
  d3_rat_point pneu;
  d3_rat_point m1,m2,m3;
  d3_rat_plane pl1(a->stor,b->stor,d->stor),pl2(a->stor,c->stor,d->stor);
  d3_rat_plane pl3(a->stor,b->stor,c->stor);

  m1=midpoint(a->stor,b->stor);
  m2=midpoint(a->stor,c->stor);
  m3=midpoint(a->stor,d->stor);

  rat_vector r1,r2,r3;
  r1=pl1.normal(); r2=pl2.normal(); r3=pl3.normal();
  
  d3_rat_plane sr1(m1,m1+r1,m1+r3),sr2(m2,m2+r2,m2+r3),sr3(m3,m3+r1,m3+r2);
  rat_vector n1,n2,n3;
  n1=sr1.normal(); n2=sr2.normal(); n3=sr3.normal();

  rational D1,D2,D3;
  D1=-(n1*m1.to_vector()); D2=-(n2*m2.to_vector()); D3=-(n3*m3.to_vector());

  pneu=intersect(n1,n2,n3,D1,D2,D3);
  return pneu;
 }

 rational Simplex::vol()
 // simplex volume
 {
  if (HULL_TR) return 0; //HULLTRIANGLE is flat

  return volume(a->stor,b->stor,c->stor,d->stor);
 }


 Simpl_Triang::Simpl_Triang()
 {
  S1=NULL; S2=NULL;
  A=NULL; B=NULL; C=NULL;
 }

 Simpl_Triang::Simpl_Triang(sc_simplex sim1,sc_simplex sim2,sc_vertex Ap,sc_vertex Bp,sc_vertex Cp)
 {
  S1=sim1; S2=sim2; A=Ap; B=Bp; C=Cp;
 }
 Simpl_Triang::Simpl_Triang(const Simpl_Triang& S)
 {
  S1=S.S1; S2=S.S2; A=S.A; B=S.B; C=S.C;
 }


  void d3_simpl_complex::reset_flip_counters(){
   flk30=0; flk31=0; flk32=0; flk33=0; flk34=0; flk35=0;
   flk1=0; flk10=0; flk11=0; flk12=0;
  }

  void d3_simpl_complex::output_flip_counters(){
/*
   cout << "Flips:\n";
   cout << "------\n";
   cout << "configuration 30/31/32/33/34/35:" << flk30 << " " << flk31 << " " << flk32 << " " << flk33 << " " 
        << flk34 << " " << flk35 << "\n";
   cout << "configuraton 1/10/11/12:" << flk1 << " " << flk10 << " " << flk11 << " " << flk12 << "\n";
*/
  }


  d3_simpl_complex::d3_simpl_complex()
  {
    hull_last=NULL;
    reset_flip_counters();
  }

  d3_simpl_complex::~d3_simpl_complex()
  {
   sc_simplex z;
   forall(z,Sil) delete z;
   sc_vertex d;
   forall(d,Vert) delete d;
  }

  list<d3_rat_point> d3_simpl_complex::points()
  {
    list<d3_rat_point> L;
    sc_vertex v;

    forall(v,Vert) L.append(pos(v));

    return L;
  }
  
  void d3_simpl_complex::clear()
  {
    hull_last=NULL;
    sc_simplex z;
    forall(z,Sil) delete z;
    sc_vertex d;
    forall(d,Vert) delete d;
    Sil.clear();
    Vert.clear();
    reset_flip_counters();
  }

  void d3_simpl_complex::reset_markers()
  {
    sc_simplex akt;

    forall(akt,Sil) akt->Marker=-1;
  }

  sc_simplex d3_simpl_complex::get_simplex(sc_vertex v)
  {
    if (v->com_of != this) return NULL;
    return v->my;
  }

  sc_simplex d3_simpl_complex::opposite_simplex(sc_simplex S,int i)
  {
    if (S->com_of != this) return NULL; 
    if (i==1) return S->bcd;      
    if (i==2) return S->acd;
    if (i==3) return S->abd;
    if (i==4) return S->abc;
    return NULL;
  }

  sc_simplex d3_simpl_complex::opposite_simplex(sc_simplex S,sc_vertex v)
  {
   int i= S->index(v);
   if (i==0) return NULL;
   return opposite_simplex(S,i);
  }

  sc_vertex d3_simpl_complex::opposite_vertex(sc_simplex S,int i)
  {
   sc_simplex nb;
   sc_vertex v = 0;
   nb=opposite_simplex(S,i);
   if (nb==NULL) return NULL;
   else {
     switch(i) {
     case 1: v=nb->find_other(S->b,S->c,S->d);
             break;
     case 2: v=nb->find_other(S->a,S->c,S->d);
             break;
     case 3: v=nb->find_other(S->a,S->b,S->d);
             break;
     case 4: v=nb->find_other(S->a,S->b,S->c);
             break;
     }
     return v;
   }   
  }

  sc_vertex d3_simpl_complex::opposite_vertex(sc_simplex S,sc_vertex v)
  {
   int i= S->index(v);
   return (i==0) ? NULL : opposite_vertex(S,i);
  }

  sc_vertex d3_simpl_complex::add_vertex(d3_rat_point p)
  {
    sc_vertex z= new D3_vertex(p,this);
    z->it=Vert.append(z);
    return z;
  }

  sc_simplex d3_simpl_complex::add_simplex(sc_vertex a,sc_vertex b,sc_vertex c)
  { 
   if (a->com_of != this || b->com_of != this || c->com_of != this) return NULL;
   sc_simplex S= new Simplex(a,b,c); 
   S->com_of=this;
   S->it=Sil.append(S);
   hull_last=S;

   return S;      
  }

  sc_simplex d3_simpl_complex::add_simplex(sc_vertex a,sc_vertex b,sc_vertex c,sc_vertex d)
  { 
    if (a->com_of != this || b->com_of != this || 
        c->com_of != this || d->com_of != this) return NULL;   

    sc_simplex S= new Simplex(a,b,c,d);
    S->com_of=this;
    S->it = Sil.append(S);

    return S; 
  }

  sc_simplex d3_simpl_complex::search_simplex(d3_rat_point p) 
  {
   //should be improved ( "walk through" method or special data str.)
   sc_simplex akt;
   
   forall(akt,Sil){
     if (akt->in_simplex(p)) return akt;
   }

   return NULL;
  }

  sc_simplex d3_simpl_complex::search_simplex(sc_vertex v)
  {
   d3_rat_point p= v->stor;
   if (v->com_of != this) return NULL; // vertex has to be in the complex...
   return search_simplex(p);
  }

  sc_simplex d3_simpl_complex::wtsearch_simplex(d3_rat_point p)
  {
   if (Sil.empty()) return NULL; // no simplices in the complex...

   sc_simplex akt=Sil.head();

   do {
    if (akt->in_simplex(p)) return akt;
    else {
    // *akt verbessern...
    }
   } while (akt!=NULL);

   return akt;
  }

  sc_simplex d3_simpl_complex::wtsearch_simplex(sc_vertex v)
  {
   d3_rat_point p= v->stor;
   if (v->com_of != this) return NULL;
   return wtsearch_simplex(p);   
  }

  int d3_simpl_complex::remove_simplex(sc_simplex SR)
  {
    if (SR->com_of != this) return 0; 
 

    sc_simplex S1,S2,S3,S4;
    S1=SR->abc; S2=SR->abd; S3=SR->acd; S4=SR->bcd;

    if (S1 != NULL) S1->update(SR,NULL);
    if (S2 != NULL) S2->update(SR,NULL);
    if (S3 != NULL) S3->update(SR,NULL);
    if (S4 != NULL) S4->update(SR,NULL);
    
    // change vertex-links...
    if (SR->a->my == SR){ 
       if (SR->abc !=NULL) SR->a->my=SR->abc;
       else {
         if (SR->abd !=NULL) SR->a->my=SR->abd;
         else SR->a->my=SR->acd;
       }
    }
    if (SR->b->my == SR){ 
       if (SR->abc !=NULL) SR->b->my=SR->abc;
       else {
         if (SR->abd !=NULL) SR->b->my=SR->abd;
         else SR->b->my=SR->bcd;
       }
    }
    if (SR->c->my == SR){ 
       if (SR->abc !=NULL) SR->c->my=SR->abc;
       else {
         if (SR->acd !=NULL) SR->c->my=SR->acd;
         else SR->c->my=SR->bcd;
       }  
    }
    if (! SR->HULL_TR){
     if (SR->d->my == SR){ 
       if (SR->abd !=NULL) SR->d->my=SR->abd;
       else {
         if (SR->acd !=NULL) SR->d->my=SR->acd;
         else SR->d->my=SR->bcd;
       }      
     }
    }
      
    list_item i;
    i= SR->it;
    SR->com_of=NULL; delete SR;
    (Sil).del_item(i);
    
    return 1;
  }

  bool d3_simpl_complex::flippable(sc_simplex S1, sc_vertex v)
  {
    sc_vertex v2 = opposite_vertex(S1,v); 

    if (v2==NULL) return false;
    
    d3_rat_point Ap= S1->a->stor;
    d3_rat_point Bp= S1->b->stor;
    d3_rat_point Cp= S1->c->stor;
    d3_rat_point Ep= v2->stor;

    if (!S1->HULL_TR){
     d3_rat_point Dp= S1->d->stor;
     return inside_sphere(Ap,Bp,Cp,Dp,Ep);
    }
    else { // Hulltr.
     if (coplanar(Ap,Bp,Cp,Ep)){
        return test_inside(Ap,Bp,Cp,Ep);
     }
    }
    return false;
  }

  bool d3_simpl_complex::flippable(sc_simplex S1, sc_simplex S2,Simpl_Triang& Tr)
  // is common triangle flippable ?
  {
    int wert=common_triangle(S1,S2,Tr);
    if (wert==0) return false; //no common tr.

    sc_vertex v2= S2->find_other(Tr.A,Tr.B,Tr.C); 
    if (v2==NULL) return false;

    d3_rat_point Ap= S1->a->stor;
    d3_rat_point Bp= S1->b->stor;
    d3_rat_point Cp= S1->c->stor;
    d3_rat_point Ep= v2->stor;

    if (! S1->HULL_TR){
     d3_rat_point Dp= S1->d->stor;
     return inside_sphere(Ap,Bp,Cp,Dp,Ep);
    }
    else { // Hulltr.
     if (coplanar(Ap,Bp,Cp,Ep)){
        return test_inside(Ap,Bp,Cp,Ep);
     }
    }
    return false;
  }

  void d3_simpl_complex::init_dia(bool w)
  {
   sc_simplex akt;
   forall(akt,Sil){
    akt->abcdia=w;akt->abddia=w;akt->acddia=w;akt->bcddia=w;
   }
  }

  int  d3_simpl_complex::set_dia_nondia()
  // set non-diagramedge - informationsd
  {
    sc_simplex akt,other;
    d3_rat_point Ap,Bp,Cp,Dp,op;
    sc_vertex v;
    int ND_SETS=0;
    
    forall(akt,Sil){
      if (! akt->HULL_TR){
        Ap= akt->a->stor; 
        Bp= akt->b->stor; 
        Cp= akt->c->stor; 
        Dp= akt->d->stor;
        other=akt->abc;
        if (other != NULL && !other->HULL_TR){
          v=other->find_other(akt->a,akt->b,akt->c); op=v->stor;
          if (on_sphere(Ap,Bp,Cp,Dp,op)) { akt->abcdia=false; ND_SETS++;}
        }
        other=akt->abd;
        if (other != NULL && !other->HULL_TR){
          v=other->find_other(akt->a,akt->b,akt->d); op=v->stor;
          if (on_sphere(Ap,Bp,Cp,Dp,op)) { akt->abddia=false; ND_SETS++;} 
        }
        other=akt->acd;
        if (other != NULL && !other->HULL_TR){
          v=other->find_other(akt->a,akt->c,akt->d); op=v->stor;
          if (on_sphere(Ap,Bp,Cp,Dp,op)) { akt->acddia=false; ND_SETS++;}
        }
        other=akt->bcd;         
        if (other != NULL && !other->HULL_TR){
          v=other->find_other(akt->b,akt->c,akt->d); op=v->stor;
          if (on_sphere(Ap,Bp,Cp,Dp,op)) { akt->bcddia=false; ND_SETS++;}
        }   
      }
    }
    return ND_SETS;
    
  }

  bool d3_simpl_complex::local_delaunay(sc_simplex S1)
  // is  simplex S1 "local delaunay" ?
  {
    bool wert;
    wert=flippable(S1,S1->a);
    if (wert) return false;
    wert=flippable(S1,S1->b);
    if (wert) return false;
    wert=flippable(S1,S1->c);
    if (wert) return false;
    wert=flippable(S1,S1->d);
    if (wert) return false;  

    return true;  
  }

  bool d3_simpl_complex::global_delaunay(sc_simplex fst)
  {
    sc_vertex ac;
    bool flag=true;
             
    forall(ac,Vert){
     if (fst->insphere(ac->stor)) flag=false;
    }

    return flag;    
  }

  int d3_simpl_complex::get_link_index(sc_simplex S1,sc_simplex S2)
  {
    if (S1->com_of != this || S2->com_of !=this) return -1;
    if (S1 == S2) return -2;

    if (S1->abc==S2) return 1;
    if (S1->abd==S2) return 2;
    if (S1->acd==S2) return 3;
    if (S1->bcd==S2) return 4;

    return 0;
  }
  
  sc_simplex d3_simpl_complex::set_link(sc_simplex S1,int i,sc_simplex S2,int j)
  {
    if (S1->com_of != this) return NULL;
    if (S2->com_of != this) return NULL; 
    if ( i>4 || i<1 || j>4 || j<1) return NULL;
    S1->set_link(S2,i);
    S2->set_link(S1,j);

    return S1;
  }

  sc_simplex d3_simpl_complex::set_link(sc_simplex S1,sc_simplex S2, sc_vertex v1, sc_vertex v2, sc_vertex v3)
  // (*v1),(*v2),(*v3) the common vertices...
  {
    sc_vertex os1,os2;

    os1= S1->find_other(v1,v2,v3);
    os2= S2->find_other(v1,v2,v3);
    int i,j;

    if (os1 !=NULL) i= S1->index(os1); else i=4; //abc
    if (os2 !=NULL) j= S2->index(os2); else j=4; //abc

    return set_link(S1,i,S2,j);
  }

  sc_simplex d3_simpl_complex::set_link(sc_simplex S1,sc_vertex i,sc_simplex S2,sc_vertex j)
  {
    int i2,j2;
    if (i !=NULL) i2= S1->index(i); else i2=4; 
    if (j !=NULL) j2= S2->index(j); else j2=4; 
    if (i2==0 || j2==0) return NULL;
    
    return set_link(S1,i2,S2,j2);
     
  }  

  bool d3_simpl_complex::check_vert_sim_links()
  {
   sc_vertex v;
   sc_simplex akt;
   forall(v,Vert){
     if (v->my == NULL) return false;
     akt= v->my;
     if (akt->index(v) == 0 || akt->com_of != this) return false;
   }
   return true;
  }

  int d3_simpl_complex::check_delaunay()
  {
   //  all simplices local delaunay ?
   sc_simplex akt;
   bool tf, haupt=true;
   int counter=0;

   haupt=true;
   forall(akt,Sil){
    tf=local_delaunay(akt);
    if (tf==false) { haupt=false; counter++; }
   }

   return counter;
  }

  int join_or_link(GRAPH<d3_rat_sphere,int>& Gout,node n1,node n2)
  {
   d3_rat_sphere s1,s2;
   s1=Gout[n1];
   s2=Gout[n2];

   Gout.new_edge(n1,n2); return 0; 
  }

  int join(GRAPH<d3_rat_point,int>& Gout,node n1,node n2)
  {
   d3_rat_point s1,s2;
   s1=Gout[n1]; s2=Gout[n2];

   if (n1==n2) return 0;
    
   if (s1==s2){
     Gout.merge_nodes(n1,n2);
     return 1;
   }
   else return 0;
  }
  

  bool d3_simpl_complex::compute_voronoi(GRAPH<d3_rat_sphere,int>& Gout)
  {
    int bz= check_delaunay();
    if (bz>0) return false;

    Gout.clear();
    sc_simplex akt;
    d3_rat_sphere *neu;
    d3_rat_point ap,bp,cp,dp;
    node nn;
    int w;

    forall(akt,Sil){
      ap= akt->a->stor; 
      bp= akt->b->stor;
      cp= akt->c->stor;

      if (akt->HULL_TR){ 
       neu= new d3_rat_sphere(ap,bp,cp,cp);
      }
      else {
       dp= akt->d->stor;
       neu= new d3_rat_sphere(ap,bp,cp,dp);
      }

      nn=Gout.new_node(*neu); 
      akt->helper= (void*)nn;
    }

    node n,n2;
    sc_simplex s1;

    forall(akt,Sil){
      n= (node)(akt->helper);
      s1= akt->abc;
      if (s1 != NULL) { n2=(node)(s1->helper);
                        w=join_or_link(Gout,n,n2);
                        if (w==1) { s1->helper= (void*)(n); } // merge...
                       }
      s1= akt->abd;
      if (s1 != NULL) { n2=(node)(s1->helper);
                        w=join_or_link(Gout,n,n2);
                        if (w==1) { s1->helper= (void*)(n); } // merge...
                       }
      s1= akt->acd;
      if (s1 != NULL) { n2=(node)(s1->helper);
                        w=join_or_link(Gout,n,n2);
                        if (w==1) { s1->helper= (void*)(n); } // merge...
                       }
      s1= akt->bcd;
      if (s1 != NULL) { n2=(node)(s1->helper); 
                        w=join_or_link(Gout,n,n2); 
                        if (w==1) { s1->helper= (void*)(n); } // merge...
                       }
    }

    return true;
  }

  bool d3_simpl_complex::get_incident_simplices(sc_vertex v,list<sc_simplex>& ret)
  {
   if (v->com_of != this) { return false; }
   if (v->my == NULL) { return false; } 

   sc_simplex start= v->my;
   sc_simplex akt;
   sc_simplex l1,l2,l3,l4;
   reset_markers();
   start->Marker=100;
   stack<sc_simplex> CAND;
   CAND.push(start);

   while (! CAND.empty()){
     akt=CAND.pop();
     ret.append(akt);
     l1=akt->abc; l2=akt->abd; l3=akt->acd; l4=akt->bcd;
     if (l1 != NULL && (l1->Marker ==-1) && (l1->index(v)!=0)) { CAND.push(l1); l1->Marker=100; }
     if (l2 != NULL && (l2->Marker ==-1) && (l2->index(v)!=0)) { CAND.push(l2); l2->Marker=100; }
     if (l3 != NULL && (l3->Marker ==-1) && (l3->index(v)!=0)) { CAND.push(l3); l3->Marker=100; }
     if (l4 != NULL && (l4->Marker ==-1) && (l4->index(v)!=0)) { CAND.push(l4); l4->Marker=100; }
   }   

   return true;
  }

  bool d3_simpl_complex::get_incident_points(sc_vertex v,list<d3_rat_point>& dl)
  {
   list<sc_simplex> si;
   sc_simplex akt;
   d3_rat_point pneu;

   bool rw=get_incident_simplices(v,si);
   if (! rw) return false;

   forall(akt,si){
      if (akt->HULL_TR) pneu= akt->circle_midpoint();
      else pneu= akt->sphere_midpoint();
      
      dl.append(pneu);
   }
   return true;
  }

  void d3_simpl_complex::kill_vlists()
  {
    sc_vertex v;
    void* zeig;
    forall(v,Vert){
        zeig=v->helper; if (zeig!=NULL) delete ((list<d3_rat_point>*)(zeig));
    }
  }

  bool d3_simpl_complex::make_vlists()
  {
    int bz= check_delaunay();

    assert(bz == 0);

    list<d3_rat_point>* aktli;
    sc_vertex v;
    forall(v,Vert){
      aktli= new list<d3_rat_point>;
      v->helper= (void*) aktli;
    }

    sc_simplex akt;

    d3_rat_point pneu;
    d3_rat_point phelp1,phelp2;
    rat_vector rv;

    void * zeig;

    forall(akt,Sil){
      if (akt->HULL_TR){ 
       pneu= akt->circle_midpoint();
       /*
       //compute here some kind of "ray"
       sc_simplex nb=akt->abc;
       if (nb==NULL) pneu= akt->circle_midpoint();
       else {
        phelp1= nb->sphere_midpoint();
        phelp2= akt->circle_midpoint();
        rv=(phelp2-phelp1);
        rv=rational(10.0)*rv;
        pneu=phelp1+rv;
       }
       */
      }
      else {
       pneu= akt->sphere_midpoint();
      }
      if (akt->HULL_TR){
        zeig=akt->a->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
        zeig=akt->b->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
        zeig=akt->c->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
      }
      else {
        zeig=akt->a->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
        zeig=akt->b->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
        zeig=akt->c->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
        zeig=akt->d->helper; ((list<d3_rat_point>*)zeig)->append(pneu);
      }
      
    }  
    // show only closed voronoi cells...
    forall(akt,Sil){
      if (akt->HULL_TR){
        zeig=akt->a->helper; ((list<d3_rat_point>*)zeig)->clear();
        zeig=akt->b->helper; ((list<d3_rat_point>*)zeig)->clear();
        zeig=akt->c->helper; ((list<d3_rat_point>*)zeig)->clear();
      }
    }
    
    return true;
  }

  bool d3_simpl_complex::compute_voronoi(GRAPH<d3_rat_point,int>& Gout,bool hflag) 
  {
    int bz= check_delaunay();
    if (bz>0) return false; 

    // compute VD 

    Gout.clear();

    sc_simplex akt,akt2;

    d3_rat_point pneu;
    node neu;

    forall(akt,Sil){
     if (! akt->HULL_TR || hflag) {
      if (akt->HULL_TR){ // no voronoi nodes...

  d3_rat_plane pl(akt->a->stor,akt->c->stor,akt->b->stor);
  pneu = pl.normal();
  

/*
       pneu= akt->circle_midpoint();
       assert(coplanar(pneu,akt->a->stor,akt->b->stor,akt->c->stor));
*/
      }
      else { // voronoi nodes ...
       pneu= akt->sphere_midpoint();
      }

      neu=Gout.new_node(pneu);
      akt->helper= (void*)neu;
     }
    }

    node n,n2;
    sc_simplex s1;
    int w;
    edge e1,e2;

    forall(akt,Sil){
     if (! akt->HULL_TR /*|| hflag */) {

       n= (node)(akt->helper);
       s1= akt->abc;

       if ((s1 != NULL ) && (! s1->HULL_TR || hflag))
       { n2= (node)(s1->helper); 
         e1=Gout.new_edge(n,n2); 
         e2=Gout.new_edge(n2,n); 
         Gout.set_reversal(e1,e2); 
        }

       s1= akt->abd;
       if ((s1 != NULL ) && (! s1->HULL_TR || hflag)) 
       { n2= (node)(s1->helper); 
         e1=Gout.new_edge(n,n2); 
         e2=Gout.new_edge(n2,n); 
         Gout.set_reversal(e1,e2); 
        }

       s1= akt->acd;
       if ((s1 != NULL ) && (! s1->HULL_TR || hflag)) 
       { n2= (node)(s1->helper); 
         e1=Gout.new_edge(n,n2); 
         e2=Gout.new_edge(n2,n); 
         Gout.set_reversal(e1,e2); 
        }

       s1= akt->bcd;
       if ((s1 != NULL ) && (! s1->HULL_TR || hflag)) 
       { n2= (node)(s1->helper); 
         e1=Gout.new_edge(n,n2); 
         e2=Gout.new_edge(n2,n); 
         Gout.set_reversal(e1,e2); 
        }
     }
    }

   
    // try to merge nodes

    forall(akt,Sil)
    { if (! akt->HULL_TR || hflag) 
      { n = (node)(akt->helper);

        s1 = akt->abc;
        if (s1 != NULL && (! s1->HULL_TR || hflag)) 
        { n2= (node)(s1->helper); 
          w=join(Gout,n,n2);
          if (w==1) 
          { s1->helper= (void*)(n);
	    forall(akt2,Sil) 
               if ((node)akt2->helper==n2) akt2->helper=(void*)n; 
           }
         }

        s1= akt->abd;
        if (s1 != NULL && (! s1->HULL_TR || hflag)) 
        { n2= (node)(s1->helper);
          w=join(Gout,n,n2);
          if (w==1) 
          { s1->helper= (void*)(n);
            forall(akt2,Sil) 
               if ((node)akt2->helper==n2) akt2->helper=(void*)n; 
           }
         }

        s1= akt->acd;
        if (s1 != NULL && (! s1->HULL_TR || hflag)) 
        { n2= (node)(s1->helper);
          w=join(Gout,n,n2);
          if (w==1) 
          { s1->helper= (void*)(n); 
            forall(akt2,Sil) 
              if ((node)akt2->helper==n2) akt2->helper=(void*)n; 
           }
         }

        s1= akt->bcd;
        if (s1 != NULL && (! s1->HULL_TR || hflag)) 
        { n2= (node)(s1->helper); 
          w=join(Gout,n,n2);
          if (w==1) 
          { s1->helper= (void*)(n);
            forall(akt2,Sil)
              if ((node)akt2->helper==n2) akt2->helper=(void*)n; 
           }
         }
     }
   }
 
    return true;
  }

  void d3_simpl_complex::compute_graph(sc_simplex sakt,
                                       GRAPH<d3_rat_point,int>& Gout)
  {
   // computes a GRAPH Gout  containing (*sakt) and its neighbours

   d3_rat_point a,b,c,d;

   Gout.clear();

   list<sc_simplex> me_and_nb;
   me_and_nb.push(sakt);
   if (sakt->abc != NULL) me_and_nb.push(sakt->abc);
   if (sakt->abd != NULL) me_and_nb.push(sakt->abd);
   if (sakt->acd != NULL) me_and_nb.push(sakt->acd);
   if (sakt->bcd != NULL) me_and_nb.push(sakt->bcd);

   sc_simplex S;
   forall(S,me_and_nb){
    a=S->a->stor; 
    b=S->b->stor; 
    c=S->c->stor; 
    if (!S->HULL_TR) d=S->d->stor;

    node o1=Gout.new_node(a); 
    node o2=Gout.new_node(b); 
    node o3=Gout.new_node(c);
    node o4 = 0;
    if (!S->HULL_TR) o4=Gout.new_node(d);

    edge e1=Gout.new_edge(o1,o2); 
    edge e2=Gout.new_edge(o2,o1); 
    Gout.set_reversal(e1,e2);

    e1=Gout.new_edge(o1,o3); 
    e2=Gout.new_edge(o3,o1); 
    Gout.set_reversal(e1,e2);

    if (!S->HULL_TR){
     e1=Gout.new_edge(o1,o4); 
     e2=Gout.new_edge(o4,o1); 
     Gout.set_reversal(e1,e2);
    }

    e1=Gout.new_edge(o2,o3); 
    e2=Gout.new_edge(o3,o2); 
    Gout.set_reversal(e1,e2);

    if (!S->HULL_TR){
     e1=Gout.new_edge(o2,o4); 
     e2=Gout.new_edge(o4,o2); 
     Gout.set_reversal(e1,e2);
     e1=Gout.new_edge(o3,o4); 
     e2=Gout.new_edge(o4,o3); 
     Gout.set_reversal(e1,e2);
    }
   }   
  }

  void d3_simpl_complex::compute_surface_graph(GRAPH<d3_rat_point,int>& Gout)
  //  computes a GRAPH Gout only containing the HullTriangles of the complex
  {
   Gout.clear();

   sc_simplex S;
   forall(S,Sil){

     if (!S->HULL_TR) continue;

     d3_rat_point a=S->a->stor;
     d3_rat_point b=S->b->stor;
     d3_rat_point c=S->c->stor;

     node o1=Gout.new_node(a); 
     node o2=Gout.new_node(b); 
     node o3=Gout.new_node(c);

     edge e1=Gout.new_edge(o1,o2); 
     edge e2=Gout.new_edge(o2,o1); 
     Gout.set_reversal(e1,e2);

     e1=Gout.new_edge(o1,o3); 
     e2=Gout.new_edge(o3,o1); 
     Gout.set_reversal(e1,e2);

     e1=Gout.new_edge(o2,o3); 
     e2=Gout.new_edge(o3,o2); 
     Gout.set_reversal(e1,e2);
   } 

  }


  void d3_simpl_complex::compute_graph(GRAPH<d3_rat_point,int>& Gout)
  {
   node o1,o2,o3,o4=0;
   edge e1,e2;
   sc_simplex S;
   d3_rat_point a,b,c,d;
   Gout.clear();

   forall(S,Sil){
    a=S->a->stor; 
    b=S->b->stor; 
    c=S->c->stor;
    if (! S->HULL_TR) d=S->d->stor;

    o1=Gout.new_node(a); 
    o2=Gout.new_node(b); 
    o3=Gout.new_node(c);
    if (! S->HULL_TR) o4=Gout.new_node(d);

    e1=Gout.new_edge(o1,o2); 
    e2=Gout.new_edge(o2,o1); 
    Gout.set_reversal(e1,e2);
    e1=Gout.new_edge(o1,o3); 
    e2=Gout.new_edge(o3,o1); 
    Gout.set_reversal(e1,e2);
    if (! S->HULL_TR){
     e1=Gout.new_edge(o1,o4); 
     e2=Gout.new_edge(o4,o1); 
     Gout.set_reversal(e1,e2);
    }
    e1=Gout.new_edge(o2,o3); 
    e2=Gout.new_edge(o3,o2); 
    Gout.set_reversal(e1,e2);

    if (! S->HULL_TR){
     e1=Gout.new_edge(o2,o4); 
     e2=Gout.new_edge(o4,o2); 
     Gout.set_reversal(e1,e2);

     e1=Gout.new_edge(o3,o4); 
     e2=Gout.new_edge(o4,o3); 
     Gout.set_reversal(e1,e2);
    }
   } 
  }

/*
  void d3_simpl_complex::compute_graph(GRAPH<d3_rat_point,int>& Gout,
                                       set<sc_vertex>& DS,list<edge>& erg)
  {
    // fuer d3-window einen ausgabefaehigen Graphen erstellen...
    // Verbindungen zwischen vertices im set als edge-liste zurück, 
    // um Farbmark. zu ermöglichen
   node o1,o2,o3,o4=0;
   edge e1,e2;
   sc_simplex S;
   d3_rat_point a,b,c,d;
   bool aw,bw,cw,dw=false;
   Gout.clear();

   forall(S,Sil){
    a=S->a->stor; 
    b=S->b->stor; 
    c=S->c->stor;
    aw= DS.member(S->a); 
    bw= DS.member(S->b); 
    cw= DS.member(S->c);

    if (! S->HULL_TR) { d=S->d->stor; dw= DS.member(S->d); }

    o1=Gout.new_node(a); 
    o2=Gout.new_node(b); 
    o3=Gout.new_node(c);
    if (! S->HULL_TR) o4=Gout.new_node(d);

    e1=Gout.new_edge(o1,o2); 
    e2=Gout.new_edge(o2,o1); 
    Gout.set_reversal(e1,e2);
    if (aw && bw) { erg.append(e1); erg.append(e2); }

    e1=Gout.new_edge(o1,o3); 
    e2=Gout.new_edge(o3,o1); 
    Gout.set_reversal(e1,e2);
    if (aw && cw) { erg.append(e1); erg.append(e2); }

    if (! S->HULL_TR){
     e1=Gout.new_edge(o1,o4); 
     e2=Gout.new_edge(o4,o1); 
     Gout.set_reversal(e1,e2);
     if (aw && dw) { erg.append(e1); erg.append(e2); }
    }
    e1=Gout.new_edge(o2,o3); 
    e2=Gout.new_edge(o3,o2); 
    Gout.set_reversal(e1,e2);
    if (bw && cw) { erg.append(e1); erg.append(e2); }
    if (! S->HULL_TR){
     e1=Gout.new_edge(o2,o4); 
     e2=Gout.new_edge(o4,o2); 
     Gout.set_reversal(e1,e2);
     if (bw && dw) { erg.append(e1); erg.append(e2); }
     e1=Gout.new_edge(o3,o4); 
     e2=Gout.new_edge(o4,o3); 
     Gout.set_reversal(e1,e2);
     if (cw && dw) { erg.append(e1); erg.append(e2); }
    }
   }
  }
*/


bool in_sim(const d3_rat_point& p1,const d3_rat_point& p2,
            const d3_rat_point& p3,const d3_rat_point& cand)
{
 array<d3_rat_point> AT(3);
 AT[0]=p1; AT[1]=p2; AT[2]=p3;
 return contained_in_simplex(AT,cand);
}

int d3_simpl_complex::common_triangle(sc_simplex S1, sc_simplex S2, Simpl_Triang& T)
// attention! neighbour settings have to be correct for this function!
{
 int wert=get_link_index(S1,S2);
 if (wert==0) return 0;
 T.S1=S1;
 T.S2=S2;

 switch(wert){
 case 1:
  T.A=S1->a; T.B=S1->b; T.C=S1->c;
  break;
 case 2:
  if (S1->HULL_TR && S2->HULL_TR){
    T.A=S1->a; T.B=S1->b; T.C=S1->b;
  }
  else {
    T.A=S1->a; T.B=S1->b; T.C=S1->d;
  }
  break;
 case 3:
  if (S1->HULL_TR && S2->HULL_TR){
    T.A=S1->a; T.B=S1->c; T.C=S1->c;
  }
  else {
    T.A=S1->a; T.B=S1->c; T.C=S1->d;
  }
  break;
 case 4:
  if (S1->HULL_TR && S2->HULL_TR){
    T.A=S1->b; T.B=S1->c; T.C=S1->c;
  }
  else {
    T.A=S1->b; T.B=S1->c; T.C=S1->d;
  }
  break;
 }
 
 return wert;
}

int d3_simpl_complex::configuration(sc_simplex S1, sc_simplex S2, Simpl_Triang& T)
// see B. Joe (construction of 3d Delaunay triangulations using local transformations)
// for desription of configurations
{
 sc_vertex v1_other,v2_other;
 d3_rat_point ap,bp,cp,dp,ep;

 ap = T.A->stor; 
 bp = T.B->stor; 
 cp = T.C->stor;

 v1_other= S1->find_other(T.A,T.B,T.C);
 if (v1_other==NULL) return 0;
 dp= v1_other->stor;
 v2_other= S2->find_other(T.A,T.B,T.C);
 if (v2_other==NULL) return 0;
 ep= v2_other->stor;

 int o1=orientation(ap,bp,dp,ep);
 int o2=orientation(cp,ap,dp,ep);
 int o3=orientation(bp,cp,dp,ep);

 sc_vertex f1,f2;
 sc_simplex sim1,sim2;

 if (o1==0 || o2==0 || o3==0){ // configuration 3 or 4 or 5
   if (o1==0){
     if (collinear(dp,ap,ep) || collinear(dp,bp,ep)) return 4; //  conf. 4
     if (in_sim(dp,ep,bp,ap) || in_sim(dp,ep,ap,bp)) return 5; //  conf. 5
     
     f1=opposite_vertex(S1,T.C);
     f2=opposite_vertex(S2,T.C);

     // configuration 3
     if (f1==NULL && f2==NULL) return 30;
     if (f1==f2) return 31;
   }
   if (o2==0){
     if (collinear(dp,ap,ep) || collinear(dp,cp,ep)) return 4; // conf. 4
     if (in_sim(cp,dp,ep,ap) || in_sim(ap,dp,ep,cp)) return 5; // conf. 5

     f1=opposite_vertex(S1,T.B);
     f2=opposite_vertex(S2,T.B);

     // configuration 3
     if (f1==NULL && f2==NULL) return 32;
     if (f1==f2) return 33;
   }
   if (o3==0){
     if (collinear(dp,bp,ep) || collinear(dp,cp,ep)) return 4; // conf. 4
     if (in_sim(bp,dp,ep,cp) || in_sim(dp,ep,cp,bp)) return 5; // conf. 5

     f1=opposite_vertex(S1,T.A);
     f2=opposite_vertex(S2,T.A);

     // configuration 3
     if (f1==NULL && f2==NULL) return 34;
     if (f1==f2) return 35;
   }   
   return 0;
   
 }
 else { // configuration 1 or 2
  bool cs1=false,cs2=false,cs3=false;
  contained_in_simplex(bp,cp,dp,ep,ap);  
  if (!cs1) cs2=contained_in_simplex(ap,cp,dp,ep,bp);
  if (!cs1 && !cs2) cs3=contained_in_simplex(ap,bp,dp,ep,cp);   

  if (cs1 || cs2 || cs3) return 2; // configuration 2
  // configuration 1

  d3_rat_point mi= midpoint(midpoint(ap,bp),cp);
  int omi=orientation(ap,bp,dp,mi);

  if (o1==o2 && o2==o3) return 1; //abcde convex...
 
  if (o1!=omi){
   sim1= opposite_simplex(S1,T.C);
   sim2= opposite_simplex(S2,T.C);
   if ((sim1==sim2) && (sim1 !=NULL)) return 10; 
  }
  if (o2!=omi){
   sim1= opposite_simplex(S1,T.B);
   sim2= opposite_simplex(S2,T.B);
   if ((sim1==sim2) && (sim1 !=NULL)) return 11; 
  }
  if (o3!=omi){
   sim1= opposite_simplex(S1,T.A);
   sim2= opposite_simplex(S2,T.A);
   if ((sim1==sim2) && (sim1 !=NULL)) return 12; 
  }

  return 0;                   // configuration 1, but not transformable...
 }

}

void d3_simpl_complex::link_save(sc_simplex SI, sc_simplex pt1, sc_simplex pt2, list<sc_simplex>& L)
{
 sc_simplex akt;
 list<sc_simplex> nb= SI->all_neighbours();
 forall(akt,nb){
  if (akt!=pt1 && akt!=pt2) L.append(akt);
  akt->update(SI,NULL);
 }
}

void d3_simpl_complex::link_korr(sc_simplex SI, list<sc_simplex>& L,list<Simpl_Triang>& TRL)
{
  sc_simplex akt;
  Simpl_Triang atl;

  int fl;
  if (SI->HULL_TR){
    forall(akt,L) if (akt->HULL_TR) SI->link_new(akt);
  }
  else {
    forall(akt,L) {
      fl=SI->link_new(akt);
      if (fl!=0) {
        atl.S1=SI;
        atl.S2=akt;
        switch(fl){ 
	case 1: atl.A=SI->a; atl.B=SI->b; atl.C=SI->c; break; //abc
	case 2: atl.A=SI->a; atl.B=SI->b; atl.C=SI->d; break; //abd
	case 3: atl.A=SI->a; atl.B=SI->c; atl.C=SI->d; break; //acd
	case 4: atl.A=SI->b; atl.B=SI->c; atl.C=SI->d; break; //bcd
        }
        TRL.append(atl);
      }
    }
  }
}

void d3_simpl_complex::link_korr(sc_simplex SI, list<sc_simplex>& L,queue<Simpl_Triang>& TQ,sc_vertex PV)
// correct links after a flip of SI
{
  sc_simplex akt;
  Simpl_Triang atl;

  int fl;
  if (SI->HULL_TR){
    forall(akt,L) if (akt->HULL_TR) SI->link_new(akt);
  }
  else {
    forall(akt,L) {
      fl=SI->link_new(akt);
      if (fl!=0) {
        atl.S1=SI;
        atl.S2=akt;
        switch(fl){
	case 1: atl.A=SI->a; atl.B=SI->b; atl.C=SI->c; break; //abc
	case 2: atl.A=SI->a; atl.B=SI->b; atl.C=SI->d; break; //abd
	case 3: atl.A=SI->a; atl.B=SI->c; atl.C=SI->d; break; //acd
	case 4: atl.A=SI->b; atl.B=SI->c; atl.C=SI->d; break; //bcd
        }
        if (atl.A !=PV && atl.B !=PV && atl.C !=PV) TQ.append(atl);
      }
    }
  }
}

void d3_simpl_complex::flip_konf3(sc_simplex S1, sc_simplex S2,
                                  d3_simpl_complex* Cl,int konf,
                                  Simpl_Triang& T,queue<Simpl_Triang>& TQ,
                                  sc_vertex PV)
{ 
  list<sc_simplex> SN;
  list<sc_simplex> PSN; 

  sc_simplex Ps1=0, Ps2=0; 

  sc_vertex av,bv,cv,dv,ev; 

  sc_vertex fv; //for configs 31/33/35
  av=T.A; bv=T.B; cv=T.C;
  dv= S1->find_other(T.A,T.B,T.C); ev= S2->find_other(T.A,T.B,T.C);

  switch (konf) {
    case 30:
     Ps1=Cl->opposite_simplex(S1,T.C); Ps2=Cl->opposite_simplex(S2,T.C); break;
    case 31:
     Ps1=Cl->opposite_simplex(S1,T.C); Ps2=Cl->opposite_simplex(S2,T.C); break;
    case 32:
     Ps1=Cl->opposite_simplex(S1,T.B); Ps2=Cl->opposite_simplex(S2,T.B); break;
    case 33:
     Ps1=Cl->opposite_simplex(S1,T.B); Ps2=Cl->opposite_simplex(S2,T.B); break;
    case 34:
     Ps1=Cl->opposite_simplex(S1,T.A); Ps2=Cl->opposite_simplex(S2,T.A); break;
    case 35:
     Ps1=Cl->opposite_simplex(S1,T.A); Ps2=Cl->opposite_simplex(S2,T.A); break;
  }

  // save old links
  link_save(S1,S2,Ps1,SN); link_save(S2,S1,Ps2,SN);
  if (Ps1 != NULL) link_save(Ps1,Ps2,S1,PSN);
  if (Ps2 != NULL) link_save(Ps2,Ps1,S2,PSN);
  
  if (konf==30 || konf==32 || konf==34){

    S1->clear_links(); S2->clear_links();
    if (Ps1 !=NULL) Ps1->clear_links(); 
    if (Ps2 !=NULL) Ps2->clear_links();

    switch (konf){
    case 30:
     Cl->flk30++;
     S1->change(av,cv,dv,ev);
     S2->change(bv,cv,dv,ev);
     Cl->set_link(S1,S2,cv,dv,ev);
     if (Ps1 !=NULL) Ps1->change(av,dv,ev);
     if (Ps2 !=NULL) Ps2->change(bv,dv,ev);
     if (Ps1 !=NULL && Ps2!=NULL)  Cl->set_link(Ps1,Ps2,dv,dv,ev); 

     if (Ps1 !=NULL) Cl->set_link(S1,Ps1,av,dv,ev); 
     if (Ps2 !=NULL) Cl->set_link(S2,Ps2,bv,dv,ev); 
     break;
    case 32: 
     Cl->flk32++;
     S1->change(av,bv,dv,ev);
     S2->change(cv,bv,dv,ev);
     Cl->set_link(S1,S2,bv,dv,ev);
     if (Ps1 !=NULL) Ps1->change(av,dv,ev);
     if (Ps2 !=NULL) Ps2->change(cv,dv,ev);
     if (Ps1 !=NULL && Ps2!=NULL) Cl->set_link(Ps1,Ps2,dv,dv,ev);

     if (Ps1 !=NULL) Cl->set_link(S1,Ps1,av,dv,ev);
     if (Ps2 !=NULL) Cl->set_link(S2,Ps2,cv,dv,ev);
     break;
    case 34: 
     Cl->flk34++;
     S1->change(bv,av,dv,ev);
     S2->change(cv,av,dv,ev);
     Cl->set_link(S1,S2,av,dv,ev);
     if (Ps1 !=NULL) Ps1->change(bv,dv,ev);
     if (Ps2 !=NULL) Ps2->change(cv,dv,ev);
     if (Ps1 !=NULL && Ps2 !=NULL) Cl->set_link(Ps1,Ps2,dv,dv,ev); 

     if (Ps1 !=NULL) Cl->set_link(S1,Ps1,bv,dv,ev);
     if (Ps2 !=NULL) Cl->set_link(S2,Ps2,cv,dv,ev);
     break;
    }
  }
  else { // konf==31/33/35

    switch (konf){
    case 31: 
     Cl->flk31++;
     fv=Cl->opposite_vertex(S1,T.C);
     S1->clear_links(); S2->clear_links(); 
     if (Ps1 !=NULL) Ps1->clear_links(); 
     if (Ps2 !=NULL) Ps2->clear_links();
     S1->change(av,cv,dv,ev);
     S2->change(bv,cv,dv,ev);
     Cl->set_link(S1,S2,cv,dv,ev);
     if (Ps1 !=NULL) Ps1->change(av,fv,dv,ev);
     if (Ps2 !=NULL) Ps2->change(bv,fv,dv,ev);
     if (Ps1 !=NULL && Ps2!=NULL) Cl->set_link(Ps1,Ps2,fv,dv,ev);

     if (Ps1 !=NULL) Cl->set_link(S1,Ps1,av,dv,ev);
     if (Ps2 !=NULL) Cl->set_link(S2,Ps2,bv,dv,ev);
     break;
    case 33:
     Cl->flk33++;
     fv=Cl->opposite_vertex(S1,T.B);
     S1->clear_links(); S2->clear_links(); 
     if (Ps1 !=NULL) Ps1->clear_links();
     if (Ps2 !=NULL) Ps2->clear_links();
     S1->change(av,bv,dv,ev);
     S2->change(cv,bv,dv,ev);
     Cl->set_link(S1,S2,bv,dv,ev);
     if (Ps1 !=NULL) Ps1->change(av,fv,dv,ev);
     if (Ps2 !=NULL) Ps2->change(cv,fv,dv,ev);
     if (Ps1 !=NULL && Ps2!=NULL) Cl->set_link(Ps1,Ps2,fv,dv,ev);
     if (Ps1 !=NULL) Cl->set_link(S1,Ps1,av,dv,ev);
     if (Ps2 !=NULL) Cl->set_link(S2,Ps2,cv,dv,ev);
     break;
    case 35: 
     Cl->flk35++;
     fv=Cl->opposite_vertex(S1,T.A);
     S1->clear_links(); S2->clear_links(); 
     if (Ps1 !=NULL) Ps1->clear_links();
     if (Ps2 !=NULL) Ps2->clear_links();
     S1->change(bv,av,dv,ev);
     S2->change(cv,av,dv,ev);
     Cl->set_link(S1,S2,av,dv,ev);
     if (Ps1 !=NULL) Ps1->change(bv,fv,dv,ev);
     if (Ps2 !=NULL) Ps2->change(cv,fv,dv,ev);
     if (Ps1 !=NULL && Ps2 !=NULL) Cl->set_link(Ps1,Ps2,fv,dv,ev); 
     if (Ps1 !=NULL) Cl->set_link(S1,Ps1,bv,dv,ev);
     if (Ps2 !=NULL) Cl->set_link(S2,Ps2,cv,dv,ev);
     break;
    }
  }

  // correction of links
  link_korr(S1,SN,TQ,PV); link_korr(S2,SN,TQ,PV);
  if (Ps1 !=NULL) link_korr(Ps1,PSN,TQ,PV);
  if (Ps2 !=NULL) link_korr(Ps2,PSN,TQ,PV);
}


void d3_simpl_complex::flip_konf1(sc_simplex S1, sc_simplex S2,d3_simpl_complex* Cl,int konf,Simpl_Triang& T,queue<Simpl_Triang>& TQ,sc_vertex PV)
// put boundaries into TL
{
  list<sc_simplex> SN;

  sc_simplex S3; 

  sc_vertex av,bv,cv,dv,ev;
  av=T.A; bv=T.B; cv=T.C; // 3 common D3_vertices...
  dv= S1->find_other(T.A,T.B,T.C); ev= S2->find_other(T.A,T.B,T.C);

  if (konf==1){ // 1 new simplex!!
   // save old links
   link_save(S1,S2,S2,SN); link_save(S2,S1,S1,SN);
   Cl->flk1++;
   S1->change(bv,av,dv,ev); S2->change(cv,av,dv,ev);
   S1->clear_links(); S2->clear_links();
   S3=Cl->add_simplex(bv,cv,dv,ev);
   S1->bcd=S2; S2->bcd=S1; 
   S1->acd=S3; S3->acd=S1; 
   S2->acd=S3; S3->bcd=S2; 
   
   link_korr(S1,SN,TQ,PV); link_korr(S2,SN,TQ,PV); link_korr(S3,SN,TQ,PV);
  }
  else { // remove 1 simplex
    switch(konf){
    case 10:
     Cl->flk10++;
     S3=Cl->opposite_simplex(S1,T.C);
     link_save(S1,S2,S3,SN); link_save(S2,S1,S3,SN); link_save(S3,S1,S2,SN);
     Cl->remove_simplex(S3);
     S1->clear_links(); S2->clear_links();
     S1->change(cv,bv,dv,ev); S2->change(cv,av,dv,ev);  
     S1->acd=S2; S2->acd=S1; 
     break;
    case 11:
     Cl->flk11++;
     S3=Cl->opposite_simplex(S1,T.B);
     link_save(S1,S2,S3,SN); link_save(S2,S1,S3,SN); link_save(S3,S1,S2,SN); 
     Cl->remove_simplex(S3);
     S1->clear_links(); S2->clear_links();
     S1->change(bv,av,dv,ev); S2->change(bv,cv,dv,ev);
     S1->acd=S2; S2->acd=S1; 
     break;
    case 12:
     Cl->flk12++;
     S3=Cl->opposite_simplex(S1,T.A);
     link_save(S1,S2,S3,SN); link_save(S2,S1,S3,SN); link_save(S3,S1,S2,SN);  
     Cl->remove_simplex(S3);
     S1->clear_links(); S2->clear_links();
     S1->change(av,bv,dv,ev); S2->change(av,cv,dv,ev);
     S1->acd=S2; S2->acd=S1; 
     break;
    }
    link_korr(S1,SN,TQ,PV); link_korr(S2,SN,TQ,PV);
  }
}

int d3_simpl_complex::d2_test_convex(sc_vertex a,sc_vertex b,sc_vertex c,sc_vertex d)
//  returns 1, if the 4 points stored by 4 D3-vertices, that lie in a plane (!), form a convex quadrangle.

{
 d3_rat_point ap,bp,cp,dp;
 int o1,o2,o3,o4;
 ap=a->stor; bp=b->stor;cp=c->stor; dp=d->stor;

 o1=orientation_xy(ap,bp,cp);o2=orientation_xy(bp,cp,dp);
 o3=orientation_xy(cp,dp,ap);o4=orientation_xy(dp,ap,bp);

 if (o1==0 && o2==0 && o3==0 && o4==0){ 
  o1=orientation_xz(ap,bp,cp);o2=orientation_xz(bp,cp,dp);
  o3=orientation_xz(cp,dp,ap);o4=orientation_xz(dp,ap,bp);
  if (o1==0 && o2==0 && o3==0 && o4==0){
    o1=orientation_yz(ap,bp,cp);o2=orientation_yz(bp,cp,dp);
    o3=orientation_yz(cp,dp,ap);o4=orientation_yz(dp,ap,bp);
  }
 }

 if (o1==o2 && o3==o4 && o2==o3) {
  if (o1 !=0) return 1;
  else {
    o1=orientation_xz(ap,bp,cp);o2=orientation_xz(bp,cp,dp);
    o3=orientation_xz(cp,dp,ap);o4=orientation_xz(dp,ap,bp);    
    if (o1==o2 && o3==o4 && o2==o3) {
     if (o1 !=0) return 1;    
    }
    else {
     o1=orientation_yz(ap,bp,cp);o2=orientation_yz(bp,cp,dp);
     o3=orientation_yz(cp,dp,ap);o4=orientation_yz(dp,ap,bp);
     if (o1==o2 && o3==o4 && o2==o3 && o1!=0) return 1;         
    }
  }
 }

 return 0;
}

int d3_simpl_complex::flip(sc_simplex S1, sc_simplex S2,Simpl_Triang& T)
{
 int wert=1;

 if (S1->com_of != this || S2->com_of != this) return -1; 

 if (S1->HULL_TR && S2->HULL_TR){ // 2 Hulltriangles...
  list<Simpl_Triang> TRL;

  list<sc_simplex> LS;

  sc_vertex av,bv,dv,ev;
  av=T.A; bv=T.B; 
  dv= S1->find_other(T.A,T.B,T.B); ev= S2->find_other(T.A,T.B,T.B);

  int cv=d2_test_convex(dv,av,ev,bv); // d2-flip possible ?
  if (cv==0) return 0; 

  link_save(S1,S2,S2,LS);link_save(S2,S1,S1,LS);
  S1->clear_links(); 
  S2->clear_links();
  S1->change(av,dv,ev); 
  S2->change(bv,dv,ev);
  
  set_link(S1,av,S2,bv);
  link_korr(S1,LS,TRL); link_korr(S2,LS,TRL);

  return wert;
    
 }

 else {
  queue<Simpl_Triang> TQ;
  sc_vertex PV=NULL;

  int konf= configuration(S1,S2,T);

  if ( (konf==1) || (konf>9 && konf<13) || (konf>29 && konf<36) ){
   if ( (konf==1)|| (konf>9 && konf<13) ) flip_konf1(S1,S2,this,konf,T,TQ,PV);
   else flip_konf3(S1,S2,this,konf,T,TQ,PV);
  }
  return konf;
 }

}

int d3_simpl_complex::flip(sc_simplex S1, sc_simplex S2,
                           queue<Simpl_Triang>& TQ, Simpl_Triang& T,
                                                    sc_vertex PV)
{
  if (S1->HULL_TR && S2->HULL_TR) return flip(S1,S2,T);    

  int conf = configuration(S1,S2,T);

  if (conf==1 || (conf>9 && conf<13)) 
     flip_konf1(S1,S2,this,conf,T,TQ,PV);

  if (conf>29 && conf<36) 
     flip_konf3(S1,S2,this,conf,T,TQ,PV);

  return conf;

}

void d3_simpl_complex::input_complex()
{
 int NV,NSI,NN;
 int i;
 rational xw,yw,zw;
 int nb1,nb2,nb3,nb4;

 clear(); 

 cout << "Number of Vertices :"; cin >> NV;
 cout << "Number of Simplices:"; cin >> NSI;
 cout << "Number of Neighboursettings :"; cin >> NN;

 d3_rat_point* P = new d3_rat_point[NV];
 sc_vertex*    V = new sc_vertex[NV];
 sc_simplex*   S = new sc_simplex[NSI];

 for (i=0;i<NV;i++) {
  cout << i << ". point:\n";
  cout << " "; cin >> xw;
  cout << " "; cin >> yw;
  cout << " "; cin >> zw;
   
  P[i]=d3_rat_point(xw,yw,zw);
  V[i]=add_vertex(P[i]); 
 } 

 for (i=0;i<NSI;i++) { 
  cout << i << ". simplex:\n";
  cout << " 1. vertex:"; cin >> nb1;
  if (nb1 >= NV) { cout << "false input!\n"; nb1=0; }
  cout << " 2. vertex:"; cin >> nb2;
  if (nb2 >= NV) { cout << "false input!\n"; nb2=0; }
  cout << " 3. vertex:"; cin >> nb3;
  if (nb3 >= NV) { cout << "false input!\n"; nb3=0; }
  cout << " 4. vertex:"; cin >> nb4; 
  if (nb4 >= NV) { cout << "false input!\n"; nb4=0; }

  if (nb4 != -1) S[i]=add_simplex(V[nb1],V[nb2],V[nb3],V[nb4]);
  else S[i]=add_simplex(V[nb1],V[nb2],V[nb3]);
 }
 
 sc_vertex u1,u2;

 for (i=0;i<NN;i++) {
  cout << i << ". link:\n";
  cout << " 1. simplex:"; cin >> nb1;
  cout << " 1. vertex :"; cin >> nb2;
  cout << " 2. simplex:"; cin >> nb3;
  cout << " 2. vertex :"; cin >> nb4;  
  if (nb2 !=-1) u1=V[nb2]; else u1=NULL;
  if (nb4 !=-1) u2=V[nb4]; else u2=NULL; 
  set_link(S[nb1],u1,S[nb3],u2);
 }

 delete[] P; delete[] V; delete[] S;
}

sc_simplex d3_simpl_complex::insert_vertex(sc_simplex S,sc_vertex v)
{
 return triang_simplex(S,v);
}

sc_simplex d3_simpl_complex::triang_simplex(sc_simplex,sc_vertex, int)
{
 return 0;
}

sc_simplex d3_simpl_complex::triang_simplex(sc_simplex S,sc_vertex v)
// v inside S; triangulate new...
{
 if (v->com_of != this || S->com_of != this) return NULL;

 d3_rat_point p= v->stor;
 sc_simplex sn1,sn2,sn3;
 d3_rat_point A,B,C,D;
 sc_vertex va,vb,vc,vd;
 list<sc_simplex> Sn;

 if ( ! S->in_simplex(p)) return NULL; 

 if (S->HULL_TR){ 
  if (S->abc==NULL){ 
    va= S->a; 
    vb=S->b; 
    vc=S->c; 
    A= va->stor; 
    B= vb->stor; 
    C= vc->stor; 

    if (p==A || p==B || p==C) return NULL;

    link_save(S,NULL,NULL,Sn);

    S->clear_links();
    sn1=add_simplex(va,vb,v); 
    sn2=add_simplex(va,vc,v); 
    S->change(vb,vc,v);  
    set_link(sn1,vb,sn2,vc); 
    set_link(sn1,va,S,vc); 
    set_link(sn2,va,S,vb); 
    list<Simpl_Triang> TRL;
    link_korr(sn1,Sn,TRL); 
    link_korr(sn2,Sn,TRL); 
    link_korr(S,Sn,TRL);  

    return S;  
  }
  else {
   sc_simplex S_other=S->abc;
   
   return triang_simplex(S_other,v); 
  }
 }
 else {
  va=S->a; 
  vb=S->b; 
  vc=S->c; 
  vd=S->d;
  A = va->stor; 
  B = vb->stor; 
  C = vc->stor; 
  D = vd->stor; 

  if (p==A || p==B || p==C || p==D) return NULL;


  link_save(S,NULL,NULL,Sn);

  //new simplex
  S->clear_links();
  sn1 = add_simplex(va,vb,vc,v); 
  sn2 = add_simplex(va,vb,vd,v);
  sn3 = add_simplex(va,vc,vd,v);
  S->change(vb,vc,vd,v);

  //set new links...
  set_link(sn1,vc,sn2,vd); 
  set_link(sn1,vb,sn3,vd);
  set_link(sn1,va,S,vd);
  set_link(sn2,vb,sn3,vc); 
  set_link(sn2,va,S,vc);
  set_link(sn3,va,S,vb); 
  list<Simpl_Triang> Trl;
  link_korr(sn1,Sn,Trl); 
  link_korr(sn2,Sn,Trl); 
  link_korr(sn3,Sn,Trl); 
  link_korr(S,Sn,Trl);  

  return sn1;
 }

}

sc_simplex d3_simpl_complex::triang_simplex(sc_simplex S,d3_rat_point p)
{
 sc_vertex v= add_vertex(p);
 return triang_simplex(S,v);
}

bool d3_simpl_complex::all_delaunay_check()
{
 sc_simplex  fst;
 sc_vertex ac;
 bool flag=true;
             
 forall(fst,Sil) {
  forall(ac,Vert){
    if (fst->insphere(ac->stor))  flag=false;
  }
 }

 return flag;
}

bool d3_simpl_complex::delaunay_check(int w)
{
 int st;
 if (w==1) { st=check_delaunay(); if (st==0) return true; else return false; }
 else return all_delaunay_check();
}

int d3_simpl_complex::check(int output,ostream& ost)
// output information about the simplex complex, perform some tests...
// output==0 ... no output
// output==1 ... only errors and number of vertices/simplices
// output >1 ... like 1 but output all vertices/simplices too
{
 if (output>0) ost << "check:" << Sil.size() << " simplices; " << Vert.size() << " vertices!\n";
 sc_vertex aktual;
 sc_simplex S,S2;

 // Simplices und Vertices ausgeben...
 if (output>1){

  ost << "vertices:\n";
  ost << "---------\n";
  forall(aktual,Vert){
   ost << *aktual << "\n";
  }
  ost << "simplices:\n";
  ost << "----------\n";
  forall(S,Sil){
   ost << *S;
  }
 }

 // check links ...

 list<sc_simplex> an;
 int wert;
 int rueck=0;
 Simpl_Triang Tr;
 int tw;
 int nbsets=0;
 sc_simplex s1,s2,s3,s4;
 int sizahl=0;
 int htrzahl=0;

 forall(S,Sil){
   sizahl++;
   if ( S->HULL_TR){
     htrzahl++;
     s1= S->abd; s2= S->acd; s3= S->bcd; s4= S->abc;
     if (s1 !=NULL) {
       if (! s1->HULL_TR) { rueck=1; if (output>0) ost << "CHECKER --- error abd ---\n"; }
     }
     if (s2 !=NULL) {
       if (! s2->HULL_TR) { rueck=1; if (output>0) ost << "CHECKER --- error acd ---\n"; } 
     }
     if (s3 !=NULL) {
       if (! s3->HULL_TR) { rueck=1; if (output>0) ost << "CHECKER --- error bcd ---\n"; }
     }
     if (s4 !=NULL) {
       if ( s4->HULL_TR)  { rueck=1; if (output>0) ost << "CHECKER --- error abc ---\n"; }
     }
   }
   // checks for non-hulltrs. ("real" simplices)
   else {
     if (coplanar(S->a->stor,S->b->stor,S->c->stor,S->d->stor)) {
       rueck=1;
       if (output>0) ost << "Problem - simplex flat!\n";
     }
     if ( S->d == NULL) {
       rueck=1;
       if (output>0) ost << "Problem - d-pointer is NULL for a non-hulltr.!\n";
     }
   }

   an=S->all_neighbours();
   nbsets=nbsets+an.size();

   forall(S2,an){
     if (S2==S) { 
      rueck=1;
      if (output>0) ost << "The neighbour is the simplex himself!\n";
     }
     wert=get_link_index(S2,S);
     if (wert < 1) { 
       if (output>0) ost << "CHECKER --- a neighbour link is not reversal!\n";
       rueck=1;
     }
     else {
       tw=common_triangle(S, S2, Tr); 
       wert=S2->control_vert(Tr.A,Tr.B,Tr.C);    
       if (wert==0){
          if (output>0) 
          ost << "CHECKER --- false neighbourhood between simplices " 
               <<  S->name << "/" << S2->name << " " << Tr <<  " !\n";
          rueck=1; 
       }
     }
   }   
 }

 if (output>0) 
  cout << nbsets << " neighbour-links ( " << (int)(nbsets/2) << " pairs).\n";

 if (Sil.size() != sizahl) { 
   if (output>0) 
       cout << "error - number of simplices. Sil.size:" 
            << Sil.size() << " sizahl:" << sizahl << "\n";
   rueck=1;
 }
 
 if (output>0) cout << htrzahl << " hulltriangles!\n";

 return rueck;
}

bool test_inside(d3_rat_point a,d3_rat_point b,d3_rat_point c,d3_rat_point other)
{
 d3_rat_point d=point_on_positive_side(a,b,c);
 bool w=inside_sphere(a,b,c,d,other);
 return w;
}







// - TRIANGULATE_POINTS on a  d3-plane (points sorted)
//   look at the d2 LEDA-sources


#define left_bend(p,e)   (orientation(p,G[source(e)],G[target(e)]) > 0)



void DELAUNAY_FLIPPING(d3_simpl_complex& Comp, list<Simpl_Triang>& S)
{
  Simpl_Triang T,Trl,Th;
  int tf;
  list<sc_simplex> LS1,LS2;
  sc_simplex si;
  int flipcount=0;  

  while ( !S.empty() )
  { T=S.pop();

    if (T.S1==NULL || T.S2==NULL) continue;

    if (Comp.flippable(T.S1,T.S2,Th)) 
      tf=Comp.flip(T.S1,T.S2,Th); 
    else tf=0;

    if (tf>0)
    { 
      flipcount++;

      LS1.clear(); LS2.clear();

      if (Comp.link_acd(T.S1) != T.S2) LS1.append(Comp.link_acd(T.S1)); 
      if (Comp.link_abd(T.S1) != T.S2) LS1.append(Comp.link_abd(T.S1)); 
      if (Comp.link_bcd(T.S1) != T.S2) LS1.append(Comp.link_bcd(T.S1));

      if (Comp.link_acd(T.S2) != T.S1) LS2.append(Comp.link_acd(T.S2));
      if (Comp.link_abd(T.S2) != T.S1) LS2.append(Comp.link_abd(T.S2)); 
      if (Comp.link_bcd(T.S2) != T.S1) LS2.append(Comp.link_bcd(T.S2));
      
     
      forall(si,LS1){ Trl.S1=si; Trl.S2=T.S1; S.push(Trl); }
      forall(si,LS2){ Trl.S1=si; Trl.S2=T.S2; S.push(Trl); }
    }
  }

}

void DELAUNAY_FLIPPING(d3_simpl_complex& Comp)
{
  list<Simpl_Triang> S;
  list<sc_simplex> all= Comp.all_simplices();
  Simpl_Triang Trl;

  if (all.empty()) return;

  sc_simplex akt;
  sc_simplex sother;
  forall (akt,all) Comp.set_Marker(akt,-1);
  
  queue<sc_simplex> Q;
  akt= all.head(); Comp.set_Marker(akt,-98);
  Q.append(akt);

  //visited:Marker==-98; linked:Marker==-99

  while (! Q.empty()){
   akt=Q.pop(); Comp.set_Marker(akt,-99);
   sother= Comp.link_abd(akt);
   if (sother!=NULL && Comp.get_Marker(sother) !=-99){ 
    Trl.S1=akt; Trl.S2=sother; S.append(Trl);
    if (Comp.get_Marker(sother)==-1) { Q.append(sother); Comp.set_Marker(sother,-98); }
   }
   sother= Comp.link_acd(akt);
   if (sother!=NULL && Comp.get_Marker(sother) !=-99){
    Trl.S1=akt; Trl.S2=sother; S.append(Trl);
    if (Comp.get_Marker(sother)==-1) { Q.append(sother); Comp.set_Marker(sother,-98); }   
   }
   sother= Comp.link_bcd(akt);  
   if (sother!=NULL && Comp.get_Marker(sother) !=-99){
    Trl.S1=akt; Trl.S2=sother; S.append(Trl);
    if (Comp.get_Marker(sother)==-1) { Q.append(sother); Comp.set_Marker(sother,-98); }
   }
  }

  if (S.size() > 0) DELAUNAY_FLIPPING(Comp,S);
}

edge TRIANGULATE_POINTS(const list<rpoint_zg>& L0, GRAPH<rpoint_zg,sc_simplex>& G, d3_simpl_complex& C)
// input list has to be sorted
{ 
  G.clear();
  if (L0.empty()) return nil;

  list<rpoint_zg> L = L0;

  rpoint_zg first,sec,act;

  rpoint_zg last_p = L.pop();            
  node  last_v = G.new_node(last_p);
  first=last_p;

  while (!L.empty() && last_p == L.head()) L.pop();
  edge x,y;
  list<edge> vorn;

  if (!L.empty())
  { last_p = L.pop();
    sec= last_p;
    node v = G.new_node(last_p);
    x = G.new_edge(last_v,v,0); G[x]=NULL; vorn.append(x);
    y = G.new_edge(v,last_v,0); G[y]=NULL; 
    G.set_reversal(x,y);
    last_v = v;
  }
   
  rpoint_zg p;
  sc_simplex sakt,slast=NULL;
/*
  edge first1=x,first2=y; 
  bool fflag=rue;
*/
 int zlr=0;
  if ((!L.empty()) && collinear(first,sec,L.head())) { 
       act=L.pop();
       zlr++;
       node v= G.new_node(act);
       x= G.new_edge(last_v,v,0); G[x]=NULL; vorn.append(x);
       y= G.new_edge(v,last_v,0); G[x]=NULL;
       G.set_reversal(x,y);
       last_v=v;
  }

  forall(p,L) 
  { if (p == last_p) continue; 

    edge e =  G.last_adj_edge(last_v);

    last_v = G.new_node(p);
    last_p = p;
    slast = NULL;

    // walk up to upper tangent
    do e = G.pred_face_edge(e); while (left_bend(p,e));

    // walk down to lower tangent and triangulate
    do { edge succ_e = G.succ_face_edge(e);
         x = G.new_edge(succ_e,last_v,0,0);
         y = G.new_edge(last_v,source(succ_e),0);
         G.set_reversal(x,y);

         if (left_bend(p,succ_e)) {
            sakt=C.add_simplex((G[source(succ_e)]).v,(G[target(succ_e)]).v,(G[last_v]).v);
            G[x]=sakt;
            G[y]=sakt;
            if (slast!=NULL){
              C.set_link(slast,sakt,1); C.set_link(sakt,slast,2);
            }
            if (G[succ_e]!=NULL){
             C.set_link(sakt,G[succ_e],(G[source(succ_e)]).v,(G[target(succ_e)]).v,(G[target(succ_e)]).v);
            }
            else { G[succ_e]=sakt; G[G.reversal(succ_e)]=sakt; }

            slast=sakt;
         }
         else { G[x]=slast; G[y]=slast; }

         e = succ_e;
       } while (left_bend(p,e));
   }

  edge hull_edge = G.last_edge();

  return hull_edge;
}

edge TRIANGULATE_POINTS(list<sc_vertex>& L0, GRAPH<rpoint_zg,sc_simplex>& G,d3_simpl_complex& C,int wert)
{
 list<rpoint_zg> L1;
 sc_vertex v;
 rpoint_zg akt;
 rat_point rp;

 forall(v,L0){
   switch(wert){
   case 0:
     rp=(C.pos(v)).project_xy();
     break;
   case 1:
     rp=(C.pos(v)).project_xz();
     break;
   case 2:
     rp=(C.pos(v)).project_yz();
     break;
   }
   akt.p=rp; 
   akt.v=v;
   L1.append(akt);
 }

 return TRIANGULATE_POINTS(L1,G,C);
}

edge build_simp_comp(list<d3_rat_point>& L, d3_simpl_complex& C,int wert)
{
 d3_rat_point p;
 list<sc_vertex> LV;
 sc_vertex vz;

 forall(p,L){
  vz= C.add_vertex(p);
  LV.append(vz);
 }

 GRAPH<rpoint_zg,sc_simplex> G;
 edge he=TRIANGULATE_POINTS(LV,G,C,wert);
 return he;
}




// Delaunay-Triangulierung in 3d unter Verwendung der 
// 3d-Simplexkomplexklasse d3_simpl_complex




void calculate_triang(list<d3_rat_point>& L,d3_simpl_complex& Comp,int wert)
{ build_simp_comp(L,Comp,wert);
  DELAUNAY_FLIPPING(Comp);
  return;                                                            
}

sc_simplex spec_case(list<d3_rat_point>& L,d3_simpl_complex& Comp,
                     int fall,int wert)
{ 
 sc_simplex last=NULL;
 d3_rat_point p1,p2;
 list_item it;
 sc_vertex v1,v2;
 sc_simplex s1,s2;
 s1=NULL;s2=NULL;

 if (fall==1){
  p1=L.contents(L.first());
  v1=Comp.add_vertex(p1);
  forall_items(it,L){ 
    if (it != L.first()){
     p2=L.contents(it); v2=Comp.add_vertex(p2);
     s2=Comp.add_simplex(v1,v2,v2);
     if (s1!=NULL){
       Comp.set_link(s1,s2,1); Comp.set_link(s2,s1,3);
     }
     s1=s2;
     v1=v2;
    }
  }
  last=s2;
 }
 else { // coplanar case
  calculate_triang(L,Comp,wert); // compute triangulation 
 }
 
 return last;
}


int get_pro_plane(d3_rat_point A,d3_rat_point B,d3_rat_point C)
{
 // get projection plane
 
 rat_point ap=A.project_xy(); 
 rat_point bp=B.project_xy(); 
 rat_point cp=C.project_xy();
 if (! collinear(ap,bp,cp)) return 0;

 ap=A.project_xz(); 
 bp=B.project_xz(); 
 cp=C.project_xz();
 if (! collinear(ap,bp,cp)) return 1;

 return 2;
}

sc_simplex comp_initial_tr(list<d3_rat_point>& L, d3_simpl_complex& Comp,
                           d3_rat_point& lastp, list<sc_simplex>& trhull)
{
 // compute start-DT, put it into  C
 
 sc_simplex last;
 d3_rat_point A,B,C,D;
 sc_vertex  v1,v2,v3,v4;

 if (L.size() < 4){
  int sz=L.size();
  switch (sz){
  case 1:
   A=L.pop();
   v1=Comp.add_vertex(A);
   break;
  case 2:
   // degen. simplex
   A=L.pop(); B=L.pop();
   v1=Comp.add_vertex(A); v2=Comp.add_vertex(B);
   last=Comp.add_simplex(v1,v2,v2);
   break;
  case 3:
   A=L.pop(); B=L.pop(); C=L.pop();
   v1=Comp.add_vertex(A); v2=Comp.add_vertex(B); v3=Comp.add_vertex(C);
   last=Comp.add_simplex(v1,v2,v3);
   break;
  }
  return NULL;
 }

 // normal case
 list<d3_rat_point> cpl;
 
 A=L.pop(); B=L.pop(); C=L.pop();
 cpl.append(A); cpl.append(B);

 while ( collinear(A,B,C) && !L.empty() ){
  cpl.append(C);
  C=L.pop(); 
 }
 cpl.append(C);
 if (L.empty() && collinear(A,B,C)) { last=spec_case(cpl,Comp,1,0); return last; } // all points collinear
 if (L.empty()) {
   int wert=get_pro_plane(A,B,C);
   last=spec_case(cpl,Comp,2,wert); return last; 
 }  

 D=L.pop();
 while ( coplanar(A,B,C,D) && !L.empty() ){
   cpl.append(D);
   D=L.pop();
 }
 if (L.empty() && coplanar(A,B,C,D)) cpl.append(D);
 lastp=D;

 if (L.empty() && cpl.size()>3) { 
   int wert=get_pro_plane(A,B,C);
   last=spec_case(cpl,Comp,2,wert); return last; 
 } // all points coplanar 

 if (cpl.size() == 3){ //first 4 points not coplanar

   v1=Comp.add_vertex(A); v2=Comp.add_vertex(B); v3=Comp.add_vertex(C); v4=Comp.add_vertex(D);
    
   sc_simplex s1,s2,s3,s4,s5;
   s1=Comp.add_simplex(v1,v2,v3,v4);
   s2=Comp.add_simplex(v1,v2,v3); s3=Comp.add_simplex(v1,v2,v4);
   s4=Comp.add_simplex(v2,v3,v4); s5=Comp.add_simplex(v1,v3,v4);

   // neighboursettings
   // S1
   Comp.set_link(s1,v4,s2,NULL); Comp.set_link(s1,v3,s3,NULL);
   Comp.set_link(s1,v1,s4,NULL); Comp.set_link(s1,v2,s5,NULL);
   // others
   Comp.set_link(s2,v3,s3,v4); Comp.set_link(s2,v1,s4,v4);
   Comp.set_link(s2,v2,s5,v4); Comp.set_link(s3,v1,s4,v3);
   Comp.set_link(s3,v2,s5,v3); Comp.set_link(s4,v2,s5,v1);
   
   trhull.append(s2); trhull.append(s3); trhull.append(s5); trhull.append(s4); // hullsimplices
   return s4;
 }
 else {
  int wert=get_pro_plane(A,B,C);
  last=spec_case(cpl,Comp,2,wert);

  sc_vertex DV=Comp.add_vertex(D);
  sc_simplex sakt=0;
  sc_simplex sneu=0;
  list<sc_simplex> akt=Comp.Sil;
  list<sc_simplex> NL; 

  forall(sakt,akt){
    sneu=Comp.add_simplex(Comp.vertex_a(sakt),Comp.vertex_b(sakt),Comp.vertex_c(sakt));
    NL.append(sneu);
    Comp.set_link(sakt,sneu,4);
    Comp.set_link(sneu,sakt,4);
    Comp.set_hull_info(sakt,false);
    Comp.set_vertex(sakt,DV,4);
  }

  sc_simplex geg, pt;  
  list<sc_simplex> NHTR;

  forall(sakt,NL){ 
    geg= Comp.link_abc(sakt);
    pt= Comp.link_abd(geg);
    if (pt !=NULL) Comp.set_link(sakt,Comp.link_abc(pt),3);
    else {
     sneu=Comp.add_simplex(Comp.vertex_a(sakt),Comp.vertex_b(sakt),DV);
     trhull.append(sneu);
     Comp.set_link(sakt,sneu,3); Comp.set_link(sneu,sakt,3);
     Comp.set_link(sneu,geg,4); Comp.set_link(geg,sneu,3);
  
     Comp.set_vert_sim_link(Comp.vertex_a(sneu),NULL);
     Comp.set_vert_sim_link(Comp.vertex_b(sneu),NULL);
     NHTR.append(sneu);
    }
    pt=Comp.link_acd(geg);
    if (pt !=NULL) Comp.set_link(sakt,Comp.link_abc(pt),2); 
    else {
     sneu=Comp.add_simplex(Comp.vertex_a(sakt),Comp.vertex_c(sakt),DV);
     trhull.append(sneu);
     Comp.set_link(sakt,sneu,2); Comp.set_link(sneu,sakt,3);     
  
     Comp.set_link(sneu,geg,4); Comp.set_link(geg,sneu,2);
     Comp.set_vert_sim_link(Comp.vertex_a(sneu),NULL);
     Comp.set_vert_sim_link(Comp.vertex_b(sneu),NULL);
 
     NHTR.append(sneu); 
    }
    pt= Comp.link_bcd(geg);
    if (pt !=NULL) Comp.set_link(sakt,Comp.link_abc(pt),1); 
    else {
     sneu=Comp.add_simplex(Comp.vertex_b(sakt),Comp.vertex_c(sakt),DV);
     trhull.append(sneu);
     Comp.set_link(sakt,sneu,1); Comp.set_link(sneu,sakt,3);     

     Comp.set_link(sneu,geg,4); Comp.set_link(geg,sneu,1);
     Comp.set_vert_sim_link(Comp.vertex_a(sneu),NULL);
     Comp.set_vert_sim_link(Comp.vertex_b(sneu),NULL);
 
     NHTR.append(sneu); 
    } 
  }
  last=sneu; //a new hulltr.

  // setting links...
  sc_simplex sa,sb;

  forall(sakt,NHTR){
    sa= Comp.get_simplex(Comp.vertex_a(sakt)); sb=Comp.get_simplex(Comp.vertex_b(sakt));
    if (sa !=NULL){ 
     Comp.set_link(sakt,sa,2);
     if (Comp.vertex_a(sakt)==Comp.vertex_b(sa)) Comp.set_link(sa,sakt,1); else Comp.set_link(sa,sakt,2);
    }
    else Comp.set_vert_sim_link(Comp.vertex_a(sakt),sakt);
   
    if (sb !=NULL){
     Comp.set_link(sakt,sb,1);
     if (Comp.vertex_b(sakt)==Comp.vertex_a(sb)) Comp.set_link(sb,sakt,2); else Comp.set_link(sb,sakt,1);
    } 
    else  Comp.set_vert_sim_link(Comp.vertex_b(sakt),sakt);
 
  }  

 }
 
 return last;
}


int orientation(d3_simpl_complex& Co, sc_vertex a, sc_vertex b,
                                      sc_vertex c, d3_rat_point d)
{
 return orientation (Co.pos(a),Co.pos(b),Co.pos(c),d);
}

int test_vis(d3_simpl_complex& C,sc_simplex vis, sc_simplex other, 
             sc_vertex  c1, sc_vertex  c2, d3_rat_point p,d3_rat_point pv)
{
 if (other==NULL) return -1;

 d3_rat_point cp1,cp2,cp3;
 cp1= C.pos(c1); cp2=C.pos(c2);

 cp3= C.pos(C.find_other(other,c1,c2,c2));
 int o1,o2;
 o1=orientation(pv,cp1,cp2,p);
 o2=orientation(cp1,cp3,cp2,p);
 
 if (o1==o2) return 1; else return -1; 

}


sc_simplex really_vis(d3_simpl_complex& Cm,sc_simplex cand,d3_rat_point p,
                      list<sc_simplex>& trhull)
{
 sc_simplex last=NULL;
 sc_simplex pt;
 d3_rat_point a,b,c,other1;

 do {

  a=Cm.pos(Cm.vertex_a(cand)); b=Cm.pos(Cm.vertex_b(cand)); c=Cm.pos(Cm.vertex_c(cand));
  pt=Cm.link_abd(cand);
  other1= Cm.pos(Cm.find_other(pt,Cm.vertex_a(cand),Cm.vertex_b(cand),Cm.vertex_b(cand)));

  int o1,o2;
  o1=orientation(a,b,c,p); o2=orientation(a,b,c,other1);

  if (o2==0){
   pt= Cm.link_acd(cand);
   other1= Cm.pos(Cm.find_other(pt,Cm.vertex_a(cand),Cm.vertex_c(cand),Cm.vertex_c(cand)));
   o2=orientation(a,b,c,other1);

   if (o2==0){
    pt=Cm.link_bcd(cand);
    other1= Cm.pos(Cm.find_other(pt,Cm.vertex_b(cand),Cm.vertex_c(cand),Cm.vertex_c(cand)));    
    o2=orientation(a,b,c,other1);

    if (o2==0){
     pt=Cm.link_abc(cand);
     other1= Cm.pos(Cm.find_other(pt,Cm.vertex_a(cand),Cm.vertex_b(cand),Cm.vertex_c(cand)));  
     o2=orientation(a,b,c,other1);      
    }
   }
  }
 
  if (o1==0 || o1==o2 || o2==0) { // problem...
   // not visible...
   if (! trhull.empty()) cand=trhull.pop();
   else  last=cand; 

  }
  else {
   last=cand;
  }

 } while (last != cand);

 return last; 
}


void find_visible_triangles(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,sc_simplex n,
                            d3_rat_point p,int counter,list<sc_simplex>& trhull)
// find the from p visible hulltrs., put them into Trl
{
 Trl.clear(); Border.clear();
 n=really_vis(C,n,p,trhull);
 
 Trl.append(n);
 C.set_Marker(n,counter); //mark first visible simplex ...

 // start searching ...
 queue<sc_simplex> Q;
 sc_simplex sakt;
 Simpl_Triang T;
 Q.append(n);
 sc_simplex s1,s2,s3;
  
 while (! Q.empty()){
   sakt=Q.pop();

   s1= C.link_abd(sakt); s2=C.link_bcd(sakt); s3=C.link_acd(sakt);

   if (s1!=NULL && C.get_Marker(s1) != counter){
    if (test_vis(C,sakt,s1,C.vertex_a(sakt),C.vertex_b(sakt),p, C.pos(C.vertex_c(sakt)) ) >0) { 
      Trl.append(s1); Q.append(s1); 
      C.set_Marker(s1,counter);
    }
    else { //bordertriangle...
      T.S1=s1; T.S2=sakt; T.A=C.vertex_a(sakt); T.B=C.vertex_b(sakt); 
      T.C=C.vertex_c(sakt); 
      Border.append(T);
    }
   }
   if (s2!=NULL && C.get_Marker(s2) != counter){
    if (test_vis(C,sakt,s2,C.vertex_b(sakt),C.vertex_c(sakt),p, C.pos(C.vertex_a(sakt)) ) >0) {
      Trl.append(s2); Q.append(s2);
      C.set_Marker(s2,counter);
    }
    else { //bordertriangle...
      T.S1=s2; T.S2=sakt; T.A=C.vertex_b(sakt); T.B=C.vertex_c(sakt); 
      T.C=C.vertex_a(sakt); 
      Border.append(T);
    }
   }
   if (s3!=NULL && C.get_Marker(s3) != counter){
    if (test_vis(C,sakt,s3, C.vertex_a(sakt),C.vertex_c(sakt),p, C.pos(C.vertex_b(sakt)) ) >0) {
      Trl.append(s3); Q.append(s3);
      C.set_Marker(s3,counter);
    }
    else { //bordertriangle...
      T.S1=s3; T.S2=sakt; T.A=C.vertex_a(sakt); T.B=C.vertex_c(sakt); 
      T.C=C.vertex_b(sakt); 
      Border.append(T);
    }
   }
 }

}

void check_htr(list<sc_simplex>& trhull)
{
  /*
 sc_simplex sakt;
 forall(sakt,trhull){
  assert(sakt->HULL_TR);
  assert(sakt->abc->HULL_TR);
  assert(sakt->abd->HULL_TR);
  assert(sakt->acd->HULL_TR);
  assert(sakt->bcd->HULL_TR);
 }
 */
}


sc_simplex triangulate_new(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,sc_vertex PV,
                         queue<Simpl_Triang>& TQ,list<sc_simplex>& trhull)
{

 sc_simplex last;
 sc_simplex akt;
 sc_simplex akt_abc;
 d3_rat_point p;

 p=C.pos(PV);

 // change triangles to real simplices
 forall(akt,Trl){
    C.set_hull_info(akt,false);
    C.set_vertex(akt,PV,4);
    akt_abc=C.link_abc(akt); 
    if (akt_abc!=NULL) TQ.append(Simpl_Triang(akt,akt_abc,C.vertex_a(akt),C.vertex_b(akt),C.vertex_c(akt)));
 }

 Simpl_Triang T;
 sc_simplex newsim;
 sc_simplex sa,sb; 

 
 forall(T,Border){ C.set_vert_sim_link(T.A,NULL); C.set_vert_sim_link(T.B,NULL); } 

 forall(T,Border){
  sa=C.get_simplex(T.A); sb=C.get_simplex(T.B);  
  newsim= C.add_simplex(T.A,T.B,PV); trhull.append(newsim);
  if (sa !=NULL){ //link
    C.set_link(newsim,sa,2);
    if (C.vertex_a(newsim) == C.vertex_b(sa)) C.set_link(sa,newsim,1); else C.set_link(sa,newsim,2);
  }
  if (sb !=NULL){ //link
    C.set_link(newsim,sb,1);
    if (C.vertex_b(newsim) == C.vertex_a(sb)) C.set_link(sb,newsim,2); else C.set_link(sb,newsim,1);
  }

  C.set_vert_sim_link(T.A,newsim); C.set_vert_sim_link(T.B,newsim);

  C.set_link(newsim,T.S1,3); 
  C.update_link(T.S1,T.S2,newsim);
  C.update_link(T.S2,T.S1,newsim);
  C.set_link(newsim,T.S2,4);
 }


 last= C.get_hulltriangle();
 return last;
} 

void delaunay_flip(d3_simpl_complex& C,queue<Simpl_Triang>& TQ,sc_vertex PV)
// change complex, so that all simplices in the complex get local-delauany property
{
 Simpl_Triang Trl;
 sc_simplex S1,S2;
 int wert;

 while (! TQ.empty()){ 
  Trl= TQ.pop();
  // still there ?
  S1=Trl.S1; S2=Trl.S2;
  if (S1!=NULL && S2!=NULL && C.complex_of(S1)==&C && C.complex_of(S2)==&C){

    if (C.flippable(S1,S2,Trl)){ // common triangle flippable ?
         wert=C.flip(S1,S2,TQ,Trl,PV);
    }

  }
 }

}


sc_simplex update_tr(d3_simpl_complex& C,d3_rat_point p,int counter,sc_simplex n,list<sc_simplex>& trhull, bool dtr)
{
 sc_simplex last;
 sc_vertex PV;
 list<sc_simplex> Trl;
 list<Simpl_Triang> Border;
 queue<Simpl_Triang> TQ;

 find_visible_triangles(C,Trl,Border,n,p,counter,trhull); 

 TQ.clear(); 
 trhull.clear();

 PV=C.add_vertex(p); 
 last=triangulate_new(C,Trl,Border,PV,TQ,trhull);
 
 if (dtr) delaunay_flip(C,TQ,PV);                      

 assert(C.get_hull_info(last));

 return last;
}

void compute_tr(list<d3_rat_point>& L, d3_simpl_complex& C,bool flipflag)
// flipflag true... Delaunay Triangulation, false..  Triangulation
{
 
 int counter=0;
 sc_simplex last;
 d3_rat_point lastp;
 list<sc_simplex> trhull; 

 last=comp_initial_tr(L,C,lastp,trhull); //compute start DT

 while (!L.empty()){
    counter++;
    d3_rat_point p = L.pop();
    last=update_tr(C,p,counter,last,trhull,flipflag);
 }
 
}

void D3_TRIANG(const list<d3_rat_point>& L0, d3_simpl_complex& C)
{
  if (L0.empty()) return;
  list<d3_rat_point> L = L0;

  L.sort(); L.unique();
  compute_tr(L,C,false);  
}
void D3_DELAU(const list<d3_rat_point>& L0, d3_simpl_complex& C)
{
  if (L0.empty()) return;
  list<d3_rat_point> L = L0;

  L.sort();
  L.unique();
  compute_tr(L,C,true);  
}

void to_vdnb(GRAPH<d3_rat_point,int>& H,node nd,GRAPH<d3_rat_point,int>& outgr)
// H[nd] and H[nd] neighbours to outgr ...
{
 outgr.clear();
 node md=outgr.new_node(H[nd]);
 node v,w;
 edge e1,e2;

 forall_adj_nodes(v,nd){
   w=outgr.new_node(H[v]);
   e1=outgr.new_edge(md,w); e2=outgr.new_edge(w,md);
   outgr.set_reversal(e1,e2);
 } 
}



void to_vorocell(d3_simpl_complex& C,sc_vertex vert,GRAPH<d3_rat_point,int>& outgr)
{ }






void D3_TRIANG(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& G)
{
  G.clear();
  d3_simpl_complex C;
  D3_TRIANG(L0,C);
  C.compute_graph(G);
}


void D3_DELAUNAY(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& G)
{
  G.clear();
  d3_simpl_complex C;
  D3_DELAU(L0,C);
  C.compute_graph(G);
}

void D3_VORONOI(const list<d3_rat_point>& L, GRAPH<d3_rat_sphere,int>& G)
{
  G.clear();
  d3_simpl_complex C;

  D3_DELAU(L,C);
  C.compute_voronoi(G);
}

void D3_VORONOI(const list<d3_rat_point>& L, GRAPH<d3_rat_point,int>& G, bool flag)
{
  G.clear();
  d3_simpl_complex C;

  D3_DELAU(L,C);
  C.compute_voronoi(G,flag);
}


LEDA_END_NAMESPACE

