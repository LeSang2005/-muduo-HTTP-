#include"findbooks.h"
findbooks::findbooks(){
    setmethodname("findbooks");
}

std::string findbooks::send(std::string message){
    json js_r;
    if(message.empty()){
        js_r["success"]=0;
        js_r["message"]="参数形式错误";
        return js_r.dump();
    }
    json js=json::parse(message);
    if(js["username"].is_null()){
        js_r["success"]=0;
        js_r["message"]="who are you";
        return js_r.dump();
    }
    //std::cout<<"开始"<<std::endl;
    std::string search=js["search"];
    auto mysql=mysqlPool::instance().pop();
    std::string word="select * from book where bname=";
    word=word+"'"+mysql->escape(search)+"'"+" and status=1"+";";
    std::cout<<word<<std::endl;
    auto res=mysql->find(word);
    MYSQL_ROW row;
    js_r["books"]=json::array();
    js_r["success"]=1;
    while ((row = mysql_fetch_row(res.get())) != nullptr)
{
    json item;
    item["id"] = std::string(row[0]);
    item["bname"] = std::string(row[1]);
    item["author"] = std::string(row[2]);
    js_r["books"].push_back(item);
}

return js_r.dump();
}