#include <math.h>

/*
    Copyright (c) Mark J. Kilgard, 1997.
    Modified by Marc Frincu, 2010.
*/

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

/* Simple 3D vector routines.  This file is meant to be included in
   a .c file, hence all the routines are defined statically.  A
   good C compiler should be able to automatically inline all
   these routines. */

static void vnorm ( double vec[3] )
{
  double len;

  len = sqrt ( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
  vec[0] = vec[0] / len;
  vec[1] = vec[1] / len;
  vec[2] = vec[2] / len;
}

static double vlen ( double vec[3] )
{
    return sqrt ( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
}

static double vdot ( double a[3], double b[3] )
{
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static void vdiff ( double dst[3], double a[3], double b[3] )
{
  dst[0] = a[0] - b[0];
  dst[1] = a[1] - b[1];
  dst[2] = a[2] - b[2];
}

static void vadd ( double dst[3], double a[3], double b[3] )
{
  dst[0] = a[0] + b[0];
  dst[1] = a[1] + b[1];
  dst[2] = a[2] + b[2];
}

static void vcopy ( double dst[3], double src[3] )
{
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
}

static void vscale ( double dst[3], double src[3], double scale )
{
  dst[0] = src[0] * scale;
  dst[1] = src[1] * scale;
  dst[2] = src[2] * scale;
}

static void vcross ( double cross[3], const double v1[3], const double v2[3] )
{
  double tmp[3];

  tmp[0] = ( v1[1] * v2[2]) - (v1[2] * v2[1] );
  tmp[1] = ( v1[2] * v2[0]) - (v1[0] * v2[2] );
  tmp[2] = ( v1[0] * v2[1]) - (v1[1] * v2[0] );
  vcopy ( cross, tmp );
}
