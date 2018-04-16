# COMP323 Project 3 - Designing A Virtual Memory Manager
11/25/17
COMP323

To compile the program use:
"make clean; make"

To run:
"./proj3 addresses.txt"


Notes:
Because this was a simulation, and time was limited (by my work schedule) certain things 
had to be implemented in less than ideal ways. First, the TLB should be a very fast 
associative array (like a hashmap), but in my implementation is is searched like a regular 
array. Also, I would have liked to implement the freeFrameList as an actual list, rather 
than an array. My replacement strategy for the TLB is just FIFO, and I think the book is 
using LRU, which is why their TLB hit rate is slightly higher than mine. But given the 
circumstances, I think this meets my goals for this project. If there were more time 
I would love to polish it more (more functions, correct datastructures for TLB and 
pagetable, etc).

Thanks,
Andy Sisinger
