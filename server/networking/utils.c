#include <utils.h>

DataBlock *uniform_list_to_data_blocks(void **list, size_t count, size_t item_sz)
{
    DataBlock *blocks = (DataBlock *)malloc(sizeof(DataBlock) * count);
    if (blocks == NULL)
    {
        perror("uniform_list_to_data_blocks: failed to malloc blocks");
        return NULL;
    }

    // Convert to blocks
    for (size_t i = 0; i < count; i++)
    {
        blocks[i].data = list[i];
        blocks[i].size = item_sz;
    }
    return blocks;
}

void free_data_blocks(DataBlock *blocks, size_t block_ct)
{
    for (size_t i = 0; i < block_ct; i++)
    {
        free(blocks[i].data);
    }
    free(blocks);
}