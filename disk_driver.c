


#include <errno.h>
#include <string.h>
#include "disk_driver.h"
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <strings.h>





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
		//dimensione della bitmap in bytes, per sapere quanti blocchi del disco devo allocare per la bitmap.
		int dimBitMap = (num_blocks)/8;
		if(((num_blocks)%8)!=0){
			dimBitMap+=1;
			}

       //quanti blocchi del disco servono per allocare la bitmap?
       int block_per_bitmap = ((dimBitMap)/BLOCK_SIZE);
       if((( dimBitMap)%BLOCK_SIZE)!=0){
            block_per_bitmap +=1;
           }

        //1 blocco per diskHeader e num blocchi per la bitmap
		size_t size = (1 + block_per_bitmap)*BLOCK_SIZE;
		printf("Dimensione della bitmap in byte %d\n\n\n", dimBitMap);
        printf("Quanti blocchi servono per allocare la bitmap %d\n\n\n", block_per_bitmap);
		printf("size totale %zu\n\n\n", size);



		//se il pathname esiste apri il file altrimenti crealo.
		if(access( filename, F_OK ) != -1 ) {
			//file esiste
			printf("Il file esiste\n");
			int fd = open(filename, O_RDWR,0666);
			  if(fd==-1){
					printf("Errore apertura file\n");
					exit(-1);
				  }

				  //mappo l'header + bitmap
					void* diskH = mmap(0,size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
					if(diskH == MAP_FAILED){
						close(fd);
						printf("ERRORE NELLA MAPPATURA DEL FILE1");
						exit(-1);
					}

                    disk->fd = fd;
                    disk->header = (DiskHeader*) diskH;
					disk->bitmap_data = (char*)(diskH +BLOCK_SIZE);

		} else {
		//file non esiste
		printf("il file non esiste, lo creo!\n");
		 int fd = open(filename,O_RDWR|O_CREAT|O_TRUNC,0666);
		  //int fd = creat(filename,0666);
		  if(fd==-1){
				printf("Errore Apertura file 2 \n");
				exit(-1);
			  }

                 lseek(fd, size,SEEK_SET);
                 write(fd,"",1);

				    //mappo l'header e la bitmap
                void * diskH = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if(diskH == MAP_FAILED){
					close(fd);
					printf("ERRORE NELLA MAPPATURA DEL FILE1");
					exit(-1);
				}

                printf("STO NEL FILE APPENA CREATO\n\n\n");
				//popolo il diskheader
				DiskHeader* diskHeader =  (DiskHeader*)diskH;
				diskHeader->num_blocks= num_blocks;
				diskHeader->bitmap_blocks = num_blocks;
				diskHeader->bitmap_entries = dimBitMap;
				diskHeader->free_blocks = num_blocks - 1 - block_per_bitmap; //numero dei blocchi - i blocchi allocati per diskheader e bitmap
				diskHeader->first_free_block = 1 + block_per_bitmap; //parte dal blocco dopo la bitmap

                printf("TUTTO APPOSTO\n\n\n");
				//popolo il diskdriver
				disk->header = (DiskHeader*)diskH;
				disk->bitmap_data =(char*)(diskH+BLOCK_SIZE);
				disk->fd = fd;

				printf("num_blocks:  %d \n\n",diskHeader->num_blocks);
				printf("bitmap_blocks:  %d \n\n",diskHeader->bitmap_blocks);
				printf("bitmap_entries:  %d \n\n",diskHeader->bitmap_entries);
				printf("free_blocks:  %d \n\n",diskHeader->free_blocks);
				printf("first_free_block:  %d \n\n",diskHeader->first_free_block);

			        printf("disk_header:  %p \n\n",disk->header);
				printf("bitmap_data:  %p \n\n",disk->bitmap_data);

				//setto a zero tutti i bit della bitmap
               			// memset(disk->bitmap_data,'0', dimBitMap);
              			 bzero(disk->bitmap_data,dimBitMap);

				BitMap bm;
				bm.entries = disk->bitmap_data;
				bm.num_bits = disk->header->bitmap_blocks;

				int i;
				for(i = 0; i < bm.num_bits; i++){
					BitMapEntryKey key = BitMap_blockToIndex(i,&bm);
					printf("Entry_num : %d\tBit_num : %d\tStato : %d \n\n", key.entry_num, key.bit_num , ((((bm.entries[key.entry_num]) >> (key.bit_num))&1) ));
    }

				//close(fd)

			//disk->filed =fd;
			printf("ESCO DALLA INIT-----------------------\n\n");
}
}
	//========================================DISKDRIVER_READ================================================

int DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num){
	printf("ENTRO NELLA READ---------------------------------\n");

	//controlli
	if(disk == NULL|| dest == NULL || block_num < 2 || block_num > (disk->header->bitmap_blocks)-1 ){
		printf("Errore, impossibile leggere il blocco poichè i parametri iniziali non sono giusti!\n");
		return -1;
	}

	//char* arrayBitm = disk->bitmap_data;
    BitMap bm;
	bm.num_bits=disk->header->bitmap_blocks;
	bm.entries = disk->bitmap_data;

	//trasformo il blocco e devo andare a leggere in due indici di una BitMapEntryKey
	//individuo cosi il blocco e il bit di block_num
	BitMapEntryKey chiave = BitMap_blockToIndex(block_num,&bm);

	//devo capire se il blocco che voglio leggere è libero oppure no.
    if((bm.entries[chiave.entry_num] >> chiave.bit_num & 1)==0){
		printf("blocco libero impossibile leggere\n");
		return -1;
	}

	//allineo il fd al blocco che devo andare a leggere (header + bitmapdim + 512*blocco da leggere
	int allineo = lseek(disk->fd, BLOCK_SIZE*block_num ,SEEK_SET);
	if(allineo ==-1){
		return -1;
	}
	//dopo che ho verificato che non è un blocco vuoto, e che ho posizionato il puntatore su primo byte del blocco da leggere
	//posso iniziare a leggere... per far si che la lettura avvenga BLOCK_SIZE>= bytes_letti

	int leggo = read(disk->fd,dest,BLOCK_SIZE);
	if(leggo==-1){
		printf("Errore nella lettura\n");
		return -1;
		}



		printf("ESCO DALLA READ-----------------------------\n");
	return 0;

}

			//========================================DISKDRIVER_WRITE================================================


// writes a block in position block_num, and alters the bitmap accordingly
// returns -1 if operation not possible


/*typedef struct{
  int num_bits;
  char* entries;
}  BitMap;

typedef struct {
  int entry_num;
  char bit_num;
} BitMapEntryKey;
*/

int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num){
	printf("ENTRO NELLA WRITE------------------------------\n");
	//controlli
	if(disk ==NULL|| src ==NULL || block_num < 2 || block_num > (disk->header->bitmap_blocks) -1){
		printf("Errore, impossibile scrivere sul blocco poichè i parametri iniziali non sono giusti!\n");
		return -1;
	}
	//char* arrayBit = disk->bitmap_data;

	BitMap bm;
	bm.num_bits=disk->header->bitmap_blocks;
	bm.entries = disk->bitmap_data;

	//trasformo il blocco e devo andare a leggere in due indici di una BitMapEntryKey
	//individuo cosi il blocco e il bit di block_num

	BitMapEntryKey chiave = BitMap_blockToIndex(block_num, &bm);

    //devo capire se il blocco su cui voglio scrivere è libero oppure no.
	if((((bm.entries[chiave.entry_num] >> chiave.bit_num)&1))==1){
		printf("blocco pieno impossibile scrivere\n");
		return -1;
	}

	//allineo il fd al blocco su cui devo andare a scrivere

	int allineo = lseek(disk->fd,block_num*BLOCK_SIZE,SEEK_SET);
	if(allineo ==-1){
		return -1;
	}


	int scrivo = write(disk->fd,src,BLOCK_SIZE);
	if(scrivo==-1){
		printf("Errore nella scrittura\n");
		return -1;
		}

    //quando ho scritto, aggiorno la bitmap ad 1 per segnalare che ho il blocco pieno.
    int ris = BitMap_set(&bm, block_num, 1);
    if(ris != 0){
        printf("Errore della Bitmap_set\n");
        return -1;
    }

    //decremento i blocchi liberi del disco
    disk->header->free_blocks -= 1;

    printf("Chiave.entry_num   %d\n\n", chiave.entry_num);
    printf("Chiave.bit_num    %d\n\n",chiave.bit_num);


	int i;
	for(i = 0; i < bm.num_bits; i++){
		BitMapEntryKey key = BitMap_blockToIndex(i,&bm);
		printf("Entry_num : %d\tBit_num : %d\tStato : %d \n\n", key.entry_num, key.bit_num , ((((bm.entries[key.entry_num]) >> (key.bit_num))&1) ));
    }
	printf("ESCO DALLA WRITE-------------------\n");
	return 0;
}




