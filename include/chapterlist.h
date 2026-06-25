#pragma once
#include"bridge.h"
class chapterlist:public bridge{
public:
chapterlist();
std::string send(std::string message_)override;
};