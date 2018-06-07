#include "webserver.h"
#include "config.h"
#include "request.h"
#include "response.h"

#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


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
    std::cout << "Listen At " << Listen() << "..." << std::endl;

    while(true)
    {
        auto connection  = new ConnectionInfo;
        int len;

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
        int len;
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
    std::cout << "1\n";
    ConnectionInfo *connection = (ConnectionInfo *)arg;

    Request request(connection->m_connection);
    if(request.m_isValid)
    {
        Response response;
        //response.SendNone(connection->m_connection);
        //response.ServerFile(connection->m_connection, std::string("index.html"));
        response.ServerCGI(connection->m_connection, std::string("cgi_test"));
    }



    close(connection->m_connection);

    delete connection;

    return nullptr;
}