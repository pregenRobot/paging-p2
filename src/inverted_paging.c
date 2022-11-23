#include "paging.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Your implementation goes in here!

char* int_to_bytes(uint32_t val,int bits){
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
	// max 22 bits pid on lab machines - cat /proc/sys/kernel/pid_max
    uint16_t* pt = malloc(((uint32_t)PAGETABLE_ROWS) * MAX_PID * sizeof(uint16_t));
    return pt;
}


int32_t pt_locator(uint16_t page_number){
	uint32_t locator = ((uint32_t)(getpid() % (1<<MAX_PID))) << (9) ; 
	locator += page_number;
	return locator;
}

uint16_t virtual_to_physical(void* table, uint16_t virtual_address){
	uint16_t* pt = (uint16_t*) table;

	uint16_t page_index = virtual_address >> 7;
	uint16_t frame_index = pt[pt_locator(page_index)] >> 3;
	uint16_t offset = virtual_address & OFFSET_MASK;
	uint16_t physical_address = (frame_index << 7) + offset;

	return physical_address;
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable){
	// GET PID of current process
	// 15 bits pid , 9 bits frame number, 2 bits protection, 1 bit allocation status

	uint16_t* pt = (uint16_t*)table;
	uint16_t row = frame_number << (3);

	row+= 0b1; //allocated

	if(readonly){
		row+= 0b10;
	}

	if(executable){
		row+=0b100;
	}

	pt[pt_locator(page_number)] = row;
}

void print_table(void* table){
	uint16_t* pt = (uint16_t*) table;

	for(int i = 0; i < ((uint32_t)PAGETABLE_ROWS) * MAX_PID; i++){
		char* pt_row = int_to_bytes(pt[i],12);
		printf("%d : %s \n", i, pt_row);
		free(pt_row);
	}
}


void unmap_page(void* table, uint16_t page_number){
	uint16_t* pt = (uint16_t*) table;
	uint32_t locator = pt_locator(page_number);
	pt[locator] -= 1;
}

void store_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length){
	char* memory = (char*) store;
	char* data = (char*) buffer;

	int i = 0;
	int page;
	int offset;
	for(i = 0; i < length; i++){
		offset = i % 128;
		page = i / 128;
		uint16_t physical_address = virtual_to_physical(table, i);
		memcpy(&memory[physical_address],&data[i],1);
	}
}

void read_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length){
	
	char* memory = (char*) store;
	char* data = (char*) buffer;

	int i = 0;
	int page;
	int offset;

	for(i = 0; i < length; i++){
		offset = i % 128;
		page = i / 128;

		uint16_t physical_address = virtual_to_physical(table, i);
		memcpy(&data[i], &memory[physical_address],1);
	}
}