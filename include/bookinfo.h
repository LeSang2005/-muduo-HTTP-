#pragma once
#include"bridge.h"
class bookinfo: public bridge{
public:
bookinfo();
std::string send(std::string message_)override;
private:
};