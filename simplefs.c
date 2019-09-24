#include "simplefs.h"


 #include <stdio.h>
#include <stdlib.h>
#include <string.h>






// initializes a file system on an already made disk
// returns a handle to the top level directory stored in the first block
DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){
    printf("****************INIZIALIZZAZIONE DEL FILESYSTEM*****************\n");

    //controlli
	if(fs == NULL){
		printf("Errore, riferimento a filesystem nullo\n\n");
		return NULL;
	}
	if(disk == NULL){
		printf("Errore, riferimento a disco nullo\n\n");
		return NULL;
	}

		//creo il directoryHandle, suppongo che un disco ha sempre una directory che inizia dal blocco 0.(root directory)
		DirectoryHandle* rootDirectory = malloc(sizeof(DirectoryHandle));

		//assegno al filesystem il disco già creato
		fs->disk = disk;
		fs->blocchi_riservati = disk->header->riservati; //sono il blocco 0 dell'header e i successivi per la bitmap'
		

		FirstDirectoryBlock* fdb = malloc(sizeof(FirstDirectoryBlock));


 		int firstFreeBlock = DiskDriver_getFreeBlock(fs->disk,0);
      
        //scrivo sulla prima directory(root)
        printf("Scrivo il first directory block nel primo blocco libero\n\n");
		int val = DiskDriver_writeBlock(fs->disk, fdb, firstFreeBlock);
		if(val==-1){
			printf("Errore nella scrittura del primo blocco (FirstDirectoryBlock)\n\n");
			free(fdb);
			return NULL;
			}

		//Riempio il directoryHandle
		rootDirectory->sfs = fs;
		rootDirectory->dcb = fdb; //primo blocco della directory
		rootDirectory->directory = NULL; //genitore della directory ( in questo caso NULL perchè la directory è la radice)
		rootDirectory->current_block = &(fdb->header);
		rootDirectory->pos_in_dir = 0; //posizione zero
		rootDirectory->pos_in_block = 0;// posizione nel disco


	    printf("**************FINE INIZIALIZZAZIONE*****************\n");

		return rootDirectory;

	}

// creates the inital structures, the top level directory
// has name "/" and its control block is in the first position
// it also clears the bitmap of occupied blocks on the disk
// the current_directory_block is cached in the SimpleFS struct
// and set to the top level directory

//prende un disco e ci fa la prima directory, poi setta la bitmap tutta a zero ad eccetto blocco della directory che è 1 (e blocchi per l'occupazione di bitmap)
//sovrascrivo le info della prima parte.
void SimpleFS_format(SimpleFS* fs){
    printf("************************INIZIO FORMATTAZIONE**************************\n");
	//controlli
	if(fs == NULL){
		printf("Errore puntatore al filesystem non valido\n\n");
		return;
	}

	BitMap bm;
	bm.num_bits = (fs->disk->header->num_blocks) -1; //num blocchi bitmap
	bm.entries = fs->disk->bitmap_data;	//dimensione bitmap

	//metto a zero tutti i bit della bitmap
	 bzero(bm.entries,bm.num_bits);

	//pongo i bit della bitmap dei blocchi che la contengono a 1
	int i;
	for (i = 0; i < bm.num_bits; i++){
		if(i<fs->disk->header->bitmap_blocks){
			BitMap_set(&bm,i,1);
		}
	}
	
	//STAMPO LO STATO DEL DISCO
	for(i = 0; i < bm.num_bits; i++){
		BitMapEntryKey key = BitMap_blockToIndex(i,&bm);
		printf("Entry_num : %d\tBit_num : %d\tStato : %d \n\n", key.entry_num, key.bit_num , ((((bm.entries[key.entry_num]) >> (key.bit_num))&1) ));}

        //top level directory
	FirstDirectoryBlock* fdb = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));

        printf("blocchi riservati: %d\n\n",fs->disk->header->riservati);

        //indice del primo blocco libero ricercandolo a partire dal blocco zero
        //che sarà il blocco riservato al fcb
        int firstFreeBlock = DiskDriver_getFreeBlock(fs->disk,0);

        //dimensione fcb in blocchi del disco
        int dimFcb =fdb->fcb.size_in_bytes % BLOCK_SIZE;
            if((( dimFcb)%BLOCK_SIZE)!=0){
            dimFcb +=1;
           }

		//POPOLO IL FIRST DIRECTORY BLOCK

		//--popolo il BlockHeader
		fdb->header.previous_block =-1;
		fdb->header.next_block =-1;
		fdb->header.block_in_file =0; //posizione del file, se 0 abbiamo il file control block

		//--popolo il FileControBlock
		fdb->fcb.directory_block =-1;//non ha genitore perchè è la root
		fdb->fcb.block_in_disk= firstFreeBlock;//si trova nel primo blocco del disco dopo i blocchi riservati
		strcpy(fdb->fcb.name ,"/");
		fdb->fcb.size_in_bytes = sizeof(FirstDirectoryBlock);
		fdb->fcb.size_in_blocks = 1;
		fdb->fcb.is_dir = 1;//directory

		fdb->num_entries = 0;

        //imposto i blocchi del file della dimensione fdb->file_blocks a zero , la dimensione è BLOCK_SIZE -sizeof(BlockHeader) - sizeof(FileControlBlock) - sizeof(int)/sizeof(int)
        memset(fdb->file_blocks,0,sizeof(fdb->file_blocks));

	   for(i = 0; i < sizeof(fdb->file_blocks); i++){
        fdb->file_blocks[i] = 0;
    }

        //scrivo sulla prima directory(root)
        printf("Scrivo il first directory block nel primo blocco libero\n\n");
		int val = DiskDriver_writeBlock(fs->disk, fdb, firstFreeBlock);
		if(val==-1){
			printf("Errore nella scrittura del primo blocco (FirstDirectoryBlock)\n\n");
			free(fdb);
			return;
			}
        DiskDriver_flush(fs->disk);

	free(fdb);
	printf("*******************************FINE FORMATTAZIONE********************************\n");


  }
//----------------------------------CREATE FILE-------------------------------------------------------
// creates an empty file in the directory d
// returns null on error (file existing, no free blocks)
// an empty file consists only of a block of type FirstBlock
FileHandle* SimpleFS_createFile(DirectoryHandle* d, const char* filename){

    printf("***************CREO UN NUOVO FILE***************\n");

    //controlli parametri
    if(filename==NULL) {
    printf("Impossibile creare file, filename è NULLO\n!");
    return NULL;
    }
    
    if(d==NULL) {
    printf("Impossibile creare file, riferimento a directoryHandle è NULLO\n!");
    return NULL;
    }
    
  
    
    //se ci sono entries, quindi se c'è almeno un file, allora controlla che non ci sia già un file con lo stesso nome.
    if(d->dcb->num_entries>0){

    //verifico se non esiste già un file con lo stesso nome, se esiste ritorna -1
    int flag = verificoFile(d,filename);
    if(flag == -1) {
        printf("file già esiste\n");
        return NULL;
        }  
	}

    //controllo ci sia spazio all'interno del disco per poter creare un nuovo file
    if(d->sfs->disk->header->free_blocks<1) {
        printf("Impossibile creare nuovo file, disco pieno");
        return NULL;
    }

       /* //dimensione fcb in blocchi
        int dimFcb =d->dcb->fcb.size_in_bytes % BLOCK_SIZE;
            if((( dimFcb)%BLOCK_SIZE)!=0){
            dimFcb +=1;
           }*/

    //cerco l'indice del primo blocco libero
    int firstFreeBlock = DiskDriver_getFreeBlock(d->sfs->disk,0);{
		if(firstFreeBlock==-1){
				printf("Non ci sono blocchi liberi");
				return NULL;
			}
		}
		
			printf("FIRSTFREEBLOCK:                       %d\n\n\n\n\n", firstFreeBlock);


    //creo FileHandle e FirstFileBlock
  

	
	printf("CIAOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n\n\n\n\n");
  
	  FirstFileBlock* ffb =malloc(sizeof(FirstFileBlock));
	//header default
    ffb->header.block_in_file = 0;
    ffb->header.next_block= -1;
    ffb->header.previous_block= -1;
    
    //altri parametri del fcb
    ffb->fcb.directory_block = d->dcb->fcb.block_in_disk;// directory genitore
    ffb->fcb.block_in_disk= firstFreeBlock; //primo blocco libero 
    ffb->fcb.is_dir=0;//file 0, directory 1
    strcpy(ffb->fcb.name, filename);
    ffb->fcb.size_in_blocks= 1; //ho appena creato il file
    ffb->fcb.size_in_bytes=0;//non ci sono elementi
    	
    	
	
    //setto tutti i blocchi del file a zero, poichè ho creato un nuovo file ed è vuoto
    int dim = BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader);
    int i;
    for (i = 0; i < dim; i++){
		ffb->data[i] = 0;
	}

   //scrivo il file sul disco
   int val = DiskDriver_writeBlock(d->sfs->disk, ffb, firstFreeBlock);
		if(val==-1){
			printf("Errore nella scrittura del primo blocco (FirstFileBlock)\n\n");
			free(ffb);
			return NULL;
			}
	
	
	//Ora che ho allocato il fileblock su disco devo assegnargli una directory
	
	//controllo prima nella fdb corrente
	int dimdcb = (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int);
	int dimdb = (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(int))/sizeof(int);
	//con questa variabile tengo conto dell'ultima directory block letta
	int ultimoBloccoLetto;
	
	int bloccolibero=-1;
	if(d->dcb->num_entries < dimdcb){
		for (i = 0; i < dimdcb; i++){
			if(d->dcb->file_blocks[i]<1){
				bloccolibero=i;
				break;
			}
		}
	}
	
	//se c'è un blocco libero
	if(bloccolibero!=-1){
		//assegno l'indice di blocco del ffb nel disco al blocco libero del file blocks
		d->dcb->file_blocks[bloccolibero] = ffb->fcb.block_in_disk;
		//incremento il numero di entries
		d->dcb->num_entries+=1;
		
		DiskDriver_writeBlock(d->sfs->disk, d->dcb, bloccolibero);
		
		printf("TROVATO BLOCCO LIBERO\n");
		
	}else{ // controllo nella directory block successiva
		
		int next_block_db = d->dcb->header.next_block;
		ultimoBloccoLetto =next_block_db;
		
		DirectoryBlock* db = malloc(sizeof(DirectoryBlock));
		
		while(next_block_db!=-1){
			
			ultimoBloccoLetto = next_block_db;
			
			//controllo nel blocco db
			int val = DiskDriver_readBlock(d->sfs->disk, db, next_block_db);
			if(val==-1){
				printf("Errore lettura del blocco successivo\n");
				return NULL;
			}
			
			//se va a buon fine controllo che ci sia un blocco libero 
			
				for (i = 0; i < dimdb; i++){
					if(db->file_blocks[i]<1){
						bloccolibero=i;
						break;
					}
				}
			
			
			//se c'è un blocco libero
			if(bloccolibero!=-1){
				//assegno l'indice di blocco del ffb nel disco al blocco libero del file blocks
				db->file_blocks[bloccolibero] = ffb->fcb.block_in_disk;
				//incremento il numero di entries
		
				DiskDriver_writeBlock(d->sfs->disk, db, bloccolibero);	
			
				printf("TROVATO BLOCCO LIBERO\n");
			
		}else{
			//Vado al prossimo blocco esistente
			next_block_db = db->header.next_block;
			}
		
		}
		//in questa situazione significa che non ci sono blocchi liberi in nessun blocco di directory, per cui devo creare un nuovo directory block
		
		DirectoryBlock* new_db = malloc(sizeof(DirectoryBlock));
		
		int primoBloccoLibero = DiskDriver_getFreeBlock(d->sfs->disk,0);
		
		new_db->header.previous_block = ultimoBloccoLetto;
		new_db->header.next_block = -1;
		new_db->header.block_in_file = db->header.block_in_file +1;
		
		//imposto tutti i blocchi a 0 
		for (i = 0; i < dimdb; i++){
			new_db->file_blocks[i]=0;
		}
		
		new_db->file_blocks[0] = ffb->fcb.block_in_disk;
		
		db->header.next_block = primoBloccoLibero;
		
		int val = DiskDriver_writeBlock(d->sfs->disk,new_db,primoBloccoLibero);
		if(val==-1){
			printf("Impossibile scrivere la directory block appena creata nel nuovo blocco libero!\n");
			return NULL;
			}
		
	}
	FileHandle* fh = malloc(sizeof(FileHandle));
		fh->sfs = d->sfs;//popolo il fileHandle
    	fh->fcb = ffb;
    	fh->directory = d->dcb;
		fh->current_block = malloc(sizeof(BlockHeader));
		memcpy(fh->current_block, &(ffb->header),sizeof(BlockHeader));
		fh->pos_in_file=0;
	
		
    DiskDriver_flush(d->sfs->disk);

	return fh;

    }

//FUNZIONE PER VERIFICARE L'ESISTENZA DEL FILE
//Vado a verificare che in ogni blocco,
      int verificoFile(DirectoryHandle* d, const char* filename){
		
		//controlli
		if(d==NULL){
			printf("La DirectoryHandle ha un riferimento NULLO\n");
			return -1;
		}
		if(filename==NULL){
			printf("Nome del file non speficato, riferimento NULLO\n");
			return -1;
			} 
		
		
		//dimensione del first directory block
		int size_fdb = BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int)/sizeof(int);
		//dimensione dei blocchi successivi
		int size_db = BLOCK_SIZE - sizeof(FileControlBlock)- sizeof(BlockHeader);
		
		FirstFileBlock* firstfileblock = malloc(sizeof(FirstFileBlock));
		
		//controllo primo blocco directory range (0-num_entries e inferiore allo spazio disponibile a contenere i file)
		int i;
		for (i = 0; (i < size_fdb) && (i< d->dcb->num_entries) ; i++){
			//leggo il blocco i-esimo e se va a buon fine la lettura controllo che non esista già un file con lo stesso nome
			int val = DiskDriver_readBlock(d->sfs->disk,&firstfileblock,d->dcb->file_blocks[i]);
			
			if(val !=-1){
				if(strncmp(firstfileblock->fcb.name,filename,strlen(filename))==0){
					printf("File con stesso nome, già esiste\n");
					return -1;
				}
			}else{
				printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
				free(firstfileblock);
				return -1;
			}
		}
		
		
		//controllo tutti gli altri blocchi-------------------
		int next_block = d->dcb->header.next_block;
		DirectoryBlock* db = malloc(sizeof(DirectoryBlock));
		
		while(next_block!=-1){
			//leggo il prossimo blocco
			int val = DiskDriver_readBlock(d->sfs->disk,db,next_block);
			if(val==-1){
				printf("Impossibile leggere il blocco\n");
				free(db);
				return -1;
			}
			
			//lettura andata a buon fine, ora controllo che non ci sia un file con lo stesso nome nell'array di file
			
			for (i = 0; (i < size_db) && (i < d->dcb->num_entries); i++){
				val = DiskDriver_readBlock(d->sfs->disk,&firstfileblock,db->file_blocks[i]);
				if(val!=-1){
					if(strncmp(firstfileblock->fcb.name,filename,strlen(filename))==0){
						printf("File con stesso nome, già esiste\n");
						return -1;
					}
				}else{
					printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
					free(firstfileblock);
					free(db);
					return -1;
					}
					
			}
			next_block = db->header.next_block;	
		}	
			
        return 0;
	}


// reads in the (preallocated) blocks array, the name of all files in a directory
int SimpleFS_readDir(char** names, DirectoryHandle* d);


// opens a file in the  directory d. The file should be exisiting
FileHandle* SimpleFS_openFile(DirectoryHandle* d, const char* filename);


// closes a file handle (destroyes it)
int SimpleFS_close(FileHandle* f);

// writes in the file, at current position for size bytes stored in data
// overwriting and allocating new space if necessary
// returns the number of bytes written
int SimpleFS_write(FileHandle* f, void* data, int size);

// writes in the file, at current position size bytes stored in data
// overwriting and allocating new space if necessary
// returns the number of bytes read
int SimpleFS_read(FileHandle* f, void* data, int size);

// returns the number of bytes read (moving the current pointer to pos)
// returns pos on success
// -1 on error (file too short)
int SimpleFS_seek(FileHandle* f, int pos);

// seeks for a directory in d. If dirname is equal to ".." it goes one level up
// 0 on success, negative value on error
// it does side effect on the provided handle
 int SimpleFS_changeDir(DirectoryHandle* d, char* dirname);

// creates a new directory in the current one (stored in fs->current_directory_block)
// 0 on success
// -1 on error
int SimpleFS_mkDir(DirectoryHandle* d, char* dirname);

// removes the file in the current directory
// returns -1 on failure 0 on success
// if a directory, it removes recursively all contained files
int SimpleFS_remove(SimpleFS* fs, char* filename);
