/* Version: MPL 1.1/LGPL 3.0
 *
 * "The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is the Oblivion Graphics Extender, short OBGE.
 *
 * The Initial Developer of the Original Code is
 * Ethatron <niels@paradice-insight.us>. Portions created by The Initial
 * Developer are Copyright (C) 2011 The Initial Developer.
 * All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Library General Public License Version 3 license (the
 * "LGPL License"), in which case the provisions of LGPL License are
 * applicable instead of those above. If you wish to allow use of your
 * version of this file only under the terms of the LGPL License and not
 * to allow others to use your version of this file under the MPL,
 * indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL License.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under either the MPL or the LGPL License."
 */

#define	_CRT_SECURE_NO_WARNINGS
#define	_CRT_NONSTDC_NO_DEPRECATE

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <omp.h>
#include <intrin.h>

#include <d3d9.h>
#include <d3dx9.h>

const char *findFormat(D3DFORMAT fmt);
short findFormatDepth(D3DFORMAT fmt);
char findFormatChannels(D3DFORMAT fmt);
bool findAlpha(D3DFORMAT fmt);

#include "../globals.h"
#include "texture-squish.h"

#define	USESSE2
#ifndef	USESSE2
/* ------------------------------------------------------------------- */
#define a16

#define qfloor(x)	floor(x)
#define qsign(x)	((x) >= 0 ? 1.0f : -1.0f)
#define qhalf(x)	((x) >= 0 ? 0.5f : -0.5f)
#define qabs(x)		((x) >= 0 ? (x) : -(x))
#define qmin(a,b)	min(a,b)
#define qmax(a,b)	max(a,b)
#define qsqrt(l)	(1.0f * sqrtf(l))
#define rsqrt(l)	(1.0f / sqrtf(l))

template<const long range>
static __forceinline long rint(float n) {
  /* lattice quantizer */
  return (long)qmax(qmin(qfloor(n + 0.5f), (float)range), 0.0f);

//if (n >= 0.0f)
//  return (long)qmax(floor(n + 0.5f), 255);
//if (n <= 0.0f)
//  return (long)qmin( ceil(n - 0.5f),   0);
}

template<const long range>
static __forceinline long sint(float n) {
  const float r = (range) >> 1;

  /* dead-zone quantizer */
  if (n > r)
    return (long)(    qmin(qfloor(    n + 0.5f), (float)range    ));
//if (n <= 128.0f)
    return (long)(r - qmin(qfloor(r - n + 0.5f), (float)range - r));
}
#else
/* ------------------------------------------------------------------- */
#define a16		__declspec(align(16))

static __forceinline long qabs(long l        ) { return abs(l   ); }
static __forceinline long qmin(long m, long n) { return min(m, n); }
static __forceinline long qmax(long m, long n) { return max(m, n); }
static __forceinline int  qmin(int  m, int  n) { return min(m, n); }
static __forceinline int  qmax(int  m, int  n) { return max(m, n); }

static __forceinline float qabs(float l) {
  __m128 x = _mm_set_ss(l);
  __m128 a = _mm_castsi128_ps(_mm_set1_epi32(~0x80000000));
  __m128 s = _mm_and_ps(x, a);

  return s.m128_f32[0];
}

static __forceinline float qsign(float l) {
  __m128 x = _mm_set_ss(l);
  __m128 a = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
  __m128 s = _mm_and_ps(x, a);

  return s.m128_f32[0];
}

static __forceinline float qhalf(float l) {
  __m128 x = _mm_set_ss(l);
  __m128 a = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
  __m128 b = _mm_set_ss(0.5f);
  __m128 s = _mm_or_ps(_mm_and_ps(x, a), b);

  return s.m128_f32[0];
}

static __forceinline long qfloor(float l) {
  __m128  x = _mm_set_ss(l);
  __m128i t = _mm_cvttps_epi32(x);
  __m128i a = _mm_set1_epi32(0x80000000);
          a = _mm_and_si128(a, _mm_castps_si128(x));
          a = _mm_srli_epi32(a, 31);
  __m128i s = _mm_sub_epi32(t, a);

  return s.m128i_u32[0];
}

static __forceinline float qtrunc(float l) {
  __m128 x = _mm_set_ss(l);
  __m128 s = _mm_cvtepi32_ps(_mm_cvttps_epi32(x));

  return s.m128_f32[0];
}

static __forceinline float qmin(float m, long n) {
  __m128 x = _mm_set_ss(m);
  __m128 y; y = _mm_cvt_si2ss(y, n);
  __m128 s = _mm_min_ss(x, y);

  return s.m128_f32[0];
}

static __forceinline float qmax(float m, long n) {
  __m128 x = _mm_set_ss(m);
  __m128 y; y = _mm_cvt_si2ss(y, n);
  __m128 s = _mm_max_ss(x, y);

  return s.m128_f32[0];
}

static __forceinline float qmin(float m, float n) {
  __m128 x = _mm_set_ss(m);
  __m128 y = _mm_set_ss(n);
  __m128 s = _mm_min_ss(x, y);

  return s.m128_f32[0];
}

static __forceinline float qmax(float m, float n) {
  __m128 x = _mm_set_ss(m);
  __m128 y = _mm_set_ss(n);
  __m128 s = _mm_max_ss(x, y);

  return s.m128_f32[0];
}

static __forceinline float qsqrt(float l) {
  __m128 x = _mm_set_ss(l);
  __m128 s = _mm_sqrt_ss(x);

  return s.m128_f32[0];
}

static __forceinline float rsqrt(float l) {
  __m128 x = _mm_set_ss(l);
  __m128 s = _mm_rsqrt_ss(x);

  // Newton-Raphson step
  __m128 t = _mm_set_ss(3.0f);
  __m128 h = _mm_set_ss(0.5f);
  __m128 n;

  n = _mm_mul_ss(s, s);	  // s*s
  n = _mm_mul_ss(n, x);	  // s*s*l
  t = _mm_sub_ss(t, n);	  // 3.0f - s*s*l
  t = _mm_mul_ss(t, h);	  // (3.0f - s*s*l) * 0.5f
  s = _mm_mul_ss(s, t);	  // (3.0f - s*s*l) * 0.5f * s

  return s.m128_f32[0];
}

/* checkput: we may get away with one iteration here */
static __forceinline float rsqrt(long l) {
  __m128 x = _mm_set_ss((float)l);
  __m128 s = _mm_rsqrt_ss(x);

  // Newton-Raphson step
  __m128 t = _mm_set_ss(3.0f);
  __m128 h = _mm_set_ss(0.5f);
  __m128 n;

  n = _mm_mul_ss(s, s);	  // s*s
  n = _mm_mul_ss(n, x);	  // s*s*l
  t = _mm_sub_ss(t, n);	  // 3.0f - s*s*l
  t = _mm_mul_ss(t, h);	  // (3.0f - s*s*l) * 0.5f
  s = _mm_mul_ss(s, t);	  // (3.0f - s*s*l) * 0.5f * s

  return s.m128_f32[0];
}

template<const long range>
static __forceinline long rint(float n) {
  /* lattice quantizer */
  return (long)qmax(qmin(qtrunc(n + 0.5f), (float)range), 0.0f);
}

template<const long range>
static __forceinline long sint(float n) {
  /* lattice quantizer */
  return (long)qmax(qmin(qtrunc(n + 0.5f), (float)range), 0.0f);
}
#endif

/* -AWSOME: --------------------------------------------------------------------------------------
 * http://aras-p.info/texts/CompactNormalStorage.html
 */

#define	MIPMAP_MINIMUM		1	// 4

/* make normals occupy the full[0,255] color-cube */
#define	NORMALS_CUBESPACE

/* http://www.gamedev.net/topic/539608-mip-mapping-normal-maps/page__whichpage__1%25EF%25BF%25BD */
#define	NORMALS_SCALEBYLEVEL  normalsteepness

/* different ways of encoding the normals */
#undef	NORMALS_INTEGER
#undef	NORMALS_FLOAT_XYZ
#undef	NORMALS_FLOAT_XYZ_TANGENTSPACE
#undef	NORMALS_FLOAT_XY_TANGENTSPACE
#define	NORMALS_FLOAT_DXDY_TANGENTSPACE	0.5f

/* http://diaryofagraphicsprogrammer.blogspot.com/2009/01/partial-derivative-normal-maps.html
 *
 * The idea is to store the paritial derivate of the normal in two channels of the map like this
 *
 * dx = (-nx/nz);
 * dy = (-ny/nz);
 *
 * Then you can reconstruct the normal like this:
 *
 * nx = -dx;
 * ny = -dy;
 * nz = 1;
 * normalize(n);
 *
 * The advantage is that you do not have to reconstruct Z, so you can skip one instruction in
 * each pixel shader that uses normal maps.
 */
#undef	NORMALS_FLOAT_AZ_TANGENTSPACE

/* http://www.gamedev.net/topic/535230-storing-normals-as-spherical-coordinates/
 *
 * Encode:
 *  return (float2(atan2(nrmNorm.y, nrmNorm.x) / M_PI, nrmNorm.z) + 1.0f) * 0.5f;
 *
 * Decode:
 *  float2 spGPUAngles = spherical.xy * 2.0f - 1.0f;
 *  float2 sincosTheta; sincos(spGPUAngles.x * M_PI, sincosTheta.x, sincosTheta.y);
 *  float2 sincosPhi = float2(sqrt(1.0f - spGPUAngles.y * spGPUAngles.y), spGPUAngles.y);
 *
 * return float3(sincosTheta.y * sincosPhi.x, sincosTheta.x * sincosPhi.x, sincosPhi.y);
 *
 * Storing z instead of acos(z) just saves some ops the decoding, you still need sin(phi) and cos(phi) to reconstruct XY. You just happen to already have cos(acos(z)), and the trig identity: '1 - sin(x)^2 = cos(x)^2' does the rest.
 *
 * Edit:
 *  Didn't answer the question I guess. You are seeing odd results because the conversion back from spherical is missing a step. You are computing sincos(theta) but not sincos(phi). The reason why I say store just normal.z and not acos(normal.z) is because the length of the vector is 1.0f (did I mention this method of encode/decode only works on normalized vectors) and so doing acos(normal.z/1) and then recovering cos(acos(normal.z/1)) is a very silly thing to do. Instead I store normal.z, and then compute sin(phi) by using the law of sines.
 */

#define ACCUMODE_LINEAR		( 0 << 0)	// RGBH
#define ACCUMODE_GAMMA		( 1 << 0)	// RGBH
#define ACCUMODE_FLAT		( 0 << 0)	// XYZD
#define ACCUMODE_SCALE		( 1 << 0)	// XYZD

#define NORMMODE_LINEAR		( 0 << 0)	// RGBH
#define NORMMODE_GAMMA		( 1 << 0)	// RGBH

#define TRGTNORM_CUBESPACE	( 1 << 0)	// XYZD
#define TRGTMODE_CODING		(15 << 1)
#define TRGTMODE_CODING_RGB	( 0 << 1)
#define TRGTMODE_CODING_XYZt	( 1 << 1)
#define TRGTMODE_CODING_XYt	( 2 << 1)
#define TRGTMODE_CODING_DXDYt	( 3 << 1)	// fixed Z
#define TRGTMODE_CODING_DXDYdZt ( 4 << 1)	// 4x4 adaptive Z
#define TRGTMODE_CODING_DXDYDZt ( 5 << 1)	// 1x1 adaptive Z
#define TRGTMODE_CODING_AZt	( 6 << 1)
#define TRGTMODE_CODING_XYZ	( 7 << 1)
#define TRGTMODE_CODING_XY	( 8 << 1)

/* ####################################################################################
 */

template<const int mode>
static __forceinline void AccuRGBH(long *bs, ULONG b, int level, int l) {
  /* separate the channels and build the sum */
#if 0
  __m128i *x = (__m128i *)bs;
  __m128i up = _mm_setzero_si128();
  __m128i pu = _mm_set_epi32(b,b,b,b);

  pu = _mm_unpacklo_epi8 (pu, up);
  pu = _mm_unpacklo_epi16(pu, up);
  pu = _mm_shuffle_epi32 (pu, _MM_SHUFFLE(0,1,2,3));
  *x = _mm_add_epi32     (*x, pu);
#else
  bs[0] += (b >> 24) & 0xFF; /*h*/
  bs[1] += (b >> 16) & 0xFF; /*b*/
  bs[2] += (b >>  8) & 0xFF; /*g*/
  bs[3] += (b >>  0) & 0xFF; /*r*/
#endif
}

template<const int mode>
static __forceinline void AccuRGBM(long *bs, ULONG b, int level, int l) {
  /* separate the channels and build the sum */
#if 0
  __m128i *x = (__m128i *)bs;
  __m128i up = _mm_setzero_si128();
  __m128i pu = _mm_set_epi32(b,b,b,b);
  __m128i mk = _mm_set_epi32(-1,0,0,0);
  __m128i mx;

  pu = _mm_unpacklo_epi8 (pu, up);
  pu = _mm_unpacklo_epi16(pu, up);
  pu = _mm_shuffle_epi32 (pu, _MM_SHUFFLE(0,1,2,3));
  mx = _mm_sub_epi16     (pu, *x);
  mx = _mm_max_epi16     (mx, up);
  mx = _mm_and_si128     (mk, mx);
  pu = _mm_andnot_si128  (mk, pu);
  pu = _mm_or_si128      (mx, pu);
  *x = _mm_add_epi32     (*x, pu);
#else
  bs[0]  = qmax(bs[0], (long)
	   (b >> 24) & 0xFF); /*h*/
  bs[1] += (b >> 16) & 0xFF ; /*b*/
  bs[2] += (b >>  8) & 0xFF ; /*g*/
  bs[3] += (b >>  0) & 0xFF ; /*r*/
#endif
}

template<const int mode>
static __forceinline void AccuRGBH(float *bs, ULONG b, int level, int l) {
  const float rnrm = 1.0f / 0xFF;

  /* separate the channels and build the sum */
#if 0
  // x^2.2
  //  4th order: 0.162*x^3 +0.881*x^2 -0.043*x
  //  5th order: -0.043*x^4 +0.256*x^3 +0.819*x^2 -0.032*x
  //  6th order: 0.13*x^5 -0.417*x^4 +0.641*x^3 +0.654*x^2 -0.008*x	good one.
  //  rational 2/3: (0.145829 + 0.599076 (-(1/2) + x) + 0.616016 (-(1/2) + x)^2)/(0.670216 - 0.19632 (-(1/2) + x) + 0.144 (-(1/2) + x)^2 - 0.0704 (-(1/2) + x)^3)
  // lowest error: 6th order

  mov xx, x
  mov xxx, x

  mul x, const1
  mul xx, xx
  mov xxxxx, const5
  mul xxx, xx
  mov xxxx, xx
  mul xxxxx, xx

  mul xx, const2
  mul xxxx, xxxx
  mul xxxxx, xxx
  mul xxxx, const4
  mul xxx, const3

  xx = x*x
  xxx = xx*x
  xxxxx = xxx*xx
  xxxx = xx*xx

  // x^1/2.2
  //  4th order: 1.46*x^3 -3.188*x^2 +2.728*x
  //  5th order: -2.773*x^4 +7.211*x^3 -6.883*x^2 +3.445*x
  //  6th order: 5.859*x^5 -17.917*x^4 +21.036*x^3 -12.096*x^2 +4.117*x
  //  rational 3/4: (0.0307983 + 23.8774 x + 1048.01 x^2 + 3957.7 x^3)/(1 + 180.089 x + 2641.17 x^2 + 2621.95 x^3 - 416.455 x^4)
  // lowest error: rational 3/4

  mov xx, x
  mov xxx, x

  mul x, const1
  mul xx, xx
  mul xxx, xx
  mov xxxx, xx

  mul xx, const2
  mul xxxx, xxxx
  mul xxxx, const4
  mul xxx, const3

#else
  bs[0] +=             ((b >> 24) & 0xFF)              ; /*h*/
  bs[1] += powf((float)((b >> 16) & 0xFF) * rnrm, 2.2f); /*b*/
  bs[2] += powf((float)((b >>  8) & 0xFF) * rnrm, 2.2f); /*g*/
  bs[3] += powf((float)((b >>  0) & 0xFF) * rnrm, 2.2f); /*r*/
#endif
}

template<const int mode>
static __forceinline void AccuRGBM(float *bs, ULONG b, int level, int l) {
  const float rnrm = 1.0f / 0xFF;

  /* separate the channels and build the sum */
  bs[0]  = qmax(bs[0],
                (long )((b >> 24) & 0xFF)             ); /*h*/
  bs[1] += powf((float)((b >> 16) & 0xFF) * rnrm, 2.2f); /*b*/
  bs[2] += powf((float)((b >>  8) & 0xFF) * rnrm, 2.2f); /*g*/
  bs[3] += powf((float)((b >>  0) & 0xFF) * rnrm, 2.2f); /*r*/
}

template<const int mode>
static __forceinline void AccuXYZD(long *ns, ULONG n, int level, int l) {
  long vec[4];

  vec[0] = ((n >> 24) & 0xFF) - 0x00; /*d[ 0,1]*/
  vec[1] = ((n >> 16) & 0xFF) - 0x80; /*z[-1,1]*/
  vec[2] = ((n >>  8) & 0xFF) - 0x80; /*y[-1,1]*/
  vec[3] = ((n >>  0) & 0xFF) - 0x80; /*x[-1,1]*/

  if (mode & ACCUMODE_SCALE) {
    /* lower z (heighten the virtual displacement) every level */
    vec[1] *= (level * NORMALS_SCALEBYLEVEL) - l;
    vec[1] /= (level * NORMALS_SCALEBYLEVEL)    ;
  }

  ns[0] += vec[0];
  ns[1] += vec[1];
  ns[2] += vec[2];
  ns[3] += vec[3];
}

template<const int mode>
static __forceinline void AccuXYZD(float *nn, ULONG n, int level, int l) {
  const float rnrm = 1.0f / 0xFF;
  float vec[4], len;

  vec[0] = (float)((n >> 24) & 0xFF);
  vec[1] = (float)((n >> 16) & 0xFF); vec[1] *= rnrm; vec[1] -= 0.5f; vec[1] /= 0.5f;
  vec[2] = (float)((n >>  8) & 0xFF); vec[2] *= rnrm; vec[2] -= 0.5f; vec[2] /= 0.5f;
  vec[3] = (float)((n >>  0) & 0xFF); vec[3] *= rnrm; vec[3] -= 0.5f; vec[3] /= 0.5f;

  if (mode & ACCUMODE_SCALE) {
    /* lower z (heighten the virtual displacement) every level */
    vec[1] *=        (level * NORMALS_SCALEBYLEVEL) - l;
    vec[1] *= 1.0f / (level * NORMALS_SCALEBYLEVEL)    ;
  }

  /* prevent singularity */
  len = vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3];
  if (!len)
    len = vec[1] = 1.0f, vec[2] = vec[3] = 0.0f;
  else
    len = rsqrt(len);

  vec[1] *= len;
  vec[2] *= len;
  vec[3] *= len;

  nn[0] += vec[0];
  nn[1] += vec[1];
  nn[2] += vec[2];
  nn[3] += vec[3];
}

template<const int mode>
static __forceinline void AccuXYCD(long *nn, ULONG n, int level, int l) {
  abort();
}

template<const int mode>
static __forceinline void AccuXYCD(float *nn, ULONG n, int level, int l) {
  const float rnrm = 1.0f / 0xFF;
  float vec[5], len;

  vec[0] = (float)((n >> 24) & 0xFF);
  vec[1] = (float)((n >> 16) & 0xFF);
  vec[2] = (float)((n >>  8) & 0xFF); vec[2] *= rnrm; vec[2] -= 0.5f; vec[2] /= 0.5f;
  vec[3] = (float)((n >>  0) & 0xFF); vec[3] *= rnrm; vec[3] -= 0.5f; vec[3] /= 0.5f;
  vec[4] = qsqrt(1.0f - qmin(1.0f, vec[2] * vec[2] + vec[3] * vec[3]));

  if (mode & ACCUMODE_SCALE) {
    /* lower z (heighten the virtual displacement) every level */
    vec[4] *=        (level * NORMALS_SCALEBYLEVEL) - l;
    vec[4] *= 1.0f / (level * NORMALS_SCALEBYLEVEL)    ;
  }

  /* prevent singularity */
  len = vec[4] * vec[4] + vec[2] * vec[2] + vec[3] * vec[3];
  if (!len)
    len = vec[4] = 1.0f, vec[2] = vec[3] = 0.0f;
  else
    len = rsqrt(len);

  vec[2] *= len;
  vec[3] *= len;
  vec[4] *= len;

  nn[0] += vec[0];
  nn[1] += vec[1];
  nn[2] += vec[2];
  nn[3] += vec[3];
  nn[4] += vec[4];
}

/* ####################################################################################
 */

template<const int mode>
static __forceinline void ReduceRGBH(long *bs, long *bt) {
  /* separate the channels and build the sum */
  bs[0] += bt[0]; bt[0] = 0; /*h*/
  bs[1] += bt[1]; bt[1] = 0; /*b*/
  bs[2] += bt[2]; bt[2] = 0; /*g*/
  bs[3] += bt[3]; bt[3] = 0; /*r*/
}

template<const int mode>
static __forceinline void ReduceRGBM(long *bs, long *bt) {
  /* separate the channels and build the sum */
  bs[0]  = qmax(bs[0], bt[0]); bt[0] = 0; /*h*/
  bs[1] +=             bt[1] ; bt[1] = 0; /*b*/
  bs[2] +=             bt[2] ; bt[2] = 0; /*g*/
  bs[3] +=             bt[3] ; bt[3] = 0; /*r*/
}

template<const int mode>
static __forceinline void ReduceRGBH(float *bs, float *bt) {
  /* separate the channels and build the sum */
  bs[0] += bt[0]; bt[0] = 0; /*h*/
  bs[1] += bt[1]; bt[1] = 0; /*b*/
  bs[2] += bt[2]; bt[2] = 0; /*g*/
  bs[3] += bt[3]; bt[3] = 0; /*r*/
}

template<const int mode>
static __forceinline void ReduceRGBM(float *bs, float *bt) {
  /* separate the channels and build the sum */
  bs[0]  = qmax(bs[0], bt[0]); bt[0] = 0; /*h*/
  bs[1] +=             bt[1] ; bt[1] = 0; /*b*/
  bs[2] +=             bt[2] ; bt[2] = 0; /*g*/
  bs[3] +=             bt[3] ; bt[3] = 0; /*r*/
}

template<const int mode>
static __forceinline void ReduceXYZD(long *ns, long *nt) {
  ns[0] += nt[0]; nt[0] = 0; /*d[ 0,1]*/
  ns[1] += nt[1]; nt[1] = 0; /*z[-1,1]*/
  ns[2] += nt[2]; nt[2] = 0; /*y[-1,1]*/
  ns[3] += nt[3]; nt[3] = 0; /*x[-1,1]*/
}

template<const int mode>
static __forceinline void ReduceXYZD(float *nn, float *nt) {
  nn[0] += nt[0]; nt[0] = 0; /*d[ 0,1]*/
  nn[1] += nt[1]; nt[1] = 0; /*z[-1,1]*/
  nn[2] += nt[2]; nt[2] = 0; /*y[-1,1]*/
  nn[3] += nt[3]; nt[3] = 0; /*x[-1,1]*/
}

template<const int mode>
static __forceinline void ReduceXYCD(long *nn, long *nt) {
  abort();
}

template<const int mode>
static __forceinline void ReduceXYCD(float *nn, float *nt) {
  nn[0] += nt[0]; nt[0] = 0; /*c[ 0,1]*/
  nn[1] += nt[1]; nt[1] = 0; /*d[ 0,1]*/
  nn[2] += nt[2]; nt[2] = 0; /*z[-1,1]*/
  nn[3] += nt[3]; nt[3] = 0; /*y[-1,1]*/
  nn[4] += nt[4]; nt[4] = 0; /*x[-1,1]*/
}

/* ####################################################################################
 */

template<const int mode>
static __forceinline void NormRGBH(long *obs, long *bs, int av) {
  /* build average of each channel an join */
  obs[0] = bs[0] / av; /*h*/
  obs[1] = bs[1] / av; /*b*/
  obs[2] = bs[2] / av; /*g*/
  obs[3] = bs[3] / av; /*r*/
}

template<const int mode>
static __forceinline void NormRGBM(long *obs, long *bs, int av) {
  /* build average of each channel an join */
  obs[0] = bs[0]     ; /*h*/
  obs[1] = bs[1] / av; /*b*/
  obs[2] = bs[2] / av; /*g*/
  obs[3] = bs[3] / av; /*r*/
}

template<const int mode>
static __forceinline void NormRGBH(float *obs, float *bs, int av) {
  const float rav = 1.0f / av;

  /* build average of each channel an join */
  obs[0] =     (bs[0] * rav             )       ; /*d[ 0,1]*/
  obs[1] = powf(bs[1] * rav, 1.0f / 2.2f) * 0xFF; /*z[-1,1]*/
  obs[2] = powf(bs[2] * rav, 1.0f / 2.2f) * 0xFF; /*y[-1,1]*/
  obs[3] = powf(bs[3] * rav, 1.0f / 2.2f) * 0xFF; /*x[-1,1]*/
}

template<const int mode>
static __forceinline void NormRGBM(float *obs, float *bs, int av) {
  const float rav = 1.0f / av;

  /* build average of each channel an join */
  obs[0] =      bs[0]                           ; /*h*/
  obs[1] = powf(bs[1] * rav, 1.0f / 2.2f) * 0xFF; /*b*/
  obs[2] = powf(bs[2] * rav, 1.0f / 2.2f) * 0xFF; /*g*/
  obs[3] = powf(bs[3] * rav, 1.0f / 2.2f) * 0xFF; /*r*/
}

template<const int mode>
static __forceinline void NormXYZD(long *ons, long *ns, int av) {
  ons[0] = ns[0] / av; /*d[ 0,1]*/
  ons[1] = ns[1] / av; /*z[-1,1]*/
  ons[2] = ns[2] / av; /*y[-1,1]*/
  ons[3] = ns[3] / av; /*x[-1,1]*/
}

template<const int mode>
static __forceinline void NormXYZD(float *onn, float *nn, int av) {
  const float rav = 1.0f / av;
  float len;

  /* prevent singularity */
  len = nn[1] * nn[1] + nn[2] * nn[2] + nn[3] * nn[3];
  if (!len)
    len = nn[1] = 1.0f, nn[2] = nn[3] = 0.0f;
  else
    len = rsqrt(len);

  onn[0] = nn[0] * rav; /*d[ 0,1]*/
  onn[1] = nn[1] * len; /*z[-1,1]*/
  onn[2] = nn[2] * len; /*y[-1,1]*/
  onn[3] = nn[3] * len; /*x[-1,1]*/
}

template<const int mode>
static __forceinline void NormXYCD(long *onn, long *nn, int av) {
  abort();
}

template<const int mode>
static __forceinline void NormXYCD(float *onn, float *nn, int av) {
  const float rav = 1.0f / av;
  float len;

  /* prevent singularity */
  len = nn[4] * nn[4] + nn[2] * nn[2] + nn[3] * nn[3];
  if (!len)
    len = nn[4] = 1.0f, nn[2] = nn[3] = 0.0f;
  else
    len = rsqrt(len);

  onn[0] = nn[0] * rav; /*d[ 0,1]*/
  onn[1] = nn[1] * rav; /*c[-1,1]*/
  onn[2] = nn[2] * len; /*y[-1,1]*/
  onn[3] = nn[3] * len; /*x[-1,1]*/
//onn[4] = nn[4] * len; /*z[-1,1]*/
}

/* ####################################################################################
 */

template<const int mode>
static __forceinline void LookRGBH(long *bs, long *br) {
  /* collect magnitudes */
  br[0] = qmax(bs[0], br[1]); /*h*/
  br[1] = qmax(bs[1], br[1]); /*b*/
  br[2] = qmax(bs[2], br[2]); /*g*/
  br[3] = qmax(bs[3], br[3]); /*r*/
}

template<const int mode>
static __forceinline void LookRGBH(float *bs, float *br) {
  /* collect magnitudes */
  br[0] = qmax(bs[0], br[1]); /*h*/
  br[1] = qmax(bs[1], br[1]); /*b*/
  br[2] = qmax(bs[2], br[2]); /*g*/
  br[3] = qmax(bs[3], br[3]); /*r*/
}

template<const int mode>
static __forceinline void LookXYZD(long *ns, long *nr) {
  /* collect magnitudes */
  nr[1] = qmax(qabs(ns[1]), nr[1]); /*z[-1,1]*/
  nr[2] = qmax(qabs(ns[2]), nr[2]); /*y[-1,1]*/
  nr[3] = qmax(qabs(ns[3]), nr[3]); /*x[-1,1]*/
}

template<const int mode>
static __forceinline void LookXYZD(float *nn, float *nr) {
  if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) {
    /* calculate maximum partial derivative */
    float rel = (
      qmax(
	qabs(nn[2]),
	qabs(nn[3])
      )
      /
      qmax(
	qabs(nn[1]),
	fabs(0.001f)
      )
    );

    /* collect magnitudes */
    nr[1] = qmax(     rel   , nr[1]); /*r[ 0,inf]*/
    nr[2] = qmax(qabs(nn[2]), nr[2]); /*y[-1,1]*/
    nr[3] = qmax(qabs(nn[3]), nr[3]); /*x[-1,1]*/
  }
  else if ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_XYZ) {
    /* collect magnitudes */
    nr[1] = qmax(qabs(nn[1]), nr[1]); /*z[-1,1]*/
    nr[2] = qmax(qabs(nn[2]), nr[2]); /*y[-1,1]*/
    nr[3] = qmax(qabs(nn[3]), nr[3]); /*x[-1,1]*/
  }
}

template<const int mode>
static __forceinline void LookXYCD(long *nn, long *nr) {
  abort();
}

template<const int mode>
static __forceinline void LookXYCD(float *nn, float *nr) {
  /* collect magnitudes */
  nr[2] = qmax(abs(nn[2]), nr[2]); /*y[-1,1]*/
  nr[3] = qmax(abs(nn[3]), nr[3]); /*x[-1,1]*/
}

/* ####################################################################################
 */

template<const int mode>
static __forceinline ULONG JoinRGBH(long *bs, long *br) {
  ULONG b = 0;

  /* build average of each channel an join */
  b |= (bs[0] << 24); /*h*/
  b |= (bs[1] << 16); /*b*/
  b |= (bs[2] <<  8); /*g*/
  b |= (bs[3] <<  0); /*r*/

  return b;
}

template<const int mode>
static __forceinline ULONG JoinRGBH(float *bs, float *br) {
  ULONG b = 0;

  /* build average of each channel an join */
  b |= (rint<0xFF>(bs[0]) << 24); /*d[ 0,1]*/
  b |= (rint<0xFF>(bs[1]) << 16); /*z[-1,1]*/
  b |= (rint<0xFF>(bs[2]) <<  8); /*y[-1,1]*/
  b |= (rint<0xFF>(bs[3]) <<  0); /*x[-1,1]*/

  return b;
}

template<const int mode>
static __forceinline ULONG JoinXYZD(long *ns, long *nr) {
  ULONG n = 0;

  n |= (ns[0] + 0x00) << 24; /*d[ 0,1]*/
  n |= (ns[1] + 0x80) << 16; /*z[-1,1]*/
  n |= (ns[2] + 0x80) <<  8; /*y[-1,1]*/
  n |= (ns[3] + 0x80) <<  0; /*x[-1,1]*/

  return n;
}

template<const int mode>
static __forceinline ULONG JoinXYZD(float *nn, float *nr) {
  ULONG n = 0;
  float vec[4], len;
  float derivb = NORMALS_FLOAT_DXDY_TANGENTSPACE;		// [0.5f,1.0f]

  vec[0] = nn[0];
  vec[1] = nn[1];
  vec[2] = nn[2];
  vec[3] = nn[3];

  /* ################################################################# */
  if (((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYt) ||
      ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) ||
      ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYDZt)) {
    if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) {
      /* get the minimum divider we have to support
       * based on the vectors in the 4x4 block
       *
       * this will select a constant z-multiplier over
       * the full 4x4 block with that we calculate the
       * partial derivative:
       *
       *  x / z * multiplier;
       *  y / z * multiplier;
       *  z / z * multiplier;
       */
      float derivx = 1.0f / nr[1];				// [...,1.0f]
      if (derivb < derivx)
	derivb = derivx;
      if (derivb > 1.0f)
	derivb = 1.0f;
    }

    if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYDZt) {
      /* get the minimum divider we have to support
       * based on the vectors in the 1x1 block
       */
      float derivx = 1.0f / vec[1];				// [...,1.0f]
      if (derivb < derivx)
	derivb = derivx;
      if (derivb > 1.0f)
	derivb = 1.0f;
    }

#if 0
    vec[1] =
      qmax(
	qabs(vec[1]),
      qmax(
	qabs(vec[2]) * derivb,
	qabs(vec[3]) * derivb
      ));
#else
    float up = derivb * (
      qmax(
	qabs(vec[2]),
	qabs(vec[3])
      )
      /
      qmax(
	qabs(vec[1]),
	fabs(0.001f)
      )
    );

    /* prevent singularity */
    vec[1] = qmax(vec[1], 0.001f);
    if (up > 1.0f) {
      up = 1.0f / up;

      vec[2] *= up;
      vec[3] *= up;
    }
#endif
  }
  else if ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_XYZ) {
    vec[1] = qmax(vec[1], 0.0f);
  }

  /* ################################################################# */
  if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_XYZ) {
    if (mode & TRGTNORM_CUBESPACE)
      len = 1.0f / qmax(vec[1], qmax(vec[2], vec[3]));
    else
      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);

    vec[1] *= len; vec[1] *= 0.5f; vec[1] += 0.5f;
    vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
    vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
  }
  /* ################################################################# */
  else if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_XYZt) {
    if (mode & TRGTNORM_CUBESPACE)
      len = 1.0f / qmax(vec[1], qmax(vec[2], vec[3]));
    else
      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);

    vec[1] *= len; vec[1] *= 1.0f; vec[1] += 0.0f;
    vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
    vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
  }
  /* ################################################################# */
  else if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_XYt) {
    if (mode & TRGTNORM_CUBESPACE) {
      float lnn;

      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
      lnn = rsqrt(vec[2] * vec[2] + vec[3] * vec[3]);

      float factor = (2.0f - qmax(vec[2] * lnn, vec[3] * lnn)) * len;

      vec[1]  =                                   1.0f;
      vec[2] *= factor; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= factor; vec[3] *= 0.5f; vec[3] += 0.5f;
    }
    else {
      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);

      vec[1]  =                                1.0f;
      vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
    }
  }
  /* ################################################################# */
  else if (((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYt) ||
	   ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) ||
	   ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYDZt)) {
    if (mode & TRGTNORM_CUBESPACE) {
      float lnn;

      /* this format is a bit special */
      len = vec[1] / derivb;
      len = 1.0f / len;
      /* == equals == */
      len = derivb / vec[1];
      lnn = rsqrt(vec[2] * vec[2] + vec[3] * vec[3]);

      float factor = (2.0f - qmax(vec[2] * lnn, vec[3] * lnn)) * len;

      vec[1] *= len;    vec[1] *= 0.5f; vec[1] += 0.5f;
      vec[2] *= factor; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= factor; vec[3] *= 0.5f; vec[3] += 0.5f;
		        derivb *= 0.5f; derivb += 0.5f;

#if 0
      if (1) {
	float chk[4], cln, fct;

	chk[2] = (vec[2] - 0.5f) / 0.5f;
	chk[3] = (vec[3] - 0.5f) / 0.5f;

	cln = sqrt(chk[2] * chk[2] + chk[3] * chk[3]);

	fct = 2.0f - max(chk[2] / cln, chk[3] / cln);

	chk[1]  = 1.0f * derivb;
	chk[2] /= fct;
	chk[3] /= fct;

	cln = sqrt(chk[1] * chk[1] + chk[2] * chk[2] + chk[3] * chk[3]);

	chk[1] /= cln;
	chk[2] /= cln;
	chk[3] /= cln;

	cln = 1;
      }
#endif

      assert(fabs(vec[1] - derivb) < 0.001f);
    }
    else {
      /* this format is fully compatible with the built-in shaders */
      len = vec[1] / derivb;
      len = 1.0f / len;
      /* == equals == */
      len = derivb / vec[1];

      vec[1] *= len; vec[1] *= 0.5f; vec[1] += 0.5f;
      vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
		     derivb *= 0.5f; derivb += 0.5f;

      assert(fabs(vec[1] - derivb) < 0.001f);
    }
  }
  /* ################################################################# */
  else if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_AZt) {
    float ang;

    len = qsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
    ang = atan2(vec[2], vec[3]) / (float)M_PI; vec[2] = ang;

    vec[1] *= 1.0f; vec[1] += 0.0f;
    vec[2] *= 0.5f; vec[2] += 0.5f;
    vec[3]  =                 1.0f;
  }

  /* simple quantization (we can do better though!) */
    n |= (rint<0xFF>(vec[0] * 0x01) << 24); /*d[ 0,1]*/
  if (((mode & TRGTMODE_CODING) != TRGTMODE_CODING_XYZ) &&
      ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_DXDYt) &&
//    ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_DXDYdZt) &&	// for this Z isn't allowd to move
      ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_DXDYDZt)) {
    n |= (sint<0xFF>(vec[1] * 0xFF) << 16); /*z[-1,1]*/
    n |= (sint<0xFF>(vec[2] * 0xFF) <<  8); /*y[-1,1]*/
    n |= (sint<0xFF>(vec[3] * 0xFF) <<  0); /*x[-1,1]*/
  }
  else {
    float normal[4];
    long sides[4][2];
    long sidep[4],
	 bestp[4];
    float bbox[8];
    float best;

    /* the original vector in up-scaled quantized space (half-normal, length 0.5) */
    normal[3] = (vec[3] - 0.5f) * 0xFFL;
    normal[2] = (vec[2] - 0.5f) * 0xFFL;
    normal[1] = (vec[1] - 0.5f) * 0xFFL;

    /* the quantized opposing corner vectors in up-scaled quantized space */
    sides[3][0] = qfloor(normal[3]);
    sides[2][0] = qfloor(normal[2]);
    sides[1][0] = qfloor(normal[1]);
    sides[3][1] = min(sides[3][0] + 1, 0x7F);
    sides[2][1] = min(sides[2][0] + 1, 0x7F);
    sides[1][1] = min(sides[1][0] + 1, 0x7F);

#define	qcpy(a, b)    a[3] = b[3] , a[2] = b[2] , a[1] = b[1]
#define	qlen(z,y,x)   z[3] * z[3] + y[2] * y[2] + x[1] * x[1]

    /* "normal" is a normalized vector (though in up-scaled space)
     * and regardless the length doesn't need to be rescaled as it's
     * length gets canceled out (being in every dot-product as a constant)
     */
#define	qdot(a, b, z, y, x)	(		\
	(a[3] * (sidep[3] = b[3][z])) + 	\
	(a[2] * (sidep[2] = b[2][y])) + 	\
	(a[1] * (sidep[1] = b[1][x]))		\
  ) * rsqrt(b[3][z] * b[3][z] + b[2][y] * b[2][y] + b[1][x] * b[1][x])

    /* calculate the angles between each of the eight corners of the cube on
     * the lattice which are around the end-position of the original normal-vector
     * (the point on the unit-sphere)
     *
     * dot -> cos(angle) -> cos(0) => 1 -> select largest dot
     */
    bbox[0] = qdot(normal, sides, 0, 0, 0);                     best = bbox[0], qcpy(bestp, sidep);
    bbox[1] = qdot(normal, sides, 0, 0, 1); if (best < bbox[1]) best = bbox[1], qcpy(bestp, sidep);
    bbox[2] = qdot(normal, sides, 0, 1, 0); if (best < bbox[2]) best = bbox[2], qcpy(bestp, sidep);
    bbox[3] = qdot(normal, sides, 0, 1, 1); if (best < bbox[3]) best = bbox[3], qcpy(bestp, sidep);
    bbox[4] = qdot(normal, sides, 1, 0, 0); if (best < bbox[4]) best = bbox[4], qcpy(bestp, sidep);
    bbox[5] = qdot(normal, sides, 1, 0, 1); if (best < bbox[5]) best = bbox[5], qcpy(bestp, sidep);
    bbox[6] = qdot(normal, sides, 1, 1, 0); if (best < bbox[6]) best = bbox[6], qcpy(bestp, sidep);
    bbox[7] = qdot(normal, sides, 1, 1, 1); if (best < bbox[7]) best = bbox[7], qcpy(bestp, sidep);

    /* put the 0.5 back (8 bit -> 0x80 etc.) */
    bestp[3] += 0x80L;
    bestp[2] += 0x80L;
    bestp[1] += 0x80L;

    n |= qmax(qmin(bestp[1], 0xFFL), 0L) << 16; /*z[-1,1]*/
    n |= qmax(qmin(bestp[2], 0xFFL), 0L) <<  8; /*y[-1,1]*/
    n |= qmax(qmin(bestp[3], 0xFFL), 0L) <<  0; /*x[-1,1]*/
  }

  return n;
}

template<const int mode>
static __forceinline ULONG JoinXYCD(long *nn, long *nr) {
  abort(); return 0;
}

template<const int mode>
static __forceinline ULONG JoinXYCD(float *nn, float *nr) {
  ULONG n = 0;
  float vec[5], len;

  vec[0] = nn[0];
  vec[1] = nn[1];
  vec[2] = nn[2];
  vec[3] = nn[3];
  vec[4] = qsqrt(1.0f  - qmin(1.0f, vec[2] * vec[2] + vec[3] * vec[3]));

  len = rsqrt(vec[4] * vec[4] + vec[2] * vec[2] + vec[3] * vec[3]);

  vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
  vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
  vec[4] *= len; vec[4] *= 0.5f; vec[4] += 0.5f;

  n |= (rint<0xFF>(vec[0] * 0x01) << 24); /*d[ 0,1]*/
  n |= (rint<0xFF>(vec[1] * 0x01) << 16); /*c[-1,1]*/
  n |= (sint<0xFF>(vec[2] * 0xFF) <<  8); /*y[-1,1]*/
  n |= (sint<0xFF>(vec[3] * 0xFF) <<  0); /*x[-1,1]*/

  return n;
}

/* ####################################################################################
 */

template<const int mode, const int A, const int R, const int G, const int B>
static __forceinline USHORT QuntRGBH(long *bs, long *br) {
  USHORT b = 0;

  /* build average of each channel an join */
  b <<= A; b |= (bs[0] >> (8 - A)); /*h*/
  b <<= B; b |= (bs[3] >> (8 - B)); /*b*/
  b <<= G; b |= (bs[2] >> (8 - G)); /*g*/
  b <<= R; b |= (bs[1] >> (8 - R)); /*r*/

  return b;
}

template<const int mode, const int A, const int R, const int G, const int B>
static __forceinline USHORT QuntRGBH(float *bs, float *br) {
  const float rnrm = 1.0f / 0xFF;
  USHORT b = 0;

  /* build average of each channel an join */
  b <<= A; b |= (rint<(1 << A) - 1>(bs[0] * ((1 << A) - 1) * rnrm)); /*d[ 0,1]*/
  b <<= B; b |= (rint<(1 << B) - 1>(bs[3] * ((1 << B) - 1) * rnrm)); /*z[-1,1]*/
  b <<= G; b |= (rint<(1 << G) - 1>(bs[2] * ((1 << G) - 1) * rnrm)); /*y[-1,1]*/
  b <<= R; b |= (rint<(1 << R) - 1>(bs[1] * ((1 << R) - 1) * rnrm)); /*x[-1,1]*/

  return b;
}

template<const int mode, int D, const int X, const int Y, const int Z>
static __forceinline USHORT QuntXYZD(long *ns, long *nr) {
  USHORT n = 0;

  /* build average of each channel an join */
  n <<= D; n |= ((ns[0] + 0x00) >> (8 - D)); /*h*/
  n <<= Z; n |= ((ns[3] + 0x80) >> (8 - Z)); /*b*/
  n <<= Y; n |= ((ns[2] + 0x80) >> (8 - Y)); /*g*/
  n <<= X; n |= ((ns[1] + 0x80) >> (8 - X)); /*r*/

  return n;
}

template<const int mode, int D, const int X, const int Y, const int Z>
static __forceinline USHORT QuntXYZD(float *nn, float *nr) {
  USHORT n = 0;
  float vec[4], len;
  float derivb = NORMALS_FLOAT_DXDY_TANGENTSPACE;		// [0.5f,1.0f]

  vec[0] = nn[0];
  vec[1] = nn[1];
  vec[2] = nn[2];
  vec[3] = nn[3];

  /* ################################################################# */
  if (((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYt) ||
      ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) ||
      ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYDZt)) {
    if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) {
      /* get the minimum divider we have to support
       * based on the vectors in the 4x4 block
       *
       * this will select a constant z-multiplier over
       * the full 4x4 block with that we calculate the
       * partial derivative:
       *
       *  x / z * multiplier;
       *  y / z * multiplier;
       *  z / z * multiplier;
       */
      float derivx = 1.0f / nr[1];				// [...,1.0f]
      if (derivb < derivx)
	derivb = derivx;
      if (derivb > 1.0f)
	derivb = 1.0f;
    }
    
    if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYDZt) {
      /* get the minimum divider we have to support
       * based on the vectors in the 1x1 block
       */
      float derivx = 1.0f / vec[1];				// [...,1.0f]
      if (derivb < derivx)
	derivb = derivx;
      if (derivb > 1.0f)
	derivb = 1.0f;
    }

#if 0
    vec[1] =
      qmax(
	qabs(vec[1]),
      qmax(
	qabs(vec[2]) * derivb,
	qabs(vec[3]) * derivb
      ));
#else
    float up = derivb * (
      qmax(
	qabs(vec[2]),
	qabs(vec[3])
      )
      /
      qmax(
	qabs(vec[1]),
	fabs(0.001f)
      )
    );

    /* prevent singularity */
    vec[1] = qmax(vec[1], 0.001f);
    if (up > 1.0f) {
      up = 1.0f / up;

      vec[2] *= up;
      vec[3] *= up;
    }
#endif
  }
  else if ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_XYZ) {
    vec[1] = qmax(vec[1], 0.0f);
  }

  /* ################################################################# */
  if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_XYZ) {
    if (mode & TRGTNORM_CUBESPACE)
      len = 1.0f / qmax(vec[1], qmax(vec[2], vec[3]));
    else
      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);

    vec[1] *= len; vec[1] *= 0.5f; vec[1] += 0.5f;
    vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
    vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
  }
  /* ################################################################# */
  else if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_XYZt) {
    if (mode & TRGTNORM_CUBESPACE)
      len = 1.0f / qmax(vec[1], qmax(vec[2], vec[3]));
    else
      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);

    vec[1] *= len; vec[1] *= 1.0f; vec[1] += 0.0f;
    vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
    vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
  }
  /* ################################################################# */
  else if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_XYt) {
    if (mode & TRGTNORM_CUBESPACE) {
      float lnn;

      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
      lnn = rsqrt(vec[2] * vec[2] + vec[3] * vec[3]);

      float factor = (2.0f - qmax(vec[2] * lnn, vec[3] * lnn)) * len;

      vec[1]  =                                   1.0f;
      vec[2] *= factor; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= factor; vec[3] *= 0.5f; vec[3] += 0.5f;
    }
    else {
      len = rsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);

      vec[1]  =                                1.0f;
      vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
    }
  }
  /* ################################################################# */
  else if (((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYt) ||
	   ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYdZt) ||
	   ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_DXDYDZt)) {
    if (mode & TRGTNORM_CUBESPACE) {
      float lnn;

      /* this format is a bit special */
      len = vec[1] / derivb;
      len = 1.0f / len;
      /* == equals == */
      len = derivb / vec[1];
      lnn = rsqrt(vec[2] * vec[2] + vec[3] * vec[3]);

      float factor = (2.0f - qmax(vec[2] * lnn, vec[3] * lnn)) * len;

      vec[1] *= len;    vec[1] *= 0.5f; vec[1] += 0.5f;
      vec[2] *= factor; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= factor; vec[3] *= 0.5f; vec[3] += 0.5f;
		        derivb *= 0.5f; derivb += 0.5f;

#if 0
      if (1) {
	float chk[4], cln, fct;

	chk[2] = (vec[2] - 0.5f) / 0.5f;
	chk[3] = (vec[3] - 0.5f) / 0.5f;

	cln = sqrt(chk[2] * chk[2] + chk[3] * chk[3]);

	fct = 2.0f - max(chk[2] / cln, chk[3] / cln);

	chk[1]  = 1.0f * derivb;
	chk[2] /= fct;
	chk[3] /= fct;

	cln = sqrt(chk[1] * chk[1] + chk[2] * chk[2] + chk[3] * chk[3]);

	chk[1] /= cln;
	chk[2] /= cln;
	chk[3] /= cln;

	cln = 1;
      }
#endif

      assert(fabs(vec[1] - derivb) < 0.001f);
    }
    else {
      /* this format is fully compatible with the built-in shaders */
      len = vec[1] / derivb;
      len = 1.0f / len;
      /* == equals == */
      len = derivb / vec[1];

      vec[1] *= len; vec[1] *= 0.5f; vec[1] += 0.5f;
      vec[2] *= len; vec[2] *= 0.5f; vec[2] += 0.5f;
      vec[3] *= len; vec[3] *= 0.5f; vec[3] += 0.5f;
		     derivb *= 0.5f; derivb += 0.5f;

      assert(fabs(vec[1] - derivb) < 0.001f);
    }
  }
  /* ################################################################# */
  else if ((mode & TRGTMODE_CODING) == TRGTMODE_CODING_AZt) {
    float ang;

    len = qsqrt(vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
    ang = atan2(vec[2], vec[3]) / (float)M_PI; vec[2] = ang;

    vec[1] *= 1.0f; vec[1] += 0.0f;
    vec[2] *= 0.5f; vec[2] += 0.5f;
    vec[3]  =                 1.0f;
  }

  /* simple quantization (we can do better though!) */
    n <<= D; n |= (rint<(1 << D) - 1>(vec[0] * ((1 << D) - 1) / 0xFF)); /*d[ 0,1]*/
  if (((mode & TRGTMODE_CODING) != TRGTMODE_CODING_XYZ) &&
      ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_DXDYt) &&
//    ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_DXDYdZt) &&	// for this Z isn't allowd to move
      ((mode & TRGTMODE_CODING) != TRGTMODE_CODING_DXDYDZt)) {
    n <<= X; n |= (sint<(1 << X) - 1>(vec[3] * ((1 << X) - 1)       )); /*x[-1,1]*/
    n <<= Y; n |= (sint<(1 << Y) - 1>(vec[2] * ((1 << Y) - 1)       )); /*y[-1,1]*/
    n <<= Z; n |= (sint<(1 << Z) - 1>(vec[1] * ((1 << Z) - 1)       )); /*z[-1,1]*/
  }
  else {
    float normal[4];
    long sides[4][2];
    long sidep[4],
	 bestp[4];
    float bbox[8];
    float best = -FLT_MAX;
    long  len;

    /* the original vector in up-scaled quantized space (half-normal, length 0.5) */
    normal[3] = (vec[3] - 0.5f) * ((1 << X) - 1);
    normal[2] = (vec[2] - 0.5f) * ((1 << Y) - 1);
    normal[1] = (vec[1] - 0.5f) * ((1 << Z) - 1);

    /* make an iteration for each available lattice-point, divided by 2 (don't
     * check positions twice because of overlapping bboxes, rough approximation)
     */
    int   lattice = max((1 << Z) - 1, max((1 << Y) - 1, (1 << X) - 1));
    float expansn = 0.5f / qmax(qabs(vec[3] - 0.5f), qmax(qabs(vec[2] - 0.5f), qabs(vec[1] - 0.5f)));

    int iterations = lattice, bi,
        expansions = (long)(iterations * (expansn - 1.0f));

    for (int i = (iterations + expansions); i >= (iterations - expansions); i--) {
      /* the quantized opposing corner vectors in up-scaled quantized space */
      sides[3][0] = qfloor(normal[3] * i / iterations);
      sides[2][0] = qfloor(normal[2] * i / iterations);
      sides[1][0] = qfloor(normal[1] * i / iterations);
      /* shorten the vector per iteration */
      sides[3][1] = min(sides[3][0] + 1, ((1L << X) >> 1) - 1);
      sides[2][1] = min(sides[2][0] + 1, ((1L << X) >> 1) - 1);
      sides[1][1] = min(sides[1][0] + 1, ((1L << X) >> 1) - 1);

#define	qcpy(a, b)    a[3] = b[3] , a[2] = b[2] , a[1] = b[1]
#define	qlen(z,y,x)   z[3] * z[3] + y[2] * y[2] + x[1] * x[1]

      /* "normal" is a normalized vector (though in up-scaled space and len 0.5)
       * and regardless the length doesn't need to be rescaled as it's
       * length gets canceled out (being in every dot-product as a constant)
       */
#define	qdot(a, b, z, y, x)	(		\
	(a[3] * (sidep[3] = b[3][z])) + 	\
	(a[2] * (sidep[2] = b[2][y])) + 	\
	(a[1] * (sidep[1] = b[1][x]))		\
  ) * rsqrt(len = (b[3][z] * b[3][z] + b[2][y] * b[2][y] + b[1][x] * b[1][x]))

      /* calculate the angles between each of the eight corners of the cube on
       * the lattice which are around the end-position of the original normal-vector
       * (the point on the unit-sphere)
       *
       * dot -> cos(angle) -> cos(0) => 1 -> select largest dot
       */
      bbox[0] = qdot(normal, sides, 0, 0, 0); if (len && (best < bbox[0])) { best = bbox[0], qcpy(bestp, sidep); bi = i; }
      bbox[1] = qdot(normal, sides, 0, 0, 1); if (len && (best < bbox[1])) { best = bbox[1], qcpy(bestp, sidep); bi = i; }
      bbox[2] = qdot(normal, sides, 0, 1, 0); if (len && (best < bbox[2])) { best = bbox[2], qcpy(bestp, sidep); bi = i; }
      bbox[3] = qdot(normal, sides, 0, 1, 1); if (len && (best < bbox[3])) { best = bbox[3], qcpy(bestp, sidep); bi = i; }
      bbox[4] = qdot(normal, sides, 1, 0, 0); if (len && (best < bbox[4])) { best = bbox[4], qcpy(bestp, sidep); bi = i; }
      bbox[5] = qdot(normal, sides, 1, 0, 1); if (len && (best < bbox[5])) { best = bbox[5], qcpy(bestp, sidep); bi = i; }
      bbox[6] = qdot(normal, sides, 1, 1, 0); if (len && (best < bbox[6])) { best = bbox[6], qcpy(bestp, sidep); bi = i; }
      bbox[7] = qdot(normal, sides, 1, 1, 1); if (len && (best < bbox[7])) { best = bbox[7], qcpy(bestp, sidep); bi = i; }
    }

    /* put the 0.5 back (8 bit -> 0x80 etc.) */
    bestp[3] += (1L << X) >> 1;
    bestp[2] += (1L << Y) >> 1;
    bestp[1] += (1L << Z) >> 1;

    n <<= X; n |= qmax(qmin(bestp[3], (1L << X) - 1L), 0L); /*x[-1,1]*/
    n <<= Y; n |= qmax(qmin(bestp[2], (1L << Y) - 1L), 0L); /*y[-1,1]*/
    n <<= Z; n |= qmax(qmin(bestp[1], (1L << Z) - 1L), 0L); /*z[-1,1]*/
  }

  return n;
}

template<const int mode>
static __forceinline USHORT QuntXYCD(long *nn, long *nr) {
  abort(); return 0;
}

template<const int mode>
static __forceinline USHORT QuntXYCD(float *nn, float *nr) {
  abort(); return 0;
}

/* ####################################################################################
 */

IDirect3D9 *pD3D = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

bool TextureInit() {
  D3DPRESENT_PARAMETERS Parameters;
  D3DDISPLAYMODE Mode;

  if (pD3D && pD3DDevice)
    return true;
  if (!pD3D && !(pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    return false;

  pD3D->GetAdapterDisplayMode(0, &Mode);

  memset(&Parameters, 0, sizeof(Parameters));

  Parameters.BackBufferWidth  = 1;
  Parameters.BackBufferHeight = 1;
  Parameters.BackBufferFormat = Mode.Format;
  Parameters.BackBufferCount  = 1;
  Parameters.SwapEffect       = D3DSWAPEFFECT_COPY;
  Parameters.Windowed         = TRUE;

  /* can you believe A8B8G8R8 -> A8R8G8B8 needs hardware? */
  HRESULT res;
  if ((res = pD3D->CreateDevice(
    D3DADAPTER_DEFAULT,
    D3DDEVTYPE_NULLREF,
//  D3DDEVTYPE_REF,
//  D3DDEVTYPE_SW,
//  D3DDEVTYPE_HAL,
    GetConsoleWindow(),
    D3DCREATE_MULTITHREADED |
    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
//  D3DCREATE_HARDWARE_VERTEXPROCESSING,
    &Parameters,
    &pD3DDevice
  )) != D3D_OK) {
    pD3D->Release();
    pD3D = NULL;
    pD3DDevice = NULL;

    return false;
  }

  return true;
}

void TextureCleanup() {
  if (pD3DDevice)
    pD3DDevice->Release();
    pD3DDevice = NULL;
  if (pD3D)
    pD3D->Release();
    pD3D = NULL;
}

/* ####################################################################################
 */

bool TextureConvert(D3DSURFACE_DESC &info, LPDIRECT3DTEXTURE9 *tex, bool black) {
  LPDIRECT3DTEXTURE9 replct;
  LPDIRECT3DSURFACE9 stex, srep;
  HRESULT res;

//pD3DDevice->CreateTexture(info.Width, info.Height, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_SYSTEMMEM, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 1, 0, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 1, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 1, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 1, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8B8G8R8, D3DPOOL_SYSTEMMEM, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &replct, NULL);
//pD3DDevice->CreateTexture(info.Width, info.Height, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &replct, NULL);
  res = pD3DDevice->CreateTexture(info.Width, info.Height, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &replct, NULL);
  if (res != D3D_OK)
    return false;

  (*tex)->GetSurfaceLevel(0, &stex);
  replct->GetSurfaceLevel(0, &srep);

  res = D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, D3DX_FILTER_NONE, 0);

  /* this is not right unfortunately L8 becomes L8A8 */
#if 0
  /* put a custom default alpha-value */
  if (!findAlpha(info.Format)) {
    D3DLOCKED_RECT texs;
    unsigned char a = (black ? 0x00000000 : 0xFF000000);

    replct->LockRect(0, &texs, NULL, 0);
    ULONG *sTex = (ULONG *)texs.pBits;

    for (int y = 0; y < (int)info.Height; y += 1) {
    for (int x = 0; x < (int)info.Width ; x += 1) {
      ULONG t = sTex[(y * info.Width) + x];
      sTex[(y * info.Width) + x] = (t & 0x00FFFFFF) | a;
    }
    }

    replct->UnlockRect(0);
  }
#endif

  stex->Release();
  srep->Release();

  if (res == D3D_OK) {
    (*tex)->Release();
    (*tex) = replct;

    return true;
  }
  else {
    replct->Release();
    replct = (*tex);

    return false;
  }
}

bool TextureDownMip(int w, int h, LPDIRECT3DTEXTURE9 *tex) {
  LPDIRECT3DTEXTURE9 replct;
  D3DSURFACE_DESC texo;
  HRESULT res;

  (*tex)->GetLevelDesc(0, &texo);

  /* the lowest mip-level contains a row or a column of 4x4 blocks
   * we won't generate mip-levels for mips smaller than the BTC-area
   */
  int levels = 1;
  int ww = w;
  int hh = h;
  while ((ww > MIPMAP_MINIMUM) && (hh > MIPMAP_MINIMUM)) {
    ww = (ww + 1) >> 1;
    hh = (hh + 1) >> 1;

    levels++;
  }

  /* calculate how much levels to strip */
  int baselvl = 0;
  int trnslvl = 0;
  while ((texo.Width  > (UINT)w) ||
         (texo.Height > (UINT)h)) {
    texo.Width  = (texo.Width  + 1) >> 1;
    texo.Height = (texo.Height + 1) >> 1;

    baselvl++;
  }

  if ((res = D3DXCreateTexture(
    pD3DDevice,
    texo.Width, texo.Height, 0,
    0, texo.Format, D3DPOOL_SYSTEMMEM, &replct
  )) != D3D_OK)
    return false;

  while (trnslvl < levels) {
    LPDIRECT3DSURFACE9 stex, srep;

    if ((res = (*tex)->GetSurfaceLevel(baselvl, &stex)) != D3D_OK)
      break;
    if ((res = replct->GetSurfaceLevel(trnslvl, &srep)) != D3D_OK)
      break;

    if ((res = D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, D3DX_FILTER_NONE, 0)) != D3D_OK)
      break;

    stex->Release();
    srep->Release();

    baselvl++;
    trnslvl++;
  }

  if (res == D3D_OK) {
    (*tex)->Release();
    (*tex) = replct;

    return true;
  }
  else {
    replct->Release();
    replct = (*tex);

    return false;
  }
}

static bool TextureConvert(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool dither, bool gamma, D3DFORMAT target) {
  LPDIRECT3DTEXTURE9 replct;
  D3DSURFACE_DESC texo;
  LPDIRECT3DSURFACE9 stex, srep;
  DWORD filter = D3DX_FILTER_BOX | (dither ? D3DX_FILTER_DITHER : 0) | (gamma ? D3DX_FILTER_SRGB : 0);
  HRESULT res;

  (*tex)->GetLevelDesc(0, &texo);

  /* the lowest mip-level contains a row or a column of 4x4 blocks
   * we won't generate mip-levels for mips smaller than the BTC-area
   */
  int levels = 1;
  int ww = texo.Width;
  int hh = texo.Height;
  while ((ww > MIPMAP_MINIMUM) && (hh > MIPMAP_MINIMUM)) {
    ww = (ww + 1) >> 1;
    hh = (hh + 1) >> 1;

    levels++;
  }

  if (minlevel < 0)
    minlevel = -minlevel, levels = min(minlevel, levels);

  assert(levels >= 0);
  res = pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, target, D3DPOOL_MANAGED, &replct, NULL);
  if (res != D3D_OK)
    return false;

  (*tex)->GetSurfaceLevel(0, &stex);
  replct->GetSurfaceLevel(0, &srep);

  res = D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, filter, 0);

  stex->Release();
  srep->Release();

  if (res == D3D_OK) {
    (*tex)->Release();
    (*tex) = replct;

    if (D3DXFilterTexture((*tex), NULL, 0, filter) == D3D_OK)
      return true;

    return false;
  }
  else {
    replct->Release();
    replct = (*tex);

    return false;
  }
}

template<typename type>
static bool TextureConvert(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool dither, D3DFORMAT target);

template<>
static bool TextureConvert<long>(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool dither, D3DFORMAT target) {
  return TextureConvert(minlevel, tex, dither, false, target);
};

template<>
static bool TextureConvert<float>(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool dither, D3DFORMAT target) {
  return TextureConvert(minlevel, tex, dither, true, target);
};

static bool TextureCollapse(LPDIRECT3DTEXTURE9 *tex, D3DFORMAT target, bool swizzle) {
  LPDIRECT3DTEXTURE9 replct;
  LPDIRECT3DSURFACE9 stex, srep;
  DWORD filter = D3DX_FILTER_BOX | D3DX_FILTER_DITHER;
  HRESULT res;

  res = pD3DDevice->CreateTexture(1, 1, 0, 0, target, D3DPOOL_MANAGED, &replct, NULL);
  if (res != D3D_OK)
    return false;

  (*tex)->GetSurfaceLevel(0, &stex);
  replct->GetSurfaceLevel(0, &srep);

  RECT oo = {0,0,1,1};
  res = D3DXLoadSurfaceFromSurface(srep, NULL, &oo, stex, NULL, &oo, filter, 0);

  stex->Release();
  srep->Release();

  if (res == D3D_OK) {
    (*tex)->Release();
    (*tex) = replct;

#if 1
    /* put a custom default alpha-value */
    if (swizzle) {
      D3DLOCKED_RECT texs;

      replct->LockRect(0, &texs, NULL, 0);
      ULONG *sTex = (ULONG *)texs.pBits;

      /* swizzle ARGB -> ARBG */
      ULONG t = sTex[0];
      sTex[0] = (t & 0xFFFF0000) | ((t >> 8) & 0x00FF) | ((t & 0x00FF) << 8);
//    sTex[0] = (t & 0xFF0000FF) | ((t >> 8) & 0xFF00) | ((t & 0xFF00) << 8);

      replct->UnlockRect(0);
    }
#endif

    return true;
  }
  else {
    replct->Release();
    replct = (*tex);

    return false;
  }
}

/* ####################################################################################
 */

#define D3DFMT_ATI1	((D3DFORMAT)MAKEFOURCC('A', 'T', 'I', '1'))
#define D3DFMT_ATI2	((D3DFORMAT)MAKEFOURCC('A', 'T', 'I', '2'))
#define D3DFMT_GREY(f)	((f == D3DFMT_L8) || (f == D3DFMT_A8L8) || (f == D3DFMT_A4L4) || (f == D3DFMT_L16))
#define D3DFMT_DXT(f)	((f == D3DFMT_DXT1) || (f == D3DFMT_DXT3) || (f == D3DFMT_DXT5))

#define TCOMPRESS_L		0
#define TCOMPRESS_A		1
#define TCOMPRESS_LA		2
#define TCOMPRESS_LH		3
#define TCOMPRESS_RGB		4
#define TCOMPRESS_RGBV		5
#define TCOMPRESS_RGBA		6
#define TCOMPRESS_RGBH		7
#define	TCOMPRESS_COLOR(fmt)	(((fmt) >= TCOMPRESS_L) && ((fmt) <= TCOMPRESS_RGBH))
#define	TCOMPRESS_GREYS(fmt)	(((fmt) == TCOMPRESS_L) || ((fmt) == TCOMPRESS_LA) || ((fmt) == TCOMPRESS_LH))
#define	TCOMPRESS_TRANS(fmt)	(((fmt) == TCOMPRESS_A) || ((fmt) == TCOMPRESS_LA) || ((fmt) == TCOMPRESS_RGBA))

#define TCOMPRESS_xyZ		8
#define TCOMPRESS_XY		9
#define TCOMPRESS_XYz		10
#define TCOMPRESS_xyz		11
#define TCOMPRESS_xyzV		12
#define TCOMPRESS_xyzD		13
#define TCOMPRESS_XYZ		14
#define TCOMPRESS_XZY		15
#define TCOMPRESS_XYZV		16
#define TCOMPRESS_XZYV		17
#define TCOMPRESS_XYZD		18
#define TCOMPRESS_XZYD		19
#define	TCOMPRESS_NINDEP(fmt)	(((fmt) >= TCOMPRESS_xyZ) && ((fmt) <= TCOMPRESS_xyzD))
#define	TCOMPRESS_SWIZZL(fmt)	(((fmt) == TCOMPRESS_XZY) || ((fmt) == TCOMPRESS_XZYD) || ((fmt) == TCOMPRESS_XZYV))
#define	TCOMPRESS_NORMAL(fmt)	(((fmt) >= TCOMPRESS_xyZ) && ((fmt) <= TCOMPRESS_XZYD))

/* do we have a side-stream? */
#define	TCOMPRESS_SIDES(fmt)	((((fmt) != TCOMPRESS_L) && ((fmt) != TCOMPRESS_RGB) && ((fmt) != TCOMPRESS_RGBV)) || ((fmt) == TCOMPRESS_xyzD) || ((fmt) >= TCOMPRESS_XYZD))

static int TCOMPRESS_CHANNELS(int fmt) {
  switch (fmt) {
    case TCOMPRESS_L		: return 1;
    case TCOMPRESS_A		: return 1;
    case TCOMPRESS_LA		: return 2;
    case TCOMPRESS_LH		: return 2;
    case TCOMPRESS_RGB		: return 3;
    case TCOMPRESS_RGBV		: return 4;
    case TCOMPRESS_RGBA		: return 4;
    case TCOMPRESS_RGBH		: return 4;

    case TCOMPRESS_XY		: return 2;
    case TCOMPRESS_xyZ		: return 1;
    case TCOMPRESS_XYz		: return 2;
    case TCOMPRESS_xyz		: return 3;
    case TCOMPRESS_xyzV		: return 4;
    case TCOMPRESS_xyzD		: return 4;
    case TCOMPRESS_XYZ		: return 3;
    case TCOMPRESS_XZY		: return 3;
    case TCOMPRESS_XYZV		: return 4;
    case TCOMPRESS_XZYV		: return 4;
    case TCOMPRESS_XYZD		: return 4;
    case TCOMPRESS_XZYD		: return 4;
  }

  return 0;
}

/* in Oblivion the encoding to DXT1 apparently turns
 * off the shader which handle the alpha-information
 * to prevent that just code to DXT1 when the information
 * is neutral:
 * - for normals, black means no specular
 * - for colors, white means no transparency/parallax
 */
#define CollapseAlpha(fmt, blk, wht)	(TCOMPRESS_NORMAL(fmt) ? blk       : wht      )
#define MinimalAlpha(fmt, blk, wht, bw)	(TCOMPRESS_NORMAL(fmt) ? blk || bw : wht || bw)
#define ExistAlpha(fmt, org)		((TCOMPRESS_CHANNELS(fmt) > 3) && (org != D3DFMT_DXT1))

/* ------------------------------------------------------------------------------------
 */

template<typename UTYPE, typename type, const int format, const int A>
static bool TextureQuantizeRAW(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool optimize = true) {
  LPDIRECT3DTEXTURE9 text = NULL;
  D3DSURFACE_DESC texo;
  D3DLOCKED_RECT texs;

  (*tex)->GetLevelDesc(0, &texo);

#if 0
  /* Converts a height map into a normal map. The (x,y,z)
   * components of each normal are mapped to the (r,g,t)
   * channels of the output texture.
   */
  HRESULT D3DXComputeNormalMap(
    __out  LPDIRECT3DTEXTURE9 pTexture,
    __in   LPDIRECT3DTEXTURE9 pSrcTexture,
    __in   const PALETTEENTRY *pSrcPalette,
    __in   DWORD Flags,
    __in   DWORD Channel,
    __in   FLOAT Amplitude
    );
#endif

//assert((texo.Height) & 3 == 0);
//assert((texo.Width ) & 3 == 0);

  /* convert to ARGB8 (TODO: support at least the 16bit formats as well) */
  D3DFORMAT origFormat = texo.Format;
  if ((texo.Format != D3DFMT_A8R8G8B8) && !TextureConvert(texo, tex, TCOMPRESS_NORMAL(format)))
    return false;

  /* make a histogram of the alpha-channel */
  if (optimize /*TCOMPRESS_CHANNELS(format) == 4*/) {
    (*tex)->LockRect(0, &texs, NULL, 0);
    ULONG *sTex = (ULONG *)texs.pBits;

    /**/ if (TCOMPRESS_NORMAL(format)) {
      /* forward/backward-z */
      ULONG v = 0x00808000 | (TCOMPRESS_NINDEP(format) ? 0x00 : 0xFF);
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x] | c;

	/* black matte to forward-z */
	/**/ if (( t & 0x00FFFFFF) == 0)
	  t = (t | v);
	/* white matte to forward-z */
	else if ((~t & 0x00FFFFFF) == 0)
	  t = (t & v);

	sTex[(y * texo.Width) + x] = t;
      }
      }
    }

    else if (TCOMPRESS_GREYS(format)) {
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x] | c;
	ULONG a = (t >> 24) & 0xFF; /*a*/
	ULONG r = (t >> 16) & 0xFF; /*a*/
	ULONG g = (t >>  8) & 0xFF; /*a*/
	ULONG b = (t >>  0) & 0xFF; /*a*/

	g = ((r * 5) + (g * 8) + (b * 3) + 8) >> 4;
	t = (a << 24) | (g << 16) | (g << 8) | (g << 0);

	sTex[(y * texo.Width) + x] = t;
      }
      }
    }

    else if (!TCOMPRESS_SIDES(format)) {
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	sTex[(y * texo.Width) + x] |= c;
      }
      }
    }

    (*tex)->UnlockRect(0);
  }

  /* the lowest mip-level contains a row or a column of 4x4 blocks
   * we won't generate mip-levels for mips smaller than the BTC-area
   */
  int levels = 1;
  int ww = texo.Width;
  int hh = texo.Height;
  while ((ww > MIPMAP_MINIMUM) && (hh > MIPMAP_MINIMUM)) {
    ww = (ww + 1) >> 1;
    hh = (hh + 1) >> 1;

    levels++;
  }

  if (minlevel < 0)
    minlevel = -minlevel, levels = min(minlevel, levels);

  /* create the textures */
  assert(levels >= 0);
  switch (TCOMPRESS_CHANNELS(format)) {
    case 4: if (format == TCOMPRESS_RGBV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_X4R4G4B4 : D3DFMT_X1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_RGBA) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_A4R4G4B4 : D3DFMT_A1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_RGBH) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_A4R4G4B4 : D3DFMT_A1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_xyzV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_X4R4G4B4 : D3DFMT_X1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYZV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_X4R4G4B4 : D3DFMT_X1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL); break;
	    if (format == TCOMPRESS_XZYV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_X4R4G4B4 : D3DFMT_X1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL); break;
	    if (format == TCOMPRESS_xyzD) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_A4R4G4B4 : D3DFMT_A1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYZD) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_A4R4G4B4 : D3DFMT_A1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL); break;
	    if (format == TCOMPRESS_XZYD) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, (A > 1 ? D3DFMT_A4R4G4B4 : D3DFMT_A1R5G5B5), D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 3: if (format == TCOMPRESS_RGB ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0,                            D3DFMT_R5G6B5   , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_xyz ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0,                            D3DFMT_R5G6B5   , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYZ ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0,                            D3DFMT_R5G6B5   , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XZY ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0,                            D3DFMT_R5G6B5   , D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 2: if (format == TCOMPRESS_LA  ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0,          D3DFMT_A4L4                       , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_LH  ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0,          D3DFMT_A4L4                       , D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 1: abort(); break;
  }

  /* damit */
  if (!text)
    return false;

  (*tex)->LockRect(0, &texs, NULL, 0);

  const int texi = (int)(*tex)->GetLevelCount();
  const int texl = (int)text->GetLevelCount();
  const int level = texl;

  /* create work-pool here, and let OMP react dynamically to the demand */
  omp_set_nested(3);
//#pragma omp parallel for if(level > 6) schedule(dynamic, 1) num_threads(2)
  for (int l = 0; l < level; l++) {
    D3DSURFACE_DESC texd;
//  D3DLOCKED_RECT texc;
    D3DLOCKED_RECT texr;

    /* square dimension of this surface-level */
    /* square area of this surface-level */
    const int lv = (1 << l);
    const int av = lv * lv;

    text->GetLevelDesc(l, &texd);
    text->LockRect(l, &texr, NULL, 0);

    ULONG *sTex = (ULONG *)texs.pBits;
    UCHAR *dTex = (UCHAR *)texr.pBits;

    /* this can not work, because the input format is always
     * A8R8G8B8 ...
     */
#if 0
    /* can the level be copied out? */
    if ((l < minlevel) && (l < texi)) {
      (*tex)->LockRect(l, &texc, NULL, 0);

      /* advance pointer of compressed blocks */
      int blocks =
	texd.Height *
	texd.Width *
	(TCOMPRESS_CHANNELS(format) >> 1);

      memcpy(texr.pBits, texc.pBits, blocks);

      (*tex)->UnlockRect(l);
      continue;
    }
#endif

//  logrf("level %2d/%2d\r", l + 1, level);
    /* loop over 4x4-blocks of this level (RAW) */
#pragma omp parallel for if((int)texd.Height >= (lv >> 1))	\
			 schedule(dynamic, 1)			\
			 shared(texo, texd, texr)		\
			 firstprivate(dTex)
    for (int y = 0; y < (int)texd.Height; y += 4) {
      if (!(y & 0x3F)) {
	logrf("level %2d/%2d: line %4d/%4d processed        \r", l + 1, level, y, texd.Height);

	/* problematic, the throw() inside may not block all threads ... */
//	PollProgress();
      }

      /* calculate pointer of compressed blocks */
      UCHAR *wTex = (UCHAR *)texr.pBits;

      assert(wTex == dTex);

    for (int x = 0; x < (int)texd.Width; x += 4) {
      a16 UTYPE bTex[2][4*4];
      a16 type  fTex[2][4*4][8];

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	const int yl = ((y + ly) << l);
	const int xl = ((x + lx) << l);

	/* OpenMP can't use [] in the reduction clause, provide
	 * compatible anonymous fake union for individuals
	 */
	typedef	type a16 accu[8];
	accu ts = {0},
	     tt = {0};

	/* access all pixels this level's 4x4-block represents in
	 * the full dimension original surface (high quality mip-mapping)
	 */
	/* global constant execution-time per iteration (guaranteed) */
#pragma omp parallel for if((int)texd.Height < (lv >> 1))	\
			 schedule(static, 1) ordered		\
			 shared(sTex) firstprivate(tt)
	for (int oy = 0; oy < lv; oy += 1) {
	for (int ox = 0; ox < lv; ox += 1) {
	  /* assume tiling: wrap pixels around */
	  const int posx = (xl + ox) % texo.Width;
	  const int posy = (yl + oy) % texo.Height;

	  ULONG t = sTex[(posy * texo.Width) + posx];

	  /* read ARGB -> ABGR */
//	  if (TCOMPRESS_SWIZZL(format))
//	    t = //t;
//	        (((t >> 24) & 0xFF) << 24 /*h*/)
//	      | (((t >> 16) & 0xFF) <<  0 /*r*/)
//	      | (((t >>  0) & 0xFF) <<  8 /*g*/)
//	      | (((t >>  8) & 0xFF) << 16 /*b*/);
//	  else
	    t = //t;
	        (((t >> 24) & 0xFF) << 24 /*h*/)
	      | (((t >> 16) & 0xFF) <<  0 /*r*/)
	      | (((t >>  8) & 0xFF) <<  8 /*g*/)
	      | (((t >>  0) & 0xFF) << 16 /*b*/);

	  /**/ if (TCOMPRESS_COLOR (format))
	    AccuRGBH<ACCUMODE_LINEAR>(tt, t, level, l);	// +=
	  else if (TCOMPRESS_NORMAL(format))
	    AccuXYZD<ACCUMODE_SCALE >(tt, t, level, l);	// +=
	}

#pragma omp ordered
	{
	  /**/ if (TCOMPRESS_COLOR (format))
	    ReduceRGBH<ACCUMODE_LINEAR>(ts, tt);	// +=
	  else if (TCOMPRESS_NORMAL(format))
	    ReduceXYZD<ACCUMODE_SCALE >(ts, tt);	// +=
	}
	}

	/* build average of each channel */
	/**/ if (TCOMPRESS_COLOR (format))
	  NormRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_NINDEP(format))
	  NormXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_NORMAL(format))
	  NormXYZD<TRGTMODE_CODING_DXDYDZt>(fTex[0][(ly * 4) + lx], ts, av);
      }

      type tr[8] = {0};

      /* analyze this level's 4x4-block */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/**/ if (TCOMPRESS_COLOR (format))
	  LookRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NINDEP(format))
	  LookXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NORMAL(format))
	  LookXYZD<TRGTMODE_CODING_DXDYDZt>(fTex[0][(ly * 4) + lx], tr);
      }

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/* build average of each channel an join */
	USHORT t;

	/* write the result ABGR, BGR */
        switch (TCOMPRESS_CHANNELS(format)) {
          /* ABGR -> RGBA */
          case 4:
            if (A > 1) {
	      /**/ if (TCOMPRESS_COLOR (format))
	        t = QuntRGBH<TRGTMODE_CODING_RGB    , 4, 4, 4, 4>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NINDEP(format))
	        t = QuntXYZD<TRGTMODE_CODING_XYZ    , 4, 4, 4, 4>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NORMAL(format))
	        t = QuntXYZD<TRGTMODE_CODING_DXDYDZt, 4, 4, 4, 4>(fTex[0][(ly * 4) + lx], tr);
	    }
	    else {
	      /**/ if (TCOMPRESS_COLOR (format))
	        t = QuntRGBH<TRGTMODE_CODING_RGB    , 1, 5, 5, 5>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NINDEP(format))
	        t = QuntXYZD<TRGTMODE_CODING_XYZ    , 1, 5, 5, 5>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NORMAL(format))
	        t = QuntXYZD<TRGTMODE_CODING_DXDYDZt, 1, 5, 5, 5>(fTex[0][(ly * 4) + lx], tr);
	    }

            bTex[0][(ly * 4) + lx] = (t) | 0x0000;
            break;
          /* -BGR -> RGB- */
	  case 3:
            {
	      /**/ if (TCOMPRESS_COLOR (format))
	        t = QuntRGBH<TRGTMODE_CODING_RGB    , 0, 5, 6, 5>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NINDEP(format))
	        t = QuntXYZD<TRGTMODE_CODING_XYZ    , 0, 5, 6, 5>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NORMAL(format))
	        t = QuntXYZD<TRGTMODE_CODING_DXDYDZt, 0, 5, 6, 5>(fTex[0][(ly * 4) + lx], tr);
	    }

            bTex[0][(ly * 4) + lx] = (t) | 0x0000;
            break;
	  /* AL-- -> LA-- */
          case 2:
            {
	      /**/ if (TCOMPRESS_COLOR (format))
	        t = QuntRGBH<TRGTMODE_CODING_RGB    , 4, 0, 4, 0>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NINDEP(format))
	        t = QuntXYZD<TRGTMODE_CODING_XYZ    , 4, 0, 4, 0>(fTex[0][(ly * 4) + lx], tr);
	      else if (TCOMPRESS_NORMAL(format))
	        t = QuntXYZD<TRGTMODE_CODING_DXDYDZt, 4, 0, 4, 0>(fTex[0][(ly * 4) + lx], tr);
	    }

            bTex[0][(ly * 4) + lx] = (t) | 0x0000;
            break;
          case 1:
            break;
        }
      }

      /* put this level's 4x4-block into the destination surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/* assume tiling: wrap pixels around */
	const int posx = (x + lx) % texd.Width;
	const int posy = (y + ly) % texd.Height;

	USHORT t = bTex[0][(ly * 4) + lx];
	switch (TCOMPRESS_CHANNELS(format)) {
	  /* ABGR -> ARGB */
	  case 4:
	  /* -BGR -> -RGB */
	  case 3:
	    ((USHORT *)wTex)[((posy * texd.Width) + posx) * 1] = (USHORT)t;
	    break;
	  /* LA-- -> AL-- */
	  case 2:
	    ((UCHAR *)wTex)[((posy * texd.Width) + posx) * 1] = (UCHAR)t;
	    break;
	  case 1:
	    break;
	}
      }

      dTex += 0;
    }
    }

    text->UnlockRect(l);
  }


  logrf("                                                      \r");

  (*tex)->UnlockRect(0);
  (*tex)->Release();
  (*tex) = text;

  return true;
}

bool TextureQuantizeR5G5B5V1(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGBV, 1>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGBV, 1>(minlevel, base);

  return res;
}

bool TextureQuantizeR5G5B5A1(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGBA, 1>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGBA, 1>(minlevel, base);

  return res;
}

bool TextureQuantizeR5G5B5H1(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGBH, 1>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGBH, 1>(minlevel, base);

  return res;
}

bool TextureQuantizeR4G4B4V4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGBV, 4>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGBV, 4>(minlevel, base);

  return res;
}

bool TextureQuantizeR4G4B4A4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGBA, 4>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGBA, 4>(minlevel, base);

  return res;
}

bool TextureQuantizeR4G4B4H4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGBH, 4>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGBH, 4>(minlevel, base);

  return res;
}

bool TextureQuantizeR5G6B5(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_RGB, 0>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_RGB, 0>(minlevel, base);

  return res;
}

bool TextureQuantizeL4A4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_LA, 4>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_LA, 4>(minlevel, base);

  return res;
}

bool TextureQuantizeL4H4(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_LH, 4>(minlevel, base);
  else       res = res && TextureQuantizeRAW<USHORT, long , TCOMPRESS_LH, 4>(minlevel, base);

  return res;
}

bool TextureQuantizeX4Y4Z4D4(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_XYZD, 4>(minlevel, norm);

  return res;
}

bool TextureQuantize_X4Y4Z4D4(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_xyzD, 4>(minlevel, norm);

  return res;
}

bool TextureQuantizeX4Y4Z4V4(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_XYZV, 4>(minlevel, norm);

  return res;
}

bool TextureQuantize_X4Y4Z4V4(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_xyzV, 4>(minlevel, norm);

  return res;
}

bool TextureQuantizeX5Y6Z5(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_XYZ, 0>(minlevel, norm);

  return res;
}

bool TextureQuantizeX5Z6Y5(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_XZY, 0>(minlevel, norm);

  return res;
}

bool TextureQuantize_X5Y6Z5(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureQuantizeRAW<USHORT, float, TCOMPRESS_xyz, 0>(minlevel, norm);

  return res;
}

/* ------------------------------------------------------------------------------------
 */

template<typename UTYPE, typename type, int format>
static bool TextureConvertRAW(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool optimize = true) {
  LPDIRECT3DTEXTURE9 text = NULL;
  D3DSURFACE_DESC texo;
  D3DLOCKED_RECT texs;

  (*tex)->GetLevelDesc(0, &texo);

#if 0
  /* Converts a height map into a normal map. The (x,y,z)
   * components of each normal are mapped to the (r,g,t)
   * channels of the output texture.
   */
  HRESULT D3DXComputeNormalMap(
    __out  LPDIRECT3DTEXTURE9 pTexture,
    __in   LPDIRECT3DTEXTURE9 pSrcTexture,
    __in   const PALETTEENTRY *pSrcPalette,
    __in   DWORD Flags,
    __in   DWORD Channel,
    __in   FLOAT Amplitude
    );
#endif

//assert((texo.Height) & 3 == 0);
//assert((texo.Width ) & 3 == 0);

  /* convert to ARGB8 (TODO: support at least the 16bit formats as well) */
  D3DFORMAT origFormat = texo.Format;
  if ((texo.Format != D3DFMT_A8R8G8B8) && !TextureConvert(texo, tex, TCOMPRESS_NORMAL(format)))
    return false;

  /* make a histogram of the alpha-channel */
  if (optimize /*TCOMPRESS_CHANNELS(format) == 4*/) {
    unsigned int histo[4][256], histn[4] = {0,0,0,0};
    memset(&histo, 0, sizeof(histo));
    bool grey = true, blank = true;
    unsigned mask4 = 0;
    unsigned mask5 = 0;
    unsigned mask6 = 0;

    (*tex)->LockRect(0, &texs, NULL, 0);
    ULONG *sTex = (ULONG *)texs.pBits;

    /* exclude the border-region from checking */
    int lft = 0, rgt = (int)texo.Width;
    int top = 0, bot = (int)texo.Height;
    if (ignoreborder) {
      /* 64� -> 1px, 128� -> 2px etc. */
      int borderx = min(ignoreborder, (rgt * min(ignoreborder, 2)) / 128);
      int bordery = min(ignoreborder, (bot * min(ignoreborder, 2)) / 128);

      lft += borderx, rgt -= borderx;
      top += bordery, bot -= bordery;
    }

    /**/ if (TCOMPRESS_NORMAL(format)) {
      /* forward/backward-z */
      ULONG v = 0x00808000 | (TCOMPRESS_NINDEP(format) ? 0x00 : 0xFF);
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex, histo)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x] | c;

	/* black matte to forward-z */
	/**/ if (( t & 0x00FFFFFF) == 0)
	  t = (t | v);
	/* white matte to forward-z */
	else if ((~t & 0x00FFFFFF) == 0)
	  t = (t & v);

	sTex[(y * texo.Width) + x] = t;
      }
      }
    }

    else if (TCOMPRESS_GREYS(format)) {
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex, histo)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x] | c;
	ULONG a = (t >> 24) & 0xFF; /*a*/
	ULONG r = (t >> 16) & 0xFF; /*a*/
	ULONG g = (t >>  8) & 0xFF; /*a*/
	ULONG b = (t >>  0) & 0xFF; /*a*/

	g = ((r * 5) + (g * 8) + (b * 3) + 8) >> 4;
	t = (a << 24) | (g << 16) | (g << 8) | (g << 0);

	sTex[(y * texo.Width) + x] = t;
      }
      }
    }

    else if (!TCOMPRESS_SIDES(format)) {
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex, histo)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	sTex[(y * texo.Width) + x] |= c;
      }
      }
    }

#pragma omp parallel for schedule(dynamic, 4)			\
			 shared(sTex, histo)			\
			 reduction(&&: grey, blank)		\
			 reduction(|: mask4, mask5, mask6)
    for (int y = top; y < bot; y += 1) {
    for (int x = lft; x < rgt; x += 1) {
      ULONG t = sTex[(y * texo.Width) + x];
      ULONG a = (t >> 24) & 0xFF; /*a*/
      ULONG r = (t >> 16) & 0xFF; /*a*/
      ULONG g = (t >>  8) & 0xFF; /*a*/
      ULONG b = (t >>  0) & 0xFF; /*a*/
      ULONG q = ((a ^ (a >> 4)) << 24) |
		((r ^ (r >> 4)) << 16) |
		((g ^ (g >> 4)) <<  8) |
		((b ^ (b >> 4)) <<  0);
      ULONG p = ((a ^ (a >> 5)) << 24) |
		((r ^ (r >> 5)) << 16) |
		((g ^ (g >> 5)) <<  8) |
		((b ^ (b >> 5)) <<  0);
      ULONG o = ((a ^ (a >> 5)) << 24) |
		((r ^ (r >> 5)) << 16) |
		((g ^ (g >> 6)) <<  8) |
		((b ^ (b >> 5)) <<  0);

#pragma omp atomic
      histo[0][a]++;
#pragma omp atomic
      histo[1][r]++;
#pragma omp atomic
      histo[2][g]++;
#pragma omp atomic
      histo[3][b]++;

      mask4 |= q;
      mask5 |= p;
      mask6 |= o;
      if ((r != g) || (g != b))
	grey = false;
      if ((r != 0) || (g != 0) || (b != 0))
	blank = false;
    }}

    (*tex)->UnlockRect(0);

    for (unsigned int h = 0; h < 256; h += 1) {
      if (histo[0][h]) histn[0]++;
      if (histo[1][h]) histn[1]++;
      if (histo[2][h]) histn[2]++;
      if (histo[3][h]) histn[3]++;
    }

    if ((histn[0] == 1) &&
	(histn[1] == 1) &&
	(histn[2] == 1) &&
	(histn[3] == 1)) {
      addnote(" Planar image detected, collapsing to size 1x1.\n");

      return TextureCollapse(tex, D3DFMT_A8R8G8B8, TCOMPRESS_SWIZZL(format));
    }

    bool white = ((histn[0] == 1) && histo[0][0xFF]                  );
    bool black = ((histn[0] == 1) &&                   histo[0][0x00]);
    bool blawh = ((histn[0] == 2) && histo[0][0xFF] && histo[0][0x00]);

    if (!blank) {
      /* 6bit candidate */
      if (!(mask5 & 0x00070307)) {
	if (!grey) addnote(" Quantized 565 color image detected.\n");
	else       addnote(" Quantized 6 greyscale image detected.\n");

	/* check if Alpha is killable */
	if (CollapseAlpha(format, black, white)) {
	  if (ExistAlpha(format, origFormat))
	    addnote(" Automatic dropped alpha-channel.\n");

	  if (!grey)
	    return TextureQuantizeRAW<USHORT, type, format, 0>(minlevel, tex, false);
	  if (format != TCOMPRESS_L)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_L>(minlevel, tex, false);
	}
      }

      /* 5bit candidate */
      if (!(mask5 & 0x00070707)) {
	if (!grey) addnote(" Quantized 555 color image detected.\n");
	else       addnote(" Quantized 5 greyscale image detected.\n");

	/* check if Alpha is killable */
	if (CollapseAlpha(format, black, white) && grey) {
	  if (ExistAlpha(format, origFormat))
	    addnote(" Automatic dropped alpha-channel.\n");

	  if (format != TCOMPRESS_L)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_L>(minlevel, tex, false);
	}

	/* check if Alpha is 1bit */
	if (MinimalAlpha(format, black, white, blawh))
	  return TextureQuantizeRAW<USHORT, type, format, 1>(minlevel, tex, false);
      }

      /* 4bit candidate */
      if (!(mask4 & 0x0F0F0F0F)) {
	if (!grey) addnote(" Quantized 444 color image detected.\n");
	else       addnote(" Quantized 4 greyscale image detected.\n");

	if (CollapseAlpha(format, black, white))
	  if (ExistAlpha(format, origFormat))
	    addnote(" Automatic dropped alpha-channel.\n");

	if (grey) {
	  if (!D3DFMT_GREY(origFormat))
	    addnote(" Automatic greyscale conversion.\n");

	  /* check if Alpha is killable */
	  if (CollapseAlpha(format, black, white)) {
	    if (format != TCOMPRESS_L)
	      return TextureConvertRAW  <UTYPE, type, TCOMPRESS_L    >(minlevel, tex, false);
	  }
	  else
	    if (TCOMPRESS_TRANS(format))
	      return TextureQuantizeRAW<USHORT, type, TCOMPRESS_LA, 4>(minlevel, tex, false);
	    else
	      return TextureQuantizeRAW<USHORT, type, TCOMPRESS_LH, 4>(minlevel, tex, false);
	}
	else {
	  /* check if Alpha is killable */
	  if (CollapseAlpha(format, black, white))
	    return TextureQuantizeRAW<USHORT, type, format, 0>(minlevel, tex, false);

	  /* check if Alpha is 1bit */
	  if (MinimalAlpha(format, black, white, blawh))
	    return TextureQuantizeRAW<USHORT, type, format, 1>(minlevel, tex, false);

	    return TextureQuantizeRAW<USHORT, type, format, 4>(minlevel, tex, false);
	}
      }

      /* if it wasn't transparent it must be uncorrelated! */
      if (grey) {
	if (!D3DFMT_GREY(origFormat))
	  addnote(" Automatic greyscale conversion.\n");

	/* check if Alpha is killable */
	if (CollapseAlpha(format, black, white)) {
	  if (ExistAlpha(format, origFormat))
	    addnote(" Automatic dropped alpha-channel.\n");

	  if ((format != TCOMPRESS_L))
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_L >(minlevel, tex, false);
	}
	else
	  if ((format != TCOMPRESS_LA) && TCOMPRESS_TRANS(format))
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_LA>(minlevel, tex, false);
	else
	  if ((format != TCOMPRESS_LH))
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_LH>(minlevel, tex, false);
      }

      /* two colors in alpha */
      if (histn[0] <= 2) {
	/* check if Alpha is killable */
	if (CollapseAlpha(format, black, white)) {
	  /* if it wasn't transparent it must be uncorrelated! */
	  if (ExistAlpha(format, origFormat))
	    addnote(" Automatic dropped alpha-channel.\n");

	  /* "drop" the alpha-channel, 1bit alpha */
	  if (format == TCOMPRESS_RGBA)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_RGB>(minlevel, tex, false);
	  if (format == TCOMPRESS_RGBH)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_RGB>(minlevel, tex, false);
	  if (format == TCOMPRESS_xyzD)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_xyz>(minlevel, tex, false);
	  if (format == TCOMPRESS_XYZD)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_XYZ>(minlevel, tex, false);
	  if (format == TCOMPRESS_XZYD)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_XZY>(minlevel, tex, false);
	  if (format == TCOMPRESS_LA)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_L  >(minlevel, tex, false);
	  if (format == TCOMPRESS_LH)
	    return TextureConvertRAW<UTYPE, type, TCOMPRESS_L  >(minlevel, tex, false);
	}

	/* TODO: if all black and trans convert to 4x4 black L8 */
      }
    }
    else {
      /* check if Alpha is 1bit */
      if (MinimalAlpha(format, black, white, blawh))
	return TextureConvert<type>(minlevel, tex, !TCOMPRESS_NORMAL(format), D3DFMT_A1);

      if (format != TCOMPRESS_A)
	return TextureConvertRAW<UTYPE, type, TCOMPRESS_A >(minlevel, tex, false);
    }

    /* TODO: compare RMS of DXT3 vs. DXT5 and choose */
    if (verbose && ExistAlpha(format, origFormat))
      addnote(" Alpha-channel has %d distinct value(s).\n", histn[0]);
  }

  /* the lowest mip-level contains a row or a column of 4x4 blocks
   * we won't generate mip-levels for mips smaller than the BTC-area
   */
  int levels = 1;
  int ww = texo.Width;
  int hh = texo.Height;
  while ((ww > MIPMAP_MINIMUM) && (hh > MIPMAP_MINIMUM)) {
    ww = (ww + 1) >> 1;
    hh = (hh + 1) >> 1;

    levels++;
  }

  if (minlevel < 0)
    minlevel = -minlevel, levels = min(minlevel, levels);

  /* create the textures */
  assert(levels >= 0);
  switch (TCOMPRESS_CHANNELS(format)) {
    case 4: if (format == TCOMPRESS_RGBV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_RGBA) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_RGBH) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_xyzV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYZV) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL); break;
	    if (format == TCOMPRESS_xyzD) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYZD) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 3: if (format == TCOMPRESS_RGB ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_R8G8B8  , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_xyz ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_R8G8B8  , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYZ ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_R8G8B8  , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XZY ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_R8G8B8  , D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 2: if (format == TCOMPRESS_LA  ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8L8    , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_LH  ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8L8    , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XY  ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_CxV8U8  , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_XYz ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_CxV8U8  , D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 1: if (format == TCOMPRESS_A   ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_A8      , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_L   ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_L8      , D3DPOOL_SYSTEMMEM, &text, NULL);
	    if (format == TCOMPRESS_xyZ ) pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_L8      , D3DPOOL_SYSTEMMEM, &text, NULL); break;
  }

  /* damit */
  if (!text)
    return false;

  (*tex)->LockRect(0, &texs, NULL, 0);

  const int texi = (int)(*tex)->GetLevelCount();
  const int texl = (int)text->GetLevelCount();
  const int level = texl;

  /* create work-pool here, and let OMP react dynamically to the demand */
  omp_set_nested(3);
//#pragma omp parallel for if(level > 6) schedule(dynamic, 1) num_threads(2)
  for (int l = 0; l < level; l++) {
    D3DSURFACE_DESC texd;
//  D3DLOCKED_RECT texc;
    D3DLOCKED_RECT texr;

    /* square dimension of this surface-level */
    /* square area of this surface-level */
    const int lv = (1 << l);
    const int av = lv * lv;

    text->GetLevelDesc(l, &texd);
    text->LockRect(l, &texr, NULL, 0);

    ULONG *sTex = (ULONG *)texs.pBits;
    UCHAR *dTex = (UCHAR *)texr.pBits;

    /* this can not work, because the input format is always
     * A8R8G8B8 ...
     */
#if 0
    /* can the level be copied out? */
    if ((l < minlevel) && (l < texi)) {
      (*tex)->LockRect(l, &texc, NULL, 0);

      /* advance pointer of compressed blocks */
      int blocks =
	texd.Height *
	texd.Width *
	TCOMPRESS_CHANNELS(format);

      memcpy(texr.pBits, texc.pBits, blocks);

      (*tex)->UnlockRect(l);
      continue;
    }
#endif

//  logrf("level %2d/%2d\r", l + 1, level);
    /* loop over 4x4-blocks of this level (RAW) */
#pragma omp parallel for if((int)texd.Height >= (lv >> 1))	\
			 schedule(dynamic, 1)			\
			 shared(texo, texd, texr)		\
			 firstprivate(dTex)
    for (int y = 0; y < (int)texd.Height; y += 4) {
      if (!(y & 0x3F)) {
	logrf("level %2d/%2d: line %4d/%4d processed        \r", l + 1, level, y, texd.Height);

	/* problematic, the throw() inside may not block all threads ... */
//	PollProgress();
      }

      /* calculate pointer of compressed blocks */
      UCHAR *wTex = (UCHAR *)texr.pBits;

      assert(wTex == dTex);

    for (int x = 0; x < (int)texd.Width; x += 4) {
      a16 UTYPE bTex[2][4*4];
      a16 type  fTex[2][4*4][8];

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	const int yl = ((y + ly) << l);
	const int xl = ((x + lx) << l);

	/* OpenMP can't use [] in the reduction clause, provide
	 * compatible anonymous fake union for individuals
	 */
	typedef	type a16 accu[8];
	accu ts = {0},
	     tt = {0};

	/* access all pixels this level's 4x4-block represents in
	 * the full dimension original surface (high quality mip-mapping)
	 */
	/* global constant execution-time per iteration (guaranteed) */
#pragma omp parallel for if((int)texd.Height < (lv >> 1))	\
			 schedule(static, 1) ordered		\
			 shared(sTex) firstprivate(tt)
	for (int oy = 0; oy < lv; oy += 1) {
	for (int ox = 0; ox < lv; ox += 1) {
	  /* assume tiling: wrap pixels around */
	  const int posx = (xl + ox) % texo.Width;
	  const int posy = (yl + oy) % texo.Height;

	  ULONG t = sTex[(posy * texo.Width) + posx];

	  /* read ARGB -> ABGR */
//	  if (TCOMPRESS_SWIZZL(format))
//	    t = //t;
//	        (((t >> 24) & 0xFF) << 24 /*h*/)
//	      | (((t >> 16) & 0xFF) <<  0 /*r*/)
//	      | (((t >>  0) & 0xFF) <<  8 /*g*/)
//	      | (((t >>  8) & 0xFF) << 16 /*b*/);
//	  else
	    t = //t;
	        (((t >> 24) & 0xFF) << 24 /*h*/)
	      | (((t >> 16) & 0xFF) <<  0 /*r*/)
	      | (((t >>  8) & 0xFF) <<  8 /*g*/)
	      | (((t >>  0) & 0xFF) << 16 /*b*/);

	  /**/ if (TCOMPRESS_COLOR (format))
	    AccuRGBH<ACCUMODE_LINEAR>(tt, t, level, l);	// +=
	  else if (TCOMPRESS_XY  == format)
	    AccuXYCD<ACCUMODE_SCALE >(tt, t, level, l);	// +=
	  else if (TCOMPRESS_NORMAL(format))
	    AccuXYZD<ACCUMODE_SCALE >(tt, t, level, l);	// +=
	}

#pragma omp ordered
	{
	  /**/ if (TCOMPRESS_COLOR (format))
	    ReduceRGBH<ACCUMODE_LINEAR>(ts, tt);	// +=
	  else if (TCOMPRESS_XY  == format)
	    ReduceXYCD<ACCUMODE_SCALE >(ts, tt);	// +=
	  else if (TCOMPRESS_NORMAL(format))
	    ReduceXYZD<ACCUMODE_SCALE >(ts, tt);	// +=
	}
	}

	/* build average of each channel */
	/**/ if (TCOMPRESS_COLOR (format))
	  NormRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_XY  == format)
	  NormXYCD<TRGTMODE_CODING_XY     >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_NINDEP(format))
	  NormXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_NORMAL(format))
	  NormXYZD<TRGTMODE_CODING_DXDYDZt>(fTex[0][(ly * 4) + lx], ts, av);
      }

      type tr[8] = {0};

      /* analyze this level's 4x4-block */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/**/ if (TCOMPRESS_COLOR (format))
	  LookRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_XY  == format)
	  LookXYCD<TRGTMODE_CODING_XY     >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NINDEP(format))
	  LookXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NORMAL(format))
	  LookXYZD<TRGTMODE_CODING_DXDYDZt>(fTex[0][(ly * 4) + lx], tr);
      }

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/* build average of each channel an join */
	ULONG t;

	/**/ if (TCOMPRESS_COLOR (format))
	  t = JoinRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_XY  == format)
	  t = JoinXYCD<TRGTMODE_CODING_XY     >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NINDEP(format))
	  t = JoinXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NORMAL(format))
	  t = JoinXYZD<TRGTMODE_CODING_DXDYDZt>(fTex[0][(ly * 4) + lx], tr);

	/* write the result ABGR, BGR */
        switch (TCOMPRESS_CHANNELS(format)) {
          /* ABGR -> RGBA */
          case 4: bTex[0][(ly * 4) + lx] = (t) | 0x00000000; break;
          /* -BGR -> RGB- */
	  case 3: bTex[0][(ly * 4) + lx] = (t) | 0xFF000000; break;
	  /* --YX -> XY-- (signed) */
	  /* AL-- -> LA-- */
          case 2: /**/ if (format == TCOMPRESS_LA ) bTex[0][(ly * 4) + lx] = (t <<  8) & 0xFF000000,
						    bTex[1][(ly * 4) + lx] = (t <<  0) & 0xFF000000;
		  else if (format == TCOMPRESS_LH ) bTex[0][(ly * 4) + lx] = (t <<  8) & 0xFF000000,
						    bTex[1][(ly * 4) + lx] = (t <<  0) & 0xFF000000;
		  else if (format == TCOMPRESS_XY ) bTex[0][(ly * 4) + lx] = (t << 16) & 0xFF000000 - 0x80,
						    bTex[1][(ly * 4) + lx] = (t << 24) & 0xFF000000 - 0x80;
          	  else if (format == TCOMPRESS_XYz) bTex[0][(ly * 4) + lx] = (t << 16) & 0xFF000000 - 0x80,
						    bTex[1][(ly * 4) + lx] = (t << 24) & 0xFF000000 - 0x80;
          	  else                              bTex[0][(ly * 4) + lx] = (t << 16) & 0xFF000000,
						    bTex[1][(ly * 4) + lx] = (t << 24) & 0xFF000000;
		  break;
          /* -Z-- -> Z--- */
	  /* A--- -> A--- */
	  /* -LLL -> L--- */
          case 1: /**/ if (format == TCOMPRESS_A  ) bTex[0][(ly * 4) + lx] = (t <<  0) & 0xFF000000;
          	  else if (format == TCOMPRESS_xyZ) bTex[0][(ly * 4) + lx] = (t <<  8) & 0xFF000000;
          	  else                              bTex[0][(ly * 4) + lx] = (t << 24) & 0xFF000000;
          	  break;
        }
      }

      /* put this level's 4x4-block into the destination surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/* assume tiling: wrap pixels around */
	const int posx = (x + lx) % texd.Width;
	const int posy = (y + ly) % texd.Height;

	ULONG t0, t1, t2;
	switch (TCOMPRESS_CHANNELS(format)) {
	  /* ABGR -> ARGB */
	  case 4:
	    t0 = bTex[0][(ly * 4) + lx];

	    /* swizzle ARGB -> ARBG */
//	    if (TCOMPRESS_SWIZZL(format))
//	      t0 = //t;
//		  (((t0 >> 24) & 0xFF) << 24 /*h*/)
//	        | (((t0 >> 16) & 0xFF) <<  8 /*r*/)
//	        | (((t0 >>  8) & 0xFF) <<  0 /*g*/)
//	        | (((t0 >>  0) & 0xFF) << 16 /*b*/);
//	    else
	      t0 = //t;
		  (((t0 >> 24) & 0xFF) << 24 /*h*/)
	        | (((t0 >> 16) & 0xFF) <<  0 /*r*/)
	        | (((t0 >>  8) & 0xFF) <<  8 /*g*/)
	        | (((t0 >>  0) & 0xFF) << 16 /*b*/);

	    ((ULONG *)wTex)[((posy * texd.Width) + posx) * 1] = t0;
	    break;
	  /* -BGR -> -RGB */
	  case 3:
	    t0 = bTex[0][(ly * 4) + lx];

	    /* swizzle RGB -> RBG */
	    if (TCOMPRESS_SWIZZL(format)) {
	      wTex[((posy * texd.Width) + posx) * 3 + 0] = (t0 >> 16) & 0xFF;
	      wTex[((posy * texd.Width) + posx) * 3 + 2] = (t0 >>  8) & 0xFF;
	      wTex[((posy * texd.Width) + posx) * 3 + 1] = (t0 >>  0) & 0xFF;
	    }
	    else {
	      wTex[((posy * texd.Width) + posx) * 3 + 0] = (t0 >> 16) & 0xFF;
	      wTex[((posy * texd.Width) + posx) * 3 + 1] = (t0 >>  8) & 0xFF;
	      wTex[((posy * texd.Width) + posx) * 3 + 2] = (t0 >>  0) & 0xFF;
	    }
	    break;
	  /* XY-- -> YX-- (signed) */
	  /* LA-- -> AL-- */
	  case 2:
	    t1 = bTex[0][(ly * 4) + lx];
	    t2 = bTex[1][(ly * 4) + lx];

	    {
	      wTex[((posy * texd.Width) + posx) * 2 + 0] = (t1 >> 24) & 0xFF;
	      wTex[((posy * texd.Width) + posx) * 2 + 1] = (t2 >> 24) & 0xFF;
	    }
	    break;
	  /* Z--- */
	  /* A--- */
	  /* L--- */
	  case 1:
	    t0 = bTex[0][(ly * 4) + lx];

	    {
	      wTex[((posy * texd.Width) + posx) * 1 + 0] = (t0 >> 24) & 0xFF;
	    }
	    break;
	}
      }

      dTex += 0;
    }
    }

    text->UnlockRect(l);
  }


  logrf("                                                      \r");

  (*tex)->UnlockRect(0);
  (*tex)->Release();
  (*tex) = text;

  return true;
}

bool TextureConvertRGBV(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_RGBV>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_RGBV>(minlevel, base);

  return res;
}

bool TextureConvertRGBA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_RGBA>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_RGBA>(minlevel, base);

  return res;
}

bool TextureConvertRGBH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_RGBH>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_RGBH>(minlevel, base);

  return res;
}

bool TextureConvertRGB(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_RGB>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_RGB>(minlevel, base);

  return res;
}

bool TextureConvertLA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_LA>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_LA>(minlevel, base);

  return res;
}

bool TextureConvertLH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_LH>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_LH>(minlevel, base);

  return res;
}

bool TextureConvertL(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_L>(minlevel, base);
  else       res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_L>(minlevel, base);

  return res;
}

bool TextureConvertA(LPDIRECT3DTEXTURE9 *alpha, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_A>(minlevel, alpha);

  return res;
}

bool TextureConvertL(LPDIRECT3DTEXTURE9 *lumi, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_L>(minlevel, lumi);

  return res;
}

bool TextureConvertXYZV(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XYZV>(minlevel, norm);

  return res;
}

bool TextureConvert_XYZV(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_xyzV>(minlevel, norm);

  return res;
}

bool TextureConvertXYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XYZD>(minlevel, norm);

  return res;
}

bool TextureConvert_XYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_xyzD>(minlevel, norm);

  return res;
}

bool TextureConvertXY_Z(LPDIRECT3DTEXTURE9 *norm, LPDIRECT3DTEXTURE9 *z, int minlevel) {
  bool res = true;

  /* TODO: not really fast */
  (*z = *norm)->AddRef();

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XYz>(minlevel, norm);
  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_xyZ>(minlevel, z);

  return res;
}

bool TextureConvertXYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XYZ>(minlevel, norm);

  return res;
}

bool TextureConvertXZY(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XZY>(minlevel, norm);

  return res;
}

bool TextureConvert_XYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_xyz>(minlevel, norm);

  return res;
}

bool TextureConvertXY(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XY>(minlevel, norm);

  return res;
}

bool TextureConvertPM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureConvertRAW<ULONG, long , TCOMPRESS_RGBH>(minlevel, base);
  res = res && TextureConvertRAW<ULONG, float, TCOMPRESS_XYZD>(minlevel, norm);

  return res;
}

/* ------------------------------------------------------------------------------------
 */

template<typename UTYPE, typename type, int format>
static bool TextureCompressDXT(int minlevel, LPDIRECT3DTEXTURE9 *tex, bool optimize = true) {
  LPDIRECT3DTEXTURE9 text = NULL;
  D3DSURFACE_DESC texo;
  D3DLOCKED_RECT texs;

  (*tex)->GetLevelDesc(0, &texo);

#if 0
  /* Converts a height map into a normal map. The (x,y,z)
   * components of each normal are mapped to the (r,g,t)
   * channels of the output texture.
   */
  HRESULT D3DXComputeNormalMap(
    __out  LPDIRECT3DTEXTURE9 pTexture,
    __in   LPDIRECT3DTEXTURE9 pSrcTexture,
    __in   const PALETTEENTRY *pSrcPalette,
    __in   DWORD Flags,
    __in   DWORD Channel,
    __in   FLOAT Amplitude
    );
#endif

//assert((texo.Height) & 3 == 0);
//assert((texo.Width ) & 3 == 0);

  /* convert to ARGB8 (TODO: support at least the 16bit formats as well) */
  D3DFORMAT origFormat = texo.Format;
  if ((texo.Format != D3DFMT_A8R8G8B8) && !TextureConvert(texo, tex, TCOMPRESS_NORMAL(format)))
    return false;

  /* make a histogram of the alpha-channel */
  int squish__kDxtD = squish::kDxt5;
  if (optimize /*TCOMPRESS_CHANNELS(format) == 4*/) {
    unsigned int histo[4][256], histn[4] = {0,0,0,0};
    memset(&histo, 0, sizeof(histo));
    bool grey = true, blank = true;

    (*tex)->LockRect(0, &texs, NULL, 0);
    ULONG *sTex = (ULONG *)texs.pBits;

    /* exclude the border-region from checking */
    int lft = 0, rgt = (int)texo.Width;
    int top = 0, bot = (int)texo.Height;
    if (ignoreborder) {
      /* 64� -> 1px, 128� -> 2px etc. */
      int borderx = min(ignoreborder, (rgt * min(ignoreborder, 2)) / 128);
      int bordery = min(ignoreborder, (bot * min(ignoreborder, 2)) / 128);

      lft += borderx, rgt -= borderx;
      top += bordery, bot -= bordery;
    }

    /**/ if (TCOMPRESS_NORMAL(format)) {
      /* forward/backward-z */
      ULONG v = 0x00808000 | (TCOMPRESS_NINDEP(format) ? 0x00 : 0xFF);
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)		\
			 shared(sTex, histo)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x] | c;

	/* black matte to forward-z */
	/**/ if (( t & 0x00FFFFFF) == 0)
	  t = (t | v);
	/* white matte to forward-z */
	else if ((~t & 0x00FFFFFF) == 0)
	  t = (t & v);

	sTex[(y * texo.Width) + x] = t;
      }
      }
    }

    else if (TCOMPRESS_GREYS(format)) {
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)		\
			 shared(sTex, histo)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x] | c;
	ULONG a = (t >> 24) & 0xFF; /*a*/
	ULONG r = (t >> 16) & 0xFF; /*a*/
	ULONG g = (t >>  8) & 0xFF; /*a*/
	ULONG b = (t >>  0) & 0xFF; /*a*/

	g = ((r * 5) + (g * 8) + (b * 3) + 8) >> 4;
	t = (a << 24) | (g << 16) | (g << 8) | (g << 0);

	sTex[(y * texo.Width) + x] = t;
      }
      }
    }

    else if (!TCOMPRESS_SIDES(format)) {
      ULONG c =              (TCOMPRESS_SIDES (format) ? 0x00 : 0xFF) << 24;

#pragma omp parallel for schedule(dynamic, 4)		\
			 shared(sTex, histo)
      for (int y = 0; y < (int)texo.Height; y += 1) {
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	sTex[(y * texo.Width) + x] |= c;
      }
      }
    }

#pragma omp parallel for schedule(dynamic, 4)		\
			 shared(sTex, histo)		\
			 reduction(&&: grey, blank)
    for (int y = top; y < bot; y += 1) {
    for (int x = lft; x < rgt; x += 1) {
      ULONG t = sTex[(y * texo.Width) + x];
      ULONG a = (t >> 24) & 0xFF; /*a*/
      ULONG r = (t >> 16) & 0xFF; /*a*/
      ULONG g = (t >>  8) & 0xFF; /*a*/
      ULONG b = (t >>  0) & 0xFF; /*a*/

#pragma omp atomic
      histo[0][a]++;
#pragma omp atomic
      histo[1][r]++;
#pragma omp atomic
      histo[2][g]++;
#pragma omp atomic
      histo[3][b]++;

      if ((r != g) || (g != b))
	grey = false;
      if ((r != 0) || (g != 0) || (b != 0))
	blank = false;
    }}

    (*tex)->UnlockRect(0);

    for (unsigned int h = 0; h < 256; h += 1) {
      if (histo[0][h]) histn[0]++;
      if (histo[1][h]) histn[1]++;
      if (histo[2][h]) histn[2]++;
      if (histo[3][h]) histn[3]++;
    }

    if ((histn[0] == 1) &&
	(histn[1] == 1) &&
	(histn[2] == 1) &&
	(histn[3] == 1)) {
      addnote(" Planar image detected, collapsing to size 1x1.\n");

      return TextureCollapse(tex, D3DFMT_A8R8G8B8, TCOMPRESS_SWIZZL(format));
    }

    bool white = ((histn[0] == 1) && histo[0][0xFF]                  );
    bool black = ((histn[0] == 1) &&                   histo[0][0x00]);
    bool blawh = ((histn[0] == 2) && histo[0][0xFF] && histo[0][0x00]);

    /* if it wasn't transparent it must be uncorrelated! */
    if (grey) {
      if ((TCOMPRESS_CHANNELS(format) + (TCOMPRESS_GREYS(format) ? 2 : 0)) > 2)
	addnote(" Automatic greyscale conversion.\n");

      /* give the same 1:4 compression at no loss (DXT1 is 1:6) */
      /* check if Alpha is killable */
      if (CollapseAlpha(format, black, white) && (origFormat != D3DFMT_DXT1)) {
	if (ExistAlpha(format, origFormat))
	  addnote(" Automatic dropped alpha-channel.\n");

	/* these are already destroyed, no point in elevation */
	if ((origFormat != D3DFMT_DXT1) && (origFormat != D3DFMT_DXT3) && (origFormat != D3DFMT_DXT5))
	  return TextureConvertRAW<UTYPE, type, TCOMPRESS_L >(minlevel, tex, false);
	else
	  origFormat = D3DFMT_DXT1;
      }
      /* check if Color is killable */
      else if (blank) {
	if (TCOMPRESS_TRANS(format))
	  addnote(" Automatic dropped color-channels.\n");

	/* may even go down to A1 */
	return TextureConvertRAW<UTYPE, type, TCOMPRESS_A >(minlevel, tex, true);
      }
      /* gives only 1:2 or 1:4 compression (DXT1 is 1:6, other 1:4) */
      else if ((origFormat != D3DFMT_DXT1) && (origFormat != D3DFMT_DXT3) && (origFormat != D3DFMT_DXT5)) {
	/* may even go down to A4L4 */
	if (TCOMPRESS_TRANS(format))
	  return TextureConvertRAW<UTYPE, type, TCOMPRESS_LA>(minlevel, tex, true);
	else
	  return TextureConvertRAW<UTYPE, type, TCOMPRESS_LH>(minlevel, tex, true);
      }
    }

    /* two colors in alpha */
    if (histn[0] <= 2) {
      /* check if Alpha is killable */
      if (CollapseAlpha(format, black, white) && (origFormat != D3DFMT_DXT1)) {
	if (ExistAlpha(format, origFormat))
	  addnote(" Automatic dropped alpha-channel.\n");

	/* "drop" the alpha-channel, 1bit alpha */
	if (format == TCOMPRESS_RGBA)
	  return TextureCompressDXT<UTYPE, type, TCOMPRESS_RGB>(minlevel, tex, false);
	if (format == TCOMPRESS_RGBH)
	  return TextureCompressDXT<UTYPE, type, TCOMPRESS_RGB>(minlevel, tex, false);
	if (format == TCOMPRESS_XYZD)
	  return TextureCompressDXT<UTYPE, type, TCOMPRESS_XYZ>(minlevel, tex, false);
	if (format == TCOMPRESS_xyzD)
	  return TextureCompressDXT<UTYPE, type, TCOMPRESS_xyz>(minlevel, tex, false);
      }
      /* black and white?, destroy color */
      else if (MinimalAlpha(format, black, white, blawh)) {
	/* if it wasn't transparent it must be uncorrelated! */
	if (TCOMPRESS_TRANS(format))
	  origFormat = D3DFMT_DXT1;
      }
    }

    /* sixteen colors in alpha */
    if (!white && (histn[0] >= 1) && (histn[0] <= 16)) {
      int histc = 0;
      if (histo[0][0x00]) histc++; if (histo[0][0x11]) histc++;
      if (histo[0][0x22]) histc++; if (histo[0][0x33]) histc++;
      if (histo[0][0x44]) histc++; if (histo[0][0x55]) histc++;
      if (histo[0][0x66]) histc++; if (histo[0][0x77]) histc++;
      if (histo[0][0x88]) histc++; if (histo[0][0x99]) histc++;
      if (histo[0][0xAA]) histc++; if (histo[0][0xBB]) histc++;
      if (histo[0][0xCC]) histc++; if (histo[0][0xDD]) histc++;
      if (histo[0][0xEE]) histc++; if (histo[0][0xFF]) histc++;

      /* quantized only? should be extreme rare ... */
      if (histc == histn[0]) {
	if (!white && !blawh) {
	  addnote(" Quantized alpha-channel detected.\n");

	  /* go DXT3 (this can even happen if DXT5 was used
	   * with black and white end-points)
	   */
	  origFormat = D3DFMT_DXT3;
	}
      }
    }

    /* TODO: compare RMS of DXT3 vs. DXT5 and choose */
    if (verbose && ExistAlpha(format, origFormat))
      addnote(" Alpha-channel has %d distinct value(s).\n", histn[0]);
  }

  /* the lowest mip-level contains a row or a column of 4x4 blocks
   * we won't generate mip-levels for mips smaller than the BTC-area
   */
  int levels = 1;
  int ww = texo.Width;
  int hh = texo.Height;
  while ((ww > MIPMAP_MINIMUM) && (hh > MIPMAP_MINIMUM)) {
    ww = (ww + 1) >> 1;
    hh = (hh + 1) >> 1;

    levels++;
  }

  if (minlevel < 0)
    minlevel = -minlevel, levels = min(minlevel, levels);

  /* create the textures */
  int flags = 0;
  if (optimizequick)
    flags = squish::kColourRangeFit;
  else
    flags = squish::kColourIterativeClusterFit;

  assert(levels >= 0);
  switch (TCOMPRESS_CHANNELS(format) + (TCOMPRESS_GREYS(format) ? 2 : 0)) {
    case 4: if (origFormat == D3DFMT_DXT5)
	    flags |= squish::kDxt5, pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT5, D3DPOOL_SYSTEMMEM, &text, NULL); else
	    if (origFormat == D3DFMT_DXT4)
	    flags |= squish::kDxt5, pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT4, D3DPOOL_SYSTEMMEM, &text, NULL); else
	    if (origFormat == D3DFMT_DXT3)
	    flags |= squish::kDxt3, pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT3, D3DPOOL_SYSTEMMEM, &text, NULL); else
	    if (origFormat == D3DFMT_DXT2)
	    flags |= squish::kDxt3, pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT2, D3DPOOL_SYSTEMMEM, &text, NULL); else
	    if (origFormat == D3DFMT_DXT1)
	    flags |= squish::kDxt1, pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT1, D3DPOOL_SYSTEMMEM, &text, NULL); else

	    flags |= squish__kDxtD, pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT5, D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 3: flags |= squish::kDxt1; pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_DXT1, D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 2: flags |= squish::kDxt5; pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_ATI2, D3DPOOL_SYSTEMMEM, &text, NULL); break;
    case 1: flags |= squish::kDxt5; pD3DDevice->CreateTexture(texo.Width, texo.Height, levels, 0, D3DFMT_ATI1, D3DPOOL_SYSTEMMEM, &text, NULL); break;
  }

  /**/ if (TCOMPRESS_TRANS(format))
    flags |= squish::kWeightColourByAlpha;
  /**/ if (TCOMPRESS_GREYS(format))
    flags |= squish::kColourMetricUniform;
  else if (TCOMPRESS_COLOR(format))
    flags |= squish::kColourMetricPerceptual;
  else if (TCOMPRESS_NORMAL(format))
    flags |= squish::kColourMetricUniform;

  /* damit */
  if (!text)
    return false;

  /* calculate pointer of compressed blocks */
  int blocksize = 0;
  switch (TCOMPRESS_CHANNELS(format) + (TCOMPRESS_GREYS(format) ? 2 : 0)) {
    case 4: if (!(flags & squish::kDxt1)) {
	    blocksize = ((8+8) / 4); break; }/* 4x4x4 -> 16bytes */
    case 3: blocksize = (( 8 ) / 4); break;  /* 4x4x3 ->  8bytes */
    case 2: blocksize = ((4+4) / 2); break;  /* 4x4x2 -> 16bytes */
    case 1: blocksize = (( 4 ) / 2); break;  /* 4x4x1 ->  8bytes */
  }

  (*tex)->LockRect(0, &texs, NULL, 0);

  const int texi = (int)(*tex)->GetLevelCount();
  const int texl = (int)text->GetLevelCount();
  const int level = texl;

  /* create work-pool here, and let OMP react dynamically to the demand */
  omp_set_nested(3);
//#pragma omp parallel for if(level > 6) schedule(dynamic, 1) num_threads(2)
  for (int l = 0; l < level; l++) {
    D3DSURFACE_DESC texd;
//  D3DLOCKED_RECT texc;
    D3DLOCKED_RECT texr;

    /* square dimension of this surface-level */
    /* square area of this surface-level */
    const int lv = (1 << l);
    const int av = lv * lv;

    text->GetLevelDesc(l, &texd);
    text->LockRect(l, &texr, NULL, 0);

    ULONG *sTex = (ULONG *)texs.pBits;
    ULONG *dTex = (ULONG *)texr.pBits;

    /* this can not work, because the input format is always
     * A8R8G8B8 ...
     */
#if 0
    /* can the level be copied out? */
    if ((l < minlevel) && (l < texi)) {
      (*tex)->LockRect(l, &texc, NULL, 0);

      /* advance pointer of compressed blocks */
      int blocks =
	((texd.Height + 3) / 4) *
	((texd.Width  + 3) / 4);

      switch (TCOMPRESS_CHANNELS(format)) {
	case 4: blocks *= ((8+8) / 4); break; /* 4x4x4 -> 16bytes */
	case 3: blocks *= (( 8 ) / 4); break; /* 4x4x3 ->  8bytes */
	case 2: blocks *= ((4+4) / 2); break; /* 4x4x2 -> 16bytes */
	case 1: blocks *= (( 4 ) / 2); break; /* 4x4x1 ->  8bytes */
      }

      memcpy(texr.pBits, texc.pBits, blocks);

      (*tex)->UnlockRect(l);

      continue;
    }
#endif

//  logrf("level %2d/%2d\r", l + 1, level);
    /* loop over 4x4-blocks of this level (DXT5) */
#pragma omp parallel for if((int)texd.Height >= (lv >> 1))	\
			 schedule(dynamic, 1)			\
			 shared(texo, texd, texr)		\
			 firstprivate(dTex, blocksize)
    for (int y = 0; y < (int)texd.Height; y += 4) {
      if (!(y & 0x3F)) {
	logrf("level %2d/%2d: line %4d/%4d processed        \r", l + 1, level, y, texd.Height);

	/* problematic, the throw() inside may not block all threads ... */
//	PollProgress();
      }

      /* calculate pointer of compressed blocks */
      ULONG *wTex = (ULONG *)texr.pBits;
      wTex += ((y + 3) / 4) * ((texd.Width + 3) / 4) * blocksize;

//    assert(wTex == dTex);

//#pragma omp parallel if((int)texd.Height < (lv >> 1)) shared(sTex) firstprivate(wTex)
    for (int x = 0; x < (int)texd.Width; x += 4) {
      a16 UTYPE bTex[2][4*4];
      a16 type  fTex[2][4*4][8];

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	const int yl = ((y + ly) << l);
	const int xl = ((x + lx) << l);

	/* OpenMP can't use [] in the reduction clause, provide
	 * compatible anonymous fake union for individuals
	 */
	typedef	type a16 accu[8];
	accu ts = {0},
	     tt = {0};

	/* access all pixels this level's 4x4-block represents in
	 * the full dimension original surface (high quality mip-mapping)
	 */
	/* global constant execution-time per iteration (guaranteed) */
#pragma omp parallel for if((int)texd.Height < (lv >> 1))	\
			 schedule(static, 1) ordered		\
			 shared(sTex) firstprivate(tt)
//#pragma omp for schedule(static, 4)				\
//		reduction(+: ts0, ts1, ts2, ts3, ts4, ts5, ts6, ts7)
	for (int oy = 0; oy < lv; oy += 1) {
	for (int ox = 0; ox < lv; ox += 1) {
	  /* assume tiling: wrap pixels around */
	  const int posx = (xl + ox) % texo.Width;
	  const int posy = (yl + oy) % texo.Height;

	  ULONG t = sTex[(posy * texo.Width) + posx];

	  /* read ARGB -> ABGR */
//	  if (TCOMPRESS_SWIZZL(format))
//	    t = //t;
//	        (((t >> 24) & 0xFF) << 24 /*h*/)
//	      | (((t >> 16) & 0xFF) <<  0 /*r*/)
//	      | (((t >>  0) & 0xFF) <<  8 /*g*/)
//	      | (((t >>  8) & 0xFF) << 16 /*b*/);
//	  else
	    t = //t;
	        (((t >> 24) & 0xFF) << 24 /*h*/)
	      | (((t >> 16) & 0xFF) <<  0 /*r*/)
	      | (((t >>  8) & 0xFF) <<  8 /*g*/)
	      | (((t >>  0) & 0xFF) << 16 /*b*/);

	  /**/ if (TCOMPRESS_COLOR (format))
	    AccuRGBH<ACCUMODE_LINEAR>(tt, t, level, l);	// +=
	  else if (TCOMPRESS_XY  == format)
	    AccuXYCD<ACCUMODE_SCALE >(tt, t, level, l);	// +=
	  else if (TCOMPRESS_NORMAL(format))
	    AccuXYZD<ACCUMODE_SCALE >(tt, t, level, l);	// +=
	}

#pragma omp ordered
	{
	  /**/ if (TCOMPRESS_COLOR (format))
	    ReduceRGBH<ACCUMODE_LINEAR>(ts, tt);	// +=
	  else if (TCOMPRESS_XY  == format)
	    ReduceXYCD<ACCUMODE_SCALE >(ts, tt);	// +=
	  else if (TCOMPRESS_NORMAL(format))
	    ReduceXYZD<ACCUMODE_SCALE >(ts, tt);	// +=
	}
	}

	/* build average of each channel */
	/**/ if (TCOMPRESS_COLOR (format))
	  NormRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_XY  == format)
	  NormXYCD<TRGTMODE_CODING_XY     >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_NINDEP(format))
	  NormXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], ts, av);
	else if (TCOMPRESS_NORMAL(format))
	  NormXYZD<TRGTMODE_CODING_DXDYdZt>(fTex[0][(ly * 4) + lx], ts, av);
      }

      type tr[8] = {0};

      /* analyze this level's 4x4-block */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/**/ if (TCOMPRESS_COLOR (format))
	  LookRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_XY  == format)
	  LookXYCD<TRGTMODE_CODING_XY     >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NINDEP(format))
	  LookXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NORMAL(format))
	  LookXYZD<TRGTMODE_CODING_DXDYdZt>(fTex[0][(ly * 4) + lx], tr);
      }

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/* build average of each channel an join */
	ULONG t;

	/**/ if (TCOMPRESS_COLOR (format))
	  t = JoinRGBH<TRGTMODE_CODING_RGB    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_XY  == format)
	  t = JoinXYCD<TRGTMODE_CODING_XY     >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NINDEP(format))
	  t = JoinXYZD<TRGTMODE_CODING_XYZ    >(fTex[0][(ly * 4) + lx], tr);
	else if (TCOMPRESS_NORMAL(format))
	  t = JoinXYZD<TRGTMODE_CODING_DXDYdZt>(fTex[0][(ly * 4) + lx], tr);

	/* swizzle ABGR -> AGBR */
        if (TCOMPRESS_SWIZZL(format))
//	  t = (t & 0xFFFF0000) | ((t >> 8) & 0x00FF) | ((t & 0x00FF) << 8);
	  t = (t & 0xFF0000FF) | ((t >> 8) & 0xFF00) | ((t & 0xFF00) << 8);

	/* write the result ABGR, BGR */
        switch (TCOMPRESS_CHANNELS(format) + (TCOMPRESS_GREYS(format) ? 2 : 0)) {
          /* ABGR -> RGBA */
          case 4: bTex[0][(ly * 4) + lx] = (t) | 0x00000000; break;
          /* -BGR -> RGB- */
	  case 3: bTex[0][(ly * 4) + lx] = (t) | 0xFF000000; break;
	  /* --YX -> XY-- */
	  /* AL-- -> LA-- */
          case 2: /**/ if (format == TCOMPRESS_XYz) bTex[0][(ly * 4) + lx] = (t << 16) & 0xFF000000,
						    bTex[1][(ly * 4) + lx] = (t << 24) & 0xFF000000;
		  else                              bTex[0][(ly * 4) + lx] = (t <<  0) & 0xFF000000,
						    bTex[1][(ly * 4) + lx] = (t <<  8) & 0xFF000000;
		  break;
          /* -Z-- -> Z--- */
	  /* A--- -> A--- */
	  /* -LLL -> L--- */
          case 1: /**/ if (format == TCOMPRESS_A  ) bTex[0][(ly * 4) + lx] = (t <<  0) & 0xFF000000;
          	  else if (format == TCOMPRESS_xyZ) bTex[0][(ly * 4) + lx] = (t <<  8) & 0xFF000000;
          	  else                              bTex[0][(ly * 4) + lx] = (t << 24) & 0xFF000000;
          	  break;
        }
      }

      /* compress to DXT5 */
#if 0
      /**/ if (TCOMPRESS_COLOR(format))
	stb_compress_dxt_block((unsigned char *)wTex, (unsigned char *)bTex[0], true, STB_DXT_DITHER | STB_DXT_HIGHQUAL);
      else if (TCOMPRESS_NORMAL(format))
	stb_compress_dxt_block((unsigned char *)wTex, (unsigned char *)bTex[0], true, STB_DXT_NORMAL | STB_DXT_HIGHQUAL);
#else
      switch (TCOMPRESS_CHANNELS(format) + (TCOMPRESS_GREYS(format) ? 2 : 0)) {
        case 4: squish::Compress         ((unsigned char *)bTex[0], wTex + 0, flags); break;
        case 3: squish::Compress         ((unsigned char *)bTex[0], wTex + 0, flags); break;
        case 2: squish::CompressAlphaDxt5((unsigned char *)bTex[0], 0xFFFF, wTex + 0);
        	squish::CompressAlphaDxt5((unsigned char *)bTex[1], 0xFFFF, wTex + 2); break;
        case 1: squish::CompressAlphaDxt5((unsigned char *)bTex[0], 0xFFFF, wTex + 0); break;
      }
#endif

      /* advance pointer of compressed blocks */
      wTex += blocksize;
      dTex += blocksize;
    }
    }

    text->UnlockRect(l);
  }

  logrf("                                                      \r");

  (*tex)->UnlockRect(0);
  (*tex)->Release();
  (*tex) = text;

  return true;
}

bool TextureCompressRGBA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_RGBA>(minlevel, base);
  else       res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_RGBA>(minlevel, base);

  return res;
}

bool TextureCompressRGBH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_RGBH>(minlevel, base);
  else       res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_RGBH>(minlevel, base);

  return res;
}

bool TextureCompressRGB(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_RGB>(minlevel, base);
  else       res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_RGB>(minlevel, base);

  return res;
}

bool TextureCompressLA(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_LA>(minlevel, base);
  else       res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_LA>(minlevel, base);

  return res;
}

bool TextureCompressLH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_LH>(minlevel, base);
  else       res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_LH>(minlevel, base);

  return res;
}

bool TextureCompressL(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gamma) {
  bool res = true;

  if (gamma) res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_L>(minlevel, base);
  else       res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_L>(minlevel, base);

  return res;
}

bool TextureCompressA(LPDIRECT3DTEXTURE9 *base, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_RGBA>(minlevel, base);

  return res;
}

bool TextureCompressXYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_XYZD>(minlevel, norm);

  return res;
}

bool TextureCompress_XYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_xyzD>(minlevel, norm);

  return res;
}

bool TextureCompressXY_Z(LPDIRECT3DTEXTURE9 *norm, LPDIRECT3DTEXTURE9 *z, int minlevel) {
  bool res = true;

  /* TODO: not really fast */
  (*z = *norm)->AddRef();

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_XYz>(minlevel, norm);
  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_xyZ>(minlevel, z);

  return res;
}

bool TextureCompressXYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_XYZ>(minlevel, norm);

  return res;
}

bool TextureCompressXZY(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_XZY>(minlevel, norm);

  return res;
}

bool TextureCompress_XYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_xyz>(minlevel, norm);

  return res;
}

bool TextureCompressXY(LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_XY>(minlevel, norm);

  return res;
}

bool TextureCompressPM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  bool res = true;

  res = res && TextureCompressDXT<ULONG, long , TCOMPRESS_RGBH>(minlevel, base);
  res = res && TextureCompressDXT<ULONG, float, TCOMPRESS_XYZD>(minlevel, norm);

  return res;
}

/* ------------------------------------------------------------------------------------
 */

template<typename UTYPE, typename type, bool LODed>
static bool TextureCompressQDM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel) {
  LPDIRECT3DTEXTURE9 baset;
  LPDIRECT3DTEXTURE9 normt;
  D3DSURFACE_DESC based, baseo;
  D3DSURFACE_DESC normd, normo;
  D3DLOCKED_RECT baser;
  D3DLOCKED_RECT normr;
  D3DLOCKED_RECT bases;
  D3DLOCKED_RECT norms;

  (*base)->GetLevelDesc(0, &baseo);
  (*norm)->GetLevelDesc(0, &normo);

#if 0
  /* Converts a height map into a normal map. The (x,y,z)
   * components of each normal are mapped to the (r,g,b)
   * channels of the output texture.
   */
  HRESULT D3DXComputeNormalMap(
    __out  LPDIRECT3DTEXTURE9 pTexture,
    __in   LPDIRECT3DTEXTURE9 pSrcTexture,
    __in   const PALETTEENTRY *pSrcPalette,
    __in   DWORD Flags,
    __in   DWORD Channel,
    __in   FLOAT Amplitude
    );
#endif

  /* they have to have the same dimension */
  if ((baseo.Width  != normo.Width ) ||
      (baseo.Height != normo.Height))
    return false;

  /* convert to ARGB8 (TODO: support at least the 16bit formats as well) */
  if ((baseo.Format != D3DFMT_A8B8G8R8) && !TextureConvert(baseo, base, false))
    return false;
  if ((normo.Format != D3DFMT_A8B8G8R8) && !TextureConvert(normo, norm, true))
    return false;

  /* the lowest mip-level contains a row or a column of 4x4 blocks
   * we won't generate mip-levels for mips smaller than the BTC-area
   */
  int levels = 1;
  int ww = baseo.Width;
  int hh = baseo.Height;
  while ((ww > MIPMAP_MINIMUM) && (hh > MIPMAP_MINIMUM)) {
    ww = (ww + 1) >> 1;
    hh = (hh + 1) >> 1;

    levels++;
  }

  /* create the textures */
  int flags = squish::kColourIterativeClusterFit | squish::kDxt5;

  /* create the textures */
  pD3DDevice->CreateTexture(baseo.Width, baseo.Height, levels, 0, D3DFMT_DXT5, D3DPOOL_SYSTEMMEM, &baset, NULL);
  pD3DDevice->CreateTexture(normo.Width, normo.Height, levels, 0, D3DFMT_DXT5, D3DPOOL_SYSTEMMEM, &normt, NULL);

  /* damit */
  if (!baset || !normt) {
    if (baset) baset->Release();
    if (normt) normt->Release();

    return false;
  }

  (*base)->LockRect(0, &bases, NULL, 0);
  (*norm)->LockRect(0, &norms, NULL, 0);

  DWORD basel = baset->GetLevelCount();
  DWORD norml = normt->GetLevelCount();
  DWORD level = max(basel, norml);

  for (unsigned int l = 0; l < level; l++) {
    /* square dimension of this surface-level */
    /* square area of this surface-level */
    int lv = (1 << l);
    int av = lv * lv;

    baset->GetLevelDesc(l, &based);
    normt->GetLevelDesc(l, &normd);

    baset->LockRect(l, &baser, NULL, 0);
    normt->LockRect(l, &normr, NULL, 0);

    ULONG *sBase = (ULONG *)bases.pBits;
    ULONG *sNorm = (ULONG *)norms.pBits;
    ULONG *dBase = (ULONG *)baser.pBits;
    ULONG *dNorm = (ULONG *)normr.pBits;

    /* loop over 4x4-blocks of this level (DXT5) */
    for (unsigned int y = 0; y < based.Height; y += 4) {
      if (!(y & 0x3F)) {
	logrf("line processed %d/%d of level %d/%d\r", y, based.Height, l, level);

//	PollProgress();
      }

    for (unsigned int x = 0; x < based.Width; x += 4) {
      UTYPE bBase[2][4*4];
      ULONG bNorm[2][4*4];
      type  fBase[2][4*4][4];
      float fNorm[2][4*4][4];

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	type  bs[8] = {0}; int yl = ((y + ly) << l);
	long  ns[8] = {0}; int xl = ((x + lx) << l);
	float nn[8] = {0.0f};

	/* access all pixels this level's 4x4-block represents in
	 * the full dimension original surface (high quality mip-mapping)
	 */
	for (int oy = 0; oy < lv; oy += 1)
	for (int ox = 0; ox < lv; ox += 1) {
	  /* assume tiling: wrap pixels around */
	  int posx = (xl + ox) % baseo.Width;
	  int posy = (yl + oy) % baseo.Height;

	  ULONG b = sBase[(posy * baseo.Width) + posx];
	  ULONG n = sNorm[(posy * baseo.Width) + posx];

	  /* transfer heightmap into the normal-map (overwrite) */
	  if (LODed)
	    n = (n & 0x00FFFFFF) | (b & 0xFF000000);

	  /* read ARGB -> ABGR */
	  b = //b;
	      (((b >> 24) & 0xFF) << 24 /*h*/)
	    | (((b >> 16) & 0xFF) <<  0 /*r*/)
	    | (((b >>  8) & 0xFF) <<  8 /*g*/)
	    | (((b >>  0) & 0xFF) << 16 /*b*/);

	  n = //n;
	      (((n >> 24) & 0xFF) << 24 /*d*/)
	    | (((n >> 16) & 0xFF) <<  0 /*x*/)
	    | (((n >>  8) & 0xFF) <<  8 /*y*/)
	    | (((n >>  0) & 0xFF) << 16 /*z*/);

	  AccuRGBM<ACCUMODE_LINEAR>(bs, b, level, l);	// += and max
#if	defined(NORMALS_INTEGER)
	  AccuXYZD<ACCUMODE_SCALE >(ns, n, level, l);	// +=
#else
	  AccuXYZD<ACCUMODE_SCALE >(nn, n, level, l);	// +=
#endif
	}

	/* build average of each channel */
	NormRGBM<TRGTMODE_CODING_RGB                         >(fBase[0][(ly * 4) + lx], bs, av);
#if	defined(NORMALS_INTEGER)
	NormXYZD<TRGTMODE_CODING_DXDYdZt | TRGTNORM_CUBESPACE>(fNorm[0][(ly * 4) + lx], ns, av);
#else
	NormXYZD<TRGTMODE_CODING_DXDYdZt | TRGTNORM_CUBESPACE>(fNorm[0][(ly * 4) + lx], nn, av);
#endif
      }

      type  br[4] = {0};
      long  nr[4] = {0};
      float rn[4] = {0.0f};

      /* analyze this level's 4x4-block */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	LookRGBH<TRGTMODE_CODING_RGB                         >(fBase[0][(ly * 4) + lx], br);
#if	defined(NORMALS_INTEGER)
	LookXYZD<TRGTMODE_CODING_DXDYdZt | TRGTNORM_CUBESPACE>(fNorm[0][(ly * 4) + lx], nr);
#else
	LookXYZD<TRGTMODE_CODING_DXDYdZt | TRGTNORM_CUBESPACE>(fNorm[0][(ly * 4) + lx], rn);
#endif
      }

      /* generate this level's 4x4-block from the original surface */
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	/* build average of each channel an join */
	UTYPE b;
	ULONG n;

	b = JoinRGBH<TRGTMODE_CODING_RGB                         >(fBase[0][(ly * 4) + lx], br);
#if	defined(NORMALS_INTEGER)
	n = JoinXYZD<TRGTMODE_CODING_DXDYdZt | TRGTNORM_CUBESPACE>(fNorm[0][(ly * 4) + lx], nr);
#else
	n = JoinXYZD<TRGTMODE_CODING_DXDYdZt | TRGTNORM_CUBESPACE>(fNorm[0][(ly * 4) + lx], rn);
#endif

	/* write the result ABGR */
	bBase[0][(ly * 4) + lx] = b;
	bNorm[0][(ly * 4) + lx] = n;
      }

      /* compress to DXT5 */
#if 0
      stb_compress_dxt_block((unsigned char *)dBase, (unsigned char *)bBase[0], true, STB_DXT_DITHER | STB_DXT_HIGHQUAL);
      stb_compress_dxt_block((unsigned char *)dNorm, (unsigned char *)bNorm[0], true, STB_DXT_NORMAL | STB_DXT_HIGHQUAL);
#else
      squish::Compress((unsigned char *)bBase[0], dBase, flags | squish::kColourMetricPerceptual);
      squish::Compress((unsigned char *)bNorm[0], dNorm, flags | squish::kColourMetricUniform);
#endif

      /* advance pointer of compressed blocks */
      dBase += (128 / 32);
      dNorm += (128 / 32);

#if 0
      for (int ly = 0; ly < 4; ly += 1)
      for (int lx = 0; lx < 4; lx += 1) {
	dBase[((y + ly) * based.Width) + (x + lx)] = bBase[0][(ly * 4) + lx];
	dNorm[((y + ly) * based.Width) + (x + lx)] = bNorm[0][(ly * 4) + lx];
      }
#endif
    }
    }

    baset->UnlockRect(l);
    normt->UnlockRect(l);
  }

  (*base)->UnlockRect(0);
  (*norm)->UnlockRect(0);

  (*base)->Release();
  (*norm)->Release();

  (*base) = baset;
  (*norm) = normt;

  return true;
}

bool TextureCompressQDM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel, bool gamma, bool LODed) {
  bool res = true;

  if (gamma) if (LODed) res = res && TextureCompressQDM<ULONG, float, true >(base, norm, minlevel);
             else       res = res && TextureCompressQDM<ULONG, float, false>(base, norm, minlevel);
  else       if (LODed) res = res && TextureCompressQDM<ULONG, long , true >(base, norm, minlevel);
             else       res = res && TextureCompressQDM<ULONG, long , false>(base, norm, minlevel);

  return res;
}

static const struct formatID {
  D3DFORMAT fmt;
  const char *name;
  short depth;
  char channels;
  bool alpha;
} formatDatabase[] = {

  { D3DFMT_R8G8B8	      , "R8G8B8", 8, 3, false },
  { D3DFMT_A8R8G8B8           , "A8R8G8B8", 8, 4, true },
  { D3DFMT_X8R8G8B8           , "X8R8G8B8", 8, 4, false },
  { D3DFMT_R5G6B5             , "R5G6B5", 6, 3, false },
  { D3DFMT_X1R5G5B5           , "X1R5G5B5", 5, 3, false },
  { D3DFMT_A1R5G5B5           , "A1R5G5B5", 5, 4, true },
  { D3DFMT_A4R4G4B4           , "A4R4G4B4", 4, 4, true },
  { D3DFMT_R3G3B2             , "R3G3B2", 3, 3, false },
  { D3DFMT_A8                 , "A8", 8, 1, true },
  { D3DFMT_A8R3G3B2           , "A8R3G3B2", 8, 4, true },
  { D3DFMT_X4R4G4B4           , "X4R4G4B4", 4, 3, false },
  { D3DFMT_A2B10G10R10        , "A2B10G10R10", 10, 4, true },
  { D3DFMT_A8B8G8R8           , "A8B8G8R8", 8, 4, true },
  { D3DFMT_X8B8G8R8           , "X8B8G8R8", 8, 4, true },
  { D3DFMT_G16R16             , "G16R16", 16, 2, false },
  { D3DFMT_A2R10G10B10        , "A2R10G10B10", 10, 4, true },
  { D3DFMT_A16B16G16R16       , "A16B16G16R16", 16, 4, true },
  { D3DFMT_A8P8               , "A8P8", 8, 2, true },
  { D3DFMT_P8                 , "P8", 8, 1, false },
  { D3DFMT_L8                 , "L8", 8, 1, false },
  { D3DFMT_A8L8               , "A8L8", 8, 2, true },
  { D3DFMT_A4L4               , "A4L4", 4, 2, true },
  { D3DFMT_V8U8               , "V8U8", 8, 2, false },
  { D3DFMT_L6V5U5             , "L6V5U5", 6, 3, false },
  { D3DFMT_X8L8V8U8           , "X8L8V8U8", 8, 3, false },
  { D3DFMT_Q8W8V8U8           , "Q8W8V8U8", 8, 4, false },
  { D3DFMT_V16U16             , "V16U16", 16, 2, false },
  { D3DFMT_A2W10V10U10        , "A2W10V10U10", 10, 4, true },
  { D3DFMT_D16_LOCKABLE       , "D16_LOCKABLE", 16, 1, false },
  { D3DFMT_D32                , "D32", 32, 1, false },
  { D3DFMT_D15S1              , "D15S1", 15, 2, false },
  { D3DFMT_D24S8              , "D24S8", 24, 2, false },
  { D3DFMT_D24X8              , "D24X8", 24, 1, false },
  { D3DFMT_D24X4S4            , "D24X4S4", 24, 2, false },
  { D3DFMT_D16                , "D16", 16, 1, false },
  { D3DFMT_D32F_LOCKABLE      , "D32F_LOCKABLE", -23, 1, false },
  { D3DFMT_D24FS8             , "D24FS8", -23, 2, false },
  { D3DFMT_D32_LOCKABLE       , "D32_LOCKABLE", 32, 1, false },
  { D3DFMT_S8_LOCKABLE        , "S8_LOCKABLE", 8, 1, false },
  { D3DFMT_L16                , "L16", 16, 1, false },
  { D3DFMT_VERTEXDATA         , "VERTEXDATA", 0, 0, false },
  { D3DFMT_INDEX16            , "INDEX16", 16, 1, false },
  { D3DFMT_INDEX32            , "INDEX32", 32, 1, false },
  { D3DFMT_Q16W16V16U16       , "Q16W16V16U16", 16, 4, false },
  { D3DFMT_R16F               , "R16F", -10, 1, false },
  { D3DFMT_G16R16F            , "G16R16F", -10, 2, false },
  { D3DFMT_A16B16G16R16F      , "A16B16G16R16F", -10, 4, true },
  { D3DFMT_R32F               , "R32F", -23, 1, false },
  { D3DFMT_G32R32F            , "G32R32F", -23, 2, false },
  { D3DFMT_A32B32G32R32F      , "A32B32G32R32F", -23, 4, true },
  { D3DFMT_CxV8U8             , "CxV8U8", 8, 2, false },
  { D3DFMT_A1                 , "A1", 1, 1, true },
  { D3DFMT_A2B10G10R10_XR_BIAS, "A2B10G10R10_XR_BIAS", 10, 4, true },
  { D3DFMT_BINARYBUFFER       , "BINARYBUFFER", 0, 0, false },

  { D3DFMT_MULTI2_ARGB8       , "MULTI2_ARGB8", 8, 4, true },
  { D3DFMT_UYVY               , "UYVY", 8, 3, false },
  { D3DFMT_R8G8_B8G8          , "R8G8_B8G8", 8, 3, false },
  { D3DFMT_YUY2               , "YUY2", 8, 3, false },
  { D3DFMT_G8R8_G8B8          , "G8R8_G8B8", 8, 3, false },
  { D3DFMT_DXT1               , "DXT1", 8, 4, true },
  { D3DFMT_DXT2               , "DXT2", 8, 4, true },
  { D3DFMT_DXT3               , "DXT3", 8, 4, true },
  { D3DFMT_DXT4               , "DXT4", 8, 4, true },
  { D3DFMT_DXT5               , "DXT5", 8, 4, true },

  { (D3DFORMAT)MAKEFOURCC('I','N','T','Z'), "INTZ", 24, 2, false },
  { (D3DFORMAT)MAKEFOURCC('D','F','2','4'), "DF24", -23, 2, false },
  { (D3DFORMAT)MAKEFOURCC('D','F','1','6'), "DF16", -10, 2, false },
  { (D3DFORMAT)MAKEFOURCC('R','A','W','Z'), "RAWZ", -23, 2, false },

  { (D3DFORMAT)MAKEFOURCC('A','T','I','1'), "ATI1", 8, 1, false },
  { (D3DFORMAT)MAKEFOURCC('A','T','I','2'), "ATI2", 8, 2, false },

};

const char *findFormat(D3DFORMAT fmt) {
  for (int g = 0; g < (sizeof(formatDatabase) / sizeof(formatID)); g++) {
    if (formatDatabase[g].fmt == fmt)
      return formatDatabase[g].name;
  }

  return "unknown";
}

short findFormatDepth(D3DFORMAT fmt) {
  for (int g = 0; g < (sizeof(formatDatabase) / sizeof(formatID)); g++) {
    if (formatDatabase[g].fmt == fmt)
      return formatDatabase[g].depth;
  }

  return 0;
}

char findFormatChannels(D3DFORMAT fmt) {
  for (int g = 0; g < (sizeof(formatDatabase) / sizeof(formatID)); g++) {
    if (formatDatabase[g].fmt == fmt)
      return formatDatabase[g].channels;
  }

  return 0;
}

bool findAlpha(D3DFORMAT fmt) {
  for (int g = 0; g < (sizeof(formatDatabase) / sizeof(formatID)); g++) {
    if (formatDatabase[g].fmt == fmt)
      return formatDatabase[g].alpha;
  }

  return 0;
}

const char *findType(D3DRESOURCETYPE typ) {
  switch (typ) {
    case D3DRTYPE_SURFACE: return "Surface";
    case D3DRTYPE_VOLUME: return "Volume";
    case D3DRTYPE_TEXTURE: return "Planar texture";
    case D3DRTYPE_VOLUMETEXTURE: return "Volumetric texture";
    case D3DRTYPE_CUBETEXTURE: return "Cubic texture";
    case D3DRTYPE_VERTEXBUFFER: return "Vertexbuffer";
    case D3DRTYPE_INDEXBUFFER: return "Indexbuffer";
  }

  return "Unknown";
}

const char *findFileformat(D3DXIMAGE_FILEFORMAT fmt) {
  switch (fmt) {
    case D3DXIFF_BMP: return "Windows Bitmap";
    case D3DXIFF_JPG: return "JPEG";
    case D3DXIFF_TGA: return "Targa";
    case D3DXIFF_PNG: return "PNG";
    case D3DXIFF_DDS: return "DirectX texture";
    case D3DXIFF_PPM: return "NetPBM PPM";
    case D3DXIFF_DIB: return "DIB";
    case D3DXIFF_HDR: return "Radiance HDR";
    case D3DXIFF_PFM: return "NetPBM PFM";
  }

  return "Unknown";
}
