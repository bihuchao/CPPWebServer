#include "response.h"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <map>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <wait.h>
#include <fcntl.h>
#include <iostream>

static bool EndsWith(const std::string &str, const std::string &tail)
{
    return !(str.size() <= tail.size() || str.compare(str.size() - tail.size(), tail.size(), tail));
}

bool WriteN(int fd, const char *buffer, int n)
{
    int total = 0;
    while(total < n)
    {
        int count = write(fd, buffer+total, n-total);
        if(count == -1)
        {
            return false;
        }
        total += count;
    }

    return true;
}

/*
* "200" ; Section 10.2.1: OK
* "400" ; Section 10.4.1: Bad Request
* "401" ; Section 10.4.2: Unauthorized
* "403" ; Section 10.4.4: Forbidden
* "404" ; Section 10.4.5: Not Found
* "405" ; Section 10.4.6: Method Not Allowed
* "408" ; Section 10.4.9: Request Time-out
* "501" ; Section 10.5.2: Not Implemented
* "505" ; Section 10.5.6: HTTP Version not supported
*/

class ResponseReasonCode
{
public:
    static const std::string GetReason(ResponseCode code)
    {

        auto temp = ResponseReasonCode::GetObject()->m_reason.find(code);

        return (temp==ResponseReasonCode::GetObject()->m_reason.end())?"":temp->second;
    }

    static const int GetCode(ResponseCode code)
    {

        auto temp = ResponseReasonCode::GetObject()->m_code.find(code);

        return (temp==ResponseReasonCode::GetObject()->m_code.end())?0:temp->second;
    }

private:
    static const ResponseReasonCode *GetObject()
    {
        static ResponseReasonCode s_object;

        return &s_object;
    }

private:
    ResponseReasonCode()
    {
        m_code[ResponseCode_200] = 200;
        m_reason[ResponseCode_200] = std::string("OK");

        m_code[ResponseCode_404] = 404;
        m_reason[ResponseCode_404] = std::string("Not Found");
    }
private:
    std::map<ResponseCode, int> m_code;
    std::map<ResponseCode, std::string> m_reason;
};

Response::Response(ResponseCode code)
    : m_version(HTTPVersion_1_1), m_code(code)
{
    m_headers[std::string("Connection")] = std::string("close");

    {
        time_t currentTime;
        time(&currentTime);
        auto currentTime2 = localtime(&currentTime);
        char timeStr[30];
        strftime(timeStr, 30, "%a, %d %b %G %T GMT", currentTime2);

        m_headers[std::string("Date")] = std::string(timeStr);
    }
    m_headers[std::string("Server")] = std::string("CPPWebServer/0.0.1 (Ubuntu)");
}

Response::~Response()
{
}

bool Response::SendNone(int connection)
{
    if(SendResponseLine(connection))
    {
        if(SendHeaders(connection))
        {
            return true;
        }
    }

    return false;
}

void HandlePipe(int signo)
{
    std::cout << "PIPE??" << std::endl;

    return ;
}

bool Response::ServerFile(int connection, const std::string &filename)
{
    signal(SIGPIPE, SIG_IGN);
    struct stat fileStatus;
    int ret = stat(filename.c_str(), &fileStatus);
    if(-1 == ret || !S_ISREG(fileStatus.st_mode))
    {
        perror("stat");
        exit(1);
    }
    int length=(int)fileStatus.st_size;

    char temp[50];
    sprintf(temp, "%d", length);
    m_headers["CONTENT-LENGTH"] = std::string(temp);


    if(EndsWith(filename, std::string(".html")))
    {
        m_headers["CONTENT-TYPE"] = std::string("text/html");
    }
    else if(EndsWith(filename, std::string(".gif")))
    {
        m_headers["CONTENT-TYPE"] = std::string("image/gif");
    }
    else if(EndsWith(filename, std::string(".jpg")))
    {
        m_headers["CONTENT-TYPE"] = std::string("image/jpeg");
    }
    else if(EndsWith(filename, std::string(".css")))
    {
        m_headers["CONTENT-TYPE"] = std::string("text/css");
    }
    else
    {
        m_headers["CONTENT-TYPE"] = std::string("text/plain");
    }

    if(SendResponseLine(connection))
    {
        if(SendHeaders(connection))
        {
            char ch;
            std::ifstream file(filename, std::ifstream::in);

            while(!file.eof())
            {
                file.read(&ch, 1);
                if(!WriteN(connection, &ch, 1))
                {
                    return false;
                }
            }

            file.close();

            return true;
        }
    }

    return false;
}

void HandlerZombieProcess(int signo)
{
    int status;
    int pid;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        ;
    }

    return ;
}

bool ReadAll(int fd, std::string& str)
{
    char buffer[1024];
    while(true)
    {
        int count = read(fd, buffer, 1024);
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

bool Response::ServerCGI(int connection, const std::string &cgiFile, char *data)
{
    int pipeData[2];
    int pipeData2[2];
    if(-1 == pipe(pipeData) || -1 == pipe(pipeData2))
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if(-1 == pid)
    {
        perror("fork");
        exit(1);
    }
    else if(pid)
    {
        signal(SIGCHLD, HandlerZombieProcess);
        signal(SIGPIPE, SIG_IGN);
        close(pipeData[1]);
        close(pipeData2[0]);

        if(data)
        {
            WriteN(pipeData2[1], data, (int)strlen(data));
        }
        close(pipeData2[1]);

        std::string content;
        ReadAll(pipeData[0], content);

        char temp[50];
        sprintf(temp, "%d", (int)content.size());
        m_headers["CONTENT-LENGTH"] = std::string(temp);
        m_headers["CONTENT-TYPE"] = std::string("text/html");

        if(SendResponseLine(connection))
        {
            if(SendHeaders(connection))
            {
                return WriteN(connection, content.c_str(), content.size());
            }
        }
        close(pipeData[0]);

    }
    else
    {
        close(pipeData[0]);
        close(pipeData2[1]);

        dup2(pipeData[1], STDOUT_FILENO);
        dup2(pipeData2[0], STDIN_FILENO);

        execl(cgiFile.c_str(), cgiFile.c_str(), nullptr);

        close(pipeData[1]);
        close(pipeData2[0]);
    }

    return true;
}

bool Response::SendResponseLine(int connection)
{
    // to do: deal http version according to m_version
    char buffer[1024];
    sprintf(buffer, "HTTP/1.1 %d %s\r\n", ResponseReasonCode::GetCode(m_code), ResponseReasonCode::GetReason(m_code).c_str());
    if(WriteN(connection, buffer, strlen(buffer)))
    {
        return true;
    }

    return false;
}

bool Response::SendHeaders(int connection)
{
    int debug = 10000;

    for(auto it=m_headers.begin(); it!=m_headers.end(); it++)
    {
        char buffer[1024];
        sprintf(buffer, "%s: %s\r\n", it->first.c_str(), it->second.c_str());
        if(!WriteN(connection, buffer, strlen(buffer)))
        {
            return false;
        }
    }

    return WriteN(connection, "\r\n", 2);
}
