#pragma once
#include <string>
#include <json.hpp>
#include <memory>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/InetAddress.h>
#include "mysqlpool.h"

class bridge {
public:
    using json = nlohmann::json;
    void setmessage(std::string message) { message_ = message; }
    void setmethodname(std::string name) { methodName = name; }
    bool ismethod(std::string name) { return name == methodName; }
    bool isMethodEmpty() { return methodName.empty(); }
    std::string outMethName() { return methodName; }
    virtual std::string send() { return ""; }

    // 设置连接指针（在 handlequeue 中调用）
    void setConnection(const muduo::net::TcpConnectionPtr& conn) {
        ptr_ = conn;
    }

    // 获取客户端 IP:Port 字符串（例如 "192.168.1.100:54321"）
    std::string getClientIP() {
        if (ptr_) {
            auto addr = ptr_->peerAddress();
            return addr.toIpPort();
        }
        return "unknown";
    }

protected:
    std::string message_;
    std::string methodName;
    muduo::net::TcpConnectionPtr ptr_;
};