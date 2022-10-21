#include <stdio.h>
#include <stdlib.h>
#include <math.h> // for sin and cos operations
#include "v3math.h"

bool v3_equals(float *a, float *b, float tolerance) {
  return (abs(a[0] - b[0]) <= tolerance &&
	  abs(a[1] - b[1]) <= tolerance &&
	  abs(a[2] - b[2]) <= tolerance );
}

void v3_from_points(float *dst, float *a, float *b) {
  dst[0] = b[0] - a[0];
  dst[1] = b[1] - a[1];
  dst[2] = b[2] - a[2];
}

void v3_add(float *dst, float *a, float *b) {
  dst[0] = a[0] + b[0];
  dst[1] = a[1] + b[1];
  dst[2] = a[2] + b[2];
}

void v3_subtract(float *dst, float *a, float *b) {
  dst[0] = a[0] - b[0];
  dst[1] = a[1] - b[1];
  dst[2] = a[2] - b[2];
}

float v3_dot_product(float *a, float *b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void v3_cross_product(float *dst, float *a, float *b) {
  float temp[] = {0, 0, 0};
  temp[0] = a[1]*b[2] - a[2]*b[1];
  temp[1] = -(a[0]*b[2] - a[2]*b[0]);
  temp[2] = a[0]*b[1] - a[1]*b[0];
  dst[0] = temp[0];
  dst[1] = temp[1];
  dst[2] = temp[2];
}

void v3_scale(float *dst, float s) {
  dst[0] = s*dst[0];
  dst[1] = s*dst[1];
  dst[2] = s*dst[2];
}

float v3_angle(float *a, float *b) {
  float argument = v3_angle_quick(a, b);
  return acos(argument);
}

float v3_angle_quick(float *a, float *b) {
  float dotProd = v3_dot_product(a, b);
  float aLen = v3_length(a);
  float bLen = v3_length(b);
  return dotProd / (aLen * bLen);
}

void v3_reflect(float *dst, float *v, float *n) {
  float dotProd = v3_dot_product(v, n);
  dotProd *= 2;
  dst[0] = v[0] - dotProd * n[0];
  dst[1] = v[1] - dotProd * n[1];
  dst[2] = v[2] - dotProd * n[2];
}

float v3_length(float *a) {
  return sqrt(pow(a[0], 2.0) +
	      pow(a[1], 2.0) +
	      pow(a[2], 2.0) );
}

void v3_normalize(float *dst, float *a) {
  float mag = v3_length(a);
  dst[0] = a[0]/mag;
  dst[1] = a[1]/mag;
  dst[2] = a[2]/mag;
}
