/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#include <ode/common.h>
#include <ode/odemath.h>

// get some math functions under windows
#ifdef WIN32
#include <float.h>
#ifndef CYGWIN			// added by andy for cygwin
#undef copysign
#define copysign(a,b) ((dReal)_copysign(a,b))
#endif				// added by andy for cygwin
#endif

#undef dSafeNormalize3
#undef dSafeNormalize4
#undef dNormalize3
#undef dNormalize4


// this may be called for vectors `a' with extremely small magnitude, for
// example the result of a cross product on two nearly perpendicular vectors.
// we must be robust to these small vectors. to prevent numerical error,
// first find the component a[i] with the largest magnitude and then scale
// all the components by 1/a[i]. then we can compute the length of `a' and
// scale the components by 1/l. this has been verified to work with vectors
// containing the smallest representable numbers.

int _dSafeNormalize3 (dVector3 a)
{
	dReal a0, a1, a2, aa0, aa1, aa2, l;
	dAASSERT(a);
	a0 = a[0];
	a1 = a[1];
	a2 = a[2];
	aa0 = dFabs(a0);
	aa1 = dFabs(a1);
	aa2 = dFabs(a2);
	if (aa1 > aa0) 
	{
		if (aa2 > aa1) 
			goto aa2_largest;
		else 
		{	// aa1 is largest
			a0 /= aa1;
			a2 /= aa1;
			l = dRecipSqrt(a0 * a0 + a2 * a2 + 1);
			a[0] = a0 * l;
			a[1] = (dReal)_copysignf(l, a1);
			a[2] = a2 * l;
		}
	}
	else 
	{
		if (aa2 > aa0) 
		{
		aa2_largest:	// aa2 is largest
			a0 /= aa2;
			a1 /= aa2;
			l = dRecipSqrt(a0 * a0 + a1 * a1 + 1);
			a[0] = a0 * l;
			a[1] = a1 * l;
			a[2] = (dReal)_copysignf(l, a2);
		}
		else 
		{		// aa0 is largest
			if (aa0 <= 0) 
			{
				// dDEBUGMSG ("vector has zero size"); ... this messace is annoying
				a[0] = 1;	// if all a's are zero, this is where we'll end up.
				a[1] = 0;	// return a default unit length vector.
				a[2] = 0;
				return 0;
			}
			a1 /= aa0;
			a2 /= aa0;
			l = dRecipSqrt(a1 * a1 + a2 * a2 + 1);
			a[0] = (dReal)_copysignf(l, a0);
			a[1] = a1 * l;
			a[2] = a2 * l;
		}
	}
  
  return 1;
}

/* OLD VERSION */
/*
void dNormalize3 (dVector3 a)
{
  dIASSERT (a);
  dReal l = dDOT(a,a);
  if (l > 0) {
    l = dRecipSqrt(l);
    a[0] *= l;
    a[1] *= l;
    a[2] *= l;
  }
  else {
    a[0] = 1;
    a[1] = 0;
    a[2] = 0;
  }
}
*/

int  dSafeNormalize3 (dVector3 a)
{
	return _dSafeNormalize3(a);
}

void dNormalize3(dVector3 a)
{
	_dNormalize3(a);
}


int _dSafeNormalize4 (dVector4 a)
{
  dAASSERT (a);
  dReal l = dDOT(a,a)+a[3]*a[3];
  if (l > 0) 
  {
    l = dRecipSqrt(l);
    a[0] *= l;
    a[1] *= l;
    a[2] *= l;
    a[3] *= l;
	return 1;
  }
  else 
  {
    dDEBUGMSG("vector has zero size");
    a[0] = 1;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    return 0;
  }
}

int  dSafeNormalize4 (dVector4 a)
{
	return _dSafeNormalize4(a);
}

void dNormalize4(dVector4 a)
{
	_dNormalize4(a);
}


void dPlaneSpace (const dVector3 n, dVector3 p, dVector3 q)
{
  dAASSERT (n && p && q);
  if (dFabs(n[2]) > M_SQRT1_2) {
    // choose p in y-z plane
    dReal a = n[1]*n[1] + n[2]*n[2];
    dReal k = dRecipSqrt (a);
    p[0] = 0;
    p[1] = -n[2]*k;
    p[2] = n[1]*k;
    // set q = n x p
    q[0] = a*k;
    q[1] = -n[0]*p[2];
    q[2] = n[0]*p[1];
  }
  else {
    // choose p in x-y plane
    dReal a = n[0]*n[0] + n[1]*n[1];
    dReal k = dRecipSqrt (a);
    p[0] = -n[1]*k;
    p[1] = n[0]*k;
    p[2] = 0;
    // set q = n x p
    q[0] = -n[2]*p[1];
    q[1] = n[2]*p[0];
    q[2] = a*k;
  }
}


/*
* This takes what is supposed to be a rotation matrix,
* and make sure it is correct.
* Note: this operates on rows, not columns, because for rotations
* both ways give equivalent results.
*/
void dOrthogonalizeR(dMatrix3 m)
{
	dReal n0 = dLENGTHSQUARED(m);
	if (n0 != 1)
		dSafeNormalize3(m);

	// project row[0] on row[1], should be zero
	dReal proj = dDOT(m, m+4);
	if (proj != 0) {
		// Gram-Schmidt step on row[1]
		m[4] -= proj * m[0];
		m[5] -= proj * m[1];
		m[6] -= proj * m[2];
	}
	dReal n1 = dLENGTHSQUARED(m+4);
	if (n1 != 1)
		dSafeNormalize3(m+4);

	/* just overwrite row[2], this makes sure the matrix is not
	a reflection */
	dCROSS(m+8, =, m, m+4);
	m[3] = m[4+3] = m[8+3] = 0;
}
