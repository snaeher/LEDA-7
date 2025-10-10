#ifndef LEDA_D3_COMPLEX_H
#define LEDA_D3_COMPLEX_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 500871
#include <LEDA/internal/PREAMBLE.h>
#endif

// d3_complex.h
// Header-file for Simplexkomplex, Vertex, Simplex, Simpl_Triang

#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/geo/d3_hull.h>
#include <LEDA/core/list.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/queue.h>
#include <LEDA/core/set.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/graph.h>
#include <math.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/d3_window.h>
#include <LEDA/geo/d3_rat_plane.h>
#include <LEDA/numbers/rat_vector.h>
#include <LEDA/core/string.h>
#include <math.h>
#include <LEDA/geo/d3_rat_sphere.h>

LEDA_BEGIN_NAMESPACE

// because the header does not belong to LEDA
#undef __exportC
#undef __exportD
#undef __exportF
#define __exportC
#define __exportD
#define __exportF

class __exportC d3_simpl_complex;
class __exportC Simplex; 
class __exportC D3_vertex;

typedef D3_vertex* sc_vertex;
int compare(const sc_vertex&, const sc_vertex&);

typedef Simplex* sc_simplex;


class __exportC D3_vertex{
  // friend classes
 friend class __exportC d3_simpl_complex;
 friend class __exportC Simplex;
 friend class __exportC Simpl_Triang;

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

class __exportC Simplex { 
 // friend classes
 friend class __exportC d3_simpl_complex;
 friend class __exportC D3_vertex;
 friend class __exportC Simpl_Triang;

 // friend functions
 friend int compare (const Simplex&, const Simplex&);
 friend ostream& operator<<(ostream&, const Simplex&);
 friend istream& operator>>(istream&, Simplex&);

 friend istream& operator>>(istream& is, sc_simplex&) { return is; }

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



class __exportC Simpl_Triang{

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

class __exportC d3_simpl_complex{

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

  friend __exportF void DELAUNAY_FLIPPING(d3_simpl_complex&);
  friend __exportF sc_simplex comp_initial_tr(list<d3_rat_point>&,d3_simpl_complex&,d3_rat_point&,list<sc_simplex>&);
  friend __exportF void find_visible_triangles(d3_simpl_complex&,list<sc_simplex>&,list<Simpl_Triang>&,sc_simplex,d3_rat_point,int,list<sc_simplex>&);
  friend __exportF sc_simplex triangulate_new(d3_simpl_complex&,list<sc_simplex>&,list<Simpl_Triang>&,d3_rat_point,stack<Simpl_Triang>&,list<sc_simplex>&);

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

  list<sc_simplex> all_simplices()
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

  void compute_graph(GRAPH<d3_rat_point,int>& Gout,set<sc_vertex>& DS,list<edge>& erg);
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



extern __exportF bool in_sim(const d3_rat_point& p1,const d3_rat_point& p2,const d3_rat_point& p3,const d3_rat_point& cand);

extern __exportF bool test_inside(d3_rat_point a,d3_rat_point b,d3_rat_point c,d3_rat_point other);
// returns true, if the 3 d3_rat_point's a,b,c (they have to lie in one plane)
// form a circle containing other.




extern __exportF  int join_or_link(GRAPH<d3_rat_sphere,int>& Gout,node n1,node n2);
extern __exportF  int join(GRAPH<d3_rat_point,int>& Gout,node n1,node n2); 


class __exportC rpoint_zg { 
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



extern __exportF void DELAUNAY_FLIPPING(d3_simpl_complex& Comp, list<Simpl_Triang>& S);
extern __exportF void DELAUNAY_FLIPPING(d3_simpl_complex& Comp);
extern __exportF edge TRIANGULATE_POINTS(const list<rpoint_zg>& L0, GRAPH<rpoint_zg,sc_simplex>& G, d3_simpl_complex& C);
extern __exportF edge TRIANGULATE_POINTS(list<sc_vertex>& L0, GRAPH<rpoint_zg,sc_simplex>& G,d3_simpl_complex& C,int wert);
extern __exportF edge build_simp_comp(list<d3_rat_point>& L, d3_simpl_complex& C,int wert);


extern __exportF void calculate_triang(list<d3_rat_point>& L,d3_simpl_complex& Comp,int wert);

extern __exportF sc_simplex spec_case(list<d3_rat_point>& L,d3_simpl_complex& Comp,int fall,int wert);

extern __exportF int get_pro_plane(d3_rat_point A,d3_rat_point B,d3_rat_point C);

extern __exportF sc_simplex comp_initial_tr(list<d3_rat_point>& L,d3_simpl_complex& Comp,d3_rat_point& lastp,list<sc_simplex>& trhull);

extern __exportF int orientation(sc_vertex a,sc_vertex b,sc_vertex c,d3_rat_point d);

extern __exportF int test_vis(sc_simplex vis, sc_simplex other, sc_vertex c1, sc_vertex c2, d3_rat_point p,d3_rat_point pv);

extern __exportF sc_simplex really_vis(d3_simpl_complex& C,sc_simplex cand,d3_rat_point p,list<sc_simplex>& trhull);

extern __exportF void find_visible_triangles(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,sc_simplex n,d3_rat_point p,int counter,list<sc_simplex>& trhull);

extern __exportF void check_htr(list<sc_simplex>& trhull);

extern __exportF sc_simplex triangulate_new(d3_simpl_complex& C,list<sc_simplex>& Trl,list<Simpl_Triang>& Border,d3_rat_point p,stack<Simpl_Triang>& TS,list<sc_simplex>& trhull);

extern __exportF void delaunay_flip(d3_simpl_complex& C,stack<Simpl_Triang>& TS);

extern __exportF sc_simplex update_tr(d3_simpl_complex& C,d3_rat_point p,int counter,sc_simplex n,list<sc_simplex>& trhull, bool dtr);

extern __exportF void compute_tr(list<d3_rat_point>& L, d3_simpl_complex& C,bool flipflag);

extern __exportF void D3_TRIANG(const list<d3_rat_point>& L0, d3_simpl_complex& C);
/*{\Mfuncl  computes a triangulation of the points in |L0| and stores it in |C|.}*/

extern __exportF void D3_TRIANG(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& G);
/*{\Mfuncl  computes a triangulation of the points in |L0| and stores it in |G|.}*/

extern __exportF void D3_DELAU(const list<d3_rat_point>& L0, d3_simpl_complex& C);
/*{\Mfuncl  computes the delaunay triangulation of the points in |L0| and stores it in |C|.}*/

extern __exportF void D3_DELAU(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& G);
/*{\Mfuncl  computes the delaunay triangulation of the points in |L0| and stores it in |G|.}*/

extern __exportF void D3_VORONOI(const list<d3_rat_point>& L0, GRAPH<d3_rat_sphere,int>& G);
/*{\Mfuncl  computes the voronoi diagramm of the points in |L0| and stores it in |G|.}*/

extern __exportF void to_vdnb(GRAPH<d3_rat_point,int>& H,node nd,GRAPH<d3_rat_point,int>& outgr);

extern __exportF void to_vorocell(d3_simpl_complex& C,sc_vertex vert,GRAPH<d3_rat_point,int>& outgr);

#if LEDA_ROOT_INCL_ID == 500871
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

#endif 
 
