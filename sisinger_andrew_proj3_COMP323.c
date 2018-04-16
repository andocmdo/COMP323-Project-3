/*  Andy Sisinger
    Project 3 - Virtual Memory Manager
    11/25/17
    COMP323
*/

#include <stdio.h>      /* For basic IO */
#include <stdlib.h>     /* for size_t used in fread() */
#include "proj3.h"      /* this project header file */

/* Globals */
signed char mem[MEMORY_SIZE];  /* this is 256 frames of 256 bytes simulating physical memory */
int pageTable[PAGE_SIZE];      /* page table keeps the references to physical memory */
int freeFrameList[NUM_FRAMES];  /* array of free frames. 0 is free, 1 is used */
struct tlbEntry tlb[TLB_SIZE];  /* this should have been a fast hashmap, but for this excercise this will do */
int tlbIndex;           /* used for bookeeping TLB entries.*/
FILE * filePointer;             /* for opening files */
size_t readCount;               /* for checking reads on files */

int main (int argc, char *argv[])
{
  /* Get commandline arguments */
  if (argc != 2) {
    printf("\nUsage: ./proj3 [addresses.txt]\n\n");
    exit(1); // exit with error if not the right amount of cmdline args
  }
  /* open the file  */
  filePointer = fopen(argv[1], "r");
  if (filePointer==NULL) {
    printf("\nError opening file!\n");
    exit(1);
  }
  /* count the number of lines (which are our addresses) in the file */
  /* because we can't create the array without knowing how big it is first */
  unsigned int lines = 0;
  char line[10]; /* reasonable length for each line in the addresses.txt file */
  while (fgets(line, 10, filePointer) != NULL) {
    lines++;
  }

  /* now make an array of addresses from the lines in the file */
  unsigned int addresses[lines];
  rewind(filePointer);
  for (int i=0; i<lines; i++) {
    if (fgets(line, 10, filePointer) != NULL) {
      addresses[i] = atoi(line);
    }
  }

  /* now that we have the addresses we will now translate logical to physical */
  /* we need to close the addresses.txt file, and open the BACKING_STORE.bin */
  fclose(filePointer);
  filePointer = fopen(BACKING_STORE_FILENAME, "rb");
  if (filePointer==NULL) {
    printf("\nError opening file!\n");
    exit(1);
  }

  /* first we will setup/initialize our pagetable and free frame list */
  for (int i=0; i<NUM_FRAMES; i++) {
    pageTable[i] = -1;      /* -1 denotes no entry in page table */
    freeFrameList[i] = 0;   /* 0 is empty frame, 1 is used */
  }
  /* now initialize the TLB */
  tlbIndex = 0;
  for (int i=0; i<TLB_SIZE; i++) {
    tlb[i].pageNumber = -1; /* -1 is empty/no-entry */
    tlb[i].frameNumber = -1;
  }

  unsigned int offsetMask = 255;    /* 0000000011111111b */
  unsigned int pageMask = 65280;    /* 1111111100000000b */
  unsigned int pageStartAddress;
  unsigned int pageNumber;
  unsigned int offset;
  unsigned int physicalAddress; /* physical address in memory */
  int pageFaults = 0;
  int tlbHits = 0;

  /* now we go through every logical address from file, check TLB and pageTable,
    and load into mem if necessary */
  for (int i=0; i<lines; i++) {
    pageStartAddress = pageMask & addresses[i]; /* bitmask for page */
    pageNumber = pageStartAddress / PAGE_SIZE;  /* to get logical pagenumber */
    offset = offsetMask & addresses[i];         /* bitmask for offset part of address */

    /* check TLB */
    if (tlbContains(pageNumber)) {
      /* TLB hit */
      physicalAddress = tlbGet(pageNumber) * PAGE_SIZE + offset;
      tlbHits++;
    } else {
      /* TLB miss */
      if (pageTable[pageNumber] == -1) {
        /* pagefault */
        pageFaults++;
        /* find a free frame */
        int freeFrame = getFreeFrame();
        if (freeFrame == -1) { /* this would be where we would have a frame replacement strategy implemented */ }
        /* load a 256 byte page into the physical memory frame */
        fseek(filePointer, pageStartAddress, SEEK_SET); /* read starting at pageAddress */
        readCount = fread( &mem[freeFrame*PAGE_SIZE], 1, PAGE_SIZE, filePointer); /* read 256 byte page */
        if (readCount <= 0) { printf("\nError reading BACKING_STORE.bin file!\n"); exit(1); }
        pageTable[pageNumber] = freeFrame; /* the page table points to physical frame we just loaded */
        freeFrameList[freeFrame] = 1; /* we filled this frame, mark it used */
      }
      /* compute the physical address, either since it is in memory, or because we just added it from above */
      physicalAddress = pageTable[pageNumber]*PAGE_SIZE + offset;
      /* add to TLB */
      tlbAdd(pageNumber, pageTable[pageNumber] );
    }

    /* print everything out */
    printf("Virtual address: %i Physical address: %i Value: %d\n", addresses[i], physicalAddress, mem[physicalAddress]);
  }

  /* final statistics */
  printf("Number of Translated Addresses = %i\n", lines);
  printf("Page Faults = %i\n", pageFaults);
  printf("Page Fault Rate = %.3f\n", (float)pageFaults/lines);
  printf("TLB Hits = %i\n", tlbHits);
  printf("TLB Hit Rate = %.3f\n", (float)tlbHits/lines);

  /* Clean up */
  fclose(filePointer);
  return 0;
}

/*
this should be implemented as a list/set or something faster, but for
this excercise it works fine, since we never have to replace anything in
memory
*/
int getFreeFrame() {
  for (int i = 0; i<NUM_FRAMES; i++) {
      if (freeFrameList[i] == 0) {return i;}
  }
  return -1; /* every frame is full */
}

/* add an entry to the TLB, and if full, pick something to replace/overwrite (FIFO) */
void tlbAdd(int p, int f) {
  tlb[tlbIndex].pageNumber = p;
  tlb[tlbIndex].frameNumber = f;
  if (++tlbIndex >= (TLB_SIZE -1) ) { tlbIndex = 0;} /* wrap */
}

/* again, this should have been a really fast hashmap, but not enough time to implement that... */
int tlbGet(int p) {
  for (int i=0; i<TLB_SIZE; i++) {
    if (tlb[i].pageNumber == p) {
      return tlb[i].frameNumber;
    }
  }
  /* not found */
  return -1;
}

/* does the TLB contain an entry for a certain pageNumber? */
int tlbContains(int p) {
  if (tlbGet(p) == -1) {
    return 0; /* false */
  }
  return 1; /* true */
}
