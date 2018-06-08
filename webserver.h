#ifndef CPPWEBSERVER_WEBSERVER_H
#define CPPWEBSERVER_WEBSERVER_H

class Config;

class WebServer
{
public:
    WebServer();

    int run();


private:
    Config *m_config;

    int m_serverSock;

private:
    bool DealNotFound(int connection);

    unsigned int Listen();

    void *DealRequest(void *arg);
};


#endif //CPPWEBSERVER_WEBSERVER_H
