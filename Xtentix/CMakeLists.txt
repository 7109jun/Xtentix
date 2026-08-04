cmake_minimum_required(VERSION 3.20)
project(XtentixBrowser CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Xtentix WIN32
    Browser/main.cpp
    Core/Application.cpp
    Core/WindowBase.cpp
    Tabs/Tab.cpp
    Tabs/TabManager.cpp
    Bookmark/BookmarkManager.cpp
    Settings/SettingsManager.cpp
    UI/MainWindow.cpp
    UI/TabStrip.cpp
    UI/AddressBar.cpp
    UI/NewTabPage.cpp
    Rendering/Dom.cpp
    Rendering/Document.cpp
    Rendering/HtmlTokenizer.cpp
    Rendering/HtmlParser.cpp
    Rendering/CssParser.cpp
    Rendering/CssValues.cpp
    Rendering/StyleResolver.cpp
    Rendering/LayoutEngine.cpp
    Rendering/GdiPainter.cpp
    Network/HttpClient.cpp
    Network/RssParser.cpp
    Utils/ResourceImage.cpp
    Utils/Json.cpp
    Resources/app.rc
)

target_include_directories(Xtentix PRIVATE Resources)

target_link_libraries(Xtentix PRIVATE
    user32
    gdi32
    gdiplus
    shlwapi
    winhttp
    comctl32
)

target_compile_definitions(Xtentix PRIVATE UNICODE _UNICODE)
