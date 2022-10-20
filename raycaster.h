#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef enum {none, camera, sphere, plane} objType; 
extern char *fileIn;
extern char *fileOut;


int readFile(char fileName[], Objects *objects);
void constructRay(float ray[], float R_o[], int pixel[],
		  int imgW, int imgH,
		  float camW, float camH);
int intersectSphere(Object *sphere, float ray[3]);
int intersectPlane(Object *plane, float ray[3]);
Object getObject(Object *objects, enum objType kind);
void writeFile(char *fileOut, int width,
	       int height, uint8_t *image);

#endif
