#pragma once
#include"bridge.h"
#include<string>
class zhuche:public bridge{
public:
zhuche();
std::string send()override;
};