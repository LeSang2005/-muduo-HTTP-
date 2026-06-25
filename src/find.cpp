#include"find.h"
#include"ReadLog.h"
find::find(){
    setmethodname("find");
}
std::string find::send(std::string message_){
    std::string response;
    json js_r;
    json js=json::parse(message_);
    int id;
    if(!js["username"].is_null()){
        //std::cout<<"进入if\n";
    std::string idStr = js["username"];
    //std::cout<<idStr<<std::endl;
    LOG_ERROR(idStr);
    id = std::atoi(idStr.c_str());
    std::cout<<id<<std::endl;
    }
    else{
    std::cout<<"进入else\n";
    js_r["success"]=0;
    js_r["message"]="账号为空";
    response=js_r.dump();
    return response;
    }
    auto mysql=mysqlPool::instance().pop();
    std::string word;
    word="select * from user_info where id=";
    word=word+mysql->escape(std::to_string(id))+";";
    std::cout<<word<<std::endl;
    auto res=mysql->find(word);
    MYSQL_ROW row;
    
    js_r["success"]=1;
    while(row=mysql_fetch_row(res.get())){
        std::cout<<"成功"<<std::endl;
        js_r["name"]=row[1];
        js_r["sex"]=row[2];
        js_r["intre"]=row[3];
    }

    word="select role from user_pos where id=";
    word=word+mysql->escape(std::to_string(id))+";";
    res=mysql->find(word);
    std::cout<<word<<std::endl;
    while(row=mysql_fetch_row(res.get())){
        std::string role(row[0]);
        if(role=="1"){
            js_r["role"]="manger";
        }
        else if(role=="0"){
            js_r["role"]="reader";
        }
    }

    response=js_r.dump();
    return response;
}