#ifndef CPPWEBSERVER_WEBSERVER_H
#define CPPWEBSERVER_WEBSERVER_H

class Config;

class WebServer
{
public:
    WebServer();

    int run();
    unsigned int Listen();

    void *DealRequest(void *arg);

private:
    Config *m_config;

    int m_serverSock;
};


#endif //CPPWEBSERVER_WEBSERVER_H
