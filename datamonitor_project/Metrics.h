#pragma once
#define NOMINMAX
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdint>

struct CpuInfo {
    double usagePct  = 0.0;
    int    coreCount = 1;
};

struct MemInfo {
    uint64_t totalMB  = 0;
    uint64_t usedMB   = 0;
    double   usagePct = 0.0;
};

struct DiskInfo {
    std::wstring label;
    uint64_t     totalGB  = 0;
    uint64_t     usedGB   = 0;
    double       usagePct = 0.0;
};

struct DataStoreInfo {
    uint64_t     totalRecords    = 0;
    int64_t      incomingPerSec  = 0;
    int64_t      processedPerSec = 0;
    int64_t      pending         = 0;
    uint64_t     errorCount      = 0;
    std::wstring lastUpdate;
};

struct Snapshot {
    CpuInfo               cpu;
    MemInfo               mem;
    std::vector<DiskInfo> disks;
    int                   processCount = 0;
    DataStoreInfo         dataStore;
    SYSTEMTIME            timestamp{};
};
