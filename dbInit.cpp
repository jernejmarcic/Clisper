// Initializes the database if not exist
#include <iostream>
#include <sqlite3.h>
#include <filesystem>
#include <string>

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
    exit = sqlite3_open("clipser.db", &DB);
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
