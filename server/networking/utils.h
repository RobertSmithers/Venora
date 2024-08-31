#include <stdio.h>
#include <stdlib.h>

#include "networking.h"

DataBlock *uniform_list_to_data_blocks(void **list, size_t count, size_t item_sz);
void free_data_blocks(DataBlock *blocks, size_t block_ct);