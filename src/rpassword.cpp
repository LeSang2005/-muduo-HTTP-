#include"rpassword.h"
#include"addSave.h"
rpassword::rpassword(){
    setmethodname("rpassword");
}
std::string rpassword::send(std::string message_){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    int id;
    if(!js["username"].is_null()){
    std::string idStr = js["username"];
    std::cout<<idStr<<std::endl;
    id = std::atoi(idStr.c_str());
    std::cout<<id<<std::endl;
    }
    else{
    js_r["success"]=0;
    js_r["message"]="账号为空";
    response=js_r.dump();
    return response;
    }
    auto mysql=mysqlPool::instance().pop();
    std::string password=js["password"];
    std::string salt = "readbook_2026.6.18";   
    std::string hashed = sha256(salt + password); 
    std::string word="update users set password='";
    word=word+mysql->escape(hashed)+"' where id="+mysql->escape(std::to_string(id))+";";
    std::cout<<word<<std::endl;
    
    mysql->update(word);
    js_r["success"]=1;
    js_r["message"]="密码修改成功";
    response=js_r.dump();
    return response;
}