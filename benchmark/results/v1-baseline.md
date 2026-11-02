# gmem — V1 Performance Baseline

## Environment
- Language: C++17
- Compiler: g++
- Optimization: `-O2`
- Benchmark: Custom heap allocator vs C++ global `operator new` / `operator delete`
- Iterations: 100,000
- Blocks per iteration: 24

## Results

| Metric | gmem | C++ operator new |
|---|---:|---:|
| Allocation time | 59.9141 ms | 26.5305 ms |
| Deallocation time | 45.5777 ms | 27.1608 ms |
| Total time | 105.492 ms | 53.6913 ms |
| Average allocation | 24.9642 ns | 11.0544 ns |
| Throughput | 40.0574M/s | 90.4619M/s |

## Analysis

- `gmem` was approximately 1.96x slower in total execution time.
- The linked-list traversal in allocation and deallocation contributes to overhead.
- The system allocator benefits from extensive production-level optimizations.

## Next Steps

- Implement alignment handling.
- Implement block coalescing.
- Support multiple memory regions.
- Experiment with segregated free lists.
- Repeat the benchmark after each major optimization.

