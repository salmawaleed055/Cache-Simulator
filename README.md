# Two-Level Cache Performance Simulator

A comprehensive cache hierarchy simulator that analyzes the impact of L1 cache line sizes on system performance across different memory access patterns.

![Cache Simulator](https://img.shields.io/badge/Language-C++-blue.svg)
![Plotting](https://img.shields.io/badge/Visualization-Python-yellow.svg)

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Cache Configuration](#cache-configuration)
- [Memory Access Patterns](#memory-access-patterns)
- [Output Analysis](#output-analysis)
- [Visualization](#python-visualization)
- [Results Interpretation](#results-interpretation)
- [Project Structure](#project-structure)
- [Academic Context](#academic-context)

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

##  System Requirements

### Minimum Requirements
- **OS**: Windows 10, macOS 10.14+, or Linux
- **Compiler**: GCC 7+, Clang 6+, or MSVC 2017+
- **C++ Standard**: C++11 or later

##  Installation

### Method 1: Direct Compilation
```bash
# Clone the repository
git clone https://github.com/salmawaleed055/Cache-Simulator
cd Cache-Simulator

# Compile with optimizations
g++ -std=c++11 -O2 -o cache_simulator Main.cpp

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
1. Open `Main.cpp` in Visual Studio
2. Build in Release mode (Ctrl+Shift+B)
3. Run the executable

##  Usage

### Basic Execution
```bash
./cache_simulator
```

### Expected Output Flow
1. **Verification Tests** 
2. **Configuration Display**
3. **Simulation Execution** 
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

## Cache Configuration

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

##  Memory Access Patterns

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
| Generator        | 16B    | 32B    | 64B    | 128B   |
|------------------|--------|--------|--------|--------|
| Sequential       | 1.925  | 1.686  | 1.554  | 1.277  |
| Random 24KB      | 3.065  | 3.064  | 3.067  | 3.052  |
| Random           | 32.451 | 32.445 | 32.479 | 32.494 |
| Sequential 4KB   | 1.006  | 1.004  | 1.004  | 1.002  |
| Sequential 1MB   | 21.702 | 21.702 | 19.241 | 11.113 |
```

## Python Visualization

After generating the simulation results (`simulation_results.csv`), you can run the optional Python script to produce high-quality graphs for analysis.

#### Requirements:
- Python 3.7+
- `matplotlib`
- `pandas`
- `numpy`

#### Run:
```bash
python grapher.py
```


## Results Interpretation

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
Cache-Simulator/
├── main.cpp                    # Main simulator source code
├── simulation_results.csv      # Generated results data
├── grapher.py                  # Python visualization script
├── README.md                   # This file
├── build/                      # Build directory (if using CMake)
└── docs/                       # Additional documentation
```

### Key Components

#### Core Simulator (`cache_simulator.cpp`)
- **Cache Classes**: `Cache`, `CacheLine` data structures
- **Memory Generators**: 5 different access pattern functions
- **Simulation Engine**: Main simulation loop and timing model
- **Test Framework**: Automated verification and validation
- **Output System**: Console and CSV

#### Generated Files
- **`simulation_results.csv`**: Raw data for external analysis
- **Console Output**: Immediate results

## Academic Context

### Course Integration
This simulator is designed for Computer Organization and Assembly, Summer 2025 for course project 2 covering:
- **Cache Memory Systems**
- **Memory Hierarchy Design** 
- **Performance Analysis**
- **Spatial and Temporal Locality**
- **Cache Optimization Techniques**
