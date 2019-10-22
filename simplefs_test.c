#include "simplefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	printf("FirstBlock size %ld\n", sizeof(FirstFileBlock));
	printf("DataBlock size %ld\n", sizeof(FileBlock));
	printf("FirstDirectoryBlock size %ld\n", sizeof(FirstDirectoryBlock));
	printf("DirectoryBlock size %ld\n", sizeof(DirectoryBlock));
	  
	//alloco lo spazio necessario al puntatore del filesystem
	SimpleFS* sfs =(SimpleFS*)malloc(sizeof(SimpleFS));

	//alloco lo spazio necessario al puntatore del diskDriver
	DiskDriver* disco = (DiskDriver*)malloc(sizeof(DiskDriver));

	//Prendo in input il numero blocchi del disco che voglio inserire
	int num_blocchi_disco;
	printf("Inserire numero di blocchi che si vogliono allocare per il DiskDriver : ");
	scanf("%d", &num_blocchi_disco);
	fflush(stdin);
		
	//Prendo in input il nome che voglio assegnare al disco
	char filename[40];
	printf("Inserisci nome da assegnare al disco: ");
	scanf("%s", filename);
	fflush(stdin);
		
	//Inizializzo il disco 
	DiskDriver_init(disco, filename, num_blocchi_disco);
	//sfs->disk = disco;
	//SimpleFS_format(sfs);
	
	//Inizializzo il fileSystem
	DirectoryHandle* d = SimpleFS_init(sfs,disco);
	 
	printf("DirectoryHandle %p\n\n", d);
		
	//***************************CREAZIONE NUOVO FILE*******************************************
		
	char* file1 = "file1";
	char* file2 = "file6";
	char* file3 = "file1";
	//CREO FILE 1
	printf("CREO FILE 1\n\n");
	FileHandle* fh1  = SimpleFS_createFile(d,file1);
	//CREO FILE 2
	printf("CREO FILE 2\n\n");
	FileHandle* fh2 = SimpleFS_createFile(d,file2);
	//CRERO FILE 3
	printf("CREO FILE 3\n\n");
	FileHandle* fh3 = SimpleFS_createFile(d,file3);
		
	printf("Fh1 %p  \n",fh1);
	printf("Fh2 %p  \n",fh2);
	printf("Fh3 %p  \n",fh3);	

	//LEGGO DIRECTORIES
	char* nomiFile=NULL;
	
	//PROVO LA READ
	int readFile = SimpleFS_readDir(&nomiFile, d);
	if(readFile == -1){
		printf("Errore in lettura dei nomi\n");
	} else{
		printf("Ho letto tutti i nomi dei file nella directory\n\n");
	}
	//STAMPO I NOMI
	printf("Nomi dei file: %s	\n\n\n",nomiFile);
	
	SimpleFS_close(fh1);
	//PROVO L'APERTURA DEI FILE
	fh1 = SimpleFS_openFile(d,  file1);
	if(fh1!=NULL){
		printf("E' stato aperto il file %p	\n",fh1);
	}
	SimpleFS_close(fh2);
	fh2 = SimpleFS_openFile(d,  file2);
	if(fh2!=NULL){
		printf("E' stato aperto il file %p	\n",fh2);
	}
	
	//LIBERO GLI SPAZI
	SimpleFS_format(sfs);
	SimpleFS_close(fh1);
	SimpleFS_close(fh2);
	SimpleFS_close(fh3);
	free(d->dcb);
	free(nomiFile);
	free(d);
	close(sfs->disk->fd);
	free(disco);
	free(sfs);
		
	return 0;
}
