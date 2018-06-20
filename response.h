#ifndef CPPWEBSERVER_RESPONSE_H
#define CPPWEBSERVER_RESPONSE_H

#include "HTTPVersion.h"

#include <map>
#include <string>

typedef enum ResponseCode
{
    ResponseCode_200,
    ResponseCode_404,
    ResponseCode_UNDEFINED
}ResponseCode;

class Response
{
public:
    Response(ResponseCode code=ResponseCode_200);
    ~Response();

    bool SendNone(int connection);

    bool ServerFile(int connection, const std::string &filename);

    bool ServerCGI(int connection, const std::string &cgiFile, char *data);

private:
    ResponseCode m_code;
    HTTPVersion m_version;
    std::map<std::string, std::string> m_headers;

private:
    bool SendResponseLine(int connection);

    bool SendHeaders(int connection);
};

#endif //CPPWEBSERVER_RESPONSE_H
