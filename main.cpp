#include <cmath>
#include <cstdint>
#include <iostream>
#include <assert.h>
#include "cache.h"

using namespace std;


/*
Main memory model. 
The read/write are at the 32-bit granularity.
The address for read/write is assumed without the Byte Offset
*/
#define MAIN_MEM_SIZE 1000
int main_mem[MAIN_MEM_SIZE]; 
int read_main_mem_global(uint32_t addr){
    // Read data in words
    return main_mem[addr];
}
void write_main_mem_global(uint32_t addr, int data){
    // Write data in words
    main_mem[addr] = data;
}





int main()
{
    // Fill main memory
    for(int i=0; i<MAIN_MEM_SIZE; i++){
        main_mem[i] = i; // To make the number match the index
    }

    // Instatiate Cache with 8 lines and 16-bit Addresses
    cache<8, 16> my_cache;
    // Assign the Main Memory callback functions
    my_cache.assign_main_mem_callbacks(read_main_mem_global, write_main_mem_global);


    /* 
    Test 1: Write to cache. Then read from the same entry
    -   Complete Address = d'594 = b'10010_100_10
    -   Word Addr = d'128 = b'10010_100
    -   Data = d'7
    -   The data should be updated in the cache, but not the main memory.
    */
    my_cache.write_cache(594, 7);
    int data1 = my_cache.read_cache(594);
    assert(data1 == 7 && read_main_mem_global(594)!=7);


    /* 
    Test 2: Read from memory from an INVALID entry. 
    -   Complete Address = d'744 = b'10111_010_00
    -   Word Address = d'186 = b'10111_010
    -   The data should be brought from main memory, stored in the cache, then read.
    */
    data1 = my_cache.read_cache(744);
    assert(read_main_mem_global(744>>2)==data1);

    /* 
    Test 3: Overwrite a VALID entry --> Same Index, different Tags
    -   Evicted Complete Address  = d'594 = b'10010_100_10
    -   New Complete Address      = d'690 = b'10101_100_10
    -   Evicted Word Address  = d'128 = b'10010_100
    -   New Word Address      = d'172 = b'10101_100
    -   The Cache should write back the value stored at Index b'100, then evict it.
    -   The main memory should be updated with value in data1 (which is 7).
    -   Value in data2 should show the new data at address 690.
    */
    data1 = my_cache.read_cache(594);
    int data2 = my_cache.read_cache(690);
    assert(read_main_mem_global(594>>2) == data1);
    assert(read_main_mem_global(690>>2) == data2);
    
    return 0;
}
