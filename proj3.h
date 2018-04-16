/*  Andy Sisinger
    Project 3 - Header file
*/

#define BACKING_STORE_FILENAME "BACKING_STORE.bin"
#define MEMORY_SIZE 65536
#define PAGE_SIZE 256
#define NUM_FRAMES 256
#define TLB_SIZE 16

struct tlbEntry
{
    int pageNumber;
    int frameNumber;
};

int getFreeFrame();
int tlbContains(int p);
void tlbAdd(int p, int f);
int tlbGet(int p);
