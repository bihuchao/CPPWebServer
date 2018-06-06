#ifndef _CPPWEBSERVER_CONFIG_H
#define _CPPWEBSERVER_CONFIG_H

#define ConfigFile "config.ini"

class Config
{
public:
    static Config *GetConfig();

private:
    // Server port, default 5000
    unsigned int m_port;

    // Run mode, default no
    bool m_isDeamon;

private:
    Config();

public:
    // Set port of server
    void SetPort(unsigned int port=5000);
    // Set running mode of server
    void SetDeamon(bool option=false);
};


#endif // _CPPWEBSERVER_CONFIG_H
