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
	char* filename = "yesdsds.txt";
	printf("provo DiskDriver_init\n\n");
	DiskDriver_init(disco,filename,8);
   // int blocchi = disco->header->blocchi_riservati;


     char*buffer = malloc(sizeof(char)*BLOCK_SIZE);
    int i;
	for ( i = 0; i < BLOCK_SIZE; i++){
		buffer[i] ='1';
	}
    printf("Provo la write---------------------------------------\n\n");
	int scrivo = DiskDriver_writeBlock(disco,buffer,3);
	if(scrivo==-1){
		printf("impossibile scrivere\n\n");
		}
    printf("Provo la read---------------------------------------\n\n");
	int leggo = DiskDriver_readBlock(disco,buffer,5);
	if(leggo==-1){
		printf("impossibile leggere\n\n");
		}

    printf("Provo la free_block-----------------------------------\n");
    int free = DiskDriver_freeBlock(disco,0);
    if(free == -1){
    printf("impossibile liberare blocco");
        }

    printf("Provo la first_free_block-------------------------------\n");
    int first_free = DiskDriver_getFreeBlock(disco,0);
        printf("primo blocco libero = %d\n", first_free);

    printf("Provo la flush--------------------------------\n");
    int flush = DiskDriver_flush(disco);
    printf("flush %d\n", flush);
        return 0;
}
