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
	
	//************************************SCRIVO SUL FILE HANDLE 1************************************************
		int i;
		char daScrivere[500]= "L'UNESCO definisce dal 1984 l'analfabetismo funzionale come «la condizione di una persona incapace di comprendere, valutare, usare e farsi coinvolgere da testi scritti per intervenire attivamente nella società, per raggiungere i propri obiettivi e per sviluppare le proprie conoscenze e potenzialità»[2]. Il termine, coniato all'interno di un'indagine sui nuclei familiari svolta dalle Nazioni Unite nel 1984, fu introdotto per sovvenuta esigenza ";
		
	
		int write = SimpleFS_write(fh1, &daScrivere,sizeof(daScrivere));
		if(write == -1){
			printf("Impossibile Scrivere su file\n");
		} else{
			printf("Ho scritto n° bytes %d\n", write);
		}
		printf("Posizione nel file: %d\n", fh1->pos_in_file);
		printf("Dimensione del file: %d\n", fh1->fcb->fcb.size_in_bytes);
		
		
		
		//***************************************SCRIVO SUL FILE HANDLE 2**********************************
		char daScrivere2[1500] ;
		for (i = 0; i < 1500; i++)
		{
			daScrivere2[i]='a';
		}
		
		int write2 = SimpleFS_write(fh2, (char*)daScrivere2,sizeof(daScrivere2));
		if(write2 == -1){
			printf("Impossibile Scrivere su file\n");
		} else{
			printf("Ho scritto n° bytes %d\n", write2);
		}
		printf("Posizione nel file: %d\n", fh2->pos_in_file);
		printf("Dimensione del file: %d\n", fh2->fcb->fcb.size_in_bytes);
		printf("Blocco nel disco: %d\n", fh2->fcb->fcb.block_in_disk);
	
	//*******************************PROVO LA READ******************************
		char daLeggere[300]={""};
		
		int letti = SimpleFS_read(fh1,&daLeggere,300);
		if(letti == -1){
			printf("Impossibile Leggere dal file\n");
		} else{
			printf("Ho letto n° bytes %d\n", letti);
		}
		printf("Dimensione del file: %d\n", fh1->fcb->fcb.size_in_bytes);
		printf("Blocco nel disco: %d\n", fh1->fcb->fcb.block_in_disk);
		
		for (i = 0; i < 300; i++){
			printf("%c", daLeggere[i]);
		}
		printf("\n\n");
		//*******************************FINE PROVA DELLA READ******************************
		
		//*******************PROVO LA SEEK*************************
		int bytes_seek = SimpleFS_seek(fh1, 700);
		if(bytes_seek ==-1){
		 printf("Errore di seek\n\n");
		}
		printf("BYTES_SEEK  %d \n", bytes_seek);
		int bytes_seek2 = SimpleFS_seek(fh1, 400);
		if(bytes_seek2 ==-1){
		 printf("Errore di seek\n\n");
		}
		printf("BYTES_SEEK2  %d \n", bytes_seek2);
		
		int bytes_seek3 = SimpleFS_seek(fh2, 1000);
		if(bytes_seek3 ==-1){
		 printf("Errore di seek\n\n");
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
