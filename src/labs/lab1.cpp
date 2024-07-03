#include <algorithm>
#include <iomanip>
#include <random>

#include "gtest/gtest.h"
#include "tests/PIMKernel.h"

using namespace DRAMSim;

class lab1 : public testing::Test
{
  public:
    lab1() {}
    ~lab1() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(lab1, 1_seq)
{
    uint64_t cur_cycle = 0;
    uint64_t data_size_in_byte;
    BurstType nullBst;

    shared_ptr<MultiChannelMemorySystem> mem;

    /* 1. set configuration */
    mem = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini",
                                                "system_hbm_2ch.ini", ".", "example_app", 256 * 2);
    // set data size
    data_size_in_byte = 1 << 22;  // 4MB

    /* 2. make input trace */
    bool is_write = false;
    uint64_t block_size_in_byte = 32;

    for (uint64_t dram_addr = 0; dram_addr < data_size_in_byte; dram_addr += block_size_in_byte)
    {
        mem->addTransaction(is_write, dram_addr, &nullBst);
    }

    /* 3. simulation */
    while (mem->hasPendingTransactions())  // while pending transactions (= input trace) remained
    {
        cur_cycle++;
        mem->update();  // update 1 cycle
    }

    /*
     * TODO: Lab1-1
     *  Modify `measured_mem_bw` to calculate memory bandwidth based on the simulated cycles
     *  - data size in GB / sec(measured cycle)
     *  - tCK = 1ns
     */
    float measured_mem_bw = 0.0;
    float ideal_mem_bw = 16.0 * 2;  // GBps * num_channels

    /* 4. result of simulation */
    cout << "execution cycles: " << cur_cycle << endl;
    cout << fixed;
    cout << setprecision(2) << "memory bandwidth(GB/s, ideal): " << ideal_mem_bw << endl;
    cout << setprecision(2) << "memory bandwidth(GB/s, measured): " << measured_mem_bw << endl;
}

TEST_F(lab1, 1_ran)
{
    bool is_write;
    uint64_t cur_cycle = 0;
    uint64_t data_size_in_byte;
    uint64_t block_size_in_byte;
    BurstType nullBst;
    vector<int> vec_raddr;
    random_device rd;
    mt19937 g(rd());

    shared_ptr<MultiChannelMemorySystem> mem;

    /* 1. set configuration */
    mem = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini",
                                                "system_hbm_2ch.ini", ".", "example_app", 256 * 2);
    block_size_in_byte =
        (getConfigParam(UINT, "JEDEC_DATA_BUS_BITS") * getConfigParam(UINT, "BL") / 8);

    is_write = false;  // false: read, true: write
    // set data size
    data_size_in_byte = 1 << 22;  // 4MB

    /* 2. make input trace */
    // insert addr to vector
    for (uint64_t dram_addr = 0; dram_addr < data_size_in_byte; dram_addr += block_size_in_byte)
    {
        vec_raddr.push_back(dram_addr);
    }
    // shuffle data to make random access pattern
    shuffle(vec_raddr.begin(), vec_raddr.end(), g);

    for (auto dram_addr : vec_raddr)
    {
        mem->addTransaction(is_write, dram_addr, &nullBst);
    }

    /* 3. simulation */
    while (mem->hasPendingTransactions())  // while pending transactions (= input
                                           // trace) remained
    {
        cur_cycle++;
        mem->update();  // update 1 cycle
    }

    /* 4. result of simulation */
    cout << "execution cycles: " << cur_cycle << endl;
}

TEST_F(lab1, 3)
{
    bool is_write;
    uint64_t cur_cycle = 0;
    uint64_t data_size_in_byte;
    uint64_t block_size_in_byte;
    vector<BurstType> vec_WriteBst;
    vector<BurstType> vec_ReadBst;
    BurstType WriteBst;
    BurstType ReadBst;
    random_device rd;
    mt19937 g(rd());

    shared_ptr<MultiChannelMemorySystem> mem;

    int cnt = 0;

    /* 1. set configuration */
    mem = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini",
                                                "system_hbm_2ch.ini", ".", "example_app", 256 * 2);
    block_size_in_byte =
        (getConfigParam(UINT, "JEDEC_DATA_BUS_BITS") * getConfigParam(UINT, "BL") / 8);

    // set data size
    data_size_in_byte = 1 << 10;  // 2 channels, 16 banks, 1 columns
    // data_size_in_byte = 1 << 15;    // 2 channels, 16 banks, 32 columns

    for (uint64_t dram_addr = 0; dram_addr < data_size_in_byte; dram_addr += block_size_in_byte)
    {
        // store address value to DRAM
        WriteBst.set((fp16)(dram_addr));
        vec_WriteBst.push_back(WriteBst);

        // make dummy BurstType
        vec_ReadBst.push_back(ReadBst);
        cnt++;
    }

    /* 2. make input trace */
    // write traces
    is_write = true;  // false: read, true: write
    for (uint64_t dram_addr = 0; dram_addr < data_size_in_byte; dram_addr += block_size_in_byte)
    {
        /*
         * TODO: Lab1-3
         * Modify `burst_index` for indexing data in addTransaction()
         */
        int burst_index = 0;

        is_write = true;  // false: read, true: write
        mem->addTransaction(is_write, dram_addr, &vec_WriteBst[burst_index]);
    }

    // read traces
    is_write = false;  // false: read, true: write
    int i = 0;
    for (uint64_t dram_addr = 0; dram_addr < data_size_in_byte; dram_addr += block_size_in_byte)
    {
        mem->addTransaction(is_write, dram_addr, &vec_ReadBst[i++]);
    }

    /* 3. simulation */
    while (mem->hasPendingTransactions())  // while pending transactions (= input
                                           // trace) remained
    {
        cur_cycle++;
        mem->update();  // update 1 cycle
    }

    /* 4. result of simulation */
    cout << "execution cycles: " << cur_cycle << endl;
}
