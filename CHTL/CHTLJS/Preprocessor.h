#pragma once

#include <string>
#include <vector>
#include <map>

class CHTLJS_Preprocessor {
public:
    std::string preprocess(std::string& source);

private:
    int placeholder_index = 0;
};
