#pragma once
#include <functional>
#include <string>

#include "HttpMethod.h"

class PostMethod : public HttpMethod
{
private:
    //json类型
    std::string request_body;

public:
    PostMethod(): HttpMethod(), request_body("")
    {
    }

    std::string request_body1() const
    {
        return request_body;
    }

    void set_request_body(const std::string& request_body)
    {
        this->request_body = request_body;
    }
};
