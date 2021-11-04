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

#define new_allocator(name, size)   static char pool_##name[size] = {0}; \
                                    static Allocator name = {pool_##name, (void*)(pool_##name + size), size, 0, NULL, NULL}

int canHold(Allocator* allocator, unsigned int size)
{
    return allocator->heap_size - allocator->mem_used >= size;
}
int contains(Allocator* allocator, void* ptr)
{
    return ptr > allocator->heap && ptr < allocator->heap_end;
}
void* memalloc(Allocator* allocator, unsigned int size)
{
    if(allocator->heap == NULL)
        return NULL;
    if(!canHold(allocator, size))
        return NULL;
    void* ptr = NULL;
    if(allocator->free_flags != NULL)
    {
        allocator_flag* flag = allocator->free_flags;
        if(flag->size >= size)
        {
            ptr = (void*)(allocator->heap + flag->offset + sizeof(flag));

            // Removing flag from free_flags chain
            allocator->free_flags = flag->next;

            // Inserting flag in used_flags chain
            flag->next = allocator->used_flags;
            allocator->used_flags = flag;
            return ptr;
        }
        allocator_flag* flag_move = allocator->free_flags;
        while(flag != NULL)
        {
            if(flag->next != NULL)
            {
                if(flag->next->size >= size)
                {
                    flag_move = flag->next;
                    ptr = (void*)(allocator->heap + flag_move->offset + sizeof(flag_move));

                    // Removing flag from free_flags chain
                    flag->next = flag_move->next;

                    // Inserting flag in used_flags chain
                    flag_move->next = allocator->used_flags;
                    allocator->used_flags = flag_move;
                    return ptr;
                }
            }
            flag = flag->next;
        }
    }
    allocator_flag* flag = (allocator_flag*)(allocator->heap + allocator->mem_used);
    flag->size = size;
    flag->offset = allocator->mem_used + sizeof(allocator_flag);
    flag->next = allocator->used_flags;
    allocator->used_flags = flag;
    ptr = (void*)(allocator->heap + allocator->mem_used + sizeof(allocator_flag));
    allocator->mem_used += sizeof(allocator_flag) + size;

    return ptr;
}

void memfree(Allocator* allocator, void* ptr)
{
    if(!contains(allocator, ptr))
        return;
    
    if(allocator->used_flags == NULL)
        return;

    unsigned int offset = (char*)ptr - allocator->heap;
    allocator_flag* flag = allocator->used_flags;
    if(flag->offset == offset)
    {
        // Removing flag from used_flags chain
        allocator->used_flags = flag->next;

        // Inserting flag in free_flags chain
        flag->next = allocator->free_flags;
        allocator->free_flags = flag;
        ptr = NULL;
        return;
    }
    allocator_flag* flag_move = allocator->used_flags;
    while(flag != NULL)
    {
        if(flag->next != NULL && flag->next->offset == offset)
        {
            flag_move = flag->next;
            // Removing flag from used_flags chain
            flag->next = flag_move->next;

            // Inserting flag in free_flags chain
            flag_move->next = allocator->free_flags;
            allocator->free_flags = flag_move;
            ptr = NULL;
            return;
        }
        flag = flag->next;
    }

    ptr = NULL;
}

#endif // __ALLOCATOR__
