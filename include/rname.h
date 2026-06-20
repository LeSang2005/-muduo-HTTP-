#pragma once
#include"bridge.h"
class rname:public bridge{
public:
rname();
std::string send()override;
};