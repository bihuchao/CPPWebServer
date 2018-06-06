#ifndef CPPWEBSERVER_REQUEST_H
#define CPPWEBSERVER_REQUEST_H

#include <string>
#include <map>

typedef enum RequestMethod
{
    RequestMethod_UNDEFINED = 0,
    RequestMethod_GET       = 1,
    RequestMethod_POST      = 2
}RequestMethod;

class Request
{
public:
    Request(int connection);

private:
    bool m_isValid;
    RequestMethod m_method;
    std::string m_url, m_version;
    std::map<std::string, std::string> m_headers;
    char *m_entityData;

private:
    bool ReadRequestLine(int connection);

    bool ReadHeaders(int connection);

    bool ReadEntity(int connection, int length);

friend class WebServer;
};


#endif //CPPWEBSERVER_REQUEST_H
