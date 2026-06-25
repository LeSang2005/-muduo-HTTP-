#pragma once
#include"bridge.h"
#include <fstream>
#include <iterator>
class readchapter: public bridge{
    public:
    readchapter();
    std::string send(std::string message_)override;
};