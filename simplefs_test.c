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
		printf("\n\nInserire numero di blocchi che si vogliono allocare per il DiskDriver \nLA DIMENSIONE DEVE ESSERE MAGGIORE DI 10  : ");
		scanf("%d", &num_blocchi_disco);
		fflush(stdin);
		
		//Prendo in input il nome che voglio assegnare al disco
		char filename[40];
		printf("\nInserisci nome da assegnare al disco: ");
		scanf("%s", filename);
		fflush(stdin);
		
		//Inizializzo il disco 
		DiskDriver_init(disco, filename, num_blocchi_disco);
		sfs->disk = disco;
		//SimpleFS_format(sfs);
	
		//Inizializzo il fileSystem
		DirectoryHandle* d = SimpleFS_init(sfs,sfs->disk);
		 
		printf("DirectoryHandle %p\n\n", d);
		
			//***************************CREAZIONE NUOVO FILE*******************************************
		
		char* file1 = (char*)"file1";
		char* file2 = (char*)"file2";
		char* file3 = (char*)"file1";
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
		char* nomiFile =(char*) "";
	
		//*********************************PROVO LA READ DIR*************************
		int readFile = SimpleFS_readDir(&nomiFile, d);
		if(readFile == -1){
			printf("Errore in lettura dei nomi\n");
		} else{
			printf("Ho letto tutti i nomi dei file nella directory\n\n");
		}
		//STAMPO I NOMI
		printf("Nomi dei file:  %s	\n\n\n",nomiFile);
	
	
		free(fh1);	
		//*********************************APRO IL FILE 1****************************************
		fh1 = SimpleFS_openFile(d,  file1);
		if(fh1!=NULL){
			printf("E' stato aperto il file con nome:  %s	\n\n\n",fh1->fcb->fcb.name);
		}
		free(fh2);
		//*******************************APRO IL FILE 2******************************************
		fh2 = SimpleFS_openFile(d,  file2);
		if(fh2!=NULL){
			printf("E' stato aperto il file con nome: %s	\n\n\n",fh2->fcb->fcb.name);
		}
			
		
		//************************************SCRIVO SUL FILE1************************************************
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
		char daScrivere2[1500]="" ;
		for (i = 0; i < 1500; i++)
		{
		
			daScrivere2[i]='b';
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
		printf("BYTES_SEEK mi deve tornaare -1 :  %d \n", bytes_seek);
		int bytes_seek2 = SimpleFS_seek(fh1, 400);
		if(bytes_seek2 ==-1){
		 printf("Errore di seek\n\n");
		}
		printf("BYTES_SEEK2 mi deve tornare 400: %d \n", bytes_seek2);
		
		int bytes_seek3 = SimpleFS_seek(fh2, 1000);
		if(bytes_seek3 ==-1){
		 printf("Errore di seek\n\n");
		}
		printf("BYTES_SEEK3 mi deve tornare 1000:  %d \n", bytes_seek3);
		
		//****************FINE PROVA SEEK******************
		
		//******************CREO DIRECTORY*****************
		
		printf("**************************CREO DIRECTORY***********************************\n\n");
		
		char* directory = (char*)"Directory1";
		
		int creodir = SimpleFS_mkDir(d, directory);
		if(creodir == -1){
			printf("Impossibile creare directory\n\n");
		}
		
		printf("Directory Creata\n\n");
		
		
		//*********************PROVO CHANGE DIR******************************************
		int cambioDir = SimpleFS_changeDir(d, directory);
		if(cambioDir == -1){
			printf("Errore nel cambio directory\n");
		} else{
			printf("Mi sono spostata in : %s\n", d->dcb->fcb.name);
		}
		
		printf("Creo una cartella nella directory 'Directory1'\n");
		//*********************PROVO MKDIR******************************************
		char* directoryname = (char*)"cartella";
		
		creodir = SimpleFS_mkDir(d, directoryname);
		if(creodir == -1){
			printf("Errore nella creazione della cartella\n");
		}
		
		//*********************PROVO REMOVE******************************************	
		
    
		int rimuovi = SimpleFS_remove(d, directoryname);
		if(rimuovi == -1){
			printf("Errore Rimozione\n");
		} else{
			printf("Ho rimosso la cartella\n");
		}
		
		printf("Creo un file nella directory 'Directory1'\n");
		
		char* file6 = (char*)"file1";
		FileHandle* fh6 = SimpleFS_createFile(d,file6);
		fh6 = SimpleFS_createFile(d, file6);
    
		printf("%p\n\n", fh6);

		
		char* indietro = (char*)"..";
		cambioDir = SimpleFS_changeDir(d, indietro);
		if(cambioDir == -1){
			printf("Errore nel cambio directory\n");
		} else{
			
			printf("Mi sono spostata in : %s\n", d->dcb->fcb.name);
		}
 
		
		
		//PROVO FORMATTAZIONEEEE
		SimpleFS_format(sfs);


		//LIBERO GLI SPAZI
		
		free(fh1->fcb);
		free(fh2->fcb);
		
		SimpleFS_close(fh1);
		SimpleFS_close(fh2);
		SimpleFS_close(fh3);
			
		
		free(nomiFile);
		free(d->dcb);
		free(d);
		close(sfs->disk->fd);
		free(disco);
		free(sfs);
		
		return 0;
}

