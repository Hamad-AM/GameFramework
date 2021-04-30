#ifndef MEMORY_ALLOCATOR_HH
#define MEMORY_ALLOCATOR_HH

#include "platform.h"

class MemAlloc
{

};

class FreeList
{

};

class MemNode
{

};

class MemArena
{
public:
    MemArena(u64 size);
    ~MemArena();

    u64
    size() { return size_;}

    u64
    max_size() { return max_size_; }

private:
    u64 max_size_;
    u64 size_;
    u8* head_;
};

#endif