#include <stdio.h>
#include "bitmap.h"
#include "bitmap.c"
#include <stdlib.h>

/*typedef struct{
  int num_bits;
  char* entries;
}  BitMap;

typedef struct {
  int entry_num;
  char bit_num;
} BitMapEntryKey;
*/

int main(){
	
	
printf("inizio\n\n");
	BitMap* bmap = malloc(sizeof(BitMap));
	int numBits = bmap->num_bits = 24;
	bmap->entries =  malloc(sizeof(char) * numBits );
		


	//Inizializzazione tutti i bit sono free
		for (int i = 0; i < numBits ; i++){
		bmap->entries[i] = 0;
	}
		
	
	//----SET----
	BitMap_set(bmap,5,1);
	BitMap_set(bmap,15,1);
	BitMap_set(bmap,22,1);
	BitMap_set(bmap,8,1);
	
	//---GET---
	printf("getttt\n\n");
	
	
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
	
	int trovaloc3 = BitMap_indexToBlock(2,4,bmap);
	
	printf("Dato il byte 0 e il bit 8 ottengo l'indice numero:  ");
	printf("%d",trovaloc3);
	printf("%c",'\n');
	


	return 0;
}
