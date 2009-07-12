// d2vector.c
// some simple 2d vector code
//
// author: Scott McPeak, July 2001
// minor changes (renaming, etc.): 2005-12-31
//
// This file is hereby placed in the public domain.

#include "d2vector.h"   // this module

#include <assert.h>     // assert
#include <stdio.h>      // printf
#include <math.h>       // isnan, isinf, sin, cos, M_PI

double lengthD2Vector(D2Vector const *v)
{
  return sqrt(v->x * v->x + v->y * v->y);
}

void rotD2Vector90(D2Vector *v)
{
  double tmp = v->x;
  v->x = -(v->y);
  v->y = tmp;
}

void rotD2Vector180(D2Vector *v)
{
  v->x = -(v->x);
  v->y = -(v->y);
}

void rotD2Vector270(D2Vector *v)
{
  double tmp = v->x;
  v->x = v->y;
  v->y = -tmp;
}

void rotD2VectorAngle(D2Vector *ret, D2Vector const *v, double theta)
{
  double cos_t = cos(theta);
  double sin_t = sin(theta);
  assert(ret != v);
  ret->x = (v->x * cos_t) - (v->y * sin_t);
  ret->y = (v->x * sin_t) + (v->y * cos_t);
}


// this fn (and the below) are specifically written so it works
// to have p1, p2, and/or ret aliased
void addD2Points(D2Point *ret, D2Point const *p1, D2Point const *p2)
{
  ret->x = p1->x + p2->x;
  ret->y = p1->y + p2->y;
}

void subD2Points(D2Point *ret, D2Point const *p1, D2Point const *p2)
{
  ret->x = p1->x - p2->x;
  ret->y = p1->y - p2->y;
}

void scaleD2Vector(D2Vector *ret, D2Vector const *p, double scalar)
{
  ret->x = p->x * scalar;
  ret->y = p->y * scalar;
}

void scaleD2VectorTo(D2Vector *v, double finalLength)
{
  scaleD2Vector(v, v, finalLength / lengthD2Vector(v));
}

void pointOnD2Line(D2Point *ret, D2Line const *line, double t)
{
  assert(ret != &line->origin && ret != &line->vector);
  scaleD2Vector(ret, &line->vector, t);
  addD2Points(ret, ret, &line->origin);
}


double dotProdD2Vector(D2Vector const *a, D2Vector const *b)
{
  return a->x * b->x + a->y * b->y;
}

double projectD2PointLine(D2Point const *pt, D2Line const *line)
{
  double bLen, dot;

  // let a be the vector from line->origin to pt
  D2Vector a;
  subD2Points(&a, pt, &line->origin);

  // let b be line->vector
  bLen = lengthD2Vector(&line->vector);          // |b|

  dot = dotProdD2Vector(&a, &line->vector);      // |a||b|cos(theta)
  return dot/bLen;                               // |a|cos(theta)
}


double crossProdZD2Vector(D2Vector const *v1, D2Vector const *v2)
{
  return (v1->x * v2->y) - (v1->y * v2->x);
}

double distanceD2PointLine(D2Point const *pt, D2Line const *line)
{
  double bLen, cross;

  // let a be the vector from line->origin to pt
  D2Vector a;
  subD2Points(&a, pt, &line->origin);

  // let b be line->vector
  bLen = lengthD2Vector(&line->vector);          // |b|

  cross = crossProdZD2Vector(&a, &line->vector); // |a||b|sin(theta)
  return cross/bLen;                             // |a|sin(theta)
}


bool nonzeroD2Vector(D2Vector const *v)
{
  return v->x != 0 || v->y != 0;
}

double const d2_nan = 0.0/0.0;

bool isSpecial(double value)
{
  return isnan(value) || isinf(value);
}


double intersectD2Lines(D2Line const *pvline, D2Line const *qwline)
{
  // some convenient names
  D2Point const *p = &pvline->origin;
  D2Vector const *v = &pvline->vector;
  D2Point const *q = &qwline->origin;
  D2Vector const *w = &qwline->vector;
  double t;

  // sanity check
  assert(nonzeroD2Vector(v) && nonzeroD2Vector(w));

  // solve for t, the multiplier considered to apply to 'qwline',
  // at the intersection point (this formula was worked out on paper)
  t = ((p->x - q->x) + (v->x / v->y) * (q->y - p->y)) /
   // -----------------------------------------------
               (w->x - (v->x / v->y) * w->y) ;

  if (isSpecial(t)) {
    // the computation failed, either because 'v' is vertical, or
    // because the lines are parallel; so try to solve for 's'
    // instead, the multiplier considered to apply to 'pvline'
    // (same formula as above except p<->q and v<->w swapped)
    double s = ((q->x - p->x) + (w->x / w->y) * (p->y - q->y)) /
            // -----------------------------------------------
                      (v->x - (w->x / w->y) * v->y) ;

    if (isSpecial(s)) {    
      // the lines must be parallel
      return d2_nan;
    }

    // the point given by p + v*s is the intersection point; use
    // 's' to compute 't', the multiplier for 'line'
    t = (p->x + (v->x * s) - q->x) / w->x;

    if (isSpecial(t)) {
      // try the other formula, using the 'y' components
      t = (p->y + (v->y * s) - q->y) / w->y;

      if (isSpecial(t)) {
        // I don't know what would cause this (other than some
        // malformation of v or w that nonzeroD2Vector doesn't cover,
        // like nan)
        assert(!"Unexpected failure in intersection computation");
      }
    }
  }

  assert(!isSpecial(t));
  return t;
}


void printD2Point(D2Point const *p)
{
  printf("(%g,%g)", p->x, p->y);
}

void printD2Line(D2Line const *line)
{
  printf("from (%g,%g) along (%g,%g)",
         line->origin.x, line->origin.y,
         line->vector.x, line->vector.y);
}


// -------------------- test code --------------------
#ifdef TEST_D2VECTOR

void runIntersect(double px, double py, double vx, double vy,
                  double qx, double qy, double wx, double wy)
{
  D2Line L1, L2;
  double t;

  printf("computing intersection:\n");

  L1.origin.x = px;
  L1.origin.y = py;
  L1.vector.x = vx;
  L1.vector.y = vy;
  printf("  L1: "); printD2Line(&L1); printf("\n");

  L2.origin.x = qx;
  L2.origin.y = qy;
  L2.vector.x = wx;
  L2.vector.y = wy;
  printf("  L2: "); printD2Line(&L2); printf("\n");

  t = intersectD2Lines(&L1, &L2);
  if (isSpecial(t)) {
    printf("  these lines are parallel\n");
  }
  else {
    D2Point p;

    printf("  t is %g\n", t);

    // compute it as a point
    pointOnD2Line(&p, &L2, t);
    printf("  intersection is (%g,%g)\n", p.x, p.y);
  }
}

void rotTest()
{
  D2Vector v;
  int i;

  printf("\nrot test\n");

  v.x = 2;
  v.y = 1;
  printD2Point(&v);
  rotD2Vector90(&v);
  printD2Point(&v);
  rotD2Vector180(&v);
  printD2Point(&v);
  rotD2Vector270(&v);
  printD2Point(&v);
  printf("\n");

  v.x = 1;
  v.y = 0;
  printD2Point(&v);
  for (i=0; i<12; i++) {
    D2Vector tmp;
    if (i % 3 == 0) {
      printf("\n");
    }
    rotD2VectorAngle(&tmp, &v, 30.0 * M_PI / 180.0);   // 30 degrees
    v = tmp;
    printD2Point(&v);
  }
  printf("\n");
}

int main()
{
  // from (1,0) pointing up, intersected with
  // from (0,1) pointing right
  runIntersect(1,0, 0,1,  0,1, 1,0);
  runIntersect(0,1, 1,0,  1,0, 0,1);

  // vertical and diagonal
  runIntersect(1,0, 0,1,  0,0, 1,1);

  // vertical and diagonal
  runIntersect(1,0, 0,1,  0,0, 1,2);

  // vertical and diagonal
  runIntersect(2,0, 0,1,  0,0, 1,2);

  // parallel vertical
  runIntersect(1,0, 0,1,  0,0, 0,1);

  // parallel horizontal
  runIntersect(0,1, 1,0,  0,0, 1,0);

  // horizontal and diagonal
  runIntersect(0,1, 1,0,  0,10, 1,-0.1);

  // both diagonal, not parallel
  runIntersect(1,0, 1,2,  0,1, 2,1);

  // diagonal and parallel (but the vector isn't identical)
  runIntersect(1,0, 1,2,  0,1, 2,4);

  rotTest();

  return 0;
}

#endif // TEST_D2VECTOR
