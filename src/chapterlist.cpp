#include"chapterlist.h"
chapterlist::chapterlist(){
    setmethodname("chapterlist");
}
std::string chapterlist::send(){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    std::string bid=js["book_id"];
    std::string word="select * from chapter where bid=";
    
    auto mysql=mysqlPool::instance().pop();
    word=word+mysql->escape(bid);
    auto res=mysql->find(word);
    MYSQL_ROW row;
    js_r["chapters"]=json::array();
    while(row=mysql_fetch_row(res.get())){
        std::string cid=std::string(row[1]);
        std::string chapter_num=std::string(row[1]);
        std::string title=std::string(row[2]);
        std::string status=std::string(row[4]);
        json chap;
        chap["id"]=cid;
        chap["chapter_num"]=chapter_num;
        chap["title"]=title;
        if(status=="1"){
            chap["status"]=status;
        }
        else{
            continue;
        }
        js_r["chapters"].push_back(chap);
    }
    js_r["success"]=1;
    response=js_r.dump();
    return response;
}