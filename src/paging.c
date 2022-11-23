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

	uint16_t* pt = (uint16_t*) table;

	uint16_t page_index = virtual_address >> 7;
	uint16_t frame_index = pt[page_index] >> 3;
	uint16_t offset = virtual_address & OFFSET_MASK;
	uint16_t physical_address = (frame_index << 7) + offset;

	return physical_address;
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable) {

	uint16_t* pt = (uint16_t*)table;
	
	// 9 bit frame address shifted to most significant bits of the row
	uint16_t row = frame_number<<(3);

	/**
	 * INCREASING SIGNIFICANCE
	 * 0 -- 0 unallocated, 1 allocated
	 * 1 -- readonly bit
	 * 2 -- executable indicator bit
	 * 3:11 frame address
	*/

	row+= 0b1; // allocated

	if(readonly){
		row+=0b10;
	}

	if(executable){
		row+=0b100;
	}

	pt[page_number] = row;
}


void print_table(void* table) {

	uint16_t* pt = (uint16_t*) table;

	printf("PAGE TABLE: \n");
	printf("PAGE ID   | FRAME ID  | ERA\n");
	for(int i = 0; i < PAGETABLE_ROWS; i++){
		if(pt[i]&0b1 == 1){
			char* pt_row = int_to_bytes(pt[i],12);
			char* page_id = int_to_bytes(i,9);
			char* frame_id = int_to_bytes(pt[i]>>3, 9);
			char* meta = int_to_bytes(pt[i]&0b111,3);
			printf("%s | %s | %s\n",page_id, frame_id, meta);
			free(pt_row);
		}
	}
}

void unmap_page(void* table, uint16_t page_number) {

	uint16_t* pt = (uint16_t*) table;
	pt[page_number]-=1;

}

void store_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length) {
	char* memory = (char*) store;
	char* data = (char*) buffer;
	
	int i = 0;
	int page;
	int offset;
	for(i = 0; i < length; i++){

		offset = i % 128;
		page = i / 128;

		uint16_t physical_address = virtual_to_physical(table,i);
		memcpy(&memory[physical_address], &data[i],1);
	}
}

void read_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length) {

	char* memory = (char*) store;
	char* data = (char*) buffer;

	int i = 0;
	int page;
	int offset;

	for(i = 0; i < length; i++){
		offset = i % 128;
		page = i / 128;

		uint16_t physical_address = virtual_to_physical(table, i);
		memcpy(&data[i],&memory[physical_address],1);
	}

}

