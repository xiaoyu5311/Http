#pragma once
#include <functional>
#include <string>

class HttpMethod
{
public:
    HttpMethod(): relative_path("")
    {
    }

    virtual ~HttpMethod() = default; // 添加虚析构函数

protected:
    std::function<void(const std::string&)> http_callback;
    std::string relative_path;

public:
    std::function<void(const std::string&)> http_callback1() const
    {
        return http_callback;
    }

    void set_http_callback(const std::function<void(const std::string&)>& http_callback)
    {
        this->http_callback = http_callback;
    }

    std::string relative_path1() const
    {
        return relative_path;
    }

    void set_relative_path(const std::string& relative_path)
    {
        this->relative_path = relative_path;
    }
};
