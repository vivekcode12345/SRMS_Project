#include "EditStudentWindow.h"
#include "../include/resource.h"
#include "../include/DataManager.h"
#include <windows.h>
#include <string>
#include <cstdio>

// Defines the struct expected from MainWindow
struct EditParams { DataManager* pdm; int id; };

INT_PTR CALLBACK EditStudentProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static DataManager* pdm = nullptr;
    static int editId = -1;

    switch (message) {

    case WM_INITDIALOG: {
        // Retrieve the params passed from MainWindow
        EditParams* params = (EditParams*)lParam;
        if (!params || !params->pdm) {
            MessageBoxA(hDlg, "Error loading edit parameters.", "Error", MB_OK);
            EndDialog(hDlg, 0);
            return TRUE;
        }

        pdm = params->pdm;
        editId = params->id;

        // Load student info using the ID we received
        Student* s = pdm->findById(editId);
        if (!s) {
            MessageBoxA(hDlg, "Student not found in DB.", "Error", MB_OK);
            EndDialog(hDlg, 0);
            return TRUE;
        }

        // Prefill controls
        SetDlgItemTextA(hDlg, IDC_EDIT_ROLL,   s->roll.c_str());
        SetDlgItemTextA(hDlg, IDC_EDIT_NAME,   s->name.c_str());
        SetDlgItemTextA(hDlg, IDC_EDIT_BRANCH, s->branch.c_str());

        char buf[32];
        sprintf(buf, "%d", s->year);
        SetDlgItemTextA(hDlg, IDC_EDIT_YEAR, buf);

        sprintf(buf, "%.2f", s->cgpa);
        SetDlgItemTextA(hDlg, IDC_EDIT_CGPA, buf);

        // Optionally hide the ID box since it is automatic now
        // ShowWindow(GetDlgItem(hDlg, IDC_EDIT_ID), SW_HIDE);

        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_EDIT_SAVE: {
            if (!pdm || editId == -1) {
                EndDialog(hDlg, 0);
                return TRUE;
            }

            Student* s = pdm->findById(editId);
            if (!s) {
                MessageBoxA(hDlg, "Student not found!", "Error", MB_OK);
                EndDialog(hDlg, 0);
                return TRUE;
            }

            char buf[256];
            GetDlgItemTextA(hDlg, IDC_EDIT_ROLL, buf, sizeof(buf));
            s->roll = buf;

            GetDlgItemTextA(hDlg, IDC_EDIT_NAME, buf, sizeof(buf));
            s->name = buf;

            GetDlgItemTextA(hDlg, IDC_EDIT_BRANCH, buf, sizeof(buf));
            s->branch = buf;

            GetDlgItemTextA(hDlg, IDC_EDIT_YEAR, buf, sizeof(buf));
            s->year = atoi(buf);

            GetDlgItemTextA(hDlg, IDC_EDIT_CGPA, buf, sizeof(buf));
            s->cgpa = atof(buf);

            pdm->saveAll();
            MessageBoxA(hDlg, "Student updated successfully.", "Success", MB_OK);
            EndDialog(hDlg, 1);
            return TRUE;
        }

        case IDC_EDIT_CANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
