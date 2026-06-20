#pragma once
#include"bridge.h"
class booklist: public bridge{
    public:
    booklist();
    std::string send()override;
};