#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
void SetHook();
void ReleaseHook();
void PrintClicks(bool forcePrint = false);

HHOOK hMouseHook;
int leftButtonClicks = 0;
int rightButtonClicks = 0;
const int millionClicks = 1000000;

int main() {
    SetHook();

    std::cout << "Click counter is running. Press 'End' to exit.\n";

    MSG msg;
    auto lastPrintTime = std::chrono::steady_clock::now();

    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }

        // Check for messages and dispatch them
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto now = std::chrono::steady_clock::now();
        if (now - lastPrintTime >= std::chrono::milliseconds(100)) {
            PrintClicks();
            lastPrintTime = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ReleaseHook();

    std::cout << "Final Counts:\n";
    PrintClicks(true);

    return 0;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_LBUTTONDOWN) {
            leftButtonClicks++;
        }
        else if (wParam == WM_RBUTTONDOWN) {
            rightButtonClicks++;
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void SetHook() {
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (hMouseHook == NULL) {
        std::cerr << "Mouse hook failed!\n";
        exit(1);
    }
}

void ReleaseHook() {
    UnhookWindowsHookEx(hMouseHook);
}

void PrintClicks(bool forcePrint) {
    static int lastLeftClicks = 0;
    static int lastRightClicks = 0;

    if (forcePrint || lastLeftClicks != leftButtonClicks || lastRightClicks != rightButtonClicks) {
        system("cls");
        std::cout << "LB: " << leftButtonClicks << "  RB: " << rightButtonClicks << "\n";

        int totalClicks = leftButtonClicks + rightButtonClicks;
        if (totalClicks >= millionClicks) {
            std::cout << "Damn bro, " << totalClicks << " clicks, go outside!\n";
        }

        lastLeftClicks = leftButtonClicks;
        lastRightClicks = rightButtonClicks;
    }
}
