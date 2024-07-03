#include "gtest/gtest.h"
#include "labs/CustomPIMKernel.h"
#include "tests/TestCases.h"

using namespace DRAMSim;

// A predicate-formatter for asserting that two integers are mutually prime.
::testing::AssertionResult fp16EqualHelper(const char *m_expr, const char *n_expr, fp16 m, fp16 n);
::testing::AssertionResult fp16BstEqualHelper(const char *m_expr, const char *n_expr,
                                              DRAMSim::BurstType mb, DRAMSim::BurstType nb);
#define EXPECT_FP16_BST_EQ(val1, val2) EXPECT_PRED_FORMAT2(fp16BstEqualHelper, val1, val2)
#define EXPECT_FP16_EQ(val1, val2) EXPECT_PRED_FORMAT2(fp16EqualHelper, val1, val2)
class DataDim;

class lab4 : public testing::Test
{
  public:
    lab4() {}
    ~lab4() {}
    virtual void TearDown() {}

    void SetUp()
    {
        auto argv = ::testing::internal::GetArgvs();

        do_functional_test_ = false;
        for (int i = 0; i < argv.size(); i++)
        {
            if (argv[i].compare("--use_data") == 0 || argv[i].compare("--u") == 0)
            {
                do_functional_test_ = static_cast<bool>(atoi(argv[i + 1].c_str()));
            }
        }

        if (!do_functional_test_)
            cout << " Performance Test (w/o data)" << endl;
        else
            cout << " Functional Test (w/ data)" << endl;
    }

    bool do_functional_test_;
};

TEST_F(lab4, 1)
{
    uint64_t cur_cycle = 0;
    BurstType nullBst;
    shared_ptr<CustomPIMKernel> custom_pim_kernel;

    shared_ptr<MultiChannelMemorySystem> pim_mem;

    /* 1. set configuration */
    pim_mem = make_shared<MultiChannelMemorySystem>(
        "ini/HBM2_samsung_2M_16B_x64.ini", "system_hbm_64ch.ini", ".", "example_app", 256 * 64);

    /* get PIM Kernel */
    custom_pim_kernel = make_shared<CustomPIMKernel>(pim_mem, 64, 1);

    /* 2. GEMV pim kernel configuration */
    int input_dim = 4096;
    int output_dim = 4096;

    DataDim *dim_data = new DataDim(KernelType::GEMV, 1, output_dim, input_dim, true);
    dim_data->printDim(KernelType::GEMV);
    if (do_functional_test_)
        custom_pim_kernel->preloadGemv(&dim_data->weight_npbst_);

    /* 3. generate input traces from PIM Kernel (GEMV) */
    custom_pim_kernel->executeGemv(&dim_data->weight_npbst_, &dim_data->input_npbst_);

    /* 4. get result */
    BurstType *result = new BurstType[dim_data->output_dim_ * dim_data->batch_size_];
    if (do_functional_test_)
    {
        unsigned end_col = custom_pim_kernel->getResultColGemv(
            dim_data->dimTobShape(dim_data->input_dim_), dim_data->output_dim_);
        custom_pim_kernel->readResult(result, pimBankType::ODD_BANK,
                                      dim_data->output_dim_ * dim_data->batch_size_, 0, 0, end_col);
    }
    /* 4. pim simulation */
    custom_pim_kernel->runPIM();

    /* check functionality */
    if (do_functional_test_)
    {
        int num_tests = output_dim;
        for (int i = 0; i < num_tests; i++)
        {
            EXPECT_FP16_EQ(result[i].fp16ReduceSum(),
                           dim_data->output_npbst_.getBurst(0).fp16Data_[i]);
        }
    }

    /* 5. get cycle of simulation (GEMV) */
    cur_cycle = custom_pim_kernel->getCycle();

    cout << "execution cycles: " << cur_cycle << endl;
}
