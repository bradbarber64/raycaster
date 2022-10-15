#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "v3math.c"

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
  int kind; // flag for what kind of object
  char name[10];
  
  union {
    // sphere properties
    struct {
      float position[3];
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
    }
  };
  float color[3];
  // add other properties later
  
} Object;


// For now, static alloc - will change to LL
Object objects[128];


// FUNCTION for reading CSV data
int readFile(fileName) {
  char maxLines[128];
  FILE *fh;
  
  fh = fopen(fileName, "r");

  if(!fh) {
      printf("\nCannot locate file");
  }

  char str[15];

  // instantiate an object called currentOBJ
  
  while (!FEOF) {
    fscanf(fh, "%s ", &str);
    
    // CAMERA CHECK
    if (strcmp(str, "camera, ") == 1) {
      // set type of object
      while (strcmp(str, "\n") != 1) {
	fscanf(fh, "%s ", &str);
	if (strcmp(str, "width: ") == 1) {
	  fscanf(fh, "%f ", &currentOBJ->width);
	}
	if (strcmp(str, "height: ") == 1) {
	  fscanf(fh, "%f ", &currentOBJ->height);
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
		 &currentOBJ->pos[0],
		 &currentOBJ->pos[1],
		 &currentOBJ->pos[2]);
	}
	if (strcmp(str, "radius: ") == 1) {
	  fscanf(fh, "%f ", &currentOBJ->radius);
	}
	if (strcmp(str, "color: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 &currentOBJ->color[0],
		 &currentOBJ->color[1],
		 &currentOBJ->color[2]);
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
		 &currentOBJ->pos[0],
		 &currentOBJ->pos[1],
		 &currentOBJ->pos[2]);
	}
	if (strcmp(str, "normal: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 &currentOBJ->norm[0],
		 &currentOBJ->norm[1],
		 &currentOBJ->norm[2]);
	}
	if (strcmp(str, "color: ") == 1) {
	  fscanf(fh, "[%f, %f, %f]",
		 &currentOBJ->color[0],
		 &currentOBJ->color[1],
		 &currentOBJ->color[2]);
	}
      }
    }

    // Store currentOBJ in LinkedList
  }
  
  
}


// FUNCTION for SPHERE intersection
int intersectSphere(Object sphere) {
  printf("sphere intersection\n");
  return 0;
}

// FUNCTION for PLANE intersection
int intersectPlane(Object plane) {
  printf("plane intersection\n");
  return 0;
}



// FUNCTION for writing ppm image
void writeFile(fileOut) {
  printf("hi\n");
}


// MAIN FUNCTION
int main(int argc, char **argv) {
  if (argc != 5) {
    help();
  }

  int imageWidth = atoi(argv[1]);
  int imageHeight = atoi(argv[2]);

  char fileIn[] = argv[3];
  char fileOut[] = argv[4];

  uint8_t *image = malloc(imageWidth*imageHeight*3);

  // either set LL here and pass in, or set LL in readFile
  
  readFile(fileIn);

  Object *objects;
  // Do intersections for every pixel (x, y)
  for (int x = 0; x < imageWidth; x += 1) {
    for (int y = 0; y < imageHeight; y += 1) {
      // construct ray

      int nearestT = 0;
      Object nearestObj;
      for (int i = 0; i < 128; i += 1) {
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

  // writeFile(fileOut);
  
  return 0;
}

