#include"HttpServer.h"
#include<iostream>
#include<thread>
#include<condition_variable>
#include"web.h"

#include"ReadLog.h"
HttpServer::HttpServer():addr_(readfile()),tcpserver_(&loop_,addr_,"httpserver"),threadNum_(4){
    //tcpserver_.setReuseAddr(true);
    tcpserver_.setConnectionCallback(bind(&HttpServer::onConnection,this,std::placeholders::_1));
    tcpserver_.setMessageCallback(bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
}
// void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn){
//     if(!conn->connected()){
//         std::cout<<"__关闭"<<std::endl;
//         conn->shutdown();
//     }
// }

void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        //std::cout << "__关闭" << std::endl;
        
        LOG_INFO(conn->name()+"关闭");
        connBuf_.erase(conn.get());  // 清除累积数据
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
                            muduo::Timestamp) {
    // 1. 将新数据追加到该连接的累积缓冲区
    LOG_INFO(conn->name()+"连接");
    std::string& allData = connBuf_[conn.get()];
    allData += buf->retrieveAllAsString();

    while (true) {
        // 2. 检查是否收到完整的 HTTP 请求头（以 \r\n\r\n 结束）
        size_t headerEnd = allData.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            break;  // 头部还没收完，等待下一次数据
        }

        // 3. 尝试解析 Content-Length
        size_t clPos = allData.find("Content-Length:");
        bool hasBody = (clPos != std::string::npos && clPos < headerEnd);
        int contentLen = 0;

        if (hasBody) {
            clPos += 15; // "Content-Length:" 长度
            size_t clEnd = allData.find("\r\n", clPos);
            if (clEnd == std::string::npos) {
                // 格式错误：找不到头部行的结束，断开连接保护
                conn->shutdown();
                return;
            }

            // ✅ 防止 std::stoi 在遇到非数字字符时抛出异常导致程序崩溃
            // 例如客户端发送 "Content-Length: abc" 或 "Content-Length: 123,456"
            try {
                contentLen = std::stoi(allData.substr(clPos, clEnd - clPos));
            } catch (const std::invalid_argument&) {
                //std::cout << "非法的 Content-Length 值（非数字）" << std::endl;
                LOG_WARN("非法的 Content-Length 值（非数字）");
                conn->shutdown();
                return;
            } catch (const std::out_of_range&) {
                //std::cout << "Content-Length 值过大，超出 int 范围" << std::endl;
                LOG_WARN("Content-Length 值过大，超出 int 范围" );
                conn->shutdown();
                return;
            }

            // ✅ 额外防护：Content-Length 不应为负数
            if (contentLen < 0) {
               // std::cout << "Content-Length 为负数" << std::endl;
               LOG_ERROR("Content-Length 为负数");
                conn->shutdown();
                return;
            }
        }

        size_t totalLen = headerEnd + 4 + contentLen;
        if (allData.size() < totalLen) {
            break;  // 请求体还没收全，等待下次数据
        }

        // 4. 提取一个完整的请求报文
        std::string fullRequest = allData.substr(0, totalLen);
        allData.erase(0, totalLen);  // 从缓冲区移除已处理的数据

        // 5. 交给 web 模块处理
        std::cout << fullRequest << std::endl;
        web b;
        b.setmessage(fullRequest);
        b.setTcpPtr(conn);
        b.run();
    }
}
muduo::net::InetAddress HttpServer::readfile(){
    //std::cout<<"开始"<<std::endl;
    FILE* f=fopen("net.txt","r");
    if(f==nullptr){
        //std::cout<<__FILE__<<":"<<__LINE__<<"出现错误,"<<f<<"文件无法打开"<<strerror(errno)<<std::endl;
        LOG_ERROR("net.txt打开失败");
        exit(EXIT_FAILURE);
    }
    while(!feof(f)){
        char buf[1024];
        fgets(buf,1024,f);
        std::string buf_(buf);
        int n=buf_.find_first_not_of(" ");
        buf_=buf_.substr(n);
        n=buf_.find_last_not_of("\n");
        buf_=buf_.substr(0,n+1);
        n=buf_.find_last_not_of(" ");
        buf_=buf_.substr(0,n+1);
        n=buf_.find('=');
        if(n==std::string::npos){
            //std::cout<<__FILE__<<":"<<__LINE__<<"行："<<f<<"文本格式出现问题"<<std::endl;
            LOG_ERROR("文本格式出现问题");
            exit(EXIT_FAILURE);
        }
        std::string key=buf_.substr(0,n);
        std::string value=buf_.substr(n+1);
        if(key=="ip"){
            ip=value;
        }
        else if(key=="port"){
            port=atoi(value.c_str());
        }
    }
    std::cout<<ip<<" "<<port<<std::endl;
    return muduo::net::InetAddress(ip,port);
}  

void HttpServer::run(){
   tcpserver_.setThreadNum(4);
    tcpserver_.start();
    for(int i=0;i<threadNum_;i++){
        std::thread t(&HttpServer::handlequeue,this);
        t.detach();
    }
    loop_.loop();
}

void HttpServer::handlequeue(){
    while(1){
        post_struct q=web_queue::instance().pop();
        if(b_.count(q.method)){
             b_[q.method]->setConnection(q.ptr_);
            b_[q.method]->setmessage(q.data);
            std::string response=b_[q.method]->send();
           
            response=web::setpost(response);
            q.ptr_->getLoop()->runInLoop([conn = q.ptr_, resp = response]() {
                conn->send(resp);
                });
        }
        else{
             q.ptr_->getLoop()->runInLoop([conn = q.ptr_]() {
                conn->send(web::error());
                });
        }
    }
}

std::unordered_map<std::string,muduo::net::TcpConnectionPtr>HttpServer::IdOfConn;


std::mutex HttpServer::idOfConnMtx_;