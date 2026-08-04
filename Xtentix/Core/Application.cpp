#include "Application.h"
#include <gdiplus.h>

namespace Xtentix {

int Application::Run(HINSTANCE instance, int cmdShow) {
    // GDI+는 새 탭 페이지의 로고 렌더링(Gdiplus::Bitmap/Graphics)에 쓰이므로
    // 어떤 윈도우보다도 먼저 초기화되어야 한다.
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    int exitCode = -1;
    if (m_mainWindow.Create(instance, cmdShow)) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        exitCode = (int)msg.wParam;
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return exitCode;
}

} // namespace Xtentix
