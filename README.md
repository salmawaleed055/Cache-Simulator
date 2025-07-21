# Two-Level Cache Performance Simulator

A comprehensive cache hierarchy simulator that analyzes the impact of L1 cache line sizes on system performance across different memory access patterns.

![Cache Simulator](https://img.shields.io/badge/Language-C++-blue.svg)
![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Cache Configuration](#cache-configuration)
- [Memory Access Patterns](#memory-access-patterns)
- [Output Analysis](#output-analysis)
- [Visualization](#visualization)
- [Results Interpretation](#results-interpretation)
- [Project Structure](#project-structure)
- [Academic Context](#academic-context)
- [Contributing](#contributing)

##  Overview

This simulator models a realistic two-level cache hierarchy found in modern processors. It evaluates how different L1 cache line sizes (16B, 32B, 64B, 128B) affect overall system performance measured in Cycles Per Instruction (CPI) across five distinct memory access patterns.

### Key Objectives:
- Analyze spatial locality effects in cache performance
- Compare cache behavior across different memory access patterns  
- Quantify the impact of cache line size on system performance
- Demonstrate cache hierarchy principles through simulation

##  Features

### Cache System
- **Two-level hierarchy**: L1 → L2 → Main Memory
- **Configurable L1 line sizes**: 16B, 32B, 64B, 128B
- **Set-associative caches**: 4-way L1, 8-way L2
- **Write-back policy** with dirty bit tracking
- **Random replacement algorithm**
- **Realistic timing model**: 1/10/50 cycle penalties

### Memory Patterns
- **Sequential access** (best-case spatial locality)
- **Random access in 24KB** (moderate working set)
- **Random access in 64MB** (worst-case scenario)
- **Sequential access in 4KB** (optimal working set)
- **Strided access (32B stride)** (stride vs. line size effects)

### Analysis Features
- **Automated verification tests**
- **Performance validation**
- **Hit rate analysis** (L1 and L2)
- **Multiple output formats**
- **ASCII visualization**
- **Statistical analysis**

##  System Requirements

### Minimum Requirements
- **OS**: Windows 10, macOS 10.14+, or Linux
- **Compiler**: GCC 7+, Clang 6+, or MSVC 2017+
- **C++ Standard**: C++11 or later
- **RAM**: 256MB available memory
- **Storage**: 50MB free space

### Recommended
- **Compiler**: GCC 9+ or equivalent
- **RAM**: 1GB+ for comfortable execution
- **Terminal**: Support for extended ASCII characters

### Optional (for Python visualization)
- **Python**: 3.7+
- **Packages**: pandas, matplotlib, seaborn

##  Installation

### Method 1: Direct Compilation
```bash
# Clone the repository
git clone https://github.com/yourusername/cache-simulator.git
cd cache-simulator

# Compile with optimizations
g++ -std=c++11 -O2 -o cache_simulator cache_simulator.cpp

# Run the simulator
./cache_simulator
```

### Method 2: Using CMake
```bash
mkdir build && cd build
cmake ..
make
./cache_simulator
```

### Method 3: Windows (Visual Studio)
1. Open `cache_simulator.cpp` in Visual Studio
2. Build in Release mode (Ctrl+Shift+B)
3. Run the executable

## 🎮 Usage

### Basic Execution
```bash
./cache_simulator
```

### Expected Output Flow
1. **Verification Tests** (~2 seconds)
2. **Configuration Display**
3. **Simulation Execution** (~30-60 seconds)
   - 5 memory generators × 4 line sizes = 20 test runs
   - 1M instructions per test = 20M total instructions
4. **Results Analysis and Visualization**

### Command Line Interface
```
Two-Level Cache Performance Simulator
=====================================

RUNNING VERIFICATION TESTS...
[PASS] Test 1 passed: First access is a miss
[PASS] Test 2 passed: Repeated access is a hit

Configuration:
  L1: 16KB, 4-way, variable line size
  L2: 128KB, 8-way, 64B line size
  Memory access probability: 35%
  Write probability: 50%
  Iterations per test: 1,000,000

Testing memGen1:
  Running memGen1 with L1 line size 16B... CPI = 1.925
  Running memGen1 with L1 line size 32B... CPI = 1.686
  ...
```

## ⚙️ Cache Configuration

### L1 Cache (Primary)
- **Size**: 16KB
- **Associativity**: 4-way set associative
- **Line Size**: 16B, 32B, 64B, or 128B (variable)
- **Hit Time**: 1 cycle
- **Sets**: Calculated as Size/(LineSize × Ways)

### L2 Cache (Secondary)  
- **Size**: 128KB
- **Associativity**: 8-way set associative
- **Line Size**: 64B (fixed)
- **Hit Time**: 10 cycles
- **Sets**: 256 sets

### Main Memory (DRAM)
- **Size**: 64GB (simulation space)
- **Access Time**: 50 cycles
- **Models**: Realistic DRAM latency

### Cache Policies
- **Write Policy**: Write-back with dirty bits
- **Replacement**: Random replacement algorithm
- **Allocation**: Write-allocate on misses

## 🔍 Memory Access Patterns

### memGen1: Sequential Access
```cpp
addr = 0, 1, 2, 3, 4, ... (modulo DRAM_SIZE)
```
- **Purpose**: Best-case spatial locality
- **Expected**: Strong line size benefits
- **Working Set**: Entire DRAM space (sequentially)

### memGen2: Random in 24KB
```cpp
addr = random() % (24 * 1024)
```
- **Purpose**: Medium working set analysis
- **Expected**: Stable performance across line sizes
- **Working Set**: 24KB (larger than L1, smaller than L2)

### memGen3: Random in 64MB
```cpp
addr = random() % (64 * 1024 * 1024)
```
- **Purpose**: Worst-case cache performance
- **Expected**: Poor hit rates, high CPI
- **Working Set**: 64MB (much larger than both caches)

### memGen4: Sequential in 4KB
```cpp
addr = 0, 1, 2, 3, ... (modulo 4KB)
```
- **Purpose**: Optimal working set size
- **Expected**: Excellent performance
- **Working Set**: 4KB (fits entirely in L1)

### memGen5: Strided Access (32B stride)
```cpp
addr = 0, 32, 64, 96, 128, ... (stride = 32B)
```
- **Purpose**: Stride vs. line size analysis
- **Expected**: Dramatic improvement when line_size ≥ stride
- **Working Set**: 1MB with 32-byte gaps

##  Output Analysis

### Performance Metrics

#### Primary Metric: CPI (Cycles Per Instruction)
- **Formula**: `Total_Cycles / Total_Instructions`
- **Range**: 1.0+ (1.0 = ideal performance)
- **Interpretation**: Lower is better

#### Cache Hit Rates
- **L1 Hit Rate**: `L1_Hits / (L1_Hits + L1_Misses)`
- **L2 Hit Rate**: `L2_Hits / (L2_Hits + L2_Misses)`
- **Range**: 0.0 to 1.0 (1.0 = 100% hit rate)

#### Average Memory Access Time
- **Formula**: `Total_Memory_Cycles / Total_Memory_Accesses`
- **Units**: Cycles per memory access
- **Factors**: Hit rates and penalty costs

### Sample Results Table
```
CPI Results:
============
   Generator         16B         32B         64B        128B
------------------------------------------------------------
     memGen1       1.925       1.686       1.554       1.277
     memGen2       3.065       3.064       3.067       3.052  
     memGen3      32.451      32.445      32.479      32.494
     memGen4       1.006       1.004       1.004       1.002
     memGen5      21.702      21.702      19.241      11.113
```

## 📈 Visualization

### ASCII Graphs (Built-in)
```
=== ASCII Performance Graph ===
CPI vs Line Size (each * represents ~0.5 CPI)

     memGen1 | *** (16B)  *** (32B)  *** (64B)  ** (128B)
     memGen4 | ** (16B)   ** (32B)   ** (64B)   ** (128B)
     memGen5 | *******************************************(16B)
               ********************** (128B)
```

### Performance Chart
```
Generator    | 16B    | 32B    | 64B    | 128B   | Trend
------------------------------------------------------------
     memGen1 |   1.92 |   1.69 |   1.55 |   1.28 |  DOWN BETTER
     memGen2 |   3.06 |   3.06 |   3.07 |   3.05 |  -> STABLE
     memGen5 |  21.70 |  21.70 |  19.24 |  11.11 |  DOWN BETTER
```

### Python Visualization (Optional)
If you have Python with matplotlib installed:

```bash
# After running the simulator
python3 analyze_cache_results.py
```

This generates professional graphs:
- **CPI vs Line Size** (log and linear scales)
- **Performance Heatmap**
- **Hit Rate Analysis**
- **Individual Pattern Analysis**

## 🔬 Results Interpretation

### Expected Performance Patterns

#### memGen1 (Sequential): Line Size Benefits
- **16B → 128B**: ~33% CPI improvement
- **Reason**: Larger lines capture more sequential data
- **L1 Hit Rate**: 93% → 99%

#### memGen2 (Random 24KB): Size-Limited
- **Performance**: Stable across line sizes (~3.0 CPI)
- **Reason**: Working set exceeds L1 capacity
- **L2 Hit Rate**: 99.8% (working set fits in L2)

#### memGen3 (Random 64MB): Locality-Limited
- **Performance**: Poor across all sizes (~32 CPI)
- **Reason**: No spatial or temporal locality
- **Hit Rates**: Near 0% (working set exceeds both caches)

#### memGen4 (Sequential 4KB): Capacity-Optimal
- **Performance**: Excellent (~1.0 CPI)
- **Reason**: Working set fits entirely in L1
- **L1 Hit Rate**: ~100%

#### memGen5 (Strided 32B): Stride Effects
- **16B-32B lines**: Poor performance (stride ≥ line size)
- **64B-128B lines**: Major improvement (line size > stride)
- **Demonstration**: Importance of line size vs. access stride

### Key Insights
1. **Spatial Locality Matters**: Sequential patterns benefit greatly from larger lines
2. **Working Set Size**: Performance limited by cache capacity vs. working set
3. **Access Pattern Impact**: Random access negates cache benefits
4. **Stride Relationship**: Line size must exceed stride for spatial locality benefits
5. **Cache Hierarchy**: L2 effectively catches L1 misses for medium working sets

##  Project Structure

```
cache-simulator/
├── cache_simulator.cpp          # Main simulator source code
├── simulation_results.csv       # Generated results data
├── cache_results.html          # Interactive HTML report
├── analyze_cache_results.py    # Python visualization script
├── README.md                   # This file
├── build/                      # Build directory (if using CMake)
└── docs/                       # Additional documentation
    ├── cache_architecture.md   # Cache design details
    ├── memory_patterns.md      # Memory pattern analysis
    └── performance_analysis.md # Results interpretation guide
```

### Key Components

#### Core Simulator (`cache_simulator.cpp`)
- **Cache Classes**: `Cache`, `CacheLine` data structures
- **Memory Generators**: 5 different access pattern functions
- **Simulation Engine**: Main simulation loop and timing model
- **Test Framework**: Automated verification and validation
- **Output System**: Console, CSV, and HTML generation

#### Generated Files
- **`simulation_results.csv`**: Raw data for external analysis
- **`cache_results.html`**: Interactive charts using Chart.js
- **Console Output**: Immediate results and ASCII visualizations

## 🎓 Academic Context

### Course Integration
This simulator is designed for computer architecture courses covering:
- **Cache Memory Systems**
- **Memory Hierarchy Design** 
- **Performance Analysis**
- **Spatial and Temporal Locality**
- **Cache Optimization Techniques**

### Learning Objectives
Students will understand:
1. **Cache Hierarchy Operation**: How multi-level caches work together
2. **Line Size Trade-offs**: Benefits and costs of larger cache lines
3. **Access Pattern Impact**: How software affects hardware performance
4. **Performance Metrics**: CPI analysis and optimization strategies
5. **Design Space Exploration**: Systematic performance evaluation

### Assignment Usage
- **Data Collection**: 20 simulation runs (5 patterns × 4 line sizes)
- **Analysis Report**: Graph interpretation and conclusions
- **Performance Comparison**: Quantitative analysis of results
- **Design Recommendations**: Cache optimization strategies

##  Files Generated

### Simulation Outputs
| File | Format | Purpose | Size |
|------|---------|---------|------|
| `simulation_results.csv` | CSV | Python/Excel analysis | ~2KB |
| `cache_results.html` | HTML | Interactive visualization | ~15KB |
| Console output | Text | Immediate results | N/A |

CSV Data Format
```csv
Generator,LineSize,CPI,L1_HitRate,L2_HitRate,AvgMemTime
memGen1,16,1.9246,0.9375,0.8467,3.6478
memGen1,32,1.6860,0.9687,0.7137,2.9646
...
```

### HTML Features
- **Interactive Charts**: Zoom, pan, toggle data series
- **Responsive Design**: Works on mobile and desktop
- **Export Options**: Save charts as images
- **Real-time Data**: Updates with new simulation runs

## 
