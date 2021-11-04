# PSMA
Portable Static Memory Allocator is a memory allocator in C. It is fully portable and requires no dependencies. Its portability comes from the way it allocates. It allocates memory on static memory, not the heap.

It is used in a special way (for C)
```C
#include "psma.h"

int main(int argc, char** argv)
{
	new_allocator(myAllocator, 255/*bytes for the allocator*/);

    int* myInt = memalloc(&myAllocator, sizeof(int));

    memfree(myInt); // You don't even need to free it, the system will do it for you

	return 0;
}
```
