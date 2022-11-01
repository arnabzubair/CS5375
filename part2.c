/*Copied includes and convert address function from cachesim.c source code file*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

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

/*Function to read trace files and find cache hits and misses*/
void execute_cache(int num_blocks, int n_way, int block_size)
{
    /*Initialize variables for fields, hit count, and miss count */
    int num_sets = num_blocks / n_way;
    struct direct_mapped_cache
    {
        unsigned valid_field[num_blocks];
        unsigned dirty_field[num_blocks];
        uint64_t tag_field[num_blocks];
        int hits;
        int misses;
    };
    
    struct direct_mapped_cache d_cache;
    /*Initialize direct mapped cache */
    for (int i = 0; i < num_blocks; i++)
    {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    char memory_request[20];
    FILE *fp;
    fp = fopen(trace_file_name, "r");
    uint64_t address;

    while (fgets(memory_request, 20, fp) != NULL)
    {
        address = convert_address(memory_request);
        uint64_t block_address = address >> (unsigned)log2(block_size);
        int set_num = block_address % num_sets;
        uint64_t tag = block_address >> (unsigned)log2(num_sets);
        int start_ind = ((int)set_num) * n_way;
        int end_ind = start_ind + n_way - 1;
        int hit = 0;
        int empty_space = 0;
        int n_way_temp = n_way;
        int loop_ind = start_ind;
        int i = 0;
        
        while (n_way_temp > 0)
        {
            i++;
            if (d_cache.valid_field[loop_ind] && d_cache.tag_field[loop_ind] == tag)
            { /*Cache hit */
                d_cache.hits += 1;
                hit = 1;
                break;
            }
            if (d_cache.valid_field[loop_ind] == 0)
            {
                empty_space = 1;
            }

            loop_ind += 1;
            n_way_temp--;
        }
        /*Cache miss*/
        if (hit == 0)
        {
            d_cache.misses += 1;
            if (empty_space > 0)
            {
                n_way_temp = n_way;
                loop_ind = start_ind;
                while (n_way_temp > 0)
                {
                    if (d_cache.valid_field[loop_ind] == 0)
                    {
                        d_cache.valid_field[loop_ind] = 1;
                        d_cache.tag_field[loop_ind] = tag;
                        break;
                    }

                    loop_ind += 1;
                    n_way_temp--;
                }
            }
            else
            /*Randomly select an index to replace*/
            {
                int random_ind = (rand() % (end_ind - start_ind + 1)) + start_ind;
                d_cache.valid_field[random_ind] = 1;
                d_cache.tag_field[random_ind] = tag;
            }
        }
    }
    printf("==================================\n");
    printf("Cache Hits:    %d\n", d_cache.hits);
    printf("Cache Misses:  %d\n", d_cache.misses);
    printf("Hit Rate: %0.2f%%\n", ((float)d_cache.hits / (float)(d_cache.hits + d_cache.misses))*100);
    printf("Miss Rate: %0.2f%%\n", ((float)d_cache.misses / (float)(d_cache.hits + d_cache.misses))*100);
    printf("\n");
    fclose(fp);
}

void execute_cache_helper(int cache_size, int num_blocks, int n_way, int block_size)
{
    execute_cache(num_blocks, n_way, block_size);
}

void p1()
{
    int continue_choice = 0;
    do
    {
         long cache_size = 32 * 1024;
    int num_blocks = 0;
    int ca = 0;
    printf("32KB Cache Size: Select 1 for 16-byte cache line size, 2 for 32-byte cache line size, or 3 for 128-byte cache line size: \n");
    scanf("%d", &ca);
    int selection = 0;
    printf("Select 1 for fully associative execution, 2 for 2-way, 4 for 4-way, 8 for 8-way: ");
    scanf("%d", &selection);
    switch (ca)
    {
    case 1:
        printf("\n\n 16-byte cache line size:\n");
        num_blocks = (int)cache_size / 16;
        switch (selection)
        {
        case 8:
            execute_cache_helper(32, num_blocks, 8, 16);
            break;
        case 4:
            execute_cache_helper(32, num_blocks, 4, 16);
            break;
        case 2:
            execute_cache_helper(32, num_blocks, 2, 16);
            break;
        case 1:
            execute_cache_helper(32, num_blocks, num_blocks, 16);
            break;
        default:
            break;
        }
        break;
    case 2:
        printf("\n\n 32-byte cache line size:\n");
        num_blocks = (int)cache_size / 32;
        switch (selection)
        {
        case 8:
            execute_cache_helper(32, num_blocks, 8, 32);
            break;
        case 4:
            execute_cache_helper(32, num_blocks, 4, 32);
            break;
        case 2:
            execute_cache_helper(32, num_blocks, 2, 32);
            break;
        case 1:
            execute_cache_helper(32, num_blocks, num_blocks, 32);
            break;
        default:
            break;
        }

        break;
    case 3:
        printf("\n\n 128-byte cache line size:\n");
        num_blocks = (int)cache_size / 128;
        switch (selection)
        {
        case 8:
            execute_cache_helper(32, num_blocks, 8, 128);
            break;
        case 4:
            execute_cache_helper(32, num_blocks, 4, 128);
            break;
        case 2:
            execute_cache_helper(32, num_blocks, 2, 128);
            break;
        case 1:
            execute_cache_helper(32, num_blocks, num_blocks, 1628);
            break;
        default:
            break;
        }

        break;
    }
    printf("End of execution");
        printf("Select 1 to continue , any other to exit: ");
        scanf("%d",&continue_choice);
    } while (continue_choice==1);
   
}
void p2()
{

     int continue_choice = 0;
    do
    {
 long cache_size = 16 * 1024;
    int num_blocks = 0;
    int ca = 0;
    printf("64KB cache line size: Select 1 for 16KB cache, 2 for 32KB cache, or 3 for 64KB cache\n");
    scanf("%d", &ca);
    int selection;
    printf("Select 1 for fully associative execution, 2 for 2-way, 4 for 4-way, 8 for 8-way: ");
    scanf("%d", &selection);
        switch (ca)
        {
        case 1:
            num_blocks = (int)cache_size / 64;
            printf("\n 16KB cache size: \n");
            switch (selection)
            {
            case 8:
                execute_cache_helper(16, num_blocks, 8, 64);
                break;
            case 4:
                execute_cache_helper(16, num_blocks, 4, 64);
                break;
            case 2:
                execute_cache_helper(16, num_blocks, 2, 64);
                break;
            case 1:
                execute_cache_helper(16, num_blocks, num_blocks, 64);
                break;

            default:
                break;
            }

            break;
        case 2:
            printf("\n 32KB cache size: \n");
            cache_size = 32 * 1024;
            num_blocks = (int)cache_size / 64;  /*calculation for number of blocks*/
            switch (selection)
            {
            case 8:
                execute_cache_helper(32, num_blocks, 8, 64);
                break;
            case 4:
                execute_cache_helper(32, num_blocks, 4, 64);
                break;
            case 2:
                execute_cache_helper(32, num_blocks, 2, 64);
                break;
            case 1:
                execute_cache_helper(32, num_blocks, num_blocks, 64);
                break;

            default:
                break;
            }
            break;
        case 3:
            printf("\n 64KB cache size: \n");
            cache_size = 64 * 1024;
            num_blocks = (int)cache_size / 64;
            switch (selection)
            {
            case 8:
                execute_cache_helper(64, num_blocks, 8, 64);
                break;
            case 4:
                execute_cache_helper(64, num_blocks, 4, 64);
                break;
            case 2:
                execute_cache_helper(64, num_blocks, 2, 64);
                break;
            case 1:
                execute_cache_helper(64, num_blocks, num_blocks, 64);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        printf("Select 1 to continue , any other to exit: ");
        scanf("%d",&continue_choice);
    } while (continue_choice==1);

}

int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int switchCase = 0;
    printf("Select 1 for fixed cache size of 32KB or 2 for fixed cache line size of 64 bytes: \n");
    scanf("%d", &switchCase);

    switch (switchCase)
    {
    case 1:
        p1();
        break;

    case 2:
        p2();
        break;
    }
}