/* Test the `vreinterpretQs8_s64' ARM Neon intrinsic.  */
/* This file was autogenerated by neon-testgen.  */

/* { dg-do assemble } */
/* { dg-require-effective-target arm_neon_ok } */
/* { dg-options "-save-temps -O0" } */
/* { dg-add-options arm_neon } */

#include "arm_neon.h"

void test_vreinterpretQs8_s64 (void)
{
  int8x16_t out_int8x16_t;
  int64x2_t arg0_int64x2_t;

  out_int8x16_t = vreinterpretq_s8_s64 (arg0_int64x2_t);
}

