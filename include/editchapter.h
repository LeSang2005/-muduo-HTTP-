#pragma once
#include "bridge.h"
#include <fstream>
#include <filesystem>

class editchapter : public bridge {
public:
    editchapter();
    std::string send() override;
};