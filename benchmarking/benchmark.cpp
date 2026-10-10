#include "inc/ome.h"
#include <chrono>
#include <random>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

//Writing out output to the console slows down the code so this needs to be sorted for more accurate benchmarking
class CoutSuppressor
{
public:
    CoutSuppressor()  : old_(std::cout.rdbuf(sink_.rdbuf())) {}
    ~CoutSuppressor() { std::cout.rdbuf(old_); }
private:
    std::ostringstream sink_;
    std::streambuf* old_;
};

//Generate a set of random numbers within a set range for insertion
static std::vector<Order> generateOrders(int count, int min_price, int max_price,
                                          int min_qty, int max_qty,
                                          OrderType side, unsigned seed)
{
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> price_dist(min_price, max_price);
    std::uniform_int_distribution<int> qty_dist(min_qty, max_qty);

    std::vector<Order> orders;
    orders.reserve(count);
    for(int i = 0; i < count; ++i)
    {
        orders.push_back(Order{ side, qty_dist(rng), price_dist(rng) }); // matches struct field order: orderType, quantity, price
    }
    return orders;
}

//Benchmark the timing
template<typename Func>
static long long timeBenchmarkCalc(Func&& f)
{
    auto start = std::chrono::steady_clock::now();
    f();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main()
{
    const int N            = 100000;
    const int MIN_QTY      = 1;
    const int MAX_QTY_BUY  = 100;
    const int MAX_QTY_SELL = 120;
    const unsigned SEED    = 42;

    // Benchmark 1: resting-order insertion
    // Sells with no buys present, so every one rests. This helps to isolate the
    // map-insert path from any matching processes.
    long long insert_micros;
    {
        OrderMatchingEngine engine;
        auto resting_sells = generateOrders(N, 200, 300, MIN_QTY, MAX_QTY_SELL, OrderType::Sell, SEED);

        CoutSuppressor suppress;
        insert_micros = timeBenchmarkCalc([&]()
        {
            for(auto o : resting_sells)
            {
                engine.orderMatchingLogic(&o);
            }
        });
    }

    // Benchmark 2: matching throughput
    //Pre-populate the book with N sells spread across a price range,
    //then fire N buys priced to guarantee crossing -- isolates the
    //match/pop/erase path, including partial fills across levels.
    size_t buy_levels_after = 0, sell_levels_after = 0;
    long long match_micros;
    {
        OrderMatchingEngine engine;
        auto resting_sells = generateOrders(N, 100, 200, MIN_QTY, MAX_QTY_SELL, OrderType::Sell, SEED);
        auto crossing_buys = generateOrders(N, 200, 200, MIN_QTY, MAX_QTY_BUY, OrderType::Buy, SEED + 1); //By setting the price range to only 200,
                                                                                                          //we avoid hitting both the fill and insertion paths of the matching logic,
                                                                                                          //instead focusing only on the fill, to get a cleaner idea of what the end result represents
        CoutSuppressor suppress;
        for(auto o : resting_sells)
        {
            engine.orderMatchingLogic(&o);
        }

        match_micros = timeBenchmarkCalc([&]()
        {
            for(auto o : crossing_buys)
            {
                engine.orderMatchingLogic(&o);
            }
        });
        buy_levels_after  = engine.buyLevels();
        sell_levels_after = engine.sellLevels();
    }

    std::vector<long long> latencies_ns;
    {
        // Benchmark 3: Latency per order
        //For better insight into the latencies per order, we will avoid getting an average, since this could hide any latency spikes
        //and instead focus on getting the median value, the p99 (99% of orders were as fast as this value)
        //and the p99.9 percentile (99.9% of orders were as fast as this value).
        //Figures are in nanoseconds and include the overhead of the timer calls.
        OrderMatchingEngine engine;
        auto resting_sells = generateOrders(N, 100, 200, MIN_QTY, MAX_QTY_SELL, OrderType::Sell, SEED);
        auto crossing_buys = generateOrders(N, 200, 200, MIN_QTY, MAX_QTY_BUY, OrderType::Buy, SEED + 1);

        CoutSuppressor suppress;
        for(auto o : resting_sells)
        {
            engine.orderMatchingLogic(&o);
        }

        latencies_ns.reserve(crossing_buys.size());
        for(auto o : crossing_buys)
        {
            auto start = std::chrono::steady_clock::now();
            engine.orderMatchingLogic(&o);
            auto end = std::chrono::steady_clock::now();

            //We want to look at how much time it takes to process a single order
            //Units for this are in nanoseconds. Each sample includes the cost of two steady_clock::now()
            //calls (roughly 20-30 ns on typical hardware)
            latencies_ns.push_back(
                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }
    }


    // Sort so that a percentile is simply the value at that position.
    std::sort(latencies_ns.begin(), latencies_ns.end());
    auto pct = [&](double p) {
        return latencies_ns[static_cast<size_t>(p * (latencies_ns.size() - 1))];
    };

    std::cout << "p50: " << pct(0.50) << " ns\n";
    std::cout << "p99: " << pct(0.99) << " ns\n";
    std::cout << "p99.9: " << pct(0.999) << " ns\n";
    std::cout << "max: " << latencies_ns.back() << " ns\n";

    std::cout << "N = " << N << "\n";
    std::cout << "Resting-order insert: " << insert_micros << " us total, "
              << (double)insert_micros / N << " us/order\n";
    std::cout << "Matching throughput:  " << match_micros << " us total, "
              << (double)match_micros / N << " us/order\n";

    // buy_levels_after should be 0; non-zero means some buys rested instead of filling
    std::cout << "buy levels after:  " << buy_levels_after  << "\n";
    std::cout << "sell levels after: " << sell_levels_after << "\n";

    return 0;
}