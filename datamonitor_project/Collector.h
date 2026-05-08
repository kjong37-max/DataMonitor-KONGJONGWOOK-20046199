#pragma once
#include "Metrics.h"

class Collector {
public:
    Collector();
    Snapshot collect();

private:
    FILETIME prevIdle_{}, prevKernel_{}, prevUser_{};
    bool     firstSample_ = true;

    CpuInfo               collectCpu();
    MemInfo               collectMem();
    std::vector<DiskInfo> collectDisks();
    int                   collectProcessCount();

    static double ftToSec(const FILETIME& ft);
};
