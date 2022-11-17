#include "paging.h"
#include <stdlib.h>
#include <stdio.h>
#include <glib-2.0/glib.h>

// Your implementation goes in here!

void* pt_init() {
	return NULL;
}

uint16_t virtual_to_physical(void* table, uint16_t virtual_address) {
	return -1;
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable) {
}

void print_table(void* table) {
}

void unmap_page(void* table, uint16_t page_number) {
}

void store_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length) {
}

void read_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length) {
}

