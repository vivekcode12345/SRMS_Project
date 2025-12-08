#include <windows.h>
#include "../include/resource.h"
#include "../include/DataManager.h"
#include "../include/Logger.h"
#include "../gui/LoginWindow.h"
#include "../gui/MainWindow.h"

// required by some linkers for resources on Windows
extern "C" IMAGE_DOS_HEADER __ImageBase;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    DataManager dm("data/students.csv");
    Logger logger("data/activity.log");
    // ensure sample data exists
    dm.loadAll();
    bool auth = ShowLoginDialog(hInst);
    if (!auth) return 0;
    ShowMainDialog(hInst, dm, logger);
    return 0;
}

