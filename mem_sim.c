/***************************************************************************
 * *    Inf2C-CS Coursework 2: TLB and Cache Simulation
 * *    
 * *    Instructor: Boris Grot
 * *
 * *    TA: Priyank Faldu
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
/* Do not add any more header files */

/*
 * Various structures
 */
typedef enum {tlb_only, cache_only, tlb_cache} hierarchy_t;
typedef enum {instruction, data} access_t;
const char* get_hierarchy_type(uint32_t t) {
    switch(t) {
        case tlb_only: return "tlb_only";
        case cache_only: return "cache-only";
        case tlb_cache: return "tlb+cache";
        default: assert(0); return "";
    };
    return "";
}

typedef struct {
    uint32_t address;
    access_t accesstype;
} mem_access_t;

// These are statistics for the cache and TLB and should be maintained by you.
typedef struct {
    uint32_t tlb_data_hits;
    uint32_t tlb_data_misses;
    uint32_t tlb_instruction_hits;
    uint32_t tlb_instruction_misses;
    uint32_t cache_data_hits;
    uint32_t cache_data_misses;
    uint32_t cache_instruction_hits;
    uint32_t cache_instruction_misses;
} result_t;


/*
 * Parameters for TLB and cache that will be populated by the provided code skeleton.
 */
hierarchy_t hierarchy_type = tlb_cache;
uint32_t number_of_tlb_entries = 0; 
uint32_t page_size = 0;
uint32_t number_of_cache_blocks = 0; 
uint32_t cache_block_size = 0;
uint32_t num_page_table_accesses = 0;


/*
 * Each of the variables (subject to hierarchy_type) below must be populated by you.
 */
uint32_t g_total_num_virtual_pages = 0;
uint32_t g_num_tlb_tag_bits = 0;
uint32_t g_tlb_offset_bits = 0;
uint32_t g_num_cache_tag_bits = 0;
uint32_t g_cache_offset_bits= 0;
result_t g_result;


/* Reads a memory access from the trace file and returns
 * 1) access type (instruction or data access)
 * 2) 32-bit virtual memory address
 */
mem_access_t read_transaction(FILE *ptr_file) {
    char buf[1002];
    char* token = NULL;
    char* string = buf;
    mem_access_t access;

    if (fgets(buf, 1000, ptr_file)!=NULL) {

        /* Get the access type */
        token = strsep(&string, " \n");        
        if (strcmp(token,"I") == 0) {
            access.accesstype = instruction;
        } else if (strcmp(token,"D") == 0) {
            access.accesstype = data;
        } else {
            printf("Unkown access type\n");
            exit(-1);
        }

        /* Get the address */        
        token = strsep(&string, " \n");
        access.address = (uint32_t)strtol(token, NULL, 16);

        return access;
    }

    /* If there are no more entries in the file return an address 0 */
    access.address = 0;
    return access;
}

/* 
 * Call this function to get the physical page number for a given virtual number.
 * Note that this function takes virtual page number as an argument and not the whole virtual address.
 * Also note that this is just a dummy function for mimicing translation. Real systems maintains multi-level page tables.
 */
uint32_t dummy_translate_virtual_page_num(uint32_t virtual_page_num) {
    uint32_t physical_page_num = virtual_page_num ^ 0xFFFFFFFF;
    num_page_table_accesses++;
    if ( page_size == 256 ) {
        physical_page_num = physical_page_num & 0x00FFF0FF;
    } else {
        assert(page_size == 4096);
        physical_page_num = physical_page_num & 0x000FFF0F;
    }
    return physical_page_num;
}

void print_statistics(uint32_t num_virtual_pages, uint32_t num_tlb_tag_bits, uint32_t tlb_offset_bits, uint32_t num_cache_tag_bits, uint32_t cache_offset_bits, result_t* r) {
    /* Do Not Modify This Function */

    printf("NumPageTableAccesses:%u\n", num_page_table_accesses);
    printf("TotalVirtualPages:%u\n", num_virtual_pages);
    if ( hierarchy_type != cache_only ) {
        printf("TLBTagBits:%u\n", num_tlb_tag_bits);
        printf("TLBOffsetBits:%u\n", tlb_offset_bits);
        uint32_t tlb_total_hits = r->tlb_data_hits + r->tlb_instruction_hits; 
        uint32_t tlb_total_misses = r->tlb_data_misses + r->tlb_instruction_misses; 
        printf("TLB:Accesses:%u\n", tlb_total_hits + tlb_total_misses);
        printf("TLB:data-hits:%u, data-misses:%u, inst-hits:%u, inst-misses:%u\n", r->tlb_data_hits, r->tlb_data_misses, r->tlb_instruction_hits, r->tlb_instruction_misses);
        printf("TLB:total-hit-rate:%2.2f%%\n", tlb_total_hits / (float)(tlb_total_hits + tlb_total_misses) * 100.0); 
    }

    if ( hierarchy_type != tlb_only ) {
        printf("CacheTagBits:%u\n", num_cache_tag_bits); 
        printf("CacheOffsetBits:%u\n", cache_offset_bits); 
        uint32_t cache_total_hits = r->cache_data_hits + r->cache_instruction_hits; 
        uint32_t cache_total_misses = r->cache_data_misses + r->cache_instruction_misses; 
        printf("Cache:data-hits:%u, data-misses:%u, inst-hits:%u, inst-misses:%u\n", r->cache_data_hits, r->cache_data_misses, r->cache_instruction_hits, r->cache_instruction_misses);
        printf("Cache:total-hit-rate:%2.2f%%\n", cache_total_hits / (float)(cache_total_hits + cache_total_misses) * 100.0); 
    }
}

/*
 *
 * Add any global variables and/or functions here as you wish.
 *
 */

//get cache tag from physical memory
uint32_t getTag(uint32_t address){
    return address >> (32 - g_num_cache_tag_bits);
}

void callcache(uint32_t* cache, mem_access_t access){
    uint32_t physicalAddress = dummy_translate_virtual_page_num(access.address);
    uint32_t currentTag = getTag(physicalAddress);
    uint32_t index = (physicalAddress << g_num_cache_tag_bits) >> (g_num_cache_tag_bits + g_cache_offset_bits) ;
    
    if(*(cache+index)==0){        //if the required block is empty, inset currrnt tag
        *(cache+index)=currentTag ;
    }else{
        //if required block is not empty, find the tag in block
        uint32_t block_tag = *(cache+index) ;
        //if currrenr tag equals block tag, update hit correctly
        if(block_tag==currentTag){
            if(access.accesstype==0){
                g_result.cache_instruction_hits++;
            }else{
                g_result.cache_data_hits++;
            }
        }
        //if currrenr tag is not equal to block tag, update miss correctly,and replace new 
        if(block_tag!=currentTag){
            if(access.accesstype==0){
                *(cache+index)=currentTag ;
                g_result.cache_instruction_misses++;
            }else{
                *(cache+index)=currentTag ;
                g_result.cache_data_misses++;
            }
        }
    }
}


void calltlb(int* LRU, uint32_t* TLB, mem_access_t access){
    uint32_t virtual_tag = access.address >>  g_tlb_offset_bits;
    int index = number_of_tlb_entries+1;  //remember the hit occur place, ready to save in LRU
    //search virtual in TLB, to see whether it gives a hit
    //if it's a hit, index won't be number_of_tlb_entries+1
    for(int i=0; i<= number_of_tlb_entries; i++){
        uint32_t taginTLB = *(TLB+i);
        if(taginTLB==virtual_tag){
            index=i;
            i=number_of_tlb_entries+9;
        }
    }
    
    //tlb search hit
    if(index!=number_of_tlb_entries+1){
        //update hit counter
        if(access.accesstype==0){
            g_result.tlb_instruction_hits++;
        }else{
            g_result.tlb_data_hits++;
        }
        //update LRU
        
        //if LRU is not full, put index at first empty place
        if(*(number_of_tlb_entries+LRU)==number_of_tlb_entries+1){
            for(int i=0; i<=number_of_tlb_entries; i++){
                if(*(i+LRU)==number_of_tlb_entries+1){
                    *(i+LRU)=index;
                    i=number_of_tlb_entries+3;
                }
            }
        }else{//if LRU is full
            //place is the index of 'index' mentioned above stored in LRU
            int place;
            for(int i=0; i<=number_of_tlb_entries; i++){
                if(*(LRU+i)==index){
                    place=i;
                    i=number_of_tlb_entries+8; //end loop
                }
            }
            //UPDATE VALUES FROM PLACE TO END
            for(int i=place; i<number_of_tlb_entries; i++){
                *(i+LRU)=*(i+LRU+1);
            }
            
            
            *(number_of_tlb_entries+LRU)=index;
        }
        
    }//hit tlb end
    
    //if index have not updated, then tlb is miss
    if(index==number_of_tlb_entries+1){
        dummy_translate_virtual_page_num(access.address);
        //update miss counter
        if(access.accesstype==0){
            g_result.tlb_instruction_misses++;
        }else{
            g_result.tlb_data_misses++;
        }
        
        //if LRU and tlb are not full
        if(*(number_of_tlb_entries+LRU)==number_of_tlb_entries+1){
            
            //UPDATE ENTRY IN THE FIRST EMPTY PLACE IN TLB
            
            for(int i=0; i <= number_of_tlb_entries; i++){
                if(*(TLB+i)==0){
                    index=i;
                    i=number_of_tlb_entries+9;
                }
            }
            *(index+TLB) = virtual_tag;
            
            //UPDATE LRU IN THE FIRST EMPTY PLACE IN LRU
            for(int i=0; i<=number_of_tlb_entries; i++){
                if(*(i+LRU)==number_of_tlb_entries+1){
                    *(i+LRU)=index;
                    i=number_of_tlb_entries+3;
                }
            }
        }//if LRU and ltb is full
        else{
            index=*LRU;
            //update TLB
            *(index+TLB) = virtual_tag;
            //update LRU
            //remove the first element in LRU, update LRU
            for(int i=0; i<number_of_tlb_entries; i++){
                *(i+LRU)=*(i+LRU+1);
            }
            *(number_of_tlb_entries+LRU)=index;
        }
    }
}

int main(int argc, char** argv) {

    /*
     * 
     * Read command-line parameters and initialize configuration variables.
     *
     */
    int improper_args = 0;
    char file[10000];
    if ( argc < 2 ) {
        improper_args = 1;
        printf("Usage: ./mem_sim [hierarchy_type: tlb-only cache-only tlb+cache] [number_of_tlb_entries: 8/16] [page_size: 256/4096] [number_of_cache_blocks: 256/2048] [cache_block_size: 32/64] mem_trace.txt\n");
    } else  {
        /* argv[0] is program name, parameters start with argv[1] */
        if ( strcmp(argv[1], "tlb-only") == 0 ) {
            if ( argc != 5 ) { 
                improper_args = 1;
                printf("Usage: ./mem_sim tlb-only [number_of_tlb_entries: 8/16] [page_size: 256/4096] mem_trace.txt\n");
            } else {
                hierarchy_type = tlb_only;
                number_of_tlb_entries = atoi(argv[2]); 
                page_size = atoi(argv[3]);
                strcpy(file, argv[4]);
            }
        } else if ( strcmp(argv[1], "cache-only") == 0 ) {
            if ( argc != 6 ) { 
                improper_args = 1;
                printf("Usage: ./mem_sim cache-only [page_size: 256/4096] [number_of_cache_blocks: 256/2048] [cache_block_size: 32/64] mem_trace.txt\n");
            } else {
                hierarchy_type = cache_only;
                page_size = atoi(argv[2]);
                number_of_cache_blocks = atoi(argv[3]);
                cache_block_size = atoi(argv[4]);
                strcpy(file, argv[5]);
            }
        } else if ( strcmp(argv[1], "tlb+cache") == 0 ) {
            if ( argc != 7 ) { 
                improper_args = 1;
                printf("Usage: ./mem_sim tlb+cache [number_of_tlb_entries: 8/16] [page_size: 256/4096] [number_of_cache_blocks: 256/2048] [cache_block_size: 32/64] mem_trace.txt\n");
            } else {
                hierarchy_type = tlb_cache;
                number_of_tlb_entries = atoi(argv[2]); 
                page_size = atoi(argv[3]);
                number_of_cache_blocks = atoi(argv[4]);
                cache_block_size = atoi(argv[5]);
                strcpy(file, argv[6]);
            }
        } else {
            printf("Unsupported hierarchy type: %s\n", argv[1]);
            improper_args = 1;
        }
    }
    if ( improper_args ) {
        exit(-1);
    }
    assert(page_size == 256 || page_size == 4096);
    if ( hierarchy_type != cache_only) {
        assert(number_of_tlb_entries == 8 || number_of_tlb_entries == 16);
    }
    if ( hierarchy_type != tlb_only) {
        assert(number_of_cache_blocks == 256 || number_of_cache_blocks == 2048);
        assert(cache_block_size == 32 || cache_block_size == 64);
    }

    printf("input:trace_file: %s\n", file);
    printf("input:hierarchy_type: %s\n", get_hierarchy_type(hierarchy_type));
    printf("input:number_of_tlb_entries: %u\n", number_of_tlb_entries);
    printf("input:page_size: %u\n", page_size);
    printf("input:number_of_cache_blocks: %u\n", number_of_cache_blocks);
    printf("input:cache_block_size: %u\n", cache_block_size);
    printf("\n");
    
    //populate some of the output values , we will use them during access
    g_total_num_virtual_pages = pow(2,(32 - log2(page_size)));
    g_num_tlb_tag_bits = 32 - log2(page_size);
    g_tlb_offset_bits = log2(page_size);
    g_num_cache_tag_bits = cache_block_size - log2(cache_block_size) - log2(number_of_cache_blocks);
    g_cache_offset_bits = log2(cache_block_size);
    
    /* Open the file mem_trace.txt to read memory accesses */
    FILE *ptr_file;
    ptr_file =fopen(file,"r");
    if (!ptr_file) {
        printf("Unable to open the trace file: %s\n", file);
        exit(-1);
    }

    /* result structure is initialized for you. */
    memset(&g_result, 0, sizeof(result_t));

    /* Do not delete any of the lines below.
     * Use the following snippet and add your code to finish the task. */
    
     // in this simulation we are not concern about valid and drity.
    //since instruction have not mention anything about in which condition we make the block or entry to be in valid or dirty.
    
    //construct cache
    //we are not going to use data and valid bit to produce anything
    // keep simplisity, only tag is in cache
    uint32_t* cache;
    cache = calloc(number_of_cache_blocks,cache_block_size);
    
    //construct TLB
    //tlb entries have tag and physical page number.
    //in this coursework, we won't use physical page number to produce anything.
    //for simplisity, we modal each TLB entries only have tag.
    
    uint32_t* TLB;
    TLB = calloc(number_of_tlb_entries, 4);
    
    //construct LRU, LRU will point to the least use TLB index. initial LRU to -1
    int* LRU;
    LRU = calloc (number_of_tlb_entries , sizeof(int));
    for(int i=0; i<=number_of_tlb_entries; i++){
        *(LRU+i)=number_of_tlb_entries+1;
    }
   
    /* You may want to setup your TLB and/or Cache structure here. */
    mem_access_t access;
    /* Loop until the whole trace file has been read. */
    while(1) {
        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop.
        if (access.address == 0){
            break;
        }
        /* Add your code here */
        /* Feed the address to your TLB and/or Cache simulator and collect statistics. */
        
        //cache-only mode
        if(strcmp(get_hierarchy_type(hierarchy_type), "cache-only")==0){
            
            callcache(cache, access);
           
        }
        
        if(strcmp(get_hierarchy_type(hierarchy_type), "tlb_only")==0){
            
            calltlb(LRU,TLB, access);
            
        }
        if(strcmp(get_hierarchy_type(hierarchy_type), "tlb+cache")==0){
            
            callcache(cache, access);
            calltlb(LRU,TLB, access);
            
        }
        
    }//file end

    
    /* Do not modify code below. */
    /* Make sure that all the parameters are appropriately populated. */
    print_statistics(g_total_num_virtual_pages, g_num_tlb_tag_bits, g_tlb_offset_bits, g_num_cache_tag_bits, g_cache_offset_bits, &g_result);

    /* Close the trace file. */
    fclose(ptr_file);
    free(cache);//DELECT
    free(TLB);//DELECT
    free(LRU);//DELECT
    return 0;
}
