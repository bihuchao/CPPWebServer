#include "utilities.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <map>



bool WriteN(int fd, const char *buffer, int n)
{
    int total = 0;
    while(total < n)
    {
        int count = write(fd, buffer+total, n-total);
        total += count;
    }

    return true;
}

bool ReadAll(std::string& str)
{
    char buffer[1024];
    while(true)
    {
        int count = read(STDIN_FILENO, buffer, 1024);
        if(-1 == count)
        {
            perror("read");
            exit(1);
        }
        else if(count)
        {
            buffer[count] = '\0';
            str += buffer;
        }
        else
        {
            break;
        }
    }

    return true;
}

bool GetPostData(std::map<std::string, std::string> &postData)
{
    std::string str;

    ReadAll(str);

    int start = 0;
    int equal = -1;
    for(int i=0; i<=str.size(); i++)
    {
        if(equal == -1 && str[i] == '=')
        {
            equal = i;
        }
        if(str[i] == '&' || i==str.size())
        {
            if(equal == -1 || start==equal)
            {
                std::cout << "GetPostData Error" << std::endl;
                exit(1);
            }
            else
            {
                postData[std::string(str.begin()+start, str.begin()+equal)] = std::string(str.begin()+equal+1, str.begin()+i);
                if(i==str.size())
                {
                    break;
                }
                start = i+1;
                equal = -1;
            }
        }
    }

    return true;
}

bool InsertMessage()
{
    std::map<std::string, std::string> postData;
    GetPostData(postData);
    postData["email"].replace(postData["email"].find(std::string("%40")), 3, "@");

    std::string sql("insert into messageboard (name, email, message, date) values (\"");
    sql += postData["name"];
    sql += "\", \"";
    sql += postData["email"];
    sql += "\", \"";
    sql += postData["message"];
    sql += "\", \"";
    {
        time_t currentTime;
        time(&currentTime);
        auto currentTime2 = localtime(&currentTime);
        char timeStr[30];
        strftime(timeStr, 30, "%Y-%m-%d %H:%M", currentTime2);
        sql += timeStr;
    }
    sql += "\");";

    char *errMsg=nullptr;
    int ret;

    sqlite3 *db = openMessageBoard();

    ret = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if(ret != SQLITE_OK)
    {
        std::cout << "sqlite3_exec : " << errMsg << std::endl;
        exit(1);
    }

    sqlite3_close(db);

    return true;
}

int main()
{
    //

    //std::cout << timeStr << std::endl;

    //exit(1);

    /*
    // Test data
    {
        const char *message = "name=李四&email=dsadas%40qq.com&message=你好，张三";
        int data[2];
        int ret = pipe(data);
        if(-1 == ret)
        {
            perror("pipe");
            exit(1);
        }
        dup2(data[0], STDIN_FILENO);
        WriteN(data[1], message, strlen(message));
        close(data[1]);
    }
     */


    InsertMessage();
    execl("index", "index", nullptr);

    return 0;
}