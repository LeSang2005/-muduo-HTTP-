#include "bookreviewlist.h"
#include "ReadLog.h"

BookReviewListBridge::BookReviewListBridge() {
    setmethodname("bookreviewlist");
}

std::string BookReviewListBridge::send(const std::string message) {
    json js = json::parse(message);
    std::string userid = js.value("userid", "");
    int page = js.value("page", 1);
    int size = js.value("size", 10);
    int offset = (page - 1) * size;

    auto mysql = mysqlPool::instance().pop();
    if (!mysql) {
        return R"({"success":0,"message":"数据库繁忙"})";
    }
    std::cout<<"开始"<<std::endl;
    std::string roleSql = "SELECT role FROM user_pos WHERE id=" + mysql->escape(userid)+";";

    auto roleRes = mysql->find(roleSql);
    if (!roleRes) return R"({"success":0,"message":"查询权限失败"})";
    MYSQL_ROW roleRow = mysql_fetch_row(roleRes.get());
    if (!roleRow || std::string(roleRow[0]) == "0") {
        return R"({"success":0,"message":"无审核权限"})";
    }

    std::string countSql = "SELECT COUNT(*) FROM book WHERE status = 0;";
    auto countRes = mysql->find(countSql);
    int total = 0;
    if (countRes) {
        MYSQL_ROW row = mysql_fetch_row(countRes.get());
        if (row) total = std::stoi(row[0]);
    }
    int totalPages = (total + size - 1) / size;


    std::string sql = "SELECT bid, bname, another, description FROM book WHERE status = 0 "
                      "ORDER BY bid LIMIT " + std::to_string(size) + " OFFSET " + std::to_string(offset)+";";
    auto res = mysql->find(sql);
    if (!res) {
        return R"({"success":0,"message":"查询失败"})";
    }

    json resp;
    resp["success"] = 1;
    resp["totalPages"] = totalPages;
    resp["books"] = json::array();

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        json book;
        book["bid"] = row[0] ? std::stoi(row[0]) : 0;
        book["bookname"] = row[1] ? row[1] : "";
        book["author"] = row[2] ? row[2] : "";
        book["description"] = row[3] ? row[3] : "";
        resp["books"].push_back(book);
    }
    return resp.dump();
}