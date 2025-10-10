#ifndef GW_HS_OBSERVER_H
#define GW_HS_OBSERVER_H

#include <LEDA/graphics/anim/TextLoader.h>

#include "Heapsort.h"
#include "ViewVariable.h"
#include "AnimationTree.h"


class gw_hs_observer {

  private:  

  event_item it[15];  
  
  // call by references
  //
  GraphWin& gw;
  array<node>& A_node;
  int *A_int;
  string& LanguagePath;
  int& ArraySize;

  TextLoader *TL_ptr_run;
  TextLoader *TL_ptr_source;
  ViewVariable *VV_ptr_state;
  AnimationTree *AT_ptr_tree;

  list<string> VariableName;
  list<string> VariableValue;
  
  bool Abort;
  bool BuildHeap;
  bool isZoom;  
  bool SemiAuto;
  
  int AnimationDelay;

  int actual_i;
  int actual_j;
  int actual_k;
  int actual_N;

  color col_k;
  color col_j;
  color col_marked;

  int GotoLine;
  int Compares;
  int Swaps;

  string goto_label;
  string delay_label;

  array<int> *ap;
  
  void show_active_line(int);
  void wait_of();
  void delay_panel();
  void update_status_window();

  void zoom_to_interval(node, node);
  void swap_nodes(const int, const int);

  int swap_nodes_start(int i, int j);
  void swap_nodes_step();
  void swap_nodes_finish();

  public:

  gw_hs_observer(alg_heapsort& HS, 
                 GraphWin& GW, 
                 array<node>& A_NODES,
                 int *A_INT, 
                 string& LANGUAGEPATH,
                 int& ARRAYSIZE)       :  gw(GW), 
                                          A_node(A_NODES), 
                                          A_int(A_INT),
                                          LanguagePath(LANGUAGEPATH),
                                          ArraySize(ARRAYSIZE)          
  {   
    it[0]  = attach(HS.start_event,  *this, &gw_hs_observer::on_start);
    it[1]  = attach(HS._1_event,     *this, &gw_hs_observer::on_1);
    it[2]  = attach(HS._2_event,     *this, &gw_hs_observer::on_2);    
    it[3]  = attach(HS._3_event,     *this, &gw_hs_observer::on_3);    
    it[4]  = attach(HS._4_event,     *this, &gw_hs_observer::on_4);
    it[5]  = attach(HS._5_event,     *this, &gw_hs_observer::on_5);    
    it[6]  = attach(HS._6_event,     *this, &gw_hs_observer::on_6);    
    it[7]  = attach(HS._7_event,     *this, &gw_hs_observer::on_7);
    it[8]  = attach(HS._8_event,     *this, &gw_hs_observer::on_8);    
    it[9]  = attach(HS._9_event,     *this, &gw_hs_observer::on_9);    
    it[10] = attach(HS._10_event,    *this, &gw_hs_observer::on_10);
    it[11] = attach(HS._11_event,    *this, &gw_hs_observer::on_11);    
    it[12] = attach(HS._12_event,    *this, &gw_hs_observer::on_12);    
    it[13] = attach(HS._13_event,    *this, &gw_hs_observer::on_13);
    it[14] = attach(HS.finish_event, *this, &gw_hs_observer::on_finish);
  
/*
    TL_ptr_run = new TextLoader(550, 180);
    TL_ptr_source = new TextLoader(350, 420);
    VV_ptr_state = new ViewVariable(350, 180);
    AT_ptr_tree = new AnimationTree(550, 200);
*/
    TL_ptr_run = new TextLoader(1200, 300);
    AT_ptr_tree = new AnimationTree(1200, 500);
    TL_ptr_source = new TextLoader(850, 1000);
    VV_ptr_state = new ViewVariable(850, 300);

  
    ap = new array<int>(1, ArraySize);
    for(int i = (*ap).low(); i <= ArraySize; i++)
      (*ap)[i] = A_int[i];

    AnimationDelay = 12;
    
    isZoom    = true;
    Abort     = false; 
    BuildHeap = true;
    SemiAuto  = false;
    
    Compares  = 0;
    Swaps     = 0;
    actual_i  = -1;
    actual_j  = -1;
    actual_k  = -1;
    actual_N  = -1;
  }

 ~gw_hs_observer() 
  { 
    delete TL_ptr_run;
    delete TL_ptr_source;
    delete VV_ptr_state;
    delete AT_ptr_tree;
    delete ap;
    detach(it, 15); 
  }

  void on_start(alg_heapsort&);
  void on_1 (alg_heapsort&, int);
  void on_2 (alg_heapsort&, int);
  void on_3 (alg_heapsort&, int, int);
  void on_4 (alg_heapsort&, int);
  void on_5 (alg_heapsort&, int, int);
  void on_6 (alg_heapsort&, int);
  void on_7 (alg_heapsort&, int, int);
  void on_8 (alg_heapsort&, int, int);
  void on_9 (alg_heapsort&, int, int);
  void on_10(alg_heapsort&, int, int);
  void on_11(alg_heapsort&, int);
  void on_12(alg_heapsort&, int);
  void on_13(alg_heapsort&, int);
  void on_finish(alg_heapsort&);
};


#endif
