#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")

using namespace Gdiplus;

// 윈도우 프로시저
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static Bitmap* image = NULL;

    switch (uMsg) {
    case WM_PAINT:
    {
        // 그림 그리기
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Graphics graphics(hdc);

        // 이미지 객체가 있다면 화면에 그리기
        if (image != NULL) {
            graphics.DrawImage(image, 0, 0); // 이미지 그리기
        }

        EndPaint(hwnd, &ps);
    }
    return 0;
    case WM_DESTROY:
        // 이미지 메모리 해제
        if (image != NULL) {
            delete image;
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void displayImage(const wchar_t* filename) {
    // GDI+ 초기화
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 이미지 파일을 로드
    Bitmap* image = Bitmap::FromFile(filename);
    if (image->GetLastStatus() != Ok) {
        printf("Unable to load image: %S\n", filename);
        delete image;
        GdiplusShutdown(gdiplusToken);
        return;
    }

    // 윈도우 클래스 등록
    const wchar_t* className = L"ImageDisplayWindow";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    RegisterClass(&wc);

    // 윈도우 창 생성
    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"Image Viewer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, wc.hInstance, NULL
    );

    // 이미지 객체를 전역 변수로 전달
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)image);

    // 윈도우 창 표시
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    // 메세지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // GDI+ 종료
    GdiplusShutdown(gdiplusToken);
}

int main() {
    // "jcshim.jpg"와 "jcshim.png" 이미지를 출력
    displayImage(L"jcshim.jpg");
    displayImage(L"jcshim.png");

    return 0;
}
