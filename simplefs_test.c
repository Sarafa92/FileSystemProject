#include "simplefs.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  printf("FirstBlock size %ld\n", sizeof(FirstFileBlock));
  printf("DataBlock size %ld\n", sizeof(FileBlock));
  printf("FirstDirectoryBlock size %ld\n", sizeof(FirstDirectoryBlock));
  printf("DirectoryBlock size %ld\n", sizeof(DirectoryBlock));
  
  
  
  
//************************************************************TESTO LA BITMAP***********************************************************************
 /* 
  printf("*****************************************************BITMAP TEST***********************************************************************\n\n");
	BitMap* bmap = (BitMap*)malloc(sizeof(BitMap));
	int numBits = bmap->num_bits = 40;
	int numBytes = numBits/8;
	
	if(numBytes%8 == 1){
			numBytes+=1;
		}
	
	bmap->entries =  (char*)malloc(sizeof(char) * numBytes);
		


	//Inizializzazione tutti i bit sono free
		for (int i = 0; i < numBits ; i++){
		bmap->entries[i] = 0;
	}
		
	 //---set----//
 printf("Provo la set\n");

 BitMap_set(bmap,3,1);
 BitMap_set(bmap,6,1);
 BitMap_set(bmap,7,1);
 BitMap_set(bmap,11,1);
 BitMap_set(bmap,23,1);
 BitMap_set(bmap,32,1);
 BitMap_set(bmap,33,1);
 BitMap_set(bmap,34,1);
 BitMap_set(bmap,35,1);
 BitMap_set(bmap,36,1);

 //---get----//

int a1 =  BitMap_get(bmap,0,0); //deve ritornare 0
int a2 =  BitMap_get(bmap,0,1); //deve ritornare 3
int a3 =  BitMap_get(bmap,5,1); //deve ritornare 6
int a4 =  BitMap_get(bmap,8,1); //deve ritornare 11
int a5 =  BitMap_get(bmap,16,0);//deve ritornare 16
int a6 =  BitMap_get(bmap,16,1); // deve ritornare 23
int a7 =  BitMap_get(bmap,24,1); // deve ritornare 32
int a8 =  BitMap_get(bmap,33,1); //deve ritronare 33
int a9 =  BitMap_get(bmap,34,0); //deve ritornare 37

printf("Deve tornare 0 -----> %d\n", a1);
printf("Deve tornare 3 -----> %d\n", a2);
printf("Deve tornare 6 -----> %d\n", a3);
printf("Deve tornare 11 -----> %d\n", a4);
printf("Deve tornare 16 -----> %d\n", a5);
printf("Deve tornare 23 -----> %d\n", a6);
printf("Deve tornare 32 -----> %d\n", a7);
printf("Deve tornare 33 -----> %d\n", a8);
printf("Deve tornare 37 -----> %d\n", a9);

	
	//---BLOCKTOINDEX---
	printf("---------BLOCK TO INDEX---------\n\n");
	int num;
	printf("\n\nBlock to index, inserisci un indice\n");
	scanf("%d", &num);
	
	int num= 18;
	BitMapEntryKey coppia = BitMap_blockToIndex(num,bmap);
	
	printf("dato l'indice %d , otteniamo il byte numero: ", num);
	printf("%d" "%s" "%d", coppia.entry_num,", bit numero: ", coppia.bit_num);
	printf("\n\n");
	
	//---INDEXTOBLOCK---
	
	printf("---------INDEX TO BLOCK---------\n\n");
	int trovaloc1 = BitMap_indexToBlock(2,7,bmap);
	
	printf("Dato il byte 2 e il bit 7 ottengo l'indice numero:  ");
	printf("%d",trovaloc1);
	printf("%c",'\n');
	
	int trovaloc2 = BitMap_indexToBlock(2,8,bmap);
	
	printf("Dato il byte 2 e il bit 8 ottengo l'indice numero:  ");
	printf("%d",trovaloc2);
	printf("%c",'\n');
	
	int trovaloc3 = BitMap_indexToBlock(2,4,bmap);
	
	printf("Dato il byte 2 e il bit 4 ottengo l'indice numero:  ");
	printf("%d",trovaloc3);
	printf("%c",'\n');
	


printf("***********************************************************FINE BITMAP TEST*********************************************************\n\n\n");*/

//*************************************************************FINE TEST BITMAP**************************************************************/



	printf("SONO QUIIIIIIIIIIIIIIIIIIIII\n\n\n");
    //alloco lo spazio necessario al puntatore del filesystem
    SimpleFS* sfs =(SimpleFS*)malloc(sizeof(SimpleFS));
		printf("SONO QUIIIIIIIIIIIIIIIIIIIII------------1\n\n\n");

    //alloco lo spazio necessario al puntatore del diskDriver
    DiskDriver* disco = (DiskDriver*)malloc(sizeof(DiskDriver));
   		printf("SONO QUIIIIIIIIIIIIIIIIIIIII--------------2\n\n\n");

    //Prendo in input il numero blocchi del disco che voglio inserire
	int num_blocchi_disco;
    printf("Inserire numero di blocchi che si vogliono allocare per il DiskDriver : ");
    scanf("%d", &num_blocchi_disco);
    fflush(stdin);
   	
   	printf("SONO QUIIIIIIIIIIIIIIIIIIIII--------------------3\n\n\n");

    //Prendo in input il nome che voglio assegnare al disco
    char filename[40];
    printf("Inserisci nome da assegnare al disco: ");
    scanf("%s", filename);
    fflush(stdin);
    
    
    printf("SONO QUIIIIIIIIIIIIIIIIIIIII--------------4\n\n\n");

    //Inizializzo il disco 
	DiskDriver_init(disco, filename, num_blocchi_disco);
	//sfs->disk = disco;
	//SimpleFS_format(sfs);
	
	
	printf("SONO QUIIIIIIIIIIIIIIIIIIIII----------------5\n\n\n");

	 //Inizializzo il fileSystem
	 DirectoryHandle* d = SimpleFS_init(sfs,disco);
	 
	 printf("DirectoryHandle %p\n\n", d);
	  
	printf("SONO QUIIIIIIIIIIIIIIIIIIIII---------------6\n\n\n");

	//***************************CREAZIONE NUOVO FILE*******************************************
	
	  char* file1 = "file1";
	 char* file2 = "file2";
	 char* file3 = "file3";
   		printf("SONO QUIIIIIIIIIIIIIIIIIIIII---------------7\n\n\n");

   
	 FileHandle* fh1 =  (FileHandle*)malloc(sizeof(FileHandle));
		 FileHandle* fh2 = (FileHandle*) malloc(sizeof(FileHandle));
	 FileHandle* fh3 = (FileHandle*) malloc(sizeof(FileHandle));
   		printf("SONO QUIIIIIIIIIIIIIIIIIIIII---------------8\n\n\n");

	printf("CREO FILE 1\n\n");

	fh1 = SimpleFS_createFile(d,file1);
		printf("CREO FILE 2\n\n");
	


	fh2 = SimpleFS_createFile(d,file2);
		printf("CREO FILE 3\n\n");

	fh3 = SimpleFS_createFile(d,file3);
	
	printf("gia esiste il file con questo nome\n\n");
	fh1 = SimpleFS_createFile(d,file2);

	
	printf("Fh1 %p  \n",fh1);
	
	printf("Fh2 %p  \n",fh2);
	
	printf("Fh3 %p  \n",fh3);	
	

	
	//Leggi directory 
	/*
	char* nomiDirectories;
	SimpleFS_readDir(&nomiDirectories,d);
	int i;
		printf("Ho letto tutti i nome delle directories\n\n");
		for (i = 0; i < d->dcb->num_entries ; i++){
			printf("%c",nomiDirectories[i]);
			printf("\n");
		}
	
	*/
	
	free(sfs);
	free(disco);
	

	free(d);



/* ********************************************************FORMATTAZIONE************************************************************************
	SimpleFS_format(sfs,d);*/
	
    return 0;
}
