#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "v3math.c"


// GLOBAL VARIABLES
char *fileIn, *fileOut;  
enum objType {none, camera, sphere, plane, light};



// FUNCTION for usage help
void help() {
  printf("Usage: raycaster IMAGE_WIDTH IMAGE_HEIGHT FILE_IN FILE_OUT\n");
  exit(0);
}

void setArray(float arr[3], float a, float b, float c) {
  arr[0] = a;
  arr[1] = b;
  arr[2] = c;
}

void clamp(float arr[3]) {
  for (int i = 0; i < 3; i += 1) {
    if (arr[i] < 0) {
      arr[i] = 0;
    }
    if (arr[i] > 1) {
      arr[i] = 1;
    }
  }
}

// STRUCT for storing objects
typedef struct {
  // 0 = Not an object ( default )
  // 1 = camera
  // 2 = plane
  // 3 = sphere
  // 4 = light
  enum objType kind;
  int index;
  
  union {
    // sphere properties
    struct {
      float center[3];
      float radius;
    };
    
    // plane properties
    struct {
      float position[3];
      float n[3];
    };
    // camera properties
    struct {
      float width;
      float height;
    };
    // light properties
    struct {
      float theta;
      float angAttn0;
      float radAttn0;
      float radAttn1;
      float radAttn2;
      float location[3];
      float color[3];
      float direction[3];
    };
  };

  float diffuseColor[3];
  float specularColor[3];
  
} Object;


Object objects[128];
int objectCount = 0;
int lightCount = 0;


// FUNCTION FOR DEBUGGING ONLY ****************************
void printObj(Object *obj) {
  if (obj == NULL) {
    printf("No object.\n");
    return;
  }
  printf("Type: %i\n", obj->kind);
  /* printf("Color: %f %f %f\n", obj->color[0], obj->color[1], */
  /* 	 obj->color[2]); */
  if (obj->kind == 1) {
    printf("width: %f, height: %f\n", obj->width, obj->height);
  }
  if (obj->kind == 2) {
    printf("center: %f %f %f, radius: %f\n",
	   obj->center[0], obj->center[1],
	   obj->center[2], obj->radius);
    printf("diffuse: %f %f %f, specular: %f %f %f\n",
	   obj->diffuseColor[0], obj->diffuseColor[1], obj->diffuseColor[2],
	   obj->specularColor[0], obj->specularColor[1], obj->specularColor[2] );
  }
  if (obj->kind == 3) {
    printf("n: %f %f %f, position: %f %f %f\n", obj->n[0],
	   obj->n[1], obj->n[2], obj->position[0],
	   obj->position[1], obj->position[2]);
    printf("diffuse: %f %f %f, specular: %f %f %f\n",
	   obj->diffuseColor[0], obj->diffuseColor[1], obj->diffuseColor[2],
	   obj->specularColor[0], obj->specularColor[1], obj->specularColor[2] );
  }
  if (obj->kind == 4) {
    printf("theta: %f, radAttn: %f, %f, %f\n", obj->theta, obj->radAttn0, obj->radAttn1, obj->radAttn2);
    printf("angAttn: %f, location: %f, %f, %f\n", obj->angAttn0,
	   obj->location[0], obj->location[1], obj->location[2]);
    printf("color: %f, %f, %f, direction: %f, %f, %f\n", obj->color[0], obj->color[1], obj->color[2],
	   obj->direction[0], obj->direction[1], obj->direction[2]);
  }
}


// FUNCTION for reading CSV data
int readFile(char *fileName) {
  char maxObjects[128]; // doesn't do anything, but breaks if we remove it...

  FILE *fh;
  fh = fopen(fileName, "r");

  if(!fh) {
      printf("\nCannot locate file");
  }

  char str[20];
  int index = 0;
  
  while (!feof(fh)) {
    fscanf(fh, "%s, ", &str);
    Object currentOBJ;
    // zero out mem -> sets default variable values to 0
    memset(&currentOBJ, 0, sizeof(Object));
    currentOBJ.angAttn0 = 0;
    //printf("Object kind: %i\n", currentOBJ.kind);
    
    // CAMERA CHECK
    if (strcmp(str, "camera,") == 0) {
      currentOBJ.kind = 1;
      for (int i = 0; i < 2; i += 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "width:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.width);
	}
	if (strcmp(str, "height:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.height);
	}
      }
      printf("Read Camera\n");
    }

    // SPHERE CHECK
    if (strcmp(str, "sphere,") == 0) {
      currentOBJ.kind = 2;
      for (int j = 0; j < 4; j += 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "position:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.center[0],
		 &currentOBJ.center[1],
		 &currentOBJ.center[2]);
	}
	if (strcmp(str, "radius:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.radius);
	}
	if (strcmp(str, "diffuse_color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.diffuseColor[0],
		 &currentOBJ.diffuseColor[1],
		 &currentOBJ.diffuseColor[2]);
	}
	if (strcmp(str, "specular_color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.specularColor[0],
		 &currentOBJ.specularColor[1],
		 &currentOBJ.specularColor[2]);
	}	
      }
      printf("Read Sphere\n");
    }
    
    // PLANE CHECK
    if (strcmp(str, "plane,") == 0) {
      currentOBJ.kind = 3;
      for (int k = 0; k < 4; k += 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "position:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.position[0],
		 &currentOBJ.position[1],
		 &currentOBJ.position[2]);
	}
	if (strcmp(str, "normal:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.n[0],
		 &currentOBJ.n[1],
		 &currentOBJ.n[2]);
	}
	if (strcmp(str, "diffuse_color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.diffuseColor[0],
		 &currentOBJ.diffuseColor[1],
		 &currentOBJ.diffuseColor[2]);
	}
	if (strcmp(str, "specular_color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.specularColor[0],
		 &currentOBJ.specularColor[1],
		 &currentOBJ.specularColor[2]);
	} 
      }
      printf("Read Plane\n");
    }

    // LIGHT CHECK
    if (strcmp(str, "light,") == 0) {
      currentOBJ.kind = 4;
      for (int l = 0; l < 8; l += 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "position:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.location[0],
		 &currentOBJ.location[1],
		 &currentOBJ.location[2]);
	}
	if (strcmp(str, "direction:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.direction[0],
		 &currentOBJ.direction[1],
		 &currentOBJ.direction[2]);
	}
	if (strcmp(str, "color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.color[0],
		 &currentOBJ.color[1],
		 &currentOBJ.color[2]);
	}
	if (strcmp(str, "angular-a0:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.angAttn0);
	}
	if (strcmp(str, "radial-a0:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.radAttn0);
	}
	if (strcmp(str, "radial-a1:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.radAttn1);
	}
	if (strcmp(str, "radial-a2:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.radAttn2);
	}
	if (strcmp(str, "theta:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.theta);
	}
      }
      lightCount += 1;
      printf("Read Light\n");
    }

    printObj(&currentOBJ);
    
    objects[index] = currentOBJ;
    currentOBJ.index = index;
    index += 1;
    objectCount += 1;
    printf("objectCount: %i\n", objectCount);
  }
  
  fclose(fh);
}


Object getObject(int startIdx, enum objType kind) {
  Object current = objects[startIdx];
  while (current.kind != kind) {
    startIdx += 1;
    current = objects[startIdx];
  }
  return current;
}

void constructR_d(float R_d[], float R_o[], int pixel[],
		   int imgW, int imgH,
		   float camW, float camH ) {
  float pixW = camW / imgW;
  float pixH = camH / imgH;
  float x0 = (-camW/2 + pixW/2) + (pixW * pixel[0]);
  float y0 = (camH/2 - pixH/2) - (pixH * pixel[1]);
  float z0 = pixel[2]; // z coord of current pixel
  float pixel0[] = {x0, y0, z0};
  v3_from_points(R_d, R_o, pixel0);
  v3_normalize(R_d, R_d);
}

// FUNCTION for SPHERE intersection
float intersectSphere(Object *sphere, float R_o[3], float R_d[3]) {
  float Xd = R_d[0];
  float Yd = R_d[1];
  float Zd = R_d[2];
  int Xo = R_o[0];
  int Yo = R_o[1];
  int Zo = R_o[2];
  float Xs = sphere->center[0];
  float Ys = sphere->center[1];
  float Zs = sphere->center[2];
  float A = pow(Xd, 2) + pow(Yd, 2) + pow(Zd, 2);
  float B = 2 * ( (Xd*(Xo - Xs)) + (Yd*(Yo - Ys)) + (Zd*(Zo - Zs)) );
  float C = pow((Xo - Xs), 2) + pow((Yo - Ys), 2) + pow((Zo - Zs), 2) - pow(sphere->radius, 2);
  
  float t0 = 0;
  float t1 = 0;
  float discr = pow(B, 2) - 4*C;

  if (discr < 0) {
    return -1;
  }
  else {
    t0 = (-B - sqrt(discr)) / 2*A;
    t1 = (-B + sqrt(discr)) / 2*A;
  
      if (t0 < 0 && t1 < 0) {
	return -1;
      }
      else if (t0 < 0 || t1 <= t0) {
	  return t1;
      }
      else {
	return t0;
      }
  }

}

// FUNCTION for PLANE intersection
float intersectPlane(Object *plane, float R_o[3], float R_d[3]) {
  float P_n[3];
  setArray(P_n, plane->n[0],
	   plane->n[1], plane->n[2]);
  
  float P_o[3];
  setArray(P_o, plane->position[0],
	   plane->position[1], plane->position[2]);

  float D = -(v3_dot_product(P_n, P_o));
  float num = - ((v3_dot_product(P_n, R_o)) + D);
  float denom = v3_dot_product(P_n, R_d);
  
  float t = num / denom;

  if (t < 0) {
    return 0;
  }
  
  return t;
}

float shoot(float *R_d, float *R_o, Object *current, Object **nearestObj) {
  float t = -1;
  float nearestT = INFINITY;

  
  if (nearestObj == 0) {
    return -1;
  }
  
  //*nearestObj = NULL;
  //printf("objectCount: %i\n", objectCount);
  for (int i = 0; i < objectCount; i += 1) {
    Object *object = &objects[i];
    //printf("object in shoot at index %i:\n", i);
    //printObj(object);
    if (object == current) {
      // skip current object
    }
    //printObj(object);
    else if (object->kind == 3) {
      t = intersectPlane(object, R_o, R_d);
    }
    else if (object->kind == 2) {
      t = intersectSphere(object, R_o, R_d);
    }
    else if (object->kind == 1 || object->kind == 4) {
      // skip camera and lights
    } else {
      printf("Unknown object: %d\n", object->kind);
      printObj(object);
      exit(1);
    }
    if (t < nearestT && t > 0) {
      nearestT = t;
      *nearestObj = object;
    }
  }

  return nearestT;
}

float *illuminate(float *R_d, float *point, Object *currentOBJ){
  static float color[3];
  Object prevLight;
  prevLight.index = 0;
  float redComp = 0;
  float greenComp = 0;
  float blueComp = 0;
  
  for(int i = 0; i < lightCount; i += 1)
    {
      // Get light and properties
      Object light = getObject(prevLight.index, 4);
      float lightOrigin[3];
      setArray(lightOrigin, light.location[0],
	       light.location[1], light.location[2]);

      //printf("CHK: got light & properties\n");

      // calculate necessary vectors
      float L[3];
      v3_from_points(L, point, lightOrigin);
      float t = shoot(L, point, currentOBJ, 0);

      //printf("CHK: got necessary vectors\n");

      float Lmag = v3_length(L);
      if (t > 0 && t < Lmag) {
	break;
      }

      //printf("CHK: got and verified Lmag\n");

      // radial attenuation
      float radAttn = (1 / (light.radAttn0
			    + (light.radAttn1 * Lmag)
			    + (light.radAttn2 * pow(Lmag, 2)) )
		       );

      //printf("radAttn: %f\n", radAttn);

      // normalize light vector
      float Lnorm[3];
      v3_normalize(Lnorm, L);
      
      // angular attenuation
      float Vo[3];
      float Vl[3];
      float VoDotVl;
      setArray(Vl, light.direction[0],
	       light.direction[1], light.direction[2]);
      setArray(Vo, -Lnorm[0], -Lnorm[1], -Lnorm[2]);  ////// ERRORS somewhere near here
      VoDotVl = v3_dot_product(Vo, Vl);
      float angAttn;

      
      if ((light.direction[0] == 0 && light.direction[1] == 0 && light.direction[2] == 0) && light.theta == 0) {
	angAttn = 1;
      }
      else if (VoDotVl < cos(light.theta)) {
	angAttn = 0;
      }
      else {
	angAttn = pow(VoDotVl, light.angAttn0);
      }
      
      //printf("angAttn: %f\n", angAttn);
      
      // diffuse and specular components

      float N[3];
      if (currentOBJ->kind == 2) {
	float norm[3];
	v3_from_points(norm, currentOBJ->center, point);
	v3_normalize(norm, norm);
      	setArray(N, norm[0], norm[1], norm[2]);
      }
      else if (currentOBJ->kind == 3) {
	v3_normalize(currentOBJ->n, currentOBJ->n);
	setArray(N, currentOBJ->n[0], currentOBJ->n[1], currentOBJ->n[2]);
      }

      // get camera object:
      Object cam = getObject(0, 1);
      
      float Idiff[3];
      float Ispec[3];
      float R[3];
      float V[3];
      float camPos[3];

      setArray(camPos, 0, 0, 0);
      v3_reflect(R, Lnorm, N);
      v3_normalize(R, R);
      v3_from_points(V, point, camPos);
      v3_normalize(V, V);
      
      float NdotL = v3_dot_product(N, Lnorm);
      float VdotR = v3_dot_product(V, R);
      if (NdotL < 0) {
	setArray(Idiff, 0, 0, 0);
	if (VdotR < 0) {
	  setArray(Ispec, 0, 0, 0);
	}
      }
      else {
	setArray(Idiff,
		 currentOBJ->diffuseColor[0] * light.color[0] * NdotL,
		 currentOBJ->diffuseColor[1] * light.color[1] * NdotL,
		 currentOBJ->diffuseColor[2] * light.color[2] * NdotL );

	setArray(Ispec,
		 currentOBJ->specularColor[0] * light.color[0] * pow(VdotR, 20),
		 currentOBJ->specularColor[1] * light.color[1] * pow(VdotR, 20),
		 currentOBJ->specularColor[2] * light.color[2] * pow(VdotR, 20) );
	
      }

      


      redComp += (angAttn * radAttn * (Ispec[0] + Idiff[0]));
      greenComp += (angAttn * radAttn * (Ispec[1] + Idiff[1]));
      blueComp += (angAttn * radAttn * (Ispec[2] + Idiff[2]));
      

      //printf("currentOBJ: ");
      //printObj(currentOBJ);
      //printf("Color: %f %f %f\n", redComp, greenComp, blueComp);
      
      prevLight = light;
    }

  clamp(color);
  
  setArray(color, redComp, greenComp, blueComp);
  return color;
}

// FUNCTION for writing ppm image
void writeFile(char *fileOut, int width,
	       int height, uint8_t *image) {
  FILE *fh = fopen(fileOut, "wb");
  fprintf(fh, "P6 %d %d 255\n", width, height);
  fwrite(image, sizeof(uint8_t), width*height*3, fh);
  fclose(fh);
}


// MAIN FUNCTION
int main(int argc, char* argv[]) {
  if (argc != 5) {
    help();
  }

  //Object *objects = malloc(sizeof(Object)*128);
  
  int imageWidth = atoi(argv[1]);
  int imageHeight = atoi(argv[2]);

  fileIn = argv[3];
  fileOut = argv[4];

  uint8_t *image = malloc(imageWidth*imageHeight*3);

  readFile(fileIn);

  // get camera data
  Object camera = getObject(0, 1);
  float R_o[] = {0, 0, 0}; // origin of camera

  
  // Do intersections for every pixel (x, y)
  for (int x = 0; x < imageWidth; x += 1) {
    for (int y = 0; y < imageHeight; y += 1) {
      int z = -1;
      int pixel[] = {x, y, z};
      float R_d[3];
      constructR_d(R_d, R_o, pixel,
		   imageWidth, imageHeight,
		   camera.width, camera.height);
      v3_normalize(R_d, R_d);


      //Object *current;
      Object *nearestObj;

      float nearestT = shoot(R_d, R_o, 0, &nearestObj);
      //printf("nearestObj: \n");
      //printObj(nearestObj);

      //printf("Successfully got nearestT\n");

      //printf("nearestT: %f\n", nearestT);

      
      float point[3];
      setArray(point,
	       R_o[0] + (R_d[0] * nearestT),
	       R_o[1] + (R_d[1] * nearestT),
	       R_o[2] + (R_d[2] * nearestT)
	       );
      
      float *color;
      color = illuminate(R_d, point, nearestObj);

      //printf("Successfully illuminated point\n");
      
      for (int k = 0; k < 3; k += 1) {
	int p = 3 * (imageWidth * y + x) + k;
	if (nearestT > 0 && nearestT < INFINITY) {
	  image[p] = color[k] * 255;
	}
	else {
	  image[p] = 0;
	}
      }
      
    }
  }
  //printf("Successfully updated pixmap data \n");

  writeFile(fileOut, imageWidth, imageHeight, image);

  //free(objects);
  free(image);

  printf("Successfully Ran Program\n");
  return 0;
}
