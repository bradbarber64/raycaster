#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "v3math.c"


// GLOBAL VARIABLES
char *fileIn, *fileOut;
enum objType {none, camera, plane, sphere};


// FUNCTION for usage help
void help() {
  printf("Usage: raycaster IMAGE_WIDTH IMAGE_HEIGHT FILE_IN FILE_OUT\n");
  exit(0);
}

// STRUCT for storing objects
typedef struct Object {

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

Object *objects;
size = sizeof(Object);
objects = malloc(1*size);
objects = realloc(objects, 2*size);


// FUNCTION for reading CSV data
int readFile(char fileName[]) {
  char maxObjects[128];
  FILE *fh;
  
  fh = fopen(fileName, "r");

  if(!fh) {
      printf("\nCannot locate file");
  }

  char str[15];
  Object objects[128];
  int index = 0;
  Object currentOBJ = objects[index];
  // instantiate an object called currentOBJ
  
  while (!feof(fh)) {
    fscanf(fh, "%s ", &str);
    
    // CAMERA CHECK
    if (strcmp(str, "camera, ") == 1) {
      // set type of object
      currentOBJ.kind = 1;
      while (strcmp(str, "\n") != 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "width: ") == 1) {
	  fscanf(fh, "%f ", currentOBJ.width);
	}
	if (strcmp(str, "height: ") == 1) {
	  fscanf(fh, "%f ", currentOBJ.height);
	}
      }
    }

    // PLANE CHECK
    if (strcmp(str, "plane, ") == 1) {
      // set type of object
      while (strcmp(str, "\n") != 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "position: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 currentOBJ.position[0],
		 currentOBJ.position[1],
		 currentOBJ.position[2]);
	}
	if (strcmp(str, "radius: ") == 1) {
	  fscanf(fh, "%f ", currentOBJ.radius);
	}
	if (strcmp(str, "color: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 currentOBJ.color[0],
		 currentOBJ.color[1],
		 currentOBJ.color[2]);
	}
      }
    }

    // SPHERE CHECK
    if (strcmp(str, "sphere, ") == 1) {
      // set type of object
      while (strcmp(str, "\n") != 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "position: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 currentOBJ.center[0],
		 currentOBJ.center[1],
		 currentOBJ.center[2]);
	}
	if (strcmp(str, "normal: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 currentOBJ.n[0],
		 currentOBJ.n[1],
		 currentOBJ.n[2]);
	}
	if (strcmp(str, "color: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 currentOBJ.color[0],
		 currentOBJ.color[1],
		 currentOBJ.color[2]);
	}
      }
    }

    // Store currentOBJ in LinkedList
  }
  
  fclose(fh);
}

float constructRay(float R_o[], int x, int y, int z) {
  // R_o is aperture (camera origin)
  // R_d is aperture - pixel position in space
  // R_d is normalized
  // float R_o[] = [0,0,0]
  float pixel[] = {x, y, z};
  float R_d[3];
  v3_subtract(R_d, R_o, pixel);

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

  int imageWidth = atoi(argv[1]);
  int imageHeight = atoi(argv[2]);

  fileIn = argv[3];
  fileOut = argv[4];

  uint8_t *image = malloc(imageWidth*imageHeight*3);

  // either set LL here and pass in, or set LL in readFile
  
  readFile(fileIn);

  // RECOMMENDED:
  // get project working with statically allocated mem,
  // then get fancy

  
  Object *objects;
  // Do intersections for every pixel (x, y)
  for (int x = 0; x < imageWidth; x += 1) {
    for (int y = 0; y < imageHeight; y += 1) {
      // construct ray R(t) = R_o + t * R_d
      float ray[] = {0, 0, 0};

      int nearestT = 0;
      Object nearestObj;
      for (int i = 0; i < 2; i += 1) {
	Object *current = &objects[i];
	if (plane) {
	  int t = intersectPlane(current, ray);
	}
	if (sphere) {
	  int t = intersectSphere(current, ray);
	}
	// set nearestObj from nearestT
	// get color from nearestObj
	// image[] = nearestObject->color;
      }
      
    }
  }

  writeFile(fileOut, imageWidth, imageHeight, image);

  free(image);
  
  return 0;
}

