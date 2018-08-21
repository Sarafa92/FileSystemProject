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
	
	printf("%s", "Ciaone1");
	printf("%c",'\n');

	BitMap* bmap = malloc(sizeof(BitMap));
	int numBits = bmap->num_bits = 23;
	bmap->entries =  malloc(sizeof(char) * numBits );
		printf("%s", "Ciaone2");
		printf("%c",'\n');

	//Inizializzazione tutti i bit sono free
		for (int i = 0; i < numBits; i++){
		bmap->entries[i] = 0;
	}
		printf("%s", "Ciaone3");
		printf("%c",'\n');

	BitMap_set(bmap,5,1);
	
	int locazione = BitMap_get(bmap,0,1);
	printf("%s", "Ciaone");
	printf("%c",'\n');
	
	printf("%d", locazione);
	printf("%c",'\n');
	
	BitMapEntryKey coppia = BitMap_blockToIndex(18);
	
	int trovaloc2 = BitMap_indexToBlock(2,3);
	
	printf("%s","data locazione 15");
	printf("%c",'\n');
	
	printf("%s","byte numero   ");
	printf("%d" "%s" "%d", coppia.entry_num," bit numero ", coppia.bit_num);
	printf("%c",'\n');
	printf("%s","una sola");
	printf("%c",'\n');
	printf("%d",trovaloc2);
	printf("%c",'\n');
	

	return 0;
}
