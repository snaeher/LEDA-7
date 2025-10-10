/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dic.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/
#include <LEDA/core/dictionary.h>
#include <LEDA/core/impl/ch_hash.h>
#include <LEDA/core/impl/avl_tree.h>
#include <LEDA/core/impl/avl_tree1.h>

using std::cout;
using std::cin;
using std::endl;

using namespace leda;

// inline int Hash(const string& s)  { return (s.length() > 0) ? s[0] : 0; }

struct xyz {

  int x,y,z;

  friend bool operator<(const xyz& a, const xyz& b) { 
     if (a.x < b.x) return true;
     if (a.x > b.x) return false;
     if (a.y < b.y) return true;
     if (a.y > b.y) return false;
     if (a.z < b.z) return true;
     if (a.z > b.z) return false;
     return false;
   }
  
};

//int compare(const xyz& p, const xyz& q) { return compare(p.x,q.x); }

int cmp(const xyz& p, const xyz& q) { return compare(p,q); }

int main()
{

 dictionary<xyz,int> D1;
 dictionary<xyz,int> D2(cmp);

/*
  std::function<int(const xyz&, const xyz&)>
     lambda = [D1](const xyz& x, const xyz& y) { return compare(x,y); };
*/

  std::function<int(xyz,xyz)>
     lambda = [D1](xyz x, xyz y) { return compare(x,y); };

  dictionary<xyz,int> DD(lambda);

  dictionary<string,int> D3;
  //dictionary<string,int>::item it;
  //dictionary<string,int,ch_hash> D([](string x, string y) { return x-y; });

  // auto
  std::function<int(const string&, const string&)>
    lambda1 = [](const string& x, const string& y) { return compare(x,y); };

  //int (*cmp)(const string&, const string&) = lambda1;

  typedef dictionary<string,int,avl_tree1> dic_type;

  //dic_type D;
  dic_type D(lambda1);
  dic_type::item it;
  dic_type::iterator dit;

  string s;

  while (cin >> s)
  { it = D.lookup(s);
    if (it==nil) D.insert(s,1);
    else D.change_inf(it,D.inf(it)+1);
   }

 for (dit = D.begin(); dit != D.end(); dit++)
    //cout << dit->first << " : " << dit->second << endl;
      cout << (*dit).first << " : " << (*dit).second << endl;
  cout << endl;

  forall_items(it,D) 
  {  cout <<  D.key(it) << " : " << D.inf(it) << "\n";
     D.del_item(it);
   }
  cout << endl;

  cout << "size after deletion: " << D.size() << endl;

  return 0;
}
