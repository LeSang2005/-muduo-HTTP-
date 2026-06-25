#include"zhuche.h"
#include"addSave.h"
#include"RateLimiter.h"
zhuche::zhuche(){
    setmethodname("zhuche");
}
std::string zhuche::send(std::string message_){
    std::string ip = getClientIP();  
    if (!RateLimiter::instance().allow(ip, 3, 120)) {  
        return R"({"success":0,"message":"注册太频繁，请稍后再试"})";
    }
    std::string response;
    json js=json::parse(message_);
    std::string password=js["password"];
    std::string salt = "readbook_2026.6.18";   
    std::string hashed = sha256(salt + password); 
    auto mysql=mysqlPool::instance().pop();
    std::string word="insert into users(password) value('";
    word=word+mysql->escape(hashed)+"');";
    if(mysql->insert(word)){
    int id=mysql_insert_id(mysql->outinit());
    std::string name=js["name"];
    std::string intor=js["jieshao"];
    std::string sex=js["sex"];
    word="insert into user_info(id,name,sex,intro) value(";
    word=word+mysql->escape(std::to_string(id))+",'"+mysql->escape(name)+"','"+mysql->escape(sex)+"','"+mysql->escape(intor)+"')";
    if(mysql->insert(word)){

        word="insert into user_pos(id,role) value(";
        word=word+mysql->escape(std::to_string(id))+","+"0);";
        if(!mysql->insert(word)){
            json js_r;
        js_r["success"]=0;
        js_r["message"]="后端出现问题";
        js_r["username"]="空";
        response=js_r.dump();
        return response;
        }
        json js_r;
        js_r["success"]=1;
        js_r["message"]="注册成功";
        js_r["username"]=id;
        response=js_r.dump();
        return response;
    }
    else{
        json js_r;
        js_r["success"]=0;
        js_r["message"]="后端出现问题";
        js_r["username"]="空";
        response=js_r.dump();
        return response;
    }
    }
    else{
        json js_r;
        js_r["success"]=0;
        js_r["message"]="后端出现问题";
        js_r["username"]="空";
        response=js_r.dump();
        return response;
    }
}