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
	/*if(disk == NULL){
		printf("Errore, riferimento a disco nullo\n\n");
		return NULL;
	}*/
	printf("SONO QUI\n\n");
		//creo il directoryHandle, suppongo che un disco ha sempre una directory che inizia dal blocco 0.(root directory)
		DirectoryHandle* rootDirectory = (DirectoryHandle*) malloc(sizeof(DirectoryHandle));
	printf("SONO QUI 2\n\n");

		//assegno al filesystem il disco già creato
		//fs->disk = disk;
		/*fs->blocchi_riservati = fs->disk->header->riservati; //sono il blocco 0 dell'header e i successivi per la bitmap'
		*/
			printf("SONO QUI3\n\n");

		  //top level directory
		FirstDirectoryBlock* fdb = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));
		printf("SONO QUI  4\n\n");

        //printf("blocchi riservati: %d\n\n",fs->disk->header->riservati);

        //indice del primo blocco libero ricercandolo a partire dal blocco zero
        //che sarà il blocco riservato al fcb
        int firstFreeBlock = DiskDriver_getFreeBlock(fs->disk,0);
printf("SONO QUI  5\n\n");
       /* //dimensione fcb in blocchi del disco
        int dimFcb =fdb->fcb.size_in_bytes % BLOCK_SIZE;
            if((( dimFcb)%BLOCK_SIZE)!=0){
            dimFcb +=1;
           } */
		
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
printf("SONO QUI  6\n\n");
        //imposto i blocchi del file della dimensione fdb->file_blocks a zero , la dimensione è BLOCK_SIZE -sizeof(BlockHeader) - sizeof(FileControlBlock) - sizeof(int)/sizeof(int)
        memset(fdb->file_blocks,0,(int)sizeof(fdb->file_blocks));
	

        //scrivo sulla prima directory(root)
        printf("Scrivo il first directory block nel primo blocco libero\n\n");
        
		int val = DiskDriver_writeBlock(fs->disk, fdb, firstFreeBlock);
		if(val==-1){
			printf("Errore nella scrittura del primo blocco (FirstDirectoryBlock)\n\n");
			free(fdb);
			return NULL;
		}
			
		// Ritorno un Puntatore Directory Handle
		//Riempio il directoryHandle
		rootDirectory->sfs = fs;
		rootDirectory->dcb = fdb; //primo blocco della directory
		rootDirectory->directory = NULL; //genitore della directory ( in questo caso NULL perchè la directory è la radice)
		rootDirectory->current_block = &(fdb->header);
		rootDirectory->pos_in_dir = 0; //posizione zero
		rootDirectory->pos_in_block = 0;// posizione nel disco
		DiskDriver_flush(fs->disk);
		
		
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
	bm.entries = fs->disk->bitmap_data + sizeof(BitMap);	//dimensione bitmap

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

       /* //dimensione fcb in blocchi del disco
        int dimFcb =fdb->fcb.size_in_bytes % BLOCK_SIZE;
            if((( dimFcb)%BLOCK_SIZE)!=0){
            dimFcb +=1;
           } */
		
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

	 /*  for(i = 0; i < (int)sizeof(fdb->file_blocks); i++){
        fdb->file_blocks[i] = 0;
    }*/

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
		if(filename == NULL) {
			printf("Impossibile creare file, filename è NULLO\n!");
			return NULL;
		}
		
		if(d == NULL) {
			printf("Impossibile creare file, riferimento a directoryHandle è NULLO\n!");
			return NULL;
		}
		
		
		printf("Controllo il numero di entries %d \n\n", d->dcb->num_entries);
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

		//cerco l'indice del primo blocco libero
		int firstFreeBlock = DiskDriver_getFreeBlock(d->sfs->disk,0);
		if(firstFreeBlock==-1){
			printf("Non ci sono blocchi liberi");
			return NULL;
		}
		
			
		printf("PRIMO BLOCCO LIBERO TROVATO: %d\n\n\n\n\n", firstFreeBlock);


		//creo FileHandle e FirstFileBlock
		FirstFileBlock* ffb =(FirstFileBlock*)malloc(sizeof(FirstFileBlock));
		//header default
		ffb->header.block_in_file = 0;
		ffb->header.next_block= -1;
		ffb->header.previous_block= -1;
		
		//altri parametri del fcb
		ffb->fcb.directory_block = d->dcb->fcb.block_in_disk;// directory genitore
		printf("DIRECTORY GENITORE = %d\n\n", d->dcb->fcb.block_in_disk); 
		ffb->fcb.block_in_disk= firstFreeBlock; //primo blocco libero 
		ffb->fcb.is_dir=0;//file 0, directory 1
		strncpy(ffb->fcb.name, filename, strlen(filename));
		//printf("NOME %s\n\n",ffb->fcb.name);
		ffb->fcb.size_in_blocks= 1; //ho appena creato il file
		ffb->fcb.size_in_bytes=0;//non ci sono elementi
			
	   
		//setto tutti i blocchi del file a zero, poichè ho creato un nuovo file ed è vuoto
		memset(ffb->data, 0, sizeof(ffb->data));
		

	
		//controllo prima nella fdb corrente

		//con questa variabile tengo conto dell'ultima directory block letta
		int ultimoBloccoLetto;
		int i;
		DirectoryBlock* db = (DirectoryBlock*)malloc(sizeof(DirectoryBlock));

		printf("Dimensione file block %ld    \n\n\n", sizeof(d->dcb->file_blocks));
		//se trovo un blocco libero allora lo uso per inserire il file nella FIRSTDIRECTORYBLOCK
		if(d->dcb->num_entries < (int)sizeof(d->dcb->file_blocks)){
			memcpy(d->current_block,&(d->dcb->header), sizeof(BlockHeader));
			for (i = 0; i < (int)sizeof(d->dcb->file_blocks); i++){
				if(d->dcb->file_blocks[i]==0){
					d->pos_in_dir = i+1;
					printf("TROVATO BLOCCO LIBERO\n");
					//assegno l'indice di blocco del ffb nel disco al blocco libero del file blocks
					d->dcb->file_blocks[i] =  firstFreeBlock;
					//incremento il numero di entries
					d->dcb->num_entries +=1;
					printf("BLOCCOOOOO DOVE DEVO SCRIVERE DELLA WRITE, %d\n\n\n\n\n", firstFreeBlock);
					//vado a scrivere nel blocco che ho individuato come libero.
					DiskDriver_writeBlock(d->sfs->disk, ffb, firstFreeBlock);
					break;
				}
			}
			
		}else{ // controllo nella directory block successiva se c'è un blocco libero
			
			
			
			while(d->dcb->header.next_block!=-1){
				ultimoBloccoLetto = d->dcb->header.next_block;
				printf("OKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK\n\n\n\n");
				
				//controllo nel blocco db
				int val = DiskDriver_readBlock(d->sfs->disk, db, ultimoBloccoLetto);
				if(val==-1){
					printf("Errore lettura del blocco successivo\n");
					return NULL;
				}
				//se va a buon fine controllo che ci sia un blocco libero 
				for (i = 0; i < (int)sizeof(db->file_blocks); i++){
					d->pos_in_dir = i;
					if(db->file_blocks[i]==0){
						printf("TROVATO BLOCCO LIBERO\n");
						//assegno l'indice di blocco del ffb nel disco al blocco libero del file blocks
						db->file_blocks[i] = firstFreeBlock;
						//incremento il numero di entries
						d->dcb->num_entries += 1;
						DiskDriver_writeBlock(d->sfs->disk, db, ultimoBloccoLetto);	
						break;
					}
				}
				//Vado al prossimo blocco esistente
				d->dcb->header.next_block = db->header.next_block;
			}
			
			//in questa situazione significa che non ci sono blocchi liberi in nessun blocco di directory, per cui devo creare un nuovo directory block	
			DirectoryBlock* new_db =(DirectoryBlock*) malloc(sizeof(DirectoryBlock));
			
			int primoBloccoLibero = DiskDriver_getFreeBlock(d->sfs->disk,0);
			if(primoBloccoLibero == -1) {
				printf("BLOCCHI ESAURITI\n\n");
				return NULL;
			}
				
			new_db->header.previous_block = ultimoBloccoLetto;
			new_db->header.next_block = -1;
			new_db->header.block_in_file = db->header.block_in_file +1;
			
			//imposto tutti i blocchi a 0 
			memset(new_db->file_blocks,0,sizeof(db->file_blocks));
			//assegno al valore del primo blocco della nuova db l'indice del blocco nel disco
			new_db->file_blocks[0] = primoBloccoLibero;
			
			db->header.next_block = primoBloccoLibero;
			
			int val = DiskDriver_writeBlock(d->sfs->disk,new_db,primoBloccoLibero);
			if(val==-1){
				printf("Impossibile scrivere la directory block appena creata nel nuovo blocco libero!\n");
				free(ffb);
				free(db);
				free(new_db);
				return NULL;
			}
			free(new_db);
		}
	
		
		FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
			fh->sfs = d->sfs;//popolo il fileHandle
			fh->fcb = ffb;
			fh->directory = d->dcb;
			fh->current_block = &(ffb->header);
			fh->pos_in_file=0;
		free(db);
		free(ffb);
		free(fh);
		printf("***************************FINE CREAZIONE FILE**********************\n\n\n\n");

		return fh;
	}
		
//FUNZIONE PER VERIFICARE L'ESISTENZA DEL FILE
//Vado a verificare che in ogni blocco,
int verificoFile(DirectoryHandle* d, const char* filename){
		
		printf("VERIFICO L'ESISTENZA DEL FILE \n\n");
		
		//controlli
		if(d == NULL){
			printf("La DirectoryHandle ha un riferimento NULLO\n");
			return -1;
		}
		if(filename == NULL){
			printf("Nome del file non speficato, riferimento NULLO\n");
			return -1;
		} 
		
		FirstFileBlock* ffb = (FirstFileBlock*)malloc(sizeof(FirstFileBlock));
		
		//controllo primo blocco directory range (0-num_entries e inferiore allo spazio disponibile a contenere i file)
		int i;
		for (i = 0; (i < (int)sizeof(d->dcb->file_blocks)) && (d->dcb->file_blocks[i] >0) ; i++){
			//leggo il firstFileBlock del blocco i-esimo e se va a buon fine la lettura controllo che non esista già un file con lo stesso nome
			int val = DiskDriver_readBlock(d->sfs->disk, ffb,d->dcb->file_blocks[i]);	
			if(val == -1){
				printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
				free(ffb);
				exit(1);
			}
			if(strncmp(ffb->fcb.name,filename,strlen(filename))==0){
				printf("File con stesso nome, già esiste\n");
				free(ffb);
				return -1;
			}
		}
		
		
		//controllo tutti gli altri blocchi-------------------
		int next_block = d->dcb->header.next_block;
		DirectoryBlock* db = (DirectoryBlock*)malloc(sizeof(DirectoryBlock));
		
		while(next_block!=-1){
			//leggo il prossimo blocco
			int val = DiskDriver_readBlock(d->sfs->disk,db,next_block);
			if(val==-1){
				printf("Impossibile leggere il blocco\n");
				free(db);
				free(ffb);
				exit(1);
			}
			
			//lettura andata a buon fine, ora controllo che non ci sia un file con lo stesso nome nell'array di file
			
			for (i =0; (i < (int)sizeof(db->file_blocks)) && (db->file_blocks[i]>0) ; i++){
				val = DiskDriver_readBlock(d->sfs->disk,ffb,db->file_blocks[i]);
				if(val == -1){
					printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
					free(ffb);
					free(db);
					exit(1);
				}
				if(strncmp(ffb->fcb.name,filename,strlen(filename))==0){
					printf("File con stesso nome, già esiste\n");
					free(db);
					free(ffb);
					return -1;
				}
					
			}
			next_block = db->header.next_block;		
		}	
		free(db);
		free(ffb);
		printf("FINE VERIFICA\n\n");
		
      return 0;
	}

//A partire dalla directoryhandle data, vado nella diretory corrispondente e inizio a scorrere la lista di fle contenuti e li inserisco nella lista names
//devo preallocare names,del numero di entries che ho
// reads in the (preallocated) blocks array, the name of all files in a directory
int SimpleFS_readDir(char** names, DirectoryHandle* d){
		printf("*************************READ_DIR************************\n\n");
		
		//controlli
		if(d == NULL){
			printf("Errore impossibile leggere la directory poichè il riferimento al DirectoryHandle è NULLO\n\n");
			return -1;
		}
		//Se num entries è pari a zero significa che non ci sono elementi da leggere
		if(d->dcb->num_entries==0){
			printf("Non ci sono elementi da leggere\n\n");
			return -1;
		}
		
		int i, val;
		
		*names=(char*) malloc((d->dcb->num_entries)*sizeof(d->dcb->fcb.name));

		memset(*names, 0,(d->dcb->num_entries*sizeof(d->dcb->fcb.name)));
	
		//scorro il fileblocks della prima directory
		FirstFileBlock* firstFileBlock =  (FirstFileBlock*)malloc(sizeof(FirstFileBlock));
		for (i = 0; (i < d->dcb->num_entries) && (i < (int)sizeof(d->dcb->file_blocks)); i++){
			val = DiskDriver_readBlock(d->sfs->disk,firstFileBlock, d->dcb->file_blocks[i]);
			if(val==-1){
				printf("Lettura nomi file non riuscita\n\n");
				free(firstFileBlock);
				return -1;
			}
				
				 strncat((*names)+ strlen(*names), firstFileBlock->fcb.name , strlen(firstFileBlock->fcb.name));
				 //strncat((*names), " " ,sizeof(char));	    
		}
		
		//scorro il fileblocks delle directory successive e inserisco i nomi nell'array names
		DirectoryBlock* db = (DirectoryBlock*) malloc(sizeof(DirectoryBlock));
		int next_block = d->dcb->header.next_block;
		
		while(next_block!=-1){
			val = DiskDriver_readBlock(d->sfs->disk,db,next_block);
			if(val==-1){
				printf("Impossibile leggere blocchi successivi\n\n");
				free(db);
				free(firstFileBlock);
				return -1;
			}
			for (i = 0; i<(d->dcb->num_entries) && i < (int)sizeof(db->file_blocks); i++){
				val = DiskDriver_readBlock(d->sfs->disk,firstFileBlock, db->file_blocks[i]);
				if(val==-1){
					printf("Lettura nomi file non riuscita\n\n");
					free(db);
					free(firstFileBlock);
					return -1;
				}
				strncat((*names)+ strlen(*names), firstFileBlock->fcb.name, strlen(firstFileBlock->fcb.name));
				strncat((*names), " " ,sizeof(char));
			}
			next_block= db->header.next_block;
		}
		
		free(db);
		free(firstFileBlock);
		
		printf("*******************************FINE READ DIR************************************\n\n");
		return 0;
}

// opens a file in the  directory d. The file should be exisiting
FileHandle* SimpleFS_openFile(DirectoryHandle* d, const char* filename){

		printf("*******************************OPEN_FILE************************************\n\n");

		//controlli
		if(d==NULL){
		printf("Impossibile aprire file riferimento a directoryHandle NULLO\n\n\n");
			 return NULL;
		}
		if(filename==NULL){
			printf("Impossibile aprire file riferimento a filename NULLO\n\n\n");
			 return NULL;
		}
		
		FirstFileBlock* ffb =(FirstFileBlock*) malloc(sizeof(FirstFileBlock));
		
		//creo filehandle da ritornare come puntatore al file che apro
		FileHandle* fh = (FileHandle*) malloc(sizeof(FileHandle));
		fh->sfs = d->sfs;
		
		
		//scorro il fileblocks 
		int i,val;
		for (i = 0; i < d->dcb->num_entries && i<(int)sizeof(d->dcb->file_blocks); i++){
			val = DiskDriver_readBlock(d->sfs->disk, ffb, d->dcb->file_blocks[i]);
			if(val==-1){
				printf("Lettura nomi file non riuscita\n\n");
				free(ffb);
				return NULL;
			}
				
		//se il nome è lo stesso ed è un file allora popolo il fileheader
			if(strcmp(ffb->fcb.name, filename) == 0 && ffb->fcb.is_dir == 0){
				//popolo fileHeader
				fh->sfs = d->sfs;
				fh->fcb = ffb;
				fh->directory = d->dcb;
				fh->current_block = &(ffb->header);
				fh->pos_in_file = 0;
				//free(ffb);
				// Restituisco il file handle
				return fh;
			}
		}
		
		//se non sta nella prima directory controllo le directory successive
		DirectoryBlock* db = (DirectoryBlock*) malloc(sizeof(DirectoryBlock));
		int next_block = d->dcb->header.next_block;
		
		//leggo la prossima directory finchè ce ne sono
		while(next_block!=-1){
			val = DiskDriver_readBlock(d->sfs->disk,db,next_block);
			if(val==-1){
				printf("Impossibile leggere blocchi successivi\n\n");
				free(db);
				free(ffb);
				return NULL;
			}
				//scorro il fileblock
			for (i = 0; i<(d->dcb->num_entries) && i < (int)sizeof(db->file_blocks); i++){
				val = DiskDriver_readBlock(d->sfs->disk,db, db->file_blocks[i]);
				if(val==-1){
					printf("Lettura nomi file non riuscita\n\n");
					free(db);
					free(ffb);
					return NULL;
				}
				//se il nome è uguale ed è un file popolo il file header
				if(strcmp(ffb->fcb.name, filename) == 0 && ffb->fcb.is_dir == 0){
					//popolo il fileheader
					fh->sfs = d->sfs;
					fh->fcb = ffb;
					fh->directory = d->dcb;
					fh->current_block = &(ffb->header);
					fh->pos_in_file = 0;
					//free(ffb);
					free(db);
					// Restituisco il file handle
					return fh;
				}
			}
		}	
		printf("*******************************FINE OPEN_FILE************************************\n\n");
		free(fh);
		free(ffb);
		//se non lo trovo, allora ritorna riferimento nullo
		printf("File da aprire non trovato\n\n");
		return NULL;
	}


// closes a file handle (destroyes it)
int SimpleFS_close(FileHandle* f){
		printf("****************************CLOSE*************************\n\n");
		if(f==NULL){
			printf("Riferimento a fileHandle NULL, IMPOSSIBILE DISTRUGGERE FILEHANDLE\n\n");
			return  -1;
		}
		//libero il file handle
		free(f);
		printf("****************************FINE CLOSE*************************\n\n");
		return 0;
}

// writes in the file, at current position for size bytes stored in data
// overwriting and allocating new space if necessary
// returns the number of bytes written
int SimpleFS_write(FileHandle* f, void* data, int size){
		printf("***************************************************WRITE***********************************************\n\n\n");
		
		//CONTROLLI
		if(f==NULL){
			printf("Impossibile scrivere nel file, riferimento a FileHandle NULLO\n\n");
			return -1;
		}
		if(data==NULL){
			printf("Impossibile scrivere nel file, riferimento a data NULLO\n\n");
			return -1;
		}
		if(size<1){
			printf("Impossibile scrivere nel file, dimensione di data non specificata\n\n");
			return -1;
		}
	
		int next_block=0; //per identificare il blocco successivo da andare a scrivere 
		int bloccoCorrente = f->fcb->fcb.block_in_disk; //mi indica in che blocco del disco mi trovo
		int bytes_scritti=0; //da ritornare
		int bytes_daScrivere=0; //mi tengono contro del numero di bytes da scrivere ancora 
		int val;
		int bloccoLibero;
		
	
		
		printf("BLOCCO CORRENTE  %d  \n\n", bloccoCorrente);
		printf("f->pos_infile-------------------------  %d\n\n\n", f->pos_in_file);
		printf("f->fcb->data-------------------------  %ld\n\n\n", sizeof(f->fcb->data));
		printf("size---------------------------- %d\n\n\n", size);
		
		//controllo se la dimensione dei dati contenuti nel ffb - la posizione in cui mi trovo è minore della dimensione(size) dei bytes da scrivere
		//allora posso scriverli completamente del FFB, scrivo tutti i dati in ffb.
		
		if(size - f->pos_in_file < (int)sizeof(f->fcb->data) ){//----------------------------------------------------------------------------------se dim minore di size ffb
			FirstFileBlock* ffb = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));
			val = DiskDriver_readBlock(f->sfs->disk,ffb,f->fcb->fcb.block_in_disk);
			if(val==-1){
				printf("Impossibile leggere il ffb\n\n");
				free(ffb);
				return -1; 
			}
			//altrimenti la lettura del blocco è andata bene quindi scrivo
			memcpy(ffb->data + f->pos_in_file,(char*)data,strlen((char*)data));
			val = DiskDriver_writeBlock(f->sfs->disk,ffb, f->fcb->fcb.block_in_disk);
			if(val==-1){
				printf("Impossibile scrivere nel ffb\n\n");
				free(ffb);
				return -1;
			}
		
			bytes_scritti = strlen((char*)data);
			
			//aggiorno valori delle dimensioni nel File Control Block
			f->fcb->fcb.size_in_bytes = f->pos_in_file + strlen((char*)data);
			f->fcb->fcb.size_in_blocks =1;
			f->current_block->block_in_file = 0;	
			//aggiorno posizione nel file
			f->pos_in_file = f->pos_in_file + bytes_scritti;
			printf("Blocco nel disco: %d\n", f->fcb->fcb.block_in_disk);
			free(ffb);
		}
		
		
		else{ // la dimensione dei dati non rientra nel ffb, bisogna scrivere anche in altri blocchi-----------------------DEBBO SCRIVERE IN PIU BLOCCHI
			//leggo il file
			FirstFileBlock* ffb = (FirstFileBlock*) malloc(sizeof(FirstFileBlock)); 
			val = DiskDriver_readBlock(f->sfs->disk,ffb,f->fcb->fcb.block_in_disk);
			if(val==-1){
				printf("Impossibile leggere il ffb\n\n");
				free(ffb);
				return -1; 
			}
				
			//vedo la differenza di blocchi che posso scrivere nel primo FirstFileBlock
			//int differenzaffb = sizeof(f->fcb->data) - f->pos_in_file;
			int differenzaffb = size - sizeof(ffb->data);	
			//mi salvo in una variabile i restanti bytes da scrivere negli altri blocchi
			bytes_daScrivere = differenzaffb;	
			
			//Scrivo nel ffb il primo blocco
			//intanto scrivo i blocchi che mi entrano nel primo blocco  --------------------PRIMO BLOCCO
			memcpy((char*)ffb->data ,(char*)data,sizeof(ffb->data));
			val = DiskDriver_writeBlock(f->sfs->disk,ffb, f->fcb->fcb.block_in_disk);
			if(val==-1){
				printf("Impossibile scrivere nel ffb\n\n");
				free(ffb);
				return -1;
			}	
			bytes_scritti += sizeof(ffb->data);
			f->pos_in_file = f->pos_in_file+sizeof(ffb->data);
			f->current_block->block_in_file = 0;	
			printf("Blocco nel disco: %d\n", f->fcb->fcb.block_in_disk);
			printf("Bytes Scritti:  %d  \n",bytes_scritti);
			printf("Bytes da Scrivere:  %d  \n",bytes_daScrivere);
				 
			bloccoCorrente = f->fcb->fcb.block_in_disk;
			//f->current_block->next_block = ffb->header.next_block;
			
			if(differenzaffb > 0){ //-----------------------------------------------------------------------SE CI STA ANCORA ROBA DA SCRIVERE 
						
				FileBlock* fb = (FileBlock*)malloc(sizeof(FileBlock));
									
				//VALE PER I BLOCCHI SUCCESSIVI AL FIRST
				next_block = ffb->header.next_block;
				
				printf("NEXTBLOCKKKKKK    %d \n\n\n", next_block);
			
				//finchè il prossimo blocco è diverso da -1  e bytes da scrivere sono > 0 vado a scrivere nel prossimo blocco i bytes restanti
				while((next_block != -1)&&(bytes_daScrivere>0)){ //---------------------------SE CI STA UN PROSSIMO BLOCCO
					
					//aggiorno il puntatore al bloccoCorrente
					bloccoCorrente = next_block;
					f->current_block->next_block = next_block;					
					//leggo il blocco successivo 
					val = DiskDriver_readBlock(f->sfs->disk,fb, bloccoCorrente);
					if(val == -1){
						printf("Impossibile leggere dal fb\n\n");
						free(ffb);
						free(fb);
						return -1;
					}
					//se ci entrano tutti li scrivo tutti nel blocco
					if(((int)sizeof(fb->data) > bytes_daScrivere)){//----------------------------------Se i BYTES MI ENTRANO TUTTI NEL PROX BLOCCO	
						memcpy((char*)fb->data,(char*)data  ,bytes_daScrivere);
						val = DiskDriver_writeBlock(f->sfs->disk,fb, bloccoCorrente);
						if(val==-1){
							printf("Impossibile scrivere nel fb\n\n");
							free(ffb);
							free(fb);
							return -1;
						}
										
						bytes_scritti += bytes_daScrivere;
						//sovrascrivo
						f->pos_in_file = bytes_daScrivere;
						
						printf("Blocco nel disco: %d\n", bloccoCorrente);
						printf("Blocco nel disco: %d\n", bloccoCorrente);
						printf("Bytes Scritti:  %d  \n",bytes_scritti);
						printf("Bytes da Scrivere:  %d  \n",bytes_daScrivere);
					}else{	//ricalcolo le dimensioni dei bytes_daScrivere, e riempio il blocco corrente con i dati  ---------------------------DEVO RIDIMENSIONARE
						
						bytes_daScrivere = bytes_daScrivere - sizeof(fb->data);
						memcpy((char*)fb->data ,(char*) data   ,sizeof(fb->data));
						val = DiskDriver_writeBlock(f->sfs->disk,fb, bloccoCorrente);
						if(val==-1){
							printf("Impossibile scrivere nel fb\n\n");
							free(ffb);
							free(fb);
							return -1;
						}
											
						bytes_scritti += sizeof(fb->data);
						f->pos_in_file = sizeof(fb->data);

						printf("Blocco nel disco: %d\n",bloccoCorrente);
						
						printf("Blocco nel disco: %d\n",bloccoCorrente);
						printf("Bytes Scritti:  %d  \n",bytes_scritti);
						printf("Bytes da Scrivere:  %d  \n",bytes_daScrivere);
					} 
					//aggiorno next_block ai blocchi successivi al fb
					next_block = f->current_block->next_block;
				}		
				
				//Altrimenti se non ci sono più blocchi successivi 
				if(bytes_daScrivere > 0){	//--------------------------------------------SE CI STA ANCORA ROBA DA SCRIVERE MA NON CI SONO BLOCCHI
				FileBlock * fb2 = (FileBlock*)malloc(sizeof(FileBlock));
						fb2->header.next_block = -1;
						fb2->header.previous_block = bloccoCorrente; //blocco prima
						fb2->header.block_in_file =  f->fcb->header.block_in_file+1 ;
						
							
						bloccoLibero = DiskDriver_getFreeBlock(f->sfs->disk,0);
						if(bloccoLibero==-1){
							printf("Non ci sono blocchi liberi,DISCO PIENO\n\n");
							free(fb2);
							return -1;
						}
						f->current_block->next_block = bloccoLibero;
						
					
						
					while(bytes_daScrivere > 0 && bytes_scritti <size ){
				
					//ma devo continuare a inserire dati devo trovare un blocco libero del disco ed allocarlo per scrivere i dati 
					
					//altrimenti ho trovato un blocco libero e setto l'header
							
					
					 
						bloccoCorrente = bloccoLibero;	
						
						//se ci entrano tutti li scrivo tutti nel blocco
						if(((int)sizeof(fb2->data) > bytes_daScrivere)){
							
						
							memcpy((char*)fb2->data,(char*)data ,bytes_daScrivere);
							bytes_scritti += bytes_daScrivere;
							f->pos_in_file = bytes_daScrivere;
							bytes_daScrivere=0;
							
							
							printf("Blocco nel disco: %d\n", bloccoCorrente);
							
							printf("Blocco nel disco: %d\n", bloccoCorrente);
							printf("Bytes Scritti:  %d  \n",bytes_scritti);
							printf("Bytes da Scrivere:  %d  \n",bytes_daScrivere);
							val = DiskDriver_writeBlock(f->sfs->disk,fb2, bloccoLibero);
							if(val==-1){
								printf("Impossibile scrivere nel fb\n\n");
								free(ffb);
								free(fb2);
								return -1;
							}
							
							
						}else{	
							//ricalcolo le dimensioni dei bytes_daScrivere, e riempio il blocco corrente con i dati
							bytes_daScrivere = bytes_daScrivere - sizeof(fb2->data);
							
										
							memcpy(fb2->data ,(char*)data ,sizeof(fb2->data));
							bytes_scritti += sizeof(fb2->data);
							f->pos_in_file = sizeof(fb2->data);
							
							//bloccoCorrente = bloccoLibero;
							printf("Blocco nel disco: %d\n", bloccoCorrente);
							
							printf("Blocco nel disco: %d\n",bloccoCorrente);
							printf("Bytes Scritti:  %d  \n",bytes_scritti);
							printf("Bytes da Scrivere:  %d  \n",bytes_daScrivere);
												
							val = DiskDriver_writeBlock(f->sfs->disk,fb2, bloccoLibero);
							if(val==-1){
								printf("Impossibile scrivere nel fb\n\n");
								free(ffb);
								free(fb2);
								return -1;
							}	
							fb2->header.next_block = bloccoLibero;
						}
						
						bloccoLibero = DiskDriver_getFreeBlock(f->sfs->disk,0);
						if(bloccoLibero==-1){
							printf("Non ci sono blocchi liberi,DISCO PIENO\n\n");
							free(fb2);
							return -1;
						}
						fb2->header.next_block =-1;
						fb2->header.previous_block = bloccoCorrente; //blocco prima
						fb2->header.block_in_file =  f->fcb->header.block_in_file+1 ;	
					}
					free(fb2);
				}
				free(fb);
			}
			free(ffb);
		}
		f->fcb->fcb.size_in_bytes = bytes_scritti;
		return bytes_scritti;
	}

// reads in the file, at current position size bytes stored in data
// returns the number of bytes read
int SimpleFS_read(FileHandle* f, void* data, int size){
	
		//controlli
		if(f==NULL){
			printf("Impossibile leggere, riferimento a fileHandle NULLO\n\n");
			return -1;
		}
		
		if(data==NULL){
			printf("Impossibile leggere, riferimento a Data nullo\n\n");
			return -1;
		}
		
		if(size<1){
			printf("Impossibile leggere, size inferiore a 1\n\n");
			return -1;
		}
		
		int bytesLetti = 0;
		int bytesDaLeggere=0;
		int val = 0;
		int bloccoCorrente = f->fcb->fcb.block_in_disk;
		
		//se la dimensione da leggere è maggiore dei bytes del file allora leggo fino alla dimensione massima del file.
		if(size > f->fcb->fcb.size_in_bytes){
			printf("Non posso leggere la dimensione specificata: %d, ma leggo fino alla dimensione del file %d\n", size,f->fcb->fcb.size_in_bytes); 
			bytesDaLeggere = f->fcb->fcb.size_in_bytes;
		}else{
			//altrimenti leggo la dimensione passata in input
			bytesDaLeggere = size;
		} 
	
		//se la dimensione dei bytes da leggere è minore della dimensione del primo blocco FirstFileBlock
		//Allora leggo fino fino a bytesDaLeggere 
		if(bytesDaLeggere < (int)sizeof(f->fcb->data) ){//----------------------------------------------------------------------------------se dim minore di size ffb
			FirstFileBlock* ffb = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));
			val = DiskDriver_readBlock(f->sfs->disk,ffb,f->fcb->fcb.block_in_disk);
			if(val==-1){
				printf("Impossibile leggere il ffb\n\n");
				free(ffb);
				return -1; 
			}
			//altrimenti la lettura del blocco è andata bene quindi scrivo i blocchi da Leggere in data
			memcpy((char*)data, ffb->data ,bytesDaLeggere);
		
			bytesLetti += bytesDaLeggere;
			printf("SONO QUIIIIIIIIIIIIIIIIIII\n");	
			printf("Blocco nel disco: %d\n", f->fcb->fcb.block_in_disk);
			free(ffb);
		}else{ // la dimensione dei dati da leggere non rientra nel ffb, bisogna leggere anche altri blocchi
				//leggo il primo blocco interamente
				printf("SECONDAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n");
				FirstFileBlock* ffb = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));
				val = DiskDriver_readBlock(f->sfs->disk,ffb,f->fcb->fcb.block_in_disk);
				if(val==-1){
					printf("Impossibile leggere il ffb\n\n");
					free(ffb);
					return -1; 
				}
					
				//vedo la differenza di blocchi che mi mancano da leggere 
				int differenzaffb = bytesDaLeggere - sizeof(ffb->data);	
				//mi salvo in una variabile i restanti bytes da scrivere negli altri blocchi
				bytesDaLeggere = differenzaffb;	
				
				//Scrivo nel ffb il primo blocco
				//intanto scrivo i blocchi che mi entrano nel primo blocco  --------------------PRIMO BLOCCO
				memcpy((char*)data + bytesLetti,(char*)ffb->data,(int)sizeof(ffb->data));
				
				bytesLetti += (int)sizeof(ffb->data);
				
					
				printf("Blocco nel disco: %d\n", f->fcb->fcb.block_in_disk);
				printf("Bytes Letti:  %d  \n",bytesLetti);
				printf("Bytes da Leggere:  %d  \n",bytesDaLeggere);
				 
				bloccoCorrente = ffb->fcb.block_in_disk;
				printf("FINO A QUAAAA\n\n");
				 //----------------------SE CI STA ANCORA ROBA DA LEGGERE
				if(bytesDaLeggere>0){		
					FileBlock* fb = (FileBlock*)malloc(sizeof(FileBlock));
								
					//VALE PER I BLOCCHI SUCCESSIVI AL FIRST
					int next_block =f->fcb->header.next_block;
					//f->current_block->next_block = next_block;
					printf("next_block = %d \n\n", next_block);		
					printf("current next_block = %d \n\n",f->current_block->next_block);
				
					//finchè il prossimo blocco è diverso da -1  e bytes da leggere sono > 0 vado a leggere nel prossimo blocco i bytes restanti
					while((next_block!=-1) && (bytesDaLeggere > 0)){ //---------------------------SE CI STA UN PROSSIMO BLOCCO
						printf("FINO A QUAAAA2222222\n\n");
						//aggiorno il puntatore al bloccoCorrente
						bloccoCorrente = next_block;
											
						//leggo il blocco successivo 
						val = DiskDriver_readBlock(f->sfs->disk,fb, bloccoCorrente);
						if(val == -1){
							printf("Impossibile leggere dal fb\n\n");
							free(ffb);
							free(fb);
							return -1;
						}
						//se i bytes da leggere sono maggiori della dim del blocco, leggo tutto il blocco
						if(bytesDaLeggere > (int)sizeof(fb->data)){	
							printf("FINO A QUAAAA333333\n\n");
							printf("sizzeof(fb->data)  %ld  \n\n", sizeof(fb->data));
							memcpy((char*)data+bytesLetti +1,(char*)fb->data,(int)sizeof(fb->data));
							
							bytesLetti += sizeof(fb->data);				
							bytesDaLeggere -=sizeof(fb->data);
							//sovrascrivo
							
							
							
							printf("Blocco nel disco: %d\n", bloccoCorrente);
							printf("Bytes Letti:  %d  \n",bytesLetti);
							printf("Bytes da Leggere:  %d  \n",bytesDaLeggere);
						}else{ //rientrano sotto la dimensione del blocco che devo leggere 						
							printf("FINO A QUAAAA 4444444444444\n\n");
							memcpy((char*)data ,(char*)fb->data, bytesDaLeggere);
												
							bytesLetti += bytesDaLeggere;
							bytesDaLeggere = 0;
										
							
							
							printf("Blocco nel disco: %d\n", bloccoCorrente);
							printf("Bytes Letti:  %d  \n",bytesLetti);
							printf("Bytes da Leggere:  %d  \n",bytesDaLeggere);
							break;
						} 
						//aggiorno next_block ai blocchi successivi al fb
						next_block = f->current_block->next_block;
					}		
					free(fb);
				
					
			}
			free(ffb);
		}
		
	return bytesLetti;
	}

// returns the number of bytes read (moving the current pointer to pos)
// returns pos on success
// -1 on error (file too short)
int SimpleFS_seek(FileHandle* f, int pos){

	//controlli
	if(f == NULL) return -1;
	if(pos == -1) return -1;
	
	//se pos è maggiore della dimensione del file ritorno -1 
	if(pos>f->fcb->fcb.size_in_bytes){
		printf("Errore impossibile ritornare posizione poichè il file è più corto della posizione scelta\n");
		return -1;
	}
	//Altrimenti inizio a leggere i blocchi dal primo all'ultimo del file
	FirstFileBlock* ffb = (FirstFileBlock*)malloc(sizeof(FirstFileBlock));
	
	int val = DiskDriver_readBlock(f->sfs->disk, ffb, f->fcb->fcb.block_in_disk);
	if ( val == -1){
		printf("Impossibile leggere blocco\n\n");
		return -1;
		free(ffb);
	}
	//se pos è minore della dimensione massima del primo blocco ritorno assegno a f->pos_in_file la dimensione di pos
	if(pos < sizeof(ffb->data)){
		f->pos_in_file = pos;
	}else{
			//altrimenti se è maggiore della dimensione, gli assegno i bytes della dimensione del primo blocco e poi leggo i blocchi successivi
			//finchè non arrivo alla stessa dimensione di pos
			f->pos_in_file = sizeof(ffb->data);
			
			int next_block = f->current_block->next_block;
			
			FileBlock* fb = (FileBlock*)malloc(sizeof(FileBlock));
			
			//calcolo quanto mi devo spostare ancora
			int daLeggere = pos-f->pos_in_file;
			
			//finchè c'è un blocco successivo e  pos non è uguale leggo prox blocco e aggiungo i bytes
			while(next_block!=-1 && f->pos_in_file < pos){
				
				int val = DiskDriver_readBlock(f->sfs->disk, fb, next_block);
				if ( val == -1){
					printf("Impossibile leggere blocco\n\n");
					return -1;
					free(ffb);
				}
				
				if(daLeggere < sizeof(fb->data)){
					
						f->pos_in_file += daLeggere;
						
				}else{
					f->pos_in_file += sizeof(fb->data);
					daLeggere -= sizeof(fb->data);
					next_block = f->current_block->next_block;
				}
			}
			free(fb);
		}
		free(ffb);
	
return f->pos_in_file;
}

// seeks for a directory in d. If dirname is equal to ".." it goes one level up
// 0 on success, negative value on error
// it does side effect on the provided handle
int SimpleFS_changeDir(DirectoryHandle* d, char* dirname){
	
		//controlli
		if(d == NULL){
			printf("Impossibile cambiare directory, riferimento a DirectoryHandle NULLO\n");
			return -1;
		}
		if(dirname == NULL) {
			printf("Impossibile cambiare directory, riferimento a nome directory NULLO\n");
			return -1;
		}
		
		
		//Controllo se dirname = ".."   È IL CASO IN CUI VOGLIO TORNARE INDIETRO DI UNA DIRECTORY  
		if(strncmp("..", dirname, strlen(dirname)) == 0){
			//se directory è null significa che sono la root, non posso andare indietro 
			if(strcmp(d->dcb->fcb.name, "/")==0){
				printf("Sono la root, non ho genitore\n");
				return -1;
			}
			
			//altrimenti se non sono la root cerco il padre della directory corrente con strutture ausiliarie
				
			FirstDirectoryBlock* precedente = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));
			//leggo il blocco che contiene la FirstDirectoryBlock della cartella principale
			int val  = DiskDriver_readBlock(d->sfs->disk, precedente, d->directory->fcb.block_in_disk);
			if(val == -1){
				printf("Errore nella lettura\n");
				free(precedente);
				return -1;
			}
			
			
			//assegno alla directory il genitore
			d->dcb = d->directory;
			d->current_block = &(d->dcb->header);
			d->pos_in_dir = 0;
			d->pos_in_block = d->dcb->fcb.block_in_disk;
			d->directory = precedente;
			free(precedente);
			return 0;
			
		}else{ //ALTRIMENTI CASO IN CUI VOGLIO ENTRARE IN UNA DETERMINATA DIRECTORY
			
			FirstDirectoryBlock* fdb = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));
			//controllo primo blocco directory range (0-num_entries e inferiore allo spazio disponibile a contenere i file)
			int i;
			for (i = 0; (i < (int)sizeof(d->dcb->file_blocks)) && (d->dcb->file_blocks[i] > 0) ; i++){
				//leggo il firstFileBlock del blocco i-esimo e se va a buon fine la lettura controllo che non esista già un file con lo stesso nome
				int val = DiskDriver_readBlock(d->sfs->disk, fdb,d->dcb->file_blocks[i]);	
				if(val == -1){
					printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
					free(fdb);
					exit(1);
				}
				if(strncmp(fdb->fcb.name,dirname,strlen(dirname))==0 && fdb->fcb.is_dir == 1){
					val = DiskDriver_readBlock(d->sfs->disk, fdb,d->dcb->file_blocks[i]);	
					if(val == -1){
						printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
						free(fdb);
						exit(1);
					}
					//se va a buon fine 
					d->pos_in_block = 0;
					d->directory = d->dcb;
					d->dcb = fdb;
					return 0;
				}
			}
			
			
			//controllo tutti gli altri blocchi-------------------
			int next_block =fdb->header.next_block;
			DirectoryBlock* db = (DirectoryBlock*)malloc(sizeof(DirectoryBlock));
			
			while(next_block!=-1){
				//leggo il prossimo blocco
				int val = DiskDriver_readBlock(d->sfs->disk,db,next_block);
				if(val==-1){
					printf("Impossibile leggere il blocco\n");
					free(db);
					free(fdb);
					exit(1);
				}
				
				//lettura andata a buon fine, ora controllo che non ci sia un file con lo stesso nome nell'array di file
				
				for (i =0; (i < (int)sizeof(db->file_blocks)) && (db->file_blocks[i]>0) ; i++){
					val = DiskDriver_readBlock(d->sfs->disk,fdb,db->file_blocks[i]);
					if(val == -1){
						printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
						free(fdb);
						free(db);
						exit(1);
					}
					if(strncmp(fdb->fcb.name,dirname,strlen(dirname))==0 && fdb->fcb.is_dir ==1){
						d->pos_in_block = 0;
						d->directory = d->dcb;
						d->dcb = fdb;
						return 0;
					}
						
				}
				next_block = db->header.next_block;		
			}	
			free(fdb);
		}	
		
			return -1;

}

// creates a new directory in the current one (stored in fs->current_directory_block)
// 0 on success
// -1 on error
int SimpleFS_mkDir(DirectoryHandle* d, char* dirname){
	
		printf("***************CREO UNA NUOVA DIRECTORY***************\n");

		//controlli parametri
		if(dirname == NULL) {
			printf("Impossibile creare directory, dirname è NULLO\n!");
			return -1;
		}
		
		if(d == NULL) {
			printf("Impossibile creare directory, riferimento a directoryHandle è NULLO\n!");
			return -1;
		}
		
		
		printf("Controllo il numero di entries %d \n\n", d->dcb->num_entries);
		//se ci sono entries, quindi directory non è vuota, allora controlla che non ci sia già una directory con lo stesso nome.
		if(d->dcb->num_entries>0){
			//verifico se non esiste già un file con lo stesso nome, se esiste ritorna -1
			int flag = verificoFile(d,dirname);
			if(flag == -1) {
				printf("Impossibile creare directory, già esiste \n");
				return -1;
			}  
		}

		//controllo ci sia spazio all'interno del disco per poter creare un nuovo file
		if(d->sfs->disk->header->free_blocks<1) {
			printf("Impossibile creare nuovo file, disco pieno");
			return -1;
		}

		//cerco l'indice del primo blocco libero
		int firstFreeBlock = DiskDriver_getFreeBlock(d->sfs->disk,0);
		if(firstFreeBlock==-1){
			printf("Non ci sono blocchi liberi");
			return -1;
		}
		
			
		printf("BLOCCO LIBERO TROVATO: %d\n\n\n\n\n", firstFreeBlock);


		//creo FirstDirectoryBlock
		FirstDirectoryBlock* fdb =(FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));
		//header default
		fdb->header.block_in_file = 0;
		fdb->header.next_block= -1;
		fdb->header.previous_block= -1;
		
		//altri parametri del fcb
		fdb->fcb.directory_block = d->dcb->fcb.block_in_disk;
		printf("DIRECTORY GENITORE = %d\n\n", d->dcb->fcb.block_in_disk); 
		fdb->fcb.block_in_disk= firstFreeBlock; //primo blocco libero 
		fdb->fcb.is_dir=1;//file 0, directory 1
		strncpy(fdb->fcb.name, dirname, strlen(dirname));
		fdb->fcb.size_in_blocks= 1; //ho appena creato il file
		fdb->fcb.size_in_bytes=0;//non ci sono elementi
		
		fdb->num_entries = 0;
	   
		//setto tutti i blocchi della directory a zero, poichè ho creato un nuova directory ed è vuota
		memset(fdb->file_blocks, 0, sizeof(fdb->file_blocks));
		

	
		//controllo prima nella fdb corrente

		//con questa variabile tengo conto dell'ultima directory block letta
		int ultimoBloccoLetto = fdb->fcb.block_in_disk;
		int i;
		DirectoryBlock* db = (DirectoryBlock*)malloc(sizeof(DirectoryBlock));

		printf("Dimensione file block %ld    \n\n\n", sizeof(d->dcb->file_blocks));
		//se trovo un blocco libero allora lo uso per inserire il file nella FIRSTDIRECTORYBLOCK
		if(d->dcb->num_entries < (int)sizeof(d->dcb->file_blocks)){
			memcpy(d->current_block,&(d->dcb->header), sizeof(BlockHeader));
			for (i = 0; i < (int)sizeof(d->dcb->file_blocks); i++){
				if(d->dcb->file_blocks[i]==0){
					d->pos_in_dir = i+1;
					printf("TROVATO BLOCCO LIBERO\n");
					//assegno l'indice di blocco del fdb nel disco al blocco libero del file blocks
					d->dcb->file_blocks[i] =  firstFreeBlock;
					//incremento il numero di entries
					d->dcb->num_entries += 1;
					printf("BLOCCOOOOO DOVE DEVO SCRIVERE DELLA WRITE, %d\n\n\n\n\n", firstFreeBlock);
					//vado a scrivere nel blocco che ho individuato come libero.
					DiskDriver_writeBlock(d->sfs->disk, fdb, firstFreeBlock);
					break;
				}
			}
			
		}else{ // controllo nella directory block successiva se c'è un blocco libero
			
			
			
			while(d->dcb->header.next_block!=-1){
				ultimoBloccoLetto = d->dcb->header.next_block;
				printf("OKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK\n\n\n\n");
				
				//controllo nel blocco db
				int val = DiskDriver_readBlock(d->sfs->disk, db, ultimoBloccoLetto);
				if(val==-1){
					printf("Errore lettura del blocco successivo\n");
					return -1;
				}
				//se va a buon fine controllo che ci sia un blocco libero 
				for (i = 0; i < (int)sizeof(db->file_blocks); i++){
					d->pos_in_dir = i;
					if(db->file_blocks[i]==0){
						printf("TROVATO BLOCCO LIBERO\n");
						//assegno l'indice di blocco del ffb nel disco al blocco libero del file blocks
						db->file_blocks[i] = firstFreeBlock;
						//incremento il numero di entries
						d->dcb->num_entries += 1;
						DiskDriver_writeBlock(d->sfs->disk, db, ultimoBloccoLetto);	
						break;
					}
				}
				//Vado al prossimo blocco esistente
				d->dcb->header.next_block = db->header.next_block;
			}
			
			//in questa situazione significa che non ci sono blocchi liberi in nessun blocco di directory, per cui devo creare un nuovo directory block	
			DirectoryBlock* new_db =(DirectoryBlock*) malloc(sizeof(DirectoryBlock));
			
			int primoBloccoLibero = DiskDriver_getFreeBlock(d->sfs->disk,0);
			if(primoBloccoLibero == -1) {
				printf("BLOCCHI ESAURITI\n\n");
				return -1;
			}
				
			new_db->header.previous_block = ultimoBloccoLetto;
			new_db->header.next_block = -1;
			new_db->header.block_in_file = db->header.block_in_file +1;
			
			//imposto tutti i blocchi a 0 
			memset(new_db->file_blocks,0,sizeof(db->file_blocks));
			//assegno al valore del primo blocco della nuova db l'indice del blocco nel disco
			new_db->file_blocks[0] = primoBloccoLibero;
			
			db->header.next_block = primoBloccoLibero;
			
			int val = DiskDriver_writeBlock(d->sfs->disk,new_db,primoBloccoLibero);
			if(val==-1){
				printf("Impossibile scrivere la directory block appena creata nel nuovo blocco libero!\n");
				free(fdb);
				free(db);
				free(new_db);
				return -1;
			}
			free(new_db);
		}

		free(db);
		free(fdb);
	
		printf("***************************FINE CREAZIONE DIRECTORY**********************\n\n\n\n");

		return 0;
	}
	
// removes the file in the current directory
// returns -1 on failure 0 on success
// if a directory, it removes recursively all contained files
int SimpleFS_remove(DirectoryHandle* d, char* filename){
	
		//controlli
		if(d == NULL){
			printf("Impossibile rimuovere il file, riferimento a file system NULL\n\n");
			return -1;
		}
		if(filename == NULL){
				printf("Impossibile rimuovere il file, poichè il nome è NULLO\n\n");
				return -1;
		}
		
		FirstDirectoryBlock* ffb = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));
		int indiceffb;
		//controllo primo blocco directory range (0-num_entries e inferiore allo spazio disponibile a contenere i file)
		int i;
		for (i = 0; (i < (int)sizeof(d->dcb->file_blocks)) && (d->dcb->file_blocks[i] >0) ; i++){
			//leggo il firstFileBlock del blocco i-esimo e se va a buon fine la lettura controllo che non esista già un file con lo stesso nome
			int val = DiskDriver_readBlock(d->sfs->disk, ffb,d->dcb->file_blocks[i]);	
			if(val == -1){
				printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
				free(ffb);
				exit(1);
			}
			if(strncmp(ffb->fcb.name,filename,strlen(filename))==0){
			printf("Trovato File, nell'indice %d\n",i);
			indiceffb = i;	
			break;
			}
		}
		
		
		//controllo tutti gli altri blocchi-------------------
		int next_block = d->dcb->header.next_block;
		DirectoryBlock* db = (DirectoryBlock*)malloc(sizeof(DirectoryBlock));
		
		while(next_block!=-1){
			//leggo il prossimo blocco
			int val = DiskDriver_readBlock(d->sfs->disk,db,next_block);
			if(val==-1){
				printf("Impossibile leggere il blocco\n");
				free(db);
				free(ffb);
				exit(1);
			}
			
			//lettura andata a buon fine, ora controllo che non ci sia un file con lo stesso nome nell'array di file
			
			for (i =0; (i < (int)sizeof(db->file_blocks)) && (db->file_blocks[i]>0) ; i++){
				val = DiskDriver_readBlock(d->sfs->disk,ffb,db->file_blocks[i]);
				if(val == -1){
					printf("Impossibile leggere blocco per verificare se esiste un file con lo stesso nome\n");
					free(ffb);
					free(db);
					exit(1);
				}
				if(strncmp(ffb->fcb.name,filename,strlen(filename))==0){
					printf("Trovato File, nell'indice %d\n",i);
					indiceffb=i;
					break;
				}
					
			}
			next_block = db->header.next_block;		
		}	
		
		
		
		
		return -1;
	}
