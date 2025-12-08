#include "LoginWindow.h"
#include "../include/resource.h"
#include <windows.h>
#include <fstream>
#include <string>
#include <cstring>
#include <functional> // For std::hash

// Simple hashing function
std::string simple_hash(const std::string& input) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(input));
}

static bool check_credentials(const char* u, const char* p) {
    std::ifstream in("data/users.cfg");
    if (!in.is_open()) {
        // Fallback: Default admin / admin
        return (std::strcmp(u, "admin") == 0 && std::strcmp(p, "admin") == 0);
    }

    std::string line;
    std::string inputHash = simple_hash(std::string(p)); // Hash the input password

    // Format in file expected: username:hashed_password
    // Example for 'admin': admin:1289372193
    while (std::getline(in, line)) {
        size_t delim = line.find(':');
        if (delim == std::string::npos) continue;

        std::string fileUser = line.substr(0, delim);
        std::string filePass = line.substr(delim + 1);

        if (fileUser == u) {
            // Check if file has plain text (legacy) or hash
            // If the file password matches the HASH of input, it's valid
            if (filePass == inputHash) return true;
            // Provide a fallback for plain text during transition (Optional)
            if (filePass == p) return true;
        }
    }
    return false;
}

INT_PTR CALLBACK LoginProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        // Optional: Pre-fill admin for testing
        // SetDlgItemText(hDlg, IDC_EDIT_USER, "admin");
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BTN_LOGIN) {
            char user[128]={0}, pass[128]={0};
            GetDlgItemTextA(hDlg, IDC_EDIT_USER, user, sizeof(user));
            GetDlgItemTextA(hDlg, IDC_EDIT_PASS, pass, sizeof(pass));

            if (check_credentials(user, pass)) {
                EndDialog(hDlg, 1);
            } else {
                MessageBoxA(hDlg, "Login Failed!\nCheck username or password.", "Access Denied", MB_OK | MB_ICONERROR);
            }
            return TRUE;
        }
        break;
    }
    return FALSE;
}

bool ShowLoginDialog(HINSTANCE hInst) {
    INT_PTR res = DialogBoxParamA(hInst, MAKEINTRESOURCEA(IDD_LOGIN), NULL, LoginProc, 0);
    return (res == 1);
}
