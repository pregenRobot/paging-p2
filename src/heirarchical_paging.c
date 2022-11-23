#include "paging.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Your implementation goes in here!

char* int_to_bytes(uint16_t val,int bits){
	char* buff = malloc(bits + 1);

	for(int i = bits - 1; i >=0;i--) {
		if (val == 0){
			buff[i] = '0';
		}else if (val & 1) { // Least significant bit is 1.
			buff[i] = '1';
		}else{
			buff[i] = '0';
		}
		val >>= 1;
	}
	buff[bits] = '\0';

	return buff;
}


void* pt_init() {

	uint16_t* pt = malloc(PAGETABLE_ROWS * sizeof(uint16_t));
	return pt;
}

uint16_t virtual_to_physical(void* table, uint16_t virtual_address) {

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

