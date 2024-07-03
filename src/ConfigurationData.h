/***************************************************************************************************
 * Copyright (C) 2021 Samsung Electronics Co. LTD
 *
 * This software is a property of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed,
 * transmitted, transcribed, stored in a retrieval system, or translated into any human
 * or computer language in any form by any means,electronic, mechanical, manual or otherwise,
 * or disclosed to third parties without the express written permission of Samsung Electronics.
 * (Use of the Software is restricted to non-commercial, personal or academic, research purpose
 * only)
 **************************************************************************************************/

#ifndef DEFAULT_SYSTEM_CONFIGURATION_H_
#define DEFAULT_SYSTEM_CONFIGURATION_H_

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace DRAMSim
{
typedef enum _variableType
{
    STRING = 0,
    UINT,
    UINT64,
    FLOAT,
    BOOL,
} varType;
typedef enum _paramType
{
    SYS_PARAM = 0,
    DEV_PARAM,
} paramType;

typedef struct _ConfigurationData
{
    string name;
    varType variableType;
    paramType parameterType;
    string value;
} ConfigurationData;

#define DEFINE_STRING_CONFIG(name, paramtype) \
    {                                         \
#name, STRING, paramtype, ""          \
    }
#define DEFINE_UINT_CONFIG(name, paramtype)  \
    {                                        \
#name, UINT, paramtype, to_string(0) \
    }
#define DEFINE_UINT64_CONFIG(name, paramtype)  \
    {                                          \
#name, UINT64, paramtype, to_string(0) \
    }
#define DEFINE_FLOAT_CONFIG(name, paramtype)  \
    {                                         \
#name, FLOAT, paramtype, to_string(0) \
    }
#define DEFINE_BOOL_CONFIG(name, paramtype) \
    {                                       \
#name, BOOL, paramtype, "false"     \
    }
#define DEFINE_ENDOF_CONFIG     \
    {                           \
        "", UINT, SYS_PARAM, "" \
    }
#define DEFINE_DEFAULT_CONFIG(name, type, paramtype, val) \
    {                                                     \
#name, type, paramtype, val                       \
    }

const static ConfigurationData defaultConfiguration[] = {
    DEFINE_UINT_CONFIG(NUM_BANKS, DEV_PARAM),
    DEFINE_UINT_CONFIG(NUM_BANK_GROUPS, DEV_PARAM),
    DEFINE_UINT_CONFIG(NUM_ROWS, DEV_PARAM),
    DEFINE_UINT_CONFIG(NUM_COLS, DEV_PARAM),
    DEFINE_UINT_CONFIG(NUM_PIM_BLOCKS, DEV_PARAM),
    DEFINE_UINT_CONFIG(DEVICE_WIDTH, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRFC, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRFCSB, DEV_PARAM),
    DEFINE_UINT_CONFIG(tREFI, DEV_PARAM),
    DEFINE_UINT_CONFIG(tREFISB, DEV_PARAM),
    DEFINE_FLOAT_CONFIG(tCK, DEV_PARAM),
    DEFINE_UINT_CONFIG(AL, DEV_PARAM),
    DEFINE_UINT_CONFIG(BL, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRAS, DEV_PARAM),
    DEFINE_UINT_CONFIG(RL, DEV_PARAM),
    DEFINE_UINT_CONFIG(WL, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRCDRD, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRCDWR, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRC, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRP, DEV_PARAM),
    DEFINE_UINT_CONFIG(tWR, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRTRS, DEV_PARAM),
    DEFINE_UINT_CONFIG(XAW, DEV_PARAM),
    DEFINE_UINT_CONFIG(tXAW, DEV_PARAM),
    DEFINE_UINT_CONFIG(tCKE, DEV_PARAM),
    DEFINE_UINT_CONFIG(tXP, DEV_PARAM),
    DEFINE_UINT_CONFIG(tCMD, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD0, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD1, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD2P, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD2Q, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD2N, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD3Pf, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD3Ps, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD3N, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD4W, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD4R, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD5, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD6, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD6L, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD7, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD0C, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD0Q, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD3NC, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD3NQ, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD4WC, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD4WQ, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD4RC, DEV_PARAM),
    DEFINE_UINT_CONFIG(IDD4RQ, DEV_PARAM),
    DEFINE_FLOAT_CONFIG(Vddc, DEV_PARAM),
    DEFINE_FLOAT_CONFIG(Vddq, DEV_PARAM),
    DEFINE_FLOAT_CONFIG(Vpp, DEV_PARAM),
    DEFINE_FLOAT_CONFIG(Vdd, DEV_PARAM),
    DEFINE_UINT_CONFIG(Ealu, DEV_PARAM),
    DEFINE_UINT_CONFIG(Ereg, DEV_PARAM),
    DEFINE_UINT_CONFIG(NUM_CHANS, SYS_PARAM),
    DEFINE_UINT_CONFIG(JEDEC_DATA_BUS_BITS, SYS_PARAM),
    // Pre defined parameters
    DEFINE_UINT_CONFIG(READ_TO_WRITE_DELAY, DEV_PARAM),
    DEFINE_UINT_CONFIG(READ_TO_PRE_DELAY, DEV_PARAM),
    DEFINE_UINT_CONFIG(READ_TO_PRE_DELAY_LONG, DEV_PARAM),
    DEFINE_UINT_CONFIG(READ_TO_PRE_DELAY_SHORT, DEV_PARAM),
    DEFINE_UINT_CONFIG(WRITE_TO_PRE_DELAY, DEV_PARAM),
    DEFINE_UINT_CONFIG(READ_TO_WRITE_DELAY, DEV_PARAM),
    DEFINE_UINT_CONFIG(READ_AUTOPRE_DELAY, DEV_PARAM),
    DEFINE_UINT_CONFIG(WRITE_AUTOPRE_DELAY, DEV_PARAM),
    DEFINE_UINT_CONFIG(WRITE_TO_READ_DELAY_B_LONG, DEV_PARAM),
    DEFINE_UINT_CONFIG(WRITE_TO_READ_DELAY_B_SHORT, DEV_PARAM),
    DEFINE_UINT_CONFIG(WRITE_TO_READ_DELAY_R, DEV_PARAM),

    // Memory Controller related parameters
    DEFINE_UINT_CONFIG(TRANS_QUEUE_DEPTH, SYS_PARAM),
    DEFINE_UINT_CONFIG(CMD_QUEUE_DEPTH, SYS_PARAM),
    DEFINE_UINT_CONFIG(EPOCH_LENGTH, SYS_PARAM),
    // Power
    DEFINE_BOOL_CONFIG(USE_LOW_POWER, SYS_PARAM),
    DEFINE_UINT_CONFIG(TOTAL_ROW_ACCESSES, SYS_PARAM),
    DEFINE_STRING_CONFIG(ROW_BUFFER_POLICY, SYS_PARAM),
    DEFINE_STRING_CONFIG(SCHEDULING_POLICY, SYS_PARAM),
    DEFINE_STRING_CONFIG(ADDRESS_MAPPING_SCHEME, SYS_PARAM),
    DEFINE_STRING_CONFIG(QUEUING_STRUCTURE, SYS_PARAM),
    // debug flags
    DEFINE_BOOL_CONFIG(DEBUG_TRANS_Q, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_CMD_Q, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_ADDR_MAP, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_BANKSTATE, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_BUS, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_BANKS, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_POWER, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_CMD_TRACE, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_PIM_BLOCK, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_PIM_TIME, SYS_PARAM),
    DEFINE_BOOL_CONFIG(DEBUG_TRANS_DATA, DEV_PARAM),
    DEFINE_BOOL_CONFIG(VIS_FILE_OUTPUT, SYS_PARAM),
    DEFINE_BOOL_CONFIG(VERIFICATION_OUTPUT, SYS_PARAM),
    DEFINE_BOOL_CONFIG(PRINT_CHAN_STAT, DEV_PARAM),
    DEFINE_BOOL_CONFIG(SHOW_SIM_OUTPUT, DEV_PARAM),
    DEFINE_BOOL_CONFIG(LOG_OUTPUT, DEV_PARAM),
    // DDR4 support
    DEFINE_UINT_CONFIG(tCCDL, DEV_PARAM),
    DEFINE_UINT_CONFIG(tCCDS, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRRDL, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRRDS, DEV_PARAM),
    DEFINE_UINT_CONFIG(tWTRL, DEV_PARAM),
    DEFINE_UINT_CONFIG(tWTRS, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRTPL, DEV_PARAM),
    DEFINE_UINT_CONFIG(tRTPS, DEV_PARAM),
    DEFINE_STRING_CONFIG(PIM_PRECISION, SYS_PARAM),
    DEFINE_STRING_CONFIG(SIM_TRACE_FILE, SYS_PARAM),
    // number of latencies per bucket in the latency histogram
    DEFINE_DEFAULT_CONFIG(HISTOGRAM_BIN_SIZE, UINT, SYS_PARAM, "10"),
    // PIM related
    DEFINE_DEFAULT_CONFIG(PIM_MODE, STRING, SYS_PARAM, "mac_in_bankgroup"),
    DEFINE_DEFAULT_CONFIG(PIM_PRECISION, STRING, SYS_PARAM, "FP16"),
    // Controller related
    DEFINE_DEFAULT_CONFIG(ROW_BUFFER_POLICY, STRING, SYS_PARAM, "open_page"),
    DEFINE_DEFAULT_CONFIG(SCHEDULING_POLICY, STRING, SYS_PARAM, "rank_then_bank_round_robin"),
    DEFINE_DEFAULT_CONFIG(QUEUING_STRUCTURE, STRING, SYS_PARAM, "per_rank"),
    DEFINE_DEFAULT_CONFIG(ADDRESS_MAPPING_SCHEME, STRING, SYS_PARAM, "Scheme8"),  // shcha
    // WARNING, do not remove end of config macro
    DEFINE_ENDOF_CONFIG};
};  // namespace DRAMSim

#endif
