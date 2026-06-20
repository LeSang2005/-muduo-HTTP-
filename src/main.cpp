#include "HttpServer.h"
#include"zhuche.h"
#include"login.h"
#include"find.h"
#include"rname.h"
#include"rpassword.h"
#include"booklist.h"
#include"chapterlist.h"
#include"readchapter.h"
#include"bookinfo.h"
#include"createbook.h"
#include"updateEdit.h"
#include"createchapter.h"
#include"editchapter.h"
#include "reviewlist.h"
#include "review.h"
int main() {
    HttpServer server_;

    // 注册业务模块（在服务器启动之前）
    auto loginBridge=std::make_shared<login>();
    auto zhucheRridge=std::make_shared<zhuche>();
    auto findRridge=std::make_shared<find>();
    auto rnameRridge=std::make_shared<rname>();
    auto rpasswordRridge=std::shared_ptr<rpassword>(new rpassword());
    auto booklistBridge=std::shared_ptr<booklist>(new booklist());
    auto chapterlistBridge=std::shared_ptr<chapterlist>(new chapterlist());
    auto readchaperBrdge=std::shared_ptr<readchapter>(new readchapter());
    auto bookinfoBridge=std::shared_ptr<bookinfo>(new bookinfo());
    auto createbookBridge=std::shared_ptr<createbook>(new createbook());
    auto updateEditBridge=std::shared_ptr<updateEdit>(new updateEdit());
    auto createchapterBridge=std::shared_ptr<createchapter>(new createchapter());
    auto editChapterBridge = std::make_shared<editchapter>();
    auto reviewlistBridge = std::make_shared<reviewlist>();
auto reviewBridge = std::make_shared<review>();
server_.setbridge(reviewlistBridge);
server_.setbridge(reviewBridge);
    server_.setbridge(editChapterBridge);
    server_.setbridge(findRridge);
    server_.setbridge(zhucheRridge);
    server_.setbridge(loginBridge);
    server_.setbridge(rnameRridge);
    server_.setbridge(rpasswordRridge);
    server_.setbridge(booklistBridge);
    server_.setbridge(chapterlistBridge);
    server_.setbridge(readchaperBrdge);
    server_.setbridge(bookinfoBridge);
    server_.setbridge(createbookBridge);
    server_.setbridge(updateEditBridge);
    server_.setbridge(createchapterBridge);
    // auto loginBridge = std::make_shared<LoginBridge>();
    // server_.setbridge(loginBridge);
    server_.setThreadsize(8);
    server_.run();
    return 0;
}