#include "gtest/gtest.h"
#include "labs/CustomPIMKernel.h"
#include "tests/TestCases.h"

using namespace DRAMSim;

// A predicate-formatter for asserting that two integers are mutually prime.
::testing::AssertionResult fp16EqualHelper(const char* m_expr, const char* n_expr, fp16 m, fp16 n);
::testing::AssertionResult fp16BstEqualHelper(const char* m_expr, const char* n_expr,
                                              DRAMSim::BurstType mb, DRAMSim::BurstType nb);
#define EXPECT_FP16_BST_EQ(val1, val2) EXPECT_PRED_FORMAT2(fp16BstEqualHelper, val1, val2)
#define EXPECT_FP16_EQ(val1, val2) EXPECT_PRED_FORMAT2(fp16EqualHelper, val1, val2)
class DataDim;

class lab2 : public testing::Test
{
  public:
    lab2() {}
    ~lab2() {}
    virtual void TearDown() {}

    void SetUp()
    {
        dim_ = 65536;
        stride_ = 16;
        set_val_ = (fp16)1234;
    }

    int dim_;
    int stride_;
    fp16 set_val_;
};

TEST_F(lab2, 1)
{
    uint64_t cur_cycle = 0;
    BurstType nullBst;
    shared_ptr<CustomPIMKernel> custom_pim_kernel;

    shared_ptr<MultiChannelMemorySystem> mem;

    /* 1. set configuration */
    mem = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini",
                                                "system_hbm_1ch.ini", ".", "example_app", 256 * 1);

    /* get PIM Kernel */
    custom_pim_kernel = make_shared<CustomPIMKernel>(mem, 1, 1);

    /* 2. data configuration */
    int num_bst = dim_ / stride_;

    /* 2.1 Set Data */
    NumpyBurstType memset_data_npbst;
    BurstType burst((set_val_), (set_val_), (set_val_), (set_val_), (set_val_), (set_val_),
                    (set_val_), (set_val_), (set_val_), (set_val_), (set_val_), (set_val_),
                    (set_val_), (set_val_), (set_val_), (set_val_));
    memset_data_npbst.bData.push_back(burst);

    /* 3. generate input traces from PIM Kernel (GEMV) */
    custom_pim_kernel->executeMemset(num_bst, &memset_data_npbst);

    /* 4. pim simulation */
    custom_pim_kernel->runPIM();

    /* 5. get cycle of simulation (GEMV) */
    cur_cycle = custom_pim_kernel->getCycle();

    /* 6. get result for functionality test */
    BurstType* result = new BurstType[num_bst];
    custom_pim_kernel->readResult(result, pimBankType::EVEN_BANK, num_bst, 0, 0, 0);
    custom_pim_kernel->runPIM();

    /* 7. check functionality */
    BurstType golden_Bst;
    golden_Bst.set((fp16)1234);

    for (int i = 0; i < num_bst; i++)
    {
        for (int j = 0; j < stride_; j++)
        {
            EXPECT_FP16_EQ(result[i].fp16Data_[j], golden_Bst.fp16Data_[j]);
        }
    }
    cout << "execution cycles: " << cur_cycle << endl;
}

TEST_F(lab2, 2)
{
    uint64_t cur_cycle = 0;
    BurstType nullBst;

    shared_ptr<MultiChannelMemorySystem> mem;
    shared_ptr<CustomPIMKernel> custom_pim_kernel;

    /* 1. set configuration */
    mem = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini",
                                                "system_hbm_1ch.ini", ".", "example_app", 256 * 1);
    custom_pim_kernel = make_shared<CustomPIMKernel>(mem, 1, 1);

    /* 2. data configuration */
    int num_bst = dim_ / stride_;

    /* 2.1 Set Data */
    BurstType memset_data_bst;
    memset_data_bst.set(set_val_);  // same functions as lines 57-59

    /* 2. make input trace */
    bool is_write = true;
    uint64_t block_size_in_byte = 32;

    uint64_t dram_addr = 0;
    for (int bst_num = 0; bst_num < num_bst; dram_addr += block_size_in_byte, bst_num++)
    {
        mem->addTransaction(is_write, dram_addr, &memset_data_bst);
    }

    /* 3. pim simulation */
    custom_pim_kernel->runPIM();

    /* 4. result of simulation */
    cur_cycle = custom_pim_kernel->getCycle();
    cout << "execution cycles: " << cur_cycle << endl;
}
