#include "AddStudentWindow.h"
#include "../include/resource.h"
#include "../include/DataManager.h"
#include "../include/Validator.h"
#include "../include/Logger.h"
#include <windows.h>
#include <string>

INT_PTR CALLBACK AddStudentProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    DataManager* pdm = (DataManager*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
    if (message == WM_INITDIALOG) {
        // lParam is DataManager pointer passed by caller in DialogBoxParam
        pdm = (DataManager*)lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pdm);
        return TRUE;
    }
    if (message == WM_COMMAND) {
        if (LOWORD(wParam) == IDC_ADD_SAVE) {
            char roll[128]={0}, name[256]={0}, branch[128]={0}, yearc[16]={0}, cgpac[16]={0};
            GetDlgItemTextA(hDlg, IDC_ADD_ROLL, roll, sizeof(roll));
            GetDlgItemTextA(hDlg, IDC_ADD_NAME, name, sizeof(name));
            GetDlgItemTextA(hDlg, IDC_ADD_BRANCH, branch, sizeof(branch));
            GetDlgItemTextA(hDlg, IDC_ADD_YEAR, yearc, sizeof(yearc));
            GetDlgItemTextA(hDlg, IDC_ADD_CGPA, cgpac, sizeof(cgpac));
            double cgpa=0;
            if (!Validator::validRoll(roll)) {
                MessageBoxA(hDlg, "Invalid roll. Alphanumeric and - allowed.", "Validation", MB_OK|MB_ICONWARNING);
                return TRUE;
            }
            if (!Validator::validCGPA(cgpac, cgpa)) {
                MessageBoxA(hDlg, "Invalid CGPA. Use number 0.0 - 10.0", "Validation", MB_OK|MB_ICONWARNING);
                return TRUE;
            }
            if (pdm && pdm->existsRoll(roll)) {
                std::string msg = "Student with Roll Number " + std::string(roll) + " already exists!";
                MessageBoxA(hDlg, msg.c_str(), "Duplicate Error", MB_OK | MB_ICONERROR);
                return TRUE; // Stop saving
            }
            Student s;
            s.roll = roll;
            s.name = name;
            s.branch = branch;
            s.year = atoi(yearc);
            s.cgpa = cgpa;
            if (pdm && pdm->addStudent(s)) {
                MessageBoxA(hDlg, "Student saved", "OK", MB_OK);
            } else {
                MessageBoxA(hDlg, "Failed to save", "Error", MB_OK|MB_ICONERROR);
            }
            EndDialog(hDlg, 1);
            return TRUE;
        } else if (LOWORD(wParam) == IDC_ADD_CANCEL) {
            EndDialog(hDlg, 0);
            return TRUE;
        }
    }
    return FALSE;
}

