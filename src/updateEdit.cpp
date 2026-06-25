#include"updateEdit.h"
updateEdit::updateEdit(){
    setmethodname("updateEdit");
}
std::string updateEdit::send(std::string message_){
    json js_r;
    std::string response;
    json js=json::parse(message_);
    auto mysql=mysqlPool::instance().pop();
    std::string id;
    if(js["username"].is_null()){
        js["success"]=0;
        js["message"]="未登录";
        response=js_r.dump();
        return response;
    }
    else{
        id=js["username"];
    }
    std::string bid=js["bid"];
    std::string bname=js["bname"];
    std::string edit=js["edit"];
    std::string word="select id from book where bid=";
    word=word+mysql->escape(bid)+" and bname='"+mysql->escape(bname)+"';";
    auto res=mysql->find(word);
    if(res==nullptr){
        js_r["success"]=0;
        js_r["message"]="无效字段";
        response=js_r.dump();
        return response;
    }
    MYSQL_ROW row=mysql_fetch_row(res.get());
     if(!row||row[0]==NULL){
        js_r["success"]=0;
        js_r["message"]="没有这本书或者不是该书作者";
        response=js_r.dump();
        return response;
    }
    std::string true_id=row[0];

    if(true_id!=id){
        js_r["success"]=0;
        js_r["message"]="你不是该书作者，无法更改";
        response=js_r.dump();
        return response;
    }
    word="update book set description='";
    word=word+mysql->escape(edit)+"' where bid="+mysql->escape(bid)+";";
    if(mysql->update(word)){
        js_r["success"]=1;
        js_r["message"]="修改成功";
        response=js_r.dump();
        return response;
    }
    else{
        js_r["success"]=0;
        js_r["message"]="更新失败";
        response=js_r.dump();
        return response;
    }
}