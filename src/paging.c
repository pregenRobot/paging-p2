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
	uint16_t frame_index = pt[page_index] >> 4;
	uint16_t offset = virtual_address & OFFSET_MASK;
	uint16_t physical_address = (frame_index << 7) + offset;

	return physical_address;
}

void pt_block_continue(void* table, uint16_t page_number){
	uint16_t* pt = (uint16_t*)table;

	// if row indicates (CHAIN FINISHED), change to (CHAIN CONTINUE)
	if ((pt[page_number]&0b11) == 0b11){
		pt[page_number]-=0b1;
	}
}

void pt_block_finish(void* table, uint16_t page_number){
	uint16_t* pt = (uint16_t*)table;

	if((pt[page_number]&0b11) == 0b10){
		pt[page_number]+=0b1;
	}
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable) {

	uint16_t* pt = (uint16_t*)table;
	
	// 9 bit frame address shifted to most significant bits of the row
	uint16_t row = frame_number<<(4);

	/**
	 * INCREASING SIGNIFICANCE
	 * 0:1 -- 00 unallocated, 11 allocated (CHAIN FINISH) 10 allocated (CHAIN CONTINUE)
	 * 2 -- readonly bit
	 * 3 -- executable indicator bit
	 * 4:12 frame address
	*/

	row+= 0b11; // 11 chain finished

	if(readonly){
		row+=0b100;
	}

	if(executable){
		row+=0b1000;
	}

	pt[page_number] = row;
}


void print_table(void* table) {

	uint16_t* pt = (uint16_t*) table;

	for(int i = 0; i < PAGETABLE_ROWS; i++){
		char* pt_row = int_to_bytes(pt[i],13);

		// GET ALLOCATION STATUS BITS
		int allocation_status = pt[i]&0b11;

		if(allocation_status == 2 || allocation_status == 3){ // frame value is assigned
			printf("%d : %s \n", i, pt_row);
			free(pt_row);
		}
	}
}

void unmap_page(void* table, uint16_t page_number) {

	uint16_t* pt = (uint16_t*) table;
	pt[page_number]&=0b10;

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
		pt_block_continue(table,page);

		if(i == length - 1){
			pt_block_finish(table,page);
		}
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

