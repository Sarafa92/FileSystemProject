#include "disk_driver.h"
#include "bitmap.h"
#include "bitmap.c"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


/*
#define BLOCK_SIZE 512
// this is stored in the 1st block of the disk
typedef struct {
  int num_blocks;  
  int bitmap_blocks;   // how many blocks in the bitmap
  int bitmap_entries;  // how many bytes are needed to store the bitmap
  
  int free_blocks;     // free blocks
  int first_free_block;// first block index
} DiskHeader; 

typedef struct {
  DiskHeader* header; // mmapped
  char* bitmap_data;  // mmapped (bitmap)
  int fd; // for us
} DiskDriver;


*/
//--------------------------------------------------------------------------

// opens the file (creating it if necessary_
// allocates the necessary space on the disk
// calculates how big the bitmap should be
// if the file was new
// compiles a disk header, and fills in the bitmap of appropriate size
// with all 0 (to denote the free space);
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
           
	
           
           //========================================DISKDRIVER_INIT================================================
           
void DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks){
	
		//controlli
		if(disk==NULL || filename == NULL || num_blocks<1){
			handle_error("Impossibile inizializzare, poichè i parametri non sono corretti");
			exit(-1);
		}
		
		
		
		//filedescriptor
		int fd;
		
		//Dimensione della bitmap( numero di blocchi del disco/8)
		
		int dimBitMap = num_blocks/8;
		
		//int dimDisco = BLOCK_SIZE * num_blocks;
		
		//int blocchibitmap = dimDisco/BLOCK_SIZE;
		
		int size = sizeof(DiskHeader)+ dimBitMap;
	
	
		
		
		/* //se il descrittore torna -1 e errno mi dice EEXIST significa che il file è già esistente quindi posso solo aprirlo.
		 
		 
		 if ((fd == -1) && (EEXIST == errno)){ 
			 fd = open(fd, O_RDWR);
		  }*/
		  
		   //verifico che il pathname sia valido, se non è valido significa che il file non esiste, quindi devo crearlo..
		   //altrimenti esiste e devo solo aprirlo          
		   //La funzione access() ritorna 0 nel caso che tutti i permessi richiesti siano verificati con successo;
		   // -1 nel caso che almeno un permesso sia negato.
		   
		 
		 //-------------FILE ESISTE-----------
		 
		  if(access(filename, F_OK) == 0){
			  
				//IL FILE ESISTE E LO APRO in lettura e scrittura
				fd = open(filename,O_RDWR,0666);
				if( fd==-1 ) {
					perror("Errore in apertura del file1");
					exit(-1);
			    }
			//	disk = (DiskDriver*) malloc(size);
			   //mappo il file 
				void* diskH =  (DiskHeader*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if(diskH == MAP_FAILED){
					close(fd);
					handle_error("ERRORE NELLA MAPPATURA DEL FILE1");
					exit(-1);
				}
				
				//disk->fd = fd;
				disk->header = (DiskHeader*) diskH;
				disk->bitmap_data = sizeof(DiskHeader)+ (char*)diskH;
				
				//close(fd);
			    
			 //--------------FILE NON ESISTE----------------
			}else{
					//IL FILE NON ESISTE E VIENE QUINDI CREATO
					printf("Il file NON esiste....creo file");
					// creo il file,  O_EXCL non serve poichè già sopra faccio il controllo se il file è esistente.
					//int fd = creat(filename, 0644);
					fd = open(filename, O_RDWR|O_CREAT,0666);
					if( fd==-1 ) {
						perror("Errore in apertura del file2");
						exit(-1);	 
					}
					
				
				
				//mappo il file 
				void* diskH =  (DiskHeader*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if(diskH == MAP_FAILED){
					close(fd);
					handle_error("ERRORE NELLA MAPPATURA DEL FILE2");
					exit(-1);
				}
			
			
				//popolo il diskheader
				DiskHeader* diskHeader = (DiskHeader*) diskH;
				diskHeader->num_blocks= num_blocks;
				diskHeader->bitmap_blocks = num_blocks;
				diskHeader->bitmap_entries = dimBitMap;
				
				diskHeader->free_blocks = num_blocks;
				diskHeader->first_free_block=0;
		
				//popolo il diskdriver
				disk->header = (DiskHeader*) diskH;
				disk->bitmap_data = sizeof(DiskHeader)+ (char*)diskH;
				disk->fd = fd;
				
				
				//setto a zero tutti i bit della bitmap, denotando cosi lo spaz
				memset(disk->bitmap_data,'0', dimBitMap);
				
				//close(fd)
			}
	}
	
	// reads the block in position block_num
// returns -1 if the block is free accrding to the bitmap
// 0 otherwise



/*typedef struct{
  int num_bits;
  char* entries;
}  BitMap;

typedef struct {
  int entry_num;
  char bit_num;
} BitMapEntryKey;*/



 //========================================DISKDRIVER_READ================================================

int DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num){
	
	
	//controlli
	if(disk == NULL|| dest == NULL || block_num < 0 || block_num > disk->header->bitmap_blocks ){
		handle_error("Errore, impossibile leggere il blocco poichè i parametri iniziali non sono giusti!");
		return -1;
	}
	
	char* arrayBit = disk->bitmap_data;
	
	//trasformo il blocco e devo andare a leggere in due indici di una BitMapEntryKey
	//individuo cosi il blocco e il bit di block_num
	
	BitMapEntryKey chiave = BitMap_blockToIndex(block_num,(BitMap*)arrayBit);
	
	//devo capire se il blocco che voglio leggere è libero oppure no.
	
    if((arrayBit[chiave.entry_num] >> chiave.bit_num & 1)==0){
		printf("blocco libero impossibile leggere");
		return -1;
	}
		
	int fd = disk->fd;
	//allineo il fd al blocco che devo andare a leggere (header + bitmapdim + 512*blocco da leggere
	off_t allineo = lseek(fd,sizeof(DiskHeader)+disk->header->bitmap_entries+ block_num*BLOCK_SIZE,SEEK_SET);
	if(allineo ==-1){
		return -1;
	}
	
	//dopo che ho verificato che non è un blocco vuoto, e che ho posizionato il puntatore su primo byte del blocco da leggere 
	//posso iniziare a leggere... per far si che la lettura avvenga BLOCK_SIZE>= bytes_letti
	
	
	int val;
	int bytesLetti = 0;
	
	//se la dimensione della destinazione che deve contenere i bytes è grande almeno quanto il blocco da leggere(512bytes)
	//allora continuo con l'operazione di lettura, altrimenti ritorno -1 
	if(sizeof(dest)< BLOCK_SIZE){
			printf("errore il buffer per immagazzinare i byte è minore del blocco da leggere");
			return -1;
		}
		
	
			//finchè i bytes da leggere non sono finiti vado a scrivere le info nel buffer
			while(bytesLetti<=BLOCK_SIZE-1) {
	
				//aggiungo i bytesletti nel buffer puntato da dest 
				val= read(fd,dest + bytesLetti, BLOCK_SIZE - bytesLetti);
				//Se l’interruzione avviene prima di riuscire a leggere qualsiasi dato (zero byte letti), la read()
				// ritorna -1 ed errno viene settato a EINTR
				if(val == -1 && errno == EINTR) continue;
				if (val == -1 && errno != EINTR) {
				perror("la read è stata interrotta");
				return -1;
				exit(EXIT_FAILURE);
				}
				
				bytesLetti += val;
	//	}
	}
	

	
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
} BitMapEntryKey;*/


int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num){
	
	//controlli
	if(disk ==NULL|| src ==NULL || block_num < 0 || block_num > disk->header->bitmap_blocks ){
		handle_error("Errore, impossibile scrivere sul blocco poichè i parametri iniziali non sono giusti!");
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
	int fd = disk->fd;
	off_t allineo = lseek(fd,sizeof(DiskHeader)+disk->header->bitmap_entries+ block_num*BLOCK_SIZE,SEEK_SET);
	if(allineo ==-1){
		return -1;
	}
	
	
	   int val;
	   int bscritti= 0;
	
	//finchè il mio blocco non è finito vado a leggere le info dal buffer src e le scrivo nel fd
		while(bscritti <= BLOCK_SIZE-1){
        
		val = write(fd, src + bscritti, BLOCK_SIZE - bscritti);
		//se arriva una interruzione continua.
		if (val == -1 && errno == EINTR) continue;

		bscritti +=val;
	}


	   //quando ho scritto aggiorno la bitmap ad 1 per segnalare che ho il blocco pieno.
    int ris = BitMap_set((BitMap*) arrayBit, chiave.entry_num, 1);
    if(ris != 0){
        printf("Errore della Bitmap_set");
        return -1;
    }
	
	return 0;
}



//----------------------------------FREE BLOCK---------------------------------------
// frees a block in position block_num, and alters the bitmap accordingly
// returns -1 if operation not possible
int DiskDriver_freeBlock(DiskDriver* disk, int block_num){
	
	if(disk==NULL || block_num < 0 || block_num > disk->header->bitmap_blocks){
		handle_error("Errore, impossibile liberare blocco parametri iniziali non sono giusti!");
		return -1;
	}
	
	if(disk->header->free_blocks == disk->header->num_blocks){
		printf("impossibile liberare blocco, sono già tutti liberi");
		return -1;
		}
		
	char* arrayBit = disk->bitmap_data;
	
	//trasformo il blocco e devo andare a leggere in due indici di una BitMapEntryKey
	//individuo cosi il blocco e il bit di block_num
	
	BitMapEntryKey chiave = BitMap_blockToIndex(block_num,(BitMap*)arrayBit );
	
    //devo capire se il blocco su cui voglio scrivere è libero oppure no.
	
    if((arrayBit[chiave.entry_num] >> chiave.bit_num & 1)==0){
		printf("blocco già libero ");
		return -1;
	}
	
		//allineo il fd al blocco che devo liberare
	int fd = disk->fd;
	off_t allineo = lseek(fd,sizeof(DiskHeader)+disk->header->bitmap_entries+ block_num*BLOCK_SIZE,SEEK_SET);
	if(allineo ==-1){
		return -1;
	}
	
	 
	   int val;
	   int bscritti= 0;
	   //creo un buffer di char di dimensione del blocco, impostandoli tutti a zero
	   char* buf[BLOCK_SIZE];
	   
	   for (int i = 0; i < BLOCK_SIZE; i++){
		   buf[i] = 0;
	   }
	
	//finchè il mio blocco non è finito vado a leggere le info dal buffer src e le scrivo nel fd
		while(bscritti <= BLOCK_SIZE-1){
        
		val = write(fd, buf + bscritti, BLOCK_SIZE - bscritti);
		//se arriva una interruzione continua.
		if (val == -1 && errno == EINTR) continue;

		bscritti +=val;
	}
	
	//quando ho liberato il blocco aggiorno la bitmap a 0 per segnalare che ho il blocco vuoto.
    int ris = BitMap_set((BitMap*)arrayBit, chiave.entry_num, 0);
    if(ris != 0){
        printf("Errore della Bitmap_set");
        return -1;
    }
    
    //aggiorno il puntatore al primo blocco libero
    if(disk->header->first_free_block> block_num){
		disk->header->first_free_block = block_num;
		}
    
    //Aggiorno anche il numero dei blocchi liberi
    disk->header->free_blocks += 1;
	
	return 0;
	}
	
	
	// returns the first free blockin the disk from position (checking the bitmap)
int DiskDriver_getFreeBlock(DiskDriver* disk, int start){
	
	int bloccoLibero;
	if(disk == NULL || start < 0 || start > disk->header->bitmap_blocks){
			handle_error("Impossibile ritornare il primo blocco libero, poichè i parametri non sono corretti");
			exit(-1);
		}
		//prendo attraverso bitmapget, l'indice del primo blocco libero che trovo.
	    bloccoLibero = BitMap_get((BitMap*)disk->bitmap_data,start,0);
		if(bloccoLibero == -1){
				printf("Non ci sono blocchi liberi");
				return -1;
			}
		//aggiorno il puntatore al primo blocco libero
		disk->header->first_free_block = bloccoLibero;
		
		
		return bloccoLibero;
	}

// writes the data (flushing the mmaps)
int DiskDriver_flush(DiskDriver* disk){
		// controlli
		if(disk == NULL){
			handle_error("Impossibile aggiornare i dati il file su disco, parametri non corretti");
			return -1;
			}
			//calcolo la dimensione che devo andare ad aggiornare.
			int dimBitMap = disk->header->num_blocks;
			int dim =dimBitMap/8;
			int size = sizeof(DiskHeader)+ dim;
			//attraverso la funzione msync vado ad aggiornare il file su disco, con le modifiche fatte alla mappa di memoria.
			int valoreRit = msync(disk->header, size, MS_SYNC);
			if(valoreRit =-1){
				handle_error("Impossibile aggiornare il file su disco");
			}
		
		
		return valoreRit;
	}

	
