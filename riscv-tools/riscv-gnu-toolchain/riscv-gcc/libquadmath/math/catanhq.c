/* Return arc hyperbole tangent for __float128 value.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include "quadmath-imp.h"


__complex128
catanhq (__complex128 x)
{
  __complex128 res;
  int rcls = fpclassifyq (__real__ x);
  int icls = fpclassifyq (__imag__ x);

  if (rcls <= QUADFP_INFINITE || icls <= QUADFP_INFINITE)
    {
      if (icls == QUADFP_INFINITE)
	{
	  __real__ res = copysignq (0.0, __real__ x);
	  __imag__ res = copysignq (M_PI_2q, __imag__ x);
	}
      else if (rcls == QUADFP_INFINITE || rcls == QUADFP_ZERO)
	{
	  __real__ res = copysignq (0.0, __real__ x);
	  if (icls >= QUADFP_ZERO)
	    __imag__ res = copysignq (M_PI_2q, __imag__ x);
	  else
	    __imag__ res = nanq ("");
	}
      else
	{
	  __real__ res = nanq ("");
	  __imag__ res = nanq ("");
	}
    }
  else if (rcls == QUADFP_ZERO && icls == QUADFP_ZERO)
    {
      res = x;
    }
  else
    {
      __float128 i2, num, den;

      i2 = __imag__ x * __imag__ x;

      num = 1.0 + __real__ x;
      num = i2 + num * num;

      den = 1.0 - __real__ x;
      den = i2 + den * den;

      __real__ res = 0.25 * (logq (num) - logq (den));

      den = 1 - __real__ x * __real__ x - i2;

      __imag__ res = 0.5 * atan2q (2.0 * __imag__ x, den);
    }

  return res;
}
