#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <iostream>
#include <sqlite3.h>

using namespace std;

// Callback function for query results
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

int main() {
    sqlite3* db;
    char* errorMessage;
    
    // Open database (creates it if not exists)
    int exit = sqlite3_open("library.db", &db);
    
    if (exit) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return exit;
    } else {
        cout << "Connected to database successfully!" << endl;
    }

    // Create a "Books" table
    string sql = "CREATE TABLE IF NOT EXISTS Books ("
                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "Title TEXT NOT NULL, "
                 "Author TEXT NOT NULL, "
                 "Year INT);";
                 
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
    
    if (exit != SQLITE_OK) {
        cerr << "Error creating table: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << "Books table created successfully!" << endl;
    }

    sqlite3_close(db); // Close database
    return 0;
}
int main() {
    cout << "HELOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO, World!" << endl;
    return 0;
}
