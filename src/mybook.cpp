#include"mybook.h"
mybook::mybook(){
    setmethodname("mybook");
}

std::string mybook::send(std::string message){
    json js_r;
    std::string response;
    if(message.empty()){
        js_r["success"]=0;
        js_r["message"]="无消息";
        return js_r.dump();
    } 
    json js=json::parse(message);
    std::string user;
    if(js["username"].is_null()){
        js_r["success"]=0;
        js_r["message"]="参数错误";
        return js_r.dump();
    }
    else{
        user=js["username"];
    }
    auto mysql=mysqlPool::instance().pop();
    std::string word="select * from book where id=";
    word=word+mysql->escape(user)+";";
    auto res=mysql->find(word);
    MYSQL_ROW row;
    js_r["books"]=json::array();
    while(row=mysql_fetch_row(res.get())){
        json js_book;
        js_book["bid"]=std::string(row[0]);
        js_book["bname"]=row[1];
        js_book["desc"]=row[3];
        js_r["books"].push_back(js_book);
    }
    js_r["success"]=1;
    return js_r.dump();
}