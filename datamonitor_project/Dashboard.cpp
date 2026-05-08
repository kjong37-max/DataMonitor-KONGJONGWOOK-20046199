#include "Dashboard.h"
#include <cstdio>
#include <cwchar>
#include <algorithm>

Dashboard::Dashboard()
    : hOut_(GetStdHandle(STD_OUTPUT_HANDLE)) {
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (GetConsoleScreenBufferInfo(hOut_, &csbi))
        defaultAttr_ = csbi.wAttributes;

    CONSOLE_CURSOR_INFO ci{ 1, FALSE };
    SetConsoleCursorInfo(hOut_, &ci);
    SetConsoleTitleW(L"DataMonitor - \xb370\xc774\xd130 모\xb2c8\xd130\xb9c1 관\xb9ac\xc790 \xb3c4구");

    updateWidth();

    GetConsoleScreenBufferInfo(hOut_, &csbi);
    DWORD sz = static_cast<DWORD>(csbi.dwSize.X) * csbi.dwSize.Y, written;
    FillConsoleOutputCharacterW(hOut_, L' ', sz, { 0, 0 }, &written);
    FillConsoleOutputAttribute(hOut_, defaultAttr_, sz, { 0, 0 }, &written);
    moveTo(0, 0);
}

Dashboard::~Dashboard() {
    CONSOLE_CURSOR_INFO ci{ 10, TRUE };
    SetConsoleCursorInfo(hOut_, &ci);
    reset();
}

void Dashboard::moveTo(int x, int y) {
    SetConsoleCursorPosition(hOut_, { static_cast<SHORT>(x), static_cast<SHORT>(y) });
}

void Dashboard::color(C c) {
    SetConsoleTextAttribute(hOut_, static_cast<WORD>(c));
}

void Dashboard::reset() {
    SetConsoleTextAttribute(hOut_, defaultAttr_);
}

void Dashboard::padLine() {
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    GetConsoleScreenBufferInfo(hOut_, &csbi);
    int cur = csbi.dwCursorPosition.X;
    int row = csbi.dwCursorPosition.Y;
    int pad = width_ - cur;
    if (pad > 0) {
        DWORD written;
        COORD pos{ static_cast<SHORT>(cur), static_cast<SHORT>(row) };
        FillConsoleOutputCharacterW(hOut_, L' ', static_cast<DWORD>(pad), pos, &written);
        FillConsoleOutputAttribute(hOut_, defaultAttr_, static_cast<DWORD>(pad), pos, &written);
    }
}

void Dashboard::updateWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (GetConsoleScreenBufferInfo(hOut_, &csbi))
        width_ = std::max(60, std::min(static_cast<int>(csbi.dwSize.X) - 1, 100));
}

void Dashboard::sectionLabel(const wchar_t* label, int row) {
    moveTo(0, row);
    color(C::Cyan);
    wprintf(L" ┌─ ");
    color(C::BWhite);
    wprintf(L"%ls", label);
    padLine();
    reset();
}

void Dashboard::bar(double pct, int w, C fillColor) {
    int filled = std::clamp(static_cast<int>(pct / 100.0 * w), 0, w);
    color(fillColor);
    for (int i = 0; i < filled; ++i)  wprintf(L"█");
    color(C::Gray);
    for (int i = filled; i < w; ++i)  wprintf(L"░");
    reset();
}

void Dashboard::dataRow(int row, const wchar_t* label, const wchar_t* value, C vc) {
    moveTo(0, row);
    color(C::Gray);
    wprintf(L"  %-16ls  ", label);
    color(vc);
    wprintf(L"%ls", value);
    padLine();
    reset();
}

Dashboard::C Dashboard::levelColor(double pct, double warn, double crit) {
    if (pct >= crit) return C::Red;
    if (pct >= warn) return C::Yellow;
    return C::Green;
}

int Dashboard::renderHeader(const SYSTEMTIME& ts, int refreshSec, bool paused, int row) {
    auto hline = [&](int r) {
        moveTo(0, r);
        color(C::Cyan);
        for (int i = 0; i < width_; ++i) wprintf(L"═");
    };

    hline(row);

    moveTo(0, row + 1);
    color(C::Cyan);   wprintf(L"  ");
    color(C::BWhite); wprintf(L"DataMonitor");
    color(C::Cyan);   wprintf(L" │ ");
    color(C::BWhite); wprintf(L"\xb370\xc774\xd130 모\xb2c8\xd130\xb9c1 관\xb9ac\xc790 \xb3c4구  v0.1 PoC");
    padLine();

    moveTo(0, row + 2);
    color(C::Cyan);  wprintf(L"  ");
    color(C::Gray);
    wprintf(L"%04d-%02d-%02d %02d:%02d:%02d",
        ts.wYear, ts.wMonth, ts.wDay,
        ts.wHour, ts.wMinute, ts.wSecond);
    color(C::Cyan);   wprintf(L"  │  ");
    color(C::Yellow); wprintf(L"갱신: %d초", refreshSec);
    if (paused) { color(C::Red); wprintf(L"  [일시정지]"); }
    padLine();

    hline(row + 3);
    reset();
    return row + 5;
}

int Dashboard::renderCpu(const CpuInfo& cpu, int row) {
    sectionLabel(L"CPU", row);
    moveTo(0, row + 1);

    C c = levelColor(cpu.usagePct);
    color(C::Gray);
    wprintf(L"  CPU  (%2d코어)  ", cpu.coreCount);
    bar(cpu.usagePct, 24, c);
    color(c);
    wprintf(L"  %5.1f%%", cpu.usagePct);
    padLine();
    reset();
    return row + 3;
}

int Dashboard::renderMem(const MemInfo& mem, int row) {
    sectionLabel(L"메모리", row);
    moveTo(0, row + 1);

    C c = levelColor(mem.usagePct);
    color(C::Gray);
    wprintf(L"  RAM              ");
    bar(mem.usagePct, 24, c);
    color(c);
    wprintf(L"  %5.1f%%  ", mem.usagePct);
    color(C::Gray);
    wprintf(L"(%llu / %llu MB)", mem.usedMB, mem.totalMB);
    padLine();
    reset();
    return row + 3;
}

int Dashboard::renderDisks(const std::vector<DiskInfo>& disks, int row) {
    sectionLabel(L"디스크", row);
    int r = row + 1;

    for (const auto& d : disks) {
        moveTo(0, r);
        C c = levelColor(d.usagePct);
        color(C::Gray);
        wprintf(L"  %ls%-13ls", d.label.c_str(), L"");
        bar(d.usagePct, 24, c);
        color(c);
        wprintf(L"  %5.1f%%  ", d.usagePct);
        color(C::Gray);
        wprintf(L"(%llu / %llu GB)", d.usedGB, d.totalGB);
        if (d.usagePct >= 85.0) { color(C::Red); wprintf(L"  ⚠ 경고"); }
        padLine();
        reset();
        ++r;
    }
    return r + 1;
}

int Dashboard::renderDataStore(const DataStoreInfo& ds, int row) {
    sectionLabel(L"데이터 스토어 현황", row);

    wchar_t buf[64];

    swprintf_s(buf, L"%llu 건", ds.totalRecords);
    dataRow(row + 1, L"전체 레코드", buf, C::BWhite);

    swprintf_s(buf, L"+%lld rec/s", ds.incomingPerSec);
    dataRow(row + 2, L"수신 (초당)", buf, C::Green);

    swprintf_s(buf, L"%lld rec/s", ds.processedPerSec);
    dataRow(row + 3, L"처리 (초당)", buf, C::Cyan);

    C pendingC = ds.pending > 10000 ? C::Red : ds.pending > 1000 ? C::Yellow : C::Green;
    swprintf_s(buf, L"%lld 건", ds.pending);
    dataRow(row + 4, L"처리 대기", buf, pendingC);

    C errC = ds.errorCount > 50 ? C::Red : ds.errorCount > 10 ? C::Yellow : C::Green;
    swprintf_s(buf, L"%llu 건", ds.errorCount);
    dataRow(row + 5, L"오류 건수", buf, errC);

    dataRow(row + 6, L"마지막 갱신", ds.lastUpdate.c_str(), C::Cyan);

    return row + 8;
}

int Dashboard::renderProcess(int count, int row) {
    sectionLabel(L"프로세스", row);
    moveTo(0, row + 1);
    color(C::Gray);
    wprintf(L"  실행 중         ");
    color(C::BWhite);
    wprintf(L"%d 개", count);
    padLine();
    reset();
    return row + 3;
}

void Dashboard::renderFooter(int row, int refreshSec, bool paused) {
    moveTo(0, row);
    color(C::Cyan);
    for (int i = 0; i < width_; ++i) wprintf(L"─");

    moveTo(0, row + 1);
    color(C::Gray);
    wprintf(L"  [Q] 종료   [P] %ls   [+] 빠르게   [-] 느리게   (현재: %d초)",
        paused ? L"재개    " : L"일시정지",
        refreshSec);
    padLine();
    reset();
}

void Dashboard::render(const Snapshot& snap, int refreshSec, bool paused) {
    updateWidth();
    int row = 0;
    row = renderHeader(snap.timestamp, refreshSec, paused, row);
    row = renderCpu(snap.cpu, row);
    row = renderMem(snap.mem, row);
    row = renderDisks(snap.disks, row);
    row = renderDataStore(snap.dataStore, row);
    row = renderProcess(snap.processCount, row);
    renderFooter(row, refreshSec, paused);
}
