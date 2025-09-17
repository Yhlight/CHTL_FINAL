#pragma once

#include "CjsBaseNode.h"
#include <string>

class CjsSelectorNode : public CjsBaseNode {
public:
    CjsSelectorNode(const std::string& selector) {
        this->content = selector;
    }

    void accept(CjsVisitor& visitor) override {
        visitor.visit(*this);
    }
};
