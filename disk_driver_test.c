#include <stdio.h>
#include <stdlib.h>
#include "bitmap.c"
#include <fcntl.h>

#include "disk_driver.c"

int main(){

//------------------DISK DRIVER-------------------
	printf("\n\n Sono nel test del disk_driver \n\n");

	DiskDriver* disco = (DiskDriver*)malloc (sizeof(DiskDriver));
	printf("Ho allocato\n\n");
	char* filename = "prkovddda.txt";
	printf("provo DiskDriver_init\n\n");
	DiskDriver_init(disco,filename,4096);
	printf("fine\n\n");

/*	char buffer[BLOCK_SIZE];

	for (int i = 0; i < BLOCK_SIZE; i++){
		buffer[i] = '1';
	}
	int scrivo = DiskDriver_writeBlock(disco,buffer,1);
	if(scrivo==-1){
		printf("impossibile scrivere");
		}
	int leggo = DiskDriver_readBlock(disco,buffer,1);
	if(leggo==-1){
		printf("impossibile leggere");
		}*/
return 0;
}
