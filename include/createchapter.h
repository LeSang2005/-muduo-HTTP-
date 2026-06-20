#pragma once
#include"bridge.h"
#include <fstream>
#include <iterator>
#include <filesystem>
class createchapter :public bridge{
public:
createchapter();
std::string send();
};