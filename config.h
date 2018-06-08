#ifndef _CPPWEBSERVER_CONFIG_H
#define _CPPWEBSERVER_CONFIG_H

#define ConfigFile "config.ini"

#include <string>

class Config
{
public:
    static Config *GetConfig();

private:
    // Server port, default 5000
    unsigned int m_port;

    // Run mode, default no
    bool m_isDeamon;

    // Path of hypertext path
    std::string m_htDocPath;

private:
    Config();

public:
    // Set port of server
    void SetPort(unsigned int port=5000);
    // Set running mode of server
    void SetDeamon(bool option=false);

friend class WebServer;
};


#endif // _CPPWEBSERVER_CONFIG_H
