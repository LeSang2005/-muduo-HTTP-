#include"rname.h"
rname::rname(){
    setmethodname("rname");
}

std::string rname::send(){
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
    std::string name=js["name"];
    std::string sex=js["sex"];
    std::string intre=js["intre"];
    std::string word="update user_info set name='";
    word=word+mysql->escape(name)+"',sex='"+mysql->escape(sex)+"',intro='"+mysql->escape(intre)+"' where id="+mysql->escape(std::to_string(id))+";";
    std::cout<<word<<std::endl;
    
    mysql->update(word);
    word="update book set another='";
    word=word+mysql->escape(name)+"' where id="+mysql->escape(std::to_string(id))+";";
    std::cout<<word<<std::endl;
    mysql->update(word);
    js_r["success"]=1;
    js_r["message"]="修改成功";
    response=js_r.dump();
    return response;
}