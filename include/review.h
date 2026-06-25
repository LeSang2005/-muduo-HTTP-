#pragma once
#include "bridge.h"

class review : public bridge {
public:
    review();
    std::string send(std::string message_) override;
};