#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <queue>
#include <chrono>
#include<ReadLog.h>
class RateLimiter {
public:
    static RateLimiter& instance() {
        static RateLimiter rl;
        return rl;
    }

    bool allow(const std::string& ip, int maxRequests, int seconds) {
        std::lock_guard<std::mutex> lock(mtx_);
        
        // 当前时间点（单调时钟，不会往回跳）
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        
        // 这个 IP 的访问记录队列
        std::queue<std::chrono::steady_clock::time_point>& records = records_[ip];
        
        // 清理超过时间窗口的旧记录
        while (!records.empty()) {
            // 队首记录距今多少秒
            std::chrono::seconds elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - records.front());
            if (elapsed.count() > seconds) {
                records.pop(); // 超时了，扔掉
            } else {
                break; // 队首还没超时，后面的更不会超时，不用继续了
            }
        }
        
        // 如果队列长度 >= 允许的最大次数，拒绝
        if (records.size() >= maxRequests) {
            LOG_WARN(ip+"访问频繁");
            return false;

        }
        
        // 记录这次访问
        records.push(now);
        return true;
    }

private:
    std::mutex mtx_;
    std::unordered_map<std::string, std::queue<std::chrono::steady_clock::time_point>> records_;
};