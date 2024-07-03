#ifndef __PIM_KERNEL_HPP__
#define __PIM_KERNEL_HPP__

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "MultiChannelMemorySystem.h"
#include "PIMCmd.h"
#include "SystemConfiguration.h"
#include "tests/KernelAddrGen.h"

using namespace std;
using namespace DRAMSim;

class CustomPIMKernel
{
  public:
    CustomPIMKernel(shared_ptr<MultiChannelMemorySystem> mem, int num_pim_chan, int num_pim_rank)
        : mem_(mem),
          num_pim_chans_(num_pim_chan),
          num_pim_ranks_(num_pim_rank),
          mode_(PIMConfiguration::getPIMMode()),
          num_banks_(getConfigParam(UINT, "NUM_BANKS")),
          num_pim_blocks_(getConfigParam(UINT, "NUM_PIM_BLOCKS")),
          num_bank_groups_(getConfigParam(UINT, "NUM_BANK_GROUPS")),
          srf_bst_(NULL),
          cycle_(0)
    {
        transaction_size_ = getConfigParam(UINT, "BL") *
                            (getConfigParam(UINT, "JEDEC_DATA_BUS_BITS") / 8);  // in byte

        // FIXME: HARDCODED
        num_grf_ = num_grfA_ = num_grfB_ = 8;
        num_total_pim_blocks_ = num_pim_blocks_ * num_pim_chans_ * num_pim_ranks_;
        zero_padding->set((fp16)0);

        pim_chans_.clear();
        for (int i = 0; i < num_pim_chans_; i++) pim_chans_.push_back(i);

        pim_ranks_.clear();
        for (int i = 0; i < num_pim_ranks_; i++) pim_ranks_.push_back(i);

        pim_addr_mgr_ = make_shared<PIMAddrManager>(num_pim_chan, num_pim_rank);
    }

    int transaction_size_;
    int num_pim_chans_, num_pim_ranks_;
    int num_grfA_, num_grfB_, num_grf_;
    int num_used_grfB_;
    int changed_in_shape;
    int changed_out_shape;
    shared_ptr<PIMAddrManager> pim_addr_mgr_;
    shared_ptr<BurstType> zero_padding = make_shared<BurstType>();

    void addBarrier();
    void runPIM();
    uint64_t getCycle();
    void parkIn();
    void parkOut();
    void changePIMMode(dramMode mode1, dramMode mode2);
    void addTransactionAll(bool isWrite, int bg, int bank, int row, int col, const std::string tag,
                           BurstType* bst, bool use_barrier = false, int num_loop = 1);
    void addTransactionAll(bool isWrite, int bg, int bank, int row, int col, BurstType* bst,
                           bool use_barrier = false, int num_loop = 1);
    void programCrf(vector<PIMCmd>& cmds);
    void setControl(BurstType* bst, bool op, int crf_toggle_cond, bool grfA_zero, bool grfB_zero);
    unsigned getResultColGemv(int input_dim, int output_dim);
    void changeBank(pimBankType bank_types, int& cidx, int& rank, int& bg, int& bank,
                    unsigned& startingRow, unsigned& startingCol, unsigned& row, unsigned& col);
    void preloadGemv(NumpyBurstType* operand, unsigned starting_row = 0, unsigned starting_col = 0);
    void preloadGemv_lab4_2(NumpyBurstType* operand, unsigned starting_row = 0,
                            unsigned starting_col = 0);
    void preloadGemv_lab4_3(NumpyBurstType* operand, unsigned starting_row = 0,
                            unsigned starting_col = 0);
    void executeGemv(NumpyBurstType* w_data, NumpyBurstType* i_data);
    void executeGemv_lab4_2(NumpyBurstType* w_data, NumpyBurstType* i_data);
    void executeGemv_lab4_3(NumpyBurstType* w_data, NumpyBurstType* i_data);
    void executeMemset(int dim, NumpyBurstType* i_data);
    void computeGemv(NumpyBurstType* data, int num_input_tiles, int num_output_tile, int input_tile,
                     int output_tile, int batch_idx, pimBankType bank_types);
    void computeGemv_lab4_2(NumpyBurstType* data, int num_input_tiles, int num_output_tile,
                            int input_tile, int output_tile, int batch_idx, pimBankType bank_types);
    void computeGemv_lab4_3(NumpyBurstType* data, int num_input_tiles, int num_output_tile,
                            int input_tile, int output_tile, int batch_idx, pimBankType bank_types);
    void readResult(BurstType* resultBst, pimBankType bank_types, int output_dim,
                    uint64_t baseAddr = 0, unsigned startingRow = 0, unsigned startingCol = 0);
    void readResult_lab4_2(BurstType* resultBst, pimBankType bank_types, int output_dim,
                           uint64_t baseAddr = 0, unsigned startingRow = 0,
                           unsigned startingCol = 0);
    void readResult_lab4_3(BurstType* resultBst, pimBankType bank_types, int output_dim,
                           uint64_t baseAddr = 0, unsigned startingRow = 0,
                           unsigned startingCol = 0);
    void readData(BurstType* bst_data, size_t bst_cnt, unsigned s_row = 0, unsigned s_col = 0);

  private:
    unsigned cycle_;
    unsigned num_banks_, num_pim_blocks_, num_bank_groups_, num_total_pim_blocks_;
    BurstType null_bst_, bst_hab_pim_, bst_hab_;
    BurstType crf_bst_[4];
    BurstType* srf_bst_;
    vector<int> pim_chans_;
    vector<int> pim_ranks_;
    PIMMode mode_;
    shared_ptr<MultiChannelMemorySystem> mem_;
    const uint32_t pim_reg_ra_ = 0x3fff;
    const uint32_t pim_abmr_ra_ = 0x27ff;
    const uint32_t pim_sbmr_ra_ = 0x2fff;
    int inline getToggleCond(pimBankType pb_type = pimBankType::ALL_BANK)
    {
        // set Toggle Condition
        switch (pb_type)
        {
            case pimBankType::EVEN_BANK:
                return 2;
            case pimBankType::ODD_BANK:
                return 1;
            case pimBankType::ALL_BANK:
                return 0;
            default:
                return -1;
        }
    }
};

#endif
