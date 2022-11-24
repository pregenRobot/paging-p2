#include "paging.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
	uint32_t* pt = malloc(sizeof(uint32_t) * PAGETABLE_ROWS);
    return pt;
}

uint16_t virtual_to_physical(void* table, uint16_t virtual_address){

	uint32_t* pt = (uint32_t*) table;
	uint32_t page_index = virtual_address >> 7;

	pid_t pid = getpid();
	int frame_index;
	for(int i = 0; i < PAGETABLE_ROWS; i++){
		uint32_t row_pid = pt[i] >> 12;
		uint32_t row_page_number = (pt[i] & 4095) >> 3; // 0b00000000000000000000111111111111
		if(page_index == row_page_number && row_pid == pid){
			frame_index = i;
			break;
		}
	}

	uint16_t offset = virtual_address & OFFSET_MASK;
	uint16_t physical_address = (frame_index << 7) + offset;

	return physical_address;
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable){
	// 20 bits pid , 9 bits frame number, 2 bits protection, 1 bit allocation status
	uint32_t* pt = (uint32_t*)table;
	pid_t pid = getpid();

	uint32_t row = ((uint32_t)pid) % (1<<MAX_PID_REF);
	row = row<<9;
	row+=page_number;
	row= row <<3;

	row+=0b1;
	if(readonly){
		row+=0b10;
	}
	if(executable){
		row+=0b100;
	}

	pt[frame_number] = row;
}

void print_table(void* table){

	uint32_t* pt = (uint32_t*) table;
	printf("FRAME INDEX      | PID                  | PAGE ID   | ERA\n");
	for(uint16_t i = 0; i < PAGETABLE_ROWS; i++){
		if(pt[i]&0b1 == 1){
			char* pt_row = int_to_bytes(pt[i],32);
			char* pid = int_to_bytes(pt[i]>>12,20);
			char* page_id = int_to_bytes((pt[i]&0b00000000000000000000111111111000)>>3,9);
			char* meta = int_to_bytes(pt[i]&0b111, 3);
			char* frame_id = int_to_bytes(i,16);
			// printf("%d, %d, %s \n", i, (uint16_t)pt[i], pt_row);
			printf("%s | %s | %s | %s\n", frame_id, pid, page_id, meta);
			free(pt_row);
		}
	}
}


void unmap_page(void* table, uint16_t page_number){
	uint32_t* pt = (uint32_t*) table;
	pid_t pid = getpid();

	for(int i = 0; i < PAGETABLE_ROWS; i++){
		uint32_t row_pid = pt[i] >> 12;
		uint32_t row_page_number = (pt[i] & 4095) >> 3; // 0b00000000000000000000111111111111
		if(page_number == row_page_number && row_pid == pid){
			pt[i]-=1;
			return;
		}
	}
}

bool virtual_address_is_mapped(void* table, uint16_t virtual_address){
	uint32_t* pt = (uint32_t*) table;

	pid_t pid = getpid();
	uint16_t page_number = virtual_address >> 7;

	for(int i = 0; i < PAGETABLE_ROWS; i++){
		uint32_t row_pid = pt[i] >>  12;
		uint32_t row_page_number = (pt[i] & 4095) >> 3;
		if(page_number == row_page_number && row_pid == pid){
			uint16_t allocation_status = pt[i] & 0b1;
			if(allocation_status == 1){
				return true;
			}else{
				return false;
			}
		}
	}
	return false;
}

bool virtual_address_is_writeable(void* table, uint16_t virtual_address){
	uint32_t* pt = (uint32_t*) table;
	uint16_t page_number = virtual_address >> 7;

	pid_t pid = getpid();
	
	for(int i = 0; i < PAGETABLE_ROWS; i++){
		uint32_t row_pid = pt[i] >> 12;
		uint32_t row_page_number = (pt[i] & 4095) >> 3;
		if(page_number == row_page_number && row_pid == pid){
			uint16_t write_status = pt[i] & 0b10;
			if(write_status != 0b10){
				return true;
			}else{
				return false;
			}
		}
	}
	return false;
}

void store_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length){
	char* memory = (char*) store;
	char* data = (char*) buffer;

	int i = 0;
	int page;
	int offset;
	int write_index = 0;
	for(i = 0; i < length; i++){
		offset = i % 128;
		page = i / 128;
		if(virtual_address_is_mapped(table, i)){
			if(virtual_address_is_writeable(table, i)){
				uint16_t physical_address = virtual_to_physical(table,i);
				memcpy(&memory[physical_address], &data[write_index],1);
				write_index+=1;
			}
		}
	}
}


void read_data(void* table, void* store, void* buffer, uint16_t virtual_address, size_t length){
	
	char* memory = (char*) store;
	char* data = (char*) buffer;

	int i = 0;
	int page;
	int offset;
	int write_index = 0;
	for(i = 0; i < length; i++){
		offset = i % 128;
		page = i / 128;

		if(virtual_address_is_mapped(table, i)){
			uint16_t physical_address = virtual_to_physical(table, i);
			memcpy(&data[write_index],&memory[physical_address],1);
			write_index+=1;
		}
	}
}