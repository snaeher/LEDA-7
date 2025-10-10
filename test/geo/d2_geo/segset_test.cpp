/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  segset_test.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_segment_set.h>
#include <LEDA/geo/segment_set.h>

#include <LEDA/core/random_source.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


template <class SegSet>
class Test {
public:
	typedef typename SegSet::segment_type Segment;
	typedef typename SegSet::line_type    Line;

public:
	void run();

	void reproduce_error(int seed, int num_segs, int query);

private:
	static void abort();

	static void check(bool cond, const char* cond_str, const char* file, int line);
	static bool check_intersection_raw(const SegSet& S, const Segment& q, bool abort_on_error = true);
	static bool check_intersection_raw(const SegSet& S, const Line& q, bool abort_on_error = true);
	static bool check_intersection(const SegSet& S, const Segment& q, bool abort_on_error = true);
	static bool check_intersection(const SegSet& S, const Line& q, bool abort_on_error = true);

	void run_test_all_operations();
	void run_test_item_operations();
	void run_test_case1();
	void run_test_case2();
	void run_test_random(int rounds = 5, int num_segs = 1000, int num_queries = 100);
	void run_test_random_round(int num_segs, int num_queries);

	static void    random_init(int min = -200, int max = 200);
	static Segment random_horizontal_segment();
	static Segment random_vertical_segment();

private:
	static string        FailureMsg;
	static int           Seed;
	static random_source Rand;
};


#define CHECK(COND) check((COND), #COND, __FILE__, __LINE__)

template <class SegSet>
int Test<SegSet>::Seed = 0;
template <class SegSet>
string Test<SegSet>::FailureMsg;
template <class SegSet>
random_source Test<SegSet>::Rand;

template <class SegSet>
void Test<SegSet>::run()
{
	run_test_all_operations();
	run_test_item_operations();
	run_test_case1();
	run_test_case2();
	run_test_random();
	cout << endl;
}

template <class SegSet>
void Test<SegSet>::abort()
{
	cout << endl << "FAILURE (random seed = " << Seed << ")";
	if (FailureMsg != "") cout << ": " << FailureMsg;
	cout << endl;
	exit(1);
}

template <class SegSet>
void Test<SegSet>::check(bool cond, const char* cond_str, const char* file, int line)
{
	if (! cond) {
		cout << "check '" << cond_str << "' failed in file " << file << " line " << line << endl;
		abort();
	}
}

template <class SegSet>
bool Test<SegSet>::check_intersection_raw(const SegSet& S, const Segment& q, bool abort_on_error)
{
	list<seg_item> inter = S.intersection(q);
	seg_item it;
	forall(it, inter) {
		Segment seg = S.key(it);
		if (!q.intersection(seg)) {
			cout << "(seg) q=" << q.to_float() << " does not intersect " << seg.to_float() << endl;
			if (abort_on_error) abort();
			return false;
		}
	}

	int num_inter = 0;
	forall_seg_items(it, S) {
		Segment seg = S.key(it);
		if (q.intersection(seg)) ++num_inter;
	}
	if (num_inter != inter.size()) {
		cout << "(seg) num_inter = " << num_inter << ", but inter.size() == " << inter.size() << endl;
		if (abort_on_error) abort();
		return false;
	}

	return true;
}

template <class SegSet>
bool Test<SegSet>::check_intersection_raw(const SegSet& S, const Line& q, bool abort_on_error)
{
	list<seg_item> inter = S.intersection(q);
	seg_item it;
	forall(it, inter) {
		Segment seg = S.key(it);
		if (!q.intersection(seg)) {
			cout << "(line) q=" << q.to_float() << " does not intersect " << seg.to_float() << endl;
			if (abort_on_error) abort();
			return false;
		}
	}

	int num_inter = 0;
	forall_seg_items(it, S) {
		Segment seg = S.key(it);
		if (q.intersection(seg)) ++num_inter;
	}
	if (num_inter != inter.size()) {
		cout << "(line) num_inter = " << num_inter << ", but inter.size() == " << inter.size() << endl;
		if (abort_on_error) abort();
		return false;
	}

	return true;
}

template <class SegSet>
bool Test<SegSet>::check_intersection(const SegSet& S, const Segment& q, bool abort_on_error)
{
	if (! check_intersection_raw(S, q, abort_on_error)) return false;
	if (! check_intersection_raw(S, q.reverse(), abort_on_error)) return false;
	if (!q.is_trivial() && !check_intersection(S, Line(q), abort_on_error)) return false;
	return true;
}

template <class SegSet>
bool Test<SegSet>::check_intersection(const SegSet& S, const Line& q, bool abort_on_error)
{
	if (! check_intersection_raw(S, q, abort_on_error)) return false;
	if (! check_intersection_raw(S, q.reverse(), abort_on_error)) return false;
	return true;
}

template <class SegSet>
void Test<SegSet>::run_test_all_operations()
{
	cout << "Simple test to check all operations ..." << endl;

	SegSet S(1); // orientation PI/2, i.e. vertical segments
	CHECK( S.empty() );
	CHECK( S.size() == 0 );

	Segment s1( 110, -291,  110, 181);
	Segment s2(-370, -291, -370, 181);

	S.insert(s1, 1);
	CHECK( !S.empty() );
	CHECK( S.size() == 1 );

	S.insert(s2, 2);
	CHECK( !S.empty() );
	CHECK( S.size() == 2 );

	seg_item it = S.lookup(s1);
	CHECK( it != nil );
	CHECK( S.key(it) == s1 );
	CHECK( S.inf(it) == 1 );

	S.change_inf(it, -1);
	CHECK( S.inf(it) == -1 );

	int cnt = 0;
	forall_seg_items(it, S) ++cnt;
	CHECK( cnt == S.size() );

	Segment q1( -100, 0,  100, 0);
	list<seg_item> inter1 = S.intersection(q1);
	CHECK( inter1.empty() );
	check_intersection(S, q1);

	Segment q2( -100, 10,  110, 10);
	list<seg_item> inter2 = S.intersection(q2);
	CHECK( inter2.size() == 1 );
	CHECK( S.key(inter2.head()) == s1 );
	check_intersection(S, q2);

	Segment q3( -370, 181,  110, 181);
	list<seg_item> inter3 = S.intersection(q3);
	CHECK( inter3.size() == 2 );
	check_intersection(S, q3);

	Segment q4( -370, -291,  110, -291);
	list<seg_item> inter4 = S.intersection(q4);
	CHECK( inter4.size() == 2 );
	check_intersection(S, q4);

	Segment q5( -400, -10,  120, -10);
	list<seg_item> inter5 = S.intersection(q5);
	CHECK( inter5.size() == 2 );
	check_intersection(S, q5);

	Line q6(q1);
	check_intersection(S, q6);

	S.del(s1);
	CHECK( !S.empty() );
	CHECK( S.size() == 1 );

	S.del(s2);
	CHECK( S.empty() );
	CHECK( S.size() == 0 );

	S.insert(s1, 1);
	CHECK( !S.empty() );
	CHECK( S.size() == 1 );

	S.insert(s2, 2);
	CHECK( !S.empty() );
	CHECK( S.size() == 2 );

	it = S.lookup(s1);
	CHECK( it != nil );
	CHECK( S.key(it) == s1 );
	CHECK( S.inf(it) == 1 );

	S.del_item(it);
	CHECK( !S.empty() );
	CHECK( S.size() == 1 );

	S.clear();
	CHECK( S.empty() );
	CHECK( S.size() == 0 );
}

template <class SegSet>
void Test<SegSet>::run_test_item_operations()
{
	cout << "Test item related operations ..." << endl;

	Segment s1(   -1, -500,  821, -500);
	Segment s2(  499,    0, 1291,    0);
	Segment s3( 1499,    0, 2291,    0);
	Segment  q(  500,    0,  500, -640);

	list<seg_item> items;

	SegSet S;
	items.append( S.insert(s1,1) );
	items.append( S.insert(s2,2) );
	items.append( S.insert(s3,3) );

	int count = 0;
	list<seg_item> items2 = S.all_items();
	seg_item it;
	forall(it, items2) {
		++count;
		CHECK( items.search(it) != nil );
	}
	CHECK( count == items.size() );

	count = 0;
	forall_seg_items(it, S) {
		++count;
		CHECK( items.search(it) != nil );
	}
	CHECK( count == items.size() );

	list<seg_item> query_items = S.intersection(q);
	check_intersection(S, q);
	forall(it, query_items) {
		CHECK( items.search(it) != nil );
	}

	forall(it, query_items) {
		CHECK( S.lookup(S.key(it)) == it );

		Segment seg = S.key(it);
		S.del(seg); items.remove(it);
		CHECK( S.lookup(seg) == nil );

		seg_item it2;
		count = 0;
		forall_seg_items(it2, S) {
			++count;
			CHECK( items.search(it2) != nil );
		}
		CHECK( count == items.size() );
	}
}

template <class SegSet>
void Test<SegSet>::run_test_case1()
{
	cout << "Test case 1 ..." << endl;

	SegSet S;
	Segment t0(   0, 300, 2500, 300);
	Segment t1(2500, 600, 5000, 600);
	Segment t2(   0, 900, 2500, 900);
	Segment  q(2500,   0, 2500, 5000);

	S.insert(t0, 0);
	S.insert(t1, 1);
	S.insert(t2, 2);

	check_intersection(S, q);
}

template <class SegSet>
void Test<SegSet>::run_test_case2()
{
	cout << "Test case 2 ..." << endl;

	SegSet S(1);
	Segment t0(320,   19, 320, 501);
	Segment t1(100, -321, 100, 111);
	Segment  q(421,  110,  99, 110);

	S.insert(t0, 0);
	S.insert(t1, 1);

	check_intersection(S, q);
}

template <class SegSet>
void Test<SegSet>::run_test_random(int rounds, int num_segs, int num_queries)
{
	cout << "Tests with random segments (#segs = " << num_segs 
		 << ", #queries = " << num_queries << ") ..." << endl;

	for (int r = 1; r != rounds; ++r)
		run_test_random_round(num_segs, num_queries);
}

template <class SegSet>
void Test<SegSet>::run_test_random_round(int num_segs, int num_queries)
{
	random_init();

	SegSet S; // horizontal segments

	int i;
	for (i = 1; i <= num_segs; ++i) {
		S.insert( random_horizontal_segment(), i );
	}

	for (i = 1; i <= num_queries; ++i) {
		FailureMsg = string("query segment #%d", i);
		Segment q = random_vertical_segment();
		check_intersection(S, q);
	}
	FailureMsg = string();
}

template <class SegSet>
void Test<SegSet>::reproduce_error(int seed, int num_segs, int query)
{
	cout << "Reproducing error from random test ..." << endl;

	cout << "  seed = " << seed << endl;
	cout << "  num_segs = " << num_segs << endl;
	cout << "  failed query = " << query << endl;
	cout << endl;

	random_init();
	Rand.set_seed(Seed = seed);

	SegSet S; // horizontal segments

	int i;
	for (i = 1; i <= num_segs; ++i) {
		Segment seg = random_horizontal_segment();
		S.insert(seg , i);
		if (num_segs < 10) cout << i << " : " << seg.to_float() << endl;
	}

	for (i = 1; i < query; ++i) {
		random_vertical_segment();
	}

	Segment q = random_vertical_segment();
	cout << "q = " << q.to_float() << endl;
	//check_intersection_raw(S, q);
	check_intersection(S, q);
}

template <class SegSet>
void Test<SegSet>::random_init(int min, int max)
{
	Seed = Rand.reinit_seed();
	Rand.set_range(min, max);
}

template <class SegSet>
typename Test<SegSet>::Segment Test<SegSet>::random_horizontal_segment()
{
	int x0, x1, y;
	Rand >> x0;
	Rand >> x1;
	Rand >> y;
	return Segment(x0, y, x1, y);
}

template <class SegSet>
typename Test<SegSet>::Segment Test<SegSet>::random_vertical_segment()
{
	int x, y0, y1;
	Rand >> x;
	Rand >> y0;
	Rand >> y1;
	return Segment(x, y0, x, y1);
}


int main(int argc, char* argv[])
{
	if (option_list == 0) option_list = (char*)"show_memory_leaks";

	Test< segment_set<int> > double_test;
	double_test.run();

	Test< rat_segment_set<int> > rat_test;
	rat_test.run();

	return 0;
}

