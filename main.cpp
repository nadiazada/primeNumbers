#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib> //to convert raw char into number
#include <algorithm>

static std::vector<std::vector<unsigned int>> threadResults;

// defining a simple struct to hold the thread's number range
struct Range {
    unsigned int start;
    unsigned int end;
};

// worker_stub is a temporary thread function for Task 1
//it just runs in each thread and prints a message to confirm threading works
//later this will handle prime range calculations and file writing
void worker_function(unsigned int id, unsigned int limit, unsigned int numThreads) {
    std::cout << "Thread " << id << " Ready (limit=" << limit
              << ", total_threads=" << numThreads << ")\n";
    auto& out = threadResults[id];
    (void) out; //suppress unused variable warning!!!
}

int main(int argc, char* argv[]) {
 if (argc != 2) {
	std::cerr << "Usage: " << argv[0] << " <limit>\n";
        return 1;
    }

// minimal parse (no fluff)

 if (argv[1][0] == '-') { std::cout << "No primes for negative limit.\n"; return 0; }
 unsigned int limit = static_cast<unsigned int>(std::atoi(argv[1]));
 if (limit < 2) { std::cout << "No primes for limit < 2.\n"; return 0; }

 unsigned int numThreads = std::thread::hardware_concurrency();
 if (numThreads == 0) numThreads = 2;

 std::cout << "creating " << numThreads << " threads...\n";

 // dividing range [1...limit] among the threads
 std::vector<Range> ranges;
 ranges.reserve(numThreads);

 unsigned int base = limit / numThreads;
 unsigned int rem = limit % numThreads;

 unsigned int currentStart = 1;
 for (unsigned int i = 0; i < numThreads; ++i) {
    unsigned int len = base + ((i < rem) ? 1 : 0);
    unsigned int currentEnd = currentStart + len - 1;
    if (currentEnd > limit) currentEnd = limit;
    ranges.push_back({currentStart, currentEnd});
    currentStart = currentEnd + 1;
 }

 // printing the ranges to verify
 for (unsigned int i = 0; i < ranges.size(); ++i) {
    std::cout << "Thread " << i
              << " range: " << ranges[i].start
              << "-" << ranges[i].end << "\n";
 }
// preparing storage for thread results
    threadResults.clear();
    threadResults.resize(numThreads);

 std::vector<std::thread> threads;
 for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker_function, i, limit, numThreads);
    }
 
// wait for all threads to complete
    for (auto& t : threads) t.join();
// merging results
   std::vector<unsigned int> merged;
   size_t totalSize = 0;
    for (const auto& vec : threadResults) {
        totalSize += vec.size();
    }
    merged.reserve(totalSize);
    for (const auto& vec : threadResults) {
        merged.insert(merged.end(), vec.begin(), vec.end());
    }
// output total primes found
std::cout << "Total primes found: " << merged.size() << "\n";

 return 0;
}
