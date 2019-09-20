#include "bitmap.h"
#include <stdio.h>
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
	int numBits = bmap->num_bits = 40;
	int numBytes = numBits/8;
	
	if(numBytes%8 == 1){
			numBytes+=1;
		}
	
	bmap->entries =  malloc(sizeof(char) * numBytes );
		


	//Inizializzazione tutti i bit sono free
		for (int i = 0; i < numBits ; i++){
		bmap->entries[i] = 0;
	}
		
	 //---set----//
 printf("Provo la set\n");

 BitMap_set(bmap,3,1);
 BitMap_set(bmap,6,1);
 BitMap_set(bmap,7,1);
 BitMap_set(bmap,11,1);
 BitMap_set(bmap,23,1);
 BitMap_set(bmap,32,1);
 BitMap_set(bmap,33,1);
 BitMap_set(bmap,34,1);
 BitMap_set(bmap,35,1);
 BitMap_set(bmap,36,1);

 //---get----//

int a1 =  BitMap_get(bmap,0,0); //deve ritornare 0
int a2 =  BitMap_get(bmap,0,1); //deve ritornare 3
int a3 =  BitMap_get(bmap,5,1); //deve ritornare 6
int a4 =  BitMap_get(bmap,8,1); //deve ritornare 11
int a5 =  BitMap_get(bmap,16,0);//deve ritornare 16
int a6 =  BitMap_get(bmap,16,1); // deve ritornare 23
int a7 =  BitMap_get(bmap,24,1); // deve ritornare 32
int a8 =  BitMap_get(bmap,33,1); //deve ritronare 33
int a9 =  BitMap_get(bmap,34,0); //deve ritornare 37

printf("Deve tornare 0 -----> %d\n", a1);
printf("Deve tornare 3 -----> %d\n", a2);
printf("Deve tornare 6 -----> %d\n", a3);
printf("Deve tornare 11 -----> %d\n", a4);
printf("Deve tornare 16 -----> %d\n", a5);
printf("Deve tornare 23 -----> %d\n", a6);
printf("Deve tornare 32 -----> %d\n", a7);
printf("Deve tornare 33 -----> %d\n", a8);
printf("Deve tornare 37 -----> %d\n", a9);

	
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
	
	printf("Dato il byte 2 e il bit 4 ottengo l'indice numero:  ");
	printf("%d",trovaloc3);
	printf("%c",'\n');
	




//-----------------FINE TEST BITMAP -------------------------



return 0;
}
