#ifndef GW_QS_OBSERVER_H
#define GW_QS_OBSERVER_H

#include <LEDA/graphics/anim/TextLoader.h>

#include "Qsort.h"
#include "ViewVariable.h"


class gw_qs_observer {

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

  list<string> VariableName;
  list<string> VariableValue;
  
  bool Abort;
  bool firstQuicksortCall;
  bool SemiAuto;
  bool isZoom;  

  int AnimationDelay;

  int actual_i;
  int actual_j;
  int actual_r;
  int actual_l;
  int actual_v;

  int GotoLine;
  int Compares;
  int Swaps;

  string goto_label;
  string delay_label;
  
  void show_active_line(int);
  void wait_of();
  void delay_panel();
  void update_status_window();

  void zoom_to_interval(node, node);
  void swap_nodes(int, int);
    
  public:

  gw_qs_observer(alg_qsort& QS, 
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
    it[0]  = attach(QS.start_event,  *this, &gw_qs_observer::on_start);
    it[1]  = attach(QS._1_event,     *this, &gw_qs_observer::on_1);
    it[2]  = attach(QS._2_event,     *this, &gw_qs_observer::on_2);    
    it[3]  = attach(QS._3_event,     *this, &gw_qs_observer::on_3);    
    it[4]  = attach(QS._4_event,     *this, &gw_qs_observer::on_4);
    it[5]  = attach(QS._5_event,     *this, &gw_qs_observer::on_5);
    it[6]  = attach(QS._6_event,     *this, &gw_qs_observer::on_6);    
    it[7]  = attach(QS._7_event,     *this, &gw_qs_observer::on_7);    
    it[8]  = attach(QS._8_event,     *this, &gw_qs_observer::on_8);
    it[9]  = attach(QS._9_event,     *this, &gw_qs_observer::on_9);
    it[10] = attach(QS._10_event,    *this, &gw_qs_observer::on_10);    
    it[11] = attach(QS._11_event,    *this, &gw_qs_observer::on_11);    
    it[12] = attach(QS._12_event,    *this, &gw_qs_observer::on_12);
    it[13] = attach(QS._13_event,    *this, &gw_qs_observer::on_13);
    it[14] = attach(QS.finish_event, *this, &gw_qs_observer::on_finish);

/*
    TL_ptr_run = new TextLoader(585, 170);
    TL_ptr_source = new TextLoader(350, 370);
    VV_ptr_state = new ViewVariable(585, 120);
*/

    TL_ptr_run = new TextLoader(1000, 300);
    TL_ptr_source = new TextLoader(700, 700);
    VV_ptr_state = new ViewVariable(1000, 250);
    
    AnimationDelay = 10;
    
    isZoom    = true;
    Abort     = false; 
    SemiAuto  = false;

    Compares  = 0;
    Swaps     = 0;
    actual_i  = -1;
    actual_j  = -1;
    actual_l  = -1;
    actual_r  = -1;
    actual_v  = -1;
  } 


  ~gw_qs_observer() 
  { 
    delete TL_ptr_run;
    delete TL_ptr_source;
    delete VV_ptr_state;
    detach(it, 15); 
  }

  void on_start(alg_qsort&);
  void on_1(alg_qsort&, int, int);
  void on_2(alg_qsort&, int, int);
  void on_3(alg_qsort&, int);
  void on_4(alg_qsort&, int);
  void on_5(alg_qsort&, int);
  void on_6(alg_qsort&);
  void on_7(alg_qsort&, int, int);
  void on_8(alg_qsort&, int, int);
  void on_9(alg_qsort&, int, int);
  void on_10(alg_qsort&, int, int);
  void on_11(alg_qsort&, int, int);
  void on_12(alg_qsort&, int, int);
  void on_13(alg_qsort&, int, int);
  void on_finish(alg_qsort&);
};


#endif
