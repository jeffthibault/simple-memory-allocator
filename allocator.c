#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

// global definition of the size of a block
#define BLOCK_SIZE sizeof(block_t)

// structure that defines a block of memory
typedef struct block {
	size_t size;
	struct block* next;
	int free;
} block_t;

// global variable for first block of memory
block_t* head = NULL;

/*
 * Function: find_best_block
 * Description: Traverses a linked list of memory blocks to 
 * 		find the best fitting block for the memory to 
 * 		be allocated.
 * size: number of bytes to be allocated
 * return: the best block to allocate the memory to.
 * 	   returns NULL if there is no block that exists that
 * 	   can accomodate the number of bytes to be allocated
 */
block_t* find_best_block(size) {
	block_t* curr = head;
	block_t* bestBlock = NULL;
	size_t min = UINT_MAX;
	while(curr != NULL) { //check each existing block to find the best fit
		if (curr->free && curr->size >= size) {
			// the current block is free and sufficitently sized
			if (curr->size - size <= min) {
				// the current block is the best fit
				bestBlock = curr;
				min = curr->size - size;
			}
		}
		curr = curr->next;
	}

	if (bestBlock)
		return bestBlock;
	return NULL; // no free and sufficiently sized blocks available
}

/*
 * Function: mymalloc
 * Description: allocates memory for a specified number of bytes.
 * 		If a free, sufficiently large enough block of memory 
 * 		exists, the memory will be allocated there. Otherwise,
 * 		the heap will be extended by the specified size plus 
 * 		the size of the block header
 * s: specified number of bytes to be allocated
 * return: a pointer to the memory that has been allocated.
 * 	   returns NULL if the specified size is 0
 */
void* mymalloc(size_t s) {
	if (s == 0)
		return NULL;
	
	block_t* block;
	block = find_best_block(s);
	if (block == NULL) { // there was no existing block available
		block = sbrk(s + BLOCK_SIZE); //extend the heap
		if (block == (void*) -1)
			return NULL;
		block->size = s;
		block->free = 0;
		block->next = NULL;
		if (head == NULL) //list is empty
			head = block;
		else { //extend the list of mem blocks
			block_t* it = head;
			while (it->next != NULL)
				it = it->next;
			it->next = block;
		}
	}
	else //found an existing block, mark it as not free
		block->free = 0;
	
	printf("malloc %zu bytes\n", block->size);
	return (void*)(block + BLOCK_SIZE);
}

/*
 * Function: mycalloc
 * Description: allocates memory for a specified number of bytes
 * 		and initializes each byte to 0. If a free, sufficiently 
 * 		large enough block of memory exists, the memory will 
 * 		be allocated there. Otherwise, the heap will be extended 
 * 		by the specified size plus the size of the block header
 * nmemb: number of elements to allocate
 * s: size of each element
 * return: a pointer to the memory that has been allocated.
 * 	   returns NULL if the requested block cannot be allocated.
 */
void* mycalloc(size_t nmemb, size_t s) {
	if (nmemb == 0 || s == 0)
		return NULL;

	block_t* block = mymalloc(nmemb * s);
	if (block == NULL)
		return NULL;

	memset(block, 0, nmemb * s);
	block = (block_t*)block - BLOCK_SIZE;
	printf("calloc %zu bytes\n", block->size);
	return (void*)(block + BLOCK_SIZE);
}

/*
 * Function: myfree
 * Description: deallocates a block of memory
 * ptr: pointer to previously allocated block of memory
 * return: none
 */	
void myfree(void *ptr) {
	if (ptr == NULL)
		return;
	
	block_t* block = (block_t*)ptr - BLOCK_SIZE;
	block->free = 1;
	printf("Freed %zu bytes\n", block->size);
}	
