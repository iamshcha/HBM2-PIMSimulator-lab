# HBM2-PIMSimulator-lab

## 1. Overview
This repository is used for HBM2-PIM lectures at the KAIST AI-PIM Center. The lab lectures consist 
of slides explaining the lab and simulator code from this repository.

This simulator repository is based on the the HBM2-PIM simulator repository.
The existing HBM2-PIM simulator is managed and maintained at https://github.com/SAITPublic/PIMSimulator.
Please refer to this link to use it.

## 2. Setup

### 2.1 Prerequisites
* `Scons` tool for compiling PIMSimulator:
```bash
sudo apt install scons
```
* `gtest` for running test cases:
```bash
sudo apt install libgtest-dev
```

### 2.2 Installing
* To Install PIMSimulator:
```bash
# compile
scons
```

### 2.3 Launch a Test Run
* Show a list of test cases
```bash
./sim --gtest_list_tests

# Example
PIMKernelFixture.
  gemv_tree
  gemv
  mul
  add
  relu
MemBandwidthFixture.
  hbm_read_bandwidth
  hbm_write_bandwidth
PIMBenchFixture.
  gemv
  mul
  add
  relu
```

* Test Running
```bash
# Running: functionality test (GEMV)
./sim --gtest_filter=PIMKernelFixture.gemv

# Running: functionality test (MUL)
./sim --gtest_filter=PIMKernelFixture.mul

# Running: performance test (GEMV)
./sim --gtest_filter=PIMBenchFixture.gemv

# Running: performance test (ADD)
./sim --gtest_filter=PIMBenchFixture.add
```

## 3. More Information
More information about this simulator can be found [here](README_PIMSimulator.md)
