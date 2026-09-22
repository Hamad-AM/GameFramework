#pragma once


// static uint32_t allocated_memory = 0;

// void* operator new(size_t size)
// {
//     allocated_memory += size;
//     return malloc(size);
// }

// void operator delete(void* memory, size_t size)
// {
//     allocated_memory -= size;
//     free(memory);
// }

class statistics
{
public:

    static void add_memory(size_t memory) { statistics::allocated_memory += memory; }
    static void free_memory(size_t memory) { statistics::allocated_memory -= memory; }
    static size_t get_allocated_memory() { return allocated_memory; }
private:
    static size_t allocated_memory;
};