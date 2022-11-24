#include "paging.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main() {

	void *store = malloc(1<<15);
	void *table = pt_init();

	map_page_to_frame(table, 0, 4, false, false);
	map_page_to_frame(table, 1, 3, false, false);
	map_page_to_frame(table, 2, 0, false, false);
	map_page_to_frame(table, 3, 2, false, false);
	map_page_to_frame(table, 4, 1, false, false);

	char text1[] = "It was the best of times, it was the worst of times,";
	size_t length_1 = strlen(text1);
	char* buffer = malloc(length_1 + 1);
	store_data(table, store, text1, 0, length_1);
	printf("===== stored text 1 =====\n");
	read_data(table, store, buffer, 0, length_1);
	printf("===== read text 1 =====\n");
	printf("%s\n", buffer);
	printf("===== page table =====\n");
	print_table(table);
	free(buffer);

	// TEST 2
	char text2[] = "It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of light, it was the season of darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to heaven, we were all going direct the other way–in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only.";
	size_t length_2 = strlen(text2);
	buffer = malloc(length_2+1);
	store_data(table, store, text2, 0, length_2);
	printf("===== stored text 2 =====\n");
	read_data(table, store, buffer, 0, length_2);
	printf("===== read text 2 =====\n");
	printf("%s\n", buffer);
	free(buffer);

	// TEST 3
	unmap_page(table, 0);
	printf("===== unmapped page 0 =====\n");
	printf("===== page table =====\n");
	print_table(table);
	length_2 = strlen(text2);
	buffer = malloc(length_2+1);
	read_data(table,store,buffer,0, length_2);
	printf("===== read text 2 =====\n");
	printf("%s\n", buffer);
	map_page_to_frame(table, 0, 4, false, false);

	// TEST 4
	printf("===== stored text 3 =====\n");
	char text3[] = "IMPORTANT MESSAGE";
	size_t length_3 = strlen(text3);
	store_data(table, store, text3, 0,length_3);
	printf("===== made page 0 readonly =====\n");
	map_page_to_frame(table, 0, 4, true, false);
	printf("===== page table =====\n");
	print_table(table);
	char text4[] = "NOT AN IMPORTANT MESSAGE";
	size_t length_4 = strlen(text4);
	store_data(table, store, text4, 0, length_4);
	printf("=====  attempted to store text 4 to page 0 =====\n");
	buffer = malloc(length_4 + 1);
	read_data(table, store, buffer, 0, length_3);
	printf("=====  attempted to read text 4 from page 0 =====\n");
	printf("%s\n",buffer);

	free(store);
	free(table);
}
