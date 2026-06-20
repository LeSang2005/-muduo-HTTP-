#include"booklist.h"
booklist::booklist(){
    setmethodname("booklist");
}
std::string booklist::send(){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    int page=js["page"];
    int size=js["size"];
    page=(page-1)*10;
    auto mysql=mysqlPool::instance().pop();
    std::string word="select count(*) from book";
    auto res=mysql->find(word);
    MYSQL_ROW row=mysql_fetch_row(res.get());
    int total=atoi(std::string(row[0]).c_str());
    js_r["totalPages"]=total;
    word="select * from book limit ";
    word=word+mysql->escape(std::to_string(page))+","+mysql->escape(std::to_string(size))+";";
    res=mysql->find(word);
    js_r["books"]=json::array();
    while(row=mysql_fetch_row(res.get())){
        json book;
        book["id"]=std::string(row[0]);
        book["bname"]=std::string(row[1]);
        book["author"]=std::string(row[2]);
        js_r["books"].push_back(book);
    }
    js_r["success"]=1;
    response=js_r.dump();
    return response;
}