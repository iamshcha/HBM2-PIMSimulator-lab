#include "labs/CustomPIMKernel.h"

#include <iomanip>
#include <string>

#include "AddressMapping.h"
#include "labs/CustomPIMCmdGen.h"

void CustomPIMKernel::runPIM()
{
    while (mem_->hasPendingTransactions())
    {
        cycle_++;
        mem_->update();
    }
}

uint64_t CustomPIMKernel::getCycle()
{
    return cycle_;
}

void CustomPIMKernel::parkIn()
{
    addBarrier();
    for (int& ch_idx : pim_chans_)
    {
        for (int& ra_idx : pim_ranks_)
        {
            for (int bank_idx = 0; bank_idx < num_banks_ / num_bank_groups_; bank_idx++)
            {
                for (int bg_idx = 0; bg_idx < num_bank_groups_; bg_idx++)
                {
                    string str = "PARK_IN_";
                    if (bg_idx == 0 && bank_idx == 0)
                        str = "START_" + str;
                    else if (bg_idx == 3 && bank_idx == 3)
                        str = "END_" + str;
                    mem_->addTransaction(
                        false,
                        pim_addr_mgr_->addrGen(ch_idx, ra_idx, bg_idx, bank_idx, (1 << 13), 0), str,
                        &null_bst_);
                }
            }
        }
    }
    addBarrier();
}

void CustomPIMKernel::parkOut()
{
    for (int& ch_idx : pim_chans_)
    {
        for (int& ra_idx : pim_ranks_)
        {
            for (int bank_idx = 0; bank_idx < num_banks_ / num_bank_groups_; bank_idx++)
            {
                for (int bg_idx = 0; bg_idx < num_bank_groups_; bg_idx++)
                {
                    string str = "PARK_OUT_";
                    if (bg_idx == 0 && bank_idx == 0)
                        str = "START_" + str;
                    else if (bg_idx == 3 && bank_idx == 3)
                        str = "END_" + str;
                    mem_->addTransaction(
                        false,
                        pim_addr_mgr_->addrGen(ch_idx, ra_idx, bg_idx, bank_idx, (1 << 13), 0), str,
                        &null_bst_);
                }
            }
        }
    }
    addBarrier();
}

void CustomPIMKernel::addTransactionAll(bool is_write, int bg_idx, int bank_idx, int row, int col,
                                        const string tag, BurstType* bst, bool use_barrier,
                                        int num_loop)
{
    for (int& ch_idx : pim_chans_)
        for (int& ra_idx : pim_ranks_)
        {
            unsigned local_row = row;
            unsigned local_col = col;
            for (int i = 0; i < num_loop; i++)
            {
                uint64_t addr = pim_addr_mgr_->addrGenSafe(ch_idx, ra_idx, bg_idx, bank_idx,
                                                           local_row, local_col);
                (tag != "") ? mem_->addTransaction(is_write, addr, tag, bst)
                            : mem_->addTransaction(is_write, addr, bst);
                local_col++;
            }
        }

    if (use_barrier)
        addBarrier();
}

void CustomPIMKernel::addTransactionAll(bool is_write, int bg_idx, int bank_idx, int row, int col,
                                        BurstType* bst, bool use_barrier, int num_loop)
{
    addTransactionAll(is_write, bg_idx, bank_idx, row, col, "", bst, use_barrier, num_loop);
}

void CustomPIMKernel::addBarrier()
{
    for (int& ch_idx : pim_chans_) mem_->addBarrier(ch_idx);
}

void CustomPIMKernel::changePIMMode(dramMode curMode, dramMode nextMode)
{
    if (curMode == dramMode::SB && nextMode == dramMode::HAB)
    {
        addTransactionAll(true, 0, 0, pim_abmr_ra_, 0x1f, "START_SB_TO_HAB_", &null_bst_);
        addTransactionAll(true, 0, 1, pim_abmr_ra_, 0x1f, &null_bst_);
        if (num_banks_ >= 2)
        {
            addTransactionAll(true, 2, 0, pim_abmr_ra_, 0x1f, &null_bst_);
            addTransactionAll(true, 2, 1, pim_abmr_ra_, 0x1f, "END_SB_TO_HAB_", &null_bst_);
        }
    }
    else if (curMode == dramMode::HAB)
    {
        if (nextMode == dramMode::SB)
        {
            addTransactionAll(true, 0, 0, pim_sbmr_ra_, 0x1f, "START_HAB_TO_SB", &null_bst_);
            addTransactionAll(true, 0, 1, pim_sbmr_ra_, 0x1f, "END_HAB_TO_SB", &null_bst_);
        }
        else if (nextMode == dramMode::HAB_PIM)
        {
            addTransactionAll(true, 0, 0, pim_reg_ra_, 0x0, "PIM", &bst_hab_pim_);
        }
    }
    else if (curMode == dramMode::HAB_PIM && nextMode == dramMode::HAB)
        addTransactionAll(true, 0, 0, pim_reg_ra_, 0x0, "PIM", &bst_hab_);

    addBarrier();
}

void CustomPIMKernel::programCrf(vector<PIMCmd>& cmds)
{
    PIMCmd nop_cmd(PIMCmdType::NOP, 0);
    for (int i = 0; i < 4; i++)
    {
        if (i * 8 >= cmds.size())
            break;
        crf_bst_[i].set(nop_cmd.toInt(), nop_cmd.toInt(), nop_cmd.toInt(), nop_cmd.toInt(),
                        nop_cmd.toInt(), nop_cmd.toInt(), nop_cmd.toInt(), nop_cmd.toInt());
        for (int j = 0; j < 8; j++)
        {
            if (i * 8 + j >= cmds.size())
                break;
            crf_bst_[i].u32Data_[j] = cmds[i * 8 + j].toInt();
        }
        addTransactionAll(true, 0, 1, pim_reg_ra_, 0x4 + i, "PROGRAM_CRF", &(crf_bst_[i]));
    }
    addBarrier();
}

void CustomPIMKernel::setControl(BurstType* bst, bool pim_op, int crf_toggle_cond, bool grfA_zero,
                                 bool grfB_zero)
{
    bst->u8Data_[0] = pim_op;
    bst->u8Data_[16] = crf_toggle_cond;
    bst->u8Data_[20] = grfA_zero;
    bst->u8Data_[21] = grfB_zero;
}

unsigned CustomPIMKernel::getResultColGemv(int input_dim, int output_dim)
{
    int num_output_tiles = ceil(((double)output_dim / (num_total_pim_blocks_)) / num_grfB_);
    int num_input_tiles = ceil((double)input_dim / (double)num_grfA_);

    return num_output_tiles * num_input_tiles / 2 * num_grfA_ * num_grfB_;
}

void CustomPIMKernel::changeBank(pimBankType pb_type, int& ch_idx, int& ra_idx, int& bg_idx,
                                 int& bank_idx, unsigned& starting_row, unsigned& starting_col,
                                 unsigned& row, unsigned& col)
{
    bank_idx += (pb_type == pimBankType::ALL_BANK) ? 1 : (num_banks_ / num_pim_blocks_);

    if (bank_idx >= (num_banks_ / num_bank_groups_))
    {
        bank_idx = 0;
        if (++bg_idx >= num_bank_groups_)
        {
            bg_idx = 0;
            if (++ra_idx >= num_pim_ranks_)
            {
                ra_idx = 0;
                if (++ch_idx >= num_pim_chans_)
                {
                    ch_idx = 0;
                    starting_row = row;
                    starting_col = col;
                }
            }
        }
    }
}

void CustomPIMKernel::preloadGemv(NumpyBurstType* operand, unsigned starting_row,
                                  unsigned starting_col)
{
    int input_tile_size = num_grfA_;
    int output_tile_size = num_grfB_ * num_total_pim_blocks_;

    int ch_idx = 0, ra_idx = 0, bg_idx = 0, bank_idx = 0;
    unsigned row = 0, col = 0;
    uint64_t addr;

    unsigned even_starting_row = starting_row, odd_starting_row = starting_row;
    unsigned even_starting_col = starting_col, odd_starting_col = starting_col;

    for (int y = 0; y < operand->bShape[0]; y += output_tile_size)
    {
        for (int x = 0; x < operand->bShape[1]; x += input_tile_size)
        {
            bool is_odd = ((x / input_tile_size) % 2 == 1) ? true : false;

            for (int tiled_y = 0; tiled_y < output_tile_size; tiled_y += num_grfB_)
            {
                row = (is_odd) ? odd_starting_row : even_starting_row;
                col = (is_odd) ? odd_starting_col : even_starting_col;

                for (int grfb_idx = 0; grfb_idx < num_grfB_; grfb_idx++)
                {
                    for (int grfa_idx = 0; grfa_idx < num_grfA_; grfa_idx++, col++)
                    {
                        addr = pim_addr_mgr_->addrGenSafe(ch_idx, ra_idx, bg_idx, bank_idx + is_odd,
                                                          row, col);
                        int d_idx = (y + tiled_y + grfb_idx) * operand->bShape[1] + x + grfa_idx;
                        mem_->addTransaction(true, addr, &operand->bData[d_idx]);
                    }
                }
                is_odd ? changeBank(pimBankType::ODD_BANK, ch_idx, ra_idx, bg_idx, bank_idx,
                                    odd_starting_row, odd_starting_col, row, col)
                       : changeBank(pimBankType::EVEN_BANK, ch_idx, ra_idx, bg_idx, bank_idx,
                                    even_starting_row, even_starting_col, row, col);
            }
        }
    }
}

void CustomPIMKernel::executeGemv(NumpyBurstType* w_data, NumpyBurstType* i_data)
{
    int num_output_tiles = ceil(((double)w_data->bShape[0] / (num_total_pim_blocks_)) / num_grfB_);
    int num_input_tiles = ceil((double)w_data->bShape[1] / (double)num_grfA_);
    int num_batch = i_data->bShape[0];

    vector<PIMCmd> pim_cmds;
    int num_jump_of_even_bank = num_grfB_ * ceil((double)num_input_tiles / 2) - 1;
    int num_jump_of_odd_bank = num_grfB_ * floor(num_input_tiles / 2) - 1;
    pim_cmds = CustomPIMCmdGen::getPIMCmds(KernelType::GEMV, 0, num_jump_of_odd_bank,
                                           num_jump_of_even_bank);
    setControl(&bst_hab_pim_, true, getToggleCond(), false, true);
    parkIn();
    changePIMMode(dramMode::SB, dramMode::HAB);
    programCrf(pim_cmds);

    for (int j = 0; j < num_output_tiles; j++)
    {
        for (int b = 0; b < num_batch; b++)
        {
            changePIMMode(dramMode::HAB, dramMode::HAB_PIM);  // PC reset.

            int col = num_output_tiles * num_input_tiles / 2 * num_grfA_ * num_grfB_ +
                      (j + b) * num_grfB_;
            for (int i = 0; i < num_input_tiles; i += 2)
                computeGemv(i_data, num_input_tiles, num_output_tiles, i, j, b,
                            pimBankType::EVEN_BANK);
            for (int i = 1; i < num_input_tiles; i += 2)
                computeGemv(i_data, num_input_tiles, num_output_tiles, i, j, b,
                            pimBankType::ODD_BANK);
            addTransactionAll(true, 0, 1, 0, col, "GRFB_TO_BANK_", &null_bst_, true, num_grf_);
            changePIMMode(dramMode::HAB_PIM, dramMode::HAB);  // for grfBReset
        }
    }
    changePIMMode(dramMode::HAB, dramMode::SB);
    parkOut();
}

void CustomPIMKernel::computeGemv(NumpyBurstType* data, int num_input_tiles, int num_output_tiles,
                                  int inputTile, int outputTile, int batchIdx, pimBankType pb_type)
{
    for (int ch_idx = 0; ch_idx < num_pim_chans_; ch_idx++)
    {
        for (int ra_idx = 0; ra_idx < num_pim_ranks_; ra_idx++)
        {
            // input upload to GRF
            for (int gidx = 0; gidx < num_grfA_; gidx++)
            {
                string str = "WRIO_TO_GRF_";
                uint64_t addr =
                    pim_addr_mgr_->addrGen(ch_idx, ra_idx, 0, 1, pim_reg_ra_, 0x8 + gidx);
                int input_idx =
                    batchIdx * num_grfA_ * num_input_tiles + inputTile * num_grfA_ + gidx;
                mem_->addTransaction(true, addr, str, &data->bData[input_idx]);
            }
            mem_->addBarrier(ch_idx);
        }
    }

    unsigned row = 0;
    unsigned col = (num_grfA_ * num_grfB_) * (inputTile / 2 + outputTile * num_input_tiles / 2);

    unsigned num_mac_per_tile = num_grfA_ * num_grfB_;
    for (int c_idx = 0; c_idx < num_mac_per_tile; c_idx += 8)
        addTransactionAll(false, 0, (int)pb_type, row, col + c_idx, "MAC_", &null_bst_, true,
                          num_grfA_);
}

void CustomPIMKernel::readResult(BurstType* resultBst, pimBankType pb_type, int output_dim,
                                 uint64_t base_addr, unsigned starting_row, unsigned starting_col)
{
    int ch_idx = 0;
    int ra_idx = 0;
    int bg_idx = 0;
    int bank_idx = 0;
    int bank_offset = (int)pb_type % 2;
    uint64_t addr;

    for (int x = 0; x < output_dim; x += num_grf_)
    {
        unsigned row = starting_row;
        unsigned col = starting_col;

        for (int grf_idx = 0; grf_idx < num_grf_; grf_idx++)
        {
            addr = pim_addr_mgr_->addrGenSafe(ch_idx, ra_idx, bg_idx, bank_idx + bank_offset, row,
                                              col);
            mem_->addTransaction(false, base_addr + addr, "output", &resultBst[x + grf_idx]);
            col++;
        }
        changeBank(pb_type, ch_idx, ra_idx, bg_idx, bank_idx, starting_row, starting_col, row, col);
    }
}

void CustomPIMKernel::readData(BurstType* bst_data, size_t bst_cnt, unsigned starting_row,
                               unsigned starting_col)
{
    uint64_t init_addr = pim_addr_mgr_->addrGenSafe(0, 0, 0, 0, starting_row, starting_col);

    for (uint64_t addr = init_addr, i = 0; i < bst_cnt; addr += transaction_size_, i++)
    {
        mem_->addTransaction(false, addr, &bst_data[i]);
    }
}

void CustomPIMKernel::executeMemset(int dim, NumpyBurstType* i_data)
{
    /*
     * TODO: Lab2-1
     * Add the modechange functions to the appropriate line of code.
     * - SB mode: dramMode::SB
     * - HAB mode: dramMode::HAB
     * - PIM mode: dramMode::HAB_PIM
     * - mode change function from current mode to next mode: changePIMMode(currentMode, nextMode)
     *
     * exampe) SB->HAB changePIMMode(dramMode::SB, dramMode::HAB)
     */

    int num_output_tiles = ceil((dim / (num_total_pim_blocks_)));

    vector<PIMCmd> pim_cmds;
    pim_cmds = CustomPIMCmdGen::getPIMCmds(KernelType::MEMSET, 0, 0, num_output_tiles);
    setControl(&bst_hab_pim_, true, getToggleCond(), false, false);
    parkIn();
    programCrf(pim_cmds);

    // GRF update
    for (int ch_idx = 0; ch_idx < num_pim_chans_; ch_idx++)
    {
        for (int ra_idx = 0; ra_idx < num_pim_ranks_; ra_idx++)
        {
            // data upload to GRF
            for (int gidx = 0; gidx < 1; gidx++)
            {
                uint64_t addr =
                    pim_addr_mgr_->addrGen(ch_idx, ra_idx, 0, 0, pim_reg_ra_, 0x8 + gidx);
                mem_->addTransaction(true, addr, "WRIO_TO_GRF_", &i_data->bData[gidx]);
            }
            mem_->addBarrier(ch_idx);
        }
    }

    // memory set
    for (int j = 0; j < num_output_tiles; j++)
    {
        addTransactionAll(true, 0, 0, 0, j , "GRFA_TO_BANK_", &null_bst_, true, 1);
    }

    parkOut();
}
