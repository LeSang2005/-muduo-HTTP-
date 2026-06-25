#pragma once
#include "bridge.h"

class BookReviewListBridge : public bridge {
public:
    BookReviewListBridge();
    std::string send(const std::string message) override;
};