#pragma once
#include"bridge.h"
#include<iostream>
#include"HttpServer.h"
#include<functional>
#include<mutex>
#include"addSave.h"
#include <muduo/net/TcpConnection.h>   
#include <muduo/net/InetAddress.h>  
#include"RateLimiter.h"
class login: public bridge{
public:
login(){
    setmethodname("login");
}
std::string send(std::string message_){
json js=json::parse(message_);

std::string user=js["username"];
std::string password=js["password"];
std::shared_ptr<MySql>login_find=mysqlPool::instance().pop();
if(login_find==nullptr){
    LOG_WARN("数据库句柄为空");
    return  R"({"success":0,"message":"请求太频繁"})";
}
if (!RateLimiter::instance().allow(user, 5, 60)) {
    return R"({"success":0,"message":"请求太频繁"})";
}
std::string word="select password from users where id=";
word=word+login_find->escape(user)+";";
//word=word+user+";";

std::shared_ptr<MYSQL_RES>res=login_find->find(word);
MYSQL_ROW row;
std::string res_password="";
while((row=mysql_fetch_row(res.get()))){
res_password=row[0];
}
json response;

std::string salt = "readbook_2026.6.18";   
std::string hashed = sha256(salt + password); // 同样方式算出“苹果泥”
if(res_password==hashed){
    response["success"]=1;
    users.insert({user,1});
    std::string mess=message_;
    muduo::net::TcpConnectionPtr ptr;
    {
        std::unique_lock<std::mutex>lock_(HttpServer::idOfConnMtx_);
        ptr=HttpServer::IdOfConn[message_];
    }
    // ptr->getLoop()->runInLoop([mess,ptr](){
    //     ptr->setCloseCallback([mess](const muduo::net::TcpConnectionPtr& conn){
            
    //         std::unique_lock<std::mutex>lock(login::login_mutex);
    //         json js_=json::parse(mess);
    //          login::users.erase(js_["username"]);
    //          {
    //             std::unique_lock<std::mutex>lock_(HttpServer::idOfConnMtx_);
    //              HttpServer::IdOfConn.erase(mess);
    //          }
           
    //         conn->shutdown();
    //     });
    // });
}
else{
    response["success"]=0;
}
std::string target=response.dump();
return target;
}
static std::unordered_map<std::string,int>users;
static std::mutex login_mutex;
};