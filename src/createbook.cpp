#include"createbook.h"
#include"ReadLog.h"
createbook::createbook(){
    setmethodname("createbook");
}

std::string createbook::send(){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    std::string userid;

    if(!js["username"].is_null()){
        userid=js["username"];
    }
    else{
        js_r["success"]=0;
        js_r["message"]="没有登录";
        LOG_ERROR("非法登录");
        response=js_r.dump();
        return response;
    }
    auto mysql=mysqlPool::instance().pop();
    std::string word="select name from user_info where id=";
    word=word+mysql->escape(userid)+";";
    auto res=mysql->find(word);
    if(res==nullptr){
        js_r["success"]=0;
        js_r["message"]="数据库出现问题，无姓名";
        LOG_ERROR("数据库出现问题，无姓名");
        response=js_r.dump();
        return response;
    }
    MYSQL_ROW row=mysql_fetch_row(res.get());
    std::string name=row[0];
    std::string bookname=js["bname"];
    std::string desc=js["desc"];
    word="select bname from book where bname=";
    word=word+"'"+mysql->escape(bookname)+"' and id="+mysql->escape(userid)+";";
    std::cout<<word<<std::endl;
    res=mysql->find(word);
    row=mysql_fetch_row(res.get());
    if(row!=nullptr){
        js_r["success"]=0;
        js_r["message"]="无法创建重复书籍";
        response=js_r.dump();
        return response;
    }
    word="insert into book(bname,another,description,id) value(";
    word=word+"'"+mysql->escape(bookname)+"','"+mysql->escape(name)+"','"+mysql->escape(desc)+"',"+mysql->escape(userid)+");";
    if(mysql->insert(word)){
        js_r["success"]=1;
        int bid=mysql_insert_id(mysql->outinit());
        js_r["bid"]=std::to_string(bid);
        js_r["message"]="创建成功";
        response=js_r.dump();
        return response;
    }
    else{
        js_r["success"]=0;
        js_r["message"]="创建失败";
        response=js_r.dump();
        return response;
    }
}