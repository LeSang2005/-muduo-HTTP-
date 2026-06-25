#include "reviewlist.h"
reviewlist::reviewlist() {
    setmethodname("reviewlist");
}

std::string reviewlist::send(std::string message_) {
    json js_r;
    json js = json::parse(message_);
    std::string userid = js.value("userid", "");

    auto mysql = mysqlPool::instance().pop();
    if (!mysql) {
        js_r["success"] = 0;
        js_r["message"] = "数据库繁忙";
        return js_r.dump();
    }

    // 权限校验
    std::string roleSql = "select role from user_pos where id=" + mysql->escape(userid) + ";";
    auto roleRes = mysql->find(roleSql);
    MYSQL_ROW roleRow = mysql_fetch_row(roleRes.get());
    if (!roleRow || std::string(roleRow[0]) == "0") {
        js_r["success"] = 0;
        js_r["message"] = "无审核权限";
        return js_r.dump();
    }

    // 查询所有 status=0 的章节
    std::string sql = "SELECT c.bid, c.cid, c.title, c.status, b.bname "
                      "FROM chapter c JOIN book b ON c.bid = b.bid "
                      "WHERE c.status = 0 ORDER BY c.bid, c.cid";
    auto res = mysql->find(sql);
    if (!res) {
        js_r["success"] = 0;
        js_r["message"] = "查询失败";
        return js_r.dump();
    }

    js_r["chapters"] = json::array();
    MYSQL_ROW chRow;
    while ((chRow = mysql_fetch_row(res.get()))) {
        json ch;
        ch["book_id"] = chRow[0];
        ch["chapter_num"] = chRow[1];
        ch["title"] = chRow[2] ? chRow[2] : "";
        ch["book_name"] = chRow[4] ? chRow[4] : "";
        js_r["chapters"].push_back(ch);
    }
    js_r["success"] = 1;
    return js_r.dump();
}