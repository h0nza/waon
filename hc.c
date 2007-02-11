/* half-complex format routines
 * Copyright (C) 2007 Kengo Ichiki <kichiki@users.sourceforge.net>
 * $Id: hc.c,v 1.1 2007/02/09 05:56:08 kichiki Exp $
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <math.h>


/* return angle (arg) of the complex number (freq(k),freq(len-k));
 * where (real,imag) = (cos(angle), sin(angle)).
 * INPUT
 *  len        :
 *  freq [len] :
 *  conj       : set 0 for normal case.
 *               set 1 for conjugate of the complex (freq(k),freq(len-k))
 *               that is, for (freq(k),-freq(len-k)).
 * OUTPUT
 *  amp [len/2+1] :
 *  phs [len/2+1] :
 */
void HC_to_polar (long len, const double * freq,
		  int conj,
		  double * amp, double * phs)
{
  int i;
  double rl, im;

  phs [0] = 0.0;
  amp [0] = sqrt (freq [0] * freq [0]);
  for (i = 1; i < (len+1)/2; i ++)
    {
      rl = freq [i];
      im = freq [len - i];
      amp [i] = sqrt (rl * rl + im * im);
      if (amp [i] > 0.0) 
	{
	  if (conj == 0) phs [i] = atan2 (+im, rl);
	  else           phs [i] = atan2 (-im, rl);
	}
      else
	{
	  phs [i] = 0.0;
	}
    }
  if (len%2 == 0)
    {
      phs [len/2] = 0.0;
      amp [len/2] = sqrt (freq [len/2] * freq [len/2]);
    }
}

/* return angle (arg) of the complex number (freq(k),freq(len-k));
 * where (real,imag) = (cos(angle), sin(angle)).
 * INPUT
 *  len        :
 *  freq [len] :
 *  conj       : set 0 for normal case.
 *               set 1 for conjugate of the complex (freq(k),freq(len-k))
 *               that is, for (freq(k),-freq(len-k)).
 *  scale      : scale factor for amp2[]
 * OUTPUT
 *  amp2 [len/2+1] := (real^2 + imag^2) / scale
 *  phs  [len/2+1] := atan2 (+imag / real) for conj==0
 *                  = atan2 (-imag / real) for conj==1
 */
void HC_to_polar2 (long len, const double * freq,
		   int conj, double scale,
		   double * amp2, double * phs)
{
  int i;
  double rl, im;

  phs [0] = 0.0;
  amp2 [0] = freq [0] * freq [0] / scale;
  for (i = 1; i < (len+1)/2; i ++)
    {
      rl = freq [i];
      im = freq [len - i];
      amp2 [i] = (rl * rl + im * im)  / scale;
      if (amp2 [i] > 0.0) 
	{
	  if (conj == 0) phs [i] = atan2 (+im, rl);
	  else           phs [i] = atan2 (-im, rl);
	}
      else
	{
	  phs [i] = 0.0;
	}
    }
  if (len%2 == 0)
    {
      phs [len/2] = 0.0;
      amp2 [len/2] = freq [len/2] * freq [len/2] / scale;
    }
}

/* return power (amp2) of the complex number (freq(k),freq(len-k));
 * where (real,imag) = (cos(angle), sin(angle)).
 * INPUT
 *  len        :
 *  freq [len] :
 *  scale      : scale factor for amp2[]
 * OUTPUT
 *  amp2 [len/2+1] := (real^2 + imag^2) / scale
 */
void HC_to_amp2 (long len, const double * freq, double scale,
		 double * amp2)
{
  int i;
  double rl, im;

  amp2 [0] = freq [0] * freq [0] / scale;
  for (i = 1; i < (len+1)/2; i ++)
    {
      rl = freq [i];
      im = freq [len - i];
      amp2 [i] = (rl * rl + im * im)  / scale;
    }
  if (len%2 == 0)
    {
      amp2 [len/2] = freq [len/2] * freq [len/2] / scale;
    }
}

/* 
 * INPUT
 *  len           : N
 *  amp [len/2+1] :
 *  phs [len/2+1] :
 * OUTPUT
 *  freq [len] :
 * NOTE
 *  if len == even,
 *  (freq(0)     ... freq(N/2)) = (r(0)          ...      r(N/2)) and
 *  (freq(N/2+1) ... freq(N)  ) = (     i(N/2-1) ... i(1)       )
 *     where Fourier coefficient Y(k) = r(k) + i i(k).
 *     note that Y(k) = Y*(N-k) and Y(0) and Y(N/2) are real.
 *  if len == odd,
 *  (freq(0)     ... freq(N/2)) = (r(0)     ... r(N/2)) and
 *  (freq(N/2+1) ... freq(N)  ) = (i(N/2-1) ... i(1)  )
 *     note in this case that Y(0) is real but Y(N/2) is not.
 *  in either case, number of elements for the coefficients are N/2+1.
 */
void polar_to_HC (long len, const double * amp, const double * phs,
		  int conj,
		  double * freq)
{
  int i;
  double rl, im;

  /* calc phase and amplitude (power) */
  freq [0] = amp [0];
  for (i = 1; i < (len+1)/2; i ++)
    {
      rl = + amp [i] * cos (phs [i]);
      if (conj == 0) im = + amp [i] * sin (phs [i]);
      else           im = - amp [i] * sin (phs [i]);
      freq [i]       = rl;
      freq [len - i] = im;
    }
  if (len%2 == 0)
    {
      freq [len/2] = amp [len/2];
    }
}

/* Z = X * Y, that is,
 * (rz + i iz) = (rx + i ix) * (ry + i iy)
 *             = (rx * ry - ix * iy) + i (rx * iy + ix * ry)
 */
void HC_mul (long len, const double *x, const double *y,
	     double *z)
{
  int i;
  double rx, ix;
  double ry, iy;

  /* calc phase and amplitude (power) */
  z [0] = x [0] * y [0];
  for (i = 1; i < (len+1)/2; i ++)
    {
      rx = x [i];
      ix = x [len - i];
      ry = y [i];
      iy = y [len - i];
      z [i]       = rx * ry - ix * iy;
      z [len - i] = rx * iy + ix * ry;
    }
  if (len%2 == 0)
    {
      z [len/2] = x [len/2] * y [len/2];
    }
}

/* Z = X / Y, that is,
 * (rz + i iz) = (rx + i ix) / (ry + i iy)
 *             = (rx + i ix) * (ry - i iy) / (ry*ry + iy*iy)
 *             = (rx*ry + ix*iy + i (ix*ry - rx*iy)) / (ry*ry + iy*iy)
 */
void HC_div (long len, const double *x, const double *y,
	     double *z)
{
  int i;
  double rx, ix;
  double ry, iy;
  double den;

  /* calc phase and amplitude (power) */
  z [0] = x [0] / y [0];
  for (i = 1; i < (len+1)/2; i ++)
    {
      rx = x [i];
      ix = x [len - i];
      ry = y [i];
      iy = y [len - i];
      den = ry * ry + iy * iy;
      z [i]       = (rx * ry + ix * iy) / den;
      z [len - i] = (ix * ry - rx * iy) / den;
    }
  if (len%2 == 0)
    {
      z [len/2] = x [len/2] / y [len/2];
    }
}

void HC_abs (long len, const double *x,
	     double *z)
{
  int i;
  double rx, ix;

  /* calc phase and amplitude (power) */
  z [0] = fabs (x [0]);
  for (i = 1; i < (len+1)/2; i ++)
    {
      rx = x [i];
      ix = x [len - i];
      z [i]       = sqrt (rx * rx + ix * ix);
      z [len - i] = 0.0;
    }
  if (len%2 == 0)
    {
      z [len/2] = fabs (x [len/2]);
    }
}

void HC_puckette_lock (long len, const double *y,
		       double *z)
{
  int k;

  z [0] = y [0];
  for (k = 1; k < ((len+1)/2); k ++)
    {
      z [k]       = y [k];
      z [len - k] = y [len - k];
      if (k > 1)
	{
	  z [k-1]         += y [k-1];
	  z [len - (k-1)] += y [len - (k-1)];
	}
      if (k < (len+1)/2 - 1)
	{
	  z [k+1]         += y [k+1];
	  z [len - (k+1)] += y [len - (k+1)];
	}
    }
  if (len%2 == 0)
    {
      z [len/2] = y [len/2];
    }
}
