#include "config.h"
#include <fstream>
#include <iostream>
#include <regex>

Config * Config::GetConfig()
{
    static Config s_config;

    return &s_config;
}

Config::Config()
    : m_port(5000), m_isDeamon(false)
{
    std::ifstream configFile(ConfigFile, std::ifstream::in);


    while(configFile)
    {
        char line[1024];
        configFile.getline(line, 1024);
        if('\0' == line[0] || '#' == line[0])
        {
            continue;
        }

        std::regex pattern("\\s*([^ ]+)\\s*:\\s*([^ ]+)\\s*");
        std::match_results<char *> result;
        bool isAllMatch = false;

        if(std::regex_search(line, line+strlen(line), result, pattern, std::regex_constants::match_continuous))
        {
            if(result[0].second == line+strlen(line))
            {
                isAllMatch = true;
                if(strncasecmp("port", result[1].first, 4) == 0)
                {
                    std::string portStr(result[2].first, result[2].second);
                    m_port = (unsigned int)atoi(portStr.c_str());
                }
                else if(strncasecmp("isDaemon", result[1].first, 8) == 0)
                {
                    m_isDeamon = strncasecmp("no", result[2].first, 2);
                }
                else
                {
                    std::cout << "Wrong Config item : " << std::string(result[1].first, result[1].second) << std::endl;
                    exit(1);
                }
            }
        }
        if(!isAllMatch)
        {
            std::cout << "Wrong Config line : " << line << std::endl;
            exit(1);
        }
    }

    configFile.close();
}

void Config::SetPort(unsigned int port)
{
    m_port = port;
}

void Config::SetDeamon(bool option)
{
    m_isDeamon = option;
}
