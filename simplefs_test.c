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

    printf("\n\n\n\n\n");

  //alloco filesystem
    SimpleFS* sfs = (SimpleFS*) malloc(sizeof(SimpleFS));
    printf("sfs %ld\n", sizeof(sfs));
    //alloco diskDriver
    DiskDriver* disco = (DiskDriver*) malloc(sizeof(DiskDriver));
    printf("disco %ld\n", sizeof(disco));
    sfs->disk = disco;
    printf("provo simpHHHHHHHHHHHHHHHHHHHHHHHHHle_format\n");

    //----------------provo la simpleFS format --------------------------
    SimpleFS_format(sfs,"provae.txt",8);

    printf("provo la simpleFs_init\n");
    //---------------provo la simpleFS init -----------------------------
    SimpleFS_init(sfs,sfs->disk);



    return 0;
}
