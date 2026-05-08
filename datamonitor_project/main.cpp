#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <algorithm>

#include "Collector.h"
#include "DataStore.h"
#include "Dashboard.h"

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    Collector          collector;
    DataStoreSimulator dataStore;
    Dashboard          dashboard;

    bool running    = true;
    bool paused     = false;
    int  refreshSec = 1;

    while (running) {
        if (!paused) {
            Snapshot snap  = collector.collect();
            snap.dataStore = dataStore.update(refreshSec);
            dashboard.render(snap, refreshSec, paused);
        }

        // refreshSec 동안 100ms 간격으로 키 입력 폴링
        for (int i = 0, ticks = refreshSec * 10; i < ticks && running; ++i) {
            if (_kbhit()) {
                int ch = _getch();
                if (ch == 0 || ch == 0xE0) { _getch(); continue; } // 확장키 스킵
                switch (tolower(ch)) {
                case 'q': running    = false; break;
                case 'p': paused     = !paused; break;
                case '+': refreshSec = std::max(1,  refreshSec - 1); break;
                case '-': refreshSec = std::min(10, refreshSec + 1); break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}
