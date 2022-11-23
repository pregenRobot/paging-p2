#ifndef I_PAGETABLE_H
#define I_PAGETABLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define I_OFFSET_BITS 	7
#define I_OFFSET_MASK     (1<<OFFSET_BITS)-1 // 0000000001111111
#define I_PAGETABLE_ROWS	1<<(16-OFFSET_BITS + 4)
#define I_PAGETABLE_SIZE	PAGETABLE_ROWS + 3 + 4
#define I_FRAME_BITS 	1<<OFFSET_BITS


#endif