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

    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary = (binary*16) + (memory_addr[i] - '0');
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary = (binary*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary = (binary*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary = (binary*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary = (binary*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary = (binary*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary = (binary*16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}

void get_rates(int num_blocks, int n_way, int block_size)
{
    int sets = num_blocks / n_way;
    struct direct_mapped_cache
    {
    /*Initialize d_cache variables for valid, dirty, and tag fields, hits, and misses*/
        unsigned valid_field[num_blocks];
        unsigned dirty_field[num_blocks];
        uint64_t tag_field[num_blocks];
        int hits;
        int misses;
    };
    struct direct_mapped_cache d_cache;
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
        int set_num = block_address % sets;
        uint64_t tag = block_address >> (unsigned)log2(sets);
        int start_ind = ((int)set_num) * n_way;
        int end_ind = start_ind + n_way -1;
        int made_hit = 0;
        int empty_space = 0;
        int n_way_temp = n_way;
        int loop_ind = start_ind;
        int i = 0;
        
        while (n_way_temp > 0)
        {
            i++;
            if (d_cache.valid_field[loop_ind] && d_cache.tag_field[loop_ind] == tag)
            {
                /*Cache Hit*/
                d_cache.hits += 1;
                made_hit = 1;
                break;
            }
            if (d_cache.valid_field[loop_ind] == 0)
            {
                empty_space = 1;
            }
            loop_ind += 1;
            n_way_temp--;
        }
        if (made_hit == 0)
        {
            /* Hit was zero at loop index, insert at index*/
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
            {
                /*Replace at a random index*/
                nt random_ind = (rand() % (end_ind - start_ind + 1)) + start_ind;
                d_cache.valid_field[random_ind] = 1;
                d_cache.tag_field[random_ind] = tag;
            }
        }
    }

printf("==================================\n");
        printf("Cache Hits:    %d\n", d_cache.hits);
        printf("Cache Misses:  %d\n", d_cache.misses);
        printf("Cache Hit Rate: %0.9f %%\n", ((float)d_cache.hits / (float)(d_cache.hits + d_cache.misses))*100);
        printf("Cache Miss Rate: %0.9f %%\n", ((float)d_cache.misses / (float)(d_cache.hits + d_cache.misses))*100);
        printf("\n");
        fclose(fp);
}

void get_hits(int cache_size, int num_blocks, int n_way, int block_size)
{
    get_rates(num_blocks, n_way, block_size)
}

/*Given a fixed cache size of 32KB, test the fully-associative, 8-way set associative, 4-way set 
associative, and 2-way set associative cache with cache line size of 16 bytes, 32 bytes, and
128 bytes, respectively*/
void n_way_p1()
{
    int continue = 0;
    do
    {
    long cache_size = 32 * 1024;
    int num_blocks = 0;
    int ca = 0;
    printf("32KB Cache Size: Select 1 for 16 bytes cache size, 2 for 32 bytes cache size, or 3 for 128 bytes cache size: \n" );
    scanf("%d", &ca);
    int n_way_select = 0;
    printf("Select 1 for fully associative, 2 for 2-way association, 3 for 4-way, or 4 for 8-way: \n ");
    scanf("%d", &n_way_select);
    switch (ca)
    {
    case 1:
        printf("\n\n32KB Cache Size and 16 byte Block Size---------- \n");
        num_blocks = (int)cache_size / 16;
        switch (n_way_select)
        {
        case 1:
            printf("Fully Associative Execution\n");
            get_hits(32, num_blocks, num_blocks, 16);
            break;
        case 2:
            printf("2-way Execution");
            get_hits(32, num_blocks, 2, 16);
            break;
        case 3:
            printf("4-way Execution");
            get_hits(32, num_blocks 4, 16);
            break;
        case 4:
            printf("8-way Execution");
            get_hits(32, num_blocks 8, 16);
            break;
        default:
            break;
        }
        break;
    case 2:
        printf("\n\n32KB Cache Size and 32 byte Block Size---------- \n");
        num_blocks = (int)cache_size / 32;
        switch (n_way_select)
        {
        case 1:
            printf("Fully Associative Exectuion\n");
            get_hits(32, num_blocks, num_blocks, 32);
            break;
        case 2:
            printf("2-Way Exectuion\n");
            get_hits(32, num_blocks, 2, 32);
            break;
        case 3:
            printf("4-way Execution");
            get_hits(32, num_blocks 4, 32);
            break;
        case 4:
            printf("8-way Execution");
            get_hits(32, num_blocks 8, 32);
            break;
        default:
            break;
        }
        break;
    case 3:
        printf("\n\n64KB Cache Size and 128 byte Block Size---------- \n");
        num_blocks = (int)cache_size / 128;
        switch (n_way_select)
        {
        case 1:
            printf("Fully Associative Exectuion\n");
            get_hits(32, num_blocks, num_blocks, 128);
            break;
        case 2:
            printf("2-Way Exectuion\n");
            get_hits(32, num_blocks, 2, 128);
            break;
        case 3:
            printf("4-way Execution");
            get_hits(32, num_blocks 4, 128);
            break;
        case 4:
            printf("8-way Execution");
            get_hits(32, num_blocks 8, 128);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    printf("Select 1 to continue, any other key to exit: \n ");
    scanf("%d", &continue);
    } while (continue==1);
}

/*Given a fixed cache line size of 64 bytes, test the fully-associative, 8-way set associative, 4-
way set associative, and 2-way set associative cache with the cache size of 16KB, 32KB and
64KB, respectively*/
void n_way_p2()
{
    int to_continue = -1;
    do
    {
    long cache_size = 16 * 1024;
    int num_blocks = 0;
    int ca = 0;
    printf("64 Byte Cache Line Size, Select 1 for 16KB Cache Size, 2 for 32KB Cache Size, 3 for 64KB Cache Size:  \n");
    scanf("%d", &ca);
    int n_way_select;
    printf("Select 1 for fully associative, 2 for 2-way association, 3 for 4-way, or 4 for 8-way: \n ");
    scanf("%d", &n_way_select);
        switch (ca)
        {
        case 1:
            num_blocks = (int)cache_size / 64;
            printf("\n\n16KB Cache Size and 64 bytes Block Size=======\n");
            switch (n_way_select)
            {
            case 1:
                printf("Fully Associative Execution\n");
                get_hits(16, num_blocks, num_blocks, 64);
                break;
            case 2:
                printf("2-way Execution\n");
                get_hits(16, num_blocks, 2, 64);
                break;
            case 3:
                printf("4-way Execution\n");
                get_hits(16, num_blocks, 4, 64);
                break;
            case 4:
                printf("8-way Execution\n");
                get_hits(16, num_blocks, 8, 64);
                break;
            default:
                break;
            }
            break;
        case 2:
            printf("\n\n32KB Cache Size and 64 bytes Block Size=======\n");
            cache_size = 32 * 1024;
            num_blocks = (int)cache_size / 64;
            switch (n_way_select)
            {
            case 1:
                printf("Fully Associative Execution\n");
                get_hits(32, num_blocks, num_blocks, 64);
                break;
            case 2:
                printf("2-way Execution\n");
                get_hits(32, num_blocks, 2, 64);
                break;
            case 3:
                printf("4-way Execution\n");
                get_hits(32, num_blocks, 4, 64);
                break;
            case 4:
                printf("8-way Execution\n");
                get_hits(32, num_blocks, 8, 64);
                break;
            default:
                break;
            }
            break;
        case 3:
            printf("\n\n64KB cache size and 64 bytes Block Size=======\n");
            cache_size = 64 * 1024;
            num_blocks = (int)cache_wize / 64;
            switch (n_way_select)
            {
            case 1:
                printf("Fully Associatve Execution\n");
                get_hit(64, num_blocks, num_blocks, 64);
                break;
            case 2:
                printf("2-way Execution\n");
                get_hit(64, total_blocks, 2, 64);
                break;
            case 3:
                printf("4-way Execution\n");
                get_hit(64, num_blocks, 4, 64);
                break;
            case 4:
                printf("8-way Execution\n");
                get_hit(64, num_blocks, 8, 64);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        printf("Select 1 to continue, any other key to exit:\n ");
        scanf("%d",&continue);
    } while (continue==1);
}

int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int switch_case = 0;
    printf("Select 1 to execute Part 1 or 2 to Part 2:\n");
    scanf("%d", &switch_case);
    switch (switch_case)
    {
    case 1:
        n_way_p1();
        break;
    case 2:
        n_way_p2();
        break;
    }
}