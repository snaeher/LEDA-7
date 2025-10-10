#include "Qsort.h"


class test_qs_observer {
  
  event_item it[15];
  
  public:

  test_qs_observer(alg_qsort& p) 
  {   
    it[0]  = attach(p.start_event,  *this, &test_qs_observer::on_start);
    it[1]  = attach(p._1_event,     *this, &test_qs_observer::on_1);
    it[2]  = attach(p._2_event,     *this, &test_qs_observer::on_2);    
    it[3]  = attach(p._3_event,     *this, &test_qs_observer::on_3);    
    it[4]  = attach(p._4_event,     *this, &test_qs_observer::on_4);
    it[5]  = attach(p._5_event,     *this, &test_qs_observer::on_5);
    it[6]  = attach(p._6_event,     *this, &test_qs_observer::on_6);    
    it[7]  = attach(p._7_event,     *this, &test_qs_observer::on_7);    
    it[8]  = attach(p._8_event,     *this, &test_qs_observer::on_8);
    it[9]  = attach(p._9_event,     *this, &test_qs_observer::on_9);
    it[10] = attach(p._10_event,    *this, &test_qs_observer::on_10);    
    it[11] = attach(p._11_event,    *this, &test_qs_observer::on_11);    
    it[12] = attach(p._12_event,    *this, &test_qs_observer::on_12);
    it[13] = attach(p._13_event,    *this, &test_qs_observer::on_13);
    it[14] = attach(p.finish_event, *this, &test_qs_observer::on_finish);
  }

 ~test_qs_observer() { detach(it,15); }

  void on_start(alg_qsort&) {};
  void on_1(alg_qsort&, int, int){}
  void on_2(alg_qsort&, int, int){}
  void on_3(alg_qsort&, int){}
  void on_4(alg_qsort&, int){}
  void on_5(alg_qsort&, int){}
  void on_6(alg_qsort&){}
  void on_7(alg_qsort&, int, int){}
  void on_8(alg_qsort&, int, int){}
  void on_9(alg_qsort&, int, int){}
  void on_10(alg_qsort&, int, int){}
  void on_11(alg_qsort&, int, int){}
  void on_12(alg_qsort&, int, int){}
  void on_13(alg_qsort&, int, int){}
  void on_finish(alg_qsort&){}
};

