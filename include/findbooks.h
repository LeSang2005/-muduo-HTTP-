#pragma once
#include"bridge.h"
class findbooks:public bridge{
public:
findbooks();
std::string send(std::string message)override;
};