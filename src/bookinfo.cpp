#include"bookinfo.h"
bookinfo::bookinfo(){
    setmethodname("bookinfo");
}

std::string bookinfo::send(){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    std::string book_id=js["book_id"];
    auto mysql=mysqlPool::instance().pop();
    std::string word="select * from book where bid=";
    word=word+mysql->escape(book_id)+";";
    auto res=mysql->find(word);
    MYSQL_ROW row;
    std::string bname;
    std::string author;
    std::string description;
    while(row=mysql_fetch_row(res.get())){
       bname = row[1] ? row[1] : "";
       //std::cout<<bname<<std::endl;
       author = row[2] ? row[2] : "";
       description = (row[3] && row[3]) ? row[3] : "";
    }
    json resp;
    resp["success"] = 1;
    json book;
    book["bname"] = bname;
    book["author"] = author;
    book["description"] = description;
    resp["book"] = book;
    response = resp.dump();
    response=js_r.dump();
    return resp.dump();
}