// gui/MainWindow.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <windowsx.h>
#include <commdlg.h>
#include <string>
#include <sstream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <algorithm>

#include "../include/resource.h"
#include "../include/DataManager.h"
#include "../include/Logger.h"
#include "AddStudentWindow.h"
#include "EditStudentWindow.h"
#include "AboutDialog.h"

// --- 1. THEME COLORS ---
static const COLORREF COL_DIALOG_BG   = RGB(255, 255, 255);

// Header & Row Colors
static const COLORREF COL_HEADER_BG   = RGB(0, 180, 200);
static const COLORREF COL_HEADER_TEXT = RGB(255, 255, 255);
static const COLORREF COL_ROW_A       = RGB(225, 245, 255);
static const COLORREF COL_ROW_B       = RGB(255, 255, 255);

// Button Specific Colors
static const COLORREF COL_BTN_ADD     = RGB(0, 120, 215);   // Blue
static const COLORREF COL_BTN_EDIT    = RGB(40, 167, 69);   // Green
static const COLORREF COL_BTN_DEL     = RGB(220, 53, 69);   // Red
static const COLORREF COL_BTN_IMP     = RGB(253, 126, 20);  // Orange
static const COLORREF COL_BTN_EXP     = RGB(111, 66, 193);  // Purple
static const COLORREF COL_BTN_ABOUT   = RGB(23, 162, 184);  // Teal
static const COLORREF COL_BTN_EXIT    = RGB(52, 58, 64);    // Dark Grey
static const COLORREF COL_BTN_STATS   = RGB(255, 193, 7);   // Gold
static const COLORREF COL_BTN_TEXT    = RGB(255, 255, 255);
static const COLORREF COL_BTN_TEXT_BLK= RGB(0, 0, 0);

static HBRUSH g_hDlgBrush = NULL;
static HBRUSH g_hHeaderBrush = NULL;

// Helper: Safe ANSI ListView cell read
static void GetListViewTextA(HWND hList, int row, int col, char *outBuf, int outBufChars) {
    if (!hList || !outBuf || outBufChars <= 0) return;
    LVITEM li = {}; li.iItem = row; li.iSubItem = col;
    li.cchTextMax = outBufChars; li.pszText = outBuf; li.mask = LVIF_TEXT;
    ListView_GetItem(hList, &li);
}

// Forward declarations
INT_PTR CALLBACK AddStudentProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditStudentProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Helper: Draw Colored Button
void DrawColoredButton(LPDRAWITEMSTRUCT pDIS, COLORREF bg, COLORREF text, const char* label) {
    HDC hdc = pDIS->hDC;
    RECT rect = pDIS->rcItem;
    bool isPressed = (pDIS->itemState & ODS_SELECTED);

    HBRUSH hBrush = CreateSolidBrush(isPressed ? RGB(100, 100, 100) : bg);
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    FrameRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, text);
    DrawTextA(hdc, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (pDIS->itemState & ODS_FOCUS) { InflateRect(&rect, -3, -3); DrawFocusRect(hdc, &rect); }
}

static void RefreshStudentList(HWND hDlg, const std::string& query = "") {
    DataManager* pdm = (DataManager*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
    if (!pdm) return;

    HWND hList = (HWND)GetProp(hDlg, "LV");
    if (!hList) return;

    ListView_DeleteAllItems(hList);
    pdm->loadAll();
    auto rows = pdm->search(query);

    LVITEM item = {};
    char buf[256];
    int idx = 0;
    for (auto &s : rows) {
        item.mask = LVIF_TEXT; item.iItem = idx; item.iSubItem = 0;
        std::snprintf(buf, sizeof(buf), "%d", s.id);
        item.pszText = buf;
        ListView_InsertItem(hList, &item);

        ListView_SetItemText(hList, idx, 1, const_cast<char*>(s.roll.c_str()));
        ListView_SetItemText(hList, idx, 2, const_cast<char*>(s.name.c_str()));
        ListView_SetItemText(hList, idx, 3, const_cast<char*>(s.branch.c_str()));
        std::snprintf(buf, sizeof(buf), "%d", s.year); ListView_SetItemText(hList, idx, 4, buf);
        std::snprintf(buf, sizeof(buf), "%.2f", s.cgpa); ListView_SetItemText(hList, idx, 5, buf);
        ListView_SetItemText(hList, idx, 6, const_cast<char*>(s.active ? "Yes":"No"));
        idx++;
    }
}

INT_PTR CALLBACK MainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    DataManager* pdm = (DataManager*)GetWindowLongPtr(hDlg, GWLP_USERDATA);

    switch (message) {

    case WM_INITDIALOG: {
        g_hDlgBrush = CreateSolidBrush(COL_DIALOG_BG);
        g_hHeaderBrush = CreateSolidBrush(COL_HEADER_BG);

        pdm = (DataManager*)lParam;
        if (pdm) SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pdm);

        INITCOMMONCONTROLSEX icex = {}; icex.dwSize = sizeof(icex);
        icex.dwICC = ICC_LISTVIEW_CLASSES; InitCommonControlsEx(&icex);

        int sx = GetSystemMetrics(SM_CXSCREEN);
        int sy = GetSystemMetrics(SM_CYSCREEN);
        SetWindowPos(hDlg, HWND_TOP, 0, 0, sx, sy, SWP_NOZORDER | SWP_SHOWWINDOW);

        RECT rc; GetClientRect(hDlg, &rc);
        int lx = 10;
        int ly = 90;
        int lw = rc.right - 20;
        int lh = rc.bottom - 110;

        // --- TOP BAR: Search & Exit ---
        CreateWindow("STATIC", "Search:", WS_CHILD | WS_VISIBLE, 10, 12, 60, 25, hDlg, NULL, GetModuleHandle(NULL), NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 70, 10, 300, 25, hDlg, (HMENU)IDC_EDIT_SEARCH, GetModuleHandle(NULL), NULL);
        CreateWindow("BUTTON", "Go", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, 380, 10, 60, 25, hDlg, (HMENU)IDC_BTN_SEARCH, GetModuleHandle(NULL), NULL);

        // Exit Button (Top Right)
        int exitW = 80;
        int exitX = rc.right - exitW - 20;
        CreateWindow("BUTTON", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, exitX, 10, exitW, 25, hDlg, (HMENU)IDC_BTN_EXIT, GetModuleHandle(NULL), NULL);

        // --- LIST VIEW ---
        HWND hList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
            lx, ly, lw, lh, hDlg, (HMENU)IDC_MAIN_LIST, GetModuleHandle(NULL), NULL);

        SetWindowTheme(hList, L"Explorer", NULL);
        HWND hHeader = ListView_GetHeader(hList);
        if (hHeader) SetWindowTheme(hHeader, L"", L"");

        ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

        LVCOLUMN col = {}; col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 60; col.pszText = const_cast<char*>("ID"); ListView_InsertColumn(hList, 0, &col);
        col.cx = 90; col.pszText = const_cast<char*>("Roll"); ListView_InsertColumn(hList, 1, &col);
        col.cx = 200; col.pszText = const_cast<char*>("Name"); ListView_InsertColumn(hList, 2, &col);
        col.cx = 100; col.pszText = const_cast<char*>("Branch"); ListView_InsertColumn(hList, 3, &col);
        col.cx = 60; col.pszText = const_cast<char*>("Year"); ListView_InsertColumn(hList, 4, &col);
        col.cx = 70; col.pszText = const_cast<char*>("CGPA"); ListView_InsertColumn(hList, 5, &col);
        col.cx = 60; col.pszText = const_cast<char*>("Active"); ListView_InsertColumn(hList, 6, &col);

        // --- MAIN BUTTON ROW ---
        // We removed 'Exit' from this list. Added 'About' so it appears in the row.
        int ids[] = { IDC_BTN_ADD, IDC_BTN_EDIT, IDC_BTN_DELETE, IDC_BTN_STATS, IDC_BTN_IMPORT, IDC_BTN_EXPORT, IDC_BTN_ABOUT };
        const int count = sizeof(ids)/sizeof(ids[0]);

        int btnH = 30;
        int gap = 10;
        int left = 10;
        int btnW = 110; // Fixed width buttons look much better

        for (int i=0; i<count; i++) {
            HWND hb = GetDlgItem(hDlg, ids[i]);
            // Dynamically create missing buttons
            if (!hb) {
                const char* label = "Button";
                if (ids[i] == IDC_BTN_STATS) label = "Stats";
                if (ids[i] == IDC_BTN_ABOUT) label = "About"; // Ensure About is created
                hb = CreateWindow("BUTTON", label, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, 0, 0, 0, hDlg, (HMENU)ids[i], GetModuleHandle(NULL), NULL);
            }
            if (!hb) continue;

            int x = left + i * (btnW + gap);
            MoveWindow(hb, x, 50, btnW, btnH, TRUE); // Aligned at y=50
            SetWindowLongPtr(hb, GWL_STYLE, GetWindowLongPtr(hb, GWL_STYLE) | BS_OWNERDRAW);
            ShowWindow(hb, SW_SHOW);
        }

        SetProp(hDlg, "LV", (HANDLE)hList);
        RefreshStudentList(hDlg);
        return TRUE;
    }

    case WM_CTLCOLORDLG: return (INT_PTR)g_hDlgBrush;
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam; SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(0, 0, 0)); return (INT_PTR)g_hDlgBrush;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        if (pDIS->CtlType == ODT_BUTTON) {
            switch (pDIS->CtlID) {
                case IDC_BTN_ADD:    DrawColoredButton(pDIS, COL_BTN_ADD, COL_BTN_TEXT, "Add"); return TRUE;
                case IDC_BTN_EDIT:   DrawColoredButton(pDIS, COL_BTN_EDIT, COL_BTN_TEXT, "Edit"); return TRUE;
                case IDC_BTN_DELETE: DrawColoredButton(pDIS, COL_BTN_DEL, COL_BTN_TEXT, "Delete"); return TRUE;
                case IDC_BTN_IMPORT: DrawColoredButton(pDIS, COL_BTN_IMP, COL_BTN_TEXT, "Import"); return TRUE;
                case IDC_BTN_EXPORT: DrawColoredButton(pDIS, COL_BTN_EXP, COL_BTN_TEXT, "Export"); return TRUE;
                case IDC_BTN_ABOUT:  DrawColoredButton(pDIS, COL_BTN_ABOUT, COL_BTN_TEXT, "About"); return TRUE;
                case IDC_BTN_STATS:  DrawColoredButton(pDIS, COL_BTN_STATS, COL_BTN_TEXT_BLK, "Stats"); return TRUE;
                case IDC_BTN_EXIT:   DrawColoredButton(pDIS, COL_BTN_EXIT, COL_BTN_TEXT, "Exit"); return TRUE;
                case IDC_BTN_SEARCH: DrawColoredButton(pDIS, RGB(100,100,100), COL_BTN_TEXT, "Go"); return TRUE;
                default: DrawColoredButton(pDIS, RGB(180,180,180), COL_BTN_TEXT, "Button"); return TRUE;
            }
        }
        break;
    }

    case WM_NOTIFY: {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (!hdr) break;
        HWND hList = (HWND)GetProp(hDlg, "LV");

        // Header Paint
        if (hdr->hwndFrom == ListView_GetHeader(hList) && hdr->code == NM_CUSTOMDRAW) {
            LPNMCUSTOMDRAW cd = (LPNMCUSTOMDRAW)lParam;
            if (cd->dwDrawStage == CDDS_PREPAINT) return CDRF_NOTIFYITEMDRAW;
            if (cd->dwDrawStage == CDDS_ITEMPREPAINT) {
                FillRect(cd->hdc, &cd->rc, g_hHeaderBrush);
                char text[256] = ""; HDITEMA hdi = { 0 }; hdi.mask = HDI_TEXT; hdi.pszText = text; hdi.cchTextMax = sizeof(text);
                Header_GetItem(hdr->hwndFrom, cd->dwItemSpec, &hdi);
                SetBkMode(cd->hdc, TRANSPARENT); SetTextColor(cd->hdc, COL_HEADER_TEXT);
                RECT rcText = cd->rc; rcText.left += 6; DrawTextA(cd->hdc, text, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                RECT rcSep = cd->rc; rcSep.left = rcSep.right - 1; FillRect(cd->hdc, &rcSep, (HBRUSH)GetStockObject(WHITE_BRUSH));
                return CDRF_SKIPDEFAULT;
            }
        }
        // Row Paint
        if (hdr->idFrom == IDC_MAIN_LIST && hdr->code == NM_CUSTOMDRAW) {
            LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
            if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT) return CDRF_NOTIFYITEMDRAW;
            if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                lplvcd->clrTextBk = (lplvcd->nmcd.dwItemSpec % 2 == 0) ? COL_ROW_A : COL_ROW_B;
                return CDRF_DODEFAULT;
            }
        }
        // Edit on Double Click
        if (hdr->idFrom == IDC_MAIN_LIST && hdr->code == NM_DBLCLK) {
            int idx = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
            if (idx >= 0) {
                char idbuf[64] = {0}; GetListViewTextA(hList, idx, 0, idbuf, sizeof(idbuf));
                int id = atoi(idbuf);
                if (id > 0) {
                    struct EditParams { DataManager* pdm; int id; }; EditParams params{ pdm, id };
                    DialogBoxParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_EDIT), hDlg, EditStudentProc, (LPARAM)&params);
                    RefreshStudentList(hDlg);
                }
            }
            return TRUE;
        }
        // Sort
        if (hdr->idFrom == IDC_MAIN_LIST && hdr->code == LVN_COLUMNCLICK) {
            LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
            if (pdm) {
                static int lastCol = -1; static bool ascending = true;
                if (lastCol == pnmv->iSubItem) ascending = !ascending; else ascending = true;
                lastCol = pnmv->iSubItem;
                pdm->sortStudents(lastCol, ascending);
                char buf[256]={0}; GetDlgItemTextA(hDlg, IDC_EDIT_SEARCH, buf, sizeof(buf));
                RefreshStudentList(hDlg, std::string(buf));
            }
        }
        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_SEARCH: {
            char buf[256]={0}; GetDlgItemTextA(hDlg, IDC_EDIT_SEARCH, buf, sizeof(buf));
            RefreshStudentList(hDlg, std::string(buf)); return TRUE;
        }
        case IDC_BTN_STATS: {
            if (pdm) {
                StudentStats stats = pdm->getStats();
                std::string msg = "Total Students: " + std::to_string(stats.totalCount) + "\n";
                char avgBuf[32]; sprintf(avgBuf, "%.2f", stats.avgCGPA);
                msg += "Average CGPA: " + std::string(avgBuf) + "\n\nTop Performer: " + stats.topStudent;
                MessageBoxA(hDlg, msg.c_str(), "Dashboard Statistics", MB_OK | MB_ICONINFORMATION);
            }
            return TRUE;
        }
        case IDC_BTN_ADD:
            if (pdm) { DialogBoxParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_ADD), hDlg, AddStudentProc, (LPARAM)pdm);
            char buf[256]={0}; GetDlgItemTextA(hDlg, IDC_EDIT_SEARCH, buf, sizeof(buf)); RefreshStudentList(hDlg, std::string(buf)); } return TRUE;
        case IDC_BTN_EDIT: {
            HWND hList = GetDlgItem(hDlg, IDC_MAIN_LIST); int idx = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
            if (idx < 0) { MessageBoxA(hDlg, "Select a row first.", "Info", MB_OK); break; }
            char idbuf[32] = {0}; GetListViewTextA(hList, idx, 0, idbuf, sizeof(idbuf)); int id = atoi(idbuf);
            if (id > 0) { struct EditParams { DataManager* pdm; int id; }; EditParams params{ pdm, id };
            DialogBoxParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_EDIT), hDlg, EditStudentProc, (LPARAM)&params);
            char buf[256]={0}; GetDlgItemTextA(hDlg, IDC_EDIT_SEARCH, buf, sizeof(buf)); RefreshStudentList(hDlg, std::string(buf)); } return TRUE;
        }
        case IDC_BTN_DELETE:
            if (pdm) { HWND hList = GetDlgItem(hDlg, IDC_MAIN_LIST); int idx = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
            if (idx >= 0) { char idbuf[32] = {0}; GetListViewTextA(hList, idx, 0, idbuf, sizeof(idbuf)); int id = atoi(idbuf);
            if (id > 0 && MessageBoxA(hDlg, "Delete selected student?", "Confirm", MB_YESNO) == IDYES) {
                if(pdm->deleteStudent(id)) { pdm->saveAll(); char buf[256]={0}; GetDlgItemTextA(hDlg, IDC_EDIT_SEARCH, buf, sizeof(buf)); RefreshStudentList(hDlg, std::string(buf)); } } }
            else { MessageBoxA(hDlg, "Select a row to delete.", "Info", MB_OK); } } return TRUE;
        case IDC_BTN_IMPORT: {
            OPENFILENAMEA ofn = {}; char fname[MAX_PATH] = {}; ofn.lStructSize = sizeof(ofn); ofn.hwndOwner = hDlg; ofn.lpstrFile = fname; ofn.nMaxFile = MAX_PATH; ofn.lpstrFilter = "CSV Files\0*.csv\0All Files\0*.*\0"; ofn.Flags = OFN_FILEMUSTEXIST;
            if (GetOpenFileNameA(&ofn) && pdm && pdm->importCSV(std::string(fname))) { pdm->saveAll(); RefreshStudentList(hDlg); MessageBoxA(hDlg, "Imported.", "OK", MB_OK); } return TRUE;
        }
        case IDC_BTN_EXPORT: {
            OPENFILENAMEA ofn = {}; char fname[MAX_PATH] = {}; ofn.lStructSize = sizeof(ofn); ofn.hwndOwner = hDlg; ofn.lpstrFile = fname; ofn.nMaxFile = MAX_PATH; ofn.lpstrFilter = "CSV Files\0*.csv\0All Files\0*.*\0"; ofn.Flags = OFN_OVERWRITEPROMPT;
            if (GetSaveFileNameA(&ofn) && pdm && pdm->exportCSV(std::string(fname))) { MessageBoxA(hDlg, "Exported.", "OK", MB_OK); } return TRUE;
        }
        case IDC_BTN_ABOUT: ShowAboutDialog(hDlg); return TRUE;
        case IDC_BTN_EXIT: EndDialog(hDlg, 0); return TRUE;
        }
        break;

    case WM_DESTROY:
        if (g_hDlgBrush) { DeleteObject(g_hDlgBrush); g_hDlgBrush = NULL; }
        if (g_hHeaderBrush) { DeleteObject(g_hHeaderBrush); g_hHeaderBrush = NULL; }
        break;
    }
    return FALSE;
}

void ShowMainDialog(HINSTANCE hInst, DataManager& dm, Logger& logger) {
    dm.loadAll();
    DialogBoxParamA(hInst, MAKEINTRESOURCEA(IDD_MAIN), NULL, MainProc, (LPARAM)&dm);
}
