#pragma once

#include <vector>
#include <string>

#include "ETypeValue.hpp"

struct SFlagProperties {
    std::vector<ETypeValue> types;
    int numberOfValues;
    std::string helpMessage;
    int function_index;
    bool extra_s;
};
