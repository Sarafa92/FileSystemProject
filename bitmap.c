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
	
	//Dato un bit dall'inizio della struttura identifico la coppia entry_num e bit
	
	
	//BLOCK TO INDEX ----------------------------------------------------------------------------------------------
	//Dato un indice del vettore ritornare il byte e il bit che lo rappresenta
	
	BitMapEntryKey BitMap_blockToIndex(int num, BitMap* bm){
		
		BitMapEntryKey coppia;
		
		//se vado fuori dal range allora ritorni i valori -1
		if(num > (bm->num_bits) -1 || num < 0){
			printf(" \nErrore, valore fuori dal range ritorno -1 \n ");
			coppia.entry_num =-1;
			coppia.bit_num = -1;
		}else{
		//altrimenti assegno i valori
			coppia.entry_num = num/8;
			coppia.bit_num = num %8;
	}
		return coppia;
		
	}
		
	//INDEX TO BLOCK --------------------------------------------------------------------------------------------------
	//operazione inversa.. dato il byte e il bit dire quale è il bit nel vettore completo.
	
	int BitMap_indexToBlock(int entry, char bit_num, BitMap * bm){
		int bitCercato;
				if(entry<0 ||bit_num<0 ){
					printf("\nErrore entry o bit non validi ");
					return -1;
				}
				
				if( entry > (bm->num_bits/8)  ){
					printf("Errore blocco non valido\n\n ");
					return -1;
					}
					
				if( bit_num >= 8){
					printf("\nErrore bit_num non validi\n\n");
					return -1;
					}
		
			
					
			 bitCercato = ((((entry) * 8))+ bit_num);
		 
		
		    return bitCercato;
		}
	
		
		

		
		//BITMAP GET ---------------------------------------------------------------------------------------------------
		//bitmap per gestire spazio libero, data una bitmap e uno start in bitmap inizia a cercare dentro la bitmap
		// un bit alla volta e si ferma al primo bit uguale a status

		int BitMap_get(BitMap* bmap, int start, int status){
		
		BitMapEntryKey mappa;
		
	 // nel caso in cui start è al di fuori della bitmap.
		if(start > (bmap->num_bits)-1 || start<0){
		printf("Start non valido, fuori dal range");
			return -1;
		}
		
		//finchè start è minore della dim della bitmap... cerca a partire da start, il bit in cui lo stato è uguale 
		//a status e ritorna l'indice.
		while(start < bmap->num_bits){
		   mappa =BitMap_blockToIndex(start,bmap);
		   int entry = mappa.entry_num;
		   char num = mappa.bit_num;
			if(((bmap->entries[entry] >> num)&1) == status){ //uso lo shif per prendere i bit della bitmap.
				return BitMap_indexToBlock(entry, num, bmap);
			}else start++; //incremento lo start per andare avanti
		 }
		return -1;
	}
	
	
	//BITMAP SET ----------------------------------------------------------------------------------------------------------
	// setta il bit all'indice pos con il bit status (0||1) 
		int BitMap_set(BitMap* bmap, int pos, int status){
		//casi limite
		if(pos>bmap->num_bits || status <0 || status > 1 || pos <0){
				return -1;
		}
		
		BitMapEntryKey mappa = BitMap_blockToIndex(pos,bmap);
				int entry = mappa.entry_num;
				char num = mappa.bit_num;
				
				if(status ==1){
				 bmap->entries[entry] |= (1<<num); //imposto 1 utilizzando lo shift e l'operatore bit a bit or
				}else {
					bmap->entries[entry] &= (~(1<<num));	//imposto 0 utilizzando lo shift e l'operatore bit a bit and
				}
				return 0;
		}
