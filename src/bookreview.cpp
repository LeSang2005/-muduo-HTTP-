#include "bookreview.h"
#include "ReadLog.h"

BookReviewBridge::BookReviewBridge() {
    setmethodname("bookreview");
}

std::string BookReviewBridge::send(const std::string message) {
    json js = json::parse(message);
    std::string userid = js.value("userid", "");
    std::string book_id = js.value("book_id", "");
    std::string action = js.value("action", "");  // "approve" 或 "reject"

    if (userid.empty() || book_id.empty() || action.empty()) {
        return R"({"success":0,"message":"参数缺失"})";
    }

    auto mysql = mysqlPool::instance().pop();
    if (!mysql) {
        return R"({"success":0,"message":"数据库繁忙"})";
    }

    // 权限校验
    std::string roleSql = "SELECT role FROM user_pos WHERE id=" + mysql->escape(userid)+";";
    auto roleRes = mysql->find(roleSql);
    if (!roleRes) return R"({"success":0,"message":"查询权限失败"})";
    MYSQL_ROW roleRow = mysql_fetch_row(roleRes.get());
    if (!roleRow || std::string(roleRow[0]) == "0") {
        return R"({"success":0,"message":"无审核权限"})";
    }

    if (action == "approve") {
        // 通过：更新 status 为 1
        std::string sql = "UPDATE book SET status = 1 WHERE bid = " + mysql->escape(book_id)+";";
        if (mysql->update(sql)) {
            return R"({"success":1,"message":"审核通过"})";
        } else {
           // LOG_ERROR("书籍审核通过失败: " + std::string(mysql_error(mysql)));
            return R"({"success":0,"message":"数据库更新失败"})";
        }
    } else if (action == "reject") {

        std::string sql = "DELETE FROM book WHERE bid = " + mysql->escape(book_id)+";";
        if (mysql->update(sql)) {
            return R"({"success":1,"message":"书籍已删除"})";
        } else {
            //LOG_ERROR("书籍删除失败: " + std::string(mysql_error(mysql)));
            return R"({"success":0,"message":"数据库删除失败"})";
        }
    } else {
        return R"({"success":0,"message":"无效的操作"})";
    }
}