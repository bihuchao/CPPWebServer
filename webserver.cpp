#include "webserver.h"
#include "config.h"
#include "request.h"
#include "response.h"

#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>


class ConnectionInfo
{
public:
    int m_connection;
    sockaddr_in m_address;
};

WebServer::WebServer()
    : m_config(Config::GetConfig())
{
}

int WebServer::run()
{
    if(m_config->m_isDeamon)
    {
        daemon(1, 0);
    }

    std::cout << "Listen At " << Listen() << "..." << std::endl;

    while(true)
    {
        auto connection  = new ConnectionInfo;
        int len = sizeof(sockaddr);

        connection->m_connection = accept(m_serverSock, (sockaddr *)(&(connection->m_address)), (socklen_t *)(&len));

        if(-1 == connection->m_connection)
        {
            perror("accept");
            exit(1);
        }

        DealRequest(connection);
    }

    return 0;
}

unsigned int WebServer::Listen()
{
    unsigned int retPort;
    {
        m_serverSock = socket(PF_INET, SOCK_STREAM, 0);
        if(-1 == m_serverSock)
        {
            perror("socket");
            exit(1);
        }
    }

    {
        int on = 1;
        int ret = setsockopt(m_serverSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if(-1 == ret)
        {
            perror("setsockopt");
            exit(1);
        }
    }

    {
        sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_port = htons(m_config->m_port);
        serverAddr.sin_family = PF_INET;
        serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

        int ret = bind(m_serverSock, (const sockaddr *)(&serverAddr), sizeof(serverAddr));
        if(-1 == ret)
        {
            perror("bind");
            exit(1);
        }
    }

    {
        int ret = listen(m_serverSock, 100);
        if(-1 == ret)
        {
            perror("listen");
            exit(1);
        }
    }

    {
        sockaddr_in tempSockAddr;
        int len = sizeof(tempSockAddr);
        int ret = getsockname(m_serverSock, (sockaddr *)(&tempSockAddr), (socklen_t *)(&len));
        if(-1 == ret)
        {
            perror("getsockname");
            exit(1);
        }
        retPort = ntohs(tempSockAddr.sin_port);
    }

    return retPort;
}

void *WebServer::DealRequest(void *arg)
{
    ConnectionInfo *connection = (ConnectionInfo *)arg;

    Request request(connection->m_connection);

    if(request.m_isValid)
    {

        std::string path = m_config->m_htDocPath + request.m_url;

        struct stat pathSt;
        int ret = stat(path.c_str(), &pathSt);
        // Wrong path
        if(-1 == ret)
        {
            DealNotFound(connection->m_connection);
        }
        // DIR
        else if(S_ISDIR(pathSt.st_mode))
        {
            path += std::string("/index.html");
            ret = stat(path.c_str(), &pathSt);
            if(-1 == ret)
            {
                DealNotFound(connection->m_connection);
            }
            else
            {
                Response().ServerFile(connection->m_connection, path);
            }
        }
        // File with x
        else if(S_IXUSR & pathSt.st_mode)
        {
            Response().ServerCGI(connection->m_connection, path);
        }
        else
        {
            Response().ServerFile(connection->m_connection, path);
        }
    }

    close(connection->m_connection);

    delete connection;

    return nullptr;
}


bool WebServer::DealNotFound(int connection)
{
    std::string path = m_config->m_htDocPath + std::string("/404.html");
    struct stat pathSt;
    int ret = stat(path.c_str(), &pathSt);

    if(-1 == ret)
    {
        Response(ResponseCode_404).SendNone(connection);
    }
    else
    {
        Response(ResponseCode_404).ServerFile(connection, path);
    }

    return true;
}
