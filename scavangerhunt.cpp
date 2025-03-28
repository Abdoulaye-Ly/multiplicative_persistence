#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

// Global UI elements
HWND hNumberInput, hBaseInput, hNaiveButton, hOptimizedButton, hResultLabel, hStepsDisplay;
HFONT hFont;

// Extract digits in a given base
vector<int> getDigits(long long n, int base) {
    vector<int> digits;
    while (n > 0) {
        digits.push_back(n % base);
        n /= base;
    }
    return digits;
}

// Step-by-step persistence tracking
vector<string> stepsLog;

// Naive method
int multiplicativePersistenceNaive(long long n, int base) {
    stepsLog.clear(); // Clear previous steps
    int steps = 0;
    while (n >= base) {
        vector<int> digits = getDigits(n, base);
        long long product = 1;
        stringstream step;

        step << "Step " << steps + 1 << ": ";
        for (size_t i = 0; i < digits.size(); i++) {
            product *= digits[i];
            step << digits[i] << (i < digits.size() - 1 ? " x " : " = ");
        }
        step << product;
        stepsLog.push_back(step.str());

        n = product;
        steps++;
    }
    return steps;
}

// Optimized method using memoization
unordered_map<long long, int> memo;
int multiplicativePersistenceOptimized(long long n, int base) {
    stepsLog.clear();
    if (memo.count(n)) return memo[n];

    int steps = 0;
    while (n >= base) {
        if (n % base == 0) return memo[n] = steps + 1;
        vector<int> digits = getDigits(n, base);
        long long product = 1;
        stringstream step;

        step << "Step " << steps + 1 << ": ";
        for (size_t i = 0; i < digits.size(); i++) {
            product *= digits[i];
            step << digits[i] << (i < digits.size() - 1 ? " × " : " = ");
        }
        step << product;
        stepsLog.push_back(step.str());

        n = product;
        steps++;
    }
    return memo[n] = steps;
}

// Function to process button clicks
void CalculatePersistence(bool isOptimized) {
    wchar_t numBuffer[20], baseBuffer[10];
    GetWindowTextW(hNumberInput, numBuffer, 20);
    GetWindowTextW(hBaseInput, baseBuffer, 10);

    long long number = _wtoi64(numBuffer);
    int base = _wtoi(baseBuffer);

    if (number <= 0 || base < 2 || base > 16) {
        SetWindowTextW(hResultLabel, L" Invalid input! Check number and base.");
        SetWindowTextW(hStepsDisplay, L"");
        return;
    }

    int result = isOptimized ? multiplicativePersistenceOptimized(number, base)
        : multiplicativePersistenceNaive(number, base);

    wstringstream ws;
    ws << L" Persistence: " << result << L" steps.";
    SetWindowTextW(hResultLabel, ws.str().c_str());

    // Display steps dynamically
    wstringstream stepText;
    for (const string& step : stepsLog) {
        stepText << step.c_str() << L"\r\n";
    }
    SetWindowTextW(hStepsDisplay, stepText.str().c_str());
}

// Window Procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Set background color
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(240, 240, 250)));

        // Create a font for UI elements
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

        // UI Elements
        CreateWindowW(L"STATIC", L"Enter Number:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hNumberInput = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 20, 200, 20, hwnd, NULL, NULL, NULL);
        SendMessage(hNumberInput, WM_SETFONT, (WPARAM)hFont, TRUE);

        CreateWindowW(L"STATIC", L"Enter Base (2-16):", WS_VISIBLE | WS_CHILD, 20, 50, 120, 20, hwnd, NULL, NULL, NULL);
        hBaseInput = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 50, 50, 20, hwnd, NULL, NULL, NULL);
        SendMessage(hBaseInput, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Buttons with colors
        hNaiveButton = CreateWindowW(L"BUTTON", L"Naive Method", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 80, 120, 30, hwnd, (HMENU)1, NULL, NULL);
        hOptimizedButton = CreateWindowW(L"BUTTON", L"Optimized Method", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            160, 80, 140, 30, hwnd, (HMENU)2, NULL, NULL);
        SendMessage(hNaiveButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hOptimizedButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Result Display
        hResultLabel = CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 20, 120, 300, 30, hwnd, NULL, NULL, NULL);
        SendMessage(hResultLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Steps Display
        hStepsDisplay = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
            20, 160, 350, 100, hwnd, NULL, NULL, NULL);
        SendMessage(hStepsDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {  // Naive button clicked
            CalculatePersistence(false);
        }
        else if (LOWORD(wParam) == 2) {  // Optimized button clicked
            CalculatePersistence(true);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// WinMain: Entry point for Windows GUI apps
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"PersistenceCalcWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Scavenger Hunt",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}