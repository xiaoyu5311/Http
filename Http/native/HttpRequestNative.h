#ifndef HTTP_REQUEST_NATIVE_H
#define HTTP_REQUEST_NATIVE_H

#include <string>
#include <functional>

class HttpRequestNative {
public:
    HttpRequestNative(const std::string& ip, int port)
        : m_ip(ip), m_port(port) {}

    void HttpPost(const std::string& url, const std::string& json, const std::string& methodName,
                  std::function<void(const std::string&)> callback);

    void HttpGet(const std::string& url, const std::string& methodName,
                 std::function<void(const std::string&)> callback);

private:
    void perform_http_post(const std::string& url, const std::string& json, const std::string& methodName,
                           std::function<void(const std::string&)> callback);

    void perform_http_get(const std::string& url, const std::string& methodName,
                          std::function<void(const std::string&)> callback);

    std::string m_ip;
    int m_port;
};

#endif // HTTP_REQUEST_NATIVE_H
