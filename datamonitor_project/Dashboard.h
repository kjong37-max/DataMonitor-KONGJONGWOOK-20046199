#pragma once
#include "Metrics.h"
#include <Windows.h>

class Dashboard {
public:
    Dashboard();
    ~Dashboard();
    void render(const Snapshot& snap, int refreshSec, bool paused);

private:
    HANDLE hOut_;
    WORD   defaultAttr_ = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    int    width_       = 72;

    enum class C : WORD {
        BWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        Gray   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        Green  = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        Red    = FOREGROUND_RED | FOREGROUND_INTENSITY,
        Cyan   = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    };

    void moveTo(int x, int y);
    void color(C c);
    void reset();
    void padLine();
    void updateWidth();

    void sectionLabel(const wchar_t* label, int row);
    void bar(double pct, int w, C fillColor);
    void dataRow(int row, const wchar_t* label, const wchar_t* value, C vc);
    C    levelColor(double pct, double warn = 60.0, double crit = 85.0);

    int  renderHeader(const SYSTEMTIME& ts, int refreshSec, bool paused, int row);
    int  renderCpu(const CpuInfo& cpu, int row);
    int  renderMem(const MemInfo& mem, int row);
    int  renderDisks(const std::vector<DiskInfo>& disks, int row);
    int  renderDataStore(const DataStoreInfo& ds, int row);
    int  renderProcess(int count, int row);
    void renderFooter(int row, int refreshSec, bool paused);
};
