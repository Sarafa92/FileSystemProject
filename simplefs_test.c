#include "simplefs.h"
#include "bitmap.h"
#include "disk_driver.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  printf("FirstBlock size %ld\n", sizeof(FirstFileBlock));
  printf("DataBlock size %ld\n", sizeof(FileBlock));
  printf("FirstDirectoryBlock size %ld\n", sizeof(FirstDirectoryBlock));
  printf("DirectoryBlock size %ld\n", sizeof(DirectoryBlock));



    //alloco lo spazio necessario al filesystem
    SimpleFS* sfs =(SimpleFS*)malloc(sizeof(SimpleFS));
    printf("sfs %ld\n", sizeof(sfs));
    //alloco lo spazio necessario al diskDriver
    DiskDriver* disco = (DiskDriver*) malloc(sizeof(DiskDriver));
    printf("disco %ld\n", sizeof(disco));

    //numero blocchi del disco da input
    int num_blocchi_disco;
    printf("Inserire numero di blocchi che si vuole allocare per il DiskDriver : ");
    scanf("%d", &num_blocchi_disco);

    //nome disco da input
    char  filename [40];
    printf("Inserisci nome da assegnare al disco");
    scanf("%s", filename);

    //inizializzo DISK DRIVER
   DiskDriver_init(disco,filename,num_blocchi_disco);

    SimpleFS_init(sfs,disco);

char buff[40] = "ciao";

DiskDriver_writeBlock(disco,buff,5);




    return 0;
}

