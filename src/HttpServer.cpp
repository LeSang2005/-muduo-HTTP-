#include "HttpServer.h"
#include <iostream>
#include <thread>
#include <condition_variable>
#include "web.h"
#include "ReadLog.h"

HttpServer::HttpServer()
    : addr_(readfile()),
      tcpserver_(&loop_, addr_, "httpserver"),
      threadNum_(4)
{
    //tcpserver_.setReuseAddr(true);
    tcpserver_.setConnectionCallback(
        bind(&HttpServer::onConnection, this, std::placeholders::_1));
    tcpserver_.setMessageCallback(
        bind(&HttpServer::onMessage, this, std::placeholders::_1,
             std::placeholders::_2, std::placeholders::_3));
}

// void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn){
//     if(!conn->connected()){
//         std::cout<<"__关闭"<<std::endl;
//         conn->shutdown();
//     }
// }

void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        //std::cout << "__关闭" << std::endl;
        LOG_INFO(conn->name() + "关闭");
        {
            std::unique_lock<std::mutex> lock(mapReadMtx_);
            connBuf_.erase(conn.get());
        }
        conn->shutdown();
    }
}

// void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
//                             muduo::net::Buffer* buffer_,
//                             muduo::Timestamp){
//                                 std::string message=buffer_->retrieveAllAsString();
//                                 std::cout<<message<<std::endl;
//                                 web b;
//                                 b.setmessage(message);
//                                 b.setTcpPtr(conn);
//                                 b.run();
//                             }

void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp)
{
    LOG_INFO(conn->name() + "收到数据");

    // 将新数据追加到对应连接的累积缓冲区，并在锁内循环提取完整请求
    std::unique_lock<std::mutex> lock(mapReadMtx_);
    std::string& allData = connBuf_[conn.get()];
    allData += buf->retrieveAllAsString();

    while (true)
    {
        // 检查是否收到完整的 HTTP 请求头（以 \r\n\r\n 结束）
        size_t headerEnd = allData.find("\r\n\r\n");
        if (headerEnd == std::string::npos)
        {
            break; // 头部还没收完，等待下一次数据
        }

        // 大小写无关地查找 Content-Length
        size_t clPos = std::string::npos;
        // 简单实现：将头部区域转为小写再查找
        std::string headerPart = allData.substr(0, headerEnd);
        for (auto& c : headerPart)
            c = tolower(c);
        std::string lowerCL = "content-length:";
        size_t lowerPos = headerPart.find(lowerCL);
        bool hasBody = (lowerPos != std::string::npos);
        int contentLen = 0;

        if (hasBody)
        {
            // 恢复到原始字符串中的实际位置
            clPos = allData.find("Content-Length:");
            if (clPos == std::string::npos)
            {
                // 也可能是混合大小写，重新搜索（简单起见，这里先用原始查找，若找不到则按小写位置换算）
                // 实际项目中建议写一个不区分大小写的查找函数
                clPos = lowerPos; // 因为大小写转换前的位置与转换后的位置相同
            }
            else
            {
                // 使用找到的精确位置
            }
            // 统一使用 clPos（指向 "Content-Length:" 的开始）
            clPos += 15; // "Content-Length:" 长度
            size_t clEnd = allData.find("\r\n", clPos);
            if (clEnd == std::string::npos)
            {
                conn->shutdown();
                return;
            }

            try
            {
                contentLen = std::stoi(allData.substr(clPos, clEnd - clPos));
            }
            catch (const std::invalid_argument&)
            {
                LOG_WARN("非法的 Content-Length 值（非数字）");
                conn->shutdown();
                return;
            }
            catch (const std::out_of_range&)
            {
                LOG_WARN("Content-Length 值过大，超出 int 范围");
                conn->shutdown();
                return;
            }

            if (contentLen < 0)
            {
                LOG_ERROR("Content-Length 为负数");
                conn->shutdown();
                return;
            }
        }

        size_t totalLen = headerEnd + 4 + contentLen;
        if (allData.size() < totalLen)
        {
            break; // 请求体还没收全，等待下次数据
        }

        // 提取一个完整的请求报文（拷贝）
        std::string fullRequest = allData.substr(0, totalLen);
        allData.erase(0, totalLen); // 直接从原缓冲区删除

        // 解锁后交给 web 模块处理（避免长时间持锁）
        lock.unlock();
        LOG_INFO(fullRequest);
        web b;
        b.setmessage(fullRequest);
        b.setTcpPtr(conn);
        b.run();
        lock.lock(); // 重新加锁，继续检查剩余数据
    }
    // 循环结束后，剩余未处理的数据仍然保留在 connBuf_ 中，等待下次 onMessage
}

muduo::net::InetAddress HttpServer::readfile()
{
    //std::cout<<"开始"<<std::endl;
    FILE* f = fopen("net.txt", "r");
    if (f == nullptr)
    {
        //std::cout<<__FILE__<<":"<<__LINE__<<"出现错误,"<<f<<"文件无法打开"<<strerror(errno)<<std::endl;
        LOG_ERROR("net.txt打开失败");
        exit(EXIT_FAILURE);
    }
    while (!feof(f))
    {
        char buf[1024];
        fgets(buf, 1024, f);
        std::string buf_(buf);
        int n = buf_.find_first_not_of(" ");
        buf_ = buf_.substr(n);
        n = buf_.find_last_not_of("\n");
        buf_ = buf_.substr(0, n + 1);
        n = buf_.find_last_not_of(" ");
        buf_ = buf_.substr(0, n + 1);
        n = buf_.find('=');
        if (n == std::string::npos)
        {
            //std::cout<<__FILE__<<":"<<__LINE__<<"行："<<f<<"文本格式出现问题"<<std::endl;
            LOG_ERROR("文本格式出现问题");
            exit(EXIT_FAILURE);
        }
        std::string key = buf_.substr(0, n);
        std::string value = buf_.substr(n + 1);
        if (key == "ip")
        {
            ip = value;
        }
        else if (key == "port")
        {
            port = atoi(value.c_str());
        }
    }
    fclose(f); // 修复：关闭文件
    std::cout << ip << " " << port << std::endl;
    return muduo::net::InetAddress(ip, port);
}

void HttpServer::run()
{
    tcpserver_.setThreadNum(4);
    tcpserver_.start();
    for (int i = 0; i < threadNum_; i++)
    {
        std::thread t(&HttpServer::handlequeue, this);
        t.detach();
    }
    loop_.loop();
}

void HttpServer::handlequeue()
{
    while (1)
    {
        post_struct q = web_queue::instance().pop();
        if (b_.count(q.method))
{
    b_[q.method]->setConnection(q.ptr_);

    std::string response =
        b_[q.method]->send(q.data);

    response = web::setpost(response);

    auto conn = q.ptr_;

    conn->getLoop()->runInLoop(
        [conn,response]()
        {
            if(conn->connected())
            {
                conn->send(response);
            }
        }
    );
}
        else
        {
            q.ptr_->getLoop()->runInLoop([conn = q.ptr_]() {
                conn->send(web::error());
            });
        }
    }
}

std::unordered_map<std::string, muduo::net::TcpConnectionPtr> HttpServer::IdOfConn;
std::mutex HttpServer::idOfConnMtx_;