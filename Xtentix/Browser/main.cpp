#include "../Core/Application.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    Xtentix::Application app;
    return app.Run(hInstance, nCmdShow);
}
