#include <glibc-fs/prelude.h>
#include <malloc.h>

FAKEBUILTIN
__malloc_ptr_t malloc (size_t size) {}

FAKEBUILTIN
__malloc_ptr_t calloc (size_t nmemb, size_t size) {}

__malloc_ptr_t realloc (__malloc_ptr_t ptr, size_t size) { return ptr; }

void free (__malloc_ptr_t ptr) {}

void cfree (__malloc_ptr_t ptr) {}

__malloc_ptr_t memalign (size_t alignment, size_t size) {}

__malloc_ptr_t valloc (size_t size) {}

__malloc_ptr_t  pvalloc (size_t size) {}

__malloc_ptr_t (*__morecore) (ptrdiff_t size);

__malloc_ptr_t __default_morecore (ptrdiff_t size) {}

// struct mallinfo {
//     int arena;
//     int ordblks;
//     int smblks;
//     int hblks;
//     int hblkhd;
//     int usmblks;
//     int fsmblks;
//     int uordblks;
//     int fordblks;
//     int keepcost;
// };

struct mallinfo mallinfo (void) {}

int mallopt (int param, int val) {}

int malloc_trim (size_t pad) {}

size_t malloc_usable_size (__malloc_ptr_t ptr) {}

void malloc_stats (void) {}

__malloc_ptr_t malloc_get_state (void) {}

int malloc_set_state (__malloc_ptr_t ptr) {}

void (*__malloc_initialize_hook) (void);

void (*__free_hook) (__malloc_ptr_t ptr, const __malloc_ptr_t);
__malloc_ptr_t (*__malloc_hook) (size_t size, const __malloc_ptr_t);
__malloc_ptr_t (*__realloc_hook) (__malloc_ptr_t ptr, size_t size, const __malloc_ptr_t);
__malloc_ptr_t (*__memalign_hook) (size_t alignment, size_t size, const __malloc_ptr_t);
void (*__after_morecore_hook) (void);

void __malloc_check_init (void) {}

void __glibcfs_malloc_taint()
{
    // stub that calls the function pointers with tainted data
    char $tainted * t;
    t = __malloc_hook(0, t);
    __free_hook(t, t);
    t = __realloc_hook(t, 0, t);
    __memalign_hook(0, 0, t);
}
