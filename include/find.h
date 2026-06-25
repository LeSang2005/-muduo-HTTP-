#pragma once
#include"bridge.h"
class find: public bridge{
public:
find();
std::string send(std::string message_)override;
};