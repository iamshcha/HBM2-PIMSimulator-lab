#ifndef __CUSTOM_PIM_KERNEL_GEN_H__
#define __CUSTOM_PIM_KERNEL_GEN_H__

#include <vector>

#include "MultiChannelMemorySystem.h"
#include "PIMCmd.h"
#include "SystemConfiguration.h"
#include "tests/KernelAddrGen.h"

using namespace std;
using namespace DRAMSim;

class CustomPIMCmdGen
{
  public:
    static vector<PIMCmd> getPIMCmds(KernelType ktype, int num_jump_to_be_taken,
                                     int num_jump_to_be_taken_odd_bank,
                                     int num_jump_to_be_taken_even_bank);
};

#endif
