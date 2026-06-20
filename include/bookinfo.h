#pragma once
#include"bridge.h"
class bookinfo: public bridge{
public:
bookinfo();
std::string send()override;
private:
};