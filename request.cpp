#include "request.h"

#include <iostream>
#include <regex>
#include <algorithm>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


static bool ReadLine(int connection, char *buffer, int len)
{
    int i=0;
    char ch;

    while(1 == read(connection, &ch, 1))
    {
        buffer[i] = ch;

        if(i && buffer[i]=='\n' && buffer[i-1]=='\r')
        {
            buffer[i-1] = '\0';
            return true;
        }
        i++;
    }

    return false;
}

static bool ReadN(int connection, char *buffer, int len)
{
    int total = 0, ncount;
    while(total<len && (ncount = read(connection, buffer+total, len-total)))
    {
        total += ncount;
        if(total==len)
        {
            buffer[total] = '\0';
            return true;
        }
    }

    return false;
}

Request::Request(int connection)
    : m_isValid(true), m_method(RequestMethod_UNDEFINED), m_entityData(nullptr)
{
    if(ReadRequestLine(connection))
    {
        if(ReadHeaders(connection))
        {
            auto temp = m_headers.find("CONTENT-LENGTH");
            if(temp != m_headers.end())
            {
                if(ReadEntity(connection, atoi((temp->second).c_str())))
                {
                    return ;
                }
            }
        }
    }

    m_isValid = false;

    return ;
}

bool Request::ReadRequestLine(int connection)
{
    int debug10 = 100;

    char buffer[1024];

    if(ReadLine(connection, buffer, 1023))
    {
        std::regex pattern("([^ ]+) ([^ ]+) ([^ ]+)");
        std::match_results<char *> result;

        if(std::regex_search(buffer, buffer+strlen(buffer), result, pattern, std::regex_constants::match_continuous))
        {
            if(buffer+strlen(buffer) == result[0].second)
            {
                if(strncasecmp("GET", result[1].first, strlen("GET")) == 0)
                {
                    m_method = RequestMethod_GET;
                }
                else if(strncasecmp("POST", result[1].first, strlen("POST")) == 0)
                {
                    m_method = RequestMethod_POST;
                }
                else
                {
                    // 请求错误
                    m_method = RequestMethod_UNDEFINED;
                    return false;
                }

                m_url = std::string(result[2].first, result[2].second);
                m_version = std::string(result[3].first, result[3].second);

                return true;
            }
        }
    }

    return false;
}

bool Request::ReadHeaders(int connection)
{
    char buffer[1024];
    while(ReadLine(connection, buffer, 1023))
    {
        if(buffer[0] == '\0')
        {
            return true;
        }
        else
        {
            std::regex pattern("([^ ]+): (.+)");
            std::match_results<char *> result;

            if(std::regex_search(buffer, buffer+strlen(buffer), result, pattern, std::regex_constants::match_continuous))
            {
                if(buffer+strlen(buffer) == result[0].second)
                {
                    std::string header(result[1].first, result[1].second);
                    std::transform(header.begin(), header.end(), header.begin(), toupper);

                    m_headers[header] = std::string(result[2].first, result[2].second);

                    continue;
                }
            }
            std::cout <<"STRAT" << buffer << "END" << std::endl;

            return true;
        }
    }

    return false;
}

bool Request::ReadEntity(int connection, int length)
{
    m_entityData = new char[length+1];

    ReadN(connection, m_entityData, length);

    return true;
}
