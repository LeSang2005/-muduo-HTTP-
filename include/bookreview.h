#pragma once
#include "bridge.h"

class BookReviewBridge : public bridge {
public:
    BookReviewBridge();
    std::string send(const std::string message) override;
};