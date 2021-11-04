#ifndef __ALLOCATOR__
#define __ALLOCATOR__

/**
 * This allocator is based on Akel's JamAllocator logic (https://github.com/SpinWaves/Akel).
 */

typedef struct allocator_flag
{
    unsigned int size;
    unsigned int offset;

    // Linked list of flags
    struct allocator_flag* next;
} allocator_flag;

typedef struct
{
    const char* heap;
    const char* heap_end;
    unsigned int heap_size;
    unsigned int mem_used;

    allocator_flag* used_flags;
    allocator_flag* free_flags;
} Allocator;

void* memalloc(Allocator* allocator, unsigned int size);
bool canHold(Allocator* allocator, unsigned int size);
bool contains(Allocator* allocator, void* ptr);
void memfree(Allocator* allocator, void* ptr);

#define new_allocator(name, size)   static char pool_##name[size] = {0}; \
                                    static Allocator name = {pool_##name, (void*)(pool_##name + size), size, 0, NULL, NULL}

#endif // __ALLOCATOR__
