#pragma once
#include "Metrics.h"
#include <random>
#include <algorithm>

// 데이터 파이프라인/스토어를 시뮬레이션하는 PoC 클래스
class DataStoreSimulator {
public:
    DataStoreSimulator() : rng_(std::random_device{}()) {}

    DataStoreInfo update(int intervalSec = 1) {
        std::uniform_int_distribution<int64_t> inDist(200, 800);
        std::uniform_int_distribution<int64_t> procDist(180, 820);
        std::uniform_int_distribution<int>     errDist(0, 3);

        int64_t inRate   = inDist(rng_);
        int64_t procRate = procDist(rng_);

        totalRecords_ += static_cast<uint64_t>(inRate * intervalSec);
        pending_       = (std::max)(0LL, pending_ + (inRate - procRate) * intervalSec);
        errorCount_   += static_cast<uint64_t>(errDist(rng_));

        SYSTEMTIME st{};
        GetLocalTime(&st);

        wchar_t buf[32];
        swprintf_s(buf, L"%04d-%02d-%02d %02d:%02d:%02d",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond);

        DataStoreInfo info;
        info.totalRecords    = totalRecords_;
        info.incomingPerSec  = inRate;
        info.processedPerSec = procRate;
        info.pending         = pending_;
        info.errorCount      = errorCount_;
        info.lastUpdate      = buf;
        return info;
    }

private:
    std::mt19937 rng_;
    uint64_t     totalRecords_ = 1'000'000;
    int64_t      pending_      = 0;
    uint64_t     errorCount_   = 5;
};
