#include <stdio.h>
#include <stdlib.h>
#include "bitmap.c"
#include "disk_driver.h"
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>





// opens the file (creating it if necessary_
// allocates the necessary space on the disk
// calculates how big the bitmap should be
// if the file was new
// compiles a disk header, and fills in the bitmap of appropriate size
// with all 0 (to denote the free space);

           
           //========================================DISKDRIVER_INIT================================================
           
void DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks){
		printf("Sono nella init\n\n");
		
		//controlli
		if(disk==NULL || filename == NULL || num_blocks<1){
			printf("Impossibile inizializzare, poichè i parametri non sono corretti\n\n");
			exit(-1);
		}
		
		int fd;
		//dimensione della bitmap in bytes
		int dimBitMap = num_blocks/8;
		if((num_blocks%8)==1){
			dimBitMap+=1;
			}
		
		size_t size = sizeof(DiskHeader)+ dimBitMap + (BLOCK_SIZE*(num_blocks-1)) ;
		printf("dimBitMap %d\n\n\n", dimBitMap);
		
		printf("size totale %zu\n\n", size);
		
		if(access( filename, F_OK ) != -1 ) {
			//file esiste
			printf("Il file esiste\n");
			fd = open(filename, O_RDWR,0666);
			  if(fd==-1){
					printf("errore apertura file");
					exit(-1);
				  }
				  //mappo l'header + bitmap 
					void* diskH = mmap(0,size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
					if(diskH == MAP_FAILED){
						close(fd);
						printf("ERRORE NELLA MAPPATURA DEL FILE1");
						exit(-1);
					}
					printf("Ciao2\n\n");
					
			//disk->fd = fd;
				disk->header = (DiskHeader*) diskH;
					disk->bitmap_data = sizeof(DiskHeader)+ (char*)diskH;
					printf("Ciao3\n\n");
		} else {
		//file non esiste
		printf("il file non esiste, lo creo!\n");
		int filed = open(filename,O_CREAT,0666);
		  if(filed==-1){
				printf("Errore Apertura file 2 \n");
				exit(-1);
			  }
			
				DiskHeader* diskH = malloc(sizeof(DiskHeader));
				
				
				//popolo il diskheader
				DiskHeader* diskHeader = (DiskHeader*) diskH;
				diskHeader->num_blocks= num_blocks;
				diskHeader->bitmap_blocks = num_blocks;
				diskHeader->bitmap_entries = dimBitMap;
				
				diskHeader->free_blocks = num_blocks;
				diskHeader->first_free_block=0;
		
				
				
				//mappo l'header e la bitmap 
				 diskH = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, filed, 0);
				if(diskH == MAP_FAILED){
					close(filed);
					printf("ERRORE NELLA MAPPATURA DEL FILE1");
					exit(-1);
				}
				
				//popolo il diskdriver
				disk->header = (DiskHeader*)diskH;
				disk->bitmap_data =sizeof(DiskHeader)+(char*)diskH;
				disk->fd = filed;
				printf("Ciao5\n\n");
				//setto a zero tutti i bit della bitmap
				memset(disk->bitmap_data,'0', dimBitMap);
				
				//close(fd)
			}
			//disk->filed =fd;
			printf("Esco dalla init\n\n");
	}
	/*//========================================DISKDRIVER_READ================================================

int DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num){
	printf("entro nella read");

	//controlli
	if(disk == NULL|| dest == NULL || block_num < 0 || block_num > disk->header->bitmap_blocks ){
		printf("Errore, impossibile leggere il blocco poichè i parametri iniziali non sono giusti!");
		return -1;
	}
	
	char* arrayBitm = disk->bitmap_data;
	
	
	//trasformo il blocco e devo andare a leggere in due indici di una BitMapEntryKey
	//individuo cosi il blocco e il bit di block_num
	BitMapEntryKey chiave = BitMap_blockToIndex(block_num,(BitMap*)arrayBitm);
	
	//devo capire se il blocco che voglio leggere è libero oppure no.
    if((arrayBitm[chiave.entry_num] >> chiave.bit_num & 1)==0){
		printf("blocco libero impossibile leggere");
		return -1;
	}
		
	//allineo il fd al blocco che devo andare a leggere (header + bitmapdim + 512*blocco da leggere
	int allineo = lseek(disk->fd, sizeof(DiskHeader) + disk->header->bitmap_entries + (BLOCK_SIZE*block_num) ,SEEK_SET);
	if(allineo ==-1){
		return -1;
	}
	//dopo che ho verificato che non è un blocco vuoto, e che ho posizionato il puntatore su primo byte del blocco da leggere 
	//posso iniziare a leggere... per far si che la lettura avvenga BLOCK_SIZE>= bytes_letti
	
	int leggo = read(disk->fd,src,BLOCK_SIZE);
	if(leggo==-1){
		printf("Errore nella lettura");
		return -1;
		}
	
	
	int val;
	int bytesLetti = 0;
	
	//se la dimensione della destinazione che deve contenere i bytes è grande almeno quanto il blocco da leggere(512bytes)
	//allora continuo con l'operazione di lettura, altrimenti ritorno -1 
	
	if(sizeof(dest)< BLOCK_SIZE){
			printf("errore il buffer per immagazzinare i byte è minore del blocco da leggere");
			return -1;
		}
		
			//finchè i bytes da leggere non sono finiti vado a scrivere le info nel buffer
			while(bytesLetti<BLOCK_SIZE) {
	
				//aggiungo i bytesletti nel buffer puntato da dest 
				val= read(disk->fd,dest + bytesLetti, BLOCK_SIZE - bytesLetti);
				
				if(val==-1){
				perror("la read è stata interrotta");
				return -1;
				
				}		
				bytesLetti += val;
		}
		printf("Esco dalla read");
	return 0;
	
}
		
			//========================================DISKDRIVER_WRITE================================================


// writes a block in position block_num, and alters the bitmap accordingly
// returns -1 if operation not possible


typedef struct{
  int num_bits;
  char* entries;
}  BitMap;

typedef struct {
  int entry_num;
  char bit_num;
} BitMapEntryKey;


int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num){
	printf("entro nella write");
	//controlli
	if(disk ==NULL|| src ==NULL || block_num < 0 || block_num > disk->header->bitmap_blocks ){
		printf("Errore, impossibile scrivere sul blocco poichè i parametri iniziali non sono giusti!");
		return -1;
	}
	char* arrayBit = disk->bitmap_data;
	
	//trasformo il blocco e devo andare a leggere in due indici di una BitMapEntryKey
	//individuo cosi il blocco e il bit di block_num
	
	BitMapEntryKey chiave = BitMap_blockToIndex(block_num, (BitMap*)arrayBit);
	
		//devo capire se il blocco su cui voglio scrivere è libero oppure no.
	
    if((arrayBit[chiave.entry_num] >> chiave.bit_num & 1)==1){
		printf("blocco pieno impossibile scrivere");
		return -1;
	}
	
	//allineo il fd al blocco su cui devo andare a scrivere
	
	int allineo = lseek(disk->fd, sizeof(DiskHeader) + disk->header->bitmap_entries + (BLOCK_SIZE*block_num),SEEK_SET);
	if(allineo ==-1){
		return -1;
	}
	
	
	int scrivo = write(disk->fd,src,BLOCK_SIZE);
	if(scrivo==-1){
		printf("Errore nella scrittura");
		return -1;
		}
	
	   int val;
	   int bscritti= 0;
	
	//finchè il mio blocco non è finito vado a leggere le info dal buffer src e le scrivo nel fd
		while(bscritti <= BLOCK_SIZE-1){
        
		val = write(disk->fd, src + bscritti, BLOCK_SIZE - bscritti);
		//se arriva una interruzione continua.
		if (val == -1){
			printf("errore di scrittura");
			return -1;
			}

		bscritti +=val;
	}
	   //quando ho scritto aggiorno la bitmap ad 1 per segnalare che ho il blocco pieno.
    int ris = BitMap_set((BitMap*) arrayBit, chiave.entry_num, 1);
    if(ris != 0){
        printf("Errore della Bitmap_set");
        return -1;
    }
	printf("esco dalla write");
	return 0;
}*/
				


	
