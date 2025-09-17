#pragma once

#include "CjsBaseNode.h"
#include <string>

class CjsPassthroughNode : public CjsBaseNode {
public:
    CjsPassthroughNode(const std::string& content) {
        this->content = content;
    }

    void accept(CjsVisitor& visitor) override {
        visitor.visit(*this);
    }
};
