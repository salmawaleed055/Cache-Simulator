    //Cache Project

#include <iostream>
#include <iomanip>
#include <string>
#include <climits>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

#define DRAM_SIZE (64*1024*1024) // 64 GB

// Cache configuration constants
#define L1_SIZE (16*1024)        // 16 KB
#define L2_SIZE (128*1024)       // 128 KB
#define L2_LINE_SIZE 64          // 64B
#define L1_ASSOCIATIVITY 4       // 4-way set associative
#define L2_ASSOCIATIVITY 8       // 8-way set associative

// Timing constants
#define L1_HIT_TIME 1
#define L2_HIT_TIME 10
#define DRAM_ACCESS_TIME 50

int addr_memGen4 = 0;
int addr_memGen5 = 0;



enum cacheResType {MISS = 0, HIT = 1};

// Cache line structure
struct CacheLine {
    bool valid;
    unsigned int tag;
    bool dirty;  // For write-back

    CacheLine() : valid(false), tag(0), dirty(false) {}
};

// Cache structure
struct Cache {
    CacheLine** lines; // 2D array of cache lines
    int sets; // number of sets
    int ways; // associativity
    int line_size;
    int set_bits; // number of index bits
    int offset_bits;

    // Statistics
    unsigned long long hits;
    unsigned long long misses;
    unsigned long long write_backs;

    Cache() : lines(nullptr), hits(0), misses(0), write_backs(0) {}
};


// Global cache instances
Cache L1_cache, L2_cache;
int L1_line_size = 32;  // Variable line size for L1

/* Random number generator */
unsigned int m_w = 0xABABAB55;
unsigned int m_z = 0x05080902;


void resetRNG() {
    m_w = 0xABABAB55;
    m_z = 0x05080902;
}

unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

double getRandomDouble() {
    return (double)rand() / RAND_MAX; // random double between 0 and 1
}

int getRandomInt(int max) {
    return rand() % max; // random int from 0 to max - 1
}

// Memory generators
unsigned int sequential_addresses()
{

    static unsigned int addr = 0;
    return (addr++)%(DRAM_SIZE); // sequential addresses
}

unsigned int random_address_0_and_24KB()
{

    return rand()%(24*1024); // random address between 0 and 24KB (-1)
}

unsigned int random_general_access()
{
    return rand()%(DRAM_SIZE); // random address anywhere in DRAM size
}

unsigned int sequential_addresses_until_4KB()
{
    return (addr_memGen4++)%(4*1024); // sequential addresses until 4 KB
}

unsigned int sequential_addresses_of_32B_until_1MB()
{
    return (addr_memGen5+=32)%(64*16*1024); // sequential addresses in strides of 32B until 1MB
}

void resetMemGen() { 
    addr_memGen4 = 0;
    addr_memGen5 = 0;
}


void initCache(Cache* cache, int size, int line_size, int ways) {
    cache->line_size = line_size;
    cache->ways = ways;
    cache->sets = size / (line_size * ways);
    cache->hits = 0;
    cache->misses = 0;
    cache->write_backs = 0;

    // Calculate bit positions
    cache->offset_bits = 0;
    int temp = line_size;
    while (temp > 1) {
        cache->offset_bits++;
        temp >>= 1;
    }

    cache->set_bits = 0;
    temp = cache->sets;
    while (temp > 1) {
        cache->set_bits++;
        temp >>= 1;
    }

    // Allocate memory for cache lines
    cache->lines = new CacheLine*[cache->sets];
    for (int i = 0; i < cache->sets; i++) {
        cache->lines[i] = new CacheLine[ways];
    }
}

// deallocate cache memory
void cleanupCache(Cache* cache) {
    if (cache->lines) {
        for (int i = 0; i < cache->sets; i++) {
            delete[] cache->lines[i];
        }
        delete[] cache->lines;
        cache->lines = nullptr;
    }
}

// Extract tag from address
unsigned int getTag(unsigned int addr, Cache* cache) {
    return addr >> (cache->offset_bits + cache->set_bits);
}

// Extract set index from address
unsigned int getSet(unsigned int addr, Cache* cache) {
    unsigned int mask = (1 << cache->set_bits) - 1;
    return (addr >> cache->offset_bits) & mask;
}

// access cache and update hit/miss/writeback
cacheResType accessCache(unsigned int addr, Cache* cache, bool is_write, bool& needs_write_back, unsigned int& wb_addr) {
    unsigned int set = getSet(addr, cache);
    unsigned int tag = getTag(addr, cache);

    needs_write_back = false;

    // Check for hit in the set
    for (int i = 0; i < cache->ways; i++) {
        if (cache->lines[set][i].valid && cache->lines[set][i].tag == tag) {
            // Hit
            cache->hits++;
            if (is_write) {
                cache->lines[set][i].dirty = true;
            }
            return HIT;
        }
    }

    // Miss
    cache->misses++;

    // Find victim using random replacement
    int victim = getRandomInt(cache->ways);

    // Check if victim needs write-back
    if (cache->lines[set][victim].valid && cache->lines[set][victim].dirty) {
        needs_write_back = true;
        cache->write_backs++;
        // Construct write-back address
        wb_addr = (cache->lines[set][victim].tag << (cache->offset_bits + cache->set_bits)) |
                  (set << cache->offset_bits);
    }

    // Install new line
    cache->lines[set][victim].valid = true;
    cache->lines[set][victim].tag = tag;
    cache->lines[set][victim].dirty = is_write;

    return MISS;
}

// simulate a single memory access
int simulateMemoryAccess(unsigned int addr, bool is_write) {
    int cycles = 0;
    bool l1_needs_wb = false, l2_needs_wb = false;
    unsigned int l1_wb_addr = 0, l2_wb_addr = 0;

    // Access L1 cache
    cacheResType l1_result = accessCache(addr, &L1_cache, is_write, l1_needs_wb, l1_wb_addr);
    cycles += L1_HIT_TIME;

    if (l1_result == HIT) {
        // L1 hit - we're done, but handle any write-back
        if (l1_needs_wb) {
            // Write-back to L2
            bool dummy_wb;
            unsigned int dummy_addr;

            // Write dirty L1 eviction to L2; if L2 evicts a dirty block, add DRAM access
            accessCache(l1_wb_addr, &L2_cache, true, dummy_wb, dummy_addr);
            cycles += L2_HIT_TIME;
            if (dummy_wb) {
                cycles += DRAM_ACCESS_TIME; // Write-back from L2 to DRAM
            }
        }
        return cycles;
    }

    // L1 miss - handle L1 write-back first if needed
    if (l1_needs_wb) {
        bool l2_wb_from_l1;
        unsigned int l2_wb_addr_from_l1;
        accessCache(l1_wb_addr, &L2_cache, true, l2_wb_from_l1, l2_wb_addr_from_l1);
        cycles += L2_HIT_TIME;
        if (l2_wb_from_l1) {
            cycles += DRAM_ACCESS_TIME; // Write-back from L2 to DRAM
        }
    }

    // Now access L2 for the original request
    cacheResType l2_result = accessCache(addr, &L2_cache, false, l2_needs_wb, l2_wb_addr);
    cycles += L2_HIT_TIME;

    if (l2_result == MISS) {
        // L2 miss - access DRAM
        cycles += DRAM_ACCESS_TIME;
    }

    // Handle L2 write-back if needed
    if (l2_needs_wb) {
        cycles += DRAM_ACCESS_TIME;
    }

    return cycles;
}

struct SimResult {
    double cpi;
    double l1_hit_rate;
    double l2_hit_rate;
    double avg_mem_access_time;
};

// Simulates 1M instructions using given memory generator and L1 line size; tracks cache hit rates and CPI
SimResult runSimulation(unsigned int (*memGen)(), const string& genName, int lineSize) {
    // Reset for consistent results
    resetRNG();
    resetMemGen();

    L1_line_size = lineSize;

    // Initialize caches
    initCache(&L1_cache, L1_SIZE, L1_line_size, L1_ASSOCIATIVITY);
    initCache(&L2_cache, L2_SIZE, L2_LINE_SIZE, L2_ASSOCIATIVITY);

    unsigned long long total_cycles = 0;
    unsigned long long mem_accesses = 0;
    unsigned long long total_mem_cycles = 0;
    const int ITERATIONS = 1000000;

    cout << "  Running " << genName << " with L1 line size " << lineSize << "B...";

    for (int inst = 0; inst < ITERATIONS; inst++) {
        // 35% probability of memory access instruction
        double p = getRandomDouble();

        if (p <= 0.35) {
            // Memory access instruction
            unsigned int addr = memGen();
            mem_accesses++;

            // 50% of memory accesses are reads
            double rdwr = getRandomDouble();
            bool is_write = (rdwr >= 0.5);

            int mem_cycles = simulateMemoryAccess(addr, is_write);
            total_cycles += mem_cycles;
            total_mem_cycles += mem_cycles;
        } else {
            // Non-memory instruction
            total_cycles += 1;  // Base CPI = 1
        }
    }

    SimResult result;
    result.cpi = (double)total_cycles / ITERATIONS;
    result.l1_hit_rate = (double)L1_cache.hits / (L1_cache.hits + L1_cache.misses);
    result.l2_hit_rate = (double)L2_cache.hits / (L2_cache.hits + L2_cache.misses);
    result.avg_mem_access_time = (double)total_mem_cycles / mem_accesses;

    cout << " CPI = " << fixed << setprecision(3) << result.cpi << endl;

    // Cleanup
    cleanupCache(&L1_cache);
    cleanupCache(&L2_cache);

    return result;
}


class CacheSimulatorTester {
public:
    static void testBasicCacheOperations() {
        cout << "\n=== Testing Basic Cache Operations ===\n";

        resetRNG();
        initCache(&L1_cache, 1024, 32, 2);

        bool wb_needed;
        unsigned int wb_addr;
        cacheResType result = accessCache(0x0000, &L1_cache, false, wb_needed, wb_addr);
        if(result != MISS || L1_cache.misses != 1) {
            cout << "✗ Test 1 FAILED\n";
            return;
        }
        cout << "✓ Test 1 passed: First access is a miss\n";

        result = accessCache(0x0000, &L1_cache, false, wb_needed, wb_addr);
        if(result != HIT || L1_cache.hits != 1) {
            cout << "✗ Test 2 FAILED\n";
            return;
        }
        cout << "✓ Test 2 passed: Repeated access is a hit\n";

        cleanupCache(&L1_cache);
    }

    static void testMemoryGenerators() {
        cout << "\n=== Testing Memory Generators ===\n";

        bool all_in_range = true;
        for(int i = 0; i < 100; i++) {
            unsigned int addr = random_address_0_and_24KB();
            if(addr >= 24*1024) {
                all_in_range = false;
                break;
            }
        }
        cout << "memGen2 range test: " << (all_in_range ? "✓ PASS" : "✗ FAIL") << "\n";
    }
};

class PerformanceValidator {
public:
    static void validateResults(SimResult results[][4]) {
        cout << "\n=== Validating Performance Results ===\n";

        // Test: memGen4 should have good performance
        if(results[3][0].cpi < 1.1) {
            cout << "✓ memGen4 has good performance\n";
        } else {
            cout << "✗ memGen4 performance issue\n";
        }

        // Test: memGen3 should have poor performance
        if(results[2][0].cpi > 20) {
            cout << "✓ memGen3 has poor performance as expected\n";
        } else {
            cout << "✗ memGen3 performance better than expected\n";
        }

        // Test: Larger line sizes should help sequential patterns
        if(results[0][3].cpi < results[0][0].cpi) {
            cout << "✓ Larger line sizes help sequential access\n";
        } else {
            cout << "✗ Line size effect not working\n";
        }
    }
};

void outputCSVResults(SimResult results[][4], string genNames[], int lineSizes[]) {
    // Output CSV file for Python graphing
    ofstream csvFile("results/simulation_results1.csv");
    csvFile << "Memory_Generator,Line_Size,CPI,L1_Hit_Rate,L2_Hit_Rate,Avg_Memory_Access_Time\n";

    for (int genIdx = 0; genIdx < 5; genIdx++) {
        for (int ls = 0; ls < 4; ls++) {
            csvFile << genNames[genIdx] << ","
                   << lineSizes[ls] << ","
                   << fixed << setprecision(6) << results[genIdx][ls].cpi << ","
                   << results[genIdx][ls].l1_hit_rate << ","
                   << results[genIdx][ls].l2_hit_rate << ","
                   << results[genIdx][ls].avg_mem_access_time << "\n";
        }
    }
    csvFile.close();
    cout << "\nResults exported to simulation_results1.csv\n";
}

void generateASCIIGraph(SimResult results[][4], string genNames[], int lineSizes[]) {
    cout << "\n=== ASCII Performance Graph ===\n";
    cout << "CPI vs Line Size (each * represents ~0.5 CPI)\n\n";

    for(int gen = 0; gen < 5; gen++) {
        cout << setw(12) << genNames[gen] << " | ";

        for(int ls = 0; ls < 4; ls++) {
            int stars = min((int)(results[gen][ls].cpi * 2), 50); // Scale to max 50 chars
            cout << string(stars, '*') << " (" << lineSizes[ls] << "B) ";
            if(ls < 3) cout << "\n" << string(15, ' '); // New line for readability
        }
        cout << "\n\n";
    }
}

void generateSimpleChart(SimResult results[][4], string genNames[], int lineSizes[]) {
    cout << "\n=== Performance Chart ===\n";
    cout << "Generator    | 16B    | 32B    | 64B    | 128B   | Trend\n";
    cout << string(60, '-') << "\n";

    for(int gen = 0; gen < 5; gen++) {
        cout << setw(12) << genNames[gen] << " | ";

        for(int ls = 0; ls < 4; ls++) {
            cout << setw(6) << fixed << setprecision(2) << results[gen][ls].cpi << " | ";
        }

        // Show trend
        double improvement = ((results[gen][0].cpi - results[gen][3].cpi) / results[gen][0].cpi) * 100;
        if(improvement > 5) cout << " [Down] BETTER";
        else if(improvement < -5) cout << " [Down] WORSE";
        else cout << "  STABLE";

        cout << "\n";
    }
}
void generateHTMLReport(SimResult results[][4], string genNames[], int lineSizes[]) {
    ofstream htmlFile("cache_results.html");

    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile << "<title>Cache Performance Results</title>\n";
    htmlFile << "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
    htmlFile << "</head>\n<body>\n";
    htmlFile << "<h1>Cache Performance Analysis</h1>\n";
    htmlFile << "<canvas id=\"cpiChart\" width=\"800\" height=\"400\"></canvas>\n";

    // JavaScript data
    htmlFile << "<script>\n";
    htmlFile << "const ctx = document.getElementById('cpiChart').getContext('2d');\n";
    htmlFile << "const chart = new Chart(ctx, {\n";
    htmlFile << "  type: 'line',\n";
    htmlFile << "  data: {\n";
    htmlFile << "    labels: [16, 32, 64, 128],\n";
    htmlFile << "    datasets: [\n";

    for(int gen = 0; gen < 5; gen++) {
        htmlFile << "      {\n";
        htmlFile << "        label: '" << genNames[gen] << "',\n";
        htmlFile << "        data: [";
        for(int ls = 0; ls < 4; ls++) {
            htmlFile << results[gen][ls].cpi;
            if(ls < 3) htmlFile << ", ";
        }
        htmlFile << "],\n";
        htmlFile << "        borderColor: 'hsl(" << (gen * 60) << ", 70%, 50%)',\n";
        htmlFile << "        fill: false\n";
        htmlFile << "      }";
        if(gen < 4) htmlFile << ",";
        htmlFile << "\n";
    }

    htmlFile << "    ]\n  },\n";
    htmlFile << "  options: {\n";
    htmlFile << "    responsive: true,\n";
    htmlFile << "    scales: {\n";
    htmlFile << "      y: { type: 'logarithmic' }\n";
    htmlFile << "    }\n";
    htmlFile << "  }\n";
    htmlFile << "});\n";
    htmlFile << "</script>\n</body>\n</html>";

    htmlFile.close();
    cout << "✓ Interactive HTML report generated: cache_results.html\n";
}
int main() {
    srand(time(0));
    // Run tests first
    cout << "Two-Level Cache Performance Simulator\n";
    cout << "=====================================\n";
    cout << "\nRUNNING VERIFICATION TESTS...\n";
    cout << string(50, '=') << "\n";

    CacheSimulatorTester::testBasicCacheOperations();
    CacheSimulatorTester::testMemoryGenerators();

    cout << "\n" << string(50, '=') << "\n";
    cout << "ALL TESTS COMPLETED! Starting main simulation...\n";
    cout << string(50, '=') << "\n\n";

    // Configuration info
    cout << "Configuration:\n";
    cout << "  L1: " << L1_SIZE/1024 << "KB, " << L1_ASSOCIATIVITY << "-way, variable line size\n";
    cout << "  L2: " << L2_SIZE/1024 << "KB, " << L2_ASSOCIATIVITY << "-way, " << L2_LINE_SIZE << "B line size\n";
    cout << "  Memory access probability: 35%\n";
    cout << "  Write probability: 50%\n";
    cout << "  Iterations per test: 1,000,000\n\n";

    // Array of memory generators
    unsigned int (*memGens[5])() = {sequential_addresses, random_address_0_and_24KB, random_general_access, sequential_addresses_until_4KB, sequential_addresses_of_32B_until_1MB};
    string genNames[5] = {"Sequential", "Random till 24KB", "Random", "Sequential to 4KB", "Sequential 32B to 1MB"};

    // L1 line sizes to test
    int lineSizes[4] = {16, 32, 64, 128};

    // Declare results array
    SimResult results[5][4];  // [generator][line_size]

    // Run all simulations
    for (int genIdx = 0; genIdx < 5; genIdx++) {
        cout << "Testing " << genNames[genIdx] << ":\n";
        for (int ls = 0; ls < 4; ls++) {
            results[genIdx][ls] = runSimulation(memGens[genIdx], genNames[genIdx], lineSizes[ls]);
        }
        cout << endl;
    }

    // Print CPI results table
    cout << "\nCPI Results:\n";
    cout << "============\n";
    cout << setw(12) << "Generator";
    for (int i = 0; i < 4; i++) {
        cout << setw(12) << (to_string(lineSizes[i]) + "B");
    }
    cout << endl;
    cout << string(60, '-') << endl;

    for (int genIdx = 0; genIdx < 5; genIdx++) {
        cout << setw(12) << genNames[genIdx];
        for (int ls = 0; ls < 4; ls++) {
            cout << setw(12) << fixed << setprecision(3) << results[genIdx][ls].cpi;
        }
        cout << endl;
    }

    // Print L1 Hit Rate table
    cout << "\nL1 Hit Rates (%):\n";
    cout << "=================\n";
    cout << setw(12) << "Generator";
    for (int i = 0; i < 4; i++) {
        cout << setw(12) << (to_string(lineSizes[i]) + "B");
    }
    cout << endl;
    cout << string(60, '-') << endl;

    for (int genIdx = 0; genIdx < 5; genIdx++) {
        cout << setw(12) << genNames[genIdx];
        for (int ls = 0; ls < 4; ls++) {
            cout << setw(12) << fixed << setprecision(1) << (results[genIdx][ls].l1_hit_rate * 100);
        }
        cout << endl;
    }

    // Print L2 Hit Rate table
    cout << "\nL2 Hit Rates (%):\n";
    cout << "=================\n";
    cout << setw(12) << "Generator";
    for (int i = 0; i < 4; i++) {
        cout << setw(12) << (to_string(lineSizes[i]) + "B");
    }
    cout << endl;
    cout << string(60, '-') << endl;

    for (int genIdx = 0; genIdx < 5; genIdx++) {
        cout << setw(12) << genNames[genIdx];
        for (int ls = 0; ls < 4; ls++) {
            cout << setw(12) << fixed << setprecision(1) << (results[genIdx][ls].l2_hit_rate * 100);
        }
        cout << endl;
    }

    // Data for graphing (CSV format)
    cout << "\nData for Graphing (CSV format):\n";
    cout << "===============================\n";
    cout << "Generator,LineSize,CPI,L1_HitRate,L2_HitRate,AvgMemTime\n";
    for (int genIdx = 0; genIdx < 5; genIdx++) {
        for (int ls = 0; ls < 4; ls++) {
            cout << genNames[genIdx] << "," << lineSizes[ls] << ","
                 << fixed << setprecision(4) << results[genIdx][ls].cpi << ","
                 << results[genIdx][ls].l1_hit_rate << ","
                 << results[genIdx][ls].l2_hit_rate << ","
                 << results[genIdx][ls].avg_mem_access_time << endl;
        }
    }

    PerformanceValidator::validateResults(results);
    outputCSVResults(results, genNames, lineSizes);
    return 0; 
}