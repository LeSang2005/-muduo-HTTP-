#pragma once
#include<string>
#include<functional>
#include<iostream>
#include<unordered_map>
#include<muduo/net/TcpConnection.h>
#include"postqueue.h"
class web{
public:
enum ResponseStyle{
file,
text_html,
json,
video,
audio,
text,
END
};
web();
void setmessage(std::string message_){
    message=message_; 
}
void setstyle(ResponseStyle style){
style_=style;
}
void setTcpPtr(muduo::net::TcpConnectionPtr ptr){
    ptr_=ptr;
}
static::std::string setpost(std::string mess);
void run();
static std::string error();
private:
//std::string response_basemodel();
void dealget(std::string source);
void dealpost(std::string source);
void dealoptions(std::string source);

std::string message;
std::unordered_map<std::string,std::function<void(std::string)>>web_function;
ResponseStyle style_;
muduo::net::TcpConnectionPtr ptr_;
std::unordered_map<ResponseStyle,std::string>r_style;
std::unordered_map<std::string,ResponseStyle>s_style;

};