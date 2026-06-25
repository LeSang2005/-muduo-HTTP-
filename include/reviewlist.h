#pragma once
#include "bridge.h"

class reviewlist : public bridge {
public:
    reviewlist();
    std::string send(std::string message_) override;
};