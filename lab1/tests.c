/* Testing Code */

#include <limits.h>
/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
int test_addOK(int x, int y)
{
  int sum = x+y;
  return !(x < 0 && y < 0 && sum >= 0) && !(x > 0 && y > 0 && sum <= 0);
}
int test_bang(int x)
{
  return !x;
}
int test_abs(int x) {
  return (x < 0) ? -x : x;
}
int test_bitOr(int x, int y)
{
  return x|y;
}
int test_copyLSB(int x)
{
  return (x & 0x1) ? -1 : 0;
}
int test_divpwr2(int x, int n)
{
    int p2n = 1<<n;
    return x/p2n;
}
int test_getByte(int x, int n)
{
  union {
    int word;
    unsigned char bytes[4];
  } u;
  int test = 1;
  int littleEndian = (int) *(char *) &test;
  u.word = x;
  return littleEndian ? (unsigned) u.bytes[n] : (unsigned) u.bytes[3-n];
}
int test_isGreater(int x, int y)
{
  return x > y;
}
int test_isLessOrEqual(int x, int y)
{
  return x <= y;
}
int test_isNonZero(int x)
{
  return x!=0;
}
int test_isPositive(int x) {
  return x > 0;
}
int test_isPower2(int x) {
  int i;
  for (i = 0; i < 31; i++) {
    if (x == 1<<i)
      return 1;
  }
  return 0;
}
int test_negate(int x) {
  return -x;
}
int test_satAdd(int x, int y)
{
  if (x > 0 && y > 0 && x+y < 0)
    return LONG_MAX;
  if (x < 0 && y < 0 && x+y >= 0)
    return LONG_MIN;
  return x + y;
}
int test_tc2sm(int x) {
  int sign = x < 0;
  int mag = x < 0 ? -x : x;
  return (sign << 31) | mag;
}
