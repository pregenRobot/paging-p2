#include "paging.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void main() {

	// Allocate 32 KiB of memory to serve as our store
	void *store = malloc(1<<15);

	// Initialise our mapping table
	void *table = pt_init();

	// Add some basic mappings to the table
	map_page_to_frame(table, 0, 4, false, false);
	map_page_to_frame(table, 1, 3, false, false);
	map_page_to_frame(table, 2, 0, false, false);
	map_page_to_frame(table, 3, 2, false, false);
	map_page_to_frame(table, 4, 1, false, false);
	print_table(table);

	// Now try to write something to our store. Our implementation should transparently use the
	// page table to store the virtual pages into correct physical frames. This example should fit
	// into a single page
	char text[]  = "It was the best of times, it was the worst of times";
	size_t length = strlen(text);
	char *buffer = malloc(length+1);

	store_data(table, store, text, 0, length);
	read_data(table, store, buffer, 0, length);
	printf("%s\n", buffer);

	free(buffer);

	// Now try something longer and see what happens. The data read out should be exactly the same as what was put in
	char text2[] = "It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of light, it was the season of darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to heaven, we were all going direct the other way–in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only.";
	length = strlen(text2);
	buffer = malloc(length+1);

	store_data(table, store, text2, 0, length);
	read_data(table, store, buffer, 0, length);
	printf("%s\n", buffer);

	free(buffer);

	// Cleanup
	free(store);
	free(table);
}
