#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplefs.h"





// initializes a file system on an already made disk
// returns a handle to the top level directory stored in the first block
DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){
    printf("ENTRO NELLA INIT\n");

    //controlli
	if(fs == NULL || disk == NULL){
		printf("Errore, parametri iniziali\n\n");
		return NULL;
		}
		//creo il directoryHandle, suppongo che un disco ha sempre una directory che inizia dal blocco 0.(root directory)
		DirectoryHandle* rootDirectory = (DirectoryHandle*)malloc(sizeof(DirectoryHandle));

		//assegno al filesystem il disco già creato
		fs->disk = disk;
        fs->blocchi_riservati = disk->header->riservati;

		FirstDirectoryBlock* fdb = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));

        //leggo il primo blocco -->firstDirectoryBlock
		int val = DiskDriver_readBlock(fs->disk, fdb, fs->blocchi_riservati);
		if(val == -1){
			free(fdb);
			printf("Errore di lettura primo blocco\n\n");
			return NULL;
			}

		//Riempio il directoryHandle
		rootDirectory->sfs = fs;
		rootDirectory->dcb = fdb; //primo blocco della directory
		rootDirectory->directory = NULL; //genitore della directory ( in questo caso NULL perchè la directory è la radice)
		//rootDirectory->current_block = ?
		rootDirectory->pos_in_dir = 0; //posizione zero
		rootDirectory->pos_in_block = 0;//posizione zero


        printf("ESCO DALLA SIMPLE_INIT\n");
		return rootDirectory;


	}

// creates the inital structures, the top level directory
// has name "/" and its control block is in the first position
// it also clears the bitmap of occupied blocks on the disk
// the current_directory_block is cached in the SimpleFS struct
// and set to the top level directory

//prende un disco e ci fa la prima directory, poi setta la bitmap tutta a zero ad eccetto blocco della directory che è 1 (diskHeader e blocchi per l'occupazione di bitmap)
//sovrascrivo le info della prima parte.
void SimpleFS_format(SimpleFS* fs, char* filename, int num_blocks){
    printf("sono nel formatkkkkk\n");
	//controlli
	if(fs == NULL){
		printf("Errore\n\n");
        return;
		}


        //inizializzo il disco direttamente qui, quindi setto la bitmap.
        DiskDriver_init(fs->disk, filename, num_blocks);

        //top level directory
		FirstDirectoryBlock* fdb = (FirstDirectoryBlock*)malloc(sizeof(FirstDirectoryBlock));

        printf("blocchi riservati: %d\n\n",fs->disk->header->riservati);

        //indice del primo blocco libero ricercandolo a partire dal blocco zero
        //che sarà il blocco riservato al fcb
        int firstFreeBlock = DiskDriver_getFreeBlock(fs->disk,0);

		//POPOLO IL FIRST DIRECTORY BLOCK

		//--popolo il BlockHeader
		fdb->header.previous_block =-1;
		fdb->header.next_block =-1;
		fdb->header.block_in_file =firstFreeBlock; //posizione del file, se 0 abbiamo il file control block

		//--popolo il FileControBlock
		fdb->fcb.directory_block =-1;//non ha genitore perchè è la root
		fdb->fcb.block_in_disk= firstFreeBlock;//si trova nel primo blocco del disco dopo i blocchi riservati
		strcpy(fdb->fcb.name ,"/");
		fdb->fcb.size_in_bytes = sizeof(FirstDirectoryBlock);
		fdb->fcb.size_in_blocks = 1;
		fdb->fcb.is_dir = 1;//directory

		fdb->num_entries = 0;

        //imposto i blocchi del file della dimensione fdb->file_blocks a zero
        memset(fdb->file_blocks,0,sizeof(fdb->file_blocks));

        //scrivo sulla prima directory(root)
        printf("Scrivo il first directory block nel primo blocco libero\n\n");
		int val = DiskDriver_writeBlock(fs->disk, (void*)fdb, firstFreeBlock);
		if(val==-1){
			printf("Errore nella scrittura del primo blocco (FirstDirectoryBlock)\n\n");
			free(fdb);
			return;
			}
        DiskDriver_flush(fs->disk);

    free(fdb);
    printf("ESCO DALLA FORMAT\n");

  }
//----------------------------------CREATE FILE-------------------------------------------------------
// creates an empty file in the directory d
// returns null on error (file existing, no free blocks)
// an empty file consists only of a block of type FirstBlock
FileHandle* SimpleFS_createFile(DirectoryHandle* d, const char* filename){

    printf("CREO UN NUOVO FILE\n");

    //controlli parametri
    if(filename==NULL||d==NULL) {
    printf("impossibile creare file parametri iniziali non corretti\n!");
    return NULL;
    }

    //verifico se non esiste già un file con lo stesso nome, se esiste esci.
    



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
