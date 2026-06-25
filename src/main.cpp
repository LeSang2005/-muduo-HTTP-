#include "HttpServer.h"

// 用户相关
#include "zhuche.h"       // 注册
#include "login.h"        // 登录
#include "find.h"         // 查询用户信息
#include "rname.h"        // 修改姓名
#include "rpassword.h"    // 修改密码

// 书籍相关
#include "booklist.h"     // 书城列表
#include "bookinfo.h"     // 书籍详情
#include "createbook.h"   // 创建书籍
#include "updateEdit.h"   // 修改简介
#include "mybook.h"       // 个人创作书籍
#include "findbooks.h"    // 查找书籍

// 章节相关
#include "chapterlist.h"  // 章节列表
#include "readchapter.h"  // 读取章节
#include "createchapter.h"// 上传章节
#include "editchapter.h"  // 修改章节

// 审核相关
#include "reviewlist.h"   // 章节审核列表
#include "review.h"       // 章节审核操作
#include "bookreviewlist.h" // 书籍审核列表（新增）
#include "bookreview.h"   // 书籍审核操作（新增）

int main() {
    HttpServer server_;

    // ========== 用户模块 ==========
    auto loginBridge          = std::make_shared<login>();
    auto zhucheBridge         = std::make_shared<zhuche>();
    auto findBridge           = std::make_shared<find>();
    auto rnameBridge          = std::make_shared<rname>();
    auto rpasswordBridge      = std::make_shared<rpassword>();

    // ========== 书籍模块 ==========
    auto booklistBridge       = std::make_shared<booklist>();
    auto bookinfoBridge       = std::make_shared<bookinfo>();
    auto createbookBridge     = std::make_shared<createbook>();
    auto updateEditBridge     = std::make_shared<updateEdit>();
    auto mybookBridge         = std::make_shared<mybook>();
    auto findbooksBridge      = std::make_shared<findbooks>();

    // ========== 章节模块 ==========
    auto chapterlistBridge    = std::make_shared<chapterlist>();
    auto readchapterBridge    = std::make_shared<readchapter>();
    auto createchapterBridge  = std::make_shared<createchapter>();
    auto editchapterBridge    = std::make_shared<editchapter>();

    // ========== 审核模块 ==========
    // 章节审核
    auto reviewlistBridge     = std::make_shared<reviewlist>();   // 方法名 "reviewlist"
    auto reviewBridge         = std::make_shared<review>();       // 方法名 "review"
    // 书籍审核（独立于章节审核）
    auto bookReviewListBridge = std::make_shared<BookReviewListBridge>(); // 方法名 "bookreviewlist"
    auto bookReviewBridge     = std::make_shared<BookReviewBridge>();     // 方法名 "bookreview"

    // ========== 注册所有 Bridge ==========
    // 用户
    server_.setbridge(loginBridge);
    server_.setbridge(zhucheBridge);
    server_.setbridge(findBridge);
    server_.setbridge(rnameBridge);
    server_.setbridge(rpasswordBridge);

    // 书籍
    server_.setbridge(booklistBridge);
    server_.setbridge(bookinfoBridge);
    server_.setbridge(createbookBridge);
    server_.setbridge(updateEditBridge);
    server_.setbridge(mybookBridge);
    server_.setbridge(findbooksBridge);

    // 章节
    server_.setbridge(chapterlistBridge);
    server_.setbridge(readchapterBridge);
    server_.setbridge(createchapterBridge);
    server_.setbridge(editchapterBridge);

    // 审核
    server_.setbridge(reviewlistBridge);      // 章节审核列表
    server_.setbridge(reviewBridge);          // 章节审核操作
    server_.setbridge(bookReviewListBridge);  // 书籍审核列表
    server_.setbridge(bookReviewBridge);      // 书籍审核操作

    server_.setThreadsize(8);
    server_.run();
    return 0;
}