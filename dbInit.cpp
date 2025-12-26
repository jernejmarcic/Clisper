// Initializes the database if not exist
#include <cstdlib>
#include <iostream>
#include <sqlite3.h>
#include <filesystem>
#include <string>

std::string getDatabasePath() {
    const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
    const char* homeEnv = std::getenv("HOME");
    if (!homeEnv) {
        std::cerr << "HOW DO YOU NOT HAVE A HOME DIRECTORY?" << std::endl;
        exit(1);
    }
    std::string home = homeEnv;
    if (!xdgDataHome) {
        // std::cout << "XDG_DATA_HOME is not set." << std::endl;
        xdgDataHome = std::getenv("XDG_DATA_DIRS");
    }
    // std::cout << "XDG_DATA_HOME: " << xdgDataHome << std::endl;
    std::string xdgDataHomeStr = xdgDataHome ? xdgDataHome : "";
    // std::cout << "XDG_DATA_HOME as string: " << xdgDataHomeStr << std::endl;
    if (xdgDataHomeStr.contains(".local/share/")) {
        std::cout << "XDG_DATA_HOME contains .local/share/" << std::endl;
        return home+"/.local/share/clisper/clisper.db";
    }
    return home+"/.local/share/clisper/clisper.db";
};


int main(int argc, char** argv) {
    sqlite3* DB;
    int exit = 0;

    // std::string sql = "CREATE TABLE IF NOT EXISTS CLISPER("
    //                       "ID INT PRIMARY KEY     NOT NULL, "
    //                       "TITLE           CHAR(255)     NULL, "
    //                       "ENTRY           TEXT    NULL, "
    //                       "KIND            ENUM('text','image','mixed')   NOT NULL     DEFAULT 'text', "
    //                       "LANGUAGE        CHAR(16)     NULL, "
    //                       "INSERTIME       INT     NOT NULL, "
    //                       "ADDRESS        CHAR(50), "
    //                       "SALARY         REAL );";

    std::string sql = "CREATE TABLE IF NOT EXISTS clisper ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        // "kind TEXT NOT NULL DEFAULT 'text' CHECK(kind IN ('text','image','mixed')),"
        "title TEXT NULL,"
        // "source_app TEXT,"
        "language TEXT NULL,"
        "mimeType TEXT NULL,"
        "entry BLOB NOT NULL UNIQUE,"
        "createdAt INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
        // "imageBlob BLOB,"
        // "image_thumbnail BLOB,"
        // "ocr_text TEXT,"
        "imageMetadata TEXT NULL,"
        "updatedAt INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
        "accessedAt INTEGER"
        ");";

     	// std::basic_string<char> dbPath = "clipser.db";
    // std::cout << path << std::endl;
    std::string dbDir = getDatabasePath();
    std::filesystem::create_directories(std::filesystem::path(dbDir).parent_path());
    std::cout << dbDir << std::endl;
    exit = sqlite3_open(dbDir.c_str(), &DB);
    char* messaggeError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        return (-1);
    }
    else
        std::cout << "Opened Database Successfully!" << std::endl;
    sqlite3_close(DB);
    return (0);
}
