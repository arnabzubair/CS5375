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

void execache(int NumOfBlocks, int nway, int blockSize)
{
    int numberOfSets = NumOfBlocks / nway;
    struct direct_mapped_cache
    {
        unsigned valid_field[NumOfBlocks]; /* Valid field */
        unsigned dirty_field[NumOfBlocks]; /* Dirty field; since we don't distinguish writes and \\
                                             reads in this project yet, this field doesn't really matter */
        uint64_t tag_field[NumOfBlocks];   /* Tag field */
        int hits;                                  /* Hit count */
        int misses;                                /* Miss count */
    };
    
    struct direct_mapped_cache d_cache;
    /* Initialization */
    for (int i = 0; i < NumOfBlocks; i++)
    {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    char mem_request[20];
    FILE *fp;
    fp = fopen(trace_file_name, "r");
    uint64_t address;

    while (fgets(mem_request, 20, fp) != NULL)
    {
        address = convert_address(mem_request);
        uint64_t block_addr = address >> (unsigned)log2(blockSize);
        int setNumber = block_addr % numberOfSets;
        uint64_t tag = block_addr >> (unsigned)log2(numberOfSets);
        int startIndex = ((int)setNumber) * nway;

        int endIndex = startIndex + nway - 1;

        int hit = 0;
        int empty_space = 0;
        int nwayTemp = nway;
        int loopIndex = startIndex;
        int i = 0;
        
        while (nwayTemp > 0)
        {
            i++;
            if (d_cache.valid_field[loopIndex] && d_cache.tag_field[loopIndex] == tag)
            { /* Cache hit */
                d_cache.hits += 1;
                hitMade = 1;
                break;
            }
            if (d_cache.valid_field[loopIndex] == 0)
            {
                empty_space = 1;
            }

            loopIndex += 1;
            nwayTemp--;
        }

        if (hitMade == 0)
        {
            d_cache.misses += 1;
            if (empty_space > 0)
            {
                nwayTemp = nway;
                loopIndex = startIndex;
                while (nwayTemp > 0)
                {
                    if (d_cache.valid_field[loopIndex] == 0)
                    {
                        d_cache.valid_field[loopIndex] = 1;
                        d_cache.tag_field[loopIndex] = tag;
                        break;
                    }

                    loopIndex += 1;
                    nwayTemp--;
                }
            }
            else
            {
                // pick a random index and replace

                int randomIndex = (rand() % (endIndex - startIndex + 1)) + startIndex;
                d_cache.valid_field[randomIndex] = 1;
                d_cache.tag_field[randomIndex] = tag;
            }
        }
    }
    printf("==================================\n");
    printf("Number of Cache Hits:    %d\n", d_cache.hits);
    printf("Number of Cache Misses:  %d\n", d_cache.misses);
    printf("Cache Hit Rate percentage: %0.2f%%\n", ((float)d_cache.hits / (float)(d_cache.hits + d_cache.misses))*100);
    printf("Cache Miss Rate percentage : %0.2f%%\n", ((float)d_cache.misses / (float)(d_cache.hits + d_cache.misses))*100);
    printf("\n");
    fclose(fp);
}

void startProcess(int cacheSize, int NumOfBlocks, int nway, int blockSize)
{
    execache(NumOfBlocks, nway, blockSize);
}

void execute_Part1()
{
    int continueSelect = 0;
    do
    {
         long cacheSize = 32 * 1024;
    int NumOfBlocks = 0;
    int a = 0;
    printf("Fixed 32KB Cache size, Click 1 for 16 bytes cache line size, 2 for 32 bytes cache line size, 3 for 128 bytes cache line size : \n");
    scanf("%d", &a);
    int selection = 0;
    printf("Select 1 for fully associative execution, 2 for 2 way execution, 4 for 4 way execution, 8 for 8 way execution: ");
    scanf("%d", &selection);
    switch (a)
    {
    case 1:
        printf("\n\n ----------- STARTING EXECUTION FOR 16 BYTES cache line size ---------- \n");
        NumOfBlocks = (int)cacheSize / 16;
        switch (selection)
        {
        case 8:
            startProcess(32, NumOfBlocks, 8, 16);
            break;
        case 4:
            startProcess(32, NumOfBlocks, 4, 16);
            break;
        case 2:
            startProcess(32, NumOfBlocks, 2, 16);
            break;
        case 1:
            startProcess(32, NumOfBlocks, NumOfBlocks, 16);
            break;
        default:
            break;
        }

        break;
    case 2:
        printf("\n\n-----STARTING EXECUTION FOR 32 BYTES cache line size ------ \n");
        NumOfBlocks = (int)cacheSize / 32;
        switch (selection)
        {
        case 8:
            startProcess(32, NumOfBlocks, 8, 32);
            break;
        case 4:
            startProcess(32, NumOfBlocks, 4, 32);
            break;
        case 2:
            startProcess(32, NumOfBlocks, 2, 32);
            break;
        case 1:
            startProcess(32, NumOfBlocks, NumOfBlocks, 32);
            break;
        default:
            break;
        }

        break;
    case 3:
        printf("\n\n------------------STARTING EXECUTION FOR 128 BYTES cache line size ---------------- \n");
        NumOfBlocks = (int)cacheSize / 128;
        switch (selection)
        {
        case 8:
            startProcess(32, NumOfBlocks, 8, 128);
            break;
        case 4:
            startProcess(32, NumOfBlocks, 4, 128);
            break;
        case 2:
            startProcess(32, NumOfBlocks, 2, 128);
            break;
        case 1:
            startProcess(32, NumOfBlocks, NumOfBlocks, 1628);
            break;
        default:
            break;
        }

        break;
    }
    printf("=============== EXECUTION ENDED ===============\n");



        printf("Select 1 to continue , any other to exit: ");
        scanf("%d",&continueSelect);
    } while (continueSelect==1);
    

   
}
void execute_Part2()
{

     int continueSelect = 0;
    do
    {
 long cacheSize = 16 * 1024;
    int NumOfBlocks = 0;
    int a = 0;
    printf("For execution over 64Bytes Cache line size ,Click 1 for 16KB Cachesize, 2 for 32KB Cachesize, 3 for 64KB Cachesize \n  \n");
    scanf("%d", &a);
    int selection;
    printf("Select 1 for fully associative execution, 2 for 2 way execution, 4 for 4 way execution, 8 for 8 way execution: ");
    scanf("%d", &selection);
     
        /* Through Switch case 1 is fully associative, case 2 is 2-way, case 4 is 4-way and case 1 is 8 way associtive cache*/
        switch (a)
        {
        case 1:
            NumOfBlocks = (int)cacheSize / 64;
            printf("\n\n######### STARTING EXECUTION OF  16kb cache size and 64 bytes cache line size/block size=======\n");
            switch (selection)
            {
            case 8:
                startProcess(16, NumOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(16, NumOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(16, NumOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(16, NumOfBlocks, NumOfBlocks, 64);
                break;

            default:
                break;
            }

            break;
        case 2:
            printf("\n\n=======STARTING EXECUTION OF  32kb cache size and 64 bytes cache line size/block size=======\n");
            cacheSize = 32 * 1024;
            NumOfBlocks = (int)cacheSize / 64;  /*calculation for number of blocks*/
            switch (selection)
            {
            case 8:
                startProcess(32, NumOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(32, NumOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(32, NumOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(32, NumOfBlocks, NumOfBlocks, 64);
                break;

            default:
                break;
            }
            break;
        case 3:
            printf("\n\n====== STARTING EXECUTION OF  64kb cache size and 64 bytes cache line size/block size=======\n");
            cacheSize = 64 * 1024;
            NumOfBlocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(64, NumOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(64, NumOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(64, NumOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(64, NumOfBlocks, NumOfBlocks, 64);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }


        printf("Select 1 to continue , any other to exit: ");
        scanf("%d",&continueSelect);
    } while (continueSelect==1);
 
}

/*main function where everything function is called here for execution*/
int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int switchCase = 0;
    printf("Select 1 to  execute requirement 1 and  2 to requirement 2 from part 2 requirements :");
    scanf("%d", &switchCase);

    switch (switchCase)
    {
    case 1:
        execute_Part1(); /*execution of requirement 1 from the part 2 requirements*/
        break;

    case 2:
        execute_Part2();  /*execution of requirement 1 from the part 2 requirements*/
        break;
    }
}