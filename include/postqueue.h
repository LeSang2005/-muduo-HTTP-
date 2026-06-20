#pragma once
#include<queue>
#include<mutex>
#include<string>
#include<memory>
#include<iostream>
#include<condition_variable>
#include<muduo/net/TcpConnection.h>
struct post_struct{
    std::string method;
    std::string data;
    muduo::net::TcpConnectionPtr ptr_;
}; 
class web_queue{
public:
post_struct pop(){
    std::unique_lock<std::mutex>lock(q_m);
    while(web_q.empty()){
        
        cond_.wait(lock);
    }
    
    auto message=web_q.front();
    web_q.pop();

    return message;
}
void push(post_struct message){
 std::unique_lock<std::mutex>lock(q_m);
 web_q.push(message);

 cond_.notify_all();
}
bool empty(){
    std::lock_guard<std::mutex>lock(q_m);
    return web_q.empty();
}
static web_queue& instance();
std::condition_variable& outCondition(){
    return cond_;
}
private:
web_queue(){}
std::queue<post_struct>web_q;
std::condition_variable cond_;
std::mutex q_m;
};
inline web_queue& web_queue::instance()
{
    static web_queue q;
    return q;
}