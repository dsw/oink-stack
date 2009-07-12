// d2vector.h
// simple 2d points, lines, and vectors using doubles
// author: Scott McPeak, July 2001
// This file is hereby placed in the public domain.

#ifndef D2VECTOR_H
#define D2VECTOR_H

#ifdef __cplusplus
extern "C" {
#else
  #ifndef bool
    #define bool int
    #define true 1
    #define false 0
  #endif
#endif

// test if something is a special value (+inf/-inf/nan)
// (I'm assuming IEEE floats here!)
bool isSpecial(double value);

// single point in 2d space
typedef struct {
  double x;
  double y;
} D2Point;

// I use the same representation for vectors
typedef D2Point D2Vector;

// parameteric description of a line
typedef struct {
  D2Point origin;      // a single point on the line
  D2Vector vector;     // all points are origin + vector*t, -INF < t < +INF
} D2Line;


// length ("norm") of the vector
double lengthD2Vector(D2Vector const *v);

// simple reorientation of a vector by clockwise rotation
// of 90 degree increments (source and dest are same)
// (these are given specially because they avoid roundoff errors)
void rotD2Vector90(D2Vector *v);
void rotD2Vector180(D2Vector *v);
void rotD2Vector270(D2Vector *v);

// and arbitrary rotation (does *not* work when ret and v are aliased)
void rotD2VectorAngle(D2Vector *ret, D2Vector const *v, double theta);

// it is *legal* for the various pointers passed to these
// functions to be aliased (point to the same thing)
void addD2Points(D2Point *ret, D2Point const *p1, D2Point const *p2);
void subD2Points(D2Point *ret, D2Point const *p1, D2Point const *p2);
void scaleD2Vector(D2Vector *ret, D2Vector const *p, double scalar);

// change the length of a vector w/o changing its orientation
void scaleD2VectorTo(D2Vector *v, double finalLength);

// here, 'ret' can't be aliased with line's coordinates
void pointOnD2Line(D2Point *ret, D2Line const *line, double t);

// dot product = |a||b|cos(theta)
double dotProdD2Vector(D2Vector const *a, D2Vector const *b);

// project a point into a line, yielding the parameter 't' that
// denotes the point in the line
double projectD2PointLine(D2Point const *pt, D2Line const *line);

// return the z component of the cross product of v1 and v2,
// when considered to be vectors in the x-y plane; one way to
// interpret this: when it's positive, v2 is within 180 degrees
// counterclockwise of v1
//
// another identity: |a x b| = |a||b|sin(theta)
double crossProdZD2Vector(D2Vector const *a, D2Vector const *b);

// distance between a point and a line
double distanceD2PointLine(D2Point const *pt, D2Line const *line);

// true if either of v's components is not 0
bool nonzeroD2Vector(D2Vector const *v);

// given two lines in parametric form, return the value 't' such that
// qwline->origin + (qwline->vector * t) is the intersection point;
// if the lines are parallel, returns NAN
double intersectD2Lines(D2Line const *pvline, D2Line const *qwline);

void printD2Point(D2Point const *p);         // (x,y)
void printD2Line(D2Line const *line);        // from (x,y) along (v,w)

#ifdef __cplusplus
}
#endif

#endif // D2VECTOR_H
