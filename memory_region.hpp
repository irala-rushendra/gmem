#include <cstddef>

enum class heap_e {
  HEAP_SUCCESS,
  HEAP_FAILURE
};

// [   |        ]
struct heapchunk_t {
  heapchunk_t *next {nullptr};
  std::size_t size {0};
  bool in_use {false};
};

struct heapinfo_t {
  heapchunk_t *start {nullptr};
  std::size_t avail {0};
};

class gmem {
  public:
    gmem();
    heap_e heap_init();
    std::size_t getpagesize() const;
    void *heap_alloc(const std::size_t size);
    void heap_free(void *ptr);
    ~gmem();
  
  private:
    heapinfo_t heap;
    void *map_addr;
};




