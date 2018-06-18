#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sqlite3.h>
#include <sys/stat.h>

int PrintItem(void *data, int count, char **value, char **key)
{
    std::cout << "<tr>"
              << "<td>" << value[4] << "</td>"
              << "<td>" << value[3] << "</td>"
              << "<td>" << value[1] << "</td>"
              << "<td>" << value[2] << "</td>"
              << "</tr>"
              << std::endl;

    return 0;
}

bool PrintFile(const char *filename)
{
    std::fstream myfile(filename,std::ios::in|std::ios::out);
    std::string line;
    if(myfile.fail())
    {
        std::cout << "error opening file!" << std::endl;
        return false;
    }
    while(getline(myfile, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

bool PrintMessage()
{
    char *errMsg=nullptr;

    sqlite3 *db = nullptr;
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

        const char *sql = "create table messageboard(id int primary key not null, name text not null, email text not null, message text not null, date text not null);";

        ret = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if(ret != SQLITE_OK)
        {
            std::cout << "sqlite3_exec" << std::endl;
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

    /*
    ret = sqlite3_exec(db, "insert into messageboard (id, name, email, message, date) values (6, \"zhangsan\", \"zhangsan@qq.com\", \"HelloWorld\", \"2018-09-09\");", nullptr, nullptr, &errMsg);
    if(ret != SQLITE_OK)
    {
        std::cout << "sqlite3_exec : " << errMsg << std::endl;
        exit(1);
    }
     */

    ret = sqlite3_exec(db, "select * from messageboard;", PrintItem, nullptr, &errMsg);
    if(ret != SQLITE_OK)
    {
        std::cout << errMsg << std::endl;
        exit(1);
    }

    sqlite3_close(db);

    return true;
}

int main()
{
    PrintFile("data1.html");

    PrintMessage();

    PrintFile("data2.html");

    return 0;
}

