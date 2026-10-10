OrderMatchingEngine

A limit-order matching engine in C++, built in stages to explore data-structure choices and measure their effect on latency.

Stages
Vector-based order matching engine (brute force)
std::map / std::queue engine (current)
Price-indexed array with ring buffers per price level (planned)
Baseline benchmark: std::map + std::queue

Hardware: Intel Xeon Gold 5218 @ 2.30 GHz, g++ (GCC) 11.5.0 20240719 (Red Hat 11.5.0-5), -std=c++23 -O2

Build:

g++ -std=c++23 -O2 -Wall -Wextra -I. src/ome.cpp benchmarking/benchmark.cpp -o benchmark

Workload: N = 100,000; sell prices 100-200, qty 1-120; buy price 200, qty 1-100; seed 42.
All buys cross, so the latency figures measure the fill path only.
Latencies include ~20-30 ns of timer overhead. Median of 5 runs.

Metric	Result
Latency p50	185 ns
Latency p99	401 ns
Latency p99.9	667 ns
Matching throughput	~23.7 ms / 100k orders (~4.2M orders/s)
Resting-order insert	~16.2 ms / 100k orders (~0.16 us/order)

Max latency was ~3.2-4.0 ms in every run (not investigated).
Run 4’s batch matching time (47 ms) was an outlier, probably from other load on the machine.