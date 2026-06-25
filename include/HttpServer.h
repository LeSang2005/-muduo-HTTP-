#pragma once
#include<muduo/net/TcpServer.h>
#include<muduo/net/TcpConnection.h>
#include<muduo/net/Callbacks.h>
#include<functional>
#include<muduo/net/Buffer.h>
#include<muduo/net/EventLoop.h>
#include<string>
#include <cerrno>
#include"postqueue.h"
#include"bridge.h"
#include<memory>
#include<iostream>
#include"unordered_map"
#include"ReadLog.h"
class HttpServer{
public:
HttpServer();
~HttpServer()=default;
void run();
void setbridge(std::shared_ptr<bridge>b){
    if(b->isMethodEmpty()){
        LOG_ERROR("方法没有设置");
        exit(1);
    }
    b_.insert({b->outMethName(),b});
}
static std::unordered_map<std::string,muduo::net::TcpConnectionPtr>IdOfConn;
void setThreadsize(int num){threadNum_=num;}
static std::mutex idOfConnMtx_;
private:
std::string error(){
std::string response="HTTP/1.1 404 Not Found\r\n";
                                response+="Content-Type:text/html; charset=utf-8\r\n";
                                response+="Content-Length:13\r\n";
                                response+="\r\n";
                                response+="404 Not Found";
                                return response;
}
void onMessage(const muduo::net::TcpConnectionPtr&,
                            muduo::net::Buffer*,
                            muduo::Timestamp);
void onConnection(const muduo::net::TcpConnectionPtr&);

muduo::net::InetAddress readfile();
std::string ip;
int port;
muduo::net::InetAddress addr_;
muduo::net::EventLoop loop_;
muduo::net::TcpServer tcpserver_;
//web_queue shareWithHttpServer_q;
std::unordered_map<std::string,std::shared_ptr<bridge>>b_;
std::unordered_map<void*, std::string> connBuf_;  // 用裸指针作为key，存储每个连接的累积数据
void handlequeue();
std::mutex mapReadMtx_;
int threadNum_;
};