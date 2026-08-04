#include "BookmarkManager.h"
#include "../Utils/Json.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace Xtentix {

std::string BookmarkManager::DefaultFilePath() {
    const char* appdata = std::getenv("APPDATA");
    std::filesystem::path base = appdata ? std::filesystem::path(appdata) : std::filesystem::current_path();
    std::filesystem::path dir = base / "Xtentix";

    std::error_code ec;
    std::filesystem::create_directories(dir, ec); // 실패해도 조용히 진행 (Save 시점에 다시 시도됨)

    return (dir / "bookmarks.json").string();
}

void BookmarkManager::Load() {
    m_entries.clear();

    std::ifstream file(DefaultFilePath(), std::ios::binary);
    if (!file) return; // 첫 실행이라 파일이 없는 게 정상 - 조용히 빈 목록으로 시작

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    JsonValue root = JsonValue::Parse(content);
    if (!root.IsArray()) return; // 손상된 파일이어도 앱이 죽지 않고 빈 목록으로 시작

    for (auto& item : root.AsArray()) {
        const JsonValue* title = item.Find("title");
        const JsonValue* url = item.Find("url");
        if (url && !url->AsString().empty()) {
            m_entries.push_back({ title ? title->AsString() : "", url->AsString() });
        }
    }
}

void BookmarkManager::Save() const {
    JsonArray arr;
    for (auto& e : m_entries) {
        JsonObject obj;
        obj.emplace_back("title", JsonValue(e.title));
        obj.emplace_back("url", JsonValue(e.url));
        arr.push_back(JsonValue(std::move(obj)));
    }

    std::ofstream file(DefaultFilePath(), std::ios::binary | std::ios::trunc);
    if (!file) return; // 디스크 저장 실패해도 브라우저 동작 자체는 계속되게 조용히 무시
    std::string json = JsonValue(std::move(arr)).Stringify(2);
    file.write(json.data(), (std::streamsize)json.size());
}

void BookmarkManager::Add(const std::string& title, const std::string& url) {
    if (Contains(url)) return;
    m_entries.push_back({ title, url });
    Save();
}

void BookmarkManager::Remove(const std::string& url) {
    m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(),
        [&](const BookmarkEntry& e) { return e.url == url; }), m_entries.end());
    Save();
}

bool BookmarkManager::Contains(const std::string& url) const {
    return std::any_of(m_entries.begin(), m_entries.end(),
        [&](const BookmarkEntry& e) { return e.url == url; });
}

} // namespace Xtentix
