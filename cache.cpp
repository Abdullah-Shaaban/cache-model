/**************************************************************
    There is a problem with defining member functions of 
    template classes in a seperate file. The compiler gives
    Linker Errors. 
    For more detail, see: https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
**************************************************************/

// #include <stdlib.h>
// #include <cstdint>
// #include <cmath>
// #include "cache.h"


// // Constructor: all entries are INVALID and not DIRTY
// template<uint32_t num_of_lines>
// cache<num_of_lines>::cache(){
//     for (int i = 0; i < num_of_lines; i++){
//             cache_lines[i].valid = false;
//             cache_lines[i].dirty = false;
//     }
// }

// /*
// // Read function. Returns 32bit data. 
// // First, we extract the TAG and INDEX from addr.
// // We have a hit if the VALID bit is '1' AND the TAGs match
// // If not, then we have a miss, and we "issue a request to 
// // the main memory via the bus". This is modeled by a blocking call to read_main_mem 
// // (which may be supplied to the cache class as a function pointer (need a member for that).)
// // After calling read_main_mem, the returned data is stored in the proper INDEX, 
// // the VALID bit is set to 1, and data is finally returned to the caller of read_cache.
// */
// template<uint32_t num_of_lines>
// int cache<num_of_lines>::read_cache(uint32_t addr){
//     // First, we extract the TAG and INDEX from addr.
//     uint32_t indx = extract_indx(addr);
//     uint32_t tag = extract_tag(addr);     
//     // Get the entry at the extacted INDEX
//     cache_entry entry = cache_lines[indx];
//     // Check if TAGs match and if VALID is set
//     if (entry.tag==tag && entry.valid==true){
//         // We have a hit in cache, return the data
//         return entry.data;
//     }
//     else{
//         // We missed, we should issue request to main memory
//         // But first, we check if the cache line is DIRTY and call write_main_mem if necessary.
//         if (entry.dirty==true){
//             // Construct the address of the evicted entry: TAG[15:5], INDEX[4:2], Byte Offset
//             uint32_t entry_addr = entry.tag | (indx << 2) | (addr & 0x00000003);
//             // Store back to main memory
//             write_main_mem(entry_addr, entry.data);
//         }
//         // Read data from main memory, overriding the current cache entry
//         entry.data = read_main_mem(addr);
//         // Update the VALID bit, the DIRTY bit, and TAG of the entry
//         entry.tag = tag;
//         entry.valid = true;
//         entry.dirty = false;    // Cache and main memory have the same copy now
//         // Store entry it in cache_lines
//         cache_lines[indx] = entry;
//         // Finally, return the data
//         return entry.data;
//     }

// }


// // Write cache
// template<uint32_t num_of_lines>
// void cache<num_of_lines>::write_cache(uint32_t addr, int data){
//     // First, we extract the TAG and INDEX from addr.
//     uint32_t indx = extract_indx(addr);
//     uint32_t tag = extract_tag(addr);
//     // Get the entry at the extacted indx
//     cache_entry entry = cache_lines[indx];
//     // Check VALID and if TAGs match
//     if (entry.tag==tag && entry.valid==true){
//         // We have a hit in cache, write the data
//         entry.data = data;
//         // Make as DIRTY
//         entry.dirty = true;
//         // Store entry back to cache_lines 
//         cache_lines[indx] = entry;
//     }
//     else{
//         // We missed, we allocate this cache line and evict whatever is stored at that index
//         // But first, check if it DIRTY and call write_main_mem if necessary.
//         if (entry.dirty==true){
//             // Construct the address of the evicted entry
//             uint32_t entry_addr = reconstruct_addr(entry.tag, indx, addr);
//             // Store back to main memory
//             write_main_mem(entry_addr, entry.data);
//         }
//         // Update the VALID bit, the DIRTY bit and the TAG of the entry
//         entry.tag = tag;
//         entry.valid = true;
//         entry.dirty = true;
//         // write the data in cache entry
//         entry.data = data;
//         // Store entry back to cache_lines 
//         cache_lines[indx] = entry;
//         // No need to update main memory (write back policy)
//     }
// }

// // Function that extracts the Index from a 32-bit address. 
// template<uint32_t num_of_lines>
// uint32_t cache<num_of_lines>::extract_indx(uint32_t addr){
//     // Create Mask. Adding 2 because of the byte offset
//     int mask = -1 >> (32-(indx_bits+2));
//     // Mask addr to get the index bits, then shift right to discard byte offset
//     return (addr & mask) >> 2;
// }


// // Function that extracts the Tag from a 32-bit address. 
// template<uint32_t num_of_lines>
// uint32_t cache<num_of_lines>::extract_tag(uint32_t addr){
//     // Create Mask. Adding 2 because of the byte offset
//     int mask = -1 >> (32-(tag_bits+indx_bits+2));
//     // MASK addr to get tag bits, then shift right to discard index and byte offset
//     return ((addr & mask) >> (indx_bits+2));
// }

// // Function that constructs a 32-bit address from the Tag and Index
// template<uint32_t num_of_lines>
// uint32_t cache<num_of_lines>::reconstruct_addr(uint32_t tag, uint32_t indx, uint32_t addr){
//     // Address: {TAG, INDEX, Byte-Offset}
//     return (tag << (tag_bits+indx_bits+2)) | (indx << 2) | (addr & 0x00000003);
// }