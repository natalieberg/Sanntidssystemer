#include "AssignmentA.h"

void allocate(int value) {
	int *ptr = NULL;
	ptr = malloc(1024*1024*sizeof(char));
	if (ptr == NULL){
		perror("Error:" );
	}
	*ptr = value;
	printf("test of allocated memory: %i\n");
}