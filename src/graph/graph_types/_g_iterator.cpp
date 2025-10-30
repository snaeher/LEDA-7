/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_iterator.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#if defined(__GNUC__) || defined(__SUNPRO_CC)

#include <LEDA/graph/graph_iterator.h>

void NodeIt::init(const NodeIt& p) { 
_G=p._G; update(p._n); }

void NodeIt::init(const graph& G) {
init(G,G.first_node()); }

void NodeIt::init(const graph& G, const node& v) {
_G=(graphtype*)(&G); update(v); }

void NodeIt::reset() { _n=_G->first_node(); }

void NodeIt::make_invalid() { _n=nil; }

void NodeIt::reset_end() { _n=_G->last_node(); }

void NodeIt::update(node n) { _n=n; }

void NodeIt::insert() { 
graphtype& Gp(*((graph*)(_G)));
_n=Gp.new_node(); }

void NodeIt::del() { 
graphtype& Gp(*((graph*)(_G)));
Gp.del_node(_n); _n=nil; }

NodeIt& NodeIt::operator= (const NodeIt& it2) { _n=it2._n;  _G=it2._G;  return *this; }

bool NodeIt::operator== (const NodeIt& it2) const { return _n==it2._n; }

const graph& NodeIt::get_graph() const { return *_G; }

bool NodeIt::valid() const { return _n!=nil; }

bool NodeIt::eol() const { return _n==nil; }

NodeIt& NodeIt::operator++ () { _n=_G->succ_node(_n); return *this; }

NodeIt NodeIt::operator++ (int) { 
NodeIt tmp(*this);
_n=_G->succ_node(_n);
return tmp; }

NodeIt& NodeIt::operator-- () {
_n=_G->pred_node(_n);
 return *this; }

NodeIt NodeIt::operator-- (int) {
NodeIt tmp(*this);
_n=_G->pred_node(_n);
return tmp; }





void EdgeIt::init(const graph& G) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  init(G,G.first_edge()); }

void EdgeIt::init(const graph& G, const edge& e) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  _G=(graphtype*)(&G); update(e); }

  void EdgeIt::update(edge  e)  { _e=e; }

  void EdgeIt::reset() { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
_e=_G->first_edge(); }

  void EdgeIt::reset_end() { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
_e=_G->last_edge(); }

  void EdgeIt::make_invalid() { _e=nil; }

  void EdgeIt::insert(node v1, node v2) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
    graphtype& Gp(*((graph*)(_G)));
    _e=Gp.new_edge(v1,v2); }

  void EdgeIt::del() {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(503,"Iintrator: del(), graph not initialized\n");
    #endif
    graphtype& Gp(*((graphtype*)(_G)));
        Gp.del_edge(_e); _e=nil; }

  void EdgeIt::init(const EdgeIt& p) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
    _G=p._G; update(p._e); }

 
 EdgeIt& EdgeIt::operator= (const EdgeIt& it2) { _e=it2._e;  _G=it2._G;  return *this; }

  bool EdgeIt::operator== (const EdgeIt& it2) const {
    return _e==it2._e; }

  const graph& EdgeIt::get_graph() const { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
return *_G; }

  EdgeIt& EdgeIt::operator++ () { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
        _e=_G->succ_edge(_e); 
      return *this; }

  EdgeIt EdgeIt::operator++ (int)  {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    EdgeIt tmp(*this);
         _e=_G->succ_edge(_e);
    return tmp; }

  EdgeIt& EdgeIt::operator-- () {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
        _e=_G->pred_edge(_e);
      return *this; }

  EdgeIt EdgeIt::operator-- (int)  {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    EdgeIt tmp(*this);
        _e=_G->pred_edge(_e);
    return tmp; }


  void OutAdjIt::init(const OutAdjIt& p) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
    _n=p._n; _e=p._e; _G=p._G; }

void OutAdjIt::init(const graph& G) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  init(G,G.first_node()); }

void OutAdjIt::init(const graph& G, const node& n) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  init(G,n,G.first_adj_edge(n)); }

void OutAdjIt::init(const graph& G,
          const node& n,
          const edge& e) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  _G=(graphtype*)(&G); update(n,e); }

  void OutAdjIt::update(edge  e) { _e=e; }

  void OutAdjIt::reset() { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
_e=_G->first_adj_edge(_n); }

  void OutAdjIt::insert(node v1, node v2) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
        _e=_G->new_edge(v1,v2); }
  void OutAdjIt::insert(node v) { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
_e=_G->new_edge(_n,v); }

  void OutAdjIt::insert(const OutAdjIt& other) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
    node v2(other.get_node());
    _e=_G->new_edge(_n,v2); }

  void OutAdjIt::del() {  
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(503,"Iintrator: del(), graph not initialized\n");
    #endif
_G->del_edge(_e); _e=nil;}

  void OutAdjIt::reset_end() {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _e=_G->last_adj_edge(_n); }

  void OutAdjIt::make_invalid() {
    _e=nil; }

  void OutAdjIt::update(node  n) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _n=n;
    _e=_G->first_adj_edge(_n); }

  void OutAdjIt::update(node  n,
    edge  e) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _n=n; _e=e; }

  OutAdjIt& OutAdjIt::operator= (const OutAdjIt& it2) { _n=it2._n; _e=it2._e;  _G=it2._G; 
    return *this; }

  bool OutAdjIt::operator== (const OutAdjIt& it2) const  {
    return _e==it2._e; }

  const graph& OutAdjIt::get_graph() const { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
return *_G; }

  OutAdjIt OutAdjIt::curr_adj() const {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    OutAdjIt intmp(*this);
    intmp._n=target(intmp._e); 
    intmp._e=_G->first_adj_edge(intmp._n); 
    return intmp; }

  OutAdjIt& OutAdjIt::operator++ () { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _e=_G->adj_succ(_e); 
    return *this; }

  OutAdjIt OutAdjIt::operator++ (int)  {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    OutAdjIt tmp(*this);
    _e=_G->adj_succ(_e);
    return tmp; } 

  OutAdjIt& OutAdjIt::operator-- () {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _e=_G->adj_pred(_e);
    return *this; }

  OutAdjIt OutAdjIt::operator-- (int)  {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    OutAdjIt tmp(*this);
    _e=_G->adj_pred(_e);
    return tmp; }

  void InAdjIt::init(const InAdjIt& p ) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
    _n=p._n; _e=p._e; _G=p._G; }

void InAdjIt::init(const graph& G) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  init(G,G.first_node()); }

void InAdjIt::init(const graph& G, const node& n) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  init(G,n,G.first_in_edge(n)); }


void InAdjIt::init(const graph& G,
          const node& n,
          const edge& e) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G!=nil) LEDA_EXCEPTION(501,"Iintrator: graph already initialized\n");
    #endif
  _G=(graphtype*)(&G); update(n,e); }


  void InAdjIt::update(edge  e) { _e=e; }

  void InAdjIt::reset() { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
_e=((graph&)(*_G)).first_in_edge(_n); }

  void InAdjIt::insert(node v1, node v2) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
        _e=((graph&)(*_G)).new_edge(v1,v2); }

  void InAdjIt::insert(const InAdjIt& other) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
    node v2(other.get_node());
    _e=((graph&)(*_G)).new_edge(_n,v2); }

  void InAdjIt::insert(node v) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(504,"Iintrator: insert(..), graph not initialized\n");
    #endif
        _e=((graph&)(*_G)).new_edge(_n,v); }

  void InAdjIt::del() {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(503,"Iintrator: del(), graph not initialized\n");
    #endif
    ((graph&)(*_G)).del_edge(_e); _e=nil; }

  void InAdjIt::reset_end() {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _e=((graph&)(*_G)).last_in_edge(_n); }

  void InAdjIt::make_invalid() {
    _e=nil; }

  void InAdjIt::update(node  n) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _n=n;
    _e=((graph&)(*_G)).first_in_edge(_n); }

  void InAdjIt::update(node  n,
    edge  e) {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _n=n; _e=e; }

  InAdjIt& InAdjIt::operator= (const InAdjIt& it2) { _n=it2._n; _e=it2._e;  _G=it2._G; 
    return *this; }

  bool InAdjIt::operator== (const InAdjIt& it2) const  {
    return _e==it2._e; }

  const graph& InAdjIt::get_graph() const { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
return *_G; }

  InAdjIt InAdjIt::curr_adj() const {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    InAdjIt intmp(*this);
    intmp._n=source(intmp._e); 
    intmp._e=((graph&)(*_G)).first_in_edge(intmp._n); 
    return intmp; }

  InAdjIt& InAdjIt::operator++ () { 
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _e=((graph&)(*_G)).in_succ(_e); 
    return *this; }

  InAdjIt InAdjIt::operator++ (int)  {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    InAdjIt tmp(*this);
    _e=((graph&)(*_G)).in_succ(_e);
    return tmp; } 

  InAdjIt& InAdjIt::operator-- () {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    _e=_G->in_pred(_e);
    return *this; }

  InAdjIt InAdjIt::operator-- (int)  {
    #if !defined(LEDA_CHECKING_OFF)
      if (_G==nil) LEDA_EXCEPTION(502,"Iintrator: graph not initialized\n");
    #endif
    InAdjIt tmp(*this);
    _e=_G->in_pred(_e);
    return tmp; }

 void AdjIt::init(const AdjIt& p) {
   oai.init(p.oai);
   iai.init(p.iai);
   flag=p.flag; }


void AdjIt::init(const graphtype& G) {
  oai.init(G); iai.init(G); flag=0; }

void AdjIt::init(const graphtype& G, const nodetype& n) {
   oai.init(G,n); iai.init(G,n); flag=0; }


void AdjIt::init(const graphtype& G,
          const nodetype& n,
          const edgetype& e) {
   oai.init(G,n,e); iai.init(G,n,e); flag=0; }


  void AdjIt::update(edge  e) { if (flag==0) oai.update(e); else iai.update(e); }

  void AdjIt::reset() { oai.reset(); iai.reset(); flag=0; }

  void AdjIt::insert(node v1, node v2) { if (flag==0) oai.insert(v1,v2); else iai.insert(v1,v2); }
  void AdjIt::insert(node v) { if (flag==0) oai.insert(v); else iai.insert(v); }

  void AdjIt::insert(const AdjIt& other) {
    if (flag==0) oai.insert(other.get_node()); else iai.insert(other.get_node()); }

  void AdjIt::del() {  if (flag==0) oai.del(); else iai.del(); }



  void AdjIt::reset_end() { if (flag==0) oai.reset_end(); else iai.reset_end(); }

  void AdjIt::make_invalid() {  oai.make_invalid();  iai.reset(); flag=0; }

  void AdjIt::update(node  n) { flag=0;
                    oai.update(n);  iai.update(n); 
                         }


  void AdjIt::update(node  n,
    edge  e) { if (flag==0) { iai.update(n); oai.update(n,e); } else { 
                iai.update(n,e); oai.update(n,nil); } }

  AdjIt& AdjIt::operator= (const AdjIt& it2) { oai=it2.oai; iai=it2.iai; flag=it2.flag; 
    return *this; }

  bool AdjIt::operator== (const AdjIt& it2) const  {
    if (flag!=it2.flag) return false;
    if (flag==0) return oai==it2.oai;
    return iai==it2.iai; }

  const graph& AdjIt::get_graph() const { return oai.get_graph(); }

  AdjIt AdjIt::curr_adj() const {  //===
    graph& Gp(*((graph*)(&oai.get_graph())));
    if (flag==0) {
      AdjIt intmp(Gp,oai.curr_adj().get_node()); 
      if (!intmp.valid()) intmp.flag=1;
      return intmp; 
    }
    AdjIt intmp(Gp,iai.curr_adj().get_node()); 
    if (!intmp.valid())  intmp.flag=1;
    return intmp; 
  }

  AdjIt& AdjIt::operator++ () { 
    if (flag==0) {
      edgetype intmp(oai.get_edge());
      ++oai;
      if (!oai.valid()) {
        flag=1;
        iai.update(oai.get_node());
        if (iai.get_edge()==intmp) iai++; 
      } 
      return *this; 
    }
    ++iai; 
    flag=1; 
    return *this; 
  }

  AdjIt AdjIt::operator++ (int)  {
    AdjIt tmp(*this);
    if (flag==0) {
      edgetype intmp(oai.get_edge());
      ++oai;
      if (!oai.valid()) {
        flag=1;
        iai.update(oai.get_node());
        if (iai.get_edge()==intmp) iai++;
      }
      return tmp;
    }
    ++iai;
    flag=1;
    return tmp; } 

  AdjIt& AdjIt::operator-- () {
    if (flag==0) {
        --oai;
        return *this; }
    --iai;
    flag=0;
    return *this; }

  AdjIt AdjIt::operator-- (int)  {
    AdjIt tmp(*this);
    if (flag==0) {
        --oai;
        return tmp; }
    --iai;
    flag=0; 
    return tmp; }

#endif
