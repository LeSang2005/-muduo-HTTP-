#pragma once
#include"bridge.h"
class mybook: public bridge{
public:
    mybook();
    std::string send(std::string message_)override;
};