#include "AboutDialog.h"
#include "../include/resource.h"
#include <windows.h>

void ShowAboutDialog(HWND parent) {
    DialogBoxParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_ABOUT), parent, [](HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)->INT_PTR {
        if (msg==WM_COMMAND && LOWORD(wp)==IDOK) { EndDialog(hDlg,1); return TRUE; }
        return FALSE;
    }, 0);
}

