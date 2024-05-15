#pragma once
#include "PostMethod.h"

class HttpClient
{
public:
    void executeMethod(HttpMethod* HttpMethod);
    void perfrom_method(HttpMethod* http_method);

    HttpClient(): m_ip(), m_port(0)
    {
    }

    HttpClient(std::string& m_ip, int m_port)
        : m_ip(m_ip),
          m_port(m_port)
    {
    }

private:
    std::string m_ip;
    int m_port;

public:
    std::string m_ip1() const
    {
        return m_ip;
    }

    void set_m_ip(const std::string& m_ip)
    {
        this->m_ip = m_ip;
    }

    int m_port1() const
    {
        return m_port;
    }

    void set_m_port(int m_port)
    {
        this->m_port = m_port;
    }

};
