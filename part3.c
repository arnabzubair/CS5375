/*Copied includes and convert address function from cachesim.c source code file*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

/*Initialize L1 and L2 Caches*/
struct L1_cache
{
    unsigned valid_field[1024];
    unsigned dirty_field[1024];
    uint64_t tag_field[1024];
    char data_field[1024][64];
    int hits;
    int misses;
};

struct L2_cache
{
    unsigned valid_field[16384];
    unsigned dirty_field[16384];
    uint64_t tag_field[16384];
    char data_field[16384][64];
    int hits;
    int misses;
};

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n')
    {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0')
        {
            binary = (binary * 16) + (memory_addr[i] - '0');
        }
        else
        {
            if (memory_addr[i] == 'a' || memory_addr[i] == 'A')
            {
                binary = (binary * 16) + 10;
            }
            if (memory_addr[i] == 'b' || memory_addr[i] == 'B')
            {
                binary = (binary * 16) + 11;
            }
            if (memory_addr[i] == 'c' || memory_addr[i] == 'C')
            {
                binary = (binary * 16) + 12;
            }
            if (memory_addr[i] == 'd' || memory_addr[i] == 'D')
            {
                binary = (binary * 16) + 13;
            }
            if (memory_addr[i] == 'e' || memory_addr[i] == 'E')
            {
                binary = (binary * 16) + 14;
            }
            if (memory_addr[i] == 'f' || memory_addr[i] == 'F')
            {
                binary = (binary * 16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}
/*Checks to see if data is present in L1 Cache*/
int data_in_L1(uint64_t address, int n_way, struct L1_cache * l1)
{
    uint64_t block_address = address >> (unsigned)log2(64);
    int set_num = block_address % 512;
    uint64_t tag = block_address >> (unsigned)log2(512);
    int start_ind = ((int)set_num) * n_way;
    int n_way_temp = n_way;
    int loop_ind = start_ind;
    while (n_way_temp > 0)
    {
        if (l1->valid_field[loop_ind] && l1->tag_field[loop_ind] == tag)
        {
            return 1;
        }
        loop_ind += 1;
        n_way_temp--;
    }
    return 0;
}
/*Check to see if data is present in L2 Cache*/
int data_in_L2(uint64_t address, int n_way, struct L2_cache *l2)
{
    uint64_t block_address = address >> (unsigned)log2(64);
    int set_num = block_address % 2048;
    uint64_t tag = block_address >> (unsigned)log2(2048);
    int start_ind = ((int)set_num) * n_way;
    int n_way_temp = n_way;
    int loop_ind = start_ind;
    while (n_way_temp > 0)
    {
        if (l2->valid_field[loop_ind] && l2->tag_field[loop_ind] == tag)
        {
            return 1;
        }
        loop_ind += 1;
        n_way_temp--;
    }
    return 0;
}
/*Insert data into the L1 Cache*/
void insert_L1(uint64_t address, int n_way, struct L1_cache *l1)
{
    uint64_t block_address = address >> (unsigned)log2(64);
    int set_num = block_address % 512;
    uint64_t tag = block_address >> (unsigned)log2(512);
    int start_ind = ((int)set_num) * n_way;
    int n_way_temp = n_way;
    int loop_ind = start_ind;
    int empty_space = 0;
    int end_ind = start_ind + n_way - 1;
    while (n_way_temp > 0)
    {
        if (l1->valid_field[loop_ind] == 0)
        {
            empty_space = 1;
        }
        loop_ind++;
        n_way_temp--;
    }
    if (empty_space > 0)
    {
        n_way_temp = n_way;
        loop_ind = start_ind;
        while (n_way_temp > 0)
        {
            if (l1->valid_field[loop_ind] == 0)
            {
                l1->valid_field[loop_ind] = 1;
                l1->tag_field[loop_ind] = tag;
                break;
            }
            loop_ind += 1;
            n_way_temp--;
        }
    }
    else
    /*Replace value at random index*/
    {
        int rand_ind = (rand() % (end_ind - start_ind + 1)) + start_ind;
        l1->valid_field[rand_ind] = 1;
        l1->tag_field[rand_ind] = tag;
    }
}
/*Insert data into L2 Cache*/
void insert_L2(uint64_t address, int n_way, struct L2_cache *l2)
{
    uint64_t block_address = address >> (unsigned)log2(64);
    int set_num = block_address % 2048;
    uint64_t tag = block_address >> (unsigned)log2(2048);
    int start_ind = ((int)set_num) * n_way;
    int n_way_temp = n_way;
    int loop_ind = start_ind;
    int empty_space = 0;
    int end_ind = start_ind + n_way - 1;
    while (n_way_temp > 0)
    {
        if (l2->valid_field[loop_ind] == 0)
        {
            empty_space = 1;
        }
        loop_ind++;
        n_way_temp--;
    }
    if (empty_space > 0)
    {
        n_way_temp = n_way;
        loop_ind = start_ind;
        while (n_way_temp > 0)
        {
            if (l2->valid_field[loop_ind] == 0)
            {
                l2->valid_field[loop_ind] = 1;
                l2->tag_field[loop_ind] = tag;
                break;
            }

            loop_ind += 1;
            n_way_temp--;
        }
    }
    else
    {
        /*Replace value at random index*/
        int rand_ind = (rand() % (end_ind - start_ind + 1)) + start_ind;
        l2->valid_field[rand_ind] = 1;
        l2->tag_field[rand_ind] = tag;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    trace_file_name = argv[2];
    char mem_request[20];
    uint64_t address;
    struct L1_cache l1;
    struct L2_cache l2;
    int L1_blocks = 1024;
    int L2_blocks = 16384;
    int L1_n_way = 2;
    int L2_n_way = 8;
    int L1_sets = 512;
    int L2_sets = 2048;
    for (int i = 0; i < L1_blocks; i++)
    {
        l1.valid_field[i] = 0;
        l1.dirty_field[i] = 0;
        l1.tag_field[i] = 0;
    }
    for (int i = 0; i < L2_blocks; i++)
    {
        l2.valid_field[i] = 0;
        l2.dirty_field[i] = 0;
        l2.tag_field[i] = 0;
    }

    l1.hits = 0;
    l1.misses = 0;
    l2.hits = 0;
    l2.misses = 0;

    fp = fopen(trace_file_name, "r");

    if (strncmp(argv[1], "direct", 6) == 0)
    {
        while (fgets(mem_request, 20, fp) != NULL)
        {
            address = convert_address(mem_request);
            int dataInL1 = data_in_L1(address, L1_n_way, &l1);
            if (dataInL1 == 1)
            {
                l1.hits++;
                l2.hits++;
            }
            else
            {
                l1.misses++;
                int dataInL2 = data_in_L2(address, L2_n_way, &l2);
                if (dataInL2)
                {
                    l2.hits += 1;
                }
                else
                {
                    l2.misses++;
                    insert_L2(address, L2_n_way, &l2);
                }
                insert_L1(address, L1_n_way, &l1);
            }
        }
        printf("L1 Cache Hits:    %d\n", l1.hits);
        printf("L1 Cache Misses:  %d\n", l1.misses);
        printf("L1 Cache Hit Rate: %0.1f%%\n", ((float)l1.hits / (float)(l1.hits + l1.misses)) * 100);
        printf("L1 Cache Miss Rate: %0.1f%%\n", ((float)l1.misses / (float)(l1.hits + l1.misses)) * 100);
        printf("\n");
        printf("L2 Cache Hits:    %d\n", l2.hits);
        printf("L2 Cache Misses:  %d\n", l2.misses);
        printf("L2 Cache Hit Rate: %0.9f%%\n", ((float)l2.hits / (float)(l2.hits + l2.misses)) * 100);
        printf("L2 Cache Miss Rate: %0.9f%%\n", ((float)l2.misses / (float)(l2.hits + l2.misses)) * 100);
        printf("\n");
    }
    fclose(fp);
    return 0;
}