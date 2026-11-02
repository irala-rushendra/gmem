#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <new>
#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "memory_region.hpp"

std::size_t gmem::getpagesize() const {
  return sysconf(_SC_PAGESIZE);
}

gmem::gmem() : map_addr(nullptr) {
  auto flag = gmem::heap_init();
  if(flag == heap_e::HEAP_FAILURE) {
    std::cerr << "Heap Allocation Failed: " << std::strerror(errno) << '\n';
    std::exit(EXIT_FAILURE);
  }
}

// TODO:
// 1. Construct object at said addr using placment new
heap_e gmem::heap_init() {
  map_addr = ::mmap(
        nullptr, getpagesize(),
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1, 0
      );

  if(map_addr == MAP_FAILED) {
    std::cerr << "mmap failed : " << std::strerror(errno) << '\n';
    return heap_e::HEAP_FAILURE;
  }
  
  // [     4096b        ] ----> ptr*
  // [metadata|  free   ] ---rein_cast--> chunk* 
  heapchunk_t *chunk = new (reinterpret_cast<heapchunk_t*>(map_addr)) heapchunk_t {};
  chunk->size = heap.avail = getpagesize() - sizeof(heapchunk_t);
  heap.start = chunk;
  return heap_e::HEAP_SUCCESS;
}

// TODO:
// 1. Fix alignment issues
void* gmem::heap_alloc(const std::size_t size) {
  if(size == 0)
    return nullptr;

  heapchunk_t *chunk = heap.start;

  while(chunk != nullptr) {
    if(!chunk->in_use && chunk->size >= size)
      break;

    chunk = chunk->next;
  }

  if(chunk == nullptr)
    return nullptr;

  if(chunk->size >= size + sizeof(heapchunk_t) + 8) {
    std::size_t remaining_size =
        chunk->size - (size + sizeof(heapchunk_t));

    void *address = reinterpret_cast<char*>(chunk + 1) + size;
    heapchunk_t *new_chunk = new (address) heapchunk_t {};

    new_chunk->next = chunk->next;
    new_chunk->size = remaining_size;
    new_chunk->in_use = false;

    chunk->next = new_chunk;
    chunk->size = size;
  }

  heap.avail -= chunk->size;
  chunk->in_use = true;

  return chunk + 1;
}
/*
void* gmem::heap_alloc(const std::size_t size) {
  
  // chunk
  // [ header |      free          ]
  //          [   | remaining_size ]

  heapchunk_t *chunk = heap.start;
  while(chunk != nullptr) {
    if(!chunk->in_use && chunk->size >= size) {
      break;
    }
    chunk = chunk->next;
  }

  if(chunk == nullptr)
    return nullptr;

  std::size_t remaining_size = chunk->size - (size + sizeof(heapchunk_t));
  

  if(remaining_size >= sizeof(heapchunk_t) + 8) {
    void *address = reinterpret_cast<char*>(chunk + 1) + size;
    heapchunk_t *new_chunk = new (address) heapchunk_t {};

    new_chunk->next = chunk->next;
    new_chunk->in_use = false;
    new_chunk->size = remaining_size;
  
    chunk->next = new_chunk;
    chunk->size = size;
    chunk->in_use = true;

    heap.avail -= size + sizeof(heapchunk_t);

    return reinterpret_cast<void*>(chunk + 1);
  }
}
*/ 
 
// TODO:
// 1. Implement coalscing (Reduce Fragmentation)
void gmem::heap_free(void *ptr) {

  if(ptr == nullptr)
    return;

  heapchunk_t *tmp = heap.start;
  //std::size_t prevsize {0};
  while(tmp != nullptr) {
    if(ptr == static_cast<void*>(tmp + 1) && tmp->in_use) {
      tmp->in_use = false;
      heap.avail += tmp->size;
      break;
    }
    tmp = tmp->next;
  }
}

gmem::~gmem() {
  if(map_addr != nullptr && ::munmap(map_addr, getpagesize()) == -1){
    std::cerr << "::munmap error : " << std::strerror(errno) << '\n';
  }
}












