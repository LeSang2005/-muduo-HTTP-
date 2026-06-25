#pragma once
#include"bridge.h"
class rpassword:public bridge{
public:
rpassword();
std::string send(std::string message_)override;
};