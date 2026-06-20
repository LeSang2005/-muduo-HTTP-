#include "web.h"
#include "HttpServer.h"
#include <fstream>      // std::ifstream
#include <iterator>     // std::istreambuf_iterator
#include"ReadLog.h"
web::web() {
    web_function.insert({"POST", [this](std::string resource) {
        dealpost(resource);
    }});
    web_function.insert({"GET", [this](std::string resource) {
        dealget(resource);
    }});
    web_function.insert({"OPTIONS", [this](std::string resource) {
        dealoptions(resource);
    }});
    r_style.insert({ResponseStyle::text, "Content-Type: text/plain; charset=utf-8"});
    r_style.insert({ResponseStyle::json, "Content-Type: application/json; charset=utf-8"});
    r_style.insert({ResponseStyle::file, "Content-Type: text/html; charset=utf-8"});
    r_style.insert({ResponseStyle::video, "Content-Type: video/mp4"});
    r_style.insert({ResponseStyle::audio, "Content-Type: audio/mpeg"});

    s_style.insert({"html", ResponseStyle::file});
    s_style.insert({"txt", ResponseStyle::text});
    s_style.insert({"json", ResponseStyle::json});
}

void web::run() {
    // ====== 原有代码，完全不变 ======
    std::string temp_message = message;
    if (message.empty()) {
        //std::cout << "无事件处理，请设置事件" << " " << __FILE__ << " " << __LINE__ << std::endl;
        LOG_ERROR("无事件处理");
        return;
    }
    int n = message.find_first_not_of(" ");
    if (n == std::string::npos) {
       // std::cout << "格式错误" << " " << __FILE__ << " " << __LINE__ << std::endl;
       LOG_ERROR(message+"格式错误");
    }
    int m = message.find(" ");
    std::string response = message.substr(n, m - n);
    if (web_function.count(response)) {
        std::function<void(std::string)> responseBack = web_function[response];
        temp_message = temp_message.substr(m + 1);
        m = temp_message.find(" ");
        std::string source = temp_message.substr(0, m);
        responseBack(source);
    }
}

void web::dealget(std::string source) {
    // 防止路径遍历攻击
    if (source.find("..") != std::string::npos) {
        ptr_->send(error());
        return;
    }

    // 去掉 URL 中的查询参数（如 ?name=xxx&id=1）
    size_t q_pos = source.find('?');
    if (q_pos != std::string::npos) {
        source = source.substr(0, q_pos);
    }

    // 构建文件路径
    std::string root = "../static";
    root += source;

    // 以二进制方式打开文件（RAII，自动管理资源）
    std::ifstream file(root, std::ios::binary);
    if (!file.is_open()) {
        ptr_->send(error());
        return;
    }

    // 获取文件扩展名
    size_t dot_pos = source.find_last_of('.');
    if (dot_pos == std::string::npos) {
        ptr_->send(error());
        return;
    }
    std::string ext = source.substr(dot_pos + 1);

    // 安全地读取整个文件内容到字符串
    std::string content;
    try {
        // 使用 istreambuf_iterator 一次性读取全部内容
        content.assign(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
    } catch (const std::exception& e) {
        // 读取异常（例如文件过大、内存不足）时返回 404
        LOG_ERROR("文件过大");
        ptr_->send(error());
        return;
    }

    if (file.bad()) {   // 检查流状态
        ptr_->send(error());
        return;
    }

    // 构建 HTTP 响应头
    std::string temp_r = "HTTP/1.1 200 OK\r\n";
    // 查找对应 MIME 类型，如果找不到则使用通用二进制类型
    auto it = s_style.find(ext);
    if (it != s_style.end()) {
        temp_r += r_style[it->second] + "\r\n";   // r_style 里已包含 "Content-Type: ..."
    } else {
        temp_r += "Content-Type: application/octet-stream\r\n";
    }
    temp_r += "Content-Length: " + std::to_string(content.size()) + "\r\n";
    temp_r += "\r\n";                // 头部和身体的分隔空行
    temp_r += content;              // 响应体

    ptr_->send(temp_r);
}

void web::dealpost(std::string source) {

    if (source.empty()) return;
    if (source[0] == '/') source.erase(source.begin());

    // 1. 找到头部结束位置（第一个 \r\n\r\n）
    size_t header_end = message.find("\r\n\r\n");
    if (header_end == std::string::npos) return;

    // 2. 解析 Content-Length 头部
    size_t cl_pos = message.find("Content-Length:");
    if (cl_pos == std::string::npos || cl_pos > header_end) return;   // 没有 Content-Length，放弃
    cl_pos += 15;  // "Content-Length:" 长度
    size_t cl_end = message.find("\r\n", cl_pos);
    std::string cl_str = message.substr(cl_pos, cl_end - cl_pos);
    int content_length = std::stoi(cl_str);

    // 3. 检查 body 是否已完整接收
    size_t body_start = header_end + 4;
    if (message.size() < body_start + content_length) {
        return;   // 数据还没收全，等下次 onMessage 再处理
    }

    // 4. 精确截取 body
    std::string temp_message_data = message.substr(body_start, content_length);

    //std::cout << "开始储存" << std::endl;
    HttpServer::IdOfConn.insert({temp_message_data, ptr_});
    web_queue::instance().push({source, temp_message_data, ptr_});
}

void web::dealoptions(std::string source) {
    std::string response = "HTTP/1.1 204 No Content\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods:GET,POST,OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += "\r\n";
    ptr_->send(response);
}

std::string web::setpost(std::string mess) {
    std::string resp;
    resp += "HTTP/1.1 200 OK\r\n";
    resp += "Content-Type: application/json; charset=utf-8\r\n";
    resp += "Access-Control-Allow-Origin: *\r\n";
    resp += "Content-Length: " + std::to_string(mess.size()) + "\r\n";
    resp += "\r\n";
    resp += mess;
    return resp;
}

std::string web::error() {
    std::string body = "404 Not Found";
    std::string response = "HTTP/1.1 404 Not Found\r\n";
    response += "Content-Type:text/html; charset=utf-8\r\n";
    response += "Content-Length:" + std::to_string(body.size()) + "\r\n";
    response += "\r\n";
    response += body;
    return response;
}