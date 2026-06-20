#include "review.h"

review::review() {
    setmethodname("review");
}

std::string review::send() {
    json js_r;
    json js = json::parse(message_);
    std::string userid = js.value("userid", "");
    std::string book_id = js.value("book_id", "");
    std::string chapter_num = js.value("chapter_num", "");
    std::string action = js.value("action", "");

    if (userid.empty() || book_id.empty() || chapter_num.empty() || action.empty()) {
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

   
    std::string roleSql = "select role from user_pos where id=" + mysql->escape(userid) + ";";
    auto roleRes = mysql->find(roleSql);
    MYSQL_ROW roleRow = mysql_fetch_row(roleRes.get());
    if (!roleRow || std::string(roleRow[0]) == "0") {
        js_r["success"] = 0;
        js_r["message"] = "无审核权限";
        return js_r.dump();
    }

    int new_status = (action == "approve") ? 1 : 2;
    std::string sql = "UPDATE chapter SET status=" + mysql->escape(std::to_string(new_status)) +
                      " WHERE bid=" + mysql->escape(book_id) + " AND cid=" + mysql->escape(chapter_num);

    if (mysql->update(sql)) {
        js_r["success"] = 1;
        js_r["message"] = "审核完成";
    } else {
        js_r["success"] = 0;
        js_r["message"] = "数据库更新失败";
    }
    return js_r.dump();
}