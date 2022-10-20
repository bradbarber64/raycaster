#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "v3math.c"


// GLOBAL VARIABLES
char *fileIn, *fileOut;
enum objType {none, camera, sphere, plane};


// FUNCTION for usage help
void help() {
  printf("Usage: raycaster IMAGE_WIDTH IMAGE_HEIGHT FILE_IN FILE_OUT\n");
  exit(0);
}

// STRUCT for storing objects
typedef struct {

  // 0 = Not an object ( default )
  // 1 = camera
  // 2 = plane
  // 3 = sphere
  enum objType kind;
  
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
  };

  float color[3];
  // add other properties later
  
} Object;


// For now, static alloc - will change to LL
// Object objects[128];

/* struct Object *objects; */
/* objects = (struct Object *)malloc(sizeof(struct Object)*1); */
// objects = realloc(objects, 2*size);


// FUNCTION for reading CSV data
int readFile(char fileName[], Object *objects) {
  char maxObjects[128];
  FILE *fh;
  
  fh = fopen(fileName, "r");

  if(!fh) {
      printf("\nCannot locate file");
  }

  char str[15];
  float f = 0;
  // Object objects[128];
  int index = 0;
  //Object currentOBJ;
  // instantiate an object called currentOBJ
  
  while (!feof(fh)) {
    printf("OBJ num: %i.\n", index);
    fscanf(fh, "%s, ", &str);
    printf("main str: %s.\n", str);
    Object currentOBJ;
    // CAMERA CHECK
    if (strcmp(str, "camera,") == 0) {
      // set type of object
      currentOBJ.kind = 1;
      for (int i = 0; i < 2; i += 1) {  // this is specific to camera (may change later)
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "width:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.width);
	}
	if (strcmp(str, "height:") == 0) {
	  fscanf(fh, "%f, ", &currentOBJ.height);
	}
      }
      printf("Checkpoint: got a camera object\n");
    }

    // SPHERE CHECK
    if (strcmp(str, "sphere,") == 0) {
      // set type of object
      currentOBJ.kind = 2;
      for (int j = 0; j < 3; j += 1) {
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
	if (strcmp(str, "color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.color[0],
		 &currentOBJ.color[1],
		 &currentOBJ.color[2]);
	}
      }

      printf("Checkpoint: got sphere object\n");
    }

    // PLANE CHECK
    if (strcmp(str, "plane,") == 0) {
      // set type of object
      currentOBJ.kind = 3;
      for (int k = 0; k < 3; k += 1) {
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
	if (strcmp(str, "color:") == 0) {
	  fscanf(fh, "[%f, %f, %f], ",
		 &currentOBJ.color[0],
		 &currentOBJ.color[1],
		 &currentOBJ.color[2]);
	}
      }

      printf("Checkpoint: got plane object\n");
    }

    objects[index] = currentOBJ;
    index += 1;
    // Store currentOBJ in LinkedList
  }
  
  fclose(fh);
}

void constructRay(float ray[], float R_o[], int pixel[],
		   int imgW, int imgH,
		   float camW, float camH ) {
  // R_o is aperture (camera origin)
  // R_d is aperture - pixel position in space
  // R_d is normalized
  // float R_o[] = [0,0,0]
  float pixW = camW / imgW;
  float pixH = camH / imgH;
  float x0 = (-camW/2 + pixW/2) + (pixW * pixel[0]);
  float y0 = (camH/2 + pixH/2) + (pixH * pixel[1]);
  float z0 = pixel[2]; // z coord of current pixel
  float pixel0[] = {x0, y0, z0};
  float R_d[3];
  v3_subtract(R_d, R_o, pixel0);
  v3_normalize(ray, R_d);
}



// FUNCTION for SPHERE intersection
int intersectSphere(Object *sphere, float ray[3]) {
  printf("sphere intersection\n");
  return 0;
}

// FUNCTION for PLANE intersection
int intersectPlane(Object *plane, float ray[3]) {
  printf("plane intersection\n");
  return 0;
}

Object getObject(Object *objects, enum objType kind) {
  int index = 0;
  Object current = objects[0];
  while (current.kind != kind) {
    index += 1;
    current = objects[index];
  }

  return current;
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

  Object *objects = malloc(sizeof(Object)*3); // 3 is for current num objects
  
  int imageWidth = atoi(argv[1]);
  int imageHeight = atoi(argv[2]);

  fileIn = argv[3];
  fileOut = argv[4];

  uint8_t *image = malloc(imageWidth*imageHeight*3);

  readFile(fileIn, objects);
  printf("Checkpoint: read scene file\n");

  // get camera data
  Object camera = getObject(objects, 1);
  float R_o[] = {0, 0, 0};

  int z = -1;
  // Do intersections for every pixel (x, y)
  for (int x = 0; x < imageWidth; x += 1) {
    for (int y = 0; y < imageHeight; y += 1) {
      //constructRay(float ray[], float R_o[], int pixel[], int imgW, int imgH,float camW, float camH);
      int pixel[] = {x, y, z};
      float ray[] = {0, 0, 0};
      constructRay(ray, R_o, pixel,
		   imageWidth, imageHeight,
		   camera.width, camera.height);

      int nearestT = 0;
      Object nearestObj;
      for (int i = 0; i < 3; i += 1) {
	Object *current = &objects[i];
	//printf("current obj: %i.\n", current->kind);
	if (current->kind == 3) {
	  int t = intersectPlane(current, ray);
	}
	if (current->kind == 2) {
	  int t = intersectSphere(current, ray);
	}
	// set nearestObj from nearestT
	// get color from nearestObj
	// image[] = nearestObject->color;
      }
      
    }
  }

  writeFile(fileOut, imageWidth, imageHeight, image);

  free(objects);
  free(image);
  
  return 0;
}

