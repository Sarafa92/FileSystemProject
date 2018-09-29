#include <stdio.h>
#include "disk_driver.c"


int main(int agc, char** argv){
	
	
	/*typedef struct{
	  int num_bits;
	  char* entries;
	}  BitMap;
	
	typedef struct {
	  int entry_num;
	  char bit_num;
	} BitMapEntryKey;
	*/

// -----------------TEST BITMAP --------------------------

printf("\n\n\n\n---------------------------------TEST BITMAP ------------------------------------------\n");
	printf("inizio\n\n");
		BitMap* bmap = malloc(sizeof(BitMap));
		int numBits;
		bmap->num_bits = 24;
		numBits = bmap->num_bits;
		bmap->entries =  malloc(sizeof(char) * numBits );
		
		int num_blocks_bmap = numBits/8;
		printf("La mia bitmap ha '%d' blocchi\n\n", num_blocks_bmap);
		


	//Inizializzazione tutti i bit sono free
		for (int i = 0; i < numBits ; i++){
		bmap->entries[i] = 0;
	}
		
	
	//----SET----
	printf("SET\n\n");
	BitMap_set(bmap,5,1);
	BitMap_set(bmap,15,1);
	BitMap_set(bmap,22,1);
	BitMap_set(bmap,8,1);
	
	//---GET---
	printf("GET\n\n");
	
	
	int locazione = BitMap_get(bmap,0,1);
	int locazione2 = BitMap_get(bmap,6,1);
	int locazione3 = BitMap_get(bmap,13,1);
	int locazione4 = BitMap_get(bmap,16,1);
	int locazione5 = BitMap_get(bmap,6,0);
	

	
	printf("\n\n");
	printf("Deve trovare 5 -----> %d", locazione);
	printf("%c",'\n');
	printf("Deve trovare 8 -----> %d", locazione2);
	printf("%c",'\n');
	printf("Deve trovare 15 -----> %d", locazione3);
	printf("%c",'\n');
	printf("Deve trovare 22 -----> %d", locazione4);
	printf("%c",'\n');
	printf("Deve trovare 6 -----> %d", locazione5);
	printf("%c",'\n');
	
	//---BLOCKTOINDEX---
	printf("---------BLOCK TO INDEX---------\n\n");
	/*int num;
	printf("\n\nBlock to index, inserisci un indice\n");
	scanf("%d", &num);*/
	
	int num= 18;
	
	BitMapEntryKey coppia = BitMap_blockToIndex(num,bmap);
	
	printf("dato l'indice %d , otteniamo il byte numero: ", num);
	printf("%d" "%s" "%d", coppia.entry_num,", bit numero: ", coppia.bit_num);
	printf("\n\n");
	
	//---INDEXTOBLOCK---
	
	printf("---------INDEX TO BLOCK---------\n\n");
	int trovaloc1 = BitMap_indexToBlock(2,7,bmap);
	
	printf("Dato il byte 2 e il bit 7 ottengo l'indice numero:  ");
	printf("%d",trovaloc1);
	printf("%c",'\n');
	
	int trovaloc2 = BitMap_indexToBlock(2,8,bmap);
	
	printf("Dato il byte 2 e il bit 8 ottengo l'indice numero:  ");
	printf("%d",trovaloc2);
	printf("%c",'\n');
	
	int trovaloc3 = BitMap_indexToBlock(0,7,bmap);
	
	printf("Dato il byte 0 e il bit 7 ottengo l'indice numero:  ");
	printf("%d",trovaloc3);
	printf("%c",'\n');


//-----------------FINE TEST BITMAP -------------------------



//------------------TEST DISK_DRIVER-------------------------

printf("\n\n\n\n----------------------------------TEST DISK DIVER --------------------------------\n");
	
	
	DiskDriver* disk = (DiskDriver*)malloc(sizeof(DiskDriver));
	//disk->header
	int num_blocks = 8;
	printf("Ho allocato\n");
	char* filename = "disk";
	char* buf[BLOCK_SIZE];
	printf("ho dato un nome a filename\n");
	

	printf("verifico init\n\n");
	DiskDriver_init(disk, filename, num_blocks);
	printf("ho inizializzato il disco\n");

		char buffer[BLOCK_SIZE];
	int i;
	for (i = 0; i < BLOCK_SIZE ; i++)
	{
		buffer[i] = '1';
	}
	
	int write = DiskDriver_writeBlock(disk,buffer,1);
	if(write ==-1) {
		printf("errore di scrittura");
		}
	
	return 0;
}
