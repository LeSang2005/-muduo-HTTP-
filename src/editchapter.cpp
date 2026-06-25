#include "editchapter.h"
#include"ReadLog.h"
editchapter::editchapter() {
    setmethodname("editchapter");
}

std::string editchapter::send(std::string message_) {
    json js_r;
    json js = json::parse(message_);

    // 1. 安全取值
    std::string username = js.value("username", "");
    std::string book_id  = js.value("book_id", "");
    std::string chapter_num = js.value("chapter_num", "");
    std::string new_title = js.value("title", "");
    std::string new_content = js.value("content", "");

    if (username.empty() || book_id.empty() || chapter_num.empty() || new_content.empty()) {
        js_r["success"] = 0;
        js_r["message"] = "参数缺失";
        return js_r.dump();
    }

    auto mysql = mysqlPool::instance().pop();
    if (!mysql) {
        js_r["success"] = 0;
        js_r["message"] = "数据库繁忙";
        LOG_WARN("数据库繁忙");
        return js_r.dump();
    }

    // 2. 校验书籍是否属于当前用户
    std::string check_sql = "SELECT bid FROM book WHERE id=" + mysql->escape(username)  + " AND bid=" + mysql->escape(book_id) ;
    auto res = mysql->find(check_sql);
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

    // 3. 查询该章节是否存在，并获取原文件路径
    std::string get_sql = "SELECT file_path FROM chapter WHERE bid=" + mysql->escape(book_id)  + " AND cid=" + mysql->escape(chapter_num) ;
    auto res2 = mysql->find(get_sql);
    if (!res2) {
        js_r["success"] = 0;
        js_r["message"] = "查询章节失败";
        return js_r.dump();
    }
    MYSQL_ROW row2 = mysql_fetch_row(res2.get());
    if (!row2 || !row2[0]) {
        js_r["success"] = 0;
        js_r["message"] = "该章节不存在";
        return js_r.dump();
    }
    std::string file_path = row2[0];

    // 4. 覆盖写入文件
    std::ofstream ofs(file_path);
    if (!ofs) {
        js_r["success"] = 0;
        js_r["message"] = "文件写入失败";
        LOG_ERROR("文件写入失败");
        return js_r.dump();
    }
    ofs << new_content;
    ofs.close();

    // 5. 更新数据库中的标题，并将状态重置为待审核（0）
    std::string update_sql = "UPDATE chapter SET title='" + mysql->escape(new_title) + "', status=0 WHERE bid=" + mysql->escape(book_id) + " AND cid=" + mysql->escape(chapter_num);
    if (mysql->update(update_sql)) {
        js_r["success"] = 1;
        js_r["message"] = "修改成功，等待重新审核";
    } else {
        js_r["success"] = 0;
        js_r["message"] = "数据库更新失败";
        LOG_ERROR("数据库更新失败");
    }

    return js_r.dump();
}