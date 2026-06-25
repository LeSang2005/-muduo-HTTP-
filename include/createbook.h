#pragma once
#include"bridge.h"
class createbook:public bridge{
public:
createbook();
std::string send(std::string message_);
};