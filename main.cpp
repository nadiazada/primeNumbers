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

// helper function to check if a number is prime
bool isPrime(unsigned int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (unsigned int i = 3; i * i <= n; i += 2) {
        if (n % i == 0)
            return false;
    }
    return true;
}

// worker_stub is a temporary thread function for Task 1
//it just runs in each thread and prints a message to confirm threading works
//later this will handle prime range calculations and file writing
void worker_function(unsigned int id, Range r) {
    auto& out = threadResults[id];
    for (unsigned int n = r.start; n <= r.end; ++n) {
        if (isPrime(n)) {
            out.push_back(n);
        }
    }
    std::cout << "Thread " << id << " processed range "
              << r.start << "-" << r.end
              << " and found " << out.size() << " primes.\n";
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
        threads.emplace_back(worker_function, i, ranges[i]);
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

    // sorting the merged results
    std::sort(merged.begin(), merged.end());

    std::cout << "Prime numbers <= " << limit << ":\n";
    for (unsigned int prime : merged) {
        std::cout << prime << " ";
    }
// output total primes found
std::cout << "Total primes found: " << merged.size() << "\n";

 return 0;
}
