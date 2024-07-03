#include "labs/CustomPIMCmdGen.h"

vector<PIMCmd> CustomPIMCmdGen::getPIMCmds(KernelType ktype, int num_jump_to_be_taken,
                                           int num_jump_to_be_taken_odd_bank,
                                           int num_jump_to_be_taken_even_bank)
{
    vector<PIMCmd> pim_cmds;
    switch (ktype)
    {
        case KernelType::ADD: {
            vector<PIMCmd> tmp_cmds{
                PIMCmd(PIMCmdType::FILL, PIMOpdType::GRF_A, PIMOpdType::EVEN_BANK),
                PIMCmd(PIMCmdType::ADD, PIMOpdType::GRF_A, PIMOpdType::GRF_A, PIMOpdType::EVEN_BANK,
                       1),
                PIMCmd(PIMCmdType::NOP, 7),
                PIMCmd(PIMCmdType::FILL, PIMOpdType::GRF_B, PIMOpdType::ODD_BANK),
                PIMCmd(PIMCmdType::ADD, PIMOpdType::GRF_B, PIMOpdType::GRF_B, PIMOpdType::ODD_BANK,
                       1),
                PIMCmd(PIMCmdType::NOP, 7)};
            pim_cmds.assign(tmp_cmds.begin(), tmp_cmds.end());
            if (num_jump_to_be_taken != 0)
            {
                pim_cmds.push_back(
                    PIMCmd(PIMCmdType::JUMP, num_jump_to_be_taken, pim_cmds.size() + 1));
            }
            pim_cmds.push_back(PIMCmd(PIMCmdType::EXIT, 0));
            return pim_cmds;
        }
        case KernelType::GEMV: {
            vector<PIMCmd> tmp_cmds{
                PIMCmd(PIMCmdType::MAC, PIMOpdType::GRF_B, PIMOpdType::GRF_A, PIMOpdType::EVEN_BANK,
                       1, 0, 0, 0),
                PIMCmd(PIMCmdType::JUMP, num_jump_to_be_taken_even_bank, 2),
                PIMCmd(PIMCmdType::MAC, PIMOpdType::GRF_B, PIMOpdType::GRF_A, PIMOpdType::ODD_BANK,
                       1, 0, 0, 0),
                PIMCmd(PIMCmdType::JUMP, num_jump_to_be_taken_odd_bank, 2),
                PIMCmd(PIMCmdType::NOP, 7),
            };
            pim_cmds.assign(tmp_cmds.begin(), tmp_cmds.end());

            if (num_jump_to_be_taken != 0)
            {
                pim_cmds.push_back(
                    PIMCmd(PIMCmdType::JUMP, num_jump_to_be_taken, pim_cmds.size() + 1));
            }
            pim_cmds.push_back(PIMCmd(PIMCmdType::EXIT, 0));
            return pim_cmds;
        }
        case KernelType::MEMSET: {
            vector<PIMCmd> tmp_cmds{
                PIMCmd(PIMCmdType::MOV, PIMOpdType::EVEN_BANK, PIMOpdType::GRF_A, 0, 0, 0),
                PIMCmd(PIMCmdType::JUMP, num_jump_to_be_taken_even_bank - 1, 2),
            };
            pim_cmds.assign(tmp_cmds.begin(), tmp_cmds.end());

            if (num_jump_to_be_taken != 0)
            {
                pim_cmds.push_back(
                    PIMCmd(PIMCmdType::JUMP, num_jump_to_be_taken, pim_cmds.size() + 1));
            }
            pim_cmds.push_back(PIMCmd(PIMCmdType::EXIT, 0));
            return pim_cmds;
        }

        default:
            throw invalid_argument("Invalid kernel type");
            exit(0);
    }
}
