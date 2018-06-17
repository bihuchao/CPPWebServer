#ifndef CPPWEBSERVER_WEBSERVER_H
#define CPPWEBSERVER_WEBSERVER_H

class Config;
class ThreadPool;

class WebServer
{
public:
    WebServer();
    ~WebServer();
    int run();

private:
    int m_serverSock;
    ThreadPool *m_pool;

private:
    unsigned int Listen();

private:
    static Config *m_config;
    static bool DealNotFound(int connection);
    static void *DealRequest(void *arg);
};


#endif //CPPWEBSERVER_WEBSERVER_H
