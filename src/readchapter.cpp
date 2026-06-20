#include"readchapter.h"
readchapter::readchapter(){
    setmethodname("readchapter");
}
std::string readchapter::send(){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    if(js["book_id"].is_null()||js["chapter_id"].is_null()){
        return R"({"success":0,"message":"参数缺失"})";
    }
    std::string bid = js.value("book_id", "");  
    std::string cid = js.value("chapter_id", "");
    
   if (bid.empty() || cid.empty()) {
    return R"({"success":0,"message":"参数缺失"})";
    }
    auto mysql=mysqlPool::instance().pop();
    std::string word="select file_path from chapter where bid=";
    word=word+mysql->escape(bid) +" and cid="+ mysql->escape(cid)+";";
    auto res=mysql->find(word);
    MYSQL_ROW row;
    row=mysql_fetch_row(res.get());
    if (!row) {
    return R"({"success":0,"message":"章节不存在"})";
}
    std::string path=std::string(row[0]);
    std::ifstream f(path);
    if (!std::filesystem::is_regular_file(path)) {//判断是不是普通文件
    js_r["success"] = 0;
    js_r["message"] = "文件无效或不存在";
    response = js_r.dump();
    return response;
}
    if(!f){
        js_r["success"]=0;
        response=js_r.dump();
        return response; 
    }
    std::string content{std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};//一次性读完
    js_r["success"]=1;
    js_r["content"]=content;
    response=js_r.dump();
    return response;
}