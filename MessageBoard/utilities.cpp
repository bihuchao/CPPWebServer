#include "utilities.h"

#include <fstream>
#include <iostream>

#include <sys/stat.h>

bool PrintFile(const char *filename)
{
    std::fstream myfile(filename,std::ios::in|std::ios::out);
    std::string line;
    if(myfile.fail())
    {
        std::cout << "error opening file : " << filename << std::endl;
        return false;
    }
    while(getline(myfile, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

sqlite3 *openMessageBoard()
{
    sqlite3 *db = nullptr;

    char *errMsg;
    int ret;

    struct stat info;
    int exist = stat("messageboard.db", &info);
    if(-1 == exist)
    {
        std::cout << "new database" << std::endl;
        ret = sqlite3_open_v2("messageboard.db", &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr);
        if(ret != SQLITE_OK)
        {
            std::cout << "sqlite3_open_v2-ceate"<< std::endl;
            exit(1);
        }

        const char *sql = "create table messageboard(id integer primary key autoincrement, name text not null, email text not null, message text not null, date text not null);";

        ret = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if(ret != SQLITE_OK)
        {
            std::cout << "sqlite3_exec" << errMsg << std::endl;
            exit(1);
        }

    }
    else
    {
        ret = sqlite3_open_v2("messageboard.db", &db, SQLITE_OPEN_READWRITE, nullptr);
        if(ret != SQLITE_OK)
        {
            std::cout << "sqlite3_open_v2-open" << std::endl;
            exit(1);
        }
    }

    return db;
}
