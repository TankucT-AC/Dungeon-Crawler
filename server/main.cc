#include <drogon/drogon.h>
#include "DatabaseManager.h"
#include <cstdlib>
#include <filesystem>
#include <string>
#include <system_error>

#ifdef __linux__
#include <unistd.h>
#endif

namespace {

// Каталог с бинарником: все относительные пути (config.json, cert.pem,
// key.pem, users.db, uploads) резолвятся от него, а не от CWD,
// поэтому сервер можно запускать из любого места.
std::filesystem::path executableDir() {
#ifdef __linux__
    char buf[4096] = {};
    const ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n > 0)
        return std::filesystem::path(std::string(buf, n)).parent_path();
#endif
    return std::filesystem::current_path();
}

} // namespace

int main() {
    std::error_code ec;
    std::filesystem::current_path(executableDir(), ec);
    if (ec) {
        LOG_ERROR << "Cannot chdir to binary dir: " << ec.message();
        return 1;
    }

    try {
        DatabaseManager::instance();
        LOG_INFO << "Database system online.";
    } catch (const std::exception& e) {
        LOG_ERROR << "Database failed to start: " << e.what();
        return 1;
    }

    // Конфиг ищется рядом с бинарником (копируется туда при сборке),
    // переопределение — через DROGON_CONFIG.
    const char* configPath = std::getenv("DROGON_CONFIG");
    drogon::app()
        .loadConfigFile(configPath ? configPath : "./config.json")
        .run();
    return 0;
}