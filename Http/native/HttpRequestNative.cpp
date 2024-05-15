#include "HttpRequestNative.h"

#define _WIN32_WINNT 0x0601 // 支持Windows 7或者更高版本的 Windows
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HttpRequestNative::HttpPost(const std::string& url, const std::string& json, const std::string& methodName,
                                 std::function<void(const std::string&)> callback)
{
    std::thread(&HttpRequestNative::perform_http_post, this, url, json, methodName, callback).detach();
}

void HttpRequestNative::HttpGet(const std::string& url, const std::string& methodName,
                                std::function<void(const std::string&)> callback)
{
    std::thread(&HttpRequestNative::perform_http_get, this, url, methodName, callback).detach();
}

void HttpRequestNative::perform_http_post(const std::string& url, const std::string& json, const std::string& methodName,
                                          std::function<void(const std::string&)> callback)
{
    std::string ret;
    try
    {
        WSADATA wData;
        if (::WSAStartup(MAKEWORD(2, 2), &wData) != 0)
        {
            callback("WSAStartup failed");
            return;
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            WSACleanup();
            callback("Socket creation failed");
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
            callback("Connection failed");
            return;
        }

        DWORD TimeOut = 500;
        if (::setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR ||
            ::setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            callback("Setting socket options failed");
            return;
        }

        std::string strLen = std::to_string(json.length());
        std::string sendStr;
        sendStr += "POST " + methodName + " HTTP/1.1\r\n";
        sendStr += "Connection: Keep-Alive\r\n";
        sendStr += "Accept-Encoding: gzip, deflate\r\n";
        sendStr += "Accept-Language: zh-CN,en,*\r\n";
        sendStr += "Content-Length: " + strLen + "\r\n";
        sendStr += "Content-Type: application/json;charset=UTF-8\r\n";
        sendStr += "Host: " + m_ip + ":" + std::to_string(m_port) + "\r\n";
        sendStr += "Origin: " + url + "\r\n";
        sendStr += "\r\n" + json + "\r\n";

        int errNo = send(clientSocket, sendStr.c_str(), (int)sendStr.length(), 0);
        if (errNo == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            callback("Send failed");
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
        callback("Exception occurred");
        return;
    }
    callback(ret);
}

void HttpRequestNative::perform_http_get(const std::string& url, const std::string& methodName,
                                         std::function<void(const std::string&)> callback)
{
    std::string ret;
    try
    {
        WSADATA wData;
        if (::WSAStartup(MAKEWORD(2, 2), &wData) != 0)
        {
            callback("WSAStartup failed");
            return;
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            WSACleanup();
            callback("Socket creation failed");
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
            callback("Connection failed");
            return;
        }

        DWORD TimeOut = 500;
        if (::setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR ||
            ::setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            callback("Setting socket options failed");
            return;
        }

        std::string sendStr;
        sendStr += "GET " + methodName + " HTTP/1.1\r\n";
        sendStr += "Connection: Keep-Alive\r\n";
        sendStr += "Accept-Encoding: gzip, deflate\r\n";
        sendStr += "Accept-Language: zh-CN,en,*\r\n";
        sendStr += "Host: " + m_ip + ":" + std::to_string(m_port) + "\r\n";
        sendStr += "Origin: " + url + "\r\n";
        sendStr += "\r\n";

        int errNo = send(clientSocket, sendStr.c_str(), (int)sendStr.length(), 0);
        if (errNo == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            callback("Send failed");
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
        callback("Exception occurred");
        return;
    }
    callback(ret);
}
