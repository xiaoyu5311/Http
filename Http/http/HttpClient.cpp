#include "HttpClient.h"

#define _WIN32_WINNT 0x0601 // 支持Windows 7或者更高版本的 Windows
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "GetMethod.h"
#pragma comment(lib, "ws2_32.lib")


void HttpClient::executeMethod(HttpMethod* http_method)
{
    std::thread(&HttpClient::perfrom_method, this, http_method).detach();
}

void HttpClient::perfrom_method(HttpMethod* http_method)
{
    PostMethod* post_method = nullptr;
    GetMethod* get_method = nullptr;
    post_method = dynamic_cast<PostMethod*>(http_method);
    get_method = dynamic_cast<GetMethod*>(http_method);
    auto http_callback = http_method->http_callback1();
    std::string ret;
    try
    {
        WSADATA wData;
        if (::WSAStartup(MAKEWORD(2, 2), &wData) != 0)
        {
            http_callback("WSAStartup failed");
            return;
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            WSACleanup();
            http_callback("Socket creation failed");
            return;
        }

        struct sockaddr_in ServerAddr = {0};
        ServerAddr.sin_family = AF_INET;
        inet_pton(AF_INET, m_ip.c_str(), &ServerAddr.sin_addr.s_addr);
        ServerAddr.sin_port = htons(m_port);

        if (connect(clientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            http_callback("Connection failed");
            return;
        }

        DWORD TimeOut = 500;
        if (::setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR ||
            ::setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            http_callback("Setting socket options failed");
            return;
        }

        std::string sendStr;
        if (post_method)
        {
            sendStr += "POST " + post_method->relative_path1() + " HTTP/1.1\r\n";
        }
        else if (get_method)
        {
            sendStr += "GET " + get_method->relative_path1() + " HTTP/1.1\r\n";
        }
        sendStr += "Connection: Keep-Alive\r\n";
        sendStr += "Accept-Encoding: gzip, deflate\r\n";
        sendStr += "Accept-Language: zh-CN,en,*\r\n";
        if (post_method)
        {
            std::string body = post_method->request_body1();
            std::string strLen = std::to_string(body.length());
            sendStr += "Content-Length: " + strLen + "\r\n";
            sendStr += "Content-Type: application/json;charset=UTF-8\r\n";
        }
        sendStr += "Host: " + m_ip + ":" + std::to_string(m_port) + "\r\n";
        // sendStr += "Origin: http://localhost:" + std::to_string(m_port) + "\r\n\r\n";
        sendStr += "Origin: http://" + (m_ip == "127.0.0.1" ? "localhost" : m_ip) + ":" + std::to_string(m_port) +
            "\r\n\r\n";
        if (post_method)
        {
            sendStr +=  post_method->request_body1() + "\r\n";
        }

        int errNo = send(clientSocket, sendStr.c_str(), (int)sendStr.length(), 0);
        if (errNo == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            http_callback("Send failed");
            return;
        }

        char bufRecv[1024];
        std::string response;
        int receivedLength;
        do
        {
            receivedLength = recv(clientSocket, bufRecv, sizeof(bufRecv), 0);
            if (receivedLength > 0)
            {
                response.append(bufRecv, receivedLength);
            }
        }
        while (receivedLength > 0);

        if (response.empty())
        {
            int err = WSAGetLastError();
            ret = "Err: Connect Fail ErrCode->" + std::to_string(err);
        }
        else
        {
            size_t pos = response.find("\r\n\r\n");
            if (pos != std::string::npos)
            {
                ret = response.substr(pos + 4);
            }
            else
            {
                ret = response;
            }
        }

        closesocket(clientSocket);
        WSACleanup();
    }
    catch (...)
    {
        http_callback("Exception occurred");
        return;
    }
    http_callback(ret);
}
