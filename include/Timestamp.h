#pragma once
#include<chrono>
class Timestamp{
public:
void setnow(){
    t_=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
long long alive(){
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())-t_;
}
private:
std::time_t t_;
};