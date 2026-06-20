#include "createchapter.h"
#include <filesystem>
#include <fstream>
#include"ReadLog.h"
createchapter::createchapter() {
    setmethodname("uploadchapter");
}

std::string createchapter::send() {
    json js_r;
    json js = json::parse(message_);

  
    std::string username = js.value("username", "");
    std::string book_id  = js.value("book_id", "");
    std::string chapter_num = js.value("chapter_num", "");
    std::string title    = js.value("title", "无标题");
    std::string content  = js.value("content", "");

    if (username.empty() || book_id.empty() || chapter_num.empty() || content.empty()) {
        js_r["success"] = 0;
        js_r["message"] = "参数缺失";
        return js_r.dump();
    }

    auto mysql = mysqlPool::instance().pop();
    if (!mysql) {
        js_r["success"] = 0;
        js_r["message"] = "数据库繁忙";
        return js_r.dump();
    }


    std::string sql = "SELECT bid FROM book WHERE id=" + mysql->escape(username)  + " AND bid=" +mysql->escape(book_id) ;
    auto res = mysql->find(sql);
    if (!res) {
        js_r["success"] = 0;
        js_r["message"] = "查询书籍失败";
        return js_r.dump();
    }
    MYSQL_ROW row = mysql_fetch_row(res.get());
    if (!row || !row[0]) {
        js_r["success"] = 0;
        js_r["message"] = "你没有该书的创作权";
        return js_r.dump();
    }

 
    sql = "SELECT cid FROM chapter WHERE bid=" + mysql->escape(book_id) + " AND cid=" + mysql->escape(chapter_num);
    auto res2 = mysql->find(sql);
    if (res2) {
        MYSQL_ROW row2 = mysql_fetch_row(res2.get());
        if (row2 && row2[0]) {
            js_r["success"] = 0;
            js_r["message"] = "该章节号已存在，请更换章节号";
            return js_r.dump();
        }
    }


    std::string dir = "../static/book/" + book_id;
    std::filesystem::create_directories(dir);


    std::string file_path = dir + "/" + chapter_num + ".txt";
    std::ofstream ofs(file_path);
    if (!ofs) {
        js_r["success"] = 0;
        js_r["message"] = "文件创建失败";
        LOG_ERROR(file_path+"文件创建失败");
        return js_r.dump();
    }
    ofs << content;
    ofs.close();


    std::string absolute_path = std::filesystem::absolute(file_path).string();

  
    sql = "INSERT INTO chapter (bid, cid, title, file_path, status) VALUES ("
          +mysql->escape(book_id) + "," +mysql->escape(chapter_num) + ",'" +mysql->escape(title) + "','" + mysql->escape(absolute_path) + "', 0)";

    if (mysql->insert(sql)) {
        js_r["success"] = 1;
        js_r["message"] = "上传成功，等待审核";
    } else {
        js_r["success"] = 0;
        js_r["message"] = "数据库插入失败";
    }

    return js_r.dump();
}