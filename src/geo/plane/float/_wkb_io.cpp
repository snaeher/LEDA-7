/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _wkb_io.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/wkb_io.h>

#include <LEDA/geo/rat_gen_polygon.h>
#include <LEDA/numbers/fp.h>
#include <LEDA/system/file.h>
#include <assert.h>

LEDA_BEGIN_NAMESPACE

struct _wkbPoint {
  wkb_io::float64 x;
  wkb_io::float64 y;
};


wkb_io::wkb_io() : Input(0), Output(0)
{
  clear();
}


wkb_io::~wkb_io()
{
  clear();
}

 
bool wkb_io::read_from_buffer(int len, char* buf, gen_polygon& P)
{ 
  istringstream is(std::string(buf,buf+len), ios::binary);
  Input = &is;

  P = read_WKBPolygonGeometry();
  Input = 0;
  return true;
}
 
int wkb_io::write_to_buffer(char*& buf, const gen_polygon& P)
{ 
  ostringstream oss;
  Output = &oss;

  write_WKBPolygonGeometry(P);

  // int len = oss.str().length();

  // int len = oss.pcount(); // for non-std strstream (msvd 6.0) ???

  // compute number of written bytes

  int len = 0;
  len += sizeof(char);    // ByteOrder
  len += sizeof(uint32);  // Type
  len += sizeof(uint32);  // number of polygons
  polygon pol;
  forall_polygons(pol,P)
  { len += sizeof(uint32);  // number of vertices
    len += pol.size() * sizeof(_wkbPoint); // vertices
   }

  buf = new char[len];

  oss.str().copy(buf,len);
/*
  const char* p = oss.str().c_str();
  for(int i=0; i<len; i++) buf[i] = p[i];
*/

  Output = 0;
  return len;
}


bool wkb_io::read(const string& filename, gen_polygon& P)
{
  if (! is_file(filename)) {
    Input = 0;
    error("wkb_io::read: cannot open file " + filename);
    return false;
  }

  Input = new ifstream(filename,ios::binary);

  P = read_WKBPolygonGeometry();

  delete Input; 
        Input = 0;

  return true;
}

bool wkb_io::write(const string& filename, const gen_polygon& P)
{
  Output = new ofstream(filename, ios::binary);

  write_WKBPolygonGeometry(P);

  delete Output; 
        Output = 0;

  return true;
}


/// private members (global)
void wkb_io::clear()
{
  delete Input; Input = 0;
  delete Output; Output = 0;
}

void wkb_io::error(const string& msg)
{
  LEDA_EXCEPTION(1, msg);
}


/// private members (read)
void wkb_io::correct_byte_order_uint32(uint32& val)
{
  if (SwapBytes) {
    byte temp;

    byte* bytes = (byte*) &val;
    temp = bytes[0]; bytes[0] = bytes[3]; bytes[3] = temp;
    temp = bytes[1]; bytes[1] = bytes[2]; bytes[2] = temp;
  }
}

void wkb_io::correct_byte_order_float64(float64& val)
{
  if (SwapBytes) {
    byte temp;

    byte* bytes = (byte*) &val;
    temp = bytes[0]; bytes[0] = bytes[7]; bytes[7] = temp;
    temp = bytes[1]; bytes[1] = bytes[6]; bytes[6] = temp;
    temp = bytes[2]; bytes[2] = bytes[5]; bytes[5] = temp;
    temp = bytes[3]; bytes[3] = bytes[4]; bytes[4] = temp;
  }
}

wkb_io::wkbByteOrder wkb_io::read_wkbByteOrder()
{
  char order;

  Input->read( &order, sizeof(order) );
  if (Input->fail()) error("wkb_io::read_wkbByteOrder: read error");

  SwapBytes = (order != nativeByteOrder);

  return wkbByteOrder(order);
}

wkb_io::wkbGeometryType wkb_io::read_wkbType(wkbGeometryType expected_type)
{
  uint32 type=0;

  Input->read( (char*) &type, sizeof(type) );

cout << "type= " << type <<endl;

  if (Input->fail()) error("wkb_io::read_wkbType: read error");

  correct_byte_order_uint32(type);

  if ((int)type != expected_type && expected_type != wkbAny) {
    error("wkb_io::read_wkbType: wrong type!");
  }

  return wkbGeometryType(type);
}

point wkb_io::read_wkbPoint()
{
  struct _wkbPoint p;

  Input->read( (char*) &p, sizeof(p) );
  if (Input->fail()) error("wkb_io::read_wkbPoint: read error");

  correct_byte_order_float64(p.x);
  correct_byte_order_float64(p.y);

  return point(p.x, p.y);
}

polygon wkb_io::read_wkbLinearRing()
{
  uint32 count;

  Input->read( (char*) &count, sizeof(count) );
  if (Input->fail()) error("wkb_io::read_wkbLinearRing: read error");

  correct_byte_order_uint32(count);

  list<point> points;

  while (count-- != 0) {
    points.append( read_wkbPoint() );
  }

  return polygon(points, polygon::NO_CHECK, polygon::DISREGARD_ORIENTATION);
}

gen_polygon wkb_io::read_wkbPolygon()
{
  uint32 count;

  Input->read( (char*) &count, sizeof(count) );
  if (Input->fail()) error("wkb_io::read_wkbPolygon: read error");

  correct_byte_order_uint32(count);

  list<polygon> chains;

  for (uint32 i = 0; i < count; ++i) {
    polygon chain = read_wkbLinearRing();
    if (i > 0) chain = chain.complement();
    chains.append(chain);
  }

  return gen_polygon(chains, gen_polygon::NO_CHECK);
}

gen_polygon wkb_io::read_wkbMultiPolygon()
{
  uint32 count;

  Input->read( (char*) &count, sizeof(count) );
  if (Input->fail()) error("wkb_io::read_wkbMultiPolygon: read error");

  correct_byte_order_uint32(count);

  // we will use the rational kernel, because we do a union below
  list<rat_gen_polygon> polys;

  for (uint32 i = 0; i < count; ++i) {
    polys.append( read_WKBPolygon() );
  }

  rat_gen_polygon result = rat_gen_polygon::unite(polys);

  return result.to_float();
}

gen_polygon wkb_io::read_WKBPolygon()
{
  read_wkbByteOrder();
  read_wkbType(wkbPolygon);

  return read_wkbPolygon();
}

gen_polygon wkb_io::read_WKBMultiPolygon()
{
  read_wkbByteOrder();
  read_wkbType(wkbMultiPolygon);

  return read_wkbMultiPolygon();
}

gen_polygon wkb_io::read_WKBPolygonGeometry()
{
  read_wkbByteOrder();

  switch (read_wkbType()) {
  case wkbPolygon:    
    return read_wkbPolygon();

  case wkbMultiPolygon: 
    return read_wkbMultiPolygon();

  default:
    error("wkb_io::read_WKBPolygonGeometry: type not supported!");
    return gen_polygon();
  }
}


/// private members (write)
wkb_io::wkbByteOrder wkb_io::write_wkbByteOrder()
{
  char order = nativeByteOrder;

  Output->write( &order, sizeof(order) );
  if (Output->fail()) error("wkb_io::write_wkbByteOrder: write error");

  return wkbByteOrder(order);
}

void wkb_io::write_wkbType(wkbGeometryType type)
{
  uint32 _type = type;

  Output->write( (char*) &_type, sizeof(_type) );
  if (Output->fail()) error("wkb_io::write_wkbType: write error");
}

void wkb_io::write_wkbPoint(const point& pnt)
{
  struct _wkbPoint _pnt;

  _pnt.x = pnt.xcoord();
  _pnt.y = pnt.ycoord();

  Output->write( (char*) &_pnt, sizeof(_pnt) );
  if (Output->fail()) error("wkb_io::write_wkbPoint: write error");
}

void wkb_io::write_wkbLinearRing(const polygon& poly)
{
  uint32 count = (uint32) poly.size();

  Output->write( (char*) &count, sizeof(count) );
  if (Output->fail()) error("wkb_io::write_wkbLinearRing: write error");

  point pnt;
  forall_vertices(pnt, poly) {
    write_wkbPoint(pnt);
  }
}

void wkb_io::write_wkbPolygon(const gen_polygon& P)
{
  uint32 count = (uint32) P.polygons().size();

  Output->write( (char*) &count, sizeof(count) );
  if (Output->fail()) error("wkb_io::write_wkbPolygon: write error");

  polygon poly;
  forall_polygons(poly, P) {
    write_wkbLinearRing(poly);
  }
}

void wkb_io::write_wkbMultiPolygon(const list<gen_polygon>& L)
{
  uint32 count = (uint32) L.size();

  Output->write( (char*) &count, sizeof(count) );
  if (Output->fail()) error("wkb_io::write_wkbMultiPolygon: write error");

  gen_polygon P;
  forall(P, L) {
    write_WKBPolygon(P);
  }
}

void wkb_io::write_WKBPolygon(const gen_polygon& P)
{
  write_wkbByteOrder();
  write_wkbType(wkbPolygon);

  write_wkbPolygon(P);
}

void wkb_io::write_WKBMultiPolygon(const list<gen_polygon>& L)
{
  write_wkbByteOrder();
  write_wkbType(wkbMultiPolygon);

  write_wkbMultiPolygon(L);
}

void wkb_io::write_WKBPolygonGeometry(const gen_polygon& P)
{
  list<gen_polygon> L = P.regional_decomposition();

  if (L.size() == 1) {
    write_WKBPolygon(L.head());
  }
  else {
    write_WKBMultiPolygon(L);
  }
}



/*
wkbPoint {
  double x;
  double y;
};

wkbLinearRing {
  uint32 numPoints;
  wkbPoint points[numPoints];
}

WKBPoint {
  byte byteOrder;
  uint32 wkbType; // 1
  wkbPoint point;
}

WKBLineString {
  byte byteOrder;
  uint32 wkbType; // 2
  uint32 numPoints;
  wkbPoint points[numPoints];
}

WKBPolygon {
  byte byteOrder;
  uint32 wkbType; // 3
  uint32 numRings;
  wkbLinearRing rings[numRings];
}

WKBMultiPolygon {
  byte byteOrder;
  uint32 wkbType; // 6
  uint32 num_wkbPolygons;
  WKBPolygon wkbPolygons[num_wkbPolygons];
}
*/

LEDA_END_NAMESPACE
