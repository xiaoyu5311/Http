#include <iostream>
#include <string>
#include <functional>

#include "http/GetMethod.h"
#include "http/HttpClient.h"
#include "native/HttpRequestNative.h"

void my_http()
{
    std::string ip = "127.0.0.1"; // Localhost IP
    int port = 8084;
    HttpClient* http_client = new HttpClient(ip, port);
    std::string post_json = "{\"username\":\"13\",\"password\":\"12333\"}";

    PostMethod* post_method = new PostMethod();
    post_method->set_request_body(post_json);
    post_method->set_relative_path("/store");
    post_method->set_http_callback([](const std::string& response)
    {
        std::cout << "Received POST response: " << response << std::endl;
    });
    http_client->executeMethod(post_method);


    // GetMethod* get_method = new GetMethod();
    // get_method->set_relative_path("/store");
    // get_method->set_http_callback([](const std::string& response)
    // {
    //     std::cout << "Received GET response: " << response << std::endl;
    // });
    // http_client->executeMethod(get_method);


    while (true)
    {
    }
}


void native_http()
{
    std::string ip = "127.0.0.1"; // Localhost IP
    int port = 8084;
    HttpRequestNative http_request(ip, port);

    std::string post_json = "{\"username\":\"13\",\"password\":\"12333\"}";
    std::string post_methodName = "/store"; // Path to the store endpoint

    // http_request.HttpPost(post_url, post_json, post_methodName, [](const std::string& response)
    // {
    //     std::cout << "Received POST response: " << response << std::endl;
    // });
    //
    std::string get_url = "http://localhost:8084";
    std::string get_methodName = "/store"; // Path to the root endpoint

    http_request.HttpGet(get_url, get_methodName, [](const std::string& response)
    {
        std::cout << "Received GET response: " << response << std::endl;
    });

    while (true)
    {
    }
}

int main()
{
    my_http();
    return 0;
}
