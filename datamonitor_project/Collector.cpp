#include "Collector.h"
#include <TlHelp32.h>

Collector::Collector() {
    GetSystemTimes(&prevIdle_, &prevKernel_, &prevUser_);
}

Snapshot Collector::collect() {
    Snapshot s;
    s.cpu          = collectCpu();
    s.mem          = collectMem();
    s.disks        = collectDisks();
    s.processCount = collectProcessCount();
    GetLocalTime(&s.timestamp);
    return s;
}

CpuInfo Collector::collectCpu() {
    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);

    SYSTEM_INFO si{};
    GetSystemInfo(&si);

    CpuInfo cpu;
    cpu.coreCount = static_cast<int>(si.dwNumberOfProcessors);

    if (!firstSample_) {
        double dIdle   = ftToSec(idle)   - ftToSec(prevIdle_);
        double dKernel = ftToSec(kernel) - ftToSec(prevKernel_);
        double dUser   = ftToSec(user)   - ftToSec(prevUser_);
        double total   = dKernel + dUser;
        cpu.usagePct   = (total > 0.0) ? (total - dIdle) / total * 100.0 : 0.0;
    }
    firstSample_ = false;
    prevIdle_    = idle;
    prevKernel_  = kernel;
    prevUser_    = user;
    return cpu;
}

MemInfo Collector::collectMem() {
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);

    MemInfo m;
    m.totalMB  = ms.ullTotalPhys >> 20;
    m.usedMB   = (ms.ullTotalPhys - ms.ullAvailPhys) >> 20;
    m.usagePct = static_cast<double>(ms.dwMemoryLoad);
    return m;
}

std::vector<DiskInfo> Collector::collectDisks() {
    std::vector<DiskInfo> result;
    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; ++i) {
        if (!(drives & (1u << i))) continue;
        wchar_t root[4] = { static_cast<wchar_t>(L'A' + i), L':', L'\\', L'\0' };
        if (GetDriveTypeW(root) != DRIVE_FIXED) continue;

        ULARGE_INTEGER avail{}, total{}, free{};
        if (!GetDiskFreeSpaceExW(root, &avail, &total, &free)) continue;

        DiskInfo d;
        d.label    = std::wstring(1, static_cast<wchar_t>(L'A' + i)) + L":";
        d.totalGB  = total.QuadPart >> 30;
        d.usedGB   = (total.QuadPart - free.QuadPart) >> 30;
        d.usagePct = (d.totalGB > 0) ? static_cast<double>(d.usedGB) / d.totalGB * 100.0 : 0.0;
        result.push_back(std::move(d));
    }
    return result;
}

int Collector::collectProcessCount() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    int count = 0;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe))
        do { ++count; } while (Process32NextW(snap, &pe));
    CloseHandle(snap);
    return count;
}

double Collector::ftToSec(const FILETIME& ft) {
    ULARGE_INTEGER ui{};
    ui.LowPart  = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return static_cast<double>(ui.QuadPart) * 1e-7;
}
