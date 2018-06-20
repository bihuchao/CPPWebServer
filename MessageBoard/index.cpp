#include "utilities.h"

#include <iostream>
#include <fstream>

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

bool PrintMessage()
{
    char *errMsg=nullptr;
    int ret;

    sqlite3 *db = openMessageBoard();

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

