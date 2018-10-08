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
	char* filename = "prova.txt";
	printf("provo DiskDriver_init\n\n");
	DiskDriver_init(disco,filename,8);


     char*buffer = malloc(sizeof(char)*BLOCK_SIZE);
    	int i;
	for ( i = 0; i < BLOCK_SIZE; i++){
		buffer[i] =1;
	}
    printf("Provo la write---------------------------------------\n\n");
	int scrivo = DiskDriver_writeBlock(disco,buffer,4);
	if(scrivo==-1){
		printf("impossibile scrivere\n\n");
		}
    printf("Provo la read-------------------------------------\n\n");
	int leggo = DiskDriver_readBlock(disco,buffer,4);
	if(leggo==-1){
		printf("impossibile leggere\n\n");
		}
        return 0;
}
