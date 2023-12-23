#ifndef CACHE_H
#define CACHE_H

// Definitions
#define BYTE_OFFSET 2
// Types for Main Memory callback functions
typedef int (*read_main_mem_callback_t) (uint32_t addr);
typedef void (*write_main_mem_callback_t) (uint32_t addr, int data);


// Cache entry structure
struct cache_entry
{
    // - TAG - 11 bits
    uint32_t tag;
    // - DIRTY flag – 1 bit. Indicates if the data has been changed and needs to be written back to memory.
    bool dirty;
    // - VALID flag – 1 bit. Indicates if the cache row is valid, or if it is unused/invalid
    bool valid;
    // - DATABLOCK - 32-bit word containing the data in the cache row
    int32_t data;
};

/* 
Templated with:
    Number of lines in the cache
    Number of bits in the address
*/
template<uint32_t num_cache_lines, uint32_t num_addr_bits>
class cache {
private:
    
    // Number of bits in the index
    const uint32_t num_indx_bits = log2(num_cache_lines);

    // Number of bits in the Tag
    const uint32_t num_tag_bits = num_addr_bits - num_indx_bits - BYTE_OFFSET;

    // The cache should hold "num_of_lines" entries.
    // The "Cache Index" is just the array index
    cache_entry cache_lines [num_cache_lines];

    // Function pointers to read/write functions of main memory
    read_main_mem_callback_t read_main_mem;
    write_main_mem_callback_t write_main_mem;

public:
    
    // Constructor: all entries are INVALID and not DIRTY
    cache(){
        for (int i = 0; i < num_cache_lines; i++){
                cache_lines[i].valid = false;
                cache_lines[i].dirty = false;
        }
    }

    // Read function. Returns 32bit data. 
    int read_cache(uint32_t addr){
        // First, we extract the TAG and INDEX from addr.
        uint32_t indx = extract_indx(addr);
        uint32_t tag = extract_tag(addr);     
        // Get the entry at the extacted INDEX
        // cache_entry entry = cache_lines[indx];
        // Check if TAGs match and if VALID is set
        if (cache_lines[indx].tag==tag && cache_lines[indx].valid==true){
            // We have a hit in cache, return the data
            return cache_lines[indx].data;
        }
        else{
            // We missed, we should issue request to main memory
            // But first, we check if the cache line is DIRTY and call write_main_mem if necessary.
            if (cache_lines[indx].dirty==true){
                // Construct the address of the evicted entry: {TAG, INDEX} -- NO Byte Offset
                uint32_t entry_addr = reconstruct_addr(cache_lines[indx].tag, indx);
                // Store back to main memory
                write_main_mem(entry_addr, cache_lines[indx].data);
            }
            // Read data from main memory, overriding the current cache entry
            // Discard the Byte-Offset before reading from main memory.
            cache_lines[indx].data = read_main_mem(addr>>BYTE_OFFSET);
            // Update the VALID bit, the DIRTY bit, and TAG of the entry
            cache_lines[indx].tag = tag;
            cache_lines[indx].valid = true;
            // Cache and main memory have the same copy now --> dirty = false
            cache_lines[indx].dirty = false;
            // Finally, return the data
            return cache_lines[indx].data;
        }
    }
    
    // Write cache
    void write_cache(uint32_t addr, int data){
        // First, we extract the TAG and INDEX from addr.
        uint32_t indx = extract_indx(addr);
        uint32_t tag = extract_tag(addr);
        // Check VALID and if TAGs match
        if (cache_lines[indx].tag==tag && cache_lines[indx].valid==true){
            // We have a hit in cache, write the data
            cache_lines[indx].data = data;
            // Value changed in Cache --> entry is "dirty"
            cache_lines[indx].dirty = true;
        }
        else{
            // We missed, we allocate this cache line and evict whatever is stored at that index
            // But first, check if it DIRTY and call write_main_mem if necessary.
            if (cache_lines[indx].dirty==true){
                // Construct the address of the evicted entry
                uint32_t entry_addr = reconstruct_addr(cache_lines[indx].tag, indx);
                // Store back to main memory
                write_main_mem(entry_addr, cache_lines[indx].data);
            }
            // Update the VALID bit, the DIRTY bit and the TAG of the entry
            cache_lines[indx].tag = tag;
            cache_lines[indx].valid = true;
            cache_lines[indx].dirty = true;
            // Write the data in cache entry
            cache_lines[indx].data = data;
            // No need to update main memory (write back policy)
        }
    }

    // Extract the Index from a 32-bit address
    uint32_t extract_indx(uint32_t addr){
        // Create Mask. Adding 2 because of the byte offset
        int mask = ~((int)0xFFFFFFFF << (num_indx_bits+BYTE_OFFSET));
        // Mask addr to get the index bits, then shift right to discard byte offset
        return (addr & mask) >> BYTE_OFFSET;
    }

    // Extract the Tag from a 32-bit address
    uint32_t extract_tag(uint32_t addr){
        // Create Mask. Adding 2 because of the byte offset
        int mask = ~((int)0xFFFFFFFF << (num_tag_bits+num_indx_bits+BYTE_OFFSET));
        // MASK addr to get tag bits, then shift right to discard index and byte offset
        return ((addr & mask) >> (num_indx_bits+BYTE_OFFSET));
    }

    // Construct a 30-bit address (No Byte-Offset) from the Index and the Tag
    uint32_t reconstruct_addr(uint32_t tag, uint32_t indx){
        // The Index is the same, we need to overwrite the Tag
        // The Byte-Offset does not matter because all accesses are at word granularity
        return (tag << (num_indx_bits)) | indx;
    }

    // Assign the callback functions for reading/writing Main Memory
    void assign_main_mem_callbacks (read_main_mem_callback_t func1, write_main_mem_callback_t func2){
        read_main_mem = func1;
        write_main_mem = func2;
    }
};


#endif