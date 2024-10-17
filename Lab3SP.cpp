#include <windows.h>
#include <tchar.h>
#include <string>

#define IDC_COMBOBOX 101
#define IDC_KEY_INPUT 102
#define IDC_VALUE_INPUT 103
#define IDC_CREATE_BUTTON 104
#define IDC_READ_BUTTON 105
#define IDC_DELETE_BUTTON 106
#define IDC_UPDATE_BUTTON 107  
#define IDC_OUTPUT 108

HINSTANCE hInst;
HWND hComboBox, hKeyInput, hValueInput, hOutput;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HKEY GetRootKey(int selectedIndex) {
    switch (selectedIndex) {
    case 0: return HKEY_CURRENT_USER;
    case 1: return HKEY_LOCAL_MACHINE;
    case 2: return HKEY_CLASSES_ROOT;
    case 3: return HKEY_USERS;
    case 4: return HKEY_CURRENT_CONFIG;
    default: return HKEY_CURRENT_USER;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("RegistryCRUDApp");

    RegisterClass(&wc);

  
    HWND hWnd = CreateWindow(wc.lpszClassName, _T("Registry CRUD Application"),
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 470, 300, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
       
        hComboBox = CreateWindow(TEXT("COMBOBOX"), NULL,
            CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
            10, 10, 180, 200, hWnd, (HMENU)IDC_COMBOBOX, hInst, NULL);
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("HKEY_CURRENT_USER"));
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("HKEY_LOCAL_MACHINE"));
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("HKEY_CLASSES_ROOT"));
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("HKEY_USERS"));
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)TEXT("HKEY_CURRENT_CONFIG"));
        SendMessage(hComboBox, CB_SETCURSEL, 0, 0);

        hKeyInput = CreateWindow(TEXT("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 50, 430, 20, hWnd, (HMENU)IDC_KEY_INPUT, hInst, NULL);

        hValueInput = CreateWindow(TEXT("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 80, 430, 20, hWnd, (HMENU)IDC_VALUE_INPUT, hInst, NULL);

        CreateWindow(TEXT("BUTTON"), TEXT("Create"), WS_CHILD | WS_VISIBLE,
            10, 110, 100, 30, hWnd, (HMENU)IDC_CREATE_BUTTON, hInst, NULL);
        CreateWindow(TEXT("BUTTON"), TEXT("Read"), WS_CHILD | WS_VISIBLE,
            120, 110, 100, 30, hWnd, (HMENU)IDC_READ_BUTTON, hInst, NULL);

        
        CreateWindow(TEXT("BUTTON"), TEXT("Update"), WS_CHILD | WS_VISIBLE,
            230, 110, 100, 30, hWnd, (HMENU)IDC_UPDATE_BUTTON, hInst, NULL); 
        CreateWindow(TEXT("BUTTON"), TEXT("Delete"), WS_CHILD | WS_VISIBLE,
            340, 110, 100, 30, hWnd, (HMENU)IDC_DELETE_BUTTON, hInst, NULL);

        hOutput = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            10, 150, 430, 20, hWnd, (HMENU)IDC_OUTPUT, hInst, NULL);
        SetWindowText(hKeyInput, TEXT("Enter your path"));
        SetWindowText(hValueInput, TEXT("Enter your key name"));
        SetWindowText(hOutput, TEXT("value"));
        break;
    }

    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_CREATE_BUTTON) {
            WCHAR keyPath[256], valueName[256];
            GetWindowText(hKeyInput, keyPath, sizeof(keyPath) / sizeof(WCHAR));
            GetWindowText(hValueInput, valueName, sizeof(valueName) / sizeof(WCHAR));

            HKEY hKey;
            int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            if (RegCreateKeyEx(GetRootKey(selectedIndex), keyPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
                RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)valueName, sizeof(valueName));
                SetWindowText(hOutput, L"Key and value created successfully.");
                RegCloseKey(hKey);
            }
            else {
                SetWindowText(hOutput, L"Failed to create key.");
            }
        }
        else if (LOWORD(wParam) == IDC_READ_BUTTON) {
            WCHAR keyPath[256], valueName[256], value[256];
            GetWindowText(hKeyInput, keyPath, sizeof(keyPath) / sizeof(WCHAR));
            GetWindowText(hValueInput, valueName, sizeof(valueName) / sizeof(WCHAR));

            HKEY hKey;
            DWORD valueLength = sizeof(value);
            int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

            if (RegOpenKeyEx(GetRootKey(selectedIndex), keyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                if (RegQueryValueEx(hKey, valueName, NULL, NULL, (LPBYTE)value, &valueLength) == ERROR_SUCCESS) {
                    SetWindowText(hOutput, value);
                }
                else {
                    SetWindowText(hOutput, L"Failed to read value.");
                }
                RegCloseKey(hKey);
            }
            else {
                SetWindowText(hOutput, L"Failed to open key.");
            }
        }
        else if (LOWORD(wParam) == IDC_DELETE_BUTTON) {
            WCHAR keyPath[256], valueName[256];
            GetWindowText(hKeyInput, keyPath, sizeof(keyPath) / sizeof(WCHAR));
            GetWindowText(hValueInput, valueName, sizeof(valueName) / sizeof(WCHAR));

            HKEY hKey;
            int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

            if (RegOpenKeyEx(GetRootKey(selectedIndex), keyPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
                if (RegDeleteValue(hKey, valueName) == ERROR_SUCCESS) {
                    SetWindowText(hOutput, L"Value deleted successfully.");
                }
                else {
                    SetWindowText(hOutput, L"Failed to delete value.");
                }
                RegCloseKey(hKey);
            }
            else {
                SetWindowText(hOutput, L"Failed to open key.");
            }
        }
        else if (LOWORD(wParam) == IDC_UPDATE_BUTTON) {  
            WCHAR keyPath[256], valueName[256], newValue[256];
            GetWindowText(hKeyInput, keyPath, sizeof(keyPath) / sizeof(WCHAR));
            GetWindowText(hValueInput, valueName, sizeof(valueName) / sizeof(WCHAR));

            GetWindowText(hOutput, newValue, sizeof(newValue) / sizeof(WCHAR));

            HKEY hKey;
            int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

            if (RegOpenKeyEx(GetRootKey(selectedIndex), keyPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
                if (RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)newValue, (lstrlen(newValue) + 1) * sizeof(WCHAR)) == ERROR_SUCCESS) {
                    SetWindowText(hOutput, L"Value updated successfully.");
                }
                else {
                    SetWindowText(hOutput, L"Failed to update value.");
                }
                RegCloseKey(hKey);
            }
            else {
                SetWindowText(hOutput, L"Failed to open key.");
            }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
