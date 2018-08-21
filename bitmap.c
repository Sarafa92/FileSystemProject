#include <stdio.h>
#include "bitmap.h"
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

//Dato un bit dall'inizio della struttura identifico la coppia entry_num e bit



//Dato un indice del vettore ritornare il byte e il bit che lo rappresenta
BitMapEntryKey BitMap_blockToIndex(int num){
	
	BitMapEntryKey coppia;
	
	coppia.entry_num = num/8;
	coppia.bit_num = num %8;
	
	return coppia;
	
	}
	

//operazione inversa.. dato il byte e il bit dire quale è il bit nel vettore completo.
int BitMap_indexToBlock(int entry, char bit_num){
	
	//int num = bitMap.entries[ entry*8 ] |= (1 << bit_num*8);
	int bitCercato;
	
	if(entry == 0){

		 bitCercato = bit_num;
	}else{

		 bitCercato = (((entry) * 8)+ bit_num);
	}
		 return bitCercato;
	}

	//bitmap per gestire spazio libero, data una bitmap e uno start in bitmap inizia a cercare dentro la bitmap
	// un bit alla volta e si ferma al primo bit uguale a status
int BitMap_get(BitMap* bmap, int start, int status){

	int locazioneTrovata;

 // caso in cui la bitmap è null
	if(bmap==NULL)
		return -1;
 // nel caso in cui start è al di fuori della bitmap.
	if(start > bmap->num_bits || start < 0 )
		return -1;
 
	for (int i = start; i < bmap->num_bits; ++i)
	 {
		if(bmap->entries[i] == status)
			return locazioneTrovata = i;
	 }
	return -1;
}

// setta il bit all'indice pos con il bit status (0||1) 
int BitMap_set(BitMap* bmap, int pos, int status){
	
	return bmap->entries[pos]  = status;
	
	}



	
